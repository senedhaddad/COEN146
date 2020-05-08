/*************************
 * COEN 146, UDP
 * 
 * Sened Haddad
 * Lab 5 - Thursday 2:15
 ************************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>

typedef struct{
	char name[0];
	char ip [50];
	int port;
} Machine;

// Global variables and method declarations

Machine linux_machines[4];
int matrix[4][4];
int id;
void* linkState(void *);
void* receiveInfo(void*);
void printTable(void);
int sock;
struct sockaddr_in listen_addr;
socklen_t addr_size;

pthread_mutex_t lock;


/***********
 *  main
 ***********/

int main (int argc, char *argv[])
{	
	// argv[1]: This ID's machine
	// argv[2]: Total # of machines
	// argv[3]: Costs File
	// argv[4]: Machines file

	int i,j;
	id = atoi(argv[1]);
	int buffer[3];
	int newId;
	int weight;

	FILE *fp1 = NULL;
	FILE *fp2 = NULL;

	if(pthread_mutex_init(&lock, NULL) != 0)
	{
		printf("Mutex init failed \n");
		return 1;
	}

	// Open cost and machines files
	fp1 = fopen(argv[3], "rb"); 
	fp2 = fopen(argv[4], "rb");


	// Parse the data into the costs and machines matrices
	for( i = 0; i < 4; i++)
	{
		fscanf(fp2, "%s %s %d", linux_machines[i].name, linux_machines[i].ip, &linux_machines[i].port );
		for(j = 0; j < 4; j++)
		{
			fscanf(fp1, "%d", &matrix[i][j] );
		}
	}


	//init listenAddr
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_port = linux_machines[id].port;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset((char*)listen_addr.sin_zero, '\0', sizeof(listen_addr.sin_zero));
	addr_size = sizeof(listen_addr);

	//init 
	struct sockaddr_in destination_addr;
	destination_addr.sin_family  = AF_INET;

	
	if (argc != 5)
	{
		printf ("need the port number\n");
		return 1;
	}
	
	// Create socket
	if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf ("socket error\n");
		return 1;
	 }
	
	// Bind
	if (bind (sock, (struct sockaddr *)&listen_addr, sizeof (listen_addr)) != 0)
	{
		printf ("bind error\n");
		return 1;
	}
	
	// Close costs and machines files
	fclose(fp1);
	fclose(fp2);

	printTable();

	// Create 2 threads
	pthread_t thr2;
	pthread_t thr3; 

	pthread_create(&thr2, NULL, linkState, NULL);
	pthread_create(&thr3, NULL, receiveInfo, NULL);
	sleep(2);
	
	while(1){
	{

		printf("Enter an ID and weight to update: \n");
		scanf("%d %d", &newId, &weight);
		pthread_mutex_lock(&lock);
		matrix[id][newId]= weight;
		matrix[newId][id] = weight;
		pthread_mutex_unlock(&lock);
		printTable();

		
		for( i = 0; i < 4; i++)
		{
			if( i != id)
			{
			destination_addr.sin_port = linux_machines[i].port;
			inet_pton(AF_INET, linux_machines[i].ip, &destination_addr.sin_addr.s_addr);
			buffer[0] = id; 
			buffer[1] = newId;
			buffer[2] = weight;
			sendto(sock, buffer, sizeof(int)*3, 0, (struct sockaddr*)&destination_addr, addr_size);
			}
		}
		
		sleep(10);
		}
	}
	return 0;
}

// Methods

void* linkState(void *ptr){
	perror("Link State");

	bool visited[4];
	int leastDistanceArray[4];
	int  i, j, n, k;
	int sourceNode,count,closestN,nextnode, minCost=INT_MAX;
	
	while(1){
		pthread_mutex_lock(&lock);
		for(sourceNode=0;sourceNode<4;sourceNode++)
		{
			for(i= 0; i < 4; i++){
			visited[i] = false;
			leastDistanceArray[i]=matrix[sourceNode][i];
			}
			visited[sourceNode]=true;
			for(j=0; j < 4-1; j++){
				minCost = INT_MAX;
				for(count = 0; count < 4; count++)
				{
					if(!visited[count] && leastDistanceArray[count] <minCost){
						minCost = leastDistanceArray[count];
						closestN = count;
					}
				}
				visited[closestN] = true;
				for(k = 0; k < 4; k++)
				{
					if(!visited[k])
					{
						if(leastDistanceArray[k] < (leastDistanceArray[closestN]+matrix[closestN][k]))
						{
							leastDistanceArray[k]=leastDistanceArray[k];
						}
						else
						leastDistanceArray[k] = leastDistanceArray[closestN]+matrix[closestN][k];
					}
				}	

			}
			printf("Distance of node:\n");
			for(i=0; i < 4; i++){
				printf("%d ", leastDistanceArray[i]);
			}
			printf("\n");

		}

	   pthread_mutex_unlock(&lock);
   	   sleep(10);	   
	}
	return NULL;
}

void* receiveInfo(void *ptr){
	int  buffer[3];
	perror("Receive Info");
	while(1){
	recvfrom(sock, buffer, sizeof(int)*3, 0, NULL, NULL);
	pthread_mutex_lock(&lock);
	printf("Message recieved. \n");
	matrix[buffer[1]][buffer[0]] = buffer[2];
    matrix[buffer[0]][buffer[1]] = buffer[2];	
	pthread_mutex_unlock(&lock);
	printTable();
	}
}

void printTable(void){
	int i; 
	int j;
	pthread_mutex_lock(&lock);
	for(i = 0; i < 4; i++)
	{
		for(j=0; j < 4; j++)
		{	
			printf("%d ", matrix[i][j]);
		}	
		printf("\n");
	}
	printf("\n");
	pthread_mutex_unlock(&lock);
 return;
}

