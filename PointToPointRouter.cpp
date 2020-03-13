#include "provided.h"
#include "ExpandableHashMap.h"
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
		double gCost = 0;
		double hCost = 0;
		double fCost = 0;
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
	totalDistanceTravelled = 0;
	vector<StreetSegment> FirstPlaceholderVectorForTesting;
	
	m_map->getSegmentsThatStartWith(start, FirstPlaceholderVectorForTesting);
	if (FirstPlaceholderVectorForTesting.empty() == true)
	{
		return BAD_COORD;
	}
	
	vector<StreetSegment> SecondPlaceholderVectorForTesting;
	m_map->getSegmentsThatStartWith(end, SecondPlaceholderVectorForTesting);
	if (SecondPlaceholderVectorForTesting.empty() == true)
	{
		return BAD_COORD;
	}

	if (start == end)
	{
		totalDistanceTravelled = 0.00;
		route.clear();
		return DELIVERY_SUCCESS;
	}

	list<Node*> openList;
	list<Node*> closedList;

	Node inputStartNode;
	inputStartNode.m_GeoCoord = start;
	inputStartNode.prevGeoCoordNode = nullptr;
	inputStartNode.fCost = 0;
	inputStartNode.gCost = 0;
	inputStartNode.hCost = 0;

	openList.insert(openList.begin(), &inputStartNode);

	while(!openList.empty())
	{
		Node* currentNode = new Node;
		double currentSmallestValue = 999;
		int currentSmallestIndex = 0;
		
		currentNode = *openList.begin();
		for(auto i = openList.begin(); i!= openList.end(); i++)
		{
			if ((*i)->fCost < currentSmallestValue)
			{
				currentSmallestValue = (*i)->fCost;
				currentNode = *i;
			}
		}

		
		for(auto deleteIterator = openList.begin(); deleteIterator != openList.end(); deleteIterator++)
		{
			if ((*deleteIterator)->fCost == currentSmallestValue)
			{
				openList.erase(deleteIterator);
				break;
			}
		}
		closedList.push_front(currentNode);
		

		
		
		//foudn goal implementation

		//backtracking portion
		if (currentNode->m_GeoCoord == end)	//in morning: this is never returned as true.
		{
			ExpandableHashMap<GeoCoord, GeoCoord> storageOfStreetSegmentReturns;
			cerr << "finally found the end" << endl;
			while (currentNode->m_GeoCoord != start)
			{
				storageOfStreetSegmentReturns.associate(currentNode->m_GeoCoord, currentNode->prevGeoCoordNode->m_GeoCoord);
				currentNode = currentNode->prevGeoCoordNode;
			}

			GeoCoord addToroute = end;
			while (addToroute != start)
			{
				vector<StreetSegment> tempStreetSegmentReturns;
				m_map->getSegmentsThatStartWith(addToroute, tempStreetSegmentReturns);
				for (int i = 0; i < tempStreetSegmentReturns.size(); i++)
				{
					if (tempStreetSegmentReturns[i].end == *(storageOfStreetSegmentReturns.find(addToroute)))
					{
						//StreetSegment reversedInOrderToPush;
						//reversedInOrderToPush.start = tempStreetSegmentReturns[i].end;
						//reversedInOrderToPush.end = tempStreetSegmentReturns[i].start;
						route.push_back(tempStreetSegmentReturns[i]);
						addToroute = tempStreetSegmentReturns[i].end;
						break;
					}
				}
			}

			return DELIVERY_SUCCESS;
		}

		//END OF THE IF STATEMENT
		//Generate children

		vector<StreetSegment> vectorOfPreChildrenStreetSegments;
		vector<Node*> vectorOfChildrenPreCheck;

		m_map->getSegmentsThatStartWith(currentNode->m_GeoCoord, vectorOfPreChildrenStreetSegments);
		for (int i = 0; i < vectorOfPreChildrenStreetSegments.size(); ++i)
		{
			Node* childEndNode = new Node;
			childEndNode->m_GeoCoord = vectorOfPreChildrenStreetSegments[i].end;
			childEndNode->prevGeoCoordNode = (currentNode);	//prevGeoCoordNode
			vectorOfChildrenPreCheck.push_back(childEndNode);
		}

		//start of: for each child in the children
		for (int z = 0; z != vectorOfChildrenPreCheck.size(); ++z)
		{
			bool childPreCheckFoundInClosedList = false;
			for (auto y = closedList.begin(); y != closedList.end(); ++y)
			{
				if ((*y)->m_GeoCoord == vectorOfChildrenPreCheck[z]->m_GeoCoord)
				{
					childPreCheckFoundInClosedList = true;
				}
			}
			if (childPreCheckFoundInClosedList)
			{
				continue;
			}

			vectorOfChildrenPreCheck[z]->gCost = currentNode->gCost + distanceEarthMiles(vectorOfChildrenPreCheck[z]->m_GeoCoord, currentNode->m_GeoCoord);
			vectorOfChildrenPreCheck[z]->hCost = distanceEarthMiles(vectorOfChildrenPreCheck[z]->m_GeoCoord, end);
			vectorOfChildrenPreCheck[z]->fCost = vectorOfChildrenPreCheck[z]->gCost + vectorOfChildrenPreCheck[z]->hCost;
			vectorOfChildrenPreCheck[z]->prevGeoCoordNode = currentNode;	//dynamic allocated?

			for(auto openListPos = openList.begin(); openListPos != openList.end(); openListPos++)
			{
				if ((*openListPos)->m_GeoCoord == vectorOfChildrenPreCheck[z]->m_GeoCoord)
				{
					if ((vectorOfChildrenPreCheck[z]->gCost > (*openListPos)->gCost))
					{
						continue;
					}
				}
			}
			openList.push_front(vectorOfChildrenPreCheck[z]);
		}

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
