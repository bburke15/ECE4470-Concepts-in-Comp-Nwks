#include "lab1.h"

/* for (call by) transmit function...
layer 4 opens a file named sendfile.txt and reads all data stored
therein, char by char in a data buffer (array). File has at most 2 KB (2048 byt)
of data. Layer 4 terminates by returning pointer to the data buffer
*/


/* for (call by) receive function...
pointer passed to layer 4 from layer 3. layer 4 retrieves the received data and stores in 
a text file called recvfile.txt.*/


struct ret_help layer4 (char * recep_buff, int data_count, char type)
{
	
	

	FILE* fp = fopen("sendfile_ricky.txt", "r");
    	if (fp == NULL){
        	printf("Could not open file\n");
    	}
	
	
	lay_ret dbuff; //structure object to hold count and data_buffer pointer	

	dbuff.ptr = malloc(2049); //create data buffer tht can handle up to 2 KB data
	
	if(type == 't'){
		//called by transmit function
		//read text file data into buffer...
		if (fp != NULL) {  
			int n=0;
			dbuff.count = 0; //1 char is 1 byte
			char c;
			while((c=fgetc(fp)) != EOF){
				dbuff.ptr[n] = c;
				dbuff.count++;
				n++;
			}   			

		}
		fclose(fp);

		//print contents of buffer for testing...
		/*int k;		
		for(k=0; k<dbuff.count; k++){
			printf("%c", *(dbuff.ptr + k));		
		} */

		//return data buffer ptr and byte count		
  		return (dbuff);		
	}
	else if(type == 'r'){
		//called by receive function
		//write contents of data buffer to output text file

		printf("\nFrames received by layer4:\n");
		int r;
		for(r=0; r<data_count; r++){		
			printf("%c", *(recep_buff + r));		
		}		

		FILE *file = fopen("outfile.txt", "w");
		if (file == NULL){
    			printf("Error opening file\n");
		}
		fwrite(recep_buff, 1, data_count, file);
		fclose(file);
	}

}
