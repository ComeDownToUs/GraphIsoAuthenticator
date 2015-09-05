#include <stdlib.h> //for malloc
#include <time.h>	//for time
#include <unistd.h> //for close
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <sys/time.h>
#include <stdlib.h>
#include <stdarg.h>
#include "gmw.h"//all of the graph stuff
#include "miracl.h"
#include "miracl_extensions.h"

int sendit(int sock, char *position, char *sendingC, int size);
int recit(int sock, char *position, char *recingC, int size);
int verify_client(int sock);
int verify_server(int sock);

big pubB, key;

int main(int argc, char *argv[])
{
	//the socket variables
	int sock;
    struct sockaddr_in server;
    char message[500], server_reply[500];
	struct timeval tv1, tv2;
	double first, second;
	
	
	gettimeofday(&tv1, NULL);
	
	mir_init(mip, byteA);
	mir_bin_to_array(byteA, arrA);
	
	gettimeofday(&tv2, NULL);
	first = (tv1.tv_sec) * 1000 + (tv1.tv_usec) / 1000 ;
	second = (tv2.tv_sec) * 1000 + (tv2.tv_usec) / 1000 ;//seconds and microseconds
	write_duration("MirInit.txt", VERTICES, 1, 3, first, second, (double)PRIME_SIZE);
	
	key = mirvar(0);
	pubB = mirvar(0);
	
	read_user(0);
	read_devices();
	read_origin();
	
	srand(time(NULL));
	
	sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
        return 1;
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
    puts("Connected\n");
    
    
    memset(server_reply, '\0', 500);
    if( recv(sock , server_reply , 500 , 0) >= 0)
		puts(server_reply);
		
	while(1)
	{
		if(!strcmp(server_reply, "Initialise"))
        {
			
			if(verify_client(sock)==0)
			{
				strcpy(message, "Client Message: You have passed!");
				send(sock, message, strlen(message), 0);
				bzero(server_reply, 500);
				recv(sock, server_reply, 500, 0);
				puts(server_reply);
				
				puts("PubB:");
				cotnum(pubB,stdout);
				puts("priA:");
				cotnum(priA,stdout);
				//puts("prime:");
				//cotnum(prime,stdout);
				
				/** THE KEY STUFF **/
				printf("Calculates Key: ");
				powmod(pubB, priA, prime, key);
				cotnum(key, stdout);
				
				break;
			}
			else
			{
				puts("Verification failed");
				break;
			}
		}
		else
		{
			printf("Server reply : %s\n", server_reply);
			break;
		}
    } 
    close(sock);
    return 0;
}

int verify_client(int sock){
	int i; 
	char valid;
	char *sendChoice, *recChoice;
	char *sendResp, *recResp;
	
	char *roundPerm, *yourProof;
	char *roundGraph;
	
	char *theirRound;
	char *theirProof;
	
	char arrB[THEMAX], *byteB;
	
	for(i=0; i<ROUNDS; i++)
	{
		roundPerm = malloc(sizeof(char)*(VERTICES+1));
		get_permutator(roundPerm);
		mir_embed(roundPerm, arrA, i, yours.graph, origin);
		
		roundGraph = malloc(sizeof(char)*(COMPRESS_SIZE));
		permGraph(roundPerm, roundGraph, yours.graph, origin);
		
		theirRound = malloc(sizeof(char)*(COMPRESS_SIZE));
		sendit(sock, "your round graph", roundGraph, sizeof(char)*COMPRESS_SIZE);
		recit(sock, "their round graph", theirRound, sizeof(char)*COMPRESS_SIZE);
		
		sendChoice = malloc(sizeof(char));
		recChoice  = malloc(sizeof(char));
		sendChoice[0] = rand()%2;
		
		free(roundGraph);
		
		write(sock, sendChoice, sizeof(char));
		read(sock, recChoice, sizeof(char));
		
		theirProof=malloc(sizeof(char)*(VERTICES+1));
		
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
		recit(sock, "their proof", theirProof, sizeof(char)*(VERTICES+1));
		
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
		sendit(sock, "Server Response", sendResp, strlen(sendResp));
		recit(sock, "Client Response", recResp, 500);
		
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
		
	}
	
	byteB=malloc(sizeof(char)*ROUNDS);
	mir_array_to_bin(byteB, arrB);
	bytes_to_big(ROUNDS, byteB, pubB);
	printf("Server's Public: ");
	cotnum(pubB, stdout);
	free(byteB);

	return 0;
}

int verify_server(int sock){

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
	
	if (read (sock, recingC, size) < 0 )
	{
		printf("FAIL: Receiving %s", position); 
		return 1;
	}
	
	return 0;
}

