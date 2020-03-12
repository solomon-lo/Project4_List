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
	GeoCoord startingCoord = depot;
	list<StreetSegment> listOfStreetSegmentsInDeliveries;

	//adds all of the necessary street segments to travel
	for (auto it = deliveries.begin(); it != deliveries.end(); ++it)
	{
		//m_PointToPointRouter.generatePointToPointRoute(startingCoord, deliveries[i].location, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
		m_PointToPointRouter.generatePointToPointRoute(startingCoord, (*it).location, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
		startingCoord = (*it).location;
	}

	m_PointToPointRouter.generatePointToPointRoute(deliveries[deliveries.size() - 1].location, depot, listOfStreetSegmentsInDeliveries, totalDistanceTravelled);
	
	//converts all of the street segments to commands
	int whichDelivery = 0;
	auto it = listOfStreetSegmentsInDeliveries.begin();
	for(; it != listOfStreetSegmentsInDeliveries.end(); ++it)	//problem is this part of the code. FIX HERE TODO
	{
		StreetSegment startSegment = *it;
		++it;
		StreetSegment endSegment = *it;
		double distanceAlongSameNameStreet = 0;
		if (startSegment.name == endSegment.name)
		{
			while (endSegment.name == startSegment.name)
			{
				if (deliveries[whichDelivery].location == startSegment.start)
				{
					DeliveryCommand toPushAsDelivery;
					toPushAsDelivery.initAsDeliverCommand(deliveries[whichDelivery].item);
					whichDelivery++;
					
				}
				totalDistanceTravelled += distanceEarthMiles(startSegment.start, startSegment.end) + distanceEarthMiles(endSegment.start, endSegment.end);
				distanceAlongSameNameStreet += distanceEarthMiles(startSegment.start, startSegment.end) + distanceEarthMiles(endSegment.start, endSegment.end);
				++it;
				endSegment = *it;
			}
			double angleOfStartSegment = angleOfLine(startSegment);
			string directionForProceedCommand;
			
			getCardinalConversion(angleOfStartSegment, directionForProceedCommand);
			DeliveryCommand topush;
			topush.initAsProceedCommand(directionForProceedCommand, startSegment.name, distanceAlongSameNameStreet);
			commands.push_back(topush);
		}

		else if (startSegment.name != endSegment.name)
		{
			double angleBetweenDifferentNameSegments = angleBetween2Lines(startSegment, endSegment);
			if (angleBetweenDifferentNameSegments < 1.00 || angleBetweenDifferentNameSegments > 359.000)
			{
				
				DeliveryCommand toPushAsProceed;
				string cardinalForProceed;
				double angleBetweenBothSegments = angleBetween2Lines(startSegment, endSegment);
				double distanceBetweenBothSegments = distanceEarthMiles(startSegment.start, startSegment.end) + distanceEarthMiles(endSegment.start, endSegment.end);

				getCardinalConversion(angleBetweenBothSegments, cardinalForProceed);
				toPushAsProceed.initAsProceedCommand(cardinalForProceed, endSegment.name, distanceBetweenBothSegments);
				commands.push_back(toPushAsProceed);
			}

			else if (angleBetweenDifferentNameSegments >= 1.00 && angleBetweenDifferentNameSegments < 180.000)
			{

				DeliveryCommand toPushAsProceed;
				string cardinalForProceed;
				toPushAsProceed.initAsTurnCommand("left", endSegment.name);
			}
			else if (angleBetweenDifferentNameSegments >= 180.000 && angleBetweenDifferentNameSegments < 359.000)
			{

				DeliveryCommand toPushAsProceed;
				string cardinalForProceed;
				toPushAsProceed.initAsTurnCommand("right", endSegment.name);
			}
		}

		if ((*it).start == depot)
		{
			return DELIVERY_SUCCESS;
		}
	}
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


