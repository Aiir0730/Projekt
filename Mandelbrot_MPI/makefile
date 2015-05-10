all: final
final: Mandelbrot_MPI.cpp master_file.cpp slave_file.cpp structs.h 
	mpicxx -o mpi Mandelbrot_MPI.cpp 
master: master_file.cpp
	mpicxx -o master_file.o master_file.cpp
slave: slave_file.cpp
	mpicxx -o slave_file.o slave_file.cpp
