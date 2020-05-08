/*************************
 * COEN 146, UDP
 * 
 * Sened Haddad
 * Lab 5 - Thursday 2:15
 ************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

typdef struct {
	char name[50];
	char ip[50];
	int port;
} Machine;

int matrix[4][4];
//Machine linux_machines[4];
void *linkState(void *); // Thread Function
void *receiveInfo(void*); // Thread Function
/*void printTable(void)
{
	pthread_lock(&my_mutex);
	//
	pthread_unlock(&my_mutex);
}*/

int main(int, char **);

/***********
 *  main
 ***********/
int main (int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	// USE 1 & 2 TO  UPDATE COST TABLE MATRIX
	// argv[1]: This ID's machine
	// argv[2]: Total # of machines
	// argv[3]: Costs File
	// argv[4]: Machines file
	int num_mach = atoi(argv[2]);
	if (argc != 5)
	{
		printf("Missing 1 or more arguments.\n");		
		return 1;
	}

	// configure address
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (atoi (argv[1]));
	inet_pton (AF_INET, argv[2], &serverAddr.sin_addr.s_addr);
	memset (serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero));  
	addr_size = sizeof serverAddr;

	/*Create UDP socket*/
    if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf ("socket error\n");
		return 1;
	}

	// Putting machines file info into Hosts Table
	FILE *fpm = fopen(argv[4], "rb");
	if(fpm == NULL)
	{
		printf("Couldn't open file.\n");
		return 1;
	}

	char name[50];
	char ip[50];
	int port;
	int count = 0;

	// fscanf();	
	// Machine linux_machines[n];
	/*while(fscanf(fpm, "%*s %*s %d", name, ip, port) != EOF)
    {
        //linux_machines[count].name = name;
        //linux_machines[count].ip = ip;
        //linux_machines[count].port = port;
		count++;
    }*/

	// Initalize weight matrix by putting costs file info into matrix
	FILE *fpc = fopen(argv[3], "rb");
	if(fpc == NULL)
	{
		printf("Couldn't open file.\n");
		return 1;
	}

	int i, j;
	int num;
	for(i = 0; i < count; ++i)
	{	
		for(j = 0; j < count; ++j)
		{
			fscanf(fpc, "%d", num);
			matrix[i][j] = num;
		}
	}

	int q, x;
	for(q = 0; q < count; ++q){
		for(x = 0; x < count; ++x){
			printf("num %d", matrix[q][x]);
		}
	}

	/*void dij(int n, int N[], int cost[][],int dist[],int prev[])
	{
		int v;
		int w;
		int i;
		int source = 0;
		int visited[6];
		int closest;
		int isFull;
		int minimum;
		int node;

		for(v=0;v<4;v++)
		{
			dist[v]=cost[source][v];
			visited[v] = 0;
			prev[v] = 0;
		}
	 
		while(1){
			isFull = 1;
			minimum = 10000;

			for(i=1;i<4;i++){
				if(visited[i] == 0 && dist[i]<minimum){
					isFull = 0;
					minimum = dist[i];
					node = i;
				}
			}

			if(isFull){
				break;
			}

			visited[node] = 1;



			for(w=1;w<1;w++)
			{
				if(!visited[w] && cost[node][w] < 10000) {
					if(((dist[node]+cost[node][w]) < dist[w])) {
						dist[w] = dist[node]+cost[node][w];
						prev[w] = node;
					} else{
			//	prev[w] = w;
					}
				}
			}
		}
	}*/

	return 0;
}
