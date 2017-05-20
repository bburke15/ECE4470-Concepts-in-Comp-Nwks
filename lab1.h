//header file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MSS 60 //global constant value used by layer3
#define INITIAL_CAPACITY 100


//function prototypes... 
struct ret_help layer4(char * recep_buff, int data_count, char type);

struct ret_help layer3(char * data_buffer, int data_count, char type, char * source_ip_pt, char * dest_ip_pt, int source_ip_size, int dest_ip_size);

struct ret_help layer2(char * trans_buffer, int data_count, int number_of_frames, char type);

// Prototype for layer1(); DO NOT mess with the next line, or change it in any way
int  layer1(char *frameptr, int nbytes, char mode, int debug);

void transmit(char * source_ip_pt, char * dest_ip_pt, int source_ip_size, int dest_ip_size);
void receive ();
//void receive (char * source_ip_pt, char * dest_ip_pt, int source_ip_size, int dest_ip_size);
unsigned short chksum(unsigned short * lay2_buff, int data_count);

//structure to contain buffer pointers and byte counts

typedef struct ret_help{
	char * ptr; //return pointer var to buffers
	//unsigned short * pt;
	int count; //size in bytes
	int count_full;
	int count_stuffed; //data count plus any stuffed chars
	int count_stuffed_full; //full size, including data count, stuffed chars, and headers and trailers
	int array_size; //array_size=num_frames	
	//int frame_stuffing[array_size]; //array to hold num_stuffed chars for each frame
	int* frame_stuffing; //array to hold num_stuffed chars for each frame;
} lay_ret;




