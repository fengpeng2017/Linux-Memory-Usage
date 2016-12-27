/*
 * =====================================================================================
 *
 *       Filename:  memory_usage.c
 *
 *    Description:  Prints memory usage in kB, MB and as percentages in each seconds
 *
 *        Version:  1.0
 *        Created:  27-12-2016 08:59:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SITKI BURAK CALIM (https://github.com/sbcalim), sburakcalim@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define D_MEMTOTAL 0
#define D_USED 0
#define D_MEMFREE 1
#define D_BUFFERS 2
#define D_CACHED 3

#define MAX_BUF 128

#define PROCMEMINFO "/proc/meminfo"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  memory_in_kb
 *  Description:  Calculates memory details (usage, free, cached, buffered) in kB
 * =====================================================================================
 */
unsigned long long *memory_in_kb()
{
	FILE *fp;
	unsigned long long *array;
	array = (unsigned long long *)malloc(4 * sizeof(unsigned long long));
	char buffers[5][MAX_BUF];
	unsigned long long total_kb;
	int i;

	fp = fopen(PROCMEMINFO, "r");
	
	//Each row of the array contains the values we will use in a minute
	fgets(buffers[D_MEMTOTAL], MAX_BUF, fp);
	fgets(buffers[D_MEMFREE], MAX_BUF, fp);
	fgets(buffers[4], MAX_BUF, fp);		//For MemAvailable part. We will not use it
	fgets(buffers[D_BUFFERS], MAX_BUF, fp);
	fgets(buffers[D_CACHED], MAX_BUF, fp);

	fclose(fp);

	//Take the values from buffered lines
	sscanf(buffers[D_MEMTOTAL], "MemTotal: %llu", &total_kb);
	sscanf(buffers[D_MEMFREE], "MemFree: %llu", &array[D_MEMFREE]);
	sscanf(buffers[D_BUFFERS], "Buffers: %llu", &array[D_BUFFERS]);
	sscanf(buffers[D_CACHED], "Cached: %llu", &array[D_CACHED]);

	array[D_USED] = total_kb - (array[D_MEMFREE] + array[D_BUFFERS] + array[D_CACHED]);

	//Now we got all 4 values we need in array[]
	return array;
}		/* -----  end of function memory_in_kb  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  memory_in_mb
 *  Description:  Gets results from memory_in_kb (usage, free, cached, buffered) and turn them into MB
 * =====================================================================================
 */
int *memory_in_mb()
{
	//We do not need to work for all these reading non-sense. memory_in_kb calculates them for us.
	int i;
	unsigned long long *array = memory_in_kb();
	int *ret_array;
	ret_array = (int *)malloc(4 * sizeof(int));

	//Although we just need to convert kB in MB by dividing with 1024
	for(i = 0; i<4; i++)
	{
		ret_array[i] = (array[i] / 1024);
	}

	return ret_array;
}		/* -----  end of function memory_in_mb  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  total_memory_kb
 *  Description:  Finds out what is total memory in kB
 * =====================================================================================
 */
unsigned long long total_memory_kb()
{
	char buffer[MAX_BUF];
	unsigned long long val;
	FILE *fp = fopen(PROCMEMINFO, "r");

	//Total memory is the first line of /proc/meminfo file, which starts with MemTotal
	fgets(buffer, MAX_BUF, fp);
	sscanf(buffer, "MemTotal: %llu", &val);

	return val;
}		/* -----  end of function memory_in_kb  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  total_memory_mb
 *  Description:  Converts total_memory_kb into MB
 * =====================================================================================
 */
int total_memory_mb()
{
	return (int)total_memory_kb()/1024;
}		/* -----  end of function total_memory_mb  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  memory_percentage
 *  Description:  Gets the same array from memory_in_kb() and calculates usage percetages over total memory
 * =====================================================================================
 */
double *memory_percentage()
{
	int i;
	unsigned long long *array = memory_in_kb();
	unsigned long long t = total_memory_kb();
	double *ret_array;
	ret_array = (double *)malloc(4 * sizeof(double));
	
	ret_array[D_MEMFREE] = ((double)array[D_MEMFREE] / (double)t) * 100;
	ret_array[D_CACHED] = ((double)array[D_CACHED] / (double)t) * 100;
	ret_array[D_BUFFERS] = ((double)array[D_BUFFERS] / (double)t) * 100;
	ret_array[D_USED] = 100 - (ret_array[D_MEMFREE] + ret_array[D_CACHED] + ret_array[D_BUFFERS]);

	return ret_array;
}		/* -----  end of function memory_percentage  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  Prints total memory (in kB and MB) once and memory_in_kb(), memory_in_mb(), memory_percentage() arraysfor every second 
 * =====================================================================================
 */
int main(int argc, char const *argv[])
{
	printf("\nTotal memory in kB:\t%llu\n", total_memory_kb());
	printf("Total memory in M:\t%d\n\n", total_memory_mb());

	printf("UsedK\tFreeK\tCacheK\tBufferK\t\tUsedM\tFreeM\tCacheM\tBufferM\t\tUsedP\tFreeP\tCacheP\tBufferP\n");
	
	while(1)
	{
		unsigned long long *array = memory_in_kb();
		int *array2 = memory_in_mb();
		double *array3 = memory_percentage();

		printf("%llu\t%llu\t%llu\t%llu\t\t", array[0], array[1], array[2], array[3]);
		printf("%d\t%d\t%d\t%d\t\t", array2[0], array2[1], array2[2], array2[3]);
		printf("%.2lf\t%.2lf\t%.2lf\t%.2lf\n", array3[0], array3[1], array3[2], array3[3]);

		free(array);
		free(array2);
		free(array3);
		
		sleep(1);
	}	
	return 0;
}		/* -----  end of function main  ----- */