#include "helper.h"

#include <stdio.h>

#define CACHESIZE_IN_MB 15

void dump_memory(char* data, size_t len)
{
	size_t i;
	//	printf("Data in [%p..%p): ",data,data+len);
	for (i=0;i<len;i++)
		printf("%c", data[i] );
	printf("\n");
}

void clear()
{
	int *dummy_array = new int [1024*1024*CACHESIZE_IN_MB ];
	int *dummy_array2 = new int [1024*1024*CACHESIZE_IN_MB ] ;
	int sum =0;
	
	for ( int address = 0; address < 1024*1024*CACHESIZE_IN_MB; address++)
	{
		dummy_array [ address ] = address +1;
	}
	for ( int address = 0; address < 1024*1024*CACHESIZE_IN_MB; address++)
	{
		dummy_array2 [ address ] = address +1;
	}
	for(int repetition = 0; repetition < 3; repetition++)
	{
		for ( int address = 0; address < 1024*1024*CACHESIZE_IN_MB; address++)
		{
			sum += dummy_array[address];
		}
	}
	delete dummy_array;
	delete dummy_array2;
}
