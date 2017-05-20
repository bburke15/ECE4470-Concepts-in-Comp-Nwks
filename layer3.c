#include "lab1.h"

/* for (call by) transmit function...
layer3 packetizes contents of data buffer passed to it by layer4. First,
dynamically creates transmission buffer (large enough to hold all headers and trailers
that will be added by layer3 and layer2; alloc enough space for at least 40 frames from layer2).

If layer4 data exceeds MSS, break up into MSS chunks = segments. Global constant MSS = 60.

For each segment, prepend 40 byte header filler with ASCII char '0' in every byte -> L3PDUs.

L3PDUs stored sequentially in the transmission buffer.

Non-terminal L3PDUs are 100 bytes. The terminal L3PDU may be smaller.
*/


struct ret_help layer3 (char* data_buffer, int data_count, char type, char * source_ip_pt, char * dest_ip_pt, int source_ip_size, int dest_ip_size) 
//passed data_buffer, returns pointer to transmission buffer

{
	//MSS constant defined in included header file
	lay_ret tbuff; //struct object for trans buffer and byte count
	
	//dynamically create transmission buffer 
	tbuff.ptr = malloc(4800); /*large enough such that can accomodate headers and 
	trailers to be added by layers 2 and 3 - 40 frames * 120 bytes/frame = 4800 bytes (max scenario)*/

	//Var for null zero needed for Lab 3 source and destination field padding
	char Pad[1] = {'\0'};
	char *pad_pt = Pad; //pointer to Pad

	//check if sizes (in bytes) of source and destination ips are less than 15 bytes; if so, pad null '\0' until source and destination ips are both 15 bytes...
	
	//check source ip...
	int pad_leftover_src = 15 - source_ip_size;
	if(pad_leftover_src != 0){ //if there are fewer than 15 bytes in the IP
		int e;		
		for(e=0;e<pad_leftover_src;e++){ //pad null zero however many times until hit 15 bytes
			memcpy(source_ip_pt + source_ip_size, pad_pt, 1);
			source_ip_size++; //increment size in bytes of source ip (should be 15 by end of loop)
		}
	}

	//check dest ip...
	int pad_leftover_dest = 15 - dest_ip_size;
	if(pad_leftover_dest != 0){ //if there are fewer than 15 bytes in the IP...
		int a;		
		for(a=0;a<pad_leftover_dest;a++){ //pad null zero however many times until hit 15 bytes
			memcpy(dest_ip_pt + dest_ip_size, pad_pt, 1);
			dest_ip_size++; //increment size in bytes of dest ip (should be 15 by end of loop)
		}
	}

	if(type == 't'){
		//called by transmit function

		//create header to be added to every chunk
		char ASCII_ZERO[40];
		int j;
		for(j=0; j< sizeof(ASCII_ZERO); j++){
			ASCII_ZERO[j] = (char)'0';
		}
		char *header_lay3 = ASCII_ZERO; //pointer to header array
		
		//Kulk: could also just use memset...

		//Lab 3 L3PDU sub fields...

		//Version...
		char Version[1] = {(char)'1'};
		char *Version_pt = Version; //pointer to ACK

		//Reserved...
		char Reserved[9];
		int q;
		for(q=0;q<sizeof(Reserved);q++){
			Reserved[q] = (char)'0';
		}
		char *reserved_lay3 = Reserved;

		//if data received from layer 4 exceeds MSS, segment into MSS byte chunks called segments...
		if(data_count > MSS){
			
			//find number of segments (chunks)...if number of bytes is not int multiple of 60, then there's a leftover segment !=60 byt, so +1 on # chunks 
			if( (data_count % MSS) != 0){
				int num_chunks = (data_count/MSS) + 1;

				//fill up transmission buffer to the second to last chunk
				int i;
				for(i=0; i< num_chunks - 1 ; ++i){ 
					//memcpy(tbuff.ptr + 40*i + 60*i, header_lay3, 40); (Labs 1/2)
					//for Lab 3 40 byte header consists of the following...
					memcpy(tbuff.ptr + 40*i + 60*i, Version_pt, 1); //copy Version byte over
					memcpy(tbuff.ptr + 40*i + 60*i + 1, reserved_lay3, 9); //copy Reserved sub field over
					memcpy(tbuff.ptr + 40*i + 60*i + 10, source_ip_pt, 15); //copy source ip over
					memcpy(tbuff.ptr + 40*i + 60*i + 25, dest_ip_pt, 15); //copy dest ip over
					
					memcpy(tbuff.ptr + 40*(i+1) + MSS*i, data_buffer + MSS*i, MSS); //copy data segm over
				}
				//that just leaves the last chunk (and the <MSS bytes it contains) to handle...
				//i set to beginnging of last frame already...CHECK
				i= (num_chunks - 1);
				int leftover_bytes = data_count % MSS;
				//memcpy(tbuff.ptr + 40*i + 60*i, header_lay3, 40); //(Labs 1/2)
				//for Lab 3 40 byte header consists of the following...
				memcpy(tbuff.ptr + 40*i + 60*i, Version_pt, 1); //copy Version byte over
				memcpy(tbuff.ptr + 40*i + 60*i + 1, reserved_lay3, 9); //copy Reserved sub field over
				memcpy(tbuff.ptr + 40*i + 60*i + 10, source_ip_pt, 15); //copy source ip over
				memcpy(tbuff.ptr + 40*i + 60*i + 25, dest_ip_pt, 15); //copy dest ip over

				memcpy(tbuff.ptr + 40*(i+1) + MSS*i, data_buffer + MSS*i, leftover_bytes); //fill in the leftover bytes for the last frame
				//size (bytes) of buffer is going to be same as data_buffer + the bytes of all the headers (num_chunks*40)
				//tbuff.count = data_count + 40*num_chunks;
				tbuff.count = data_count; //still a count just of the total data segment bytes
				tbuff.count_full = 40*num_chunks + data_count; //count of all bytes, including headers
			}
			else if(  (data_count % MSS) == 0){
				int num_chunks = data_count/MSS;
				//fill up transmission buffer fully
				int i;				
				for(i=0; i< num_chunks; ++i){
					//memcpy(tbuff.ptr + 40*i + 60*i, header_lay3, 40); (Labs 1/2)
					//for Lab 3 40 byte header consists of the following...
					memcpy(tbuff.ptr + 40*i + 60*i, Version_pt, 1); //copy Version byte over
					memcpy(tbuff.ptr + 40*i + 60*i + 1, reserved_lay3, 9); //copy Reserved sub field over
					memcpy(tbuff.ptr + 40*i + 60*i + 10, source_ip_pt, 15); //copy source ip over
					memcpy(tbuff.ptr + 40*i + 60*i + 25, dest_ip_pt, 15); //copy dest ip over
					
					memcpy(tbuff.ptr + 40*(i+1) + MSS*i, data_buffer + MSS*i, MSS); //copy data segm over
				}
				//size (bytes) of buffer is going to be same as data_buffer + the bytes of all the headers (num_chunks*40)
				//tbuff.count = data_count + 40*num_chunks;
				tbuff.count = data_count; //still a count just of the total data segment bytes
				tbuff.count_full = 40*num_chunks + data_count; //count of all bytes, including headers
			} 			
		

		}
		else{ //if data received is less than MSS (max segment size), then no segmentation needed. Just fill trans buffer with data_count bytes.
			//memcpy(tbuff.ptr, header_lay3, 40); (Labs 1/2)
			//for Lab 3 40 byte header consists of the following...
			memcpy(tbuff.ptr, Version_pt, 1); //copy Version byte over
			memcpy(tbuff.ptr + 1, reserved_lay3, 9); //copy Reserved sub field over
			memcpy(tbuff.ptr + 10, source_ip_pt, 15); //copy source ip over
			memcpy(tbuff.ptr + 25, dest_ip_pt, 15); //copy dest ip over
			
			memcpy(tbuff.ptr + 40, data_buffer, data_count); //copy data segm over
			//size (bytes) of buffer is going to be same as data_buffer + the bytes of the one header
			tbuff.count = data_count; //still a count just of the total data segment bytes
			tbuff.count_full = 40 + data_count; //count of all bytes, including header
		}
			

		//return transmission buffer ptr (object)
 		return (tbuff);		
	}
	else if(type == 'r'){
		//called by receive function
		//remove headers, leaving just the data segments
		lay_ret recep_buff2;
		recep_buff2.ptr = malloc(2049); //new buffer to put in only what need
		
		
		if(data_count > 100){ //more than one chunk's worth of data
			
			//find number of segments (chunks)...if number of bytes is not int multiple of 60, then there's a leftover segment !=60 byt, so +1 on # chunks 
			if( (data_count % 100) != 0){
				int num_chunks = (data_count/100) + 1;

				//fill up transmission buffer to the second to last chunk
				int i;
				for(i=0; i< num_chunks - 1 ; ++i){
					memcpy(recep_buff2.ptr + 60*i, data_buffer + 100*i+ 40, 60); //all data segments except last one are of size 60
				}
				//that just leaves the last chunk (and the <MSS bytes it contains) to handle...
				//i set to beginnging of last frame already...CHECK
				i= (num_chunks - 1);
				int leftover_bytes = (data_count % 100) - 40; //bytes in last chunk minus the header

				memcpy(recep_buff2.ptr + 60*i, data_buffer + 100*i + 40, leftover_bytes); //copy last data segm in								

				recep_buff2.count_full = data_count - 40*num_chunks; //all that's left are data segments
			}
			else if(  (data_count % 100) == 0){ //last frame has data segm size MSS
				int num_chunks = data_count/100;
				//fill up transmission buffer fully
				int i;				
				for(i=0; i< num_chunks; ++i){
					memcpy(recep_buff2.ptr + 60*i, data_buffer + 100*i+ 40, 60); //all data segments are of size 60
				}
				recep_buff2.count_full = data_count - 40*num_chunks; //all that's left are data segments
			} 			
		

		}
		else{ //if data received is less than MSS (max segment size), then no segmentation needed.
			int data_bytes = data_count - 40; //just the number of bytes in data segm
			//printf("# of bytes: %d\n",data_bytes);
			//printf("\nReceived frames processed by layer3:\n");
			//int a;
			/*for(a=0; a<data_count; a++){		
				printf("%c", *(data_buffer + a));		
			}*/		
			//memcpy(recep_buff2.ptr, data_buffer, 40);
			memcpy(recep_buff2.ptr, data_buffer + 40, data_bytes);
			recep_buff2.count_full = data_bytes; //all that's left is data segment
		}	

		return (recep_buff2);
	}

	
}
