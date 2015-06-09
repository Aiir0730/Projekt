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
	if (argc != 14) 
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
		std::chrono::duration<double> elapsed_time = end - start;
		std::fstream fout_time;
		std::fstream fout_log;
		char buf[50];
		sprintf(buf, "%s%s", argv[8], "_time"); 
		fout_time.open(buf, std::fstream::out);
		fout_time << elapsed_time.count();
		fout_time.close();
		
		fout_log.open("log", std::fstream::out | std::fstream::app);
		fout_log << worldSize << " " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[9] << " " << argv[12] << " " << elapsed_time.count() << std::endl;
		fout_log.close();
	}
	else
	{
		slave(argc, argv, worldSize);
	}

	MPI_Finalize();
	return 0;
}

