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
#include <fstream>

long long get_time(){
	auto now = std::chrono::high_resolution_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto epoch = now_ms.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
}
int main(int argc, char** argv) {

	MPI_Init(NULL, NULL);

	const int SLEEP_DUR=10, INTERATIONS=120;
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	char hostname[1024];
	gethostname(hostname, 1024);
	//if (world_rank == 0)
		std::cout << "[" << world_rank << " of "<< world_size <<"] hostname " << hostname << std::endl;

	std::vector<uint64_t> times;

	int rc = MPI_Barrier(MPI_COMM_WORLD);
	assert(rc == MPI_SUCCESS);
	long long global_local_time = get_time();
	times.push_back(global_local_time);
	times.push_back(0);

	int count = 0;
	while (++count <= INTERATIONS){
		sleep(SLEEP_DUR);
		rc = MPI_Barrier(MPI_COMM_WORLD);
		assert(rc == MPI_SUCCESS);
		long long local_time = get_time() - global_local_time;
		times.push_back(local_time);
	}
	if (world_rank == 0)std::cout << "Logging ...\n";

	std::ofstream out(std::to_string(world_rank)+".txt");

	out << world_rank << "/" << world_size;
	for (size_t i=0 ; i<times.size() ; i++){
		out << "\t\t" << times[i];
	}
	out << "\n";
	out.flush();
	out.close();

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
