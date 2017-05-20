#include "lab1.h"

/* for (call by) transmit side (layer 2)...
Checksum function should be written to compute checksum of layer 2 frame over only those fields specfic to the L2PDU (layer 2 headers and trailers).
Thus, the function will return an integer value (the checksum), and at least one parameter will be the stuffed layer 2 frames (the buffer holding these).
Checksum is computed after character stuffing is done, but this is handled in layer 2. Layer 2 stores the returned checksum in the 2-byte checksum field  
*/

/*for (call by) receive side (layer 2)...
Receive calls the Internet Checksum computation function and checks it against the checksum specifed in the chksum field of the recieved frame. Either way
(whether the checksum function is called by receive or transmit), it's just doing the same checksum computation, thus 'r' or 't' doesn not have to be specified
as a parameter.
*/

unsigned short chksum (unsigned short* lay2_buff, int data_count) {

	//modify textbook algorithm...is really the 1's complement sum of two-byte chunks of data
	//to compute the checksum over an even number of bytes, the numbers must be grouped in twos.
	//if the chksum is to be computed over an odd number of bytes, then pad out the data with the decimal 1-byte # 0 (ASCII char '\0') at the very end and compute the checksum
	//DO NOT insert the padded char '\0' in the frame's data portion
	//Alg will thus yield a 16-bit checksum. This (4 hex digits) does not fit in a 2-byte char field, so insert only the LSB of the checksum in the Chksum field of the trailer (ex. insert e8 of e7e8)
 
	//Code for the internet checksum alg pres in txtbk will work fine if...
	//1. You pass any string with an even # of bytes to the checksum function OR You first pad any string tht has an odd # of bytes w/ a '\0' so tht has even No. bytes b4 passing string to function
	//2. You replace all references to 'u_short' with 'unsigned short' as the data type in the chksum function
	//3. ** You decrement the count variable in the while loop by 2 instead of by 1 each time each time tht the while loop is executed. Reason for doing is tht Count really repr # bytes in...
	//... the frame; however you're adding 2-byte #s on each iteration. 


	/*char buff[50]= {'T', 'H', 'I', '\0'};
	lay2_buff = buff;
	data_count =10;*/
	//data_count is count of what's being checksummed
	//from textbook...
	register unsigned long sum =0; //at least 32 bits so can hold carry in upper bytes...
	//sum is at least a 32 bit loc, but when you actually return result. result is 16 bits, so can afford to chop off upper half of result
	//count is 32 bytes, adding 2 bytes at a time is 16
	while(data_count > 1){ //data_count-=2
		//printf("sum: %u\n", sum);
		//printf("\n%c\n",*lay2_buff); 		
		sum += *lay2_buff++;
		if(sum & 0xFFFF0000){
			//carry occurred so wrap around			
			sum &= 0xFFFF; //take lower 2 bytes, zero out carry, and increment
			sum++;
		}
		data_count = data_count - 2;

	}
	//printf("sum: %u\n", sum); 
	//printf("Sum after: %d\n", ~(sum & 0xFFFF)); 
	return ~(sum & 0xFFFF); 
} 
