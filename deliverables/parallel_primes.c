#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

void serial_sieve(int N , int Mode); // Mode: 0= NoPrint , 1= Print , 2= Speedup 


int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    
    double p_diff,s_diff ;     

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the name of the processor
    //~ char processor_name[MPI_MAX_PROCESSOR_NAME];
    //~ int name_len;
    //~ MPI_Get_processor_name(processor_name, &name_len);
	  MPI_Barrier(MPI_COMM_WORLD);
	int N,Mode;  
	if (rank == 0)
	{	
		if (argc != 3) 
		{
			printf("Please supply a range and Mode (0= NoPrint , 1= Print , 2= Speedup) .\n");
			N= 128;
			Mode = 1;	
			//~ exit(1);
		}
		else
		{
			N = atoi(argv[1]);
			Mode = atoi(argv[2]);
		}
	}
	int *list1; /* The list of numbers <= sqrtN -- if
	list1[x] equals 1, then x is
	marked. If list1[x] equals 0, then x is
	unmarked. */
	int *list2; /* The list of numbers > sqrtN – if
	list2[x-L] equals 1, then x is marked.
	If list2[x-L] equals 0, then x is
	unmarked. */
	

	
	int S = 0; /* A near-as-possible even split of the
	count of numbers above sqrtN */
	int R = 0; /* The remainder of the near-as-possible
	even split */
	int L = 0; /* The lowest number in the current
	process’s split */
	int H = 0; /* The highest number in the current
	process’s split */
	int r = 0; /* The rank of the current process */
	int p = 0; /* The total number of processes */
	
	
	p_diff= -MPI_Wtime();

	MPI_Bcast(&N,1,MPI_INT,0,MPI_COMM_WORLD);

	
	
	int sqrtN = sqrt(N);


	r= rank;
	p= world_size;
	
	S = (N-(sqrtN+1)) / p;
	R = (N-(sqrtN+1)) % p;
	L = sqrtN + r*S + 1;
	H = L+S-1;
	
	if(r == p-1) 
	{
		H += R;
	}
		
	
	/* Allocate memory for lists */
	list1 = (int*)malloc((sqrtN+1) * sizeof(int));
	if (r==0)
	{
		list2 = (int*)malloc((H+R-L+1) * sizeof(int));
	}
	else
	{
		list2 = (int*)malloc((H-L+1) * sizeof(int));
	}
		/* Exit if malloc failed */
	if(list1 == NULL) 
	{
		fprintf(stderr, "P_Error: Failed to allocate memory for list.\n");
		//exit(-1);
	}
	/* Exit if malloc failed */
	if(list2 == NULL) 
	{
		fprintf(stderr, "P_Error: Failed to allocate memory for list.\n");
		//exit(-1);
	}
	
	
	
	int c,m;
	
	/* Run through each number in list1 */
	for(c = 2; c <= sqrtN; c++) 
	{
		/* Set each number as unmarked */
		list1[c] = 0;
	}

	/* Run through each number in list2 */
	for(c = L; c <= H; c++) 
	{
		/* Set each number as unmarked */
		list2[c-L] = 0;
	}
	
	/* Run through each number in list1 */
	for(c = 2; c <= sqrtN; c++) 
	{
		/* If the number is unmarked */
		if(list1[c] == 0)
		{
			/* Run through each number bigger than c in
			list1 */
			for(m = c+1; m <= sqrtN; m++)
			{
				/* If m is a multiple of c */
				if(m%c == 0) 
				{
					/* Mark m */
					list1[m] = 1;
				}
			}
			/* Run through each number bigger than c in
			list2 */
			for(m = L; m <= H; m++)
			{
				/* If m is a multiple of C */
				if(m%c == 0)
				{
					/* Mark m */
					list2[m-L] = 1;
				}
			}
		}
	}
	/* If Rank 0 is the current process */
	if(rank == 0) 
	{
		/* Run through each of the numbers in list1 */
		
		if(Mode == 1)  //print
		{
		
			printf("\r\n Parallel Result : \n");	
		
			for(c = 2; c <= sqrtN; c++) 
			{
				/* If the number is unmarked */
				if(list1[c] == 0) 
				{
					/* The number is prime, print it */
					printf("%d ", c);
				}
			}
		
				/* Run through each of the numbers in list2 */
			for(c = L; c <= H; c++) 
			{
				/* If the number is unmarked */
				if(list2[c-L] == 0) 
				{
					/* The number is prime, print it */
					printf("%d ", c);
				}
			}
		
		}
		
		/* Run through each of the other processes */
		for(r = 1; r <= p-1; r++) 
		{
			/* Calculate L and H for r */
			L = sqrtN + r*S + 1 ;
			H = L+S-1;
			if(r == p-1) 
			{
				H += R;
			}
			/* Receive list2 from the process */
			MPI_Recv(list2, H-L+1, MPI_INT, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);  
			
			
			/* Run through the list2 that was just
			received */
			if(Mode == 1)  //print
			{
				for(c = L; c <= H; c++) 
				{
					/* If the number is unmarked */
					if(list2[c-L] == 0) 
					{
						/* The number is prime, print it */
						printf("%d ", c);
					}
				}
			}	
		}
		
		p_diff += MPI_Wtime();

	}
	else /* If the process is not Rank 0 */
	{
		/* Send list2 to Rank 0 */
		MPI_Send(list2, H-L+1, MPI_INT, 0, 0,MPI_COMM_WORLD);
	}
	/* Deallocate memory for list */
	free(list2);
	free(list1);	
	
	
	if(rank == 0) 
	{
		s_diff = -MPI_Wtime();
		
		serial_sieve(N , Mode);
		
		s_diff += MPI_Wtime();
		
		
		if(Mode == 2)
		{
			printf("\n parallel time : %f \n" , p_diff );
			printf("serial time : %f \n" , s_diff );
			printf("speedup : %f \n" , s_diff/p_diff);
		}
		 
	}
	
		
    // Finalize the MPI environment.
    MPI_Finalize();
    
 
}



void serial_sieve(int N , int Mode) // Mode: 0= NoPrint , 1= Print , 2= Speedup 
{
	int sqrtN = sqrt(N);
	
	/* Allocate memory for list */
	int* list = (int*)malloc(N * sizeof(int));
	
	
	/* Exit if malloc failed */
	if(list == NULL) 
	{
		fprintf(stderr, "S_Error: Failed to allocate memory for list.\n");
		//exit(-1);
	}
	
	
	

	/* Run through each number in the list */
	int c=0;
	for(c = 2; c <= N-1; c++) 
	{
		/* Set each number as unmarked */
		list[c] = 0;
	}
	
  
	/* Run through each number up through the square root
	of N */
	for(c = 2; c <= sqrtN; c++) 
	{	  
		/* If the number is unmarked */
		if(list[c] == 0) 
		{

			/* Run through each number bigger than c
			*/
			int m=0;
			for(m = c+1; m <= N-1; m++) 
			{
				/* If m is a multiple of c */
				if(m%c == 0) 
				{	
					/* Mark m */
					list[m] = 1; 
				}
			}	
		}
	}
	
	
	if(Mode == 1)  //print
	{
		printf("\r\n\n serial Result : \n");
		
		/* Run through each number in the list */
		for	(c = 2; c <= N-1; c++) 
		{
			/* If the number is unmarked */
			if(list[c] == 0) 
			{
				/* The number is prime, print it */
				printf("%d ", c);
			}
		}
		printf("\n");
	}
	
	/* Deallocate memory for list */
	free(list);
	
	
}







































































