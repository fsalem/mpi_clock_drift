/*
 * main.cpp
 *
 *  Created on: Nov 3, 2017
 *      Author: farouk
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>
#include <chrono>
#include <vector>
#include <algorithm>
#include <unistd.h>

long long get_time(){
	auto now = std::chrono::high_resolution_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto epoch = now_ms.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
}

void print_offset_changes(long long** pair_offset, std::vector<long long> nums, int world_rank, int world_size, int round){
	long long diff;
	for (int i=0 ; i<world_size ; i++){
		for (int j=i+1 ; j<world_size ; j++){
			diff = nums[i] - nums[j];
			if (pair_offset[i][j] == NULL) pair_offset[i][j] = diff;
			if (pair_offset[i][j] != diff){
				std::cout << "[" << world_rank << "][" << round << "] [" << i << "][" << j << "] diff = " << pair_offset[i][j] - diff << std::endl;
				pair_offset[i][j] = diff;
			}

		}
	}
}

int main(int argc, char** argv) {

	MPI_Init(NULL, NULL);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	char hostname[1024];
	gethostname(hostname, 1024);
	std::cout << "[" << world_rank << " of "<< world_size <<"] hostname = " << hostname << std::endl;

	long long** pair_offset = NULL;
	if (world_rank == 0) {
		pair_offset = (long long **) malloc(sizeof(long long*) * world_size);
		for (int i=0 ; i<world_size ; i++){
			pair_offset[i] = (long long *) malloc(sizeof(long long) * world_size);
			for (int j=0 ; j<world_size ; j++) pair_offset[i][j] = NULL;
		}
	}

	int rc = MPI_Barrier(MPI_COMM_WORLD);
	assert(rc == MPI_SUCCESS);
	long long global_local_time = get_time();

	int count = 0;
	while (++count <= 10){
		long long* recv_buf = NULL;
		if (world_rank == 0) {
			recv_buf = (long long *) malloc(sizeof(long long) * world_size);
		}

		rc = MPI_Barrier(MPI_COMM_WORLD);
		assert(rc == MPI_SUCCESS);
		long long local_time = get_time() - global_local_time;

		MPI_Gather(&local_time, 1, MPI_LONG_LONG, recv_buf, 1, MPI_LONG_LONG, 0,MPI_COMM_WORLD);

		if (world_rank == 0) {

			std::vector<long long> nums;
			for (int i=0 ; i<world_size ; i++){
				//std::cout <<"[" << i <<"] t = " << recv_buf[i] << std::endl;
				nums.push_back(recv_buf[i]);
			}
			std::vector<long long> sorted (nums);
			std::sort(sorted.begin(), sorted.end());

			std::cout << "[" << world_rank << "][" << count << "] Min index is " << std::distance(nums.begin(), std::find(nums.begin(), nums.end(), sorted[0])) << " in ms and max time is " << std::distance(nums.begin(), std::find(nums.begin(), nums.end(), sorted[sorted.size()-1]))<< " in ms and the diff between max and min is " << (sorted[sorted.size()-1] - sorted[0])/1000.0 << " s "<< std::endl;

			print_offset_changes(pair_offset, nums, world_rank, world_size, count);

		}
		// Clean up
		if (world_rank == 0) {
			free(recv_buf);
		}
		sleep(60);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
