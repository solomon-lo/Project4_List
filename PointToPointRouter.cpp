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

	vector<Node*> openList;
	vector<Node*> closedList;

	Node inputStartNode;
	inputStartNode.m_GeoCoord = start;
	inputStartNode.prevGeoCoordNode = nullptr;
	inputStartNode.fCost = 0;
	inputStartNode.gCost = 0;
	inputStartNode.hCost = 0;

	openList.insert(openList.begin(), &inputStartNode);

	while(!openList.empty())
	{
		Node * currentNode = new Node;
		double currentSmallestValue = 999;
		int currentSmallestIndex = 0;
		for (int i = 0; i < openList.size(); i++)
		{
			if (openList[i]->fCost < currentSmallestValue)
			{
				currentSmallestValue = openList[i]->fCost;
				currentSmallestIndex = i;
			}
		}
		currentNode = openList[currentSmallestIndex];

		auto deleteIterator = openList.begin();
		for (int j = 0; j < currentSmallestIndex; j++)
		{
			deleteIterator++;
		}


		openList.erase(deleteIterator);

		closedList.insert(closedList.begin(), currentNode);
		
		//foudn goal implementation

		//backtracking portion
		if (currentNode->m_GeoCoord == end)	//in morning: this is never returned as true.
		{
			//TODO:CHECK HERE FIRST IF THERE'S ERROR
			Node similarToIteratorPointer = *currentNode;	//todo:not sure about this
			while (similarToIteratorPointer.prevGeoCoordNode != nullptr)
			{

				vector<StreetSegment> dummyVector;
				StreetSegment tempStreetSegment;
				if (m_map->getSegmentsThatStartWith((similarToIteratorPointer.m_GeoCoord), dummyVector))
				{
					for (int i = 0; i < dummyVector.size(); ++i)
					{
						int tracker = 0;
						if (dummyVector[i].end == similarToIteratorPointer.prevGeoCoordNode->m_GeoCoord)
						{

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
				}
				similarToIteratorPointer = *(similarToIteratorPointer.prevGeoCoordNode);
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
			//Node childStartNode;
			//childStartNode.m_GeoCoord = vectorOfPreChildrenStreetSegments[i].start;
			//childStartNode.prevGeoCoordNode = &currentNode;
			//currentNode = childStartNode;	//TODO:NOT SURE IF THIS LINE SHOULD BE IN HERE
			if (vectorOfPreChildrenStreetSegments[i].start == currentNode->m_GeoCoord)
			{
				Node * childEndNode = new Node;
				childEndNode->m_GeoCoord = vectorOfPreChildrenStreetSegments[i].end;
				childEndNode->prevGeoCoordNode = (currentNode->prevGeoCoordNode);	//prevGeoCoordNode
				//vectorOfChildrenPreCheck.push_back(childStartNode);
				vectorOfChildrenPreCheck.push_back(childEndNode);
			}


			//bool vectorOfchildrenSegmentsElementIAlreadyInClosedList = false;
			//for (int j = 0; j < closedList.size(); ++j)
			//{
			//	if ((closedList[j].m_GeoCoord == vectorOfPreChildrenStreetSegments[i].start) || (closedList[j].m_GeoCoord == vectorOfPreChildrenStreetSegments[i].end))
			//	{
			//		vectorOfchildrenSegmentsElementIAlreadyInClosedList = true;
			//	}
			//}
			//if (vectorOfchildrenSegmentsElementIAlreadyInClosedList)
			//{
			//	continue;
			//}

		}

		//start of: for each child in the children
		for (int z = 0; z != vectorOfChildrenPreCheck.size(); ++z)
		{
			bool childPreCheckFoundInClosedList = false;
			for (int y = 0; y != closedList.size(); ++y)
			{
				if (closedList[y]->m_GeoCoord == vectorOfChildrenPreCheck[z]->m_GeoCoord)
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
			vectorOfChildrenPreCheck[z]->fCost = vectorOfChildrenPreCheck[z]->gCost + vectorOfChildrenPreCheck[z]->gCost;
			vectorOfChildrenPreCheck[z]->prevGeoCoordNode = currentNode;	//dynamic allocated?

			for (int openListPos = 0; openListPos < openList.size(); openListPos++)
			{
				if (openList[openListPos]->m_GeoCoord == vectorOfChildrenPreCheck[z]->m_GeoCoord && vectorOfChildrenPreCheck[z]->gCost > openList[openListPos]->gCost)
				{
					continue;
				}
			}
			openList.insert(openList.begin(), vectorOfChildrenPreCheck[z]);
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
