/*
 * ELMS - Trevor Frame, Andrew Freitas, Deborah Kretzschmar
*/
#include <iostream>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include "parse_incoming.h"

using std::cout;
using std::endl;
using std::cerr;
using std::stoi;
using std::stof;
using std::replace;
using std::exception;
using std::stringstream;
using std::remove_if;


/* This function take the file path and message and opens the file and appends
 *  the current message. Once it is finished it closes the file. */
void storeMessage(string & filePath, string &message) {
	ofstream inputFile;
	inputFile.open(filePath, std::ios::out | std::ios::app);

	// if the log file is open, then write to it. 
	if (inputFile.is_open())
	{
		inputFile << message;
	}
	inputFile.close();
}

/* Reference for how to remove blank spaces in a string
 * https://stackoverflow.com/questions/83439/remove-spaces-from-stdstring-in-c
 */
message* createNewMessage(string incomingMessage)
{
	message* newMessage = new message; // create new message

	string temp;	// setup tokenizer
	char delim = ',';

	// this removes any blank spaces that may be in the message before parsing
	incomingMessage.erase(remove_if(incomingMessage.begin(), incomingMessage.end(), isspace), incomingMessage.end());

	incomingMessage = incomingMessage.substr(1, incomingMessage.length() - 1); // remove $ and *

	stringstream check1(incomingMessage);


	for (int dataPoint = 0; dataPoint < 6; dataPoint++)
	{
		getline(check1, temp, delim);

		switch (dataPoint)
		{
		case 0: newMessage->vehicle = stringToInt(temp); break;
		case 1: newMessage->time = stringToInt(temp); break;
		case 2: newMessage->latitude = stringToDouble(temp); break;
		case 3: newMessage->longitude = stringToDouble(temp); break;
		case 4: newMessage->velocity = stringToInt(temp); break;
		case 5: newMessage->bearing = stringToInt(temp); break;
		default: break;
		}
	}
	return newMessage;

}