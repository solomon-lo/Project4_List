#include "ExpandableHashMap.h"
#include "StreetMap.cpp"
#include <string>
#include <iostream>
using namespace std;
//credit:adapted from Carey Nachenberg's slides

unsigned int hasher(const std::string& k)
{
	std::hash<std::string> stringHash;
	unsigned int h = stringHash(k);
	return h;
}

void main()
{
	// Define a hashmap that maps strings to doubles and has a maximum
	// load factor of 0.3. It will initially have 8 buckets when empty.
	ExpandableHashMap<string, double> nameToGPA(0.3);
	// Add new items to the hashmap. Inserting the third item will cause
   // the hashmap to increase the number of buckets (since the maximum
   // load factor is 0.3), forcing a rehash of all items.
	nameToGPA.associate("Carey", 3.5); // Carey has a 3.5 GPA
	nameToGPA.associate("David", 3.99); // David beat Carey
	nameToGPA.associate("Abe", 3.2); // Abe has a 3.2 GPA

	double* davidsGPA = nameToGPA.find("David");
	if (davidsGPA != nullptr)
		*davidsGPA = 1.5; // after a re-grade of David’s exam
	nameToGPA.associate("Carey", 4.0); // Carey deserves a 4.0
	// replaces old 3.5 GPA
	double* lindasGPA = nameToGPA.find("Abe");
	if (lindasGPA == nullptr)
		cout << "Linda is not in the roster!" << endl;
	else
		cout << "Linda's GPA is: " << *lindasGPA << endl;
}