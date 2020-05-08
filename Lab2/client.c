 /**************************
	socket example, client
	Winter 2019
 **************************/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int main (int, char *[]);


/********************
 *	main
 ********************/
int main (int argc, char *argv[])
{
	int i;
	int sockfd = 0, n = 0;
	char buff[10];
	char *p;
	struct sockaddr_in serv_addr; 

	// argv[1]: port number
	// argv[2]: IP address
	// argv[3]: source
	// argv[4]: destination
	if (argc != 5)
	{
		printf ("Usage: %s <ip of server> \n",argv[0]);
		return 1;
	}

	FILE *src = fopen(argv[3], "rp");
	if(src == NULL)
	{
		printf("Error\n");
		return 1;
	}
	FILE *dest = fopen(argv[4], "w");

	// set up
	memset (buff, '0', sizeof (buff));
	memset (&serv_addr, '0', sizeof (serv_addr)); 

	// open socket
	// initializing socket
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf ("Error : Could not create socket \n");
		return 1;
	} 

	// set address
	serv_addr.sin_family = AF_INET;
	// Sets port
	serv_addr.sin_port = htons (atoi(argv[1])); 

	if (inet_pton (AF_INET, argv[2], &serv_addr.sin_addr) <= 0)
	{
		printf ("inet_pton error occured\n");
		return 1;
	} 

	// connect
	// Waits for server to connect
	if (connect (sockfd, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0)
	{
		printf ("Error : Connect Failed \n");
		return 1;
	} 
	
	// Sending _file name
	write(sockfd, argv[4], strlen(argv[4])+1);

	// Receiving ACK: Validates message given from server side
	read(sockfd, buff, sizeof(buff));
	
	int bytes;
	int j;
	// Need to change this so that it's reading src
	// input, send to server, receive it back, and output it
	while(!feof(src)) {
		bytes = fread(buff, 1, 10, src);
		//printf("%s", buff);
		write(sockfd, buff, bytes);
	}
	
	/*while (scanf ("%s", buff) == 1)
	{
		write (sockfd, buff, strlen (buff) + 1);
		read (sockfd, buff, sizeof (buff));
		printf ("%s\n", buff);
	} */

	fclose(src);
	close (sockfd);

	return 0;
}
