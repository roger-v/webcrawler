/*
Roger Villanueva
CSCE 463-500
Spring 2017
*/

#include "stdafx.h"

//void printParsed

void printParsed(const string & url) {
	printf("url: %s\n", url.c_str());
	printf("\thost: %s\n", getHost(url).c_str());
	printf("\tport: %s\n", getPort(url).c_str());
	printf("\tpath: %s\n", getPath(url).c_str());
	printf("\tquery: %s\n", getQuery(url).c_str());
	printf("\tfragment: %s\n", getFragment(url).c_str());
	printf("\trequest: %s\n\n", getRequest(url).c_str());
}

bool validScheme(const string & url) {
	if (url.substr(0, 7) != "http://") return false;
	return true;
}

string getHost(const string & url) {
	string rval = "";
	for (int i = 7; i < url.length(); i++) {
		if (matches(url[i], ":/?#")) break;
		rval += url[i];
	}
	return rval;
}

string getPort(const string & url) {
	string rval = "";
	bool pastColon = false;
	for (int i = 7; i < url.length(); i++) {
		if (matches(url[i], "/?#")) break;
		if (pastColon) rval += url[i];
		if (url[i] == ':') pastColon = true;
	}
	if (rval == "") rval = "80";
	return rval;
}

string getPath(const string & url) {
	string rval = "";
	bool foundSlash = false;
	// case 1: slash exists ... 2: slash doesnt exist
	for (int i = 7; i < url.length(); i++) {
		if (matches(url[i], "?#") && !foundSlash) return "/";
		if (matches(url[i], "?#") && foundSlash) return rval;
		if (url[i] == '/') foundSlash = true;
		if (foundSlash) rval += url[i];
	}
	if (!foundSlash) return "/";
	return rval;
}

string getQuery(const string & url) {
	string rval = "";
	bool pastQmark = false;
	for (int i = 7; i < url.length(); i++) {
		if (matches(url[i], "/?#") && pastQmark) break;
		if (pastQmark) rval += url[i];
		if (url[i] == '?') pastQmark = true;
	}
	return rval;
}

string getFragment(const string & url) {
	string rval = "";
	bool pastHash = false;
	for (int i = 7; i < url.length(); i++) {
		if (matches(url[i], "/?#") && pastHash) break;
		if (pastHash) rval += url[i];
		if (url[i] == '#') pastHash = true;
	}
	return rval;
}

string getRequest(const string & url) {
	string rval = "";
	bool startAdding = false;
	for (int i = 7; i < url.length(); i++) {
		if (matches(url[i], "/?#")) startAdding = true;
		if (startAdding) rval += url[i];
	}
	if (rval[0] != '/') return '/' + rval;
	return rval;
}

bool matches(const char & c, const string & contents) {
	for (int i = 0; i < contents.length(); i++) {
		if (c == contents[i]) return true;
	}
	return false;
}