/*
Roger Villanueva
CSCE 463-500
Spring 2017
*/

#include "stdafx.h"

void crawl(Parameters * p, const string & url) {
	struct hostent * remote;
	struct sockaddr_in server;
	string host = getHost(url);
	if (!p->isUniqueHost(host)) return;
	DWORD IP = inet_addr(host.c_str());
	if (IP != INADDR_NONE) {
		if (!p->isUniqueIP(IP)) return;
	}
	if (IP == INADDR_NONE) {
		if ((remote = gethostbyname(host.c_str())) == NULL) return;
		else {
			memcpy((char*)&(server.sin_addr), remote->h_addr, remote->h_length);
			InterlockedIncrement(&(p->sharedData.successfulDNSlookups));
		}
	}
	else server.sin_addr.S_un.S_addr = IP;
	if (!p->isUniqueIP(server.sin_addr.S_un.S_addr)) return;
	server.sin_family = AF_INET;
	string port = getPort(url);
	server.sin_port = htons((short)atoi(port.c_str()));
	string robotsRequest = getRequestString(host, "/robots.txt", REQTYPE_HEAD);
	bool robotsDone = connectDownloadAndVerify(server, host, robotsRequest, "4xx", 16000, p);
	if (robotsDone) InterlockedIncrement(&(p->sharedData.urlsThatPassedRobots));
	else return;
	string pageRequest = getRequestString(host, getRequest(url), REQTYPE_GET);
	bool pageDone = connectDownloadAndVerify(server, host, pageRequest, "2xx", 2000000, p);
	return;
}

string getRequestString(const string & host, const string & request, int reqType) {
	string type;
	if (reqType == REQTYPE_GET) type = "GET";
	else type = "HEAD";
	return type + ' ' + request + " HTTP/1.0\r\nUser-agent: myTAMUcrawler/1.3\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
}

bool connectDownloadAndVerify(struct sockaddr_in & server, const string & url, const string & request, char * validStatusCode, unsigned int maxDownloadSize, Parameters * p) {
	Socket my_sock;
	my_sock.maxDownloadSize = maxDownloadSize;
	if (!my_sock.Open()) return false;
	if (connect(my_sock.sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
		my_sock.Close(true);
		return false;
	}
	
	if (!my_sock.Send(request.c_str())) {
		my_sock.Close(false);
		return false;
	}
	if (!my_sock.Read()) {
		my_sock.Close(false);
		return false;
	}
	bool goodStatusCode = (validStatusCode[0] == my_sock.statusCode[0]);
	p->incrHTTPCode(my_sock.statusCode);
	p->incrBytesCount(my_sock.statusCode, my_sock.receivedBytes);
	if (!goodStatusCode) {
		my_sock.Close(true);
		return false;
	}
	p->mutexAdd(p->bytesMutex, p->sharedData.totalBytes, my_sock.receivedBytes);
	
	if (!isHTTPreply(my_sock.buf)) { 
		my_sock.Close(true);
		return false; 
	}
	bool isRobots = strstr(request.c_str(), "/robots.txt") != NULL;// (request == "/robots.txt");
	
	if (isRobots && goodStatusCode) { 
		InterlockedIncrement(&(p->sharedData.urlsThatPassedRobots)); 
		p->incrHTTPCode(validStatusCode);
	}
	if (!isRobots && goodStatusCode) {
		HTMLParserBase * parser = new HTMLParserBase;
		int nLinks;
		char * startOfContent = strstr(my_sock.buf, "\r\n\r\n") + 4;
		int lengthOfContent = my_sock.receivedBytes - (int)(startOfContent - my_sock.buf);
		if (lengthOfContent > maxDownloadSize) {
			my_sock.Close(true);
			return false;
		}
		char * baseURL = new char[8 + strlen(url.c_str())];
		strcpy(baseURL, "http://");
		strcpy(baseURL + 7, url.c_str());
		baseURL[7 + strlen(url.c_str())] = '\0';

		char *linkBuffer = parser->Parse(startOfContent, lengthOfContent, baseURL, (int)strlen(url.c_str()), &nLinks);
		// check for errors indicated by negative values
		if (nLinks < 0)
			nLinks = 0;
		//if (print) printf("done in %hu ms with %i links\n", stopwatch.stop_ms(), nLinks);
		/*else if (nLinks > 0) { 
			int tamuLinks = countTamuLinks(linkBuffer, nLinks); 
			p->mutexAdd(p->tamuLinksMutex, p->sharedData.tamuLinks, tamuLinks);
			if (tamuLinks > 0) {
				if (url.find("tamu.edu") == string::npos) {
					InterlockedIncrement(&(p->sharedData.nonTamuHosts));
				}
			}
		}*/
		InterlockedIncrement(&(p->sharedData.crawledURLs));
		p->mutexAdd(p->linksMutex, p->sharedData.totalLinksFound, nLinks);
		delete parser;
		delete[] baseURL;
		
	}
	else if (!goodStatusCode) {
		my_sock.Close(true);
		return false;//invalid status code
	}
	my_sock.Close(true);
	return true;
}

bool isHTTPreply(const char * reply) {
	char * httpReply = new char[5]; httpReply[4] = '\0';
	strncpy(httpReply, reply, 4);
	if (strncmp(httpReply, "HTTP", 4) != 0) {
		delete[] httpReply;
		return false;
	}
	delete[] httpReply;
	return true;
}

int countTamuLinks(char * buf, int nLinks) {
	char * c = buf;
	int rval = 0;
	for (int i = 0; i < nLinks; i++) {
		string currentString = "";
		while (*c != '\0') {
			currentString += *c;
			c++;
		}
		string host = getHost(currentString);
		if (host.find("tamu.edu") != string::npos) {
			return 1;
		}
		c++;
	}
	return rval;
}