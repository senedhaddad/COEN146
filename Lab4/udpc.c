/**************************
	Sened Haddad
	COEN146L Lab 4
	Thursday - 2:15pm
	CLIENT
 **************************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
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

	struct timeval tv; 	// timer
	int rv; 			// select returned value
	fd_set readfds;
	fcntl (sock, F_SETFL, O_NONBLOCK);

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
	
	// while(1)
	while((read = fread(buffer, 1, 10, src)) > 0) {
	
		// start before calling select
		FD_ZERO (&readfds);
		FD_SET (sock, &readfds);
		// set the timer
		tv.tv_sec = 10;
		tv.tv_usec = 0;

		temp = 0;
		pkt->header.length = read;
		strcpy(pkt->data, buffer);
		pkt->header.seq_ack = status;
		pkt->header.checksum = 0;
		pkt->header.checksum = calc_checksum(pkt, pkt->header.length + sizeof(HEADER));
		
		while(temp <= 2) {
		/*	random = rand()%100;
			if(random <= 10) {
				printf("Fake checksum.\n");
				pkt->header.seq_ack = !(pkt->header.seq_ack);
				pkt->header.seq_ack = status;
			} else if (random >= 97) {
				printf("Critical Failure.\n");
				pkt->header.checksum = 0;
			}*/

			sendto(sock, pkt, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);
			//recvfrom(sock, response, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, &addr_size);
						
			// call select
			rv = select (sock + 1, &readfds, NULL, NULL, &tv); // sock is the socket you are using
			if(rv == 0)						// Checking for any connection from server_addr
			{								// no data has been received so resend original data to server	
				//pkt->header.seq_ack = 0;	// set sequence number to 0 because we are resending the data
				pkt->header.seq_ack = !(pkt->header.seq_ack);
				pkt->header.seq_ack = status;				
				continue;
			} else if (rv == 1) {
				recvfrom(sock, response, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, &addr_size); // testing
			}
			

			if(pkt->header.seq_ack == status) { 	// If the server returns a correct ack
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
