/*
Roger Villanueva
CSCE 463-500
Spring 2017
*/

#pragma once
#include <queue>
#include <unordered_set>
class SharedData {
public:
	std::queue<char*> pendingURLs;
	std::unordered_set<string> seenHosts;
	std::unordered_set<DWORD> seenIPs;
	unsigned long threadCount;
	unsigned long extractedURLs;
	unsigned long successfulDNSlookups;
	unsigned long totalBytes;
	unsigned long crawledURLs;
	unsigned long totalLinksFound;
	unsigned long urlsThatPassedRobots;
	unsigned long xx2, xx3, xx4, xx5, other;
	unsigned long xx2bytes, xx3bytes, xx4bytes, xx5bytes, otherbytes;
	unsigned long tamuLinks;
	unsigned long nonTamuHosts;
	SharedData() {
		threadCount = 0;
		extractedURLs = 0;
		successfulDNSlookups = 0;
		totalBytes = 0;
		crawledURLs = 0;
		totalLinksFound = 0;
		urlsThatPassedRobots = 0;
		xx2 = 0;
		xx3 = 0;
		xx4 = 0;
		xx5 = 0;
		other = 0;
		xx2bytes = 0;
		xx3bytes = 0;
		xx4bytes = 0;
		xx5bytes = 0;
		otherbytes = 0;
		tamuLinks = 0;
		nonTamuHosts = 0;
	}
};

class Parameters {
public:
	SharedData sharedData;
	HANDLE hostsMutex, IPsMutex,
		dnsMutex, robotsMutex,
		crawledMutex, linksMutex,
		threadsMutex,
		bytesMutex, queueMutex,
		httpCodeMutex, xx2Mutex,
		xx3Mutex, xx4Mutex, 
		xx5Mutex, otherMutex,
		tamuLinksMutex, nonTamuMutex;
	Parameters(int numthreads);
	bool isUniqueHost(const std::string & hostName);
	bool isUniqueIP(DWORD ip);
	bool queueEmpty();
	std::string queuePopFront();
	void mutexAdd(HANDLE & mutex, unsigned long & addend, const unsigned long & val);
	void incrHTTPCode(char * code);     
	void incrBytesCount(char * code, unsigned long bytes);
};