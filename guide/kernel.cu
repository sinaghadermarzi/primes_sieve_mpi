#include <iostream>
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <ctime>

#define BLOCK_SIZE 256

__global__ void Sieve(bool * d_inout, int Prime){
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	
	/*          With Divergence          */
	if(((2*tid+1)%Prime) == 0){
		d_inout[tid] = 1;
	}
	/**/
	/*      Without Divergence          *//*
	d_inout[tid] = !(!(1 & (((2*tid+1)%Prime)==0)) & !(1 & d_inout[tid]));
	/**/
}

int main(){
	bool * h_inout, * d_inout;

	//-------------- Taking the range from User --------------//
	int range = 0;
	std::cin >> range;
	int r = sqrt((double)range);

	//--------- Device launch configuration ---------//* Calculating it here to eleminate the time it takes from profiling as well as using its data for memory padding */
	int block_size = BLOCK_SIZE;
	int num_blocks = range/block_size + ( range % block_size == 0 ? 0 : 1);
	dim3 globalDim(num_blocks/2 + num_blocks%2);
	dim3 blockDim(block_size);
	
	// ------ Start of Timing -------- //
	clock_t t = clock();

	//-------------- Alocating and Initializing memory in host & device ------------//
	cudaHostAlloc(&h_inout,sizeof(bool)*num_blocks*BLOCK_SIZE/2,cudaHostAllocDefault);
	cudaMalloc(&d_inout,num_blocks*BLOCK_SIZE*sizeof(bool)/2);
	cudaMemcpy(d_inout,h_inout,range*sizeof(bool)/2,cudaMemcpyHostToDevice);

	//-------------- Finding all prime numbers between 2 and sqrt(range) -------------//
	bool * Integers = (bool *) calloc(sizeof(bool),r/2);
	int * Primes = (int *) malloc(sizeof(int)*r);
	int num_primes = 1;
	for(int i = 3; i*i < r; i+=2){
		if(Integers[i/2] == 0){
			for(int j = 3*i; j < r; j+=2*i){
				Integers[j/2] = 1;
			}
		}
	}
	
	// ------- Counting the nmber of primes below sqrt(range) ----------//
	for(int i = 3; i < r; i+=2)
		if(Integers[i/2] == 0)
			num_primes += 1;

	Primes = (int *) malloc(sizeof(int)*num_primes);
	Primes[0] = 2;
	for(int i = 3, j = 1; i < r; i+=2){
		if(Integers[i/2] == 0){
			Primes[j++] = i;
		}
	}

	for(int i = 1; i < num_primes ; i++){
		Sieve<<<globalDim,blockDim>>>(d_inout,Primes[i]);
	}

	cudaMemcpy(h_inout,d_inout,range*sizeof(bool)/2,cudaMemcpyDeviceToHost);

	// ------ End of Timing ------//
	cudaThreadSynchronize();
	t = clock() - t;

	int count = num_primes;
	for(int i = 3; i < range; i+=2){
		if(h_inout[i/2] == 0){
			count += 1;
		}
	}

	std::cout << count << " ,Primes in " << ((float)t)/CLOCKS_PER_SEC << " Second(s)." << std::endl;

	free(Integers);
	free(Primes);
	cudaFreeHost(h_inout);
	cudaFree(d_inout);

	return EXIT_SUCCESS;
}