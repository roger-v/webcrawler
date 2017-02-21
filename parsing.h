/*
Roger Villanueva
CSCE 463-500
Spring 2017
*/

#pragma once
#include <string>
using std::string;
void printParsed(const string & url);
bool validScheme(const string & url);
string getHost(const string & url);
string getPort(const string & url);
string getPath(const string & url);
string getQuery(const string & url);
string getFragment(const string & url);
string getRequest(const string & url);
bool matches(const char & c, const string & contents);