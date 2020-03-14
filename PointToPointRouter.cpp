#include "provided.h"
#include "ExpandableHashMap.h"
#include <list>
#include <set>
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

	vector<GeoCoord> usedGeoCoords;
	Node* currentCoord = new Node;
	currentCoord->m_GeoCoord = start;
	usedGeoCoords.push_back(currentCoord->m_GeoCoord);

	vector<StreetSegment> eachPossibleLocation;
	m_map->getSegmentsThatStartWith(currentCoord->m_GeoCoord, eachPossibleLocation);
	for (int i = 0; i < eachPossibleLocation.size(); i++)
	{
		for(auto it)
	}

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