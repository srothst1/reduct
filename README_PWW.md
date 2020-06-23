We have implemented a distributed file system with four basic functions: open, close, read, and write with deduplication functionality.

There are 4 major components of our source code:

MasterNode: It is the node that stores all metadata about the file system in memory.

DataNode: Stores and retrieves blocks of data upon request.

ClientNode: The process/daemon that provides an interface to multiple client processes executing on the same machine and coordinates instructions and communications between the client programs, and the rest of the file system

ClientProg: Library that the user can include to access the file system when developing an application.

It is still a working progress, but we have implemented enough functionality to start conducting experiments. There is a main program in ClientProg that is a sample program that writes into and reads from the distributed file system. We will most likely change the code even as we start experiments, so we would highly recommend that you not try executing the program as it may crash (potentially with large memory leaks!) and the file blocks will be stored in local directory under /local/dfs (which must be removed). However, if you would like to run the program, you can do so with the following instructions. Begin by opening four terminal windows and finding our project directory. Enter the command line arguments in the following order:

In MasterNode:
make  	
./main

In DataNode:
make
./main IP address of MasterNode

In ClientNode:
make
./main IP address of MasterNode

In ClientProg:
make
./main IP address of the ClientNode

The program will start execution, writing files called “foo” and “foo2” into the file system and read the files into a buffer. It is important to note that the MasterNode must be executed on a machine different from the rest of the programs. Other programs may run on the same local machine.

After the sample program finishes execution, other programs will be waiting to accept socket connection. Hence, it is necessary to manually terminate each program by entering quit into the command line. quit protocol ensures that there are no major memory leaks (especially buffers). To terminate the programs, in order, enter:

In ClientNode:
quit

In DataNode:
quit

In MasterNode:
quit

Schedule (from proposal)  with Annotations

Week 1 (04/06-04/12): Finish implementing the client side API. Thoroughly test the functions using "dummy" master node and data node.    

Annotation: Finished implementing “Client Node” of the client side and the master node functionality. We collectively worked together to implement the core data structures/classes and protocols and put them together to build the main programs. Data structures were thoroughly tested, but main programs were not tested as it required other parts of the file system to function.

Week 2 (04/13-04/19): Finish implementing the master node functionality. Thoroughly test the functions using already implemented client side API and dummy data node.

Annotation: Finished implementing the client side and the DataNode. The file system was essentially complete with the core functionality.  Thus, we thoroughly tested and debugged the whole system with multiple sample programs. The system is now ready for conducting basic experiments.

Week 3 (04/20-04/26): Finish implementing the data node functionality. Thoroughly test the the fully functioning system.

Annotation: Plan what kind of experiments we will conduct. Figure out the logistics of conducting the experiments on multiple local machines. Start writing the programs and scripts for conducting the experiments with incremental testing. Start running the experiments, if possible.

Week 4 (04/27-05/03): Conduct experiments. Write the final report and prepare for presentation.

Annotation: Continue conducting experiments, and collect results. Analyze the results and start writing the final report. Start working on the class presentation

Week 5 (05/03-05/10): Finish writing the report, clean up the source code for submission.

Annotation: Present our project to the class. Finish writing the final report and complete any miscellaneous tasks.
