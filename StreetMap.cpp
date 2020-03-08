//#include <string>
//#include "ExpandableHashMap.h"
//#include <vector>
//#include <functional>
//#include "provided.h"
//
////from file i/o writeup on UCLA cs32 website
//#include <iostream> // needed for any I/O
//#include <fstream>  // needed in addition to <iostream> for file I/O
//#include <sstream>  // needed in addition to <iostream> for string stream I/O
//
//using namespace std;
////unsigned int getBucketNumber(const GeoCoord& g);
//unsigned int hasher(const GeoCoord& g)
//{
//    return hasher<string>()(g.latitudeText + g.longitudeText);
//}
//
//unsigned int getBucketNumber(const GeoCoord& g, int numOfBuckets)
//{
//
//	unsigned int h = std::hasher<string>()(g.latitudeText + g.longitudeText);
//	return h % numOfBuckets;
//}
//
//class StreetMapImpl
//{
//public:
//	StreetMapImpl();
//	~StreetMapImpl();
//	bool load(string mapFile);
//	bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
//private:
//	ExpandableHashMap<GeoCoord, vector<StreetSegment>> GeoCoordToStreetSegmentHashMap;
//	
//};
//
//StreetMapImpl::StreetMapImpl()
//{
//}
//
//StreetMapImpl::~StreetMapImpl()
//{
//}
//
//bool StreetMapImpl::load(string mapFile)
//{
//	ifstream infile(mapFile);
//	if (!infile)
//	{
//		cerr << "Error: the StreetMapImpl() function in StreetMap.cpp can't open mapdata.txt!" << endl;
//		return false;
//	}
//
//	string s;
//	while (getline(infile, s))
//	{
//		vector<StreetSegment> vectorOfStreetSegments;
//		vector<StreetSegment> vectorOfReversedStreetSegments;
//		vector<GeoCoord> vectorOfGeoCoords;
//
//
//		StreetSegment tempStreetSegment;
//		StreetSegment reveresedTempStreetSegment;	//the same as tempStreetSegment, but it's actually has switched end and start coordinates
//		tempStreetSegment.name = s;
//		reveresedTempStreetSegment.name = s;
//
//		int numOfSegmentsOnStreet = 0;
//		infile >> numOfSegmentsOnStreet;
//		infile.ignore(10000, '\n');
//		for (int i = 0; i < numOfSegmentsOnStreet; i++)
//		{
//
//
//			getline(infile, s);
//
//
//			vector<string> numbers;
//			istringstream ss(s);
//			// Traverse through all words 
//			do {
//				// Read a word 
//				string word;
//				ss >> word;
//
//				// Print the read word 
//				if (word != "")
//				{
//					numbers.push_back(word);
//				}
//
//				// While there is more to read 
//			} while (ss);
//
//			string X_X_ForStartCoord = numbers[0];
//
//			string Y_Y_ForStartCoord = numbers[1];
//
//			GeoCoord startGeoCoord(X_X_ForStartCoord, Y_Y_ForStartCoord);
//			vectorOfGeoCoords.push_back(startGeoCoord);
//			tempStreetSegment.start = startGeoCoord;
//			reveresedTempStreetSegment.end = startGeoCoord;
//
//			string X_X_ForEndingCoord = numbers[2];
//			string Y_Y_ForEndingCoord = numbers[3];
//
//
//			GeoCoord endGeoCoord(X_X_ForEndingCoord, Y_Y_ForEndingCoord);
//			tempStreetSegment.end = endGeoCoord;
//			reveresedTempStreetSegment.start = endGeoCoord;
//			
//			vectorOfGeoCoords.push_back(endGeoCoord);
//
//			vectorOfStreetSegments.push_back(tempStreetSegment);
//			vectorOfReversedStreetSegments.push_back(reveresedTempStreetSegment);
//		}
//
//
//		for (int i = 0; i < vectorOfGeoCoords.size(); i++)
//		{
//			vector<StreetSegment> StreetSegmentsToAssociate;
//
//			//pushes street segments that have a first coord the same as the GeoCoord
//			for (int j = 0; j < vectorOfStreetSegments.size(); j++)
//			{
//				if (vectorOfStreetSegments[j].start == vectorOfGeoCoords[i])
//				{
//					StreetSegmentsToAssociate.push_back(vectorOfStreetSegments[j]);
//				}
//			}
//
//			//pushes reversed street segments that have a first coord the same as the GeoCoord
//			for (int j = 0; j < vectorOfReversedStreetSegments.size(); j++)
//			{
//				if (vectorOfReversedStreetSegments[j].start == vectorOfGeoCoords[i])
//				{
//					StreetSegmentsToAssociate.push_back(vectorOfReversedStreetSegments[j]);
//				}
//			}
//
//			GeoCoordToStreetSegmentHashMap.associate(vectorOfGeoCoords[i], StreetSegmentsToAssociate);
//		}
//
//	}
//
//	cerr << "reached return true on load" << endl;
//	return true;	//if everything works to plan , this will return true since it reaches end of the text file.
//
//}
//
//bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
//{
//	//ExpandableHashMap<GeoCoord, vector<StreetSegment>> GeoCoordToStreetSegmentHashMap;
//	const vector<StreetSegment>* tempVector = GeoCoordToStreetSegmentHashMap.find(gc);
//	if (tempVector == nullptr)
//	{
//		cerr << "Coord not found" << endl;
//		return false;
//	}
//	else
//	{
//		cerr << "Coord found " << endl;
//		segs = *tempVector;
//		return true;
//	}
//}
//
////******************** StreetMap functions ************************************
//
//// These functions simply delegate to StreetMapImpl's functions.
//// You probably don't want to change any of this code.
//
//StreetMap::StreetMap()
//{
//	m_impl = new StreetMapImpl;
//}
//
//StreetMap::~StreetMap()
//{
//	delete m_impl;
//}
//
//bool StreetMap::load(string mapFile)
//{
//	return m_impl->load(mapFile);
//}
//
//bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
//{
//	return m_impl->getSegmentsThatStartWith(gc, segs);
//}
//
////int main()
////{
////	StreetMapImpl thing;
////	thing.load("shortenedmapdata.txt");
////	GeoCoord endGeoCoord("34.0554131", "-118.4804510");
////	cerr << endGeoCoord.latitudeText << endl;
////	cerr << endGeoCoord.longitudeText << endl;
////	vector<StreetSegment> firstTest;
////	thing.getSegmentsThatStartWith(endGeoCoord, firstTest);
////	for (int i = 0; i < firstTest.size(); i++)
////	{
////		cerr << firstTest[i].start.latitudeText << " " << firstTest[i].start.longitudeText << endl;
////
////		cerr << firstTest[i].end.latitudeText << " " << firstTest[i].end.longitudeText << endl;
////
////	}
////}
//
////34.0547000 -118.4794734 34.0544590 -118.4801137
//
////"34.0508737", "-118.4947372"