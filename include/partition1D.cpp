#ifndef PARTITION1D_CPP
#define PARTITION1D_CPP

#include <vector>
#include <mpi.h>
#include <exception>
#include <fstream>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}
using namespace std;

class Partition1D
{
public :

enum tags
{
	N_UP,
	N_DOWN
};
// Origin and Dimensions of the board
int X_Limit, Y_Limit;
int Gens;
int rank, num_process;
int strt_x, strt_y, len_x, len_y;
int N_up, N_down;
MPI_Request request;
MPI_Status status;
vector<int> *curr_gen, *next_gen;

// File saving operations
string output_path;
string file_name;

Partition1D(char *argv[]) :file_name(argv[1]), Gens(atoi(argv[2])),X_Limit(atoi(argv[3])), Y_Limit(atoi(argv[4]))
{
	output_path = "../OutPat1D/";
	/**
	The 0,0 of the game starts at 1,1 of the representation. Padding is done to ease neighbor calcualtion
	Start from strt_x - 1 and end at len_x + 1. Similarly strt_y - 1 and end at len_y + 1
	The representation of Partition is [X][Y] where X is columns and Y is rows  unline [R][C]
	The single dimension error is indexed as = x + (y * len_x), keeping all the indices of x together
	**/

	/** Getting rank of the current thread and total number of processes **/
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_process);
	request = MPI_REQUEST_NULL;	
	//dispParams();

	/** As mentioned - the actual cells start at 1,Y. Y is dependent on the rank of the thread **/
	strt_x = 1;
	strt_y = ((Y_Limit)/num_process * rank)+1;
	len_x = X_Limit;
	
	/**If the current partition is last partition -- grab everything left else divide paritions equally **/
	if(rank+1 != num_process)
	{
		len_y = (int)(Y_Limit / num_process);
	}
	else
	{
		len_y = (int)(Y_Limit / num_process) + (Y_Limit % num_process);
	}

	/** Allocating Memory for the partition pointers **/
	curr_gen  = new vector<int>((len_x+2) * (len_y+2));
	next_gen  = new vector<int>((len_x+2) * (len_y+2));
	//dispBounds();
	
	/**  Setting Neighboring partitions **/
	N_up = getPartition(strt_x,strt_y-1) ;
	N_down = getPartition(strt_x,strt_y + len_y + 1);
	//cout << rank << ":Neighbors of partition:  are:" << N_up << " and " <<N_down << endl;
}


void simGen()
{

	for(int i = 1; i <= len_x; i++)
	{
		for(int j = 2; j <= len_y-1; j++)
		{
			int id;
			indx2id(i,j,id);
			if((*curr_gen)[id])
			{
				int n = countLiveNeighbors(i,j);
				if(n < 2)
					(*next_gen)[id] = false;
				else if(n > 3)
					(*next_gen)[id] = false;
				else if(n == 2 || n == 3)
					(*next_gen)[id] = true;
			}
			else
			{
				int n = countLiveNeighbors(i,j);
				if(n == 3)
					(*next_gen)[id] = true;
				else
					(*next_gen)[id] = false;
			}
		}
	}
	/** Wait for ghost cells to get allocated**/
	MPI_Wait(&request,&status);
	MPI_Barrier(MPI_COMM_WORLD);
	
	for(int i = 1; i <= len_x; i++)
	{
			int id, j = 1;
			indx2id(i,j,id);
			if((*curr_gen)[id])
			{
				int n = countLiveNeighbors(i,j);
				if(n < 2)
					(*next_gen)[id] = false;
				else if(n > 3)
					(*next_gen)[id] = false;
				else if(n == 2 || n == 3)
					(*next_gen)[id] = true;
			}
			else
			{
				int n = countLiveNeighbors(i,j);
				if(n == 3)
					(*next_gen)[id] = true;
				else
					(*next_gen)[id] = false;
			}
	}
	for(int i = 1; i <= len_x; i++)
	{
			int id, j = len_y;
			indx2id(i,j,id);
			if((*curr_gen)[id])
			{
				int n = countLiveNeighbors(i,j);
				if(n < 2)
					(*next_gen)[id] = false;
				else if(n > 3)
					(*next_gen)[id] = false;
				else if(n == 2 || n == 3)
					(*next_gen)[id] = true;
			}
			else
			{
				int n = countLiveNeighbors(i,j);
				if(n == 3)
					(*next_gen)[id] = true;
				else
					(*next_gen)[id] = false;
			}
	}
	//cout << "ALL MESSAGE PASSING DONE!" << endl;
}

void swap()
{
	vector<int> *temp = curr_gen;
	curr_gen = next_gen;
	next_gen = temp;
}

void receiveGhostUp()
{
	
	if(N_up != -1)
	{
	int id = -1;
	indx2id(1,0,id);
	MPI_Irecv(curr_gen->data()+id,len_x,MPI::INT,N_up,0,MPI_COMM_WORLD, &request);
	}
	
}

void receiveGhostDown()
{
	if(N_down != -1)
	{
	int id = -1;
	indx2id(1,len_y+1,id);
	MPI_Irecv(curr_gen->data()+id,len_x,MPI::INT,N_down,0,MPI_COMM_WORLD,&request);
	}

}

void sendFirstRow()
{
	if(N_up != -1)
	{
	int id = -1;
	indx2id(1,1,id);
	MPI_Isend(curr_gen->data()+id,len_x,MPI::INT,N_up,0,MPI_COMM_WORLD,&request);
	}
}

void sendLastRow()
{
	if(N_down != -1)
	{
	int id = -1;
	indx2id(1,len_y,id);
	MPI_Isend(curr_gen->data()+id,len_x,MPI::INT,N_down,0,MPI_COMM_WORLD,&request);
	}
}

void populatePartition(vector<int> &indx)
{
	/** Has the broadcast indexes in indx and fills the local partition **/
	for(int i = 0; i < indx.size(); i++)
	{
		int x,y,id = -1;
		id2indxG(x,y,indx[i]);
		id = isPartOf(x,y);
		if(id != -1)
		{
			//cout << "id = "<<x<<","<<y<<"="<<id << endl;
			if(id > curr_gen->size())
			{
				cout << "Out of Bounds" << endl;
			}
			else
				(*curr_gen)[id] = true;
		}
	}
	//dispPartition();
}

// int countLiveNeighbors(int x, int y)
// {
// 	int cnt = 0, id = -1;
// 	for(int i = -1; i <= 1; i++)
// 	{
// 		for(int j = -1; j <= 1; j++)
// 		{
// 			if(i == 0 && j == 0)
// 			continue;
// 			indx2id(x+i,y+j,id);
// 			cnt += (*curr_gen)[id];
// 		}
// 	}
// 	return cnt;
// }

int countLiveNeighbors(int x, int y)
{
	int cnt = 0, id = -1;
	indx2id(x-1,y-1,id);
	cnt+=(*curr_gen)[id] + (*curr_gen)[id+1] + (*curr_gen)[id+2];
	indx2id(x-1,y,id);
	cnt+=(*curr_gen)[id] + (*curr_gen)[id+2];
	indx2id(x-1,y+1,id);
	cnt+=(*curr_gen)[id] + (*curr_gen)[id+1] + (*curr_gen)[id+2];
	return cnt;
}

// TODO :: Test This Logic
int getPartition(int g_x, int g_y)
{
    int rank = -1;
    if(g_y  == 0 || g_y == Y_Limit+2)
    	return -1; // Padding curr_gen

    int stride = (int)(Y_Limit/num_process);
    while(g_y > (rank+1)*stride)
    {
    	rank++;
    }
    rank = rank < num_process? rank : num_process-1;
    return rank;
}

bool savePartition()
{
  string file_path = output_path + file_name + '-' + patch::to_string(num_process) +'.'+ patch::to_string(Gens) + '.' + patch::to_string(X_Limit) + '.' + patch::to_string(Y_Limit) + ".out";
  ofstream myfile (file_path.c_str(),ios::app);
  if (myfile.is_open())
  {
  	//myfile << "Partition: " << rank << endl;
  	for(int i = 1; i <= len_x; i++)
  	{
  		for(int j = 1; j <= len_y; j++)
  		{
  			int idx;
  			indx2id(i,j,idx);
  			if((*curr_gen)[idx])
  				myfile << strt_y + j-2 << ' ' << strt_x+i-2 << endl;
  		}
  	}
  	myfile.close();
  	return true;
  }
  else cout << "Unable to open file";
  return false;
}

void dispPartitionIndices()
{
	for(int i = 1; i <= len_x; i++)
  	{
  		for(int j = 1; j <= len_y; j++)
  		{
  			int idx;
  			indx2id(i,j,idx);
  			if((*curr_gen)[idx])
  				cout << strt_y + j-2 << ' ' << strt_x+i-2 << endl;
  		}
  	}
}
void dispParams()
{
	cout << rank << ":X,Y : (" << X_Limit <<"," << Y_Limit <<  ")"<<endl;
	cout << rank <<":num_process" << num_process << endl;
}

void dispBounds()
{
	cout << rank << ":Parition created:: " ;
	cout << rank << ":Start Index: "<< strt_x << "," << strt_y << endl;
	cout << rank << ":Lengths: "<<len_x << "," << len_y <<endl;
}


	void dispPartition()
	{
		cout << "Displaying process " << rank << endl;
		cout << "O(" <<strt_x<<","<<strt_y <<")" <<endl;
		for(int  j = 0; j <= len_y+1; j++)
		{
			for(int i = 0; i <= len_x+1; i++)
			{
				if(i == 0 || i == len_x + 1)
				{
					int id = -1;
					indx2id(i,j,id);
					cout << (*curr_gen)[id];
				}
				else if(j == 0 || j == len_y + 1)
				{
					int id = -1; 
					indx2id(i,j,id);
					cout << (*curr_gen)[id];
				}
				else
				{
					int id = -1;
					indx2id(i,j,id);
					if((*curr_gen)[id])
					{
						int n = countLiveNeighbors(i,j);
						cout << n;
					}
					else
						cout << ".";
				}
			}
			cout << endl;
		}
	}

	void indx2id(int x, int y, int &id)
	{
		id = x + (y * (len_x+2));
	}

	void id2indx(int &x, int &y, int id)
	{
		x = id % (len_x+2);
		y = (int)(id/(len_x+2));
	}

	void indx2idG(int x, int y, int &id)
	{
		id = x + (y * (X_Limit+2));
	}

	void id2indxG(int &x, int &y, int id)
	{
		x = id % (X_Limit+2);
		y = (int)(id/(X_Limit+2));

	}

	int isPartOf(int g_x, int g_y)
	{
		/** Checks if global index (x,y) is part of partition
			returns idx in local partion, else returns -1; **/
		if((g_x > strt_x + len_x -	1 ) || (g_x < strt_x ))
		{
			return  -1;
		}
		if((g_y > strt_y + len_y  - 1 ) || (g_y < strt_y ))
		{
			return  -1;
		}
		else
		{
			int dx = g_x - (strt_x - 1);
			int dy = g_y - (strt_y - 1);
			return dx + (dy * (len_x+2));
		}
		
	}

};

#endif 
