/*
 * GraphInGraph.c
 * 
 * Copyright 2014 Padraig <padraig@padraig-Inspiron-1525>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread

#define VERTICES 		16
#define EDGES 			VERTICES/2
#define COMPRESS_SIZE 	((((VERTICES-1)*(VERTICES))/2)/8)+1 //(n*(n+1))/2, the +1 is because of rounding down
#define IND(a,b)		(a*VERTICES +b)


void write_graph(char *graph, int vertices, int edges);
void read_graph(char *graph, int vertices, int edges);
void print_graph(char *message, char *grr);

int main(int argc, char **argv)
{
	int largeSize = 158;
	int i, j, k, l;
	char largeGraph[largeSize*largeSize];
	char smallGraph[VERTICES*VERTICES];
	
	int startPoint;
	
	srand(time(NULL));
	
	startPoint = rand()%(158-VERTICES);
	printf("%d", startPoint);
	
	read_graph(largeGraph, largeSize, largeSize/2);
	//print_graph("blah", largeGraph);
	
	for(i=0; i<VERTICES; i++)
	{	
		k=startPoint+i;
		for(j=0; j<VERTICES; j++)
		{
			l=startPoint+j;
			memcpy(&smallGraph[IND(i,j)], &largeGraph[IND(k,l)], sizeof(char));
		}
	}
			
	print_graph("Here's the smaller one", smallGraph);
	
	return 0;
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
