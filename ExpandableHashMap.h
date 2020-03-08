// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

#ifndef expandableHashMap
#define expandableHashMap


#include <string>
#include <functional>
#include <vector>
#include <list>


template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
	ExpandableHashMap(double maximumLoadFactor = 0.5);
	~ExpandableHashMap();
	void deleteAllNodesInVectorOfNodes();
	void reset();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

	//a helper function to delete all of my nodes






private:

	//structure of vector of Lists used from suggestion from TA
	std::vector<std::list<std::pair<KeyType, ValueType>>> vectorOfLists;
	int m_numOfAssociations;
	double currentLoadFactor;
	unsigned int getBucketNumber(const KeyType& key) const
	{

		unsigned int hasher(const KeyType & k); // prototype
		unsigned int h = hasher(key);
		return h % vectorOfLists.size();
	}

};


template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
	:vectorOfLists(8)
{
	m_numOfAssociations = 8;
	currentLoadFactor = maximumLoadFactor;

	//for (int i = 0; i < 8; i++)
	//{
	//	std::list<pair<KeyType, ValueType>> newList;
	//	vectorOfLists.push_back(newList);
	//}

}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::deleteAllNodesInVectorOfNodes()
{
	for (int i = 0; i < vectorOfLists.size(); i++)
	{
		std::list<std::pair<KeyType, ValueType>> tempList = vectorOfLists[i];
		if (!tempList.empty())
		{
			for (int i = 0; i < tempList.size(); i++)
			{
				delete tempList[i];
			}
			tempList.clear();	//TODO:not sure if this is correct
		}

	}
}


template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
	vectorOfLists.clear();
	vectorOfLists.resize(8);
	m_numOfAssociations = 0;
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
	return m_numOfAssociations;
}


template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	if (find(key) == nullptr)
	{
		//checks to see if it exceeds the loadfactor, and creates a new vector if it does
		if ((m_numOfAssociations + 1) > (vectorOfLists.size() * currentLoadFactor))
		{
			std::vector<std::list<std::pair<KeyType, ValueType>>> newHashVector(currentLoadFactor * 2);
			newHashVector.resize(vectorOfLists.size() * 2);

			for (auto i = vectorOfLists.begin(); i != vectorOfLists.end(); i++)
			{
				if (!((*i).empty()))
				{
					std::list<std::pair<KeyType, ValueType>> listToCopyOver = *i;
					for (auto j = listToCopyOver.begin(); j != listToCopyOver.end(); j++)
					{
						//unsigned int getBucketNumber(const KeyType & k) const;
						unsigned int tempHash = getBucketNumber(j->first);
						newHashVector[tempHash].emplace_back(j->first, j->second);
					}
				}
			}
			swap(vectorOfLists, newHashVector);
		}

		//unsigned int getBucketNumber(const KeyType & k);
		int insertBucketHash = getBucketNumber(key);

		vectorOfLists[insertBucketHash].emplace_back(key, value);
		m_numOfAssociations++;
	}
	else
	{
		*find(key) = value;
	}
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const	//TODO:CHANGE BACK TO "KeyType&" instead of "string"
{
	//unsigned int getBucketNumber(const KeyType & k);
	unsigned int findIndex = getBucketNumber(key);
	if (vectorOfLists[findIndex].empty())
	{
		return nullptr;
	}
	else
	{
		for (auto it = vectorOfLists[findIndex].begin(); it != vectorOfLists[findIndex].end(); it++)
		{
			if ((*it).first == key)
			{
				return &((*it).second);
			}
		}

		return nullptr;
	}
}

#endif // !expandableHa