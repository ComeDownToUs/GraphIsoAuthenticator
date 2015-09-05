#define VERTICES 		158
#define EDGES 			VERTICES/2
#define COMPRESS_SIZE 	((((VERTICES-1)*(VERTICES))/2)/8)+1 //(n*(n+1))/2, the +1 is because of rounding down
#define IND(a,b)		(a*VERTICES +b)
#define THEMAX			(4900*8) //as in mirsys(-150, 0) //the 150*8=1200
#define PRIME_SIZE		960
#define ROUNDS			THEMAX/8
 
typedef struct{
	char graph[COMPRESS_SIZE];
	int userID;
	char address[16]; //IP Address "127.0.0.1"
	char portNo[5];// '4','0','0','0','\0'
	char connected;
} Devices;

typedef struct{
	char graph[COMPRESS_SIZE];
	int userID;
	char permutation[VERTICES+1];
	char address[16]; //IP Address "127.0.0.1"
	char portNo[5];// '4','0','0','0','\0'
} YourInfo;

/**--- GLOBAL VARIABLES ---**/
/*** Protocol Globals ***/
char origin[COMPRESS_SIZE];
YourInfo yours;
Devices users[33];

/**---	FUNCTIONS 	---**/
/*** GMW ***/
void compress_graph(char *graff, char *graph);
void decompress_graph(char *graff, char *graph);
void get_permutator(char *perm);
void permGraph(char *perm, char *newGraph, char *g1, char *g2);
void print_graph(char *message, char *grr);
void print_perm(char *message, char *perm);
void prover_perm_gen(char *session, char *round, char *proof, char option);
int verify_graph(char *perm, char *compProof, char *compResult);
/*** Protocol Initialisation ***/
void read_user(int id);
void read_origin();
void read_devices();
int write_duration(char *fileName, int size, int threads, int x, ...);

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

//fisher-yates swap to create permutator
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
	num = rand()%2;
	perm[VERTICES] = (char)num;
	if(perm[VERTICES]<0)
		perm[VERTICES]*=-1;
}	

//applies permutation char[VERTICES] to graph char[VERTICES*VERTICES]
void permGraph(char *perm, char *newGraph, char *g1, char *g2){
	char *first;
	char i, j;
	char *second;
	
	first = malloc(sizeof(char) *(VERTICES*VERTICES));
	
	if(perm[VERTICES]==0)
		decompress_graph(g1, first);
	else
		decompress_graph(g2, first);
	
	second = calloc((VERTICES*VERTICES), sizeof(char));
		
	for(i=0; i<VERTICES; i++)
	{
		for(j=0; j<VERTICES; j++)
		{
			second[IND(i, j)]=first[IND(perm[i], perm[j])];
		}
	}
	//print_graph("Option     :", first);
	//print_perm("Permutation:", perm);
	//print_graph("Output :", second);
	//printf("\n-----------\n");
	free(first);
	compress_graph(newGraph, second);
}

//prints graph char[VERTICES*VERTICES]
void print_graph(char *message, char *grr){
	char i, j;
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

void print_perm(char *message, char *perm){
	char i=0;
	printf("%s ", message);
	for(i=0; i<(VERTICES+1); i++)
	{
		printf("%d ", perm[i]);
	}
	printf("\n");
}

//creates permutation needed to prove round
void prover_perm_gen(char *session, char *round, char *proof, char option){
	int i, j;
	char *inversePerm;
	
	if(option==1)//simple A->B->C
	{
		for(i=0; i<VERTICES; i++)
		{
			proof[i] = session[round[i]];
		}
	}
	else if(option==0)//needs B->A->C
	{
		inversePerm = (char*)calloc(VERTICES+1, sizeof(char));
		for(i=0; i<VERTICES; i++)
			inversePerm[session[i]]=i;
			
		for(j=0; j<VERTICES; j++)
			proof[j]=inversePerm[round[j]];
		free(inversePerm);
	}
	
	//print_perm("Generated Perm", proof);
}

//compares permutation of the selected graph to the ones in the loop
int verify_graph(char *perm, char *compProof, char *compResult)
{
	char *originalG, *requiredG, *guessG;
	char i, j;
	char *tester;
	
	///decompress graphs here
	//originalG only decompressed for printing requirements below, UNNECESSARY
/**/originalG = (char*) calloc((VERTICES*VERTICES), sizeof(char));
	decompress_graph(compProof, originalG);
	
	requiredG = (char*) calloc((VERTICES*VERTICES), sizeof(char));
	decompress_graph(compResult, requiredG);
	
	//printf("We're Verifying\n");
	
	//applying permutation to compProof
	tester = malloc(sizeof(char) *(COMPRESS_SIZE));
	permGraph(perm, tester, compProof, compProof);
	
	guessG = malloc(sizeof(char) *(VERTICES*VERTICES));
	
	decompress_graph(tester, guessG);
	
	free(tester);
	/*
	print_perm("The perm: ", perm);
	print_graph("The choice	: ", originalG);
	print_graph("Required	: ", requiredG);
	print_graph("Our Guess	: ", guessG);
	*/
	
	///free original here
	free(originalG);
	
	//puts("Here?");
	for(i=0; i<VERTICES; i++)
	{
		for(j=0; j<VERTICES; j++)
		{
			//printf("%c & %c |", tester[i], proofGraph[i]);
			if(guessG[IND(i, j)]!=requiredG[IND(i, j)])
			{
				printf("NO at %d %d\n", i, j);
				return -1;
			}
		}
	}
	//printf("They passed this round\n");
	free(guessG);
	return 0;
}

void read_user(int id)
{
	char filename[100];
	FILE *fp;
	
	sprintf(filename, "./users%d/User%d.bin", VERTICES, id);
	
	fp = fopen(filename, "rb");
	fread(&yours, sizeof(YourInfo), 1, fp);
	fclose(fp);
	
}

void read_origin()
{
	FILE *fp;
	char filename[100];
	
	sprintf(filename, "./graphFiles/%dv%de.bin", VERTICES, EDGES);
	
	fp = fopen(filename, "rb");
	fread(&origin, sizeof(char)*COMPRESS_SIZE, 1, fp);
	fclose(fp);
}

void read_devices()
{
	char filename[100];
	FILE *fp;
	int i;
	
	sprintf(filename, "./users%d/userInfo.bin", VERTICES);
	
	fp = fopen(filename, "rb");
	for(i=0; i<=32; i++)
	{
		fread(&users[i], sizeof(Devices), 1, fp);
		
	}
	fclose(fp);
}
//

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


	//add in extra arguments for system information
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

