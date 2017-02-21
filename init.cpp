/*
Roger Villanueva
CSCE 463-500
Spring 2017
*/

#include "stdafx.h"

void wsaStart() {
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		printf("WSAStartup error %d\n", WSAGetLastError());
		WSACleanup();
		exit(0);
	}
}

void populateQueue(queue<string> & q, const char * fileName) {
	printf("Populating queue, please wait... \n");
	ifstream inputFile(fileName);
	int i = 0;
	if (inputFile.is_open()) {
		string line;
		while (getline(inputFile, line)) {
			q.push(line);
			//if (i++ == 100) return;
		}
		printf("done.\n");
	}
	inputFile.close();
}

void populateQueueC(queue<char*> & q, const char * fileName) {
	printf("Populating queue, please wait... \n");
	ifstream inputFile(fileName);
	int i = 0;
	if (inputFile.is_open()) {
		string line;
		
		while (getline(inputFile, line)) {
			char * p = new char[line.length() + 1];
			memcpy(p, line.c_str(), line.length());
			p[line.length()] = '\0';
			q.push(p);
			//if (i++ == 100) return;
		}
		printf("done.\n");
	}
	inputFile.close();
}