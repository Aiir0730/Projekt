#include "structs.h"
#include <iostream>
#include <mpi.h>
#include <cmath>
#include <cstdlib>



int slave(int argc, char* argv[]);
void packSlave();
void unpackSlave();
void doMath(int x0, int y0, int depth);

float MIN_X = -7.0f;
float MAX_X = 7.0f;
float MIN_Y = -7.0f;
float MAX_Y = 7.0f;
float ZOOM_PLUS = 0.3f;
float ZOOM_MINUS = 0.3f;

//funkcja licz¹ca normê wektora [x, y]
float magnitude(double x, double y);

//funkcja licz¹ca odcieñ piksela (px,py), depth - liczba wyrazów ci¹gu branych pod uwagê przy sprawdzaniu zbie¿noœci
int convergence(double px, double py, int depth);

int slave(int argc, char* argv[], int worldSize)
{
    	int y0, x0;
	std::cout << "Slave argc: "<< argc << " argv[0]: " << argv[0] << "\n";

	if (argc != 8) return -1;
	int taskPerThread = atoi(argv[2]);
	packageMaster2Slave.x = atoi(argv[3]);
	x0 = atoi(argv[3]);
	y0 = atoi(argv[4]);

	int memberSize;
	int master2SlaveSize = 0;
	int slave2MasterSize = 0;
	int position = 0;

	int tasks = (worldSize - 1) * taskPerThread; // na ile podzadañ zostanie rozbite zadanie g³ówne
	if (tasks > y0) tasks = y0; // Zabezpieczenie przed zbyt dużą liczbą zadań

	int rowNo = 0; // numery wierszy obecnie rozpatrywanych, przy podziale na taski, licz¹c od góry
	int numberOfRowsPerTask = y0 / tasks;

	char* buffSend;
	char* buffRecv;
	MPI_Status status;

	int numOfPixels = numberOfRowsPerTask * packageMaster2Slave.x;

	//MPI_Init(&argc, &argv);
	//MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	//MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

	packageSlave2Master.colorR = (unsigned char*)malloc(numOfPixels*sizeof(char));
	packageSlave2Master.colorG = (unsigned char*)malloc(numOfPixels*sizeof(char));
	packageSlave2Master.colorB = (unsigned char*)malloc(numOfPixels*sizeof(char));

	//------------------
	//----  okreœlenie potrzebnego rozmiaru bufora na komunikat master2Slave
	std::cout<<"	LOL1\n";	
	MPI_Pack_size(5, MPI_INT, MPI_COMM_WORLD, &memberSize);
	master2SlaveSize = memberSize; // master
	MPI_Pack_size(3, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
	master2SlaveSize += memberSize;
	buffRecv = (char*)malloc(master2SlaveSize);
	std::cout<<"	LOL2	\n";
	//------------------
	//----  okreœlenie potrzebnego rozmiaru bufora na komunikat slave2Master - bêdzie trzeba zrobiæ to jeszcze raz dla ostatniego taska (mo¿liwy inny rozmiar)
	MPI_Pack_size(3, MPI_INT, MPI_COMM_WORLD, &memberSize);
	slave2MasterSize = memberSize; // master
	MPI_Pack_size(3 * numOfPixels, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
	slave2MasterSize += memberSize;
	buffSend = (char*)malloc(slave2MasterSize);
	std::cout<<"	LOL3	\n";
	for (;;)
	{
		std::cout<<"	LOL4	\n";
		MPI_Recv(buffRecv, master2SlaveSize, MPI_PACKED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		std::cout<<"	LOL4.5	\n";
		if (status.MPI_TAG == DIETAG)
			return 0;

		position = 0;
		MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.jobID, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.x, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.ymin, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.ymax, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.depth, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.colorR, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.colorG, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.colorB, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);

		std::cout<<"	LOL5	\n";
		// for pixels to calculate -> calculate
		doMath(x0, y0, packageMaster2Slave.depth);

		// MPI_PACK response
		position = 0;
		MPI_Pack(&packageSlave2Master.jobID, 1, MPI_INT, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageSlave2Master.ymin, 1, MPI_INT, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageSlave2Master.ymax, 1, MPI_INT, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);
		MPI_Pack(packageSlave2Master.colorR, numOfPixels, MPI_UNSIGNED_CHAR, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);
		MPI_Pack(packageSlave2Master.colorG, numOfPixels, MPI_UNSIGNED_CHAR, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);
		MPI_Pack(packageSlave2Master.colorB, numOfPixels, MPI_UNSIGNED_CHAR, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);

		// MPI_Send to master
		MPI_Send(buffSend, position, MPI_PACKED, 0, WORKTAG, MPI_COMM_WORLD);
	}

	/*
	// temporary rubbish, MPI_Unpack jest ok
	//buffSend = (char*)malloc(sizeof(packageMaster2Slave));
	sumSize = sizeof(packageMaster2Slave);
	MPI_Recv(buffRecv, master2SlaveSize, MPI_PACKED, 0, 23, MPI_COMM_WORLD, &status);
	position = 0;
	MPI_Get_count(&status, MPI_PACKED, &master2SlaveSize);

	MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.jobID, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.x, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.ymin, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.ymax, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.depth, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.colorR, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.colorG, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.colorB, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);

	*/
}

void doMath(int x0, int y0, int depth)
{
	double i = MIN_X, j, temp_MAX_Y;
	float shade;
	double  step;
	int x_pix;
	int y_pix;

	j = MIN_Y + (MAX_Y-MIN_Y)*packageMaster2Slave.ymin/y0;
	temp_MAX_Y = MIN_Y + (MAX_Y-MIN_Y)*packageMaster2Slave.ymax/y0;


	if (x0 > y0)
		step = (MAX_X - MIN_X) / x0;
	else
		step = (MAX_Y - MIN_Y) / y0;

	y_pix = packageMaster2Slave.ymin;

	while (i <= temp_MAX_Y && y_pix < packageMaster2Slave.ymax)
	{
		x_pix = 0;
		j = MIN_X;
		while (j <= MAX_X && x_pix < x0)
		{
			shade = (float)convergence(j, i, depth) / (float) depth;
			packageSlave2Master.colorR[(y_pix-packageMaster2Slave.ymin)*x0+x_pix] = shade * packageMaster2Slave.colorR;
			packageSlave2Master.colorG[(y_pix-packageMaster2Slave.ymin)*x0+x_pix] = shade * packageMaster2Slave.colorG;
			packageSlave2Master.colorB[(y_pix-packageMaster2Slave.ymin)*x0+x_pix] = shade * packageMaster2Slave.colorB;
			j += step;
			++x_pix;
		}
		i += step;
		++y_pix;
	}
}

float magnitude(double x, double y)
{
	return sqrt(pow(x, 2.0f) + pow(y, 2.0f));
}

int convergence(double px, double py, int depth)
{
	double zx = px, zy = py;
	double tempx, tempy;

	for (int a = 0; a < depth; ++a)
	{
		if (magnitude(zx, py) < 20.0f)
		{
			tempx = pow(zx, 2.0f) - pow(zy, 2.0f) + px;
			tempy = 2.0f * zx * zy + py;
			zx = tempx;
			zy = tempy;
		}
		else
			return a;
	}
	return depth / 2.0f;
}
