#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int write_duration(char *fileName, int size, char threads, int x, ...);

int write_duration(char *fileName, int size, char threads, int x, ...)
{
	FILE *fp;
	va_list varg;
	int i;
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

