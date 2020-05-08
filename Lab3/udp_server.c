/**************************
	Sened Haddad
	COEN146L Lab 3
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
	PACKET *buffer = malloc(sizeof(PACKET));
	PACKET *response = malloc(sizeof(PACKET));
	struct sockaddr_in serverAddr, clientAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size, client_addr_size;
	char data[10];
	int first = 1;
	int state = 0;
	int len;
	int seq_ack;

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

	FILE *output;

	do{
		fprintf(stderr, "Hi");	
		nBytes = recvfrom(sock, buffer, sizeof(PACKET), 0, (struct sockaddr *)&serverStorage, &addr_size);
		fprintf(stderr, "Hi2");
		seq_ack = buffer->header.seq_ack;
		len = buffer->header.length;
		strcpy(data, buffer->data);
		fprintf(stderr, "Hi3");
	
		int pkt_checksum = buffer->header.checksum;
		buffer->header.checksum = 0;
		int cc = calc_checksum(buffer, sizeof(PACKET));

		printf("Packet checksum: %d\n", pkt_checksum);
		printf("Calculated checksum: %d\n", cc);

		if(cc != pkt_checksum)
		{
			response->header.seq_ack = (seq_ack+1)%2;
			printf("Checksum failed.\n");
			sendto(sock, response, sizeof(PACKET), 0, (struct sockaddr *)&serverStorage, addr_size);
			continue;
		}

		if(first)
		{
			first = 0;
			output = fopen(data, "wb");
		} else {
			fwrite(data, sizeof(char), len, output);
		}

		// ACK
		response->header.seq_ack = seq_ack;
		sendto(sock, response, sizeof(PACKET), 0, NULL, addr_size);

	}while(len > 0);

	fclose(output);

	return 0;
}
