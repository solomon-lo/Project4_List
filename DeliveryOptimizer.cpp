#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
	DeliveryOptimizerImpl(const StreetMap* sm);
	~DeliveryOptimizerImpl();
	void optimizeDeliveryOrder(
		const GeoCoord& depot,
		vector<DeliveryRequest>& deliveries,
		double& oldCrowDistance,
		double& newCrowDistance) const;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
	const GeoCoord& depot,
	vector<DeliveryRequest>& deliveries,
	double& oldCrowDistance,
	double& newCrowDistance) const
{
	oldCrowDistance = 0;
	oldCrowDistance += distanceEarthMiles(depot, deliveries[0].location);
	for (int i = 0; i < deliveries.size() - 1; i++)
	{
		oldCrowDistance += distanceEarthMiles(deliveries[i].location, deliveries[i + 1].location);
	}
	oldCrowDistance += distanceEarthMiles(deliveries[deliveries.size() - 1].location, depot);

	int compareDistance = 9999999;
	do
	{
		vector<DeliveryRequest> preGreedyPoint = deliveries;
		int smallestIndex = 0;
		int smallestDistanceCrow = 9999999999999;
		for (int i = 0; i < preGreedyPoint.size(); i++)
		{
			if (distanceEarthMiles(preGreedyPoint[i].location, depot) < smallestDistanceCrow)
			{
				smallestIndex = i;
				smallestDistanceCrow = distanceEarthMiles(preGreedyPoint[i].location, depot);
			}
		}

		vector<DeliveryRequest> postGreedy;
		postGreedy.push_back(preGreedyPoint[smallestIndex]);

		while (preGreedyPoint.empty() == false)
		{
			int indexTracker = 0;
			int currentSmallestCrow = 999999999;
			for (int i = 0; i < preGreedyPoint.size(); i++)
			{
				if (distanceEarthMiles(preGreedyPoint[i].location, depot) < smallestDistanceCrow)
				{
					smallestIndex = i;
					smallestDistanceCrow = distanceEarthMiles(preGreedyPoint[i].location, postGreedy[postGreedy.size() - 1].location);
				}
			}
			vector<DeliveryRequest> postGreedy;
			postGreedy.push_back(preGreedyPoint[smallestIndex]);

			auto itr = preGreedyPoint.begin();
			for (int i = 0; i < indexTracker; i++)
			{
				++itr;
			}
			preGreedyPoint.erase(itr);
		}

		compareDistance = 0;
		compareDistance += distanceEarthMiles(depot, postGreedy[0].location);
		for (int i = 0; i < postGreedy.size() - 1; i++)
		{
			compareDistance += distanceEarthMiles(postGreedy[i].location, postGreedy[i + 1].location);
		}
		compareDistance += distanceEarthMiles(postGreedy[postGreedy.size() - 1].location, depot);

		if (compareDistance < oldCrowDistance)
		{
			deliveries = postGreedy;
			newCrowDistance = compareDistance;
		}

	} while (compareDistance > oldCrowDistance);

}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
	m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
	delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
	const GeoCoord& depot,
	vector<DeliveryRequest>& deliveries,
	double& oldCrowDistance,
	double& newCrowDistance) const
{
	return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
