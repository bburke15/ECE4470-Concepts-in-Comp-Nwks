#include "lab1.h"

/* for (call by) transmit function...
Layer2 attaches header as well as a trailer for each L3PDU -> forms frame = L2PDU.

Each header is 17 bytes with 6 fields (STX, Count, ACK, Seq #, LFrame, Resvd).
Each trailer is 3 bytes with 2 fields (Chksum, ETX).
Enter ASCII char '0' for each byte of the 'ACK', 'Resvd', and 'Chksum' fields.
The Seq# field must be correctly filled in with the sequence # of the frame ('0'..'9' in ASCII repr).
After '9', Seq# rolls over to '0'. 'Count' field must carry the # of bytes in the whole frame (L2PDU; if
have n frames, first n-1 frames have 'Count' field value of 120, repr as ASCII chars '1','2','0', and last frame have 'Count' <= 120).
'LFrame' indicates whether is last frame in transmission ('0' if no, '1' if yes). 

After constructing each L2PDU, L2PDUS place sequentially in transmission buffer.
Trans buffer passed back to transmit funct which invokes layer1 once for every frame.
*/


//LAB 2 NOTE: RIGHT NOW, for multiple frame scenarios, num_stuffed is measured on a frame-by-frame basis. May to change to make a measure of total stuffed throughout all frames...

struct ret_help layer2 (char* trans_buffer, int data_count, int number_of_frames, char type) //passed trans_buffer, returns pointer to updated transmission buffer (new headers + trailers)

{

	lay_ret tbuff_2; //struct object for updated trans buffer
	tbuff_2.ptr = malloc(4800);

	lay_ret stuffed_tbuff_2;
	stuffed_tbuff_2.ptr = malloc(7200); //for max case...40 frames with 120 bytes/frame. 60 data byt/frame. If every character is stuffed, then (180 byt/frame)*40 frames = 7200 bytes

	lay_ret chk_buff;
	chk_buff.ptr = malloc(7040); //max case is (40 frames * 180 byt/frame)-(4 uninlcud byts/frame * 40 frames) = 7040 bytes

	if(type == 't'){
		//called by transmit function
		//initialize fields that will always contain ASCII zeros (ACK, Resvd, Chksum)...

		/*LAB 2 MODIFICATION: on the transmit side, when layer2 receives the L3PDU(s) from layer 3 (trans_buffer), 
		it checks for accidental occurrences of STX, ETX, DLE in the data. If so, a DLE char is stuffed before the control character.
		Count field is then updated to reflect the new frame size after stuff operations. Count field may now exceed 120 bytes, and therefore must be adjusted. 
		Assume nwk accepts frame sizes up to 180 bytes, so no fragmentation is necessary after stuffing.
		
		Checksum function should be written to compute checksum of layer 2 frame over only those fields specfic to the L2PDU (layer 2 headers and trailers).
		Checksum is computed after character stuffing is done, and after the correct frame size has been inserted into the Count field
		For transmit side, layer 2 calls the Internet Checksum computation function (alg is in txtbk, but need to modify; call after stuffing is done) and stores 
		the result of the computation in the 2-byte checksum field, 1 digit per byte.
		Insert only the LSB of the checksum in the Chksum field of the trailer (ex. insert e8 of e7e8)*/ 		
		
		//Needed for stuffing...
		//DLE
		int DLE[1] = {16}; //CHECK if dec (what doing now) or hex value...
		int *DLE_pt = DLE;

		int total_stuffed = 0; //total number of stuffed characters added
		int num_stuffed = 0; //number of characters stuffed on a frame-by-frame basis. Will be used for updating the count.

		int stuffed_data_count = data_count; //initialize new stuffed data count to current data count...

		//STX
		//char STX[1] = {(char)'2'};
		//char *STX_pt = STX; //pointer to STX ...PUT AS DECIMAL 2 not CHAR
		int STX[1] = {2};
		int *STX_pt = STX;
		//ACK...
		char ACK[1] = {(char)'0'};
		char *ACK_pt = ACK; //pointer to ACK
		
		//Resvd...
		char Resvd[10];
		int j;
		for(j=0; j< sizeof(Resvd); j++){
			Resvd[j] = (char)'0';
		}
		char *Resvd_pt = Resvd; //pointer to Resvd
		
		//Chksum...
		char Chksum[2];
		int i;
		for(i=0; i< sizeof(Chksum); i++){
			Chksum[i] = (char)'0';
		}
		char *Chksum_pt = Chksum; //pointer to Chksum

		//Vars for padding for checksum function...
		char Pad[1] = {'\0'};
		char *pad_pt = Pad; //pointer to Pad

		//unsigned short Pad_2[1] = {'\0'};
		//unsigned short *pad_pt_2 = Pad_2; //pointer to Pad
		
		//ETX
		//char ETX[1] = {(char)'3'};
		//char *ETX_pt = ETX; //pointer to ETX
		int ETX[1] = {3};		
		int *ETX_pt = ETX; 
		//initialize count value which is the same for all non-terminal L2PDUs (120)
		char Count_nonterm[3] = {(char)'1', (char)'2', (char)'0'};
		char *Count_NT = Count_nonterm; //pointer to Count_nonterm
		int L2_size = 120;

		//initialize array containing possible sequence number values...
		char seq_num[10] = {(char)'0', (char)'1', (char)'2', (char)'3', (char)'4', (char)'5', (char)'6', (char)'7', (char)'8', (char)'9'};
		char *Seq_Num = seq_num; //pointer to seq_num

		if(data_count > MSS){ //multiple frames to deal with (add headers and trailers)			

			if( (data_count % MSS) != 0){ //last frame will have a count <120
				int num_frames = (data_count/MSS) + 1;
				
				//Declare array of size num_frames to store num_chars stuffed for each frame (to be used for transmitting the stuffed buffer)
				stuffed_tbuff_2.array_size = num_frames;
				stuffed_tbuff_2.frame_stuffing = (int*)malloc(sizeof(int) * stuffed_tbuff_2.array_size); //array to hold num_stuffed values				

				//fill up transmission buffer to the second to last chunk
				int chk_data_count_total = 0; //for checksum buffer
				int n=0; //index for seq_num array				
				int i;
				for(i=0; i< num_frames - 1 ; ++i){
					//Headers...
					
					memcpy(tbuff_2.ptr + L2_size*i, STX_pt, 1); //STX 
					//NOTE: FOR SINGLE BYTE CAN JUST COPY OVER
					
					memcpy(tbuff_2.ptr + L2_size*i + 1, Count_NT, 3); //Count
					
					memcpy(tbuff_2.ptr + L2_size*i + 4, ACK_pt, 1); //ACK
					
					//Seq#...
					if(n==10){ //After '9', the sequence rolls over to '0'
						n=0;
					}
					memcpy(tbuff_2.ptr + L2_size*i + 5, Seq_Num + n, 1);
					n++; //incrment n for the next number in sequence 
					
					//Lframe (since the last frame is being handled separately, Lframe is '0' for each for loop iteration...
					char Lframe[1] = {(char)'0'};
					char *Lframe_pt = Lframe;
					memcpy(tbuff_2.ptr + L2_size*i + 6, Lframe_pt, 1);
					
					memcpy(tbuff_2.ptr + L2_size*i + 7, Resvd_pt, 10); //Resvd

					//Copy L3PDU Over (layer3 header and data segment i at once)...
					memcpy(tbuff_2.ptr + L2_size*i + 17, trans_buffer + 100*i, 100);

					//Trailers...

					memcpy(tbuff_2.ptr + L2_size*i + 117, Chksum_pt, 2); //Chksum
					memcpy(tbuff_2.ptr + L2_size*i + 119, ETX_pt, 1); //ETX
					
					//Stuffing for the first n-1 (equal-sized) frames...
					
					//copy headers (count will change but rest is same) into buffer for the stuffed frames...
					total_stuffed += num_stuffed; //up to the current frame, this is the total # of bytes stuffed that will give correct indexing for the stuffed array...
					memcpy(stuffed_tbuff_2.ptr + L2_size*i + total_stuffed, tbuff_2.ptr + L2_size*i, 17); //first 57 bytes of each frame are headers to be copied over
					//now handle the actual stuffing...
					num_stuffed=0; //for updating the count...
					int k;		
					for(k=((i*L2_size)+17); k<(100 + ((i*L2_size)+17)); k++){ //for each byte of the L3PDU of each of the first n-1 frames...
						if( (*(tbuff_2.ptr + k) == 3) || (*(tbuff_2.ptr + k) == 2) || (*(tbuff_2.ptr + k) == 16)){ //if there's a control char (DLE, STX, or ETX)
							memcpy(stuffed_tbuff_2.ptr + k + num_stuffed + total_stuffed, DLE_pt, 1); //put a DLE before the control char
							num_stuffed++; //increment number of stuffed chars
							memcpy(stuffed_tbuff_2.ptr + k + num_stuffed + total_stuffed, tbuff_2.ptr + k, 1); //copy actual (control char) data byte over
						}
						else{ //data byte is not a control char
							memcpy(stuffed_tbuff_2.ptr + k + num_stuffed + total_stuffed, tbuff_2.ptr + k, 1); //just copy the data byte over
				
						}

					}
					stuffed_tbuff_2.frame_stuffing[i] = num_stuffed; //store frame specific num_stuffed value in array
					
					stuffed_data_count += num_stuffed; //add number stuffed for this frame to total data byte count
					int stuffed_count_frame = num_stuffed + 120; //new total byte count value for frame... (60 data byt and 60 H/T bytes to start, so just add num_stuffed)	
					
					//update count...		
					if(stuffed_count_frame >= 100){ //three places to turn into ASCII repr
						int hund = ((stuffed_count_frame/100)+ 48); 
						int tens = (((stuffed_count_frame - 100)/10) + 48);
						int ones = (((stuffed_count_frame - 100)%10) + 48);
						char stuffed_count_term_array[3] = {(char)hund, (char)tens, (char)ones};
						char *stuffed_count_term_array_pt = stuffed_count_term_array;
						memcpy(stuffed_tbuff_2.ptr + L2_size*i + 1 + total_stuffed, stuffed_count_term_array_pt, 3); //Count
					}
					else if(stuffed_count_frame < 100){
						int tens = ((stuffed_count_frame/10) + 48); 
						int ones = ((stuffed_count_frame % 10) + 48);
						char stuffed_count_term_array[3] = {(char)'0', (char)tens, (char)ones};
						char *stuffed_count_term_array_pt = stuffed_count_term_array;
						memcpy(stuffed_tbuff_2.ptr + L2_size*i + 1 + total_stuffed, stuffed_count_term_array_pt, 3); //Count
					}	
					//note that the count field is in the header, thus the stuff offset includes only the total_stuffed from previous frames, not the num_stuffed from this frame
					
					//Trailers...
			
					
					memcpy(stuffed_tbuff_2.ptr + L2_size*i + 117 + num_stuffed + total_stuffed, Chksum_pt, 2);

					//ETX
					memcpy(stuffed_tbuff_2.ptr + L2_size*i + 119 + num_stuffed + total_stuffed, ETX_pt, 1); //ETX
					
					//Call checksum function and populate the chksum field with this value... 

					//fill checksum buffer... 
					int chk_data_count = stuffed_count_frame - 4; //concerned with data_count not including STX, ETX, and Chksum fields (4 bytes altogether)
					//so, for each frame, the data_count will be the 120 + num_stuffed for that frame, subtract 4 cuz we're not considering STX,ETX, Chk fields 						
					
					memcpy(chk_buff.ptr + chk_data_count_total, stuffed_tbuff_2.ptr + i*L2_size + total_stuffed + 1, chk_data_count); //copy into chksum buffer
					unsigned short* lay2_buff = chk_buff.ptr + chk_data_count_total; //pointer to pass to chksum funct					
					chk_data_count_total += chk_data_count;			
				
					/*printf("\nchk_data_count is %d\n", chk_data_count);
					int y;
					printf("Checksum debugging: \n");
					for(y=0; y<chk_data_count; y++){
					printf("%c", *(lay2_buff + y));
					}*/

					//check for odd number of bytes so that string can be padded as needed...
					if(chk_data_count % 2){ //if there is a remainder, string has an odd number of bytes, so pad a '\0' so that is has an even number of bytes
						chk_data_count++; //chk_data_count now must include the padded '\0'
						chk_data_count_total++;
						memcpy(chk_buff.ptr + chk_data_count_total, pad_pt, 1);  //add the '\0'
					}
					//if the if condition does not apply, then there is an even number of bytes and nothing needs to be done

					//debugging...
					//printf("\nch_data_count is %d\n", chk_data_count);			
					/*printf("Checksum debugging after padding: \n");
					int m;
					for(m=0; m<chk_data_count; m++){
					printf("%c", *(lay2_buff + m));
					}*/
					 
					
					unsigned short checksum_pop = 0; //var to catch result of chksum function
					checksum_pop = chksum(lay2_buff, chk_data_count); //call chksum function
					//printf("\nChecksum: %u\n", checksum_pop);
			
					int chksum_start = checksum_pop%100;			
					//now populate the chksum field...
					int chk_tens = ((chksum_start/10)+48); 
					int chk_ones = ((chksum_start % 10)+48);
					char checksum_pop_array[2] = {(char)chk_tens, (char)chk_ones};
					char *checksum_pop_pt = checksum_pop_array;
					memcpy(stuffed_tbuff_2.ptr + L2_size*i + 117 + num_stuffed + total_stuffed, checksum_pop_pt, 2); //Checksum



				}
				total_stuffed += num_stuffed; //loop terminates before any num_stuffed in the n-1 frame are added to total, so make sure to do...

				//that just leaves the last chunk (and the <MSS data segm bytes it contains) to handle...
				//i set to beginnging of last frame already...CHECK
				i= (num_frames - 1); //for the last frame (indexed 0 to n-1)
				int leftover_bytes = data_count % MSS;
				
				//Headers...
					
				memcpy(tbuff_2.ptr + L2_size*i, STX_pt, 1); //STX

				/*Kulk suggestions: 
				#define STX 2
				#define ETX ... etc
				Then, *(buf+i) = STX; //assuming that you have the position 'i' correct, within the buffer*/

				//Count...
				int count_nframe = leftover_bytes + 60; //specify a new count value for the last frame...
				if(count_nframe >= 100){ //three places to turn into ASCII repr
					int hund = ((count_nframe/100)+ 48); 
					int tens = (((count_nframe - 100)/10) + 48);
					int ones = (((count_nframe - 100)%10) + 48);
					char count_term_array[3] = {(char)hund, (char)tens, (char)ones};
					char *count_term_array_pt = count_term_array;
					memcpy(tbuff_2.ptr + L2_size*i + 1, count_term_array_pt, 3); //Count
				}
				else if(count_nframe < 100){
					int tens = ((count_nframe/10) + 48); 
					int ones = ((count_nframe % 10) + 48);
					char count_term_array[3] = {(char)'0', (char)tens, (char)ones};
					char *count_term_array_pt = count_term_array;
					memcpy(tbuff_2.ptr + L2_size*i + 1, count_term_array_pt, 3); //Count
				}
				
					
				
					
				memcpy(tbuff_2.ptr + L2_size*i + 4, ACK_pt, 1); //ACK
					
				//Seq#...
				if(n==10){ //After '9', the sequence rolls over to '0'
					n=0;
				}
				memcpy(tbuff_2.ptr + L2_size*i + 5, Seq_Num + n, 1);
				n++; //incrment n for the next number in sequence 
					
				//Lframe (the last one)...
				char Lframe[1] = {(char)'1'};
				char *Lframe_pt = Lframe;
				memcpy(tbuff_2.ptr + L2_size*i + 6, Lframe_pt, 1);
					
				memcpy(tbuff_2.ptr + L2_size*i + 7, Resvd_pt, 10); //Resvd

				//Copy L3PDU Over (layer3 header and data segment i at once)...
				memcpy(tbuff_2.ptr + L2_size*i + 17, trans_buffer + 100*i, leftover_bytes + 40);

				//Trailers...

				memcpy(tbuff_2.ptr + L2_size*i + 57 + leftover_bytes, Chksum_pt, 2); //Chksum
				
				memcpy(tbuff_2.ptr + L2_size*i + 59 + leftover_bytes, ETX_pt, 1); //ETX		
				
				tbuff_2.count = data_count; //still a count just of the total data segment bytes
				tbuff_2.count_full = 60*num_frames + data_count; //count of the total bytes, including headers and trailers

				//Character stuffing for the last frame...
				//copy over first 17 bytes of header...
				memcpy(stuffed_tbuff_2.ptr + L2_size*i + total_stuffed, tbuff_2.ptr + L2_size*i, 17); //first 57 bytes of each frame are headers to be copied over
				//now handle the actual stuffing...
				num_stuffed = 0;
				int k;		
				for(k=((i*L2_size)+17); k<( (40+leftover_bytes) + ((i*L2_size)+17)); k++){ //for each byte of the L3PDU of the final frame (with left_byte size data segm)...
					if( (*(tbuff_2.ptr + k) == 3) || (*(tbuff_2.ptr + k) == 2) || (*(tbuff_2.ptr + k) == 16)){ //if there's a control char (DLE, STX, or ETX)
						memcpy(stuffed_tbuff_2.ptr + k + num_stuffed + total_stuffed, DLE_pt, 1); //put a DLE before the control char
						num_stuffed++; //increment number of stuffed chars for this frame
						memcpy(stuffed_tbuff_2.ptr + k + num_stuffed + total_stuffed, tbuff_2.ptr + k, 1); //copy actual (control char) data byte over
					}
					else{ //data byte is not a control char
						memcpy(stuffed_tbuff_2.ptr + k + num_stuffed + total_stuffed, tbuff_2.ptr + k, 1); //just copy the data byte over
				
					}

				}
				stuffed_tbuff_2.frame_stuffing[i] = num_stuffed; //for final frame, store num_stuffed in array
			
				stuffed_data_count += num_stuffed; //add number stuffed for this frame to total data byte count
				int stuffed_count_nframe = num_stuffed + 60 + leftover_bytes; //new total byte count value for last frame is left_byte data segm, num_stuffed, and H/T	
					
				//update count...		
				if(stuffed_count_nframe >= 100){ //three places to turn into ASCII repr
					int hund = ((stuffed_count_nframe/100)+ 48); 
					int tens = (((stuffed_count_nframe - 100)/10) + 48);
					int ones = (((stuffed_count_nframe - 100)%10) + 48);
					char stuffed_count_term_array[3] = {(char)hund, (char)tens, (char)ones};
					char *stuffed_count_term_array_pt = stuffed_count_term_array;
					memcpy(stuffed_tbuff_2.ptr + L2_size*i + 1 + total_stuffed, stuffed_count_term_array_pt, 3); //Count
				}
				else if(stuffed_count_nframe < 100){
					int tens = ((stuffed_count_nframe/10) + 48); 
					int ones = ((stuffed_count_nframe % 10) + 48);
					char stuffed_count_term_array[3] = {(char)'0', (char)tens, (char)ones};
					char *stuffed_count_term_array_pt = stuffed_count_term_array;
					memcpy(stuffed_tbuff_2.ptr + L2_size*i + 1 + total_stuffed, stuffed_count_term_array_pt, 3); //Count
				}
				//Trailers...
			
				
				memcpy(stuffed_tbuff_2.ptr + L2_size*i + 57 + num_stuffed + leftover_bytes + total_stuffed, Chksum_pt, 2);

				//ETX
				memcpy(stuffed_tbuff_2.ptr + L2_size*i + 59 + num_stuffed + leftover_bytes + total_stuffed, ETX_pt, 1); //ETX
				
				//Call checksum function and populate the chksum field with this value...
				int chk_data_count = stuffed_count_nframe - 4; //concerned with data_count not including STX, ETX, and Chksum fields (4 bytes altogether)
				//so, for each frame, the data_count will be the 120 + num_stuffed for that frame, subtract 4 cuz we're not considering STX,ETX, Chk fields 					
				memcpy(chk_buff.ptr + chk_data_count_total, stuffed_tbuff_2.ptr + i*L2_size + total_stuffed + 1, chk_data_count); //put in chksum buff
				unsigned short* lay2_buff = chk_buff.ptr + chk_data_count_total;	//ptr for chksum funct				
				chk_data_count_total += chk_data_count;	

				//printf("\nchk_data_count is %d\n", chk_data_count);
				/*int y;
				printf("Checksum debugging: \n");
				for(y=0; y<chk_data_count; y++){
				printf("%c", *(lay2_buff + y));
				}*/

				//check for odd number of bytes so that string can be padded as needed...
				if(chk_data_count % 2){ //if there is a remainder, string has an odd number of bytes, so pad a '\0' so that is has an even number of bytes				
					chk_data_count++; //chk_data_count now must include the padded '\0'
					chk_data_count_total++; 
					memcpy(chk_buff.ptr + chk_data_count_total, pad_pt, 1); //add the '\0' 
				}
				//if the if condition does not apply, then there is an even number of bytes and nothing needs to be done

				//debugging...
				//printf("\nch_data_count is %d\n", chk_data_count);			
				/*printf("Checksum debugging after padding: \n");
				int m;
				for(m=0; m<chk_data_count; m++){
				printf("%c", *(lay2_buff + m));
				}*/
					 
			
				unsigned short checksum_pop = 0; //var to catch result of chksum function
				checksum_pop = chksum(lay2_buff, chk_data_count); //call chksum function
				//printf("\nChecksum: %u\n", checksum_pop);
			
				int chksum_start = checksum_pop%100;			
				//now populate the chksum field...
				int chk_tens = ((chksum_start/10)+48); 
				int chk_ones = ((chksum_start % 10)+48);
				char checksum_pop_array[2] = {(char)chk_tens, (char)chk_ones};
				char *checksum_pop_pt = checksum_pop_array;
				memcpy(stuffed_tbuff_2.ptr + L2_size*i + 57 + num_stuffed + leftover_bytes + total_stuffed, checksum_pop_pt, 2); //Checksum	





				stuffed_tbuff_2.count =  data_count; //still a count just of the total data segment bytes pre-stuffing
				stuffed_tbuff_2.count_full = 60*num_frames + data_count; //count of the total bytes pre-stuffing, including headers and trailers
				stuffed_tbuff_2.count_stuffed = stuffed_data_count; //count of the total data segment bytes plus all stuffed chars
				stuffed_tbuff_2.count_stuffed_full = 60*num_frames + stuffed_data_count; //count of total bytes (headers, trailers, stuffed chars, and orig data_count)		

	
			}
			
			else if(  (data_count % MSS) == 0){ //all frames are of size 120
				int num_frames = data_count/MSS;
				
				stuffed_tbuff_2.array_size = num_frames; //initialize array to be of size num_frames so that can store frame-specific num_stuffed values
				stuffed_tbuff_2.frame_stuffing = (int*)malloc(sizeof(int) * stuffed_tbuff_2.array_size); //to help with transmission of stuffed frames				

				//fill up transmission buffer fully
				int i;				
				int n=0; //index for seq_num array
				for(i=0; i< num_frames; ++i){ 
					//Headers...
					
					memcpy(tbuff_2.ptr + L2_size*i, STX_pt, 1); //STX
					
					memcpy(tbuff_2.ptr + L2_size*i + 1, Count_NT, 3); //Count
					
					memcpy(tbuff_2.ptr + L2_size*i + 4, ACK_pt, 1); //ACK
					
					//Seq#...
					if(n==10){ //After '9', the sequence rolls over to '0'
						n=0;
					}
					memcpy(tbuff_2.ptr + L2_size*i + 5, Seq_Num + n, 1); 
					n++;
					
					//Lframe...
					if(i== (num_frames - 1) ){
						char Lframe[1] = {(char)'1'};
						char *Lframe_last = Lframe;
						memcpy(tbuff_2.ptr + L2_size*i + 6, Lframe_last, 1);
					}
					else{
						char Lframe[1] = {(char)'0'};
						char *Lframe_pt = Lframe;
						memcpy(tbuff_2.ptr + L2_size*i + 6, Lframe_pt, 1);
					}
					memcpy(tbuff_2.ptr + L2_size*i + 7, Resvd_pt, 10); //Resvd

					//Copy L3PDU Over (layer3 header and data segment i at once)...
					memcpy(tbuff_2.ptr + L2_size*i + 17, trans_buffer + 100*i, 100);

					//Trailers...

					memcpy(tbuff_2.ptr + L2_size*i + 117, Chksum_pt, 2); //Chksum
					
					memcpy(tbuff_2.ptr + L2_size*i + 119, ETX_pt, 1); //ETX
				}
				tbuff_2.count = data_count; //still a count just of the total data segment bytes
				tbuff_2.count_full = 60*num_frames + data_count; //count of all bytes, including headers and trailers

				int chk_data_count_total=0;
				//Character Stuffing for all equal-sized frames...
				int p;				
				for(p=0; p< num_frames; p++){ //for each frame...
					total_stuffed += num_stuffed; //count of total stuffed chars throughout all prev frames (for proper indexing of stuffed buffer)
					//copy headers (count will change but rest is same) into buffer for the stuffed frames...
					memcpy(stuffed_tbuff_2.ptr + L2_size*p + total_stuffed, tbuff_2.ptr + L2_size*p, 17); //first 57 bytes of each frame are headers to be copied over
					//now handle the actual stuffing...
					num_stuffed=0; //for updating the count...
					int k;		
					for(k=((p*L2_size)+17); k<(100 + ((p*L2_size)+17)); k++){ //for each byte of the L3PDU of each frame...
						if( (*(tbuff_2.ptr + k) == 3) || (*(tbuff_2.ptr + k) == 2) || (*(tbuff_2.ptr + k) == 16)){ //if there's a control char (DLE, STX, or ETX)
							memcpy(stuffed_tbuff_2.ptr + k + num_stuffed + total_stuffed, DLE_pt, 1); //put a DLE before the control char
							num_stuffed++; //increment number of stuffed chars
							memcpy(stuffed_tbuff_2.ptr + k + num_stuffed + total_stuffed, tbuff_2.ptr + k, 1); //copy actual (control char) data byte over
						}
						else{ //data byte is not a control char
							memcpy(stuffed_tbuff_2.ptr + k + num_stuffed + total_stuffed, tbuff_2.ptr + k, 1); //just copy the data byte over
				
						}

					}
					stuffed_tbuff_2.frame_stuffing[p] = num_stuffed; //store frame specific num_stuffed value in array
					stuffed_data_count += num_stuffed; //add number stuffed for this frame to total data byte count
					int stuffed_count_frame = num_stuffed + 120; //new total byte count value for frame... (60 data byt and 60 H/T bytes to start, so just add num_stuffed)	
					
					//update count...		
					if(stuffed_count_frame >= 100){ //three places to turn into ASCII repr
						int hund = ((stuffed_count_frame/100)+ 48); 
						int tens = (((stuffed_count_frame - 100)/10) + 48);
						int ones = (((stuffed_count_frame - 100)%10) + 48);
						char stuffed_count_term_array[3] = {(char)hund, (char)tens, (char)ones};
						char *stuffed_count_term_array_pt = stuffed_count_term_array;
						memcpy(stuffed_tbuff_2.ptr + L2_size*p + 1 + total_stuffed, stuffed_count_term_array_pt, 3); //Count
					}
					else if(stuffed_count_frame < 100){
						int tens = ((stuffed_count_frame/10) + 48); 
						int ones = ((stuffed_count_frame % 10) + 48);
						char stuffed_count_term_array[3] = {(char)'0', (char)tens, (char)ones};
						char *stuffed_count_term_array_pt = stuffed_count_term_array;
						memcpy(stuffed_tbuff_2.ptr + L2_size*p + 1 + total_stuffed, stuffed_count_term_array_pt, 3); //Count
					}
					//Trailers...
			
					
					memcpy(stuffed_tbuff_2.ptr + L2_size*p + 117 + num_stuffed + total_stuffed, Chksum_pt, 2);

					//ETX
					memcpy(stuffed_tbuff_2.ptr + L2_size*p + 119 + num_stuffed + total_stuffed, ETX_pt, 1); //ETX

					//Call checksum function and populate the chksum field with this value...
					int chk_data_count = stuffed_count_frame - 4; //concerned with data_count not including STX, ETX, and Chksum fields (4 bytes altogether) 		

					memcpy(chk_buff.ptr + chk_data_count_total, stuffed_tbuff_2.ptr + p*L2_size + total_stuffed + 1, chk_data_count); //copy into chksum buff
					unsigned short* lay2_buff = chk_buff.ptr + chk_data_count_total;	//ptr for funct				
					chk_data_count_total += chk_data_count;			
				

					//check for odd number of bytes so that string can be padded as needed...
					if(chk_data_count % 2){ //if there is a remainder, string has an odd number of bytes, so pad a '\0' so that is has an even number of bytes
						chk_data_count++; //chk_data_count now must include the padded '\0'
						chk_data_count_total++;
						memcpy(chk_buff.ptr + chk_data_count_total, pad_pt, 1); //add the '\0' 
					}
					//if the if condition does not apply, then there is an even number of bytes and nothing needs to be done
			
					unsigned short checksum_pop = 0; //var to catch result of chksum function
					checksum_pop = chksum(lay2_buff, chk_data_count); //call chksum function
					//printf("\nChecksum: %u\n", checksum_pop);
			
					int chksum_start = checksum_pop%100;			
					//now populate the chksum field...
					int chk_tens = ((chksum_start/10)+48); 
					int chk_ones = ((chksum_start % 10)+48);
					char checksum_pop_array[2] = {(char)chk_tens, (char)chk_ones};
					char *checksum_pop_pt = checksum_pop_array;
					memcpy(stuffed_tbuff_2.ptr + L2_size*p + 117 + num_stuffed + total_stuffed, checksum_pop_pt, 2); //Checksum

										
				}
				
				stuffed_tbuff_2.count =  data_count; //still a count just of the total data segment bytes pre-stuffing
				stuffed_tbuff_2.count_full = 60*num_frames + data_count; //count of all bytes pre-stuffing, including headers and trailers
				stuffed_tbuff_2.count_stuffed = stuffed_data_count; //count of total data segment bytes plus all stuffed chars
				stuffed_tbuff_2.count_stuffed_full = 60*num_frames + stuffed_data_count; //count of total bytes (headers, trailers, stuffed chars, and orig data_count)		

				
			} 			

		}
		
		else{ //segmentation was not performed at layer3, just add headers and trailers to the one frame
			
			stuffed_tbuff_2.array_size = 1;
			stuffed_tbuff_2.frame_stuffing = (int*)malloc(sizeof(int) * stuffed_tbuff_2.array_size); //only one frame (need frame-specific vals for transm)
			//printf("%i", *(stuffed_tbuff_2.frame_stuffing) ); 

			int n=0; //index for seq_num array			
			//Headers...
					
			memcpy(tbuff_2.ptr, STX_pt, 1); //STX
					
			//Count...
			int count_nframe = data_count + 60; //specify a new count value for the one frame...
			if(count_nframe >= 100){ //three places to turn into ASCII repr
				int hund = ((count_nframe/100)+ 48); 
				int tens = (((count_nframe - 100)/10) + 48);
				int ones = (((count_nframe - 100)%10) + 48);
				char count_term_array[3] = {(char)hund, (char)tens, (char)ones};
				char *count_term_array_pt = count_term_array;
				memcpy(tbuff_2.ptr + 1, count_term_array_pt, 3); //Count
			}
			else if(count_nframe < 100){
				int tens = ((count_nframe/10) + 48); 
				int ones = ((count_nframe % 10) + 48);
				char count_term_array[3] = {(char)'0', (char)tens, (char)ones};
				char *count_term_array_pt = count_term_array;
				memcpy(tbuff_2.ptr + 1, count_term_array_pt, 3); //Count
			}
					
			memcpy(tbuff_2.ptr + 4, ACK_pt, 1); //ACK
					
			//Seq#...
			if(n==10){ //After '9', the sequence rolls over to '0'
				n=0;
			}
			memcpy(tbuff_2.ptr + 5, Seq_Num + n, 1); 
					
			//Lframe (1 frame which is also the last frame)...
			char Lframe[1] = {(char)'1'};
			char *Lframe_last = Lframe;
			memcpy(tbuff_2.ptr + 6, Lframe_last, 1);
				
			memcpy(tbuff_2.ptr + 7, Resvd_pt, 10); //Resvd

			//Copy L3PDU Over (layer3 header and data segment i at once)...
			memcpy(tbuff_2.ptr + 17, trans_buffer, data_count + 40);

			//Trailers...

			memcpy(tbuff_2.ptr + 57 + data_count, Chksum_pt, 2); //Chksum
		
			memcpy(tbuff_2.ptr + 59 + data_count, ETX_pt, 1); //ETX
			
			tbuff_2.count = data_count; //still a count just of the total data segment bytes
			tbuff_2.count_full = 60 + data_count; //count of total bytes, including headers and trailers

			//for debugging...
			/*int z;
			printf("\nContents of layer 2 transmission buffer before stuffing: \n");
				for(z=0; z<tbuff_2.count_full; z++){ //tbuff_2 contents for transmission		
				printf("%c", *(tbuff_2.ptr+ z));		
			}*/

			//Character Stuffing for the one frame...
			
			//move headers (first 17 bytes) into buffer for the stuffed frames...
			memcpy(stuffed_tbuff_2.ptr, tbuff_2.ptr, 17);
			//now handle the actual stuffing...
			num_stuffed=0; //for updating the count...
			int j;			
			for(j=17; j<(data_count + 57) ; j++){ 
				if( (*(tbuff_2.ptr + j) == 3) || (*(tbuff_2.ptr + j) == 2) || (*(tbuff_2.ptr + j) == 16)){ //if there's a control char (DLE, STX, or ETX)
				//if(  (tbuff_2.ptr[j] == 3) || (tbuff_2.ptr[j] == 2) || (tbuff_2.ptr[j] == 16)) {				
					memcpy(stuffed_tbuff_2.ptr + j + num_stuffed, DLE_pt, 1); //put a DLE before the control char
					num_stuffed++; //increment number of stuffed chars
					//printf("%d", num_stuffed); //debugging...
					memcpy(stuffed_tbuff_2.ptr + j+ num_stuffed, tbuff_2.ptr + j, 1); //copy actual (control char) data byte over
				}
				else{ //data byte is not a control char
					memcpy(stuffed_tbuff_2.ptr + j + num_stuffed, tbuff_2.ptr + j, 1); //just copy the data byte over
				
				}

			}
			stuffed_tbuff_2.frame_stuffing[0] = num_stuffed; //store num_stuffed value
			//Update Count...
			stuffed_data_count += num_stuffed; //stuffed_data_count is the original data_count (already initialized to) plus any stuffed chars (num_stuffed)
			int stuffed_count_nframe = stuffed_data_count + 60; //new count value for the one frame...
			if(stuffed_count_nframe >= 100){ //three places to turn into ASCII repr
				int hund = ((stuffed_count_nframe/100)+ 48); 
				int tens = (((stuffed_count_nframe - 100)/10) + 48);
				int ones = (((stuffed_count_nframe - 100)%10) + 48);
				char stuffed_count_term_array[3] = {(char)hund, (char)tens, (char)ones};
				char *stuffed_count_term_array_pt = stuffed_count_term_array;
				memcpy(stuffed_tbuff_2.ptr + 1, stuffed_count_term_array_pt, 3); //Count
			}
			else if(stuffed_count_nframe < 100){
				int tens = ((stuffed_count_nframe/10) + 48); 
				int ones = ((stuffed_count_nframe % 10) + 48);
				char stuffed_count_term_array[3] = {(char)'0', (char)tens, (char)ones};
				char *stuffed_count_term_array_pt = stuffed_count_term_array;
				memcpy(stuffed_tbuff_2.ptr + 1, stuffed_count_term_array_pt, 3); //Count
			}
			//Trailers...
			
			memcpy(stuffed_tbuff_2.ptr + 57 + stuffed_data_count, Chksum_pt, 2);

			//ETX
			memcpy(stuffed_tbuff_2.ptr + 59 + stuffed_data_count, ETX_pt, 1); //ETX

			
			//Call checksum function and populate the chksum field with this value...
			int chk_data_count = (stuffed_data_count+60) - 4; //concerned with data_count not including STX, ETX, and Chksum fields (4 bytes altogether)
			memcpy(chk_buff.ptr, stuffed_tbuff_2.ptr + 1, chk_data_count); //move one frame into chksum buffer
			
						
		
			/*unsigned short *hold = malloc(7040);
			unsigned short *chk_us = malloc(7040);			
		
			int h;
			for(h=0; h<chk_data_count;h++){
				unsigned short element = (unsigned short) (*(stuffed_tbuff_2.ptr + 1 + h) - 48);
				*(hold + h) = element;			
			}*/
			//unsigned short *hold_pt = hold;
			/*int c;
			printf("\nUnsigned checksum buffer: \n");	
			for(c=0; c<chk_data_count; c++){ //tbuff contents for transmission		
				printf("%u", *(hold_pt + c));		
			}*/
			//printf("Size of unsigned short: %d", sizeof(unsigned short));
			//printf("Size of char: %d", sizeof(char));			
			//memcpy(chk_us, hold, chk_data_count); //chk_data_count
			//unsigned short* lay2_buff = chk_us;
			unsigned short* lay2_buff = chk_buff.ptr;	//ptr for chksum funct				
						
			//for checking that checksum works properly...
			//char buff[50]= {'T', 'H', 'I'};
			/*char buff[50] = {'A','B','C','D','E'};
			lay2_buff = buff;	
			chk_data_count = 5; //3
			//lay2_buff = buff;*/

			//check for odd number of bytes so that string can be padded as needed...
			if(chk_data_count % 2){ //if there is a remainder, string has an odd number of bytes, so pad a '\0' so that is has an even number of bytes
				chk_data_count++; //chk_data_count now must include the padded '\0'
				memcpy(chk_buff.ptr + chk_data_count, pad_pt, 1); //add the '\0'
				//memcpy(chk_us + chk_data_count, pad_pt_2, 1);
				//memcpy(lay2_buff + chk_data_count, pad_pt, 1);
			}
			//if the if condition does not apply, then there is an even number of bytes and nothing needs to be done			
		
			
			/*int c;
			printf("\nChecksum buffer: \n");	
			for(c=0; c<chk_data_count; c++){ //tbuff contents for transmission		
				printf("%c", *(chk_buff.ptr + c));		
			}*/
			

			//for checking that checksum works properly...
			/*char buff[50]= {'T', 'H', 'I', '\0'};
			lay2_buff = buff;*/
			
			unsigned short checksum_pop = 0; //var to catch result of chksum function
			checksum_pop = chksum(lay2_buff, chk_data_count); //lay2_buff, chk_data_count//call chksum function
			//printf("\nChecksum: %u\n", checksum_pop);
			
			int chksum_start = checksum_pop%100;			
			//now populate the chksum field...
			int chk_tens = ((chksum_start/10)+48); 
			int chk_ones = ((chksum_start % 10)+48);
			char checksum_pop_array[2] = {(char)chk_tens, (char)chk_ones};
			char *checksum_pop_pt = checksum_pop_array;
			memcpy(stuffed_tbuff_2.ptr + 57 + stuffed_data_count, checksum_pop_pt, 2); //Checksum

			
			stuffed_tbuff_2.count = data_count; //orig data_count pre-stuffing
			stuffed_tbuff_2.count_full = data_count + 60; //full size in bytes pre-stuffing			
			stuffed_tbuff_2.count_stuffed = stuffed_data_count; //count of total data segment bytes plus any stuffed chars
			stuffed_tbuff_2.count_stuffed_full = 60 + stuffed_data_count; //count of total bytes, including headers and trailers, stuffed chars, and orig data_count



		}
			
		//return pointer to updated transmission buffer

		//SHOULD RETURN STUFFED BUFFER NOW
 		return (stuffed_tbuff_2);		
	}
	else if(type == 'r'){
		//called by receive function
		//strip off headers and trailers added by the layer 2 transmit invocation
		
		/*LAB 2 MODIFICATION: Layer 2 calls the Internet Checksum computation function and checks to see if the checksum of the received frame is correct.
		If correct, then stuffed DLEs must be destuffed by layer 2 before data is passed on to layer 3 (in addition to the already implemented header and trailer removal).
		Be careful not to removed DLEs that have not been stuffed by the data (a DLE following a stuffed DLE, for example)

		Report on the screen if the frame passed the error check after reception (Passed or did not pass)*/		

		lay_ret recep_buff;
		recep_buff.ptr = malloc(4800); //new buffer to put in only what need
		
		lay_ret destuff_buff;
		destuff_buff.ptr = malloc(4800); //temp buffer to store destuffed trans_buffer, then just strip layer 2 headers and trailers and pass to recep_buff

		lay_ret chk_buff_rec;
		chk_buff_rec.ptr = malloc(7040);

		//ADD THE FOLLOWING...
		//CALL CHECKSUM FUNCTION ON RECEIVED FRAMES..
		//IF CHECKSUM MATCHES MOVE AHEAD WITH DESTUFFING, H/T REMOVAL, AND PASSING BUFFER ON TO LAYER 3
		int l;
		int stuffing = 0;

		int chk_data_count_total = 0; //for calc own checksum
		
		char chk_rec[2]; //for catching the chksum  of each frame so that it can be converted to an int
		char *chk_rec_pt = chk_rec;

		char count_rec[3]; //for catching the count of each frame so that it can be converted to an int
		char *count_rec_pt = count_rec;

		//Vars for padding for checksum function...
		char Pad[1] = {'\0'};
		char *pad_pt = Pad; //pointer to Pad

		//unsigned short Pad_2[1] = {'\0'};
		//unsigned short *pad_pt_2 = Pad_2;

		//unsigned short *hold_rec = malloc(7040);
		//unsigned short *chk_us_rec = malloc(7040);			
	
		//declarations		
		//int count_l = 0;
		//int chk_data_count = 0;
		//unsigned short* lay2_buff;
		//int stuffing_byframe = 0;
		//int data_count_stuffing = 0;
		//int z;
		//int w;
		//int chk_recv =0;

		for(l=0; l<number_of_frames ; l++){	
			//extract count from frame
			//printf("\nStuffing: %d \n", stuffing);
			/*printf("Received frames: \n");
			int m;
			for(m=0; m<data_count; m++){		
				printf("%c", *(trans_buffer + m));		
			}*/
	
			memcpy(count_rec_pt, trans_buffer + 120*l + 1 + stuffing, 3); 
			//now that have filled char array with ascii count (chars), want to convert string to an int
			int count_l = atoi(count_rec_pt); //use c library function atoi to convert to integer that is the count of the total data bytes in the frame
			//printf("\nTotal Bytes in frame: %d\n",count_l);		
	
			int chk_data_count = count_l - 4; //concerned with data_count not including STX, ETX, and Chksum fields (4 bytes altogether)					
			//move received frames into chksum buffer...
			memcpy(chk_buff_rec.ptr + chk_data_count_total, trans_buffer + l*120 + stuffing + 1, chk_data_count);
			/*unsigned short hold_rec[chk_data_count];			
			int z;
			for(z=0; z<chk_data_count;z++){
				unsigned short elem = (unsigned short) *(trans_buffer + l*120 + stuffing + 1 + z);
				hold_rec[z] = elem;			
			}
			unsigned short *hold_rec_pt = hold_rec;			
			memcpy(chk_buff_rec.pt, hold_rec_pt, chk_data_count);*/
			/*int h;
			for(h=0; h<chk_data_count;h++){
				unsigned short elem = (unsigned short) (*(trans_buffer + l*120 + stuffing + 1 + h) - 48);
				*(hold_rec + chk_data_count_total + h) = elem;			
			}*/

			//memcpy(chk_us_rec + chk_data_count_total, hold_rec + chk_data_count_total, chk_data_count); //chk_data_count
			//unsigned short* lay2_buff = chk_us_rec + chk_data_count_total;
			unsigned short* lay2_buff = chk_buff_rec.ptr + chk_data_count_total;					
			chk_data_count_total += chk_data_count;
			/*printf("\nChecksum buffer: \n");
			int z;
			for(z=0;z<chk_data_count_total;z++){
				printf("%c", *(chk_buff_rec.ptr+ z));
			}*/	

			int data_count_stuffing = count_l - 60; //size of the data segment of the frame including stuffed chars
			int stuffing_byframe = 0;
			int w;		
			for(w=(120*l + 17 + stuffing); w<(120*l + 57 + data_count_stuffing + stuffing) ; w++){ //for each byte of the L3PDU of the frame...			
				if( (*(trans_buffer + w + stuffing_byframe) == 16)){ //if there's a stuffed DLE...
					stuffing_byframe++; //increment number of stuffing chars					
				}
			}
			//printf("\nStuffing_byframe: %d\n",stuffing_byframe);
			
			//extract checksum from frame
			memcpy(chk_rec_pt, trans_buffer + 120*l + 57 + data_count_stuffing + stuffing, 2);
			int chk_recv = atoi(chk_rec_pt);
			printf("\nChecksum received: %d\n",chk_recv);

			stuffing += stuffing_byframe;
			
			//calculate own checksum...
				
			//check for odd number of bytes so that string can be padded as needed...
			if(chk_data_count % 2){ //if there is a remainder, string has an odd number of bytes, so pad a '\0' so that is has an even number of bytes		
				//chk_data_count++; //chk_data_count now must include the padded '\0'
				chk_data_count++; //chk_data_count now must include the padded '\0'
				chk_data_count_total++;
				memcpy(chk_buff_rec.ptr + chk_data_count_total, pad_pt, 1);  //add '\0'
				//memcpy(chk_us_rec + chk_data_count_total, pad_pt_2, 1);  //add '\0'
			}
			//if the if condition does not apply, then there is an even number of bytes and nothing needs to be done

			unsigned short checksum_pop = 0; //var to catch result of chksum function
			checksum_pop = chksum(lay2_buff, chk_data_count); //call chksum function
			printf("\nChecksum: %u\n", checksum_pop);
			int chksum_start = checksum_pop%100;			
			int chk_tens = ((chksum_start/10)+48); 
			int chk_ones = ((chksum_start % 10)+48);
			char checksum_pop_array[2] = {(char)chk_tens, (char)chk_ones};
			char *checksum_pop_pt = checksum_pop_array;
			int chksum_recv_calc = atoi(checksum_pop_pt);

			if(chksum_recv_calc == chk_recv){
				printf("\nMatch. Good to go!\n");
			}
			else{
				printf("\nERROR. Checksums DO NOT MATCH.\n");
				exit(0);
			}
			
		}

		//ONLY DO THE FOLLOWING IF CHKSUM WORKS OUT
		//Perform destuffing first to get original data_count, then can move into recep_buff as was done in lab 1
		
		int k;
		int total_destuffed = 0;		
		for(k=0; k<number_of_frames; k++){ //for each frame...

			//insert headers before L3PDU as is...
			memcpy(destuff_buff.ptr + 120*k, trans_buffer + 120*k + total_destuffed, 17);
			
			//get count field and convert to int, use for indexing purposes
			memcpy(count_rec_pt, trans_buffer + 120*k + 1 + total_destuffed, 3); 
			//now that have filled char array with ascii count (chars), want to convert string to an int
			int count_k = atoi(count_rec_pt); //use c library function atoi to convert to integer that is the count of the total data bytes in the frame
			int data_count_stuff = count_k - 60; //size of the data segment of the frame including stuffed chars
			//now perform the destuffing on the L3PDU portion
			int num_destuffed = 0; //amount destuffed for each frame	
			int j;			
			for(j=(120*k + 17 + total_destuffed); j<(120*k + 57 + data_count_stuff + total_destuffed) ; j++){ //for each byte of the L3PDU of the frame...			
				if( (*(trans_buffer + j + num_destuffed) == 16)){ //if there's a stuffed DLE...
					num_destuffed++; //increment number of destuffed chars					
					memcpy(destuff_buff.ptr + (j-total_destuffed), trans_buffer + j + num_destuffed, 1); //skip over the DLE and copy the actual data byte
				}
				else{ //data byte is not a DLE
					memcpy(destuff_buff.ptr + (j-total_destuffed), trans_buffer + j + num_destuffed, 1); //just copy the data byte over
				
				}

			}
 			total_destuffed += num_destuffed; //update total destuffed count to include all destuffed chars to this point
		
		}
		destuff_buff.count_full = data_count - total_destuffed; //get orig data_count...

		//Now just proceed with layer 2 receive functionality as was done in lab 1...
		 
		if(destuff_buff.count_full > 120){ //multiple frames to deal with			

			if( (destuff_buff.count_full % 120) != 0){ //last frame will have a count <120 after destuffing
		
				int num_frames = (destuff_buff.count_full/120) + 1;
				//fill up reception buffer to the second to last chunk				
				int i;
				for(i=0; i< num_frames - 1 ; ++i){ 
					
					memcpy(recep_buff.ptr + 100*i, destuff_buff.ptr + 120*i + 17, 40); //copy headers over
					
					memcpy(recep_buff.ptr + 100*i + 40, destuff_buff.ptr + 120*i + 57, 60); //copy data segms over
	
				}
				//that just leaves the last chunk (and the <MSS data segm bytes it contains) to handle...
				//i set to beginnging of last frame already
				i= (num_frames - 1); //for the last frame (indexed 0 to n-1)
				int leftover_bytes = (destuff_buff.count_full % 120) - 60;

				memcpy(recep_buff.ptr + 100*i, destuff_buff.ptr + 120*i + 17, 40); //copy header for last frame over
					
				memcpy(recep_buff.ptr + 100*i + 40, destuff_buff.ptr + 120*i + 57, leftover_bytes); //copy <MSS size data segm over for last frame 

				recep_buff.count_full = destuff_buff.count_full - 20*num_frames;
			}
			
			else if(  (destuff_buff.count_full % MSS) == 0){ //all frames are of size 120
				int num_frames = destuff_buff.count_full/120;
				//fill up transmission buffer fully
				int i;				
				for(i=0; i< num_frames; ++i){ 
					
					memcpy(recep_buff.ptr + 100*i, destuff_buff.ptr + 120*i + 17, 40); //copy headers over
					
					memcpy(recep_buff.ptr + 40 + 100*i, destuff_buff.ptr + 120*i + 57, 60); //copy data segments over, all are of size MSS (60 byt)
							
				}
				recep_buff.count_full = destuff_buff.count_full - 20*num_frames; //count of all bytes, minus layer2 headers and trailers
			} 			

		}
		
		else{ //one frame
			int data_bytes = destuff_buff.count_full - 60; //size of data segment
			memcpy(recep_buff.ptr, destuff_buff.ptr + 17, 40); //move header over
			memcpy(recep_buff.ptr + 40, destuff_buff.ptr + 57, data_bytes); //move data segment over
			
			recep_buff.count_full = destuff_buff.count_full - 20; //all bytes minus lay 2 headers and trailers and stuffed chars

		}		

		return(recep_buff);
	}

	
}
