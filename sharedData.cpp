/*
Roger Villanueva
CSCE 463-500
Spring 2017
*/

#include "stdafx.h"
Parameters::Parameters(int numthreads) {
	hostsMutex = CreateMutex(NULL, 0, NULL),
		IPsMutex = CreateMutex(NULL, 0, NULL),
		dnsMutex = CreateMutex(NULL, 0, NULL),
		robotsMutex = CreateMutex(NULL, 0, NULL),
		crawledMutex = CreateMutex(NULL, 0, NULL),
		linksMutex = CreateMutex(NULL, 0, NULL),
		threadsMutex = CreateMutex(NULL, 0, NULL),
		bytesMutex = CreateMutex(NULL, 0, NULL),
		queueMutex = CreateMutex(NULL, 0, NULL),
		httpCodeMutex = CreateMutex(NULL, 0, NULL),
		xx2Mutex = CreateMutex(NULL, 0, NULL),
		xx3Mutex = CreateMutex(NULL, 0, NULL),
		xx4Mutex = CreateMutex(NULL, 0, NULL),
		xx5Mutex = CreateMutex(NULL, 0, NULL),
		otherMutex = CreateMutex(NULL, 0, NULL),
		tamuLinksMutex = CreateMutex(NULL, 0, NULL),
		nonTamuMutex = CreateMutex(NULL, 0, NULL);
	sharedData.threadCount = numthreads;
}
bool Parameters::isUniqueHost(const std::string & hostName) {
	WaitForSingleObject(hostsMutex, INFINITE);
	size_t prevSize = sharedData.seenHosts.size();
	
	sharedData.seenHosts.insert(hostName);
	if (sharedData.seenHosts.size() > prevSize) {
		ReleaseMutex(hostsMutex);
		return true;
	}
	ReleaseMutex(hostsMutex);
	return false;
}
bool Parameters::isUniqueIP(DWORD ip) {
	WaitForSingleObject(IPsMutex, INFINITE);
	size_t prevSize = sharedData.seenIPs.size();
	sharedData.seenIPs.insert(ip);
	if (sharedData.seenIPs.size() > prevSize) {
		ReleaseMutex(IPsMutex);
		return true;
	}
	ReleaseMutex(IPsMutex);
	return false;
}
bool Parameters::queueEmpty() {
	WaitForSingleObject(queueMutex, INFINITE);
	bool rval = sharedData.pendingURLs.size() == 0;
	ReleaseMutex(queueMutex);
	return rval;
}
string Parameters::queuePopFront() {
	WaitForSingleObject(queueMutex, INFINITE);
	if (sharedData.pendingURLs.size() <= 0) {
		ReleaseMutex(queueMutex);
		return "";
	}
	string s(sharedData.pendingURLs.front());
	//printf("%s..\n", s.c_str());
	delete[] sharedData.pendingURLs.front();
	sharedData.pendingURLs.pop();
	ReleaseMutex(queueMutex);
	return s;
}
void Parameters::mutexAdd(HANDLE & mutex, unsigned long & addend, const unsigned long & val) {
	WaitForSingleObject(mutex, INFINITE);
	addend += val;
	ReleaseMutex(mutex);
}

void Parameters::incrHTTPCode(char * code) {
	switch (code[0]) {
	case '2':
		InterlockedIncrement(&(sharedData.xx2));
		break;
	case '3':
		InterlockedIncrement(&(sharedData.xx3));
		break;
	case '4':
		InterlockedIncrement(&(sharedData.xx4));
		break;
	case '5':
		InterlockedIncrement(&(sharedData.xx5));
		break;
	default:
		InterlockedIncrement(&(sharedData.other));
		break;
	}
}

void Parameters::incrBytesCount(char * code, unsigned long bytes) {
	switch (code[0]) {
	case '2':
		mutexAdd(xx2Mutex, sharedData.xx2bytes, bytes);
		break;
	case '3':
		mutexAdd(xx3Mutex, sharedData.xx3bytes, bytes);
		break;
	case '4':
		mutexAdd(xx4Mutex, sharedData.xx4bytes, bytes);
		break;
	case '5':
		mutexAdd(xx5Mutex, sharedData.xx5bytes, bytes);
		break;
	default:
		mutexAdd(otherMutex, sharedData.otherbytes, bytes);
		break;
	}
}