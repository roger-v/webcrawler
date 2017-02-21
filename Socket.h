/*
Roger Villanueva
CSCE 463-500
Spring 2017
*/

#pragma once

#define INITIAL_BUF_SIZE 8000

class Socket {
public:
	bool print;
	SOCKET sock;
	char *buf;
	int allocatedSize;
	int curPos;
	int receivedBytes;
	Socket();
	~Socket();
	bool Read();
	bool Send(const char * request);
	bool Open();
	void Close(bool deleteBuf);
	void PrintBuf();
	char * statusCode;
	unsigned int maxDownloadSize;
};