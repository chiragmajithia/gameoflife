#ifndef PARTITION2D_CPP
#define PARTITION2D_CPP

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

class Partition2D
{
	public :

	enum tags
	{
		N_N,
		N_S,
		N_W,
		N_E,
		N_NE,
		N_NW,
		N_SE,
		N_SW
	};
// Origin and Dimensions of the board
	const int D;
	int X_Limit, Y_Limit, Gens;
	int rank, num_process;
	int cart_dim_x, cart_dim_y;
	int cart_rank_x, cart_rank_y;
	int strt_x, strt_y, len_x, len_y;
	int N_n, N_s, N_w, N_e, N_ne, N_nw, N_se, N_sw;
	MPI_Request request;
	MPI_Status status;
	vector<int> *curr_gen, *next_gen;
	MPI_Comm cart_comm;
	MPI_Datatype columntype;

// File saving operations
	string output_path;
	string file_name;
	

	Partition2D(char *argv[]) : D(2),file_name(argv[1]), Gens(atoi(argv[2])),X_Limit(atoi(argv[4])), Y_Limit(atoi(argv[3]))
	{
		output_path = "../OutPat2D/";
	/**
	The 0,0 of the game starts at 1,1 of the representation. Padding is done to ease neighbor calcualtion
	Start from strt_x - 1 and end at len_x + 1. Similarly strt_y - 1 and end at len_y + 1
	The representation of Partition is [X][Y] where X is columns and Y is rows  unline [R][C]
	The single dimension error is indexed as = x + (y * len_x), keeping all the indices of x together
	**/

	/** Getting rank of the current thread and total number of processes **/
		int dims[D], periods[D], reorder = 1;
		for(int  i = 0; i < D; i++)
		{
			dims[i] = 0;
			periods[i] = 0;
		}

		MPI_Comm_size(MPI_COMM_WORLD, &num_process);
		MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	/** dims[0] will give number of partitions in Y direction and dims[1] wull give number of partitions along X**/
		MPI_Dims_create(num_process, D, dims);
		cart_dim_x = dims[1]; cart_dim_y = dims[0];
		
		MPI_Cart_create(MPI_COMM_WORLD, D, dims, periods, reorder, &cart_comm); // What is reorder
		
		int coord[2];
		MPI_Cart_coords(cart_comm, rank, 2, coord);
		cart_rank_y = coord[0]; cart_rank_x = coord[1];
		request = MPI_REQUEST_NULL;	

	/** As mentioned - the actual cells start at 1,Y. Y is dependent on the rank of the thread **/
		
		strt_x = ((X_Limit)/cart_dim_x * cart_rank_x)+1;
		strt_y = ((Y_Limit)/cart_dim_y * cart_rank_y)+1;
	/**If the current partition is last partition -- grab everything left else divide paritions equally **/
		if(cart_rank_x+1 != cart_dim_x)
		{
			len_x = X_Limit/cart_dim_x;
		}
		else
		{
			len_x = (int)(X_Limit/cart_dim_x) + (X_Limit % cart_dim_x);
		}

		if(cart_rank_y+1 != cart_dim_y)
		{
			len_y = Y_Limit/cart_dim_y;
		}
		else
		{
			len_y = (int)(Y_Limit / cart_dim_y) + (Y_Limit % cart_dim_y);
		}

		//dispBounds();

	/** Allocating Memory for the partition pointers **/
		curr_gen  = new vector<int>((len_x+2) * (len_y+2));
		next_gen  = new vector<int>((len_x+2) * (len_y+2));
		//dispBounds();

	/**  Setting Neighboring partitions **/
		setNeighbors();
		
		// MPI_Type_vector(len_y, 1, 1, MPI::INT, &columntype);
		// MPI_Type_commit(&columntype);
	}

	void setNeighbors()
	{
		//For Top Neighbor
		N_n = getNeighborAt(cart_rank_x,cart_rank_y -1);
		//For Botton Neighbor
		N_s = getNeighborAt(cart_rank_x,cart_rank_y+1);
		//For Left Neighbor
		N_w = getNeighborAt(cart_rank_x-1,cart_rank_y);
		//For Right Neighbor
		N_e = getNeighborAt(cart_rank_x+1,cart_rank_y);
		//For top right
		N_ne = getNeighborAt(cart_rank_x+1,cart_rank_y-1);
		//For top left
		N_nw = getNeighborAt(cart_rank_x-1,cart_rank_y-1);
		//For bottom right
		N_se = getNeighborAt(cart_rank_x+1,cart_rank_y+1);
		//For bottom left
		N_sw = getNeighborAt(cart_rank_x-1,cart_rank_y+1);

	}

	void getCol(int y, int *col)
	{
		
		for(int i = 1; i <= len_y; i++)
		{
			int id = -1;
			indx2id(y,i,id);
			col[i-1] = (*curr_gen)[id];
		}
	}

	void dispNeighbors()
	{
		cout <<rank <<":Ranks of Neighbors: <"<<N_n << "," << N_s << "," << N_w << ", " << N_e <<">";
		cout <<N_nw << "," << N_ne << "," << N_sw << ", " << N_se <<endl;
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

	void sendToEast()
	{
		if(N_e != -1)
		{
			int *col = new int[len_y];
			getCol(len_x,col);
			MPI_Isend(col,len_y,MPI::INT, N_e, N_W, MPI_COMM_WORLD,&request);
		}
	}

	void receiveFromWest()
	{
		if(N_w != -1)
		{
			int *col = new int[len_y];
			
			MPI_Recv(col,len_y,MPI::INT,N_w, N_W, MPI_COMM_WORLD,&status);
			for(int i = 0; i < len_y; i++)
			{
				int id = -1;
				indx2id(0,i+1,id);
				(*curr_gen)[id] = col[i];
			}
		}
	}

	void sendToWest()
	{
		if(N_w != -1)
		{
			int *col = new int[len_y];
			getCol(1,col);
			//cout << rank << "sending to West: " << N_e << endl;
			MPI_Isend(col,len_y,MPI::INT, N_w, N_E, MPI_COMM_WORLD,&request);
		}
	}

	void receiveFromEast()
	{
		if(N_e != -1)
		{
			int *col = new int[len_y];
			
			MPI_Recv(col,len_y,MPI::INT,N_e, N_E, MPI_COMM_WORLD,&status);
			for(int i = 0; i < len_y; i++)
			{
				int id = -1;
				indx2id(len_x+1,i+1,id);
				(*curr_gen)[id] = col[i];
			}
		}
	}

	// void sendToEast()
	// {
	// 	if(N_e != -1)
	// 	{
	// 		cout << rank << "sending to East: " << N_e << endl;
	// 		int id = -1;
	// 		indx2id(len_x,1,id);
	// 		MPI_Isend(curr_gen->data()+id,len_y,columntype, N_e, N_W, MPI_COMM_WORLD, &request);
	// 	}
	// }

	// void receiveFromWest()
	// {
	// 	if(N_w != -1)
	// 	{
	// 		int buffer[len_y];
	// 		cout << buffer.size();
	// 		cout << rank << "receiving from West: " << N_w << endl;
	// 		int id = -1;
	// 		indx2id(0,1,id);
	// 		MPI_Recv(&buffer,len_y,columntype, N_w, N_W, MPI_COMM_WORLD,&status);
	// 		for(int i = 0; i < len_y; i++)
	// 		{
	// 			cout<<buffer[i] <<",";
	// 		}
	// 		cout << endl;
	// 		//MPI_Wait(&request,&status);
	// 		//cout << rank <<"Received from W"<<endl;
	// 		//dispPartition();
	// 	}
	// }

	// void sendToWest()
	// {
	// 	if(N_w != -1)
	// 	{
	// 		cout << rank << "sending to West: " << N_w << endl;
	// 		int id = -1;
	// 		indx2id(1,1,id);
	// 		MPI_Isend(curr_gen->data()+id,len_y,columntype, N_w, N_E, MPI_COMM_WORLD, &request);
	// 	}
	// }

	// void receiveFromEast()
	// {
	// 	if(N_e != -1)
	// 	{
	// 		cout << rank << "receiving from East: " << N_e << endl;
	// 		int id = -1;
	// 		indx2id(len_x,len_y+1,id);
	// 		MPI_Irecv(curr_gen->data()+id,len_y,columntype, N_e, N_E, MPI_COMM_WORLD, &request);
	// 		//MPI_Wait(&request,&status);
	// 		//cout << rank <<"Received from W"<<endl;
	// 		//dispPartition();
	// 	}
	// }

	void receiveFromNW()
	{
		if(N_nw != -1)
		{
			//cout << rank << "receiving from NW: " << N_nw;
			int id = -1;
			indx2id(0,0,id);
			MPI_Irecv(curr_gen->data()+id,1,MPI::INT,N_nw,N_NW,MPI_COMM_WORLD, &request);
		}
	}

	void sendToSE()
	{
		if(N_se != -1)
		{
			//cout << rank << "sending to SE: " << N_se;
			int id = -1;
			indx2id(len_x,len_y,id);
			//cout << ": [" << id <<"]" << (*curr_gen)[id] << endl;
			MPI_Isend(curr_gen->data()+id,1,MPI::INT,N_se,N_NW,MPI_COMM_WORLD, &request);
		}
	}


	void receiveFromNE()
	{
		if(N_ne != -1)
		{
			//cout << rank << "receiving from NE: " << N_ne;
			int id = -1;
			indx2id(1+len_x,0,id);
			MPI_Irecv(curr_gen->data()+id,1,MPI::INT,N_ne,N_NE,MPI_COMM_WORLD, &request);
		}
	}

	void sendToSW()
	{
		if(N_sw != -1)
		{
			//cout << rank << "sending to SW: " << N_sw;
			int id = -1;
			indx2id(1,len_y,id);
			//cout << ": [" << id <<"]" << (*curr_gen)[id] << endl;
			MPI_Isend(curr_gen->data()+id,1,MPI::INT,N_sw,N_NE,MPI_COMM_WORLD, &request);
		}
	}

	void receiveFromSE()
	{
		if(N_se != -1)
		{
			int id = -1;
			indx2id(1+len_x,1+len_y,id);
			MPI_Irecv(curr_gen->data()+id,1,MPI::INT,N_se,N_SE,MPI_COMM_WORLD, &request);
		}
	}

	void sendToNW()
	{
		if(N_nw != -1)
		{
			//cout << rank << "sending to NW: " << N_nw;
			int id = -1;
			indx2id(1,1,id);
			//cout << ": [" << id <<"]" << (*curr_gen)[id] << endl;
			MPI_Isend(curr_gen->data()+id,1,MPI::INT,N_nw,N_SE,MPI_COMM_WORLD, &request);
		}
	}

	void receiveFromSW()
	{
		if(N_sw != -1)
		{
			//cout << rank << "receiving from SW:" << N_sw;
			int id = -1;
			indx2id(0,1+len_y,id);
			MPI_Irecv(curr_gen->data()+id,1,MPI::INT,N_sw,N_SW,MPI_COMM_WORLD, &request);
			MPI_Wait(&request,&status);
		}
	}

	void sendToNE()
	{
		if(N_ne != -1)
		{
			//cout << rank << "sending to NE:" << N_ne;
			int id = -1;
			indx2id(len_x,1,id);
			//cout << ": [" << id <<"]" << (*curr_gen)[id] << endl;
			MPI_Isend(curr_gen->data()+id,1,MPI::INT,N_ne,N_SW,MPI_COMM_WORLD, &request);
		}
	}


	void receiveFromNorth()
	{

		if(N_n != -1)
		{
			int id = -1;
			indx2id(1,0,id);
			MPI_Irecv(curr_gen->data()+id,len_x,MPI::INT,N_n,N_N,MPI_COMM_WORLD, &request);
		}

	}

	void receiveFromSouth()
	{
		if(N_s != -1)
		{
			int id = -1;
			indx2id(1,len_y+1,id);
			MPI_Irecv(curr_gen->data()+id,len_x,MPI::INT,N_s,N_S,MPI_COMM_WORLD,&request);
		}

	}

	void sendToNorth()
	{
		if(N_n != -1)
		{
			//cout << rank << ": send first row to " << N_N << endl;
			int id = -1;
			indx2id(1,1,id);
			MPI_Isend(curr_gen->data()+id,len_x,MPI::INT,N_n,N_S,MPI_COMM_WORLD,&request);
		}
	}

	void sendToSouth()
	{
		if(N_s != -1)
		{
			int id = -1;
			indx2id(1,len_y,id);
			MPI_Isend(curr_gen->data()+id,len_x,MPI::INT,N_s,N_N,MPI_COMM_WORLD,&request);
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
			//cout <<rank <<": id = "<<x<<","<<y<<"="<<id << endl;
			if(id != -1)
			{
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

	int getNeighborAt(int cp_x, int cp_y)
	{
		if(cp_x >= 0 && cp_y >= 0 && cp_x < cart_dim_x && cp_y < cart_dim_y )
		{
			int r;
			int coords[] = {cp_y, cp_x};
			int res = MPI_Cart_rank(cart_comm,coords,&r);
			if(res != MPI_SUCCESS)
			{
				return -1;
			}
			return r;
		}
		else
		{
    		return -1;
		}
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


	void dispBounds()
	{
		cout << rank  <<" :  Cartisian Coordinates : [" <<  strt_x << "," << strt_y << "] <" <<len_x << "," << len_y << ">" <<endl;
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
