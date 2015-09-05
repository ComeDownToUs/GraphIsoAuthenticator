/*
    C socket server example, handles multiple clients using threads
*/
 
#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
 
#define VERTICES 		128
#define EDGES 			VERTICES/2
#define COMPRESS_SIZE 	((((VERTICES-1)*(VERTICES))/2)/8)+1 //(n*(n+1))/2, the +1 is because of rounding down
#define IND(a,b)		(a*VERTICES +b)
 
/*THE ZKP RELATED FUNCTIONS*/
//below two functions build a new graph to match above requirements
void build_graph();
int calc_edges(int iter, char *vert_org, char *graph_org);
//compresses a graph of ((n*n)*8 bits) to ( (((n*(n-1))/2)+1) bits)
void compress_graph(char *graff, char *graph);
//decompresses back into a graph
void decompress_graph(char *graff, char *graph);
//prints graph char[VERTICES*VERTICES]
void print_graph(char *message, char *grr);
void write_graph(char *graph, int vertices, int edges);
void read_graph(char *graph, int vertices, int edges);

/* Global variables*/
int iteration=0; //counts how many iterations of calc_edges graph takes to build
char graph1[VERTICES*VERTICES];//the initial graph

int main(int argc , char *argv[])
{

	char option;
	//Graph writes, reads;
	
    /*the graph stuff*/
	char readGraph[VERTICES*VERTICES];
    
    /*Create graph for verification */
    build_graph();
    print_graph("Write this graph? \n", graph1);
	option = getchar();
	
	if(option == 'y')
	{		
		write_graph(graph1, VERTICES, EDGES);
		read_graph(readGraph, VERTICES, EDGES);
		print_graph("The read graph", readGraph);
	}
	else
	{
		printf("oh... well, bye, I guess\n");
	}
	
    return 0;
}


void build_graph()
{
	char vertices[VERTICES];
	char graph[VERTICES*VERTICES];
	
	int i, j;
	char res;
	
	srand(time(NULL));
	
	iteration=0;
	
	for(i=0;i<VERTICES;i++)
	{
		for(j=0;j<VERTICES;j++)
		{
			graph[IND(i,j)]=0;
		}
	}
	printf("Array initialised\n");
	
	res=calc_edges(0, vertices, graph);
	if (res==-1){
		printf("CALCULATION FAILED\n");
	}
	
}

int calc_edges(int iter, char *vert_org, char *graph_org)
{
	int i, j, k, l, cnt, res;
	char vert[VERTICES];
	char graph[VERTICES*VERTICES];
	
	printf("Recursion depth: %d\n", iter);
	iteration++;
	
	if(iter==EDGES) //if finished
	{
		printf("Solution:\n");
		for(i=0;i<VERTICES;i++)
		{
			for(j=0;j<VERTICES;j++)
			{
				printf("%d ", graph_org[IND(i,j)]);
			}
			printf("\n");
		}
		printf("\n# of iterations: %d\n", iteration);
		//reassign variables to correct pointers
		//memcpy(vert_org, vert, sizeof(vert_org));
		memcpy(graph1, graph_org, sizeof(char)*VERTICES*VERTICES);
		//strcpy(temp, graph_org);
		return 0;
	}
	
	do{
		/*creating local copies of the graph and vertices variables so we
		  can backtrack in the event of there being no/limited options */
		memcpy(vert, vert_org, sizeof(char)*VERTICES);
		memcpy(graph, graph_org, sizeof(char)*VERTICES*VERTICES);
		
		for(k=0; k<VERTICES; k++)
		{
			if(vert[k]==iter)
			{
				cnt=0;
				/*get a random index, if not found within a specific range of time, give up and go back up one level*/
				do
				{
					if(cnt++ > (VERTICES*VERTICES))
					{
						return -1;
					}
					
					l=rand();
					l%=VERTICES;
				/* Get another value for l if:			*
				 * 1. vertice is on itself				*
				 * 2. vertice already has enough edges	*
				 * 3. the edge already exists			*/
				}while((l==k)||vert[l]>iter||graph[IND(k,l)]==1);	
				/*Mark vertice between k and l*/
				graph[IND(k,l)]=1;
				graph[IND(l,k)]=1;
				vert[k]++;
				vert[l]++;
			}
		}
		
		res = calc_edges(iter+1, vert, graph);
	}while(res==-1);
	return 0;
}

/* compresses graph for either long term storage or sending across limited bandwidth */
void compress_graph(char *graff, char *graph)
{
	/* using n(n+1)/2 to get number of values needed from graph that don't repeat
	 * dividing by 8 as we only need 1 bit per entry
	 * adding one extra entry to array to prevent integer rounding down*/
	//char array[((((VERTICES-1)*(VERTICES))/2)/8)+1];
	
	int h, i, j;
	int byteindex=0;
	int bitindex=0;
	
	for(h=0;h<COMPRESS_SIZE;h++)
	{
		graff[h]=0;
	}
	
	for(i=0; i<VERTICES; i++)
	{
		for(j=i+1;j<VERTICES; j++)
		{
			if(graph[IND(i,j)]==1)
			{
				graff[byteindex] |= (1<<bitindex);
			}
			if(++bitindex>7)
			{
				bitindex=0;
				byteindex++;
			}
		}
	}
}

/* breaks down received message to create a graph to analyse */
void decompress_graph(char *graff, char *graph)
{
	int byteindex=0;
	int bitindex=0;
	int i, j;
	
	for(i=0; i<VERTICES;i++)
	{
		for(j=i; j<VERTICES;j++)
		{
			if(j==i)
			{
				graph[IND(i,j)]=0;
			}
			else
			{
				graph[IND(i,j)]=(graff[byteindex]>>bitindex)&1;
				graph[IND(j,i)]=graph[IND(i,j)];
				
				if(++bitindex>7)
				{
					bitindex=0;
					byteindex++;
				}
			}
		}
	}
}

void print_graph(char *message, char *grr)
{
	int i, j;
	printf("\n%s\n", message);
	for(i=0;i<VERTICES;i++)
	{
		//if (i<10)  printf(" ");
		
		printf("%2d |", i);
		for(j=0;j<VERTICES; j++)
		{
			
			printf("%d ", grr[IND(i,j)]);
		}
		printf("\n");
	}
}


void write_graph(char *graph, int vertices, int edges){
	FILE *fp;
	char fileName[500];
	
	sprintf(fileName, "./graphFiles/%dv%de.bin", vertices, edges);
	
	fp = fopen(fileName, "wb");
	
	fwrite(graph, (sizeof(char)*(VERTICES*VERTICES)), 1, fp);
	
	fclose(fp);
}

void read_graph(char *graph, int vertices, int edges){
	FILE *fp;
	char fileName[500];
	
	sprintf(fileName, "./graphFiles/%dv%de.bin", vertices, edges);
	
	fp = fopen(fileName, "rb");
	
	fread(graph, (sizeof(char)*(VERTICES*VERTICES)), 1, fp);
	
	fclose(fp);
}
