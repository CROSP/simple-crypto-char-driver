# simple-crypto-char-driver

This is really super module you have never seen !   
This module encrypts data so no one can decrypt it, even all computers in the world working together ... 

:) I am joking, just an example of character device driver (kernel module) that performs simple shifting of input text and stores in memory.   

Usage
-----

Example is provided in `module_test.c` file.    

Installation
------------

1. Compile module `make` 
2. Optionally install it `make install` 
3. Use `depmode -a` to resolve all dependencies and recognize module
4. Call `modprobe crypto_dev` to install module
5. Remove module using `modprobe -r crypto_dev`   

**Or** 

3. Call `insmod crypto_dev.ko` 
4. Remove module using `rmmod crypto_dev`

Module parameter
----------------

You can provide module the `shift` coefficient parameter as following   
`modprobe crypto_dev shift=23`    

OR    

`insmod crypto_dev.ko shift=23`    

This is shifting coefficient used while shifting input text.      

You can also change this parameter in runtime.    
To do this obtain `root` privileges and set desired value in the file `/sys/module/crypto_dev/parameters/shift`     
This file will be missing in case you haven't provided parameter while loading the module.   
Example is `echo 32 > /sys/module/crypto_dev/parameters/shift`  


Log
---

All logs are provided in file `/var/log/kernel.log` or you can use `dmesg` command


Must read
---------

https://sysplay.in/blog/tag/character-device-files/    

http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/

