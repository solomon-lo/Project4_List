#include "provided.h"
#include <list>
using namespace std;

class PointToPointRouterImpl
{
public:
	PointToPointRouterImpl(const StreetMap* sm);
	~PointToPointRouterImpl();
	DeliveryResult generatePointToPointRoute(
		const GeoCoord& start,
		const GeoCoord& end,
		list<StreetSegment>& route,
		double& totalDistanceTravelled) const;
private:
	const StreetMap* m_map;
	struct Node
	{
		GeoCoord m_GeoCoord;
		Node* prevGeoCoordNode;
		double gCost;
		double hCost;
		double fCost;
	};
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
	m_map = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
	const GeoCoord& start,
	const GeoCoord& end,
	list<StreetSegment>& route,
	double& totalDistanceTravelled) const
{
	vector<StreetSegment> FirstPlaceholderVectorForTesting;
	
	m_map->getSegmentsThatStartWith(start, FirstPlaceholderVectorForTesting);
	if (FirstPlaceholderVectorForTesting.empty == true)
	{
		return BAD_COORD;
	}
	
	vector<StreetSegment> SecondPlaceholderVectorForTesting;
	m_map->getSegmentsThatStartWith(end, SecondPlaceholderVectorForTesting);
	if (SecondPlaceholderVectorForTesting.empty == true)
	{
		return BAD_COORD;
	}

	if (start == end)
	{
		totalDistanceTravelled = 0.00;
		route.clear();
		return;
	}

	vector<Node> openList;
	vector<Node> closedList;

	Node inputStartNode;
	inputStartNode.m_GeoCoord = start;
	inputStartNode.prevGeoCoordNode = nullptr;
	inputStartNode.fCost = 0;
	inputStartNode.gCost = 0;
	inputStartNode.hCost = 0;

	openList.insert(openList.begin(), inputStartNode);

	while(!openList.empty())
	{
		Node currentNode;
		double currentSmallestValue = 999;
		int currentSmallestIndex = 0;
		for (int i = 0; i < openList.size(); i++)
		{
			if (openList[i].fCost < currentSmallestValue)
			{
				currentSmallestValue = openList[i].fCost;
				currentSmallestIndex = i;
			}
		}
		currentNode = openList[currentSmallestValue];

		auto deleteIterator = openList.begin();
		for (int j = 0; j < currentSmallestIndex; j++)
		{
			deleteIterator++;
		}
		openList.erase(deleteIterator);

		closedList.insert(closedList.begin(), currentNode);

		//foudn goal implementation
		if (currentNode.m_GeoCoord == end)
		{
			//TODO:CHECK HERE FIRST IF THERE'S ERROR
			Node* similarToIteratorPointer = &currentNode;
			while (similarToIteratorPointer->prevGeoCoordNode != nullptr)
			{

				vector<StreetSegment> dummyVector;
				m_map->getSegmentsThatStartWith((similarToIteratorPointer->m_GeoCoord), dummyVector);
				for(int i = 0; i < dummyVector.size(); ++i)
				{
					int tracker = 0;
					if (dummyVector[i].start == similarToIteratorPointer->prevGeoCoordNode->m_GeoCoord)
					{
						//TODO:REMOVE. tHIS WAS FOR DEBUGGING
						
						route.push_front(dummyVector[i]);
						totalDistanceTravelled += distanceEarthMiles(dummyVector[i].start, dummyVector[i].end);
						tracker++;
						break;
					}
					if (tracker >= 2)
					{
						cerr << "Error: push fronted too many times!" << endl;
					}

				}
				*similarToIteratorPointer = *(similarToIteratorPointer->prevGeoCoordNode);
			}
		}

		///ended that 


	}


	return NO_ROUTE;  // Delete this line and implement this function correctly
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
	m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
	delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
	const GeoCoord& start,
	const GeoCoord& end,
	list<StreetSegment>& route,
	double& totalDistanceTravelled) const
{
	return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
