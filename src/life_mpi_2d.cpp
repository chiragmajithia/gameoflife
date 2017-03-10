#include <mpi.h>
#include "board.cpp"
#include "partition2D.cpp"
#include <string>
#include <cstdlib>

void confgMPI();
void initMPI(int argc, char *argv[]);
void seqDispPartitions(Partition2D &p);
void seqSavePartitions(Partition2D &p);
void seqOutPartitions(Partition2D &p);
void seqDispCols(Partition2D &p);
void transferGhosts(Partition2D &p);


void confgMPI(int argc, char *argv[])
{
	int num_process = -1;
	MPI_Init(&argc, &argv); 
	MPI_Comm_size(MPI_COMM_WORLD, &num_process);
}

void initMPI(int argc, char *argv[]) 
{

	int my_rank;
  // MPI initializations 
	confgMPI(argc,argv);
	Partition2D p(argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if(my_rank == 0)
	{
		Board b("../OutPat2D/");		 
		if(!b.parseArguments(argc, argv))
		{ 
			MPI_Finalize();
			exit(0);
		}
        int cnt = (b.indx.size());
		MPI_Bcast(&cnt,1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&b.indx[0],b.indx.size(), MPI_INT, 0, MPI_COMM_WORLD);
		p.populatePartition(b.indx);
        //cout << "Get Partition = " << p.getPartition(23,23) << endl;
        //seqDispPartitions(p);
	}
	else
	{

		int cnt = 0;
		MPI_Bcast(&cnt,1, MPI_INT, 0, MPI_COMM_WORLD);
		vector<int> indx(cnt);
		MPI_Bcast(&indx[0],cnt, MPI_INT, 0, MPI_COMM_WORLD);
		p.populatePartition(indx);
		//MPI_Barrier(MPI_COMM_WORLD);
        //double time_start = MPI_Wtime();
        //std::cout << "Hello World "<< argc << "my rank is " << my_rank <<" "<< MPI_Wtime() - time_start << std::endl; 
		
	}


   for(int i = 0; i < p.Gens; i++)
   {
   		MPI_Barrier(MPI_COMM_WORLD);
   		transferGhosts(p);
   		p.simGen();
       	p.swap();
       	MPI_Barrier(MPI_COMM_WORLD);
   }    
   
   MPI_Barrier(MPI_COMM_WORLD);
   seqOutPartitions(p);
   seqSavePartitions(p);
   MPI_Finalize (); 
}

void transferGhosts(Partition2D &p)
{
   p.sendToNorth();
   p.sendToSouth();
   p.sendToEast();
   p.sendToWest();
   p.receiveFromNorth();
   p.receiveFromSouth();
   p.receiveFromEast();
   p.receiveFromWest();
   p.sendToNW();
   p.sendToNE();
   p.sendToSW();
   p.sendToSE();
   p.receiveFromSE();
   p.receiveFromSW();
   p.receiveFromNE();
   p.receiveFromNW();
}
void seqDispPartitions(Partition2D &p)
{
    /** Sequential Display of all partitions **/
	for(int i = 0; i < p.num_process; i++)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		if(i == p.rank)
		{
			p.dispNeighbors();
			p.dispPartition();
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}

}



void seqSavePartitions(Partition2D &p)
{
    /** Sequential Display of all partitions **/
	for(int i = 0; i < p.num_process; i++)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		if(i == p.rank)
		{
			p.savePartition();
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}
	if(p.rank == 0)
		cout << "Output File Saved in " << p.output_path + p.file_name +'-'+ patch::to_string(p.num_process) + '.' + patch::to_string(p.Gens) + '.' + patch::to_string(p.X_Limit) + '.' + patch::to_string(p.Y_Limit) + ".out" << endl;

}

void seqOutPartitions(Partition2D &p)
{
    /** Sequential Display of all partitions **/
	for(int i = 0; i < p.num_process; i++)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		if(i == p.rank)
		{
			p.dispPartitionIndices();
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}
}

int main(int argc, char *argv[])
{
	initMPI(argc, argv);

    //calculateBoard();

    //finalizeBoard();
	return 0;
}
