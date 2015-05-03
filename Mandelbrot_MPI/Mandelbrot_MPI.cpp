#include <iostream>
#include <cstdlib>
#include <cmath>
#include <mpi.h>

#include "master_file.cpp"
#include "slave_file.cpp"

int master(int argc, char* argv[]);
int slave(int argc, char* argv[]);



// G³ówny punkt wej¶cia programu. Program dzia³a w trybie konsoli
int main(int argc, char *argv[])	// argv: depth, taskPerThread, x, y, colorR, colorG, colorB
{
	// parametry generowanego fraktala zostan¹ przekazane przez kolejne argumenty wywo³ania
	if (argc != 8) return -1;

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

