#!/bin/tcsh
#SBATCH -t 00:00:15
#SBATCH --exclusive
#SBATCH --mem-per-cpu=12288
#SBATCH --mail-user=chiragm@terpmail.umd.edu
#SBATCH --mail-type=END
#SBATCH --output=life_seq_opt.final.out.1.500.500.500
#SBATCH -p debug

module unload intel
module load openmpi/gnu

time life_seq_opt final.data 500 500 500
echo "Seq Num tasks 1, 1 node, debug node 500 Gens 500 x 500"
