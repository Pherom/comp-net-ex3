#include "resource_locator.h"

string ResourceLocator::getPathToFile(const string& i_URI) {
	int indexOfQueryStart = i_URI.find('?');
	string pathToFile = k_RootPath;
	if (indexOfQueryStart != string::npos) {
		pathToFile += i_URI.substr(0, indexOfQueryStart);
	}
	else {
		pathToFile += i_URI;
	}
}