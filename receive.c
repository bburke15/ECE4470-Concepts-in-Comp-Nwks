#include "lab1.h"

/* Outline of receive function is shown below */

void receive () //(char * source_ip_pt,char * dest_ip_pt, int source_ip_size, int dest_ip_size)

{
	/* Call Dr. Kulkarni's layer 1 function once per frame (see skeleton code below)
		Before call to layer 1, input parameters must be set as foll:
			rxbuf is your char pointer to your reception buffer in which
			 layer1 will store the incoming frame.
			The nbytes parameter will be ignored during reception
			'r' means receive mode; DO NOT change this value.
			_DEBUG sets debug mode; valid values are 1 (true), and 0 (false)

			 Output parameter retcode reports the number of bytes successfully
			 received over the PHY channel; a negative retcode value implies 
			 that there was a reception error
	*/
	//Set up input parameters...
	 
	//dynamically alloc reception buffer of char type, capable holding at least 40 full L2PDUS (4800)
	lay_ret rxbuf;
	rxbuf.ptr = malloc(7200); //for max case...receive 40 frames with 120 bytes/frame. 60 data byt/frame. If every character is stuffed, then (180 byt/frame)*40 frames = 7200 bytes

	rxbuf.count_stuffed_full = 0;

	
	int nbytes=0; //to be ignored...

	int DEBUG = 1; //turn off or on debugging messages from layer 1

	int Lframe = 0;

	int	retcode;

	char * temp = rxbuf.ptr;


	int number_of_frames=0;

	//while last frame has not been received...(Kulk provided that logic)
	while (Lframe != '1') { //check the LFRAME field of the received frame...field is at position 6 in each frame

		retcode = layer1(temp, nbytes, 'r', DEBUG); /* Function call to layer1 */
		//NOTE: orig...retcode = layer1(rxbuf, nbytes, 'r', _DEBUG); /*	
		if(retcode>0){		
			rxbuf.count_stuffed_full+=retcode;
			number_of_frames++;
		}

	
		Lframe = *(temp + 6);
		temp = temp + retcode; //each retcode represent a total frame length (bytes). since each frame is received indiv +6 bytes will be Lframe in every case

	
		/* if retcode < 0, then Layer 1 did not receive the frame
			so, display an error message and terminate the program
			otherwise, continue in the while loop till the last frame has
			been received
		*/
	}


	//printf("Retcode: %d \n", retcode);	
	printf("Received frames: \n");
	int m;
	for(m=0; m<rxbuf.count_stuffed_full; m++){		
		printf("%c", *(rxbuf.ptr+ m));		
	}

	/* Call your layer 2 function */
	lay_ret recep_buff;
	recep_buff = layer2(rxbuf.ptr, rxbuf.count_stuffed_full, number_of_frames, 'r');
	
	printf("\nReceived frames processed by layer2:\n");
	int h;
	for(h=0; h<recep_buff.count_full; h++){		
		printf("%c", *(recep_buff.ptr+ h));		
	}	

	/* Call your layer 3 function */
	lay_ret recep_buff2;
	//recep_buff2 = layer3(recep_buff.ptr, recep_buff.count_full, 'r', source_ip_pt, dest_ip_pt, source_ip_size, dest_ip_size);
	char dumb[1] = {'\0'};
	char *dumb_pt = dumb; //dummy NULL ptr; param not needed for this invocation of layer3
	recep_buff2 = layer3(recep_buff.ptr, recep_buff.count_full, 'r', dumb_pt, dumb_pt, 0, 0);
	printf("\nReceived frames processed by layer3:\n");
	int a;
	for(a=0; a<recep_buff2.count_full; a++){		
		printf("%c", *(recep_buff2.ptr+ a));		
	}

	/* Call your layer 4 function */
	layer4(recep_buff2.ptr, recep_buff2.count_full, 'r');

	//DEALLOC...
	free(rxbuf.ptr);
	free(recep_buff.ptr);
	free(recep_buff2.ptr);
  return;
}
