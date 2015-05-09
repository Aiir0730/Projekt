#include "master_file.cpp"
#include "slave_file.cpp"
#include<stdio.h>

int master(int argc, char* argv[]);
int slave(int argc, char* argv[]);


//inicjalizacja zmiennych globalnych
struct master2Slave packageMaster2Slave;
struct slave2Master packageSlave2Master;

int DIETAG = 2;
int WORKTAG = 1;





// G³ówny punkt wej¶cia programu. Program dzia³a w trybie konsoli
int main(int argc, char *argv[])	// argv: depth, taskPerThread, x, y, colorR, colorG, colorB
{
	// parametry generowanego fraktala zostan¹ przekazane przez kolejne argumenty wywo³ania
	if (argc != 8) 
	{
		printf("za malo argumentow\n");
		return -1;
	
	}
	int worldSize, worldRank;
	MPI_Init(&argc, &argv);
    	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

	if (worldRank == 0)
	{
		master(argc, argv);
	}
	else
	{
		slave(argc, argv);
	}

	MPI_Finalize();
	return 0;
}

