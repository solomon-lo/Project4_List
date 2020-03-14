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
	vector<DeliveryRequest> inputDeliveries = deliveries;
	list<StreetSegment> listOfStreetSegmentsInDeliveries;
	//reverse(inputDeliveries.begin(), inputDeliveries.end());

	GeoCoord startingCoord = depot;
	m_PointToPointRouter.generatePointToPointRoute(startingCoord, inputDeliveries[0].location, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);

	//adds all of the necessary street segments to travel
	if (inputDeliveries.size() > 1)
	{
		for (int i = 0; i < inputDeliveries.size() - 2; i++)
		{
			m_PointToPointRouter.generatePointToPointRoute(inputDeliveries[i].location, inputDeliveries[i + 1].location, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
		}
	}

	m_PointToPointRouter.generatePointToPointRoute(inputDeliveries[inputDeliveries.size() - 1].location, depot, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);

	int whichDelivery = 0;

	auto currentStreetSegmentIterator = listOfStreetSegmentsInDeliveries.begin();

	StreetSegment previousStreetSegment = *currentStreetSegmentIterator;

	do
	{
		bool wentOnSameStreet = false;
		bool alreadyDelivered = false;
		if (currentStreetSegmentIterator->start == inputDeliveries[whichDelivery].location)
		{
			alreadyDelivered = true;
			DeliveryCommand toPushAsDelivery;
			toPushAsDelivery.initAsDeliverCommand(inputDeliveries[whichDelivery].item);
			commands.push_back(toPushAsDelivery);
			continue;
		}
		

		//if (startSegment.start == depot)
		//{
		//	cerr << "reached depot at the top" << endl;
		//	return DELIVERY_SUCCESS;
		//}

		if (currentStreetSegmentIterator->start == inputDeliveries[whichDelivery].location)
		{
			alreadyDelivered = true;
			DeliveryCommand toPushAsDelivery;
			toPushAsDelivery.initAsDeliverCommand(inputDeliveries[whichDelivery].item);
			commands.push_back(toPushAsDelivery);
			continue;
		}

		if (previousStreetSegment.name != currentStreetSegmentIterator->name && !(alreadyDelivered))
		{
			double angleBetweenDifferentNameSegments = angleBetween2Lines(previousStreetSegment, *currentStreetSegmentIterator);
			if (angleBetweenDifferentNameSegments < 1.00 || angleBetweenDifferentNameSegments > 359.000)
			{

				DeliveryCommand toPushAsProceed;

				double angleBetweenBothSegments = angleBetween2Lines(previousStreetSegment, *currentStreetSegmentIterator);
				double distanceBetweenBothSegments = distanceEarthMiles(previousStreetSegment.start, previousStreetSegment.end) + distanceEarthMiles(currentStreetSegmentIterator->start, currentStreetSegmentIterator->end);

				string cardinalForProceed;
				getCardinalConversion(angleBetweenBothSegments, cardinalForProceed);
				toPushAsProceed.initAsProceedCommand(cardinalForProceed, currentStreetSegmentIterator->name, distanceBetweenBothSegments);
				commands.push_back(toPushAsProceed);
			}

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

		else if (currentStreetSegmentIterator->name == previousStreetSegment.name)
		{
			{

				double distanceAlongSameNameStreet = distanceEarthMiles(previousStreetSegment.start, previousStreetSegment.end);
				wentOnSameStreet = true;
				double angleOfStartSegment = angleOfLine(*currentStreetSegmentIterator);
				string directionForProceedCommand;

				getCardinalConversion(angleOfStartSegment, directionForProceedCommand);
				DeliveryCommand topush;
				topush.initAsProceedCommand(directionForProceedCommand, previousStreetSegment.name, distanceAlongSameNameStreet);
				commands.push_back(topush);
			}

			bool iteratedSasmeStreetMoreThanOnce = false;
			while (currentStreetSegmentIterator->name == previousStreetSegment.name)
			{
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
					break;
				}
				else
				{

					totalDistanceTravelled += distanceEarthMiles(currentStreetSegmentIterator->start, currentStreetSegmentIterator->end);
					commands[commands.size()-1].increaseDistance(distanceEarthMiles(currentStreetSegmentIterator->start, currentStreetSegmentIterator->end));
					if ((*currentStreetSegmentIterator).end == depot)
					{
						return DELIVERY_SUCCESS;
					}
					currentStreetSegmentIterator++;
				}
			}
			//if (alreadyDelivered == false) TODO:

		}

		if ((*currentStreetSegmentIterator).end == depot)
		{
			totalDistanceTravelled += distanceEarthMiles(currentStreetSegmentIterator->start, currentStreetSegmentIterator->end);
			return DELIVERY_SUCCESS;
		}
		previousStreetSegment = *currentStreetSegmentIterator;

		currentStreetSegmentIterator++;
	} while (currentStreetSegmentIterator != listOfStreetSegmentsInDeliveries.end());

	return NO_ROUTE;  // Delete this line and implement this function correctly
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
