/**************************
	Sened Haddad
	COEN146L Lab 3
	Thursday - 2:15pm
	CLIENT
 **************************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "tfv2.h"

/***********
 *  main
 ***********/
int main (int argc, char *argv[])
{
	int sock, portNum, nBytes;
	char data[10];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	int first = 1;
	int state = 0;
	int len;

	// argv[1]: port number
	// argv[2]: IP address
	// argv[3]: source
	// argv[4]: destination
	/*if (argc != 5)
	{
		printf ("Usage: %s <ip of server> \n",argv[0]);
		return 1;
	}*/

	// configure address
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (atoi (argv[1]));
	inet_pton (AF_INET, argv[2], &serverAddr.sin_addr.s_addr);
	memset (serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero));  
	addr_size = sizeof(serverAddr);

	/*Create UDP socket*/
	if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf ("socket error\n");
		return 1;
	}

	FILE *input = fopen(argv[3], "rb");

	/*PACKET pack;
	pack.header.seq_ack = 0;
	strcpy(pack.data, argv[4]);
	pack.header.checksum = calc_checksum(&pack, sizeof(pack.header) + strlen(argv[4])+1);
	pack.header.length = strlen(pack.data);*/

	PACKET *buffer = malloc(sizeof(PACKET));
	PACKET *response = malloc(sizeof(PACKET));
	/*PACKET buffer;
	PACKET response;
	PACKET *buffer = &buffer;
	PACKET *response = &response;*/

	

	do{
		if(first) {
			len = strlen(argv[4]) + 1;
			strcpy(buffer->data, argv[4]);
		}
		else {
			len = fread(data, sizeof(char), 10, input);
			printf("Len = %d\n", len);
			strcpy(buffer->data, data);
		}

		buffer->header.seq_ack = state;
		buffer->header.length = len;

		do{
			buffer->header.checksum = 0;
			buffer->header.checksum = calc_checksum(buffer, sizeof(PACKET));
			printf("Calculated checksum: %d\n", buffer->header.checksum);
			fprintf(stderr, "Here.");
			if(rand()%100 < 20) // Sending fake checksum 20% of the time
			{
				fprintf(stderr, "There.");
				buffer->header.checksum = 0;
				printf("Sent fake checksum.");
			}
			fprintf(stderr, "Everywhere.");
			sendto(sock, buffer, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);
			
			fprintf(stderr, "There.");
			recvfrom(sock, response, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, &addr_size);
			fprintf(stderr, "Here."); // Doesn't get here
		} while(response->header.seq_ack != state); // If wrong ACK, try again

		first = 0;
		printf("Correct ACK received.");
		state = (state+1)%2;
	
	} while(!feof(input));
 
	buffer->header.seq_ack = state;
	buffer->header.length = 0;
	buffer->header.checksum = 0;
	buffer->header.checksum = calc_checksum(buffer, sizeof(PACKET));
	sendto(sock, buffer, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);
/*
		while(recvfrom(sock, pack.data, strlen(pack.data), 0, NULL, NULL) > 0)
		{
			if(pack.header.seq_ack != state)	
			{
				if(count == 3)
					exit(EXIT_FAILURE);
				else
				{
					count++;
					// recompute checksum
					sendto(sock, pack.data, strlen(pack.data), 0, NULL, NULL);
					continue;
				}
			}
		
		}
	}*/

	return 0;
}
