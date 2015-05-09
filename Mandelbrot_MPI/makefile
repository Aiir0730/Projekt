all: final master slave
final: Mandelbrot_MPI.cpp master_file.o slave_file.o structs.h globals.h
	mpicxx -o mpi Mandelbrot_MPI.cpp 
master: master_file.cpp
	mpicxx -o master_file.o master_file.cpp
slave: slave_file.cpp
	mpicxx -o slave_file.o slave_file.cpp
