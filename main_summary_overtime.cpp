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

long long get_time() {
	auto now = std::chrono::high_resolution_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto epoch = now_ms.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
}
int main(int argc, char** argv) {

	MPI_Init(NULL, NULL);

	int rounds = 20;
	int sleep_dur = 30;
	if (argc >= 3) {
		//std::cout << "[" << argc << "]args = " << argv[0] << ", " << argv[1] << ", " << argv[2] << std::endl;
		rounds = atoi(argv[1]);
		sleep_dur = atoi(argv[2]);
	}

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	char hostname[1024];
	gethostname(hostname, 1024);
	std::cout << "[" << world_rank << " of " << world_size << "] hostname = "
			<< hostname << std::endl;

	int count = 0;
	while (++count <= rounds) {
		long long* recv_buf = NULL;
		if (world_rank == 0) {
			recv_buf = (long long *) malloc(sizeof(long long) * world_size);
		}

		int rc = MPI_Barrier(MPI_COMM_WORLD);
		assert(rc == MPI_SUCCESS);
		long long local_time = get_time(); // - global_local_time;

		MPI_Gather(&local_time, 1, MPI_LONG_LONG, recv_buf, 1, MPI_LONG_LONG, 0,
				MPI_COMM_WORLD);

		if (world_rank == 0) {
			/*for (int i=0 ; i<world_size ; i++){
			 std::cout << "[" << count <<"][0," << i <<"] " << ((local_time-recv_buf[i])/1000.0) << "ms" << std::endl;
			 }*/
			std::vector<long long> nums;
			for (int i = 0; i < world_size; i++) {
				//std::cout <<"[" << i <<"] t = " << recv_buf[i] << std::endl;
				nums.push_back(recv_buf[i]);
			}
			std::vector<long long> sorted(nums);
			std::sort(sorted.begin(), sorted.end());

			auto timenow = std::chrono::system_clock::to_time_t(
					std::chrono::system_clock::now());
			std::cout << std::ctime(&timenow) << "\t\t\t Min rank is "
					<< std::distance(nums.begin(),
							std::find(nums.begin(), nums.end(), sorted[0]))
					<< " and max rank is "
					<< std::distance(nums.begin(),
							std::find(nums.begin(), nums.end(),
									sorted[sorted.size() - 1]))
					<< " and the diff between max and min is "
					<< (sorted[sorted.size() - 1] - sorted[0]) / 1000.0
					<< " ms " << std::endl;
		}
		// Clean up
		if (world_rank == 0) {
			free(recv_buf);
		}
		sleep(sleep_dur);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}

