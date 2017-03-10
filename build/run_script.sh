#!/bin/tcsh
#SBATCH -t 00:00:15
#SBATCH --exclusive
#SBATCH --mem-per-cpu=12288
#SBATCH --mail-user=chiragm@terpmail.umd.edu
#SBATCH --mail-type=END
#SBATCH --output=life_2d.final.out.8.500.500.500
#SBATCH -p debug

module unload intel
module load openmpi/gnu

time mpirun -np 8 life_mpi_2d final.data 500 500 500
echo "2D Num tasks 8, 1 node, debug node 500 Gens 500 x 500"
