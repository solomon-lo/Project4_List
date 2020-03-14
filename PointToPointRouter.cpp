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

	list<StreetSegment> routeToReverse;

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

	Node* inputStartNode = new Node;
	inputStartNode->m_GeoCoord = start;
	inputStartNode->prevGeoCoordNode = nullptr;
	inputStartNode->fCost = 0;
	inputStartNode->gCost = 0;
	inputStartNode->hCost = 0;

	openList.push_back(inputStartNode);

	while (!openList.empty())
	{
		Node* currentNode = new Node;
		double currentSmallestValue = 999;
		int currentSmallestIndex = 0;

		currentNode = *openList.begin();
		int currentNodeIndex = 0;
		int currentIndex = 0;
		for (auto i = openList.begin(); i != openList.end(); i++)
		{
			if ((*i)->fCost < currentNode->fCost)
			{
				currentSmallestValue = (*i)->fCost;
				cerr << "current smallest fCost" << (*i)->fCost << endl;
				currentNode = *i;
				currentNodeIndex = currentIndex;
			}
			currentIndex++;
		}


		closedList.push_back(currentNode);
		auto deleteIterator = openList.begin();
		for (int i = 0; i < currentNodeIndex; i++)
		{
			deleteIterator++;
		}
		cerr << "test of fcost" << (*deleteIterator)->fCost << endl;
		openList.erase(deleteIterator);




		//foudn goal implementation

		//backtracking portion
		if (currentNode->m_GeoCoord == end)	//in morning: this is never returned as true.
		{
			cerr << "entered backtracking portion" << endl;
			ExpandableHashMap<GeoCoord, GeoCoord> storageOfStreetSegmentReturns;
			cerr << "finally found the end" << endl;
			while (!(currentNode->m_GeoCoord == start))
			{
				storageOfStreetSegmentReturns.associate(currentNode->m_GeoCoord, currentNode->prevGeoCoordNode->m_GeoCoord);
				currentNode = currentNode->prevGeoCoordNode;
			}

			currentNode->m_GeoCoord = end;
			while (currentNode->m_GeoCoord != start)
			{
				cerr << "entered while (currentNode->m_GeoCoord != start)" << endl;
				vector<StreetSegment> tempStreetSegmentReturns;
				m_map->getSegmentsThatStartWith(currentNode->m_GeoCoord, tempStreetSegmentReturns);
				for (int i = 0; i < tempStreetSegmentReturns.size(); i++)
				{
					if (tempStreetSegmentReturns[i].end == *(storageOfStreetSegmentReturns.find(currentNode->m_GeoCoord)))
					{
						StreetSegment reversedInOrderToPush;
						reversedInOrderToPush.start = tempStreetSegmentReturns[i].end;
						reversedInOrderToPush.end = tempStreetSegmentReturns[i].start;
						reversedInOrderToPush.name = tempStreetSegmentReturns[i].name;
						routeToReverse.push_back(reversedInOrderToPush);
						//route.push_back(tempStreetSegmentReturns[i]);
						currentNode->m_GeoCoord = tempStreetSegmentReturns[i].end;
						break;
					}
				}
			}
			routeToReverse.reverse();
			for (auto it = routeToReverse.begin(); it != routeToReverse.end(); it++)
			{
				route.push_back(*it);
			}
			return DELIVERY_SUCCESS;
		}

		//END OF THE IF STATEMENT
		//Generate children

		vector<StreetSegment> childrenStreetSegments;
		vector<Node*> vectorOfNodePointerToChildren;

		m_map->getSegmentsThatStartWith(currentNode->m_GeoCoord, childrenStreetSegments);
		for (int i = 0; i < childrenStreetSegments.size(); ++i)
		{
			Node* childEndNode = new Node;
			childEndNode->m_GeoCoord = childrenStreetSegments[i].end;
			childEndNode->prevGeoCoordNode = (currentNode);	//prevGeoCoordNode
			vectorOfNodePointerToChildren.push_back(childEndNode);
		}

		//start of: for each child in the children
		for (int z = 0; z < vectorOfNodePointerToChildren.size(); ++z)
		{
			bool childPreCheckFoundInClosedList = false;
			for (auto y = closedList.begin(); y != closedList.end(); ++y)
			{
				if ((*y)->m_GeoCoord == vectorOfNodePointerToChildren[z]->m_GeoCoord)
				{
					childPreCheckFoundInClosedList = true;
					break;
				}
			}
			if (childPreCheckFoundInClosedList)
			{
				continue;
			}

			vectorOfNodePointerToChildren[z]->gCost = currentNode->gCost + distanceEarthMiles(vectorOfNodePointerToChildren[z]->m_GeoCoord, currentNode->m_GeoCoord);
			vectorOfNodePointerToChildren[z]->hCost = distanceEarthMiles(vectorOfNodePointerToChildren[z]->m_GeoCoord, end);
			vectorOfNodePointerToChildren[z]->fCost = vectorOfNodePointerToChildren[z]->gCost + vectorOfNodePointerToChildren[z]->hCost;
			vectorOfNodePointerToChildren[z]->prevGeoCoordNode = currentNode;
			bool needToSkip = false;
			for (auto openListPos = openList.begin(); openListPos != openList.end(); openListPos++)
			{
				if ((*openListPos)->m_GeoCoord == vectorOfNodePointerToChildren[z]->m_GeoCoord)
				{
					if ((vectorOfNodePointerToChildren[z]->gCost > (*openListPos)->gCost))
					{
						needToSkip = true;
						break;
					}
				}
			}
			if (!needToSkip)
			{
				continue;
			}
			openList.push_front(vectorOfNodePointerToChildren[z]);
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