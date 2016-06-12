# simple-crypto-char-driver

This is really super module you have never seen !   
This module encrypts data so no one can decrypt it, even all computers in the world working together ... 

:) I am joking, just an example of charachter device driver (kernel module) that performs simple shifting of input text and stores in memory.   

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

Log
---

All logs are provided in file `/var/log/kernel.log` or you can use `dmesg` command
