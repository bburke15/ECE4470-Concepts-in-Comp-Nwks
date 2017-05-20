/*****************************************************************************
 * Brendan Burke							     *
 * main.c								     *
 * Copyright (C) Your Name 2017 <bburke15@villanova.edu>		     *
 * 									     *
 * This program generates a single executable for sender-receiver	     *
 * communication. 							     *
 *									     *
 *****************************************************************************/
/*****************************************************************************
 * COMPILATION INSTRUCTIONS (COMMAND LINE)						     *
 * =======================================				     *
 *									     *
 * To compile each .c src file (in gcc) without linking ..		     *
 *	In Linux/Unix: gcc  -c  file.c  -o  file.o			     *
 *	In Windows   : gcc  -c  file.c  -o  file.o			     *
 *									     *
 * To link all .o object files (in gcc) and create an executable	     *
 *	In Linux/Unix: gcc  file1.o  file2.o .. filen.o  -o  a.out	     *
 *	In Windows   : gcc  file1.o  file2.o .. filen.o  -lwsock32 -lws2_32  *
 *								-o  a.exe    *
 *****************************************************************************/
//For makefile... make -f makefile
//make clean will wipe out .o files completely. try from scratch.
//Editing: Specify .o files tht need on the line

#include <string.h>
#include <stdio.h>
#include "lab1.h"
//Kulk: every stage, look at how many chars in buffer. need to make sure bytes match up. otherwise, checksums fail.

//NOTE: Debugging performed in Anjuta
int main(int argc, char *argv[])
{	
	/* Check to see if the program was invoked with command-line correct parameters */
	if (argc != 2) {
		printf("\nERROR: Missing or incorrect number of program parameters\n");
		printf("USAGE: <program name> MODE\n");
		printf("where, MODE = 's' (Sender) or 'r' (Receiver)\n");
		return (1);
	}

	
	char mode = argv[1][0];	/* First character in argument MUST be 's' or 'r' */
	
	lay_ret source_ip;
	source_ip.ptr = malloc(15);

	lay_ret dest_ip;
	dest_ip.ptr = malloc(15);

	switch (mode) {
		
		case 's':
		case 'S': /* We are sending data from Application!    */
			  /* So, call your transmit() function here   */
			printf("\nEnter Sender's IP address: ");
			scanf("%s",source_ip.ptr);
			printf("\nEnter size of Sender IP address (in bytes): ");
			scanf("%d", &source_ip.count);
			
			printf("\nEnter Receiver's IP address: ");
			scanf("%s",dest_ip.ptr);
			printf("\nEnter size of Receiver IP address (in bytes): ");
			scanf("%d",&dest_ip.count);
		
			transmit(source_ip.ptr,dest_ip.ptr, source_ip.count, dest_ip.count);	

			  break;

		case 'r':
		case 'R': /* We are receiving Data from Application! */
			  /* So, call your receive() function here   */
			/*printf("\nEnter Sender's IP address: ");
			scanf("%s",source_ip.ptr);
			printf("\nEnter size of Sender IP address (in bytes): ");
			scanf("%d", &source_ip.count);
			printf("\nEnter Receiver's IP address: ");
			scanf("%s",dest_ip.ptr);
			printf("\nEnter size of Receiver IP address (in bytes): ");
			scanf("%d", &dest_ip.count);*/				
			receive();
			
			  break;
	}


	return 0;

}





