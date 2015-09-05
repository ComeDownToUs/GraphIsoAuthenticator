#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VERTICES 		158
#define EDGES 			VERTICES/2
#define COMPRESS_SIZE 	((((VERTICES-1)*(VERTICES))/2)/8)+1 //(n*(n+1))/2, the +1 is because of rounding down
#define IND(a,b)		(a*VERTICES +b)
#define ROUNDS			2
#define COMPRESS_SIZE 	((((VERTICES-1)*(VERTICES))/2)/8)+1 //(n*(n+1))/2, the +1 is because of rounding down

typedef struct{
	char graph[COMPRESS_SIZE];
	int userID;
	char address[16]; //IP Address "127.0.0.1"
	char portNo[5];// '4','0','0','0','\0'
} User;

typedef struct{
	char graph[COMPRESS_SIZE];
	int userID;
	char permutation[VERTICES+1];
	char address[16]; //IP Address "127.0.0.1"
	char portNo[5];// '4','0','0','0','\0'
} YourInfo;	

void write_user(YourInfo *user, int id);
void read_graph(char *graph, int vertices, int edges);
void get_permutator(char *perm);
void permGraph(char *perm, char *newGraph, char *g1, char *g2);
void compress_graph(char *graff, char *graph);
void decompress_graph(char *graff, char *graph);
void print_graph(char *message, char *grr);
void print_perm(char *message, char *perm);

int main(int argc, char **argv)
{
	char original[VERTICES*VERTICES], userComp[COMPRESS_SIZE];
	int theGraph[VERTICES*VERTICES];
	char permutation[VERTICES+1];
	int i;
	char filename[100];
	char filename2[100];
	User users[33];
	YourInfo temp;
	FILE *fp;
	
	read_graph(original, VERTICES, EDGES);
	print_graph("The first graph", original);
	
	
	
	puts("hah");
	sprintf(filename, "./users%d/userInfo.bin", VERTICES);
	fp = fopen(filename, "wb");
	for(i=0; i<=32; i++)
	{
		get_permutator(permutation);
		permGraph(permutation, userComp, original, original);
		compress_graph(userComp, original);
		
		memcpy(users[i].graph, userComp, sizeof(char)*COMPRESS_SIZE);
		users[i].userID = i;
		strcpy(users[i].address, "127.0.0.1");
		sprintf(users[i].portNo, "%d", (4000+i));
		
		memcpy(temp.graph, userComp, sizeof(char)*COMPRESS_SIZE);
		temp.userID = i;
		memcpy(temp.permutation, permutation, sizeof(char)*(VERTICES+1));
		strcpy(temp.address, "127.0.0.1");
		sprintf(temp.portNo, "%d", (4000+i));
		
		fwrite(&users[i], sizeof(User), 1, fp);
		
		write_user(&temp, i);
	}
	fclose(fp);
	
	for(i=0; i<=32; i++)
	{
		sprintf(filename2, "./users%d/User%d.bin", VERTICES, i);
		fp = fopen(filename2, "rb");
		fread(&temp, sizeof(YourInfo), 1, fp);
		fclose(fp);
		print_perm(filename2, temp.permutation);
	}
	
	
	return 0;
}

void write_user(YourInfo *user, int id)
{
	FILE *fpoint;
	char filename[100];
	
	sprintf(filename, "./users%d/User%d.bin", VERTICES, id);
	
	fpoint = fopen(filename, "wb");
	fwrite(user, sizeof(YourInfo), 1, fpoint);
	fclose(fpoint);
	
	
}

void read_graph(char *graph, int vertices, int edges){
	FILE *fp;
	char fileName[500];
	
	sprintf(fileName, "./graphFiles/%dv%de.bin", vertices, edges);
	
	fp = fopen(fileName, "rb");
	
	fread(graph, (sizeof(char)*(VERTICES*VERTICES)), 1, fp);
	
	fclose(fp);
}

void get_permutator(char *perm){
	
	int i, num, temp;
	
	for(i=0;i<VERTICES;i++){
		perm[i]=i;
	}
	
	for(i=VERTICES-1; i>=0; i--){
		num=(rand())%(i+1);
		temp=perm[i];
		perm[i]=perm[num];
		perm[num]=temp;
	}
	perm[VERTICES]=rand()%2;
}

void permGraph(char *perm, char *newGraph, char *g1, char *g2){
	char first[VERTICES*VERTICES];
	int i, j;
	char second[VERTICES*VERTICES];
	
	//first = malloc(sizeof(int) *(VERTICES*VERTICES));
	
	if(perm[VERTICES]==0)
		decompress_graph(g1, first);
	else
		decompress_graph(g2, first);
	
	//second = malloc(sizeof(int) *(VERTICES*VERTICES));
		
	for(i=0; i<VERTICES; i++)
	{
		for(j=0; j<VERTICES; j++)
		{
			second[IND(i, j)]=first[IND(perm[i], perm[j])];
		}
	}
	//print_graph("Option     :", first);
	print_perm("Permutation:", perm);
	//print_graph("Output :", second);
	printf("\n-----------\n");
	
	//printf("are we here");
	//free(first);
	compress_graph(newGraph, second);
	//free(second);
	
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

void print_perm(char *message, char *perm)
{
	int i=0;
	printf("%s ", message);
	for(i=0; i<(VERTICES+1); i++)
	{
		printf("%d ", perm[i]);
	}
	printf("\n");
}

