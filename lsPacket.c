/**
 * This file implements the functions used for building and modifying link-state packets.
 *
 * @author Jeffrey Bromen
 * @date 4/16/17
 * @info Systems and Networks II
 * @info Project 3
 */

#include "lsPacket.h"

void buildLSPacket(char *buffer, int hopCount, int seqNumber, char source, char destination, int cost)
{
	memset(buffer, hopCount % 256, 1);
	memset(buffer+1, seqNumber % 256, 1);
	memset(buffer+2, source, 1);
	memset(buffer+3, destination, 1);

	int ncost = htonl(cost);
	memcpy(buffer+4, &ncost, 4);
}

int getHopCount(char *lsPacket)
{
	char hopCount;

	memcpy(&hopCount, lsPacket, 1);

	return hopCount;
}

int getSequenceNumber(char *lsPacket)
{
	char seqNumber;

	memcpy(&seqNumber, lsPacket+1, 1);

	return seqNumber;
}

char getSourceID(char *lsPacket)
{
	char source;

	memcpy(&source, lsPacket+2, 1);

	return source;
}

char getDestinationID(char *lsPacket)
{
	char destination;

	memcpy(&destination, lsPacket+3, 1);

	return destination;
}

int getCost(char *lsPacket)
{
	int cost;

	memcpy(&cost, lsPacket+4, 4);

	return ntohl(cost);
}

void printLSPacket(char *lsPacket)
{
	int hopCount, seqNumber, cost;
	char source, destination;

	hopCount = getHopCount(lsPacket);
	seqNumber = getSequenceNumber(lsPacket);
	source = getSourceID(lsPacket);
	destination = getDestinationID(lsPacket);
	cost = getCost(lsPacket);

	printf("Hop Count:       %d\n"
	       "Sequence Number: %d\n"
	       "Source ID:       %c\n"
	       "Destination ID:  %c\n"
	       "Cost:            %d\n",
	       hopCount, seqNumber, source, destination, cost);
}

int decrementHopCount(char *lsPacket)
{
	int hopCount = getHopCount(lsPacket);

	hopCount--;

	memset(lsPacket, hopCount, 1);

	return hopCount;
}