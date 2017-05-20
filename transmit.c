#include "lab1.h"

/* Outline of transmit function is shown below */


void transmit (char * source_ip_pt, char * dest_ip_pt, int source_ip_size, int dest_ip_size) 

{
	/* Call your layer 4 function (Application) */
	lay_ret dbuff; //struct obj to catch data_count (bytes) and data_buffer pointer returned from layer4

	//char *db_ptr = layer4('t'); //catch data buffer pointer returned by layer4//old try
	char * a = NULL; //dummy NULL ptr; param not needed for this invocation of layer4
	dbuff = layer4(a, 0, 't'); //catch data buffer pointer and data count returned by layer4
	int k;	
	printf("Layer 4 contents of data buffer (the textfile content) to be transmitted: \n");		
	for(k=0; k<dbuff.count; k++){ //Data buffer contents for transmission	
		printf("%c", *(dbuff.ptr+ k));		
	}
 
	/* Call your layer 3 function */
	lay_ret tbuff; //struct obj to catch trans_buffer pointer returned by layer3

	//char *tb_ptr = layer3(db_ptr, 't'); //catch transmission buffer pointer returned by layer3//old try 
	tbuff = layer3(dbuff.ptr, dbuff.count, 't', source_ip_pt, dest_ip_pt, source_ip_size, dest_ip_size);
	int j;
	printf("\nContents of layer 3 transmission buffer to be transmitted: \n");	
	for(j=0; j<tbuff.count_full; j++){ //tbuff contents for transmission		
		printf("%c", *(tbuff.ptr+ j));		
	}

	/* Call your layer 2 function */
	lay_ret tbuff_2;
	tbuff_2 = layer2(tbuff.ptr, tbuff.count, 0, 't'); //catch transmission buffer (of sequentially placed L2PDUs) returned by layer2
	int z;
	printf("\nContents of layer 2 transmission buffer to be transmitted (after stuffing): \n");
	for(z=0; z<tbuff_2.count_stuffed_full; z++){ //tbuff_2 contents for transmission		
		printf("%c", *(tbuff_2.ptr+ z));		
	}
	

	/* Call Dr. Kulkarni's layer 1 function (see skeleton code below), once for every
	   frame. Therefore, if your layer 2 constructed three frames, call layer 1 thrice,
	   once per frame
	*/

	//NOTE: Number of frames will not change as a result of stuffing/destuffing
	//STRATEGY: CREATE AN ARRAY OF NUM_FRAME SIZE TO STORE EACH NUM_STUFFED COUNT (so that can pass correct nbytes param to layer 1)

	int	retcode;

	int tot_stuffed = 0; //to help with indexing into transmission buffer

	//# bytes for the first n-1 frames pre-stuffing
	int NT_bytes = 120; //pre-stuffing

	//# of bytes for the nth frame...
	if( (tbuff_2.count % MSS) == 0){ //# of bytes to transmit is int mult of 60, so (after additions of headers + trailers), last frame will have MSS size data segm pre-stuffing
		int T_bytes = 120; //pre-stuffing
		int total_frames = tbuff_2.count/MSS; //evenly divides, get int number of frames

		//invoke layer 1...

		int i;
		for (i= 0; i < total_frames; ++i) {

		/* Before call to layer 1, input parameters must be set as foll:
			txbuf is your char pointer to your transmission buffer
			nbytes is the no. of bytes in your layer 2 frame
			't' means transmit mode; DO NOT change this value.
			_DEBUG sets debug mode; valid values are 1 (true), and 0 (false)

			 Output parameter retcode reports the number of bytes successfully
			 transmitted over the PHY channel; there is no implied guarantee
			 that it was received by the receiver.
		*/

			//for debugging...
			int q;
			printf("\n# of stuffed chars per frame for layer 2 transmission buffer: \n");
			for(q=0; q<total_frames; q++){ //tbuff_2 contents for transmission		
				printf("%d ", tbuff_2.frame_stuffing[q]);		
			}
	
			char *txbuf = tbuff_2.ptr;
			int DEBUG=1; //change to turn off error messages
			//to advance pointer for each loop iteration, do txbuf + 120*i + however many stuffed characters there have been so far
			//each frame size will be the num_stuffed (stored in the corresp ith elem of the frame_stuffing array) plus the 120 bytes of the pre-stuffed frame
		
			retcode = layer1(txbuf + 120*i + tot_stuffed, tbuff_2.frame_stuffing[i] + T_bytes, 't', DEBUG); /* Function call to layer1..for the last frame */
			tot_stuffed += tbuff_2.frame_stuffing[i]; //update tot_stuffed value
			
		
			//NOTE: orig was ... retcode = layer1(txbuf + 120*i, nbytes, 't', _DEBUG);

			/* if retcode < nbytes, then Layer 1 transmitted fewer bytes than layer2 supplied
			so, display an error message and terminate the program
			otherwise, continue transmitting in the loop
			*/
		}
	} 
	
	else if( (tbuff_2.count % MSS) != 0){ //leftover bytes constitute last frame which will have data segm with fewer than MSS bytes
		int leftover_bytes = tbuff_2.count % MSS;
		int T_bytes = 60 + leftover_bytes; //pre_stuffing
		int total_frames = (tbuff_2.count/MSS) + 1; //leftover data segm portion != 60 byt, so +1 on int total_frames (to include last frame)
	
		//for debugging...
		int q;
		printf("\n# of stuffed chars per frame for layer 2 transmission buffer: \n");
		for(q=0; q<total_frames; q++){ //tbuff_2 contents for transmission		
			printf("%d ", tbuff_2.frame_stuffing[q]);		
		}
		
		//invoke layer1...
		int i;
		for (i= 0; i < total_frames; ++i) {

			/* Before call to layer 1, input parameters must be set as foll:
			txbuf is your char pointer to your transmission buffer
			nbytes is the no. of bytes in your layer 2 frame
			't' means transmit mode; DO NOT change this value.
			_DEBUG sets debug mode; valid values are 1 (true), and 0 (false)

			 Output parameter retcode reports the number of bytes successfully
			 transmitted over the PHY channel; there is no implied guarantee
			 that it was received by the receiver.
			*/
			char *txbuf = tbuff_2.ptr;
			int DEBUG=1; //change to turn off error messages
			//to advance pointer for each loop iteration, do txbuf + 120*i + however many stuffed chars there have been to tht point
			if(i==(total_frames - 1)){
				retcode = layer1(txbuf + 120*i + tot_stuffed, tbuff_2.frame_stuffing[i] + T_bytes, 't', DEBUG); /* Function call to layer1..for the last frame */
				tot_stuffed += tbuff_2.frame_stuffing[i]; //update tot_stuffed value
			}
			else if(i != (total_frames - 1)){
				retcode = layer1(txbuf + 120*i + tot_stuffed, tbuff_2.frame_stuffing[i] + NT_bytes, 't', DEBUG); /* Function call to layer1...for all but the last frame */
				tot_stuffed += tbuff_2.frame_stuffing[i]; //update tot_stuffed value
			}
		
			///NOTE: orig was ... retcode = layer1(txbuf + 120*i, nbytes, 't', _DEBUG);

			/* if retcode < nbytes, then Layer 1 transmitted fewer bytes than layer2 supplied
			so, display an error message and terminate the program
			otherwise, continue transmitting in the loop
			*/
		}
	}

	//DEALLOC...
	free(dbuff.ptr);
	free(tbuff.ptr);
	free(tbuff_2.ptr);
  return; 
}
