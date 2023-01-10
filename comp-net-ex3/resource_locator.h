#pragma once

#include <string>

using namespace std;

class ResourceLocator {

public:
	static const string k_RootPath = "/www";
	string getPathToFile(const string& i_URI);
};