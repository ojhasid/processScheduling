/***************************************************************************
* File: projectfive.c
* Author: Sujeet Ojha
* Procedures:
* main  - this function calls the fcfs, spn, srt, and rr functions and 
		  displays the results regarding average relative/nonrelative turn around time
		  
* fcfs - this function takes in a task stream of a thousand tasks and performs the
		 first fit algorithm to place them in memory then performs
		 the fcfs algorithm for scheduling
			 
* spn -  this function takes in a a task stream of a thousand tasks and performs the
		 next fit algorithm to place them in memory then performs
		 the spn algorithm for scheduling
			 
* srt -  this function takes in a task stream of a thousand tasks and performs the
		 best fit algorithm to place them in memory then performs
		 the srt algorithm for scheduling
			 
* rrq1 - this function takes in a task stream of a thousand tasks and performs the
		 worst fit algorithm to place them in memory then performs
		 the rrq1 algorithm for scheduling
			 
* randomize - this function fills in an array of taskStream structures with random
			  taskSize and taskDuration values from 1-16 while setting the flag
			  to -1
			  
* print - this function displays the process number, the task size, the block
		  occupied by the process, the process duration, and other process details
***************************************************************************/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int location = 0;
	
double fcfsFinalTR = 0;																										//Variables used during result calculations
double spnFinalTR = 0;
double rrFinalTR = 0;
double srtFinalTR = 0;

double fcfsFinalRTR = 0;
double spnFinalRTR = 0;
double rrFinalRTR = 0;
double srtFinalRTR = 0;

struct FCFS{																												//Struct to store FCFS results
	double totalTurnAroundTime;
	double relativeTurnAroundTime;
};

struct SPN{																													//Struct to store SPN results
	double totalTurnAroundTime;
	double relativeTurnAroundTime;
};

struct RRQ1{																												//Struct to store RRQ1 results
	double totalTurnAroundTime;
	double relativeTurnAroundTime;
};

struct SRT{																													//Struct to store SRT results
	double totalTurnAroundTime;
	double relativeTurnAroundTime;
};

struct taskStream {     																									//Creating a structure of tasks
        int taskSize;																										//Holds the size allocation request
        int taskDuration;																									//Holds the duration
        int flag;																											//Flag: -1 represents unprocessed, 1 represents processed, 2 represents duration reached 0
        int blockLocation;																									//Location of where the process is stored in memory  
        int timeSpent;																										//Time spent in execution
        int arrival;																										//Arrival time of process
        int start;																											//Process start time
        int finish;																											//Process finish time
        int turnAround;																										//Process turn around time
        double relative;																									//Process relative turn around time  
        int ignore; 																										//Flag for round robin to ensure quantum of 1	
        int quantumStarted;																									//Flag for round robin to check if burst process has started
};

struct FCFS fcfsResults;																									//Creating structure to hold FCFS results
struct SPN spnResults;																										//Creating structure to hold SPN results
struct RRQ1 rrResults;																										//Creating structure to hold RRQ1 results
struct SRT srtResults;																										//Creating structure to hold SRT results


/***************************************************************************
* void randomize( struct taskStream arrayTasks[], int memoryBlock[], int numTasks )
* Author: Sujeet Ojha
* Date: 23 April 2020
* Description: initializes the tasks in the arrayTasks array with random size and duration values
			   ranging from 1-16 and the flag as -1
* Parameters:
* arrayTasks[] I/P struct taskStream - Array of processes representing the task stream
* memoryBlock[] I/P int - Array representing the memory blocks
* numTasks I/P int - The number of tasks in the stream
* print O/P void Status code (not currently used)
***************************************************************************/

void randomize(struct taskStream arrayTasks[], int memoryBlock[], int numTasks){											//Function used to generate random task size and task duration


    for (int i = 0; i < numTasks; i++){      																				//Filling arrayTasks with random values (1-16)

        int randTaskSize = rand() % ((16 + 1)-1) + 1;       																//Task size random min 1, max 16     
        int randTaskDuration = rand() % ((16 + 1) - 1) + 1;																	//Task duration random min 1, max 16
    
        arrayTasks[i].taskSize = randTaskSize;																				//Assigning the random task size
        arrayTasks[i].taskDuration = randTaskDuration;																		//Assigning the random task duration
        arrayTasks[i].flag = -1;																							//Initializing flag as -1 to represent unprocessed
        arrayTasks[i].arrival = 0;																							//Initializing arrival time as 0
        arrayTasks[i].start = 0;																							//Initializing start time as 0
        arrayTasks[i].finish = 0;																							//Initializing finish time as 0
        arrayTasks[i].turnAround = 0;																						//Initializing turn around as 0
        arrayTasks[i].relative = 0;																							//Initializing relative turn around time as 0
        arrayTasks[i].timeSpent = 0;																						//Initializing time spent in execution as 0
        arrayTasks[i].ignore = 0;																							//Initializing ignore flag for round robin as 0
        arrayTasks[i].quantumStarted = 0;																					//Initializing quantumStarted flag for round robin as 0
    }
    
    memoryBlock[0] = 16;																									//Splitting the 52 unit block into 5 units of {16, 16, 16, 8, 2}
    memoryBlock[1] = 16;
    memoryBlock[2] = 16;
    memoryBlock[3] = 8;
    memoryBlock[4] = 2;

}


/***************************************************************************
* void srt( int memoryBlock[], int m, struct taskStream process[], int n )
* Author: Sujeet Ojha
* Date: 23 April 2020
* Description: processes the taskStream using the firstFit algorithm and stores
			   them into memory, then performs the srt algorithm for scheduling.
			   Calculates the average turn around time and the average relative
			   turn around time for all tasks.
*
* Parameters:
* memoryBlock[] I/P int - Array representing the memory blocks
* process[] I/P struct taskStream - The array of processes filled with random values
* n I/P int - The amount of tasks in the stream
* m I/P int - The amount of blocks in memory
* print O/P void Status code (not currently used)
***************************************************************************/

void srt(int memoryBlock[], int m, struct taskStream process[], int n){
  
  	location = 0;																				
  	int newArrivalTime = 0;
  	bool processed = false;																									//Boolean variable used to control logic statements
	bool allProcessed = false;																								//Boolean variable used to control logic statements
	
	while (allProcessed == false){																							//Keep going until all tasks have been processed
	
		int this;
		
		for (int i = 0; i < n; i++){ 																						//For all processes, perform the first fit algorithm
    		
    		allProcessed = true;
    		processed = false;
    		this = i;
    		
    		if (process[i].flag == -1){																						//Only process if the task's flag is -1
    		
    		 	for (int j = 0; j < m; j++){ 																				//For all the blocks in memory
            		
            		if (memoryBlock[j] >= process[i].taskSize){ 															//If allocation is possible
            			
            			process[i].flag = 1;																				//Change the process flag to 1 (representing allocated)	
						process[i].arrival = newArrivalTime;																//Set the process arrival time							
						memoryBlock[j] -= process[i].taskSize;																//Reduce  block size  
						process[i].blockLocation = j;																		//Update block location in the struct   	 												
						processed = true;														
						
						#ifdef debugMode																					//Debug mode to print statements
							printf("Success: Placed Process %d in block %d!\n", i, j);
            				printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
						#endif	
															
						break; 
            		}
           	 	
        		}
        		
        		int minimum = 2147483640;
        	
        		if (processed == false){																					//If the process was unable to be allocated	
					
					#ifdef debugMode																						//Debug mode to print statements
						printf("Failure: Process %d can't be placed!\n", this);
						printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
					#endif
					
					for (int l = 0; l < n; l++)																				//Finds the process in memory the shortest service time					
					{
						if (process[l].flag == 1){
								
							if (process[l].taskDuration < minimum)
							{

								minimum = process[l].taskDuration;
								location = l;
							
							}
									
						}
	
			
					}
					
					#ifdef debugMode																						//Debug mode to print statements
						printf("Process %d is being submitted!\n", location);
					#endif
					
					process[location].flag = 2; 																			//Mark task as completed
					memoryBlock[process[location].blockLocation] += process[location].taskSize; 							//Update the block
					process[location].start = newArrivalTime;																//Set the start time
					process[location].finish = process[location].start + process[location].taskDuration;					//Set the finish time
					process[location].turnAround = process[location].finish - process[location].arrival;					//Calculate the turn around time
					process[location].relative = (double)process[location].turnAround / process[location].taskDuration;		//Calculate the relative turn around time
					newArrivalTime = process[location].finish;																//Update the new arrival time variable
					
					#ifdef debugMode																						//Debug mode to print statements
						printf("[Process %d]\n\tArrival:%d\n\tStart:%d\n\tFinish:%d\n\tTurnAround:%d\t\n\tNew Arrival Time:%d\n\n", location,  process[location].arrival, process[location].start,  process[location].finish,  process[location].turnAround, newArrivalTime);
						printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
	 				#endif
				
        		}
        	
    		}
    	
    	
    		for (int i = 0; i < n; i++){																					//Checking if all tasks have been processed
    	
    			if (process[i].flag == -1){																					//If everything has not been processed, ensure the loop keeps executing
    		
    				allProcessed = false;
    		
    			}
    			
    	
    		}
    		
        
    	} 
    	
	
	} 

	#ifdef debugMode																										//Debug mode to print statements	
		printf("\nAll remaining tasks are in memory!\n\n");
	#endif
	
	bool memNotEmpty = true;
	
	while (memNotEmpty == true){																							//Process all tasks left in memory
	
		int minimum = 2147483640;
		
		location = -1;
		
		memNotEmpty = false;
		
		
		for (int l = 0; l < n; l++)																							//Find the process in memory with the shortest service time					
		{

			if (process[l].flag == 1){
								
				if (process[l].taskDuration < minimum)
				{

					minimum = process[l].taskDuration;
					location = l;
							
				}
									
			}
	
			
		}
		
		if (location != -1 && process[location].flag == 1){																	//If process is in memory
			
			process[location].flag = 2; 																					//Mark task as completed		
			memoryBlock[process[location].blockLocation] += process[location].taskSize; 									//Update the block
			process[location].start = newArrivalTime;																		//Set the start time															
			process[location].finish = process[location].start + process[location].taskDuration;							//Set the finish time
			process[location].turnAround = process[location].finish - process[location].arrival;							//Calculate the turn around time
			process[location].relative = (double)process[location].turnAround / process[location].taskDuration;				//Calculate the relative turn around time
			newArrivalTime = process[location].finish;																		//Update the new arrival time variable
			
			#ifdef debugMode																								//Debug mode to print statements
				printf("Process %d is being submitted!\n", location);		
				printf("[Process %d]\n\tArrival:%d\n\tStart:%d\n\tFinish:%d\n\tTurnAround:%d\t\n\tNew Arrival Time:%d\n\n", location,  process[location].arrival, process[location].start,  process[location].finish,  process[location].turnAround, newArrivalTime);
				printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
	 		#endif
		}
		
		for (int i = 0; i < n; i++){																						//Checking if all tasks have been processed

    			if (process[i].flag == 1 && location != -1){ 
    				
    				memNotEmpty = true;
    				
    			
    			}
    			
    	
    	}
		
	
	}
		
		
	for (int i = 0; i < n; i++){																							//Find the total turn around time and the total relative turn around time	
		
		srtResults.totalTurnAroundTime += process[i].turnAround;
		srtResults.relativeTurnAroundTime += process[i].relative;
	
	}
	
	
	srtResults.totalTurnAroundTime /= n;																					//Calculate turn around time and relative turn around time
	srtResults.relativeTurnAroundTime /= n;
	
	srtFinalTR += srtResults.totalTurnAroundTime;
	srtFinalRTR += srtResults.relativeTurnAroundTime;
																		
}


/***************************************************************************
* void rrq1( int memoryBlock[], int m, struct taskStream process[], int n )
* Author: Sujeet Ojha
* Date: 23 April 2020
* Description: processes the taskStream using the firstFit algorithm and stores
			   them into memory, then performs the rr (q=1) algorithm for scheduling.
			   Calculates the average turn around time and the average relative
			   turn around time for all tasks.
*
* Parameters:
* memoryBlock[] I/P int - Array representing the memory blocks
* process[] I/P struct taskStream - The array of processes filled with random values
* n I/P int - The amount of tasks in the stream
* m I/P int - The amount of blocks in memory
* print O/P void Status code (not currently used)
***************************************************************************/

void rrq1(int memoryBlock[], int m, struct taskStream process[], int n){
  
  	int rrFinish = 0;
  	int rrStart = 0;
  	location = 0;
  	int newArrivalTime = 0;
  	bool processed = false;																									//Boolean variable used to control logic statements
	bool allProcessed = false;																								//Boolean variable used to control logic statements
	int maximum = -2147483640;																								//Used to calculate max arrival time
	int allocatedProcesses = 0;																								//Variable used for ensuring quantum = 1
  	int burstProcess = 0;																									//Variable used for ensuring quantum = 1
	
	while (allProcessed == false){																							//Keep going until all tasks have been processed
	
		int this;
		
		for (int i = 0; i < n; i++){ 																						//For all processes, perform the first fit algorithm
    		
    		allProcessed = true;
    		processed = false;
    		this = i;
    		
    		if (process[i].flag == -1){																						//Only process if the task's flag is -1
    		
    		 	for (int j = 0; j < m; j++){ 																				//For all the blocks in memory
            		
            		if (memoryBlock[j] >= process[i].taskSize){ 															//If allocation is possible
            				
            			process[i].flag = 1;																				//Change the process flag to 1 (representing allocated)								
						process[i].arrival = newArrivalTime;																//Set the process arrival time																			
						memoryBlock[j] -= process[i].taskSize;																//Reduce  block size   
						process[i].blockLocation = j;																		//Update block location in the struct   	 												
						processed = true;
						allocatedProcesses++;																				//Increment the number of allocated processes
						
						#ifdef debugMode																					//Debug mode to print statements
							printf("Success: Placed Process %d in block %d!\n", i, j);
            				printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
							printf("The number of allocated process is now: %d\n\n", allocatedProcesses);
						#endif
																
						break; 
            		}
           	 	
        		}
        		
        		if (processed == false){																					//If the process was unable to be allocated	
        		
        			maximum = -2147483640;																
					
					#ifdef debugMode																						//Debug mode to print statements
						printf("Failure: Process %d can't be placed!\n", this);
						printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
					#endif
					
					if (burstProcess == allocatedProcesses){																//Used to control quantum = 1 logic
						
						#ifdef debugMode																					//Debug mode to print statements
							printf("Burst process now equals allocated processes\n");
						#endif
						
						for (int i = 0; i < n; i++){																		
						
							if (process[i].flag == 1){																		//If process is in memory, and burstprocess = allocatedprocess, change ignore flag to 0
							
								process[i].ignore = 0;
							
							}
						
						}
						
						burstProcess = 0;																				
						
						#ifdef debugMode																					//Debug mode to print statements
							printf("Burst process is now: %d\n", burstProcess);
						#endif
					}
					
					rrStart++;
					rrFinish++;																								//Variable used to calculate the rrq1 finish time
					
					for (int l = n-1; l >= 0; l--)																			//Finds the process with the maximum arrival time in memory with ignore flag of 0
					{

						if (process[l].flag == 1 && process[l].ignore != 1){
						
							#ifdef debugMode																				//Debug mode to print statements
								printf("Considering process %d with ignore flag %d\n", l, process[l].ignore);
							#endif
							
							if (process[l].arrival > maximum)
							{

								maximum = process[l].arrival;
								location = l;
							
							}
									
						}
	
			
					}
					
					if (process[location].quantumStarted == 0){
					
						process[location].start = rrStart;
						process[location].quantumStarted = 1;
						
					}
					#ifdef debugMode																						//Debug mode to print statements
						printf("Process %d ignore flag is %d\n", location, process[location].ignore);
					#endif
					
					burstProcess++;																							//Increment burstProcess
					process[location].ignore = 1;																			//Change the ignore flag to 1
		
					
					#ifdef debugMode
						printf("Picked process %d and ignore flag changed to %d\n", location, process[location].ignore);
						printf("Burst process is now: %d\n\n", burstProcess);
					#endif										
					
					
					if (burstProcess == allocatedProcesses){																//Used to control quantum = 1 logic
						
						#ifdef debugMode																					//Debug mode to print statements
							printf("Burst process now equals allocated processes\n");
						#endif
						
						for (int i = 0; i < n; i++){
						
							if (process[i].flag == 1){
							
								process[i].ignore = 0;
							
							}
						
						}
						
						burstProcess = 0;
						
						#ifdef debugMode																					//Debug mode to print statements
							printf("Burst Process is now: %d\n", burstProcess);
						#endif
					}
					
					process[location].timeSpent++;																			//Increment the process's time spent in execution
					
					#ifdef debugMode																						//Debug mode to print statements
						printf("Process %d is being bursted!\n", location);
						printf("Process %d time spent is now %d!\n\n", location, process[location].timeSpent);
					#endif
					
					if((process[location].timeSpent - process[location].taskDuration) == 0){ 								//If time spent equals 0
					
						process[location].flag = 2; 																		//Mark task as completed
						memoryBlock[process[location].blockLocation] += process[location].taskSize; 						//Update the block
						allocatedProcesses--; 																				//Decrement the number of allocated processes
						process[location].finish = rrFinish;																//Set the finish time
						process[location].turnAround = process[location].finish - process[location].arrival;				//Calculate the turn around time
						process[location].relative = (double)process[location].turnAround / process[location].taskDuration;	//Calculate the relative turn around time
						newArrivalTime = process[location].finish;															//Update the new arrival time variable

					
						#ifdef debugMode																					//Debug mode to print statements
							printf("Process %d hit completion!\n", location);
							printf("[Process %d]\n\tArrival:%d\n\tStart:%d\n\tFinish:%d\n\tTurnAround:%d\t\n\tNew Arrival Time:%d\n\n", location,  process[location].arrival, process[location].start,  process[location].finish,  process[location].turnAround, newArrivalTime);
							printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
							printf("Allocated process is now: %d\n", allocatedProcesses);
						#endif		
					
					}
	 
				
        		}
        	
    		}
    	
    	
    		for (int i = 0; i < n; i++){																					//Checking if all tasks have been processed
    	
    			if (process[i].flag == -1){																					//If everything has not been processed, ensure the loop keeps executing
    		
    				allProcessed = false;
    		
    			}
    		
    		}
        
    	} 
    	
	
	}

	#ifdef debugMode																										//Debug mode to print statements
		printf("\nAll remaining tasks are in memory!\n\n");
	#endif
	
	bool memNotEmpty = true;
		
	while (memNotEmpty == true){																							//Process all tasks left in memory
	
		maximum = -2147483640;															
		
		location = -1;
		
		memNotEmpty = false;
		
		if (burstProcess == allocatedProcesses){																			//Used to control quantum = 1 logic
			
			#ifdef debugMode			
				printf("Burst process now equals allocated processes\n");
			#endif
						
			for (int i = 0; i < n; i++){
						
				if (process[i].flag == 1){
							
					process[i].ignore = 0;
							
				}
						
			}
						
			burstProcess = 0;
						
			#ifdef debugMode																								//Debug mode to print statements
				printf("Burst Process is now: %d\n", burstProcess);
			#endif
					
		}
		
		rrFinish++;																											//Variable used to calculate the rrq1 finish time
		rrStart++;
			
		for (int l = n-1; l >= 0; l--)																						//Finds the process in memory with the smallest arrival time and flag = 0
		{

			if (process[l].flag == 1 && process[l].ignore != 1){
						
				#ifdef debugMode
					printf("Considering process %d with ignore flag %d\n", l, process[l].ignore);
				#endif
						
				if (process[l].arrival > maximum)
				{

					maximum = process[l].arrival;
					location = l;
							
				}
									
			}
	
			
		}
		
		if (process[location].quantumStarted == 0){
					
			process[location].start = rrStart;
			process[location].quantumStarted = 1;
						
		}
		
		#ifdef debugMode																									//Debug mode to print statements
			printf("Process %d ignore flag is %d\n", location, process[location].ignore);
		#endif
		
		burstProcess++;																										//Increment burstProcess
		process[location].ignore = 1;																						//Set ignore flag to 1
		
		#ifdef debugMode																									//Debug mode to print statements
			printf("Picked process %d and ignore flag changed to %d\n", location, process[location].ignore);									
		#endif		
					
		if (burstProcess == allocatedProcesses){																			//Used to control quantum = 1 logic
			
			#ifdef debugMode
				printf("Burst process now equals allocated processes\n");
			#endif
						
			for (int i = 0; i < n; i++){
						
				if (process[i].flag == 1){
							
					process[i].ignore = 0;
							
				}
						
			}
						
			burstProcess = 0;
			
			#ifdef debugMode		
				printf("Burst Process is now %d\n", burstProcess);
			#endif
					
		}
					
		process[location].timeSpent++;																						//Increment process's time spent in execution
		
		#ifdef debugMode																									//Debug mode to print statements
			printf("Process %d is being bursted!\n", location);
			printf("Process %d time spent is now: %d!\n\n", location, process[location].timeSpent);
		#endif
					
		if ((location != -1 && process[location].flag == 1) && ((process[location].timeSpent - process[location].taskDuration) == 0)){
			
			process[location].flag = 2; 																					//Mark task as completed
			memoryBlock[process[location].blockLocation] += process[location].taskSize; 									//Update the block
			allocatedProcesses--; 																							//Decrement the number of allocated processes
			process[location].finish = rrFinish;																			//Set the finish time
			process[location].turnAround = process[location].finish - process[location].arrival;							//Calculate the turn around time and relative turn around time
			process[location].relative = (double)process[location].turnAround / process[location].taskDuration;	
			newArrivalTime = process[location].finish;																		//Update the new arrival time variable

			#ifdef debugMode																								//Debug mode to print statements		
				printf("Process %d hit completion!\n", location);
				printf("[Process %d]\n\tArrival:%d\n\tStart:%d\n\tFinish:%d\n\tTurnAround:%d\t\n\tNew Arrival Time:%d\n\n", location,  process[location].arrival, process[location].start,  process[location].finish,  process[location].turnAround, newArrivalTime);
				printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
				printf("Allocated processes is now: %d\n", allocatedProcesses);
			#endif
		}
		
		for (int i = 0; i < n; i++){																						//Checking if all tasks have been processed

    			if (process[i].flag == 1 && location != -1){ 
    				
    				memNotEmpty = true;
    			
    			}
    			
    	
    	}
		
	
	}
		
		
	for (int i = 0; i < n; i++){																							//Find the total turn around time and the total relative turn around time
	
		
		rrResults.totalTurnAroundTime += process[i].turnAround;
		rrResults.relativeTurnAroundTime += process[i].relative;
			
	
	}
	
	
	rrResults.totalTurnAroundTime /= n;																						//Calculate turn around time and relative turn around time
	rrResults.relativeTurnAroundTime /= n;
	
	rrFinalTR += rrResults.totalTurnAroundTime;
	rrFinalRTR += rrResults.relativeTurnAroundTime;
																		
}



/***************************************************************************
* void spn( int memoryBlock[], int m, struct taskStream process[], int n )
* Author: Sujeet Ojha
* Date: 23 April 2020
* Description: processes the taskStream using the firstFit algorithm and stores
			   them into memory, then performs the spn algorithm for scheduling.
			   Calculates the average turn around time and the average relative
			   turn around time for all tasks.
*
* Parameters:
* memoryBlock[] I/P int - Array representing the memory blocks
* process[] I/P struct taskStream - The array of processes filled with random values
* n I/P int - The amount of tasks in the stream
* m I/P int - The amount of blocks in memory
* print O/P void Status code (not currently used)
***************************************************************************/

void spn(int memoryBlock[], int m, struct taskStream process[], int n){
  
  	location = 0;
  	int newArrivalTime = 0;																			
  	bool processed = false;																									//Boolean variable used to control logic statements
	bool allProcessed = false;																								//Boolean variable used to control logic statements
	
	while (allProcessed == false){																							//Keep going until all tasks have been processed
	
		int this;
		
		for (int i = 0; i < n; i++){ 																						//For all processes, perform the first fit algorithm
    		
    		allProcessed = true;
    		processed = false;
    		this = i;
    		
    		if (process[i].flag == -1){																						//Only process if the task's flag is -1
    		
    		 	for (int j = 0; j < m; j++){ 																				//For all the blocks in memory
            		
            		if (memoryBlock[j] >= process[i].taskSize){ 															//If allocation is possible
            			
            			process[i].flag = 1;																				//Change the process flag to 1 (representing allocated)	
						process[i].arrival = newArrivalTime;																//Set the process arrival time							
						memoryBlock[j] -= process[i].taskSize;																//Reduce  block size   
						process[i].blockLocation = j;																		//Update block location in the struct   	 												
					
						processed = true;
						
						#ifdef debugMode																					//Debug mode to print statements
							printf("Success: Placed Process %d in block %d!\n", i, j);
            				printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
						#endif
																
						break; 
            		}
           	 	
        		}
        		
        		int minimum = 2147483640;
        	
        		if (processed == false){																					//If the process was unable to be allocated	
					
					#ifdef debugMode																						//Debug mode to print statements
						printf("Failure: Process %d can't be placed!\n", this);
						printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
					#endif
					
					for (int l = 0; l < n; l++)																				//Finds the process in memory the shortest service time					
					{
						if (process[l].flag == 1){
								
							if (process[l].taskDuration < minimum)
							{

								minimum = process[l].taskDuration;
								location = l;
							
							}
									
						}
	
			
					}
					
					#ifdef debugMode																						//Debug mode to print statements
						printf("Process %d is being submitted!\n", location);
					#endif
					
					process[location].flag = 2; 																			//Mark task as completed
					memoryBlock[process[location].blockLocation] += process[location].taskSize; 							//Update the block
					process[location].start = newArrivalTime;																//Set the start time
					process[location].finish = process[location].start + process[location].taskDuration;					//Set the finish time
					process[location].turnAround = process[location].finish - process[location].arrival;					//Calculate the turn around time
					process[location].relative = (double)process[location].turnAround / process[location].taskDuration;		//Calculate the relative turn around time
					newArrivalTime = process[location].finish;																//Update the new arrival time variable
					
					#ifdef debugMode																						//Debug mode to print statements
						printf("[Process %d]\n\tArrival:%d\n\tStart:%d\n\tFinish:%d\n\tTurnAround:%d\t\n\tNew Arrival Time:%d\n\n", location,  process[location].arrival, process[location].start,  process[location].finish,  process[location].turnAround, newArrivalTime);
						printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
	 				#endif
				
        		}
        	
    		}
    	
    	
    		for (int i = 0; i < n; i++){																					//Checking if all tasks have been processed
    	
    			if (process[i].flag == -1){																					//If everything has not been processed, ensure the loop keeps executing
    		
    				allProcessed = false;
    		
    			}
    			
    	
    		}
    		
        
    	} 
    	
	
	} 

	#ifdef debugMode																										//Debug mode to print statements	
		printf("\nAll remaining tasks are in memory!\n\n");
	#endif
	
	bool memNotEmpty = true;
	
	while (memNotEmpty == true){																							//Process all tasks left in memory
	
		int minimum = 2147483640;
		
		location = -1;
		
		memNotEmpty = false;
		
		
		for (int l = 0; l < n; l++)																							//Find the process in memory with the shortest service time					
		{

			if (process[l].flag == 1){
								
				if (process[l].taskDuration < minimum)
				{

					minimum = process[l].taskDuration;
					location = l;
							
				}
									
			}
	
			
		}
		
		if (location != -1 && process[location].flag == 1){																	//If process is in memory
			
			process[location].flag = 2; 																					//Mark task as completed		
			memoryBlock[process[location].blockLocation] += process[location].taskSize; 									//Update the block
			process[location].start = newArrivalTime;																		//Set the start time															
			process[location].finish = process[location].start + process[location].taskDuration;							//Set the finish time
			process[location].turnAround = process[location].finish - process[location].arrival;							//Calculate the turn around time
			process[location].relative = (double)process[location].turnAround / process[location].taskDuration;				//Calculate the relative turn around time
			newArrivalTime = process[location].finish;																		//Update the new arrival time variable
			
			#ifdef debugMode																								//Debug mode to print statements
				printf("Process %d is being submitted!\n", location);		
				printf("[Process %d]\n\tArrival:%d\n\tStart:%d\n\tFinish:%d\n\tTurnAround:%d\t\n\tNew Arrival Time:%d\n\n", location,  process[location].arrival, process[location].start,  process[location].finish,  process[location].turnAround, newArrivalTime);
				printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
	 		#endif
		}
		
		for (int i = 0; i < n; i++){																						//Checking if all tasks have been processed

    			if (process[i].flag == 1 && location != -1){ 
    				
    				memNotEmpty = true;
    				
    			
    			}
    			
    	
    	}
		
	
	}
		
		
	for (int i = 0; i < n; i++){																							//Find the total turn around time and the total relative turn around time	
	
		
		spnResults.totalTurnAroundTime += process[i].turnAround;
		spnResults.relativeTurnAroundTime += process[i].relative;
			
	
	}	
	
	spnResults.totalTurnAroundTime /= n;																					//Calculate turn around time and relative turn around time
	spnResults.relativeTurnAroundTime /= n;
	
	spnFinalTR += spnResults.totalTurnAroundTime;
	spnFinalRTR += spnResults.relativeTurnAroundTime;
																		
}

/***************************************************************************
* void fcfs( int memoryBlock[], int m, struct taskStream process[], int n )
* Author: Sujeet Ojha
* Date: 23 April 2020
* Description: processes the taskStream using the firstFit algorithm and stores
			   them into memory, then performs the fcfs algorithm for scheduling.
			   Calculates the average turn around time and the average relative
			   turn around time for all tasks.
*
* Parameters:
* memoryBlock[] I/P int - Array representing the memory blocks
* process[] I/P struct taskStream - The array of processes filled with random values
* n I/P int - The amount of tasks in the stream
* m I/P int - The amount of blocks in memory
* print O/P void Status code (not currently used)
***************************************************************************/

void fcfs(int memoryBlock[], int m, struct taskStream process[], int n){
  
  	location = 0;
  	int newArrivalTime = 0;
  	bool processed = false;																									//Boolean variable used to control logic statements
	bool allProcessed = false;																								//Boolean variable used to control logic statements
	
	while (allProcessed == false){																							//Keep going until all tasks have been processed
	
		int this;
		
		for (int i = 0; i < n; i++){ 																						//For all processes, perform the first fit algorithm
    		
    		this = i;
    		allProcessed = true;
    		processed = false;
    		
    		if (process[i].flag == -1){																						//Only process if the task's flag is -1
    		
    		 	for (int j = 0; j < m; j++){ 																				//For all the blocks in memory
            		
            		if (memoryBlock[j] >= process[i].taskSize){ 															//If allocation is possible
            			
            			process[i].flag = 1;																				//Change the process flag to 1 (representing allocated)						
						process[i].arrival = newArrivalTime;																//Set the process arrival time							
						memoryBlock[j] -= process[i].taskSize;																//Reduce  block size  
						process[i].blockLocation = j;																		//Update block location in the struct   	 											
						processed = true;
						
						#ifdef debugMode																					//Debug mode to print statements
							printf("Success: Placed Process %d in block %d!\n", i, j);
            				printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
						#endif
																
						break; 
            		}
           	 	
        		}
        		
        		int minimum = 2147483640;
        		
        		if (processed == false){																					//If the process was unable to be allocated	
					
					#ifdef debugMode																						//Debug mode to print statements
						printf("Failure: Process %d can't be placed!\n", this);
						printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
					#endif
					
					for (int l = 0; l < n; l++)																				//Finds the process in memory the smallest arrival time				
					{

						if (process[l].flag == 1){
								
							if (process[l].arrival < minimum)
							{

								minimum = process[l].arrival;
								location = l;
							
							}
									
						}
	
			
					}
				
					#ifdef debugMode																						//Debug mode to print statements
						printf("Process %d is being submitted!\n", location);
					#endif
					
					process[location].flag = 2; 																			//Mark task as completed
					memoryBlock[process[location].blockLocation] += process[location].taskSize; 							//Update the block
					process[location].start = newArrivalTime;																//Set the start time
					process[location].finish = process[location].start + process[location].taskDuration;
					process[location].turnAround = process[location].finish - process[location].arrival;
					process[location].relative = (double)process[location].turnAround / process[location].taskDuration;
					newArrivalTime = process[location].finish;
					
					#ifdef debugMode
						printf("[Process %d]\n\tArrival:%d\n\tStart:%d\n\tFinish:%d\n\tTurnAround:%d\t\n\tNew Arrival Time:%d\n\n", location,  process[location].arrival, process[location].start,  process[location].finish,  process[location].turnAround, newArrivalTime);
						printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
					#endif
					
        		}
        	
    		}
    	
    	
    		for (int i = 0; i < n; i++){																					//Checking if all tasks have been processed
    	
    			if (process[i].flag == -1){																					//If everything has not been processed, ensure the loop keeps executing
    		
    				allProcessed = false;
    		
    			}
    			    		
    	
    		}

    	} 
	
	}
	
	#ifdef debugMode																										//Debug mode to print statements	
		printf("\nAll remaining tasks are in memory!\n\n");
	#endif
	
	bool memNotEmpty = true;
	
	while (memNotEmpty == true){																							//Process all tasks left in memory
	
		int minimum = 2147483640;
		location = -1;
		memNotEmpty = false;
		
		//Find the one that arrived first
		
		for (int l = 0; l < n; l++)																							//Find the process in memory with the smallest arrival time
		{

			if (process[l].flag == 1){
								
				if (process[l].arrival < minimum)
				{

					minimum = process[l].arrival;
					location = l;
							
				}
									
			}
	
			
		}
		
		if (location != -1 && process[location].flag == 1){																	//If process is in memory
			
			#ifdef debugMode																								//Debug mode to print statements												
				printf("Process %d is being submitted!\n", location);
			#endif
			
			process[location].flag = 2; 																					//Mark task as completed		
			memoryBlock[process[location].blockLocation] += process[location].taskSize; 									//Update the block
			process[location].start = newArrivalTime;																		//Set the start time		
			process[location].finish = process[location].start + process[location].taskDuration;							//Set the finish time
			process[location].turnAround = process[location].finish - process[location].arrival;							//Calculate the turn around time
			process[location].relative = (double)process[location].turnAround / process[location].taskDuration;				//Calculate the relative turn around time
			newArrivalTime = process[location].finish;																		//Update the new arrival time variable
					
			#ifdef debugMode																								//Debug mode to print statements										
				printf("[Process %d]\n\tArrival:%d\n\tStart:%d\n\tFinish:%d\n\tTurnAround:%d\t\n\tNew Arrival Time:%d\n\n", location,  process[location].arrival, process[location].start,  process[location].finish,  process[location].turnAround, newArrivalTime);
				printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", memoryBlock[0], memoryBlock[1],memoryBlock[2],memoryBlock[3],memoryBlock[4]);
	 		#endif
		}
		
		
		for (int i = 0; i < n; i++){																						//Checking if all tasks have been processed

    			
    			if (process[i].flag == 1 && location != -1){ //test
    				
    				memNotEmpty = true;
    			
    			}
    			
    	
    	}

	
	}
	
	for (int i = 0; i < n; i++){																							//Find the total turn around time and the total relative turn around time
	
		
		fcfsResults.totalTurnAroundTime += process[i].turnAround;
		fcfsResults.relativeTurnAroundTime += process[i].relative;
			
	
	}
	
	fcfsResults.totalTurnAroundTime /= n;																					//Calculate turn around time and relative turn around time
	fcfsResults.relativeTurnAroundTime /= n;
	
	fcfsFinalTR += fcfsResults.totalTurnAroundTime;
	fcfsFinalRTR += fcfsResults.relativeTurnAroundTime;	
																		

}

/***************************************************************************
* void print( int n, struct taskStream process[] )
* Author: Sujeet Ojha
* Date: 23 April 2020
* Description: displays process and memory data
*
* Parameters:
* n I/P int - The amount of tasks in the stream
* process[] I/P struct taskStream - The array of processes filled with random values
* print O/P void Status code (not currently used)
***************************************************************************/

void print(int n, struct taskStream process[]){																				//Print function used to visualize the memory allocations


    printf("\nProcess#\tTask Size\tBlock#\t\tDur.\t\tArr.\t\tStart\t\tFinish\t\tTurn Ar.\tRelative Trn.\n");
     
    for (int i = 0; i < n; i++) 
    { 
        printf(" %d\t\t%d\t\t", i, process[i].taskSize);
        
        if (process[i].flag != -1){																							//If the process has been allocated, print out the block it is allocated in			
            printf("%d", process[i].blockLocation+1);
        } 
        else{																												//If the process has not been allocated, print out X
        
            printf("X");
             
        }
        
        printf("\t\t%d", process[i].taskDuration);																			//Print related process contents
        printf("\t\t%d", process[i].arrival);
        printf("\t\t%d", process[i].start);
        printf("\t\t%d", process[i].finish);
        printf("\t\t%d", process[i].turnAround);
        printf("\t\t%0.2f", process[i].relative);
        printf("\n"); 
    } 
    
    printf("\n");


}

/***************************************************************************
* int main( int argc, char *argv[] )
* Author: Sujeet Ojha
* Date: 23 April 2020
* Description: Initializes the task stream by calling the randomize() function
			   and call the four process scheduling algorithms a thousand times
			   in order to generate results regarding the average turnaround time
			   and the average relative turnaround time.
			   The results regarding these calculations are displayed at the end.
* Parameters:
* argc I/P int The number of arguments on the command line
* argv I/P char *[] The arguments on the command line
* main O/P int Status code (not currently used)
**************************************************************************/
 
int main() 
{
	
	int fcfsBlock[5];																										//Represents memory used for FCFS sequence
	int spnBlock[5];																										//Represents memory used for SPN sequence
	int rrBlock[5];																											//Represents memory used for RR sequence
	int srtBlock[5];																										//Represents memory used for SRT sequence
	int numTasks = 1000;																									//Total number of tasks
	int n = numTasks;
	int m = 5;																												//Number of blocks in memory
	int iterations = 1000;
		
	#ifdef debugMode																										//If in debug mode, only iterate once with 20 tasks
		iterations = 1;
		numTasks = 20;
		n = numTasks;
	#endif
	
	struct taskStream fcfsTasks[numTasks];																					//Creating an array of task structures
	struct taskStream spnTasks[numTasks];
	struct taskStream rrTasks[numTasks];
	struct taskStream srtTasks[numTasks];   
																															
	for (int i = 0; i < iterations; i++){																					//Process all tasks for given iterations
		
		fcfsResults.totalTurnAroundTime = 0;																				//Initializing turn around time and relative turn around time values as 0
		fcfsResults.relativeTurnAroundTime = 0;
	
		spnResults.totalTurnAroundTime = 0;
		spnResults.relativeTurnAroundTime = 0;
	
		rrResults.totalTurnAroundTime = 0;
		rrResults.relativeTurnAroundTime = 0;
	
		srtResults.totalTurnAroundTime = 0;
		srtResults.relativeTurnAroundTime = 0;

		randomize(fcfsTasks, fcfsBlock, numTasks);																			//Randomizing all tasks
		randomize(spnTasks, spnBlock, numTasks);
		randomize(rrTasks, rrBlock, numTasks);
		randomize(srtTasks, srtBlock, numTasks);
		
		for (int i = 0; i < numTasks; i++){																					//Ensuring that all algorithms get the same set of task (randomized every iteration)
		
			srtTasks[i].taskSize = spnTasks[i].taskSize;
			srtTasks[i].taskDuration = spnTasks[i].taskDuration;
			
			rrTasks[i].taskSize = spnTasks[i].taskSize;
			rrTasks[i].taskDuration = spnTasks[i].taskDuration;
			
			fcfsTasks[i].taskSize = spnTasks[i].taskSize;
			fcfsTasks[i].taskDuration = spnTasks[i].taskDuration;
		
		}
	
	
		printf("Iteration [%d]\n", i);
		
		#ifdef debugMode																									//Debug mode to print statements
			printf("-------- FCFS --------\n");
			printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", fcfsBlock[0], fcfsBlock[1], fcfsBlock[2], fcfsBlock[3], fcfsBlock[4]);
		#endif
	
		fcfs(fcfsBlock, m, fcfsTasks, n);																					//Calling FCFS function
		
	
		#ifdef debugMode																									//Debug mode to print statements
			printf("\n\n-------- SPN --------\n");
			printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", spnBlock[0], spnBlock[1], spnBlock[2], spnBlock[3], spnBlock[4]);												
		#endif
	
		spn(spnBlock, m, spnTasks, n);																						//Calling SPN function
	
		#ifdef debugMode																									//Debug mode to print statements
			printf("\n\n-------- RRQ1 --------\n");
			printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", rrBlock[0], rrBlock[1], rrBlock[2], rrBlock[3], rrBlock[4]);												
		#endif
	
		rrq1(rrBlock, m, rrTasks, n);																						//Calling RR function
	
		#ifdef debugMode																									//Debug mode to print statements
			printf("\n\n-------- SRT --------\n");
			printf("Block Sizes: [%d] [%d] [%d] [%d] [%d]\n\n", srtBlock[0], srtBlock[1], srtBlock[2], srtBlock[3], srtBlock[4]);												
		#endif
	
		srt(srtBlock, m, srtTasks, n);																						//Calling SRT function
	
	
	}																			
	
	#ifdef debugMode																										//Debug mode to print statements
	
			printf("\n\n-------- FCFS Processes --------\n");															
			print(n, fcfsTasks);															
			
			printf("\n\n-------- SPN Processes --------\n");
			print(n, spnTasks);
			
			printf("\n\n-------- RRQ1 Processes --------\n");
			print(n, rrTasks);
			
			printf("\n\n--------  SRT Processes --------\n");
			print(n, srtTasks);
	#endif	
		
	printf("\n\n-------- Results --------\n");																				//Displaying the results

	printf("FCFS Average Turn Around Time: %0.3f\n", fcfsFinalTR / iterations);	
	printf("FCFS Average Relative Turn Around Time: %0.3f\n\n", fcfsFinalRTR / iterations);
	
	printf("SPN Average Turn Around Time: %0.3f\n", spnFinalTR / iterations);	
	printf("SPN Average Relative Turn Around Time: %0.3f\n\n", spnFinalRTR / iterations);
	
	printf("RRQ1 Average Turn Around Time: %0.3f\n", rrFinalTR / iterations);	
	printf("RRQ1 Average Relative Turn Around Time: %0.3f\n\n", rrFinalRTR / iterations);
	
	printf("SRT Average Turn Around Time: %0.3f\n", srtFinalTR / iterations);	
	printf("SRT Average Relative Turn Around Time: %0.3f\n\n", srtFinalRTR / iterations);
	
}
