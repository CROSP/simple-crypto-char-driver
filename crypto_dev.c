/**
 * @file    crypto_dev.c
 * @author  Molochko Alexander
 * @date    12 June 2016
 * @version 1.0
 * @brief  Simple char device that shifts characters
*/

#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>          // Required for the copy to user function

#define DEVICE_NAME "crypto_dev"
#define CLASS_NAME "crypto"
#define MESSAGE_SIZE 1024
#define MAX_CHAR_CODE 255

MODULE_LICENSE("GPL");            
MODULE_AUTHOR("Alexander Molochko, http://crosp.net"); 
MODULE_DESCRIPTION("A simple character device driver which encrypts data (simple shifting)"); 
MODULE_VERSION("1.0");
MODULE_PARM_DESC(shift, "Shift amount of each character");  ///< parameter description

// Function protoypes 

static int dev_open(struct inode *,struct file *);
static int dev_release(struct inode *,struct file *);
static ssize_t dev_read(struct file *,char *,size_t,loff_t *);
static ssize_t dev_write(struct file *, const char *,size_t, loff_t *);
static char * crypt_text(char *,size_t);
// Structure for file operations
static struct file_operations fops = 
{
	.owner = THIS_MODULE,
	.read = dev_read,
	.write = dev_write,
	.open = dev_open,
	.release = dev_release,
};
static int major_number;
// Variables 
static struct class*  dev_crypt_class  = NULL;
static struct device* dev_crypt_device = NULL;
static char  message[MESSAGE_SIZE] = {0};
static short size_of_message;
static int device_open_times = 0;

// Define mutex 

static DEFINE_MUTEX(crypto_dev_mutex);

// Module params
static short shift = 1;

module_param(shift, short, S_IRUSR | S_IWUSR);
// Initialization of module

static int __init device_init(void) {
	// Init mutex
	mutex_init(&crypto_dev_mutex); 
	// Find major number for device dynamically
	major_number = register_chrdev(0,DEVICE_NAME,&fops);
	if(major_number < 0) {
		printk(KERN_ALERT "CRYPTO DEVICE : Device registration failed .. \n");
	}
	else {
		printk(KERN_INFO "CRYPTO DEVICE : Device registration is successful. Major number is %d \n",major_number);
	}
	// Create class
	dev_crypt_class = class_create(THIS_MODULE,CLASS_NAME);
	if (IS_ERR(dev_crypt_class)){
		printk(KERN_ALERT "CRYPTO DEVICE : Failed to create device class. Unregistering device with major number %d \n",major_number);	
		unregister_chrdev(major_number, DEVICE_NAME);
		// Return error (pointer)
		return PTR_ERR(dev_crypt_class);
	}
	else {
		printk(KERN_INFO "CRYPTO DEVICE : Device class creation is successful. Major number is %d \n",major_number);		
	}
	// Create device structure
	dev_crypt_device = device_create(dev_crypt_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
	if (IS_ERR(dev_crypt_device)){
		printk(KERN_ALERT "CRYPTO DEVICE : Failed to create device structure. Unregistering device with major number %d \n",major_number);
		// Remove device class
		class_destroy(dev_crypt_class);
		unregister_chrdev(major_number, DEVICE_NAME);
		// Return error (pointer)
      	return PTR_ERR(dev_crypt_device);		
	}
	else {
		printk(KERN_INFO "CRYPTO DEVICE : Device structure creation is successful. Major number is %d \n",major_number);		
	}
	printk(KERN_INFO "CRYPTO DEVICE : Shifting coefficient is %d \n",shift);
	return 0;
}

// Clean up resource, module is going to be unloaded

static void __exit device_exit(void) {
	// Destroy mutex   
	mutex_destroy(&crypto_dev_mutex); 
	device_destroy(dev_crypt_class, MKDEV(major_number, 0));
 	class_unregister(dev_crypt_class);
	class_destroy(dev_crypt_class);
	unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "CRYPTO DEVICE : Goodbye, have a nice day!\n");
}

static int dev_open(struct inode* inod,struct file * fil) {
	if(!mutex_trylock(&crypto_dev_mutex)){
		printk(KERN_ALERT "CRYPTO DEVICE : Device is used by another process at the moment \n");
      	return -EBUSY;
	} 
	device_open_times++;
	printk(KERN_INFO "CRYPTO DEVICE : Device was opened %d times \n",device_open_times);
	printk(KERN_INFO "CRYPTO DEVICE : DEBUG: Passed %s %d \n",__FUNCTION__,__LINE__);
	return 0;
}
static ssize_t dev_read(struct file * in_file,char * buff,size_t len,loff_t *off) {
	printk(KERN_INFO "CRYPTO DEVICE : Shifting coefficient is %d \n",shift);
    int is_any_error = 0;
	is_any_error = copy_to_user(buff, message, size_of_message);
	if(is_any_error == 0) {
		printk(KERN_INFO "CRYPTO DEVICE : %d characters were sent to the user \n",size_of_message);
		size_of_message = 0;
		return 0;
	}
	else {
		printk(KERN_ALERT "CRYPTO DEVICE : Failed to sent %d characters to the user\n",is_any_error);
		return -EFAULT;
	}
}
static ssize_t dev_write(struct file * to_open,const char *buff,size_t len,loff_t *off) {
   printk(KERN_INFO "CRYPTO DEVICE : Shifting coefficient is %d \n",shift);
   // Check bounds of char
   if (shift >= MAX_CHAR_CODE) {
   		shift = 1;
  		printk(KERN_ALERT "CRYPTO DEVICE : Invalid shift coefficient, resetting to 1 \n");
   }
   // Clear buffer before write data
   memset(message,0,strlen(message));
   // Copy input buffer
   strncpy(message, buff,len);
   // Save size of message
   size_of_message = strlen(message);
   crypt_text(message,len);
   printk(KERN_INFO "CRYPTO DEVICE : Received %d characters from the user\n", len);
   return len;
}
static int dev_release(struct inode *inod,struct file * file) {
	// Release mutex
	mutex_unlock(&crypto_dev_mutex);
	printk(KERN_INFO "Releasing device with major number %d \n",major_number);
	return 0;
}
static char * crypt_text(char * input,size_t len) {
   int length = strlen(input);
   int i = 0;
   length = length <= len ? length : len;
   while (i < length) {
      input[i] = input[i]+shift;
   	  i++;
   }
   return input;
}
module_init(device_init);
module_exit(device_exit);