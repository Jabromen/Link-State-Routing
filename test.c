#include "lsPacket.h"

int main(void)
{
	char lsPacket[LS_PACKET_SIZE];

	buildLSPacket(lsPacket, 3, 0, 'A', 'B', 2142352);

	printLSPacket(lsPacket);

	int hopCount = getHopCount(lsPacket);

	while (hopCount > 0)
	{
		hopCount = decrementHopCount(lsPacket);
		printLSPacket(lsPacket);
	}

	return 0;
}