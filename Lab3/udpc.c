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
	char buffer[10];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	// argv[1]: port number
	// argv[2]: IP address
	// argv[3]: source
	// argv[4]: destination
	printf(argv[2]);
	if (argc != 5)
	{
		// printf ("Usage: %s <ip of server> \n",argv[0]);
		printf("Missing 1 or more arguments.\n");		
		return 1;
	}

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

	FILE *src = fopen(argv[3], "rb");
	if(src == NULL)
	{
		printf("Couldn't open file.\n");
		return 1;
	}

	PACKET *pkt = malloc(sizeof(PACKET));
	PACKET *response = malloc(sizeof(PACKET));
	
	// Sending destination name
	int temp = strlen(argv[4])+1;
	pkt->header.length = temp;
	pkt->header.seq_ack = 0;
	pkt->header.checksum = 0;
	strcpy(pkt->data, argv[4]);
	pkt->header.checksum = calc_checksum(pkt, temp+ sizeof(HEADER));
	sendto(sock, pkt, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);
	temp = 0;

	// Acknowledgement of destination name, resends a maximum amount of 3 times
	while(temp <= 2) {
		recvfrom(sock, response, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, &addr_size);
		if(response->header.seq_ack == 0)
			break;
		sendto(sock, pkt, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);
		++temp;
	}

	int status = 1;
	int random = 0;
	int read = 0;
	srand(time(0));
	
	while((read = fread(buffer, 1, 10, src)) > 0) {
		temp = 0;
		pkt->header.length = read;
		strcpy(pkt->data, buffer);
		pkt->header.seq_ack = status;
		pkt->header.checksum = 0;
		pkt->header.checksum = calc_checksum(pkt, pkt->header.length + sizeof(HEADER));
		
		while(temp <= 2) {
			random = rand()%100;		// Randomization of fake checksum
			if(random <= 10) {
				printf("Fake checksum.\n");
				pkt->header.seq_ack = !(pkt->header.seq_ack);
				pkt->header.seq_ack = status;
			} else if (random >= 97) {
				printf("Critical Failure.\n");
				pkt->header.checksum = 0;
			}
			sendto(sock, pkt, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);
			recvfrom(sock, response, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, &addr_size);
			if(pkt->header.seq_ack == status) { // If the server returns a correct ack
				printf("Successfully sent.\n");
				status = status == 0? 1:0;
				temp = 10;
				break;
			} else {
				printf("Send failure. Trying again...\n");
			}
			++temp;
		}
		
		if(temp != 10) {
			printf("Failed to connect with the server.\n");
			pkt->header.length = 0;
			sendto(sock, pkt, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);
			return 1;
		}
	}

	pkt->header.length = 0;
	memset(pkt->data, '\0', 1);
	sendto(sock, pkt, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);
	fclose(src);
	
	return 0;
}

	/* OLD FAKE NEWS WORK
	PACKET buffer;
	PACKET response;
	PACKET *buffer = &buffer;
	PACKET *response = &response;

	

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
