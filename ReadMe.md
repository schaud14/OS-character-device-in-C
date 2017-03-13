CS550:
Assignment 3:

Submitted by:
1. Sagar Kale
	skale4@binghamton.edu
2. Saurabh Chaudhari
	schaud14@binghamton.edu

Description:

This is a classical producer-consumer problem. Implement a miscellaneous character device in the Linux Kernel. This device must maintain a FIFO queue (i.e. a pipe) of maximum N strings (configured as a module parameter)

1. In Userspace there are muliple concurrent processes(Producers and Consumers).
2. Producers produces the random strings and writes them to character device "mypipe".
3. Consumers consumes the strings from character device "mypipe".
4. Charater device has storing limit defined when inserting the kernel module, so when the device is full, any producer trying to write will get blocked .
5. Similary when character device is empty, i.e. no strings are stored in device, any consumer trying to consume the string will get blocked. 
6. When a consumer reads from a (full) pipe, it wakes up all blocked producers. In this case, no blocked consumer should be woken up.
7. When a producer writes to a (empty) pipe, it wakes up all blocked consumers. In this case, no blocked producer should be woken up.


Instructions for testing:
1. Creating/Compiling the module.
	make
2. Inserting the module in kernel.
	sudo insmod kernmod.ko bufferLimit=<size of buffer/Character Device>
	eg.sudo insmod kernmod.ko bufferLimit=10
3. Compiling producer
	make producer
4. Compiling consumer
	make consumer
5. Running the Producer.
	./producer <No of Producers> <No of Strings each Producer will write>
	eg. ./producer 5 1 (5 Producers will create each 1 string, in total 5)
	eg. ./producer 5 1 (5 Producers will create each 2 strings, in total 10)
6. Running the Consumer.
	./consumer <No of Producers> <No of Strings each Producer will write>
	eg. ./consumer 5 1 (5 consumers will consume each 1 string, in total 5)
	eg. ./consumer 5 1 (5 consumers will consume each 2 strings, in total 10)
7. Clean the code/executables:
	make clean
8. To remove module from kernel:
	sudo rmmod kernmod
