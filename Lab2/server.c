 /**************************
 *     socket example, server
 *     Winter 2019
 ***************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>

int main (int, char *[]); 


/*********************
 * main
 *********************/
int main (int argc, char *argv[])
{
	int n;
	char *p; 
	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr; 
	char buff[10];

	// set up
	// sets allocated memory to 0 
	memset (&serv_addr, '0', sizeof (serv_addr));
	memset (buff, '0', sizeof (buff)); 

	// Sets values into struct like port number
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	serv_addr.sin_port = htons (atoi(argv[1])); 

	// create socket, bind, and listen 
	listenfd = socket (AF_INET, SOCK_STREAM, 0);
	bind (listenfd, (struct sockaddr*)&serv_addr, sizeof (serv_addr)); 
	listen (listenfd, 10); 

	// accept and interact
	while (1)
	{
		connfd = accept (listenfd, (struct sockaddr*)NULL, NULL); 
		// receive data and reply
		FILE *dest=NULL;
		while ((n = read (connfd, buff, sizeof (buff))) > 0)
		{
			//printf("%s", buff);
			if (dest == NULL)
			{
				dest = fopen(buff, "wb");
				memset (buff, '\0', sizeof (buff)); 
				write(connfd, buff, sizeof(buff)); 	// Sending ACK back
			}
			else
				fwrite(buff, 1, n, dest);
		}
		fclose(dest);
		close (connfd);
	}
	
	return 0;
}
