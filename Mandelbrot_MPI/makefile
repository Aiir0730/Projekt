all: MPI fifo
MPI: Mandelbrot_MPI.cpp master_file.cpp slave_file.cpp 
	mpicxx -o mpi Mandelbrot_MPI.cpp 
fifo: test_fifo_receiver.cpp
	g++ -o test_fifo test_fifo_receiver.cpp
