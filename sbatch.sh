#!/bin/bash

#MSUB -e job$MOAB_JOBID.err
#MSUB -o job$MOAB_JOBID.out
#MSUB -l walltime=00:10:00
#MSUB -l feature='c0-0c1'

module sw PrgEnv-cray PrgEnv-gnu
#module load cdt/16.11

# hmpf
export CPATH=/home/b/bemsalem/konrad/boost_1_62_0/bin/include:$CPATH
export LD_LIBRARY_PATH=/home/b/bemsalem/konrad/boost_1_62_0/bin/lib:$LD_LIBRARY_PATH
export LIBRARY_PATH=/home/b/bemsalem/konrad/boost_1_62_0/bin/lib:$LIBRARY_PATH

CC -Wall -std=c++1z -Iinclude/ main.cpp

#ldd ./a.out

FIRSTID=`head -n1 $PBS_NODEFILE`
SECONDID=`tail -n1 $PBS_NODEFILE`

FIRSTHOSTNAME=`printf "nid%05d" $FIRSTID`
SECONDHOSTNAME=`printf "nid%05d" $SECONDID`

FIRST=`grep $FIRSTHOSTNAME /etc/hosts | awk '{print $1}'`
SECOND=`grep $SECONDHOSTNAME /etc/hosts | awk '{print $1}'`

export FIRST
export SECOND

echo $FIRST $SECOND


aprun -N1 -n$PBS_NUM_NODES ./a.out