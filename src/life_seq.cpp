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

vector< vector<bool> > *curr_gen = new vector< vector<bool> >();
vector< vector<bool> > *next_gen = new vector< vector<bool> >();

bool parseArguments(int argc, char *arv[]);
bool makeBoard(string &file_path);
void dispBoard();
int countLiveNeighbors(int x, int y);
void simGen(); 
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
    	 (*curr_gen)[x][y] = true;
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

void dispBoard()
{
	for(int i = 0; i <= (*curr_gen).size(); i++)
	{
		for(int j = 0; j <= (*curr_gen)[0].size();j++)
		{
			if(i == 0 || i == Y_Limit+2)
				cout << "_";
			else if (j == 0 || j == X_Limit+2)
				cout << "|";
			else if((*curr_gen)[i][j])
				cout << countLiveNeighbors(i,j);
			else
				cout << ".";
		}
		cout << endl;
	}
}

void dispOutput()
{
	int cnt = 0;
	for(int i = 1; i <= X_Limit; i++)
	{
		for(int j = 1; j <= Y_Limit; j++)
			if((*curr_gen)[i][j])
				cout <<i-1 << ' ' << j-1 << '\n';
	}
}

bool saveBoard()
{
  string file_path = output_path + file_name + "-seq" + '.' + patch::to_string(Gens) + '.' + patch::to_string(X_Limit) + '.' + patch::to_string(Y_Limit) + ".out";
  ofstream myfile (file_path.c_str());
  if (myfile.is_open())
  {
  	for(int i = 1; i <= X_Limit; i++)
  	{
  		for(int j = 1; j <= Y_Limit; j++)
  			if((*curr_gen)[i][j])
  				myfile << i-1 << ' ' << j-1 << '\n';
  	}
  	myfile.close();
  	return true;
  }
  else cout << "Unable to open file";
  return false;
}

int main(int argc, char *argv[])
{

	string file_path;
	if(!parseArguments(argc,argv))
		exit(0);
	file_path = input_path + file_name;
	(*curr_gen).resize(Y_Limit+2, vector<bool>(X_Limit+2, false ) );
	(*next_gen).resize(Y_Limit+2, vector<bool>(X_Limit+2, false ) );

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
	return (*curr_gen)[x-1][y-1] + (*curr_gen)[x][y-1] + (*curr_gen)[x+1][y-1] +
		   (*curr_gen)[x-1][y] + (*curr_gen)[x+1][y] + (*curr_gen)[x-1][y+1] + 
	       (*curr_gen)[x][y+1] + (*curr_gen)[x+1][y+1];
	
}

void simGen()
{
	for(int i = 1; i <= X_Limit; i++)
	{
		for(int j = 1; j <= Y_Limit; j++)
		{
			if((*curr_gen)[i][j])
			{
				int n = countLiveNeighbors(i,j);
				if(n < 2)
					(*next_gen)[i][j] = false;
				else if(n > 3)
					(*next_gen)[i][j] = false;
				else if(n == 2 || n == 3)
					(*next_gen)[i][j] = true;
			}
			else
			{
				int n = countLiveNeighbors(i,j);
				if(n == 3)
					(*next_gen)[i][j] = true;
				else
					(*next_gen)[i][j] = false;
			}
		}
	}

	vector< vector<bool> > *temp = curr_gen;
	curr_gen = next_gen;
	next_gen = temp;

}

