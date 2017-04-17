/**
 * This file describes the functions used for building and modifying link-state packets.
 * Link-State Packets have the following format:
 * Hop Count (1B) | Sequence Number (1B) | Source ID (1B) | Destination ID (1B) | Cost (4B)
 *
 * @author Jeffrey Bromen
 * @date 4/16/17
 * @info Systems and Networks II
 * @info Project 3
 */

#ifndef _LSPACKET_H
#define _LSPACKET_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Number of bytes in a link-state packet
#define LS_PACKET_SIZE 8

/**
 * Builds a link-state packet with the given parameters and stores it in a buffer.
 *
 * @param buffer      - buffer where the link-state packet will be stored
 * @param hopCount    - number of hops for which the packet will be forwarded (0-255)
 * @param seqNumber   - sequence number of the link-state packet
 *                      used to differentiate from older instances of packets (0-255)
 * @param source      - ID of the source router (single character)
 * @param destination - ID of the destination router (single character)
 * @param cost        - cost to travel from the source router to destination router
 */
void buildLSPacket(char *buffer, int hopCount, int seqNumber, char source, char destination, int cost);

/**
 * Gets the hop count of a link-state packet.
 *
 * @param lsPacket - buffer containing the link-state packet
 *
 * @return - the hop count
 */
int getHopCount(char *lsPacket);

/**
 * Gets the sequence number of a link-state packet.
 *
 * @param lsPacket - buffer containing the link-state packet
 *
 * @return - the sequence number
 */
int getSequenceNumber(char *lsPacket);

/**
 * Gets the source ID of a link-state packet.
 *
 * @param lsPacket - buffer containing the link-state packet
 *
 * @return - the source ID
 */
char getSourceID(char *lsPacket);

/**
 * Gets the destination ID of a link-state packet.
 *
 * @param lsPacket - buffer containing the link-state packet
 *
 * @return - the destination ID
 */
char getDestinationID(char *lsPacket);

/**
 * Gets the cost of a link-state packet.
 *
 * @param lsPacket - buffer containing the link-state packet
 *
 * @return - the cost
 */
int getCost(char *lsPacket);

/**
 * Prints all of the parameters of a link-state packet. Used for debugging.
 *
 * @param lsPacket - buffer containing the link-state packet
 */
void printLSPacket(char *lsPacket);

/**
 * Decrements the hop count of a link-state packet
 *
 * @param lsPacket - buffer containing the link-state packet
 *
 * @return - the new hop count after decrementing
 */
int decrementHopCount(char *lsPacket);

#endif // _LS_PACKET_H