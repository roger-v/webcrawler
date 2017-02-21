/*
Roger Villanueva
CSCE 463-500
Spring 2017
*/

#include "stdafx.h"

Socket::Socket() {
	//allocate buffer once, then possibly reuse for multiple connections in Part 3
	print = true;
	statusCode = new char[4];
	buf = new char[INITIAL_BUF_SIZE];
	allocatedSize = INITIAL_BUF_SIZE;
	curPos = 0;
	receivedBytes = 0;
}

Socket::~Socket() {
	//Close();
}

bool Socket::Open() {
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		WSACleanup();
		return false;
	}
	return true;
}

void Socket::Close(bool deleteBuf) {
	if (deleteBuf) delete[] buf;
	delete[] statusCode;
	closesocket(sock);
}

bool Socket::Read() {
	delete[] buf;
	buf = new char[INITIAL_BUF_SIZE];
	allocatedSize = INITIAL_BUF_SIZE;
	curPos = 0;
	receivedBytes = 0;
	int ret;
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(sock, &fd);
	timeval tval;
	tval.tv_sec = 10; //used by the select function to specify the maximum time the function can take to complete.
	tval.tv_usec = 0;

	int statusCodeLength = strlen("HTTP xxx");
	while (true) {
		//"Since select() modifies the parameters you pass to it, you must reinsert sock into fd_set each time you call select()."
		//is this what he meant ?
		FD_ZERO(&fd);
		FD_SET(sock, &fd);
		if (ret = select(0, &fd, 0, 0, &tval) > 0) {
			int bytes = recv(sock, buf + curPos, allocatedSize - curPos - 1, NULL);

			receivedBytes += bytes;
			if (bytes == SOCKET_ERROR) { //error with recv
				break;
			}
			if (receivedBytes >= maxDownloadSize) {
				break;
			}
			if (bytes == 0) { //connection closed successfully. done
							  //set status code

				memcpy(statusCode, buf + 9, sizeof(char) * 4);
				statusCode[3] = '\0';
				//---------------
				buf[curPos] = '\0'; //null-terminate it for PrintBuf()

				return true;
			}
			curPos += bytes;
			if (allocatedSize - curPos < 100) {
				char * newArr = new char[(int)(2 * allocatedSize)]; //double every time it fills
				allocatedSize = (int) allocatedSize * 2;
				memcpy(newArr, buf, curPos);
				delete[] buf;
				buf = newArr;
			}
		}
		else if (ret == 0) { //timed out
			break;
		}
		else {//error with select
			break;
		}

	}
	delete[] buf;
	return false;
}

bool Socket::Send(const char * request) {
	int i;
	if (i = send(sock, request, strlen(request), 0) != strlen(request)) {
		if (print) printf("Error: send() did not send the total number of bytes expected\n");
		if (print) printf("send() error: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

void Socket::PrintBuf() {
	printf("%s", buf);
}