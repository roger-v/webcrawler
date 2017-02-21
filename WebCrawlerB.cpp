/*
Roger Villanueva
CSCE 463-500
Spring 2017
*/

// WebCrawlerB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

UINT statsThread(LPVOID parameters);
UINT crawlThread(LPVOID parameters);

int main(int argc, char * argv[])
{
	int numthreads = atoi(argv[1]);
	wsaStart();
	Parameters p(numthreads);
	populateQueueC(p.sharedData.pendingURLs, argv[2]);
	HANDLE statsHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)statsThread, &p, 0, NULL);
	HANDLE * crawlHandles = new HANDLE[numthreads];
	for (int i = 0; i < numthreads; i++)
		crawlHandles[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)crawlThread, &p, 0, NULL);
	for (int j = 0; j < numthreads; j++) {
		WaitForSingleObject(crawlHandles[j], INFINITE);
		CloseHandle(crawlHandles[j]);
	}
	WaitForSingleObject(statsHandle, INFINITE);
	CloseHandle(statsHandle);
	delete[] crawlHandles;
	printf("The program has terminated successfully.\n");
	while (true){}
    return 0;
}

UINT statsThread(LPVOID parameters) {
	Parameters* p = (Parameters*)parameters;
	unsigned long second = 0;
	unsigned long activeThreads, Q, E, H, D, I, R, C, L;
	double pps = 0;
	double mbps = 0;
	unsigned long lastCount = 0;
	unsigned long lastBytes = 0;
	while (true) {
		second += 2;
		//WaitForSingleObject(p->mutex, 2000);
		activeThreads = p->sharedData.threadCount;
		if (activeThreads == 0) {
			break;
		}
		Q = p->sharedData.pendingURLs.size();
		E = p->sharedData.extractedURLs;
		H = p->sharedData.seenHosts.size();
		D = p->sharedData.successfulDNSlookups;
		I = p->sharedData.seenIPs.size();
		R = p->sharedData.urlsThatPassedRobots;
		C = p->sharedData.crawledURLs;
		L = p->sharedData.totalLinksFound;
		pps = (double)(C - lastCount) / (double)second;
		mbps = (double)(p->sharedData.totalBytes - lastBytes) / (double)(125000 * second);
		lastCount = C;
		lastBytes = p->sharedData.totalBytes;
		//ReleaseMutex(p->mutex);

		//67665554
		printf("[%3lu] %3lu Q%6lu E%7lu H%6lu D%6lu I%5lu R%5lu C%5lu L%4luK\n", second, activeThreads, Q, E, H, D, I, R, C, L / 1000);
		printf("\t*** crawling %.1f pps @ %.1f Mbps\n", pps, mbps);
		Sleep(2000);

	}

	printf("Extracted %i URLS @ %i/s\n", E, (int)((double)E / (double)second));
	printf("Looked up %i DNS names @ %i/s\n", D, (int)((double)D / (double)second));
	printf("Downloaded %i robots @ %i/s\n", R, (int)((double)R / (double)second));
	printf("Crawled %i pages @ %i/s\n", C, (int)((double)C / (double)second));
	printf("Parsed %i links @ %i/s\n", L, (int)((double)L / (double)second));
	printf("HTTP codes: 2xx = %lu, 3xx = %lu, 4xx = %lu, 5xx = %lu, other = %lu\n", p->sharedData.xx2, p->sharedData.xx3, p->sharedData.xx4, p->sharedData.xx5, p->sharedData.other);
	printf("2xx totalBytes = %lu, 3xx totalBytes = %lu, 4xx totalBytes = %lu, 5xx totalBytes = %lu, other totalBytes = %lu\n", p->sharedData.xx2bytes, p->sharedData.xx3bytes, p->sharedData.xx4bytes, p->sharedData.xx5bytes, p->sharedData.otherbytes);
	//printf("pages that had tamu links: %lu\n", p->sharedData.tamuLinks);
	//printf("pages above that were not from tamu: %lu\n", p->sharedData.nonTamuHosts);
	return 0;
}

UINT crawlThread(LPVOID parameters) {
	Parameters * p = (Parameters*)parameters;
	while (true) {
		if (p->queueEmpty()) {
			InterlockedDecrement(&(p->sharedData.threadCount));
			return 0;
		}
		string currentUrl = p->queuePopFront();
		InterlockedIncrement(&(p->sharedData.extractedURLs));
		if (validScheme(currentUrl)) crawl(p, currentUrl);
	}
	return 0;
}

