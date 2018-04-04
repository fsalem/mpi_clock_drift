#!/bin/bash

##BATCH -A <account>
#SBATCH --exclusive
#SBATCH --time=00:30:00
#SBATCH --error=job.%J.err
#SBATCH --output=job.%J.out
#SBATCH --partition=microFLES
##SBATCH -C c0-0c1

# hmpf
export CPATH=/home/b/bemsalem/konrad/boost_1_62_0/bin/include:$CPATH
export LD_LIBRARY_PATH=/home/b/bemsalem/konrad/boost_1_62_0/bin/lib:$LD_LIBRARY_PATH
export LIBRARY_PATH=/home/b/bemsalem/konrad/boost_1_62_0/bin/lib:$LIBRARY_PATH

mpiCC -Wall -std=c++1z -Iinclude/ main.cpp

#ldd ./a.out

#FIRSTID=`head -n1 $PBS_NODEFILE`
#SECONDID=`tail -n1 $PBS_NODEFILE`
  

#FIRSTHOSTNAME=`printf "nid%05d" $FIRSTID`
#SECONDHOSTNAME=`printf "nid%05d" $SECONDID`

#FIRST=`grep $FIRSTHOSTNAME /etc/hosts | awk '{print $1}'`
#SECOND=`grep $SECONDHOSTNAME /etc/hosts | awk '{print $1}'`

#export FIRST
#export SECOND

#echo $FIRST $SECOND


srun -N1 -n$PBS_NUM_NODES ./a.out