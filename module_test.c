/**
 * @file   module_test.c
 * @author  Molochko Alexander
 * @date    12 June 2016
 * @version 1.0
 * @brief  Text source code for module
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
*/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>

#define BUFFER_LENGTH 256
#define DEVICE_NAME "/dev/crypto_dev"          
static char receive[BUFFER_LENGTH];

int main(){
   int ret, fd;
   char str_to_write[BUFFER_LENGTH];
   printf("Starting device test code example...\n");
   fd = open(DEVICE_NAME, O_RDWR);             // Open the device with read/write access
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }
   printf("Type in a short string to send to the kernel module:\n");
   scanf("%[^\n]%*c", str_to_write);                // Read in a string (with spaces)
   printf("Writing message to the device [%s].\n", str_to_write);
   ret = write(fd, str_to_write, strlen(str_to_write)); // Send the string to the LKM
   if (ret < 0){
      perror("Failed to write the message to the device.");
      return errno;
   }

   printf("Press ENTER to read back from the device...\n");
   getchar();

   printf("Reading from the device...\n");
   ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
   if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   printf("The received message is: [%s]\n", receive);
   printf("End of the program\n");
   return 0;
}
