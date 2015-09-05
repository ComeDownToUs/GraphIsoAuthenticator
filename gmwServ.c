/*
    C socket server example, handles multiple clients using threads
*/
 
#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //malloc
#include <time.h>		//time
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <sys/time.h>
#include <stdlib.h>
#include <stdarg.h>
#include "gmw.h"//all of the graph stuff
#include "miracl.h"
#include "miracl_ext_serv.h"

int verify_client(int sock);
int verify_server(int sock, big pb, char *ah);
void *connection_handler(void *socket_desc);
int sendit(int sock, char *position, char *sendingC, int size);
int recit(int sock, char *position, char *recingC, int size);

int connections = 0;

int main(int argc, char *argv[])
{
	/* Initialising socket stuff for servering */
	int socket_desc, client_sock, c, *new_sock;
	struct sockaddr_in server, client;
	
	mir_init(mip);
	
	read_user(0);
	read_devices();
	read_origin();
	
	srand(time(NULL));
	//creating server socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc == -1)  printf("could not create socket?!");
	puts("Socket created");
	
	//prepping the struct for server
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi(argv[1]));//specify which port on launch, changing this later on
	
	//bind socket
	if( bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("ERROR: Bind failed");
		return 1;
	}
	puts("bind done");
	
	listen(socket_desc, 3);
	
	//setup for accepting incoming connections
	puts("Waiting for incoming connection(s)...");
	c = sizeof(struct sockaddr_in);
	
	while ( (client_sock = accept(socket_desc,  (struct sockaddr *)&client, (socklen_t*)&c) ) )
	{
		puts("Connection accepted");
		
		//build thread
		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;
		
		if( pthread_create( &sniffer_thread, NULL, connection_handler, (void *) new_sock) < 0)
		{
			perror("Could not create thread?");
			return 1;
		}
		connections++;
		
		//unsure if needed, join thread, "so that we don't terminate before the thread completes its actions
		pthread_join(sniffer_thread, NULL);
		
		puts("Handler assigned");
	}
	
	if (client_sock < 0)
	{
		perror("accept failure");
		return 1;
	}

	return 0;
}

void *connection_handler(void *socket_desc)
{
	
	struct timeval tv1, tv2;
	double first, second;
	
	//get socket descriptor for this thread
	int sock = *(int*)socket_desc;
	int read_size, valid;
	char message[500], client_message[500];
	big pubB, key, priA;
	char byteA[ROUNDS], arrA[THEMAX];
	pubB=mirvar(0);
	key=mirvar(0);
	priA=mirvar(0);
	
	init_thre(priA, byteA);
	mir_bin_to_array(byteA, arrA);
	
	
    memset(message, '\0', 500);
	
	//send launch message to client
	strcpy(message, "Initialise");
	//sendit(sock, "Initial Message", message, NULL, strlen(message));
    write(sock , message , strlen(message));
	
	valid = verify_server(sock, pubB, arrA);//use pointer instead?
	
	if (valid == 0)
	{
		while( (read_size = recv(sock, client_message, 500, 0) > 0) )
		{
			puts(client_message);
			strcpy(message, "You have passed, well done, really great, yeah");
			send(sock, message, strlen(message), 0);
			bzero(client_message, 500);
			
			puts("PubB:");
			cotnum(pubB,stdout);
			puts("priA:");
			//cotnum(priA,stdout);
			//puts("prime:");
			//cotnum(prime,stdout);
			///KEY STUFF
			printf("Calulating the key: ");
			
			gettimeofday(&tv1, NULL);
			powmod(pubB, priA, prime, key);
			gettimeofday(&tv2, NULL);
			
			first = (tv1.tv_sec) * 1000 + (tv1.tv_usec) / 1000 ;
			second = (tv2.tv_sec) * 1000 + (tv2.tv_usec) / 1000 ;//seconds and microseconds
			write_duration("buildKey.txt", VERTICES, connections, 2, first, second);
			
			cotnum(key, stdout);
			
		}
		if(read_size == 0)
		{
			puts("Client has disconnected");
			fflush(stdout);
		}
		else if(read_size == -1)
		{
			perror("recv function failed");
		}		
	}
	else	puts("invalid client disconnected");
	
	connections--;
	
	free(socket_desc);
	
	return 0;
}

int verify_client(int sock)
{
	
	return 0;
}

int verify_server(int sock, big pb, char *ah)
{
	int i, threadss; 
	char valid;
	char *sendChoice, *recChoice;
	char *sendResp, *recResp;
	
	char *roundPerm, *yourProof;
	char *roundGraph;
	
	char *theirRound;
	char *theirProof;
	
	char arrB[THEMAX], *byteB;
	
	struct timeval tv1, tv2;
	double first, second;
	
	big testeryours;
	testeryours = mirvar(0);
	
	//memcpy(&testeryours, pb, sizeof(big));
	
	for(i=0; i<ROUNDS; i++)
	{	
		threadss = connections;
		gettimeofday(&tv1, NULL);

		roundPerm = malloc(sizeof(char)*(VERTICES+1));
		get_permutator(roundPerm);
		mir_embed(roundPerm, ah, i, yours.graph, origin);
		
		roundGraph = malloc(sizeof(char)*(COMPRESS_SIZE));
		permGraph(roundPerm, roundGraph, yours.graph, origin);
		
		theirRound = malloc(sizeof(char)*(COMPRESS_SIZE));
		recit(sock, "their round graph", theirRound, sizeof(char)*COMPRESS_SIZE);
		
		sendit(sock, "your round graph", roundGraph, sizeof(char)*COMPRESS_SIZE);
		
		sendChoice = malloc(sizeof(char));
		recChoice  = malloc(sizeof(char));
		sendChoice[0] = rand()%2;
		
		free(roundGraph);
		
		read(sock, recChoice, sizeof(char));
		
		write(sock, sendChoice, sizeof(char));
		
		theirProof=malloc(sizeof(char)*(VERTICES+1));
		
		recit(sock, "their proof", theirProof, sizeof(char)*(VERTICES+1));
		
		if(recChoice[0]==roundPerm[VERTICES])
		{
			//puts("*recChoice==roundPerm[VERTICES]");
			sendit(sock, "your proof", roundPerm, sizeof(char)*(VERTICES+1));
		}
		else
		{
			yourProof = malloc(sizeof(char)*(VERTICES+1));
			if(roundPerm[VERTICES] == 0 && recChoice[0] == 1) 
			{
				//Origin->Yours->round
				//need inverted session perm with round perm
				//puts("roundPerm[VERTICES] == 0 && *recChoice == 1");
				prover_perm_gen(yours.permutation, roundPerm, yourProof, 1);
				
			}
			else if(roundPerm[VERTICES]==1 && recChoice[0] == 0)
			{
				//Yours->Origin->Round
				//regular session perm with round perm
				//puts("roundPerm[VERTICES]==1 && *recChoice == 0");
				prover_perm_gen(yours.permutation, roundPerm, yourProof, 0);
			}
			else
			{
				printf("what the heck?!");
				return -1;
			}
			
			
			sendit(sock, "your proof", yourProof, sizeof(char)*(VERTICES+1));///yourProof is int* not char*
			free(yourProof);
			
		}
		free(roundPerm);
		free(recChoice);
		
		if(sendChoice[0]==0)
			valid = verify_graph(theirProof, users[0].graph, theirRound);
		else
			valid = verify_graph(theirProof, origin, theirRound);
		
		sendResp=malloc(sizeof(char)*500);
		if (valid==0){
			sprintf(sendResp, "Client success");
			mir_readembed(theirRound, arrB, i);
		}
		else
			sprintf(sendResp, "Client failure");
		
		free(sendChoice);
		free(theirRound);
		
		recResp=malloc(sizeof(char)*500);
		recit(sock, "Client Response", recResp, 500);
		sendit(sock, "Server Response", sendResp, strlen(sendResp));
		
		free(sendResp);
		if(valid==-1 || !strcmp(recResp, "Server failure"))
		{
			free(recResp);
			return -1;
		}
		else
		{
			free(recResp);
		}	
		
		
		gettimeofday(&tv2, NULL);
		
		first = (tv1.tv_sec) * 1000 + (tv1.tv_usec) / 1000 ;
		second = (tv2.tv_sec) * 1000 + (tv2.tv_usec) / 1000 ;//seconds and microseconds
		write_duration("roundSpeed.txt", VERTICES, threadss, 2, first, second);
	}
	
	
	gettimeofday(&tv1, NULL);
	
	byteB = malloc(sizeof(char)*ROUNDS);
	mir_array_to_bin(byteB, arrB);
	bytes_to_big(ROUNDS, byteB, pb);
	puts("Client's number: ");
	cotnum(pb, stdout);
	free(byteB);
	
	gettimeofday(&tv2, NULL);
	
	first = (tv1.tv_sec) * 1000 + (tv1.tv_usec) / 1000 ;
	second = (tv2.tv_sec) * 1000 + (tv2.tv_usec) / 1000 ;//seconds and microseconds
	write_duration("piecewise.txt", VERTICES, threadss, 2, first, second);

	return 0;
}

int sendit(int sock, char *position, char *sendingC, int size)
{
	if (write (sock, sendingC, size) < 0)
	{
		printf("FAIL: Sending %s", position); 
		return 1;
	}
	
	return 0;
}

int recit(int sock, char *position, char *recingC, int size)
{
	if (read (sock, recingC, size) <0 )
	{
		printf("FAIL: Receiving %s", position); 
		return 1;
	}
	
	return 0;
}
