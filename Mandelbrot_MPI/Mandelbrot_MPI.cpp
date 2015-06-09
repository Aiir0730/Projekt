#include "master_file.cpp"
#include "slave_file.cpp"
#include <stdio.h>
#include <chrono>
#include <fstream>

int master(int argc, char* argv[], int worldSize);
int slave(int argc, char* argv[], int worldSize);


//inicjalizacja zmiennych globalnych
struct master2Slave packageMaster2Slave;
struct slave2Master packageSlave2Master;

int DIETAG = 2;
int WORKTAG = 1;

int worldSize;
int worldRank;

std::chrono::time_point<std::chrono::system_clock> start, end;

// G³ówny punkt wej¶cia programu. Program dzia³a w trybie konsoli
int main(int argc, char *argv[])	// argv: depth, taskPerThread, x, y, colorR, colorG, colorB
{
	// parametry generowanego fraktala zostan¹ przekazane przez kolejne argumenty wywo³ania
	if (argc != 12) 
	{
		printf("za malo argumentow\n");
		return -1;
	
	}
	int worldSize, worldRank;
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

	//std::cout<<"worldSize MPI: "<<worldSize<<"\n";

	if (worldRank == 0)
	{
		start = std::chrono::system_clock::now();
		master(argc, argv, worldSize);
		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		std::fstream fout;
		char buf[50];
		sprintf(buf, "%s%s", "id_", argv[8]); 
		fout.open(buf, std::fstream::in | std::fstream::out | std::fstream::app);
		fout << " ";
		fout.close();
	}
	else
	{
		slave(argc, argv, worldSize);
	}

	MPI_Finalize();
	return 0;
}

