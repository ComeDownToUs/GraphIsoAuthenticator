#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdarg.h>

#define	VERTICES		192
#define	EDGES			VERTICES/2
#define	IND(a,b)		(a*VERTICES +b)
#define COMPRESS_SIZE	((((VERTICES-1)*(VERTICES))/2)/8)+1

int calc_edges(int iter, char *vert_org, char *graph_org);
void compress_graph(char *graff, char *graph);
void decompress_graph(char *graff, char *graph);
void write_graph(char *graph, int vertices, int edges);
void read_graph(char *graph, int vertices, int edges);
void print_graph(char *message, char *grr);
int write_duration(char *fileName, int size, int threads, int x, ...);

char graph1[COMPRESS_SIZE];
int iteration=0;
char *theGraph;
char *testRead;

int main()
{
	char vertices[VERTICES];
	char *graph;
	char option = 'n';
	struct timeval tv1, tv2;
	double first, second;
	int i;
	
	for(i=0; i<100; i++)
	{
		graph = (char*) calloc(VERTICES*VERTICES, sizeof(char));
		
		puts("?");
		
		gettimeofday(&tv1, NULL);
		calc_edges(iteration, vertices, graph);
		gettimeofday(&tv2, NULL);
		
		first = (tv1.tv_sec) * 1000 + (tv1.tv_usec) / 1000 ;
		second = (tv2.tv_sec) * 1000 + (tv2.tv_usec) / 1000 ;//seconds and microseconds
		write_duration("graphBuild.txt", VERTICES, 1, 2, first, second);
		
		
		
		gettimeofday(&tv1, NULL);
		compress_graph(graph1, graph);
		gettimeofday(&tv2, NULL);
		first = (tv1.tv_sec) * 1000 + (tv1.tv_usec) / 1000 ;
		second = (tv2.tv_sec) * 1000 + (tv2.tv_usec) / 1000 ;//seconds and microseconds
		write_duration("compress.txt", VERTICES, 1, 2, first, second);
		
		
		gettimeofday(&tv1, NULL);
		decompress_graph(graph1, graph);
		gettimeofday(&tv2, NULL);
		first = (tv1.tv_sec) * 1000 + (tv1.tv_usec) / 1000 ;
		second = (tv2.tv_sec) * 1000 + (tv2.tv_usec) / 1000 ;//seconds and microseconds
		write_duration("decompress.txt", VERTICES, 1, 2, first, second);
		
		free(graph);
		iteration=0;
	}
	
	
	printf("Write this graph? ");
	option = getchar();
	
	if(option == 'y')
	{
		write_graph(graph1, VERTICES, EDGES);
		
		testRead = malloc(sizeof(char)*COMPRESS_SIZE);
		read_graph(testRead, VERTICES, EDGES);
		
		theGraph = malloc(sizeof(char)*(VERTICES*VERTICES));
		decompress_graph(testRead, theGraph);
		free(testRead);
		print_graph("The read graph", theGraph);
		free(theGraph);
	}
	else
	{
		printf("oh... well, bye, I guess");
	}
	return 0;
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
		//memcpy(graph1, graph_org, sizeof(graph1));
		return 0;
	}
	
	do{
		/*creating local copies of the graph and vertices variables so we
		  can backtrack in the event of there being no/limited options */
		memcpy(vert, vert_org, sizeof(vert));
		memcpy(graph, graph_org, sizeof(graph));
		
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

//compresses a graph of ((n*n)*8 bits) to ( (((n*(n-1))/2)+1) bits)
void compress_graph(char *graff, char *graph){
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

//decompresses back into a graph
void decompress_graph(char *graff, char *graph){
	
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

void write_graph(char *graph, int vertices, int edges){
	FILE *fp;
	char fileName[500];
	
	sprintf(fileName, "./nopegraphFiles/%dv%de.bin", vertices, edges);
	
	fp = fopen(fileName, "wb");
	
	fwrite(&graph, (sizeof(char)*(COMPRESS_SIZE)), 1, fp);
	
	fclose(fp);
}

void read_graph(char *graph, int vertices, int edges){
	FILE *fp;
	char fileName[500];
	
	sprintf(fileName, "./nopegraphFiles/%dv%de.bin", vertices, edges);
	
	fp = fopen(fileName, "wb");
	
	fread(&graph, (sizeof(char)*(COMPRESS_SIZE)), 1, fp);
	
	fclose(fp);
}

void print_graph(char *message, char *grr){
	int i, j;
	printf("\n%s\n", message);
	
	for(i=0;i<VERTICES;i++)
	{
		printf("%2d |", i);
		for(j=0;j<VERTICES; j++)
		{
			
			printf("%d ", grr[IND(i,j)]);
		}
		printf("\n");
	}
}


int write_duration(char *fileName, int size, int threads, int x, ...)
{
	FILE *fp;
	va_list varg;
	int i;
	int processes, successes;
	double time[2];
	fp = fopen(fileName, "a");

	fprintf(fp, "%d,", size);
	fprintf(fp, "%d,", threads);
	
	va_start (varg, x); 

	for(i=0; i<x; i++)
	{
		time[i] = va_arg(varg, double);
		fprintf(fp, "%lf,", time[i]);
	}
	va_end(varg);
	
	fprintf(fp, "%lf\n", (time[1]-time[0]));
	
	fclose(fp);
	
	return 0;
}

