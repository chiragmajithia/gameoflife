#ifndef BOARD_CPP
#define BOARD_CPP

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <exception>
#include <mpi.h>
#include <cstdlib>
#include <sstream>



using namespace std;

class Board
{
public:
	int num_partitions;
	string input_path;
	string output_path;
	string file_name;
	int MAX_R, MAX_C, Gens;

	vector<int> indx;
	vector< vector<bool> > board;


	Board(string p)
	{
		output_path = p;
	}

	bool parseArguments(int argc, char *arv[])
	{
		input_path = "../Input/";
		if(argc != 5)
		{
			cout << "Err: Invalid No. of Arguments"<<endl;
			return false;
		}
		else
		{

			file_name = arv[1];
			Gens = atoi(arv[2]);
			MAX_R = atoi(arv[3]);
			MAX_C = atoi(arv[4]);
    	//cout << "Input File: " << file_name << "\nNumber of Gens: " << Gens << "\nPreparing Board["<<MAX_R<<","<<MAX_C<<"]"<<endl;
    	//cout << "Parisng Arguments done"<<endl;
			MPI_Comm_size(MPI_COMM_WORLD, &num_partitions);
    	//Check if number of processes are greater than total number of cells
    	//cout << "Number of processes = " << num_partitions << endl;
		}

		board.resize(MAX_R+2,vector<bool>(MAX_C+2));
		makeBoard(file_name);
     //dispBoard();
    //cout << "Master Board Generation Complete " << endl;
		clearFile();
		return true;
	}

	bool makeBoard(string &filename)
	{
		ifstream myfile;
		int l_cnt = 0;
		string line;
		string file_path = input_path + filename;
    //cout << "Opening File " << filename;
		myfile.open (file_path.c_str());
		if (!myfile) 
		{
			cout << "Err: Invalid Input File"<<endl;
			return false;
		}

		while(!myfile.eof())
		{
			getline(myfile,line);
    	//cout << atoi(line.substr(0,line.find(' ')));
			try
			{
				int x = atoi((line.substr(0,line.find(' '))).c_str())+1;
				int y = atoi((line.substr(line.find(' '),line.length()-1).c_str()))+1;
				board[x][y] = true;
				indx.push_back(y + x * (MAX_C+2));

				l_cnt++;
    	  //cout << l_cnt <<": " << line << "[" << x <<"," << y << "] = " << indx[l_cnt-1]<<endl;
			}
			catch(exception &e)
			{
    		//cout << "End of File Reached at line :: " << l_cnt << endl;
			}

		}

		myfile.close();
		return true;
	}


	int countLiveNeighbors(int x, int y)
	{
		int cnt = 0;
		for(int i = -1; i <= 1; i++)
		{
			for(int j = -1; j <= 1; j++)
			{
				if(i == 0 && j == 0)
					continue;
				cnt += board[x+i][y+j];
			}
		}
		return cnt;
	}

	string to_string(int a)
	{
		stringstream ss;
		ss << a;
		return ss.str();
	}
	bool clearFile()
	{

		string file_path = output_path + file_name +'-'+ to_string(num_partitions)+'.'+to_string(Gens) + '.' + to_string(MAX_R) + '.' + to_string(MAX_C) + ".out";
  //cout << "Clearing File : " << file_path << endl;
		ofstream myfile (file_path.c_str(),ios::out | ios::trunc);
		if (myfile.is_open())
		{
			myfile.close();
			return true;
		}
		else cout << "Unable to open file";
		return false;
	}

	void dispIndex()
	{
		cout << "Printing Index "<<endl;
		for(int i = 0; i < indx.size(); i++)
			cout<<indx[i]<<endl;
	}

	void dispBoard()
	{
		for(int i = 0; i < board.size(); i++)
		{
			for(int j = 0; j < board[0].size();j++)
			{
				if(i == 0 || i == MAX_R+1)
					cout << "_";
				else if (j == 0 || j == MAX_C+1)
					cout << "|";
				else if(board[i][j])
					cout << countLiveNeighbors(i,j);
				else
					cout << ".";
			}
			cout << endl;
		}
	}

	~Board()
	{
//cout << "Destroying Board" << endl;
	}


};

#endif 