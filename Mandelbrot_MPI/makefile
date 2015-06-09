all: MPI
MPI: Mandelbrot_MPI.cpp master_file.cpp slave_file.cpp 
	mpicxx -o mpi Mandelbrot_MPI.cpp -lrt -std=c++11
