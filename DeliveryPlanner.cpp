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
	m_PointToPointRouter.generatePointToPointRoute(startingCoord, inputDeliveries[0].location,  listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
	
	//adds all of the necessary street segments to travel
	if (inputDeliveries.size() > 1)
	{
		for (int i = 0; i < inputDeliveries.size() - 2; i++)
		{
			m_PointToPointRouter.generatePointToPointRoute(inputDeliveries[i].location, inputDeliveries[i + 1].location, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
		}
	}

	//for(auto it = inputDeliveries.end() - 2; it == inputDeliveries.begin(); --it)
	//{
	//	//m_PointToPointRouter.generatePointToPointRoute(startingCoord, inputDeliveries[i].location, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
	//	m_PointToPointRouter.generatePointToPointRoute((*it).location, startingCoord,  listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
	//	startingCoord = (*it).location;
	//}
	m_PointToPointRouter.generatePointToPointRoute(inputDeliveries[inputDeliveries.size() - 1].location, depot, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);

	/*
	GeoCoord currentGeocord = depot;
	for (auto currentLocationiterator = inputDeliveries.begin(); currentLocationiterator != inputDeliveries.end(); ++currentLocationiterator)
	{
		m_PointToPointRouter.generatePointToPointRoute(currentGeocord, currentLocationiterator->location, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
		currentGeocord = currentLocationiterator->location;
	}

	m_PointToPointRouter.generatePointToPointRoute(inputDeliveries[inputDeliveries.size() - 1].location, depot, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
	//converts all of the street segments to commands
		*/
	int whichDelivery = 0;

	auto StreetSegmentIterator = listOfStreetSegmentsInDeliveries.begin();


	//if ((*StreetSegmentIterator).start == depot)
	//{
	//	return DELIVERY_SUCCESS;
	//}

	do	
	{
		bool wentOnSameStreet = false;
		StreetSegment startSegment = *StreetSegmentIterator;
		//if (startSegment.start == depot)
		//{
		//	cerr << "reached depot at the top" << endl;
		//	return DELIVERY_SUCCESS;
		//}
		bool alreadyDelivered = false;
		if (startSegment.start == inputDeliveries[whichDelivery].location)
		{
			alreadyDelivered = true;
			DeliveryCommand toPushAsDelivery;
			toPushAsDelivery.initAsDeliverCommand(inputDeliveries[whichDelivery].item);
			commands.push_back(toPushAsDelivery);
			if (whichDelivery + 1 != inputDeliveries.size())
			{
				whichDelivery++;
			}
			continue;//NOT SURE TODO
		}

		if (!alreadyDelivered)
		{
			++StreetSegmentIterator;
		}


		if (startSegment.name == StreetSegmentIterator->name)
		{
			double distanceAlongSameNameStreet = 0;
			wentOnSameStreet = true;
			while (StreetSegmentIterator->name == startSegment.name)
			{
				//if ((*StreetSegmentIterator).start == depot)
				//{
				//	return DELIVERY_SUCCESS;
				//}
				if (startSegment.start == inputDeliveries[whichDelivery].location)
				{

					DeliveryCommand toPushAsDelivery;
					toPushAsDelivery.initAsDeliverCommand(inputDeliveries[whichDelivery].item);
					commands.push_back(toPushAsDelivery);
					if (whichDelivery + 1 != inputDeliveries.size())
					{
						whichDelivery++;
					}
					break;	//NOT SURE TODO
				}
				else
				{

					totalDistanceTravelled += distanceEarthMiles(startSegment.start, startSegment.end) + distanceEarthMiles(StreetSegmentIterator->start, StreetSegmentIterator->end);
					distanceAlongSameNameStreet += distanceEarthMiles(startSegment.start, startSegment.end) + distanceEarthMiles(StreetSegmentIterator->start, StreetSegmentIterator->end);

					StreetSegmentIterator++;
				}
			}
			double angleOfStartSegment = angleOfLine(startSegment);
			string directionForProceedCommand;
			
			getCardinalConversion(angleOfStartSegment, directionForProceedCommand);
			DeliveryCommand topush;
			topush.initAsProceedCommand(directionForProceedCommand, startSegment.name, distanceAlongSameNameStreet);
			commands.push_back(topush);
		}

		else if (startSegment.name != StreetSegmentIterator->name)
		{
			double angleBetweenDifferentNameSegments = angleBetween2Lines(startSegment, *StreetSegmentIterator);
			if (angleBetweenDifferentNameSegments < 1.00 || angleBetweenDifferentNameSegments > 359.000)
			{
				
				DeliveryCommand toPushAsProceed;
				string cardinalForProceed;
				double angleBetweenBothSegments = angleBetween2Lines(startSegment, *StreetSegmentIterator);
				double distanceBetweenBothSegments = distanceEarthMiles(startSegment.start, startSegment.end) + distanceEarthMiles(StreetSegmentIterator->start, StreetSegmentIterator->end);

				getCardinalConversion(angleBetweenBothSegments, cardinalForProceed);
				toPushAsProceed.initAsProceedCommand(cardinalForProceed, StreetSegmentIterator->name, distanceBetweenBothSegments);
				commands.push_back(toPushAsProceed);
			}

			else if (angleBetweenDifferentNameSegments >= 1.00 && angleBetweenDifferentNameSegments < 180.000)
			{

				DeliveryCommand toPushAsProceed;
				string cardinalForProceed;
				toPushAsProceed.initAsTurnCommand("left", StreetSegmentIterator->name);
				commands.push_back(toPushAsProceed);
			}
			else if (angleBetweenDifferentNameSegments >= 180.000 && angleBetweenDifferentNameSegments < 359.000)
			{

				DeliveryCommand toPushAsProceed;
				string cardinalForProceed;
				toPushAsProceed.initAsTurnCommand("right", StreetSegmentIterator->name);
				commands.push_back(toPushAsProceed);
			}
		}
		if ((*StreetSegmentIterator).start == depot || startSegment.start == depot)
		{
			return DELIVERY_SUCCESS;
		}
		//++StreetSegmentIterator;
	}
	while (StreetSegmentIterator != listOfStreetSegmentsInDeliveries.end());

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


