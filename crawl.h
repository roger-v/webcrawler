/*
Roger Villanueva
CSCE 463-500
Spring 2017
*/

#pragma once
#include <string>
#define REQTYPE_HEAD 0
#define REQTYPE_GET 1
using std::string;
void crawl(Parameters * p, const string & url);
bool connectDownloadAndVerify(struct sockaddr_in & server, const string & url, const string & request, char * validStatusCode, unsigned int maxDownloadSize, Parameters * p);
string getRequestString(const string & host, const string & request, int reqType);
bool isHTTPreply(const char * reply);
int countTamuLinks(char * buf, int nLinks);