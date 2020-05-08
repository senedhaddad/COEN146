/**************************
	Sened Haddad
	COEN146L Lab 4
	Thursday - 2:15pm
	SERVER
 **************************/


#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include "tfv2.h"

/********************
 * main
 ********************/
int main (int argc, char *argv[])
{
	int sock, nBytes;
	struct sockaddr_in serverAddr, clientAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size, client_addr_size;
	char buffer[10];
	int i;

    if (argc != 2)
    {
        printf ("need the port number\n");
        return 1;
    }

	// init 
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons ((short)atoi (argv[1]));
	serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	memset ((char *)serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero));  
	addr_size = sizeof (serverStorage);

	// create socket
	if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf ("socket error\n");
		return 1;
	}

	// bind
	if (bind (sock, (struct sockaddr *)&serverAddr, sizeof (serverAddr)) != 0)
	{
		printf ("bind error\n");
		return 1;
	}

	PACKET *pkt = malloc(sizeof(PACKET));
	int temp = 0;
	int status = 1;
	while(temp < 1)	{
		recvfrom(sock, pkt, sizeof(PACKET), 0, (struct sockaddr *)&serverStorage, &addr_size);
		nBytes = pkt->header.checksum;
		pkt->header.checksum = 0;
		pkt->header.checksum = calc_checksum(pkt, sizeof(HEADER)+pkt->header.length);
		if(pkt->header.seq_ack != 0 || pkt->header.checksum != nBytes){
			pkt->header.seq_ack = !status;
			sendto(sock, pkt, sizeof(PACKET), 0, (struct sockaddr *)&serverStorage, addr_size);
			++temp;
		} else {
			sendto(sock, pkt, sizeof(PACKET), 0, (struct sockaddr *)&serverStorage, addr_size);
			temp = 10;
			break;
		}
	}
	
	if(temp != 10)
		return 0;
	
	FILE *dest = fopen(pkt->data, "wb");

	int random = 0;
	
	while(1) {
		recvfrom(sock, pkt, sizeof(PACKET), 0, (struct sockaddr *)&serverStorage, &addr_size);
		if(random = rand()%10 < 3)
			continue;
		if(pkt->header.length == 0)
			break;
		nBytes = pkt->header.checksum;
		pkt->header.checksum = 0;
		pkt->header.checksum = calc_checksum(pkt, sizeof(HEADER) + pkt->header.length);
		if(pkt->header.seq_ack != status || pkt->header.checksum != nBytes) {
			pkt->header.seq_ack = !(pkt->header.seq_ack);
		} else {
			fwrite(pkt->data, 1, pkt->header.length, dest);
			status = status == 0 ? 1:0;
		}
		sendto(sock, pkt, sizeof(PACKET), 0, (struct sockaddr *)&serverStorage, addr_size);
	}
	fclose(dest);
	
	return 0;
}
