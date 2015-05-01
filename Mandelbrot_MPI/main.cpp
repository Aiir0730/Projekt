// Tutorial - http://mpitutorial.com/tutorials/mpi-hello-world/
// Przykłady - http://people.sc.fsu.edu/~jburkardt/cpp_src/mpi/mpi.html

#include <iostream>
#include <mpi.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // Uzyskanie rangi procesora
    int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    // Uzyskanie nazwy procesora
    char processorName[MPI_MAX_PROCESSOR_NAME];
    int nameLen;
    MPI_Get_processor_name(processorName, &nameLen);
	
    if (worldRank == 0)
    std::cout << "Master -  " << processorName
    		<< ", rank:" << worldRank << " worldSize " << worldSize << " procesorów\n";

    else
    std::cout << "Slave -  " << processorName
                << ", rank:" << worldRank << " worldSize " << worldSize << " procesorów\n";


    // Kończenie środowiska MPI. Nie wiem jeszcze, co dokładnie robi,
    // ale nie jest to najistotniejsza w tej chwili kwestia.
    MPI_Finalize();
    return 0;
}

