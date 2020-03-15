#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
	DeliveryPlannerImpl(const StreetMap* sm);
	~DeliveryPlannerImpl();
	DeliveryResult generateDeliveryPlan(
		const GeoCoord& depot,
		const vector<DeliveryRequest>& deliveries,
		vector<DeliveryCommand>& commands,
		double& totalDistanceTravelled) const;

private:
	const StreetMap* m_map;
	PointToPointRouter m_PointToPointRouter;
	
	void getCardinalConversion(double angleOfStartSegment, string& directionForProceedCommand) const
	{
		if (0.00 <= angleOfStartSegment && angleOfStartSegment < 22.5)
		{
			directionForProceedCommand = "east";
		}
		else if (22.5 <= angleOfStartSegment && angleOfStartSegment < 67.5)
		{
			directionForProceedCommand = "northeast";
		}

		else if (67.5 <= angleOfStartSegment && angleOfStartSegment < 112.5)
		{
			directionForProceedCommand = "north";
		}

		else if (112.5 <= angleOfStartSegment && angleOfStartSegment < 157.5)
		{
			directionForProceedCommand = "northwest";
		}
		else if (157.5 <= angleOfStartSegment && angleOfStartSegment < 202.5)
		{
			directionForProceedCommand = "west";
		}
		else if (202.5 <= angleOfStartSegment && angleOfStartSegment < 247.5)
		{
			directionForProceedCommand = "southwest";
		}
		else if (247.5 <= angleOfStartSegment && angleOfStartSegment < 292.5)
		{
			directionForProceedCommand = "south";
		}
		else if (292.5 <= angleOfStartSegment && angleOfStartSegment < 337.5)
		{
			directionForProceedCommand = "southeast";
		}
		else if (angleOfStartSegment >= 337.5)
		{
			directionForProceedCommand = "east";
		}
	}
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
	:m_PointToPointRouter(sm)
{
	m_map = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
	const GeoCoord& depot,
	const vector<DeliveryRequest>& deliveries,
	vector<DeliveryCommand>& commands,
	double& totalDistanceTravelled) const
{
	vector<StreetSegment> testVector;
	m_map->getSegmentsThatStartWith(depot, testVector);
	if (testVector.empty())
	{
		return BAD_COORD;
	}

	for (int i = 0; i < deliveries.size(); ++i)
	{
		vector<StreetSegment> inspectVector;
		m_map->getSegmentsThatStartWith(deliveries[i].location, inspectVector);
		if (testVector.empty())
		{
			return BAD_COORD;
		}
	}


	//this is used to call the optimzieDeliveryOrder, which changes th order so less distance has to be traveled
	DeliveryOptimizer m_DeliveryOptimizer(m_map);
	double oldCrowDistance;
	double newCrowDistance;
	vector<DeliveryRequest> inputDeliveries = deliveries;
	m_DeliveryOptimizer.optimizeDeliveryOrder(depot, inputDeliveries, oldCrowDistance, newCrowDistance);
	totalDistanceTravelled = 0;
	list<StreetSegment> listOfStreetSegmentsInDeliveries;

	//this generates the street segments from each point to the next
	GeoCoord startingCoord = depot;
	m_PointToPointRouter.generatePointToPointRoute(startingCoord, inputDeliveries[0].location, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
	cerr << "generated for depot to " << inputDeliveries[0].item << endl;

	//adds all of the necessary street segments to travel
	if (inputDeliveries.size() > 1)
	{
		for (int i = 0; i < inputDeliveries.size() - 1; i++)
		{
			m_PointToPointRouter.generatePointToPointRoute(inputDeliveries[i].location, inputDeliveries[i + 1].location, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
			cerr << "generated for" << inputDeliveries[i].item << " to " << inputDeliveries[i + 1].item << endl;
		}
	}

	//this goes back to the depot
	m_PointToPointRouter.generatePointToPointRoute(inputDeliveries[inputDeliveries.size() - 1].location, depot, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
	cerr << "generated for" << inputDeliveries[inputDeliveries.size() - 1].item << " to " << "depot " << endl;
	int whichDelivery = 0;

	auto currentStreetSegmentIterator = listOfStreetSegmentsInDeliveries.begin();

	StreetSegment previousStreetSegment = *currentStreetSegmentIterator;

	currentStreetSegmentIterator++;

	//this is the first proceed from the depot
	double distanceAlongSameNameStreet = distanceEarthMiles(previousStreetSegment.start, previousStreetSegment.end);
	double angleOfStartSegment = angleOfLine(*currentStreetSegmentIterator);
	string directionForProceedCommand;

	getCardinalConversion(angleOfStartSegment, directionForProceedCommand);
	DeliveryCommand topush;
	topush.initAsProceedCommand(directionForProceedCommand, previousStreetSegment.name, distanceAlongSameNameStreet);
	commands.push_back(topush);

	//whle there are still street segments to traverse
	while (currentStreetSegmentIterator != listOfStreetSegmentsInDeliveries.end())
	{

		bool alreadyDelivered = false;
		bool wentOnSameStreet = false;

		//this initiates a delivery if it detects its at a delivery location
		if (currentStreetSegmentIterator->start == inputDeliveries[whichDelivery].location)
		{
			alreadyDelivered = true;
			DeliveryCommand toPushAsDelivery;
			toPushAsDelivery.initAsDeliverCommand(inputDeliveries[whichDelivery].item);
			commands.push_back(toPushAsDelivery);
			if (whichDelivery + 1 != inputDeliveries.size())
			{
				whichDelivery++;
			}
		}
		//if they are different names, that means we must turn or proceed
		if (previousStreetSegment.name != currentStreetSegmentIterator->name && !(alreadyDelivered))
		{
			double angleBetweenDifferentNameSegments = angleBetween2Lines(previousStreetSegment, *currentStreetSegmentIterator);
			if (angleBetweenDifferentNameSegments < 1.00 || angleBetweenDifferentNameSegments > 359.000)
			{

				DeliveryCommand toPushAsProceed;

				double angleBetweenBothSegments = angleBetween2Lines(previousStreetSegment, *currentStreetSegmentIterator);
				double distanceBetweenBothSegments = distanceEarthMiles(previousStreetSegment.start, previousStreetSegment.end);

				string cardinalForProceed;
				getCardinalConversion(angleBetweenBothSegments, cardinalForProceed);
				toPushAsProceed.initAsProceedCommand(cardinalForProceed, currentStreetSegmentIterator->name, distanceBetweenBothSegments);
				commands.push_back(toPushAsProceed);
			}

			//this part decides whether we take a left or right turn
			else if (angleBetweenDifferentNameSegments >= 1.00 && angleBetweenDifferentNameSegments < 180.000)
			{

				DeliveryCommand toPushAsLeftTurn;
				string cardinalForProceed;
				toPushAsLeftTurn.initAsTurnCommand("left", currentStreetSegmentIterator->name);
				commands.push_back(toPushAsLeftTurn);
			}
			else if (angleBetweenDifferentNameSegments >= 180.000 && angleBetweenDifferentNameSegments < 359.000)
			{
				DeliveryCommand toPushAsRightTurn;
				string cardinalForProceed;
				toPushAsRightTurn.initAsTurnCommand("right", currentStreetSegmentIterator->name);
				commands.push_back(toPushAsRightTurn);
			}
		}

		//if they are different names, we can initialzie a proceed to the street segment with the new name
		if (previousStreetSegment.name != (*currentStreetSegmentIterator).name)
		{
			double distanceAlongSameNameStreet = distanceEarthMiles(previousStreetSegment.start, previousStreetSegment.end);
			double angleOfStartSegment = angleOfLine(*currentStreetSegmentIterator);
			string directionForProceedCommand;

			getCardinalConversion(angleOfStartSegment, directionForProceedCommand);
			DeliveryCommand topush;
			topush.initAsProceedCommand(directionForProceedCommand, previousStreetSegment.name, distanceAlongSameNameStreet);
			commands.push_back(topush);
		}
		//this part doesn't run if we already have delivered, since delivery blocks us from certain actions
		else if (alreadyDelivered)
		{
			double distanceAlongSameNameStreet = distanceEarthMiles(previousStreetSegment.start, previousStreetSegment.end);
			double angleOfStartSegment = angleOfLine(*currentStreetSegmentIterator);
			string directionForProceedCommand;

			getCardinalConversion(angleOfStartSegment, directionForProceedCommand);
			DeliveryCommand topush;
			topush.initAsProceedCommand(directionForProceedCommand, previousStreetSegment.name, distanceAlongSameNameStreet);
			commands.push_back(topush);
		}
		else if (!alreadyDelivered)
		{
			double distanceAlongSameNameStreet = distanceEarthMiles(previousStreetSegment.start, previousStreetSegment.end);
			commands[commands.size() - 1].increaseDistance(distanceEarthMiles(currentStreetSegmentIterator->start, currentStreetSegmentIterator->end));
		}

		previousStreetSegment = *currentStreetSegmentIterator;
		currentStreetSegmentIterator++;
		alreadyDelivered = false;
	}

	return DELIVERY_SUCCESS;  
}



//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
	m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
	delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
	const GeoCoord& depot,
	const vector<DeliveryRequest>& deliveries,
	vector<DeliveryCommand>& commands,
	double& totalDistanceTravelled) const
{
	return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}