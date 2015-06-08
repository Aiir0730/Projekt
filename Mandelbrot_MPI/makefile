all: MPI
MPI: Mandelbrot_MPI.cpp master_file.cpp slave_file.cpp structs.h
	mpicxx -o mpi Mandelbrot_MPI.cpp -lrt
