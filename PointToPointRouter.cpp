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


//creates a list of StreetSegments to get form the start to the end
DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
	const GeoCoord& start,
	const GeoCoord& end,
	list<StreetSegment>& route,
	double& totalDistanceTravelled) const
{
	//used to see if there are any good or bad coordinates
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

	//sees if the start is equal to the end, and what will occurr
	if (start == end)
	{
		totalDistanceTravelled = 0.00;
		route.clear();
		return DELIVERY_SUCCESS;
	}

	//A* implementation, this is needed as the two lists
	list<Node*> openList;
	list<Node*> closedList;

	//pushes in the start aas the new node
	Node* inputStartNode = new Node;
	inputStartNode->m_GeoCoord = start;
	inputStartNode->prevGeoCoordNode = nullptr;
	inputStartNode->fCost = 0;
	inputStartNode->gCost = 0;
	inputStartNode->hCost = 0;
	openList.push_back(inputStartNode);


	while (!openList.empty())
	{
		Node* currentNode = *openList.begin();
		double currentSmallestValue = 999;
		int currentSmallestIndex = 0;
		int currentIndex = 0;

		//finds the smallest fCost n the openList
		for (auto i = openList.begin(); i != openList.end(); i++)
		{
			if ((*i)->fCost < currentSmallestValue)
			{
				currentNode = *i;
				currentSmallestValue = (*i)->fCost;
				cerr << "current smallest fCost" << (*i)->fCost << endl;
				currentSmallestIndex = currentIndex;
			}
			currentIndex++;
		}


		//erases the smallest fCost form the openList and pushes it into the closedList.
		auto it = openList.begin();
		for (int i = 0; i < currentSmallestIndex; i++)
		{
			++it;
		}
		closedList.push_back(*it);
		openList.erase(it);


		//ran when we have reached our goal, and we need to start backtracking
		if (currentNode->m_GeoCoord == end)
		{
			ExpandableHashMap<GeoCoord, GeoCoord> storageOfStreetSegmentReturns;
			cerr << "finally found the end" << endl;
			while (!(currentNode->m_GeoCoord == start))
			{
				storageOfStreetSegmentReturns.associate(currentNode->m_GeoCoord, currentNode->prevGeoCoordNode->m_GeoCoord);
				currentNode = currentNode->prevGeoCoordNode;
			}

			//goes form the last to the first
			currentNode->m_GeoCoord = end;
			while (currentNode->m_GeoCoord != start)
			{
				cerr << "entered while (currentNode->m_GeoCoord != start)" << endl;
				vector<StreetSegment> tempStreetSegmentReturns;
				m_map->getSegmentsThatStartWith(currentNode->m_GeoCoord, tempStreetSegmentReturns);
				for (int i = 0; i != tempStreetSegmentReturns.size(); i++)
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

			//reverses the route, calculates the distance
			routeToReverse.reverse();
			for (auto it = routeToReverse.begin(); it != routeToReverse.end(); it++)
			{
				route.push_back(*it);
				totalDistanceTravelled += distanceEarthMiles(it->start, it->end);
			}

			//deallocates our dynamically allocated variables to make sure we don't have any memory leaks
			for (auto itr = openList.begin(); itr != openList.end(); ++itr)
			{
				Node* child = *itr;
				delete child;
			}
			for (auto itr = closedList.begin(); itr != closedList.end(); ++itr)
			{
				Node* child = *itr;
				delete child;
			}
			return DELIVERY_SUCCESS;
		}

		//Generate children by calling getSegmentsThatStartWith

		vector<StreetSegment> vectorOfPreChildrenStreetSegments;
		vector<Node> vectorOfChildrenNodes;

		m_map->getSegmentsThatStartWith(currentNode->m_GeoCoord, vectorOfPreChildrenStreetSegments);
		for (int i = 0; i < vectorOfPreChildrenStreetSegments.size(); ++i)
		{
			Node childEndNode;
			childEndNode.m_GeoCoord = vectorOfPreChildrenStreetSegments[i].end;
			childEndNode.prevGeoCoordNode = currentNode;	//prevGeoCoordNode
			vectorOfChildrenNodes.push_back(childEndNode);
		}

		//deletes the dynamically allocated elements in vectorOfChildrenNodes

		//start of: for each child in the children
		for (int z = 0; z != vectorOfChildrenNodes.size(); ++z)
		{
			bool childPreCheckFoundInClosedList = false;

			//looks to see if the child node was found in the closedList
			for (auto y = closedList.begin(); y != closedList.end(); ++y)
			{
				if ((*y)->m_GeoCoord == vectorOfChildrenNodes[z].m_GeoCoord)
				{
					childPreCheckFoundInClosedList = true;
				}
			}
			if (childPreCheckFoundInClosedList)
			{
				continue;
			}


			//generates the proper values for the g, h, and f cost of the child
			Node* toPushIntoOpenList = new Node;
			toPushIntoOpenList->gCost = currentNode->gCost + distanceEarthMiles(vectorOfChildrenNodes[z].m_GeoCoord, currentNode->m_GeoCoord);
			toPushIntoOpenList->hCost = distanceEarthMiles(vectorOfChildrenNodes[z].m_GeoCoord, end);
			toPushIntoOpenList->fCost = vectorOfChildrenNodes[z].gCost + vectorOfChildrenNodes[z].hCost;
			toPushIntoOpenList->prevGeoCoordNode = currentNode;
			toPushIntoOpenList->m_GeoCoord = vectorOfChildrenNodes[z].m_GeoCoord;
			bool needToSkip = false;

			//if the child is found in the openList, to also analyze it's gCost
			for (auto openListPos = openList.begin(); openListPos != openList.end(); openListPos++)
			{
				if (((*openListPos)->m_GeoCoord == toPushIntoOpenList->m_GeoCoord) && ((toPushIntoOpenList->gCost > (*openListPos)->gCost)))
				{

					needToSkip = true;
				}
			}
			if (needToSkip)
			{
				delete toPushIntoOpenList;
				continue;
			}
			openList.push_front(toPushIntoOpenList);
		}
	}

	//deallocates all of the used memory before it returns
	for (auto itr = openList.begin(); itr != openList.end(); ++itr)
	{
		Node* child = *itr;
		delete child;
	}
	for (auto itr = closedList.begin(); itr != closedList.end(); ++itr)
	{
		Node* child = *itr;
		delete child;
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