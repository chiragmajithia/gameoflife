#include <mpi.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <exception>
#include <cstdlib>
#include <sstream>
using namespace std;

string input_path = "../Input/";
string output_path = "../OutSeq/";
string file_name;

namespace patch
{
    template < typename T > std::string to_string( const T& n )
	{
		std::ostringstream stm ;
		stm << n ;
		return stm.str() ;
	}
}

int X_Limit = 0, Y_Limit = 0, Gens = 0;

vector<int> *curr_gen, *next_gen;

bool parseArguments(int argc, char *arv[]);
bool makeBoard(string &file_path);
void dispBoard();
int countLiveNeighbors(int x, int y);
void simGen(); 
inline void indx2id(int x, int y, int &id);
bool saveBoard();

bool parseArguments(int argc, char *arv[])
{
	if(argc != 5)
	{
		cout << "Err: Invalid No. of Arguments"<<endl;
		return false;
	}
	else
	{
		//cout << "Parsing Arguments:.. "<<endl;
		file_name = arv[1];
		Gens = atoi(arv[2]);
		X_Limit = atoi(arv[3]);
		Y_Limit = atoi(arv[4]);
		//cout << "Input File: " << file_name << "\nNumber of Gens: " << Gens << "\nPreparing Board["<<X_Limit<<","<<Y_Limit<<"]"<<endl;
		//cout << "Parisng Arguments done"<<endl;
	}
	return true;
}

bool makeBoard(string &filename)
{
	ifstream myfile;
	vector< vector<int> > indx;
	int l_cnt = 0;
	string line;
	filename = input_path + filename;
	myfile.open (filename.c_str());
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
			int id = -1;
			indx2id(x,y,id);
			(*curr_gen)[id] = true;
			l_cnt++;
		 //cout << l_cnt <<": " << line << "[" << x <<"," << y << "]" << endl;
		}
		catch(exception &e)
		{
			//cout << "End of File Reached at line :: " << l_cnt << endl;
		}
	}

	myfile.close();
	return true;
}

// void dispBoard()
// {
// 	for(int i = 0; i <= (*curr_gen).size(); i++)
// 	{
// 		for(int j = 0; j <= (*curr_gen)[0].size();j++)
// 		{
// 			if(i == 0 || i == Y_Limit+2)
// 				cout << "_";
// 			else if (j == 0 || j == X_Limit+2)
// 				cout << "|";
// 			else if((*curr_gen)[i][j])
// 				cout << countLiveNeighbors(i,j);
// 			else
// 				cout << ".";
// 		}
// 		cout << endl;
// 	}
// }

void dispOutput()
{
	int cnt = 0;
	for(int i = 1; i <= X_Limit; i++)
	{
		for(int j = 1; j <= Y_Limit; j++)
			{
				int idx;
  			indx2id(i,j,idx);
  			if((*curr_gen)[idx])
  				cout << i-1 << ' ' << j-1 << endl;

			}
		}
	}

	bool saveBoard()
	{
		string file_path = output_path + file_name + "-seq" +'.'+ patch::to_string(Gens) + '.' + patch::to_string(X_Limit) + '.' + patch::to_string(Y_Limit) + ".out";
		ofstream myfile (file_path.c_str(),ios::app);
		if (myfile.is_open())
		{
  	//myfile << "Partition: " << rank << endl;
			for(int i = 1; i <= X_Limit; i++)
			{
				for(int j = 1; j <= Y_Limit; j++)
				{
					int idx;
					indx2id(i,j,idx);
					if((*curr_gen)[idx])
						myfile << j-1<< ' ' << i-1 << endl;
				}
			}
			myfile.close();
			return true;
		}
		else cout << "Unable to open file";
		return false;
	}

	inline void indx2id(int x, int y, int &id)
	{
		id = x + (y * (X_Limit+2));
	}

	int main(int argc, char *argv[])
	{

		string file_path;
		if(!parseArguments(argc,argv))
			exit(0);
		file_path = input_path + file_name;
		curr_gen  = new vector<int>((X_Limit+2) * (Y_Limit+2));
		next_gen  = new vector<int>((X_Limit+2) * (Y_Limit+2));

	//dispBoard();
		if(!makeBoard(file_path))
			exit(0);

	//cout << " BOARD SIZE " << (*curr_gen).size()<<endl;
	//dispBoard();
		for(int i = 0; i < Gens; i++)
		{
		//cout << "Simulating Gen: "<<i+1 << endl;
			simGen();
		}
	//dispBoard();
		dispOutput();
		if(saveBoard())
			cout << "Output Saved in "+ output_path + file_name + "-seq" + '.' + patch::to_string(Gens) + '.' + patch::to_string(X_Limit) + '.' + patch::to_string(Y_Limit) + ".out" << endl;
		else
			cout << "Error Saving Output" << endl;

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

	void simGen()
	{

		for(int i = 1; i <= X_Limit; i++)
		{
			for(int j = 1; j <= Y_Limit; j++)
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

		vector< int > *temp = curr_gen;
		curr_gen = next_gen;
		next_gen = temp;

	}

