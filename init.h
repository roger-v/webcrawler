/*
Roger Villanueva
CSCE 463-500
Spring 2017
*/

#pragma once
#include <queue>
#include <string>
void wsaStart();
void populateQueue(std::queue<std::string> & q, const char * fileName);
void populateQueueC(std::queue<char*> & q, const char * fileName);