#pragma once

#include"structs.h"
#include<iostream>
#include<mpi.h>
#include<cmath>
#include<stdlib.h>
#define WORKTAG 1
#define DIETAG 2
struct master2Slave packageMaster2Slave;
struct slave2Master packageSlave2Master;

int slave(int argc, char* argc[]);
void packSlave();
void unpackSlave();



int slave(int argc, char* argc[])
{
	std::cout<<"Slave argc: "<<argc<<"  argv[0]: "<<argv[0]<<"\n";
	
	if (argc != 8) return -1;
	int taskPerThread = atoi(argv[2]);
	packageMaster2Slave.x = atoi(argv[3]);
	
	int y0 = atoi(argv[4]);
	
	int memberSize, msgSize, worldSize, worldRank;
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



	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

	packageSlave2Master.colorR = (unsigned char*)malloc(numOfPixels*sizeof(char));
	packageSlave2Master.colorG = (unsigned char*)malloc(numOfPixels*sizeof(char));
	packageSlave2Master.colorB = (unsigned char*)malloc(numOfPixels*sizeof(char));

	//------------------
	//----  okreœlenie potrzebnego rozmiaru bufora na komunikat master2Slave
	MPI_Pack_size(5, MPI_INT, MPI_COMM_WORLD, &memberSize);
	master2SlaveSize += memberSize; // master
	MPI_Pack_size(3, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
	master2SlaveSize += memberSize;
	buffRecv = (char*)malloc(master2SlaveSize);

	//------------------
	//----  okreœlenie potrzebnego rozmiaru bufora na komunikat slave2Master - bêdzie trzeba zrobiæ to jeszcze raz dla ostatniego taska (mo¿liwy inny rozmiar)
	MPI_Pack_size(3, MPI_INT, MPI_COMM_WORLD, &memberSize);
	slave2MasterSize += memberSize; // master
	MPI_Pack_size(3 * numOfPixels, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
	slave2MasterSize += memberSize;
	buffSend = (char*)malloc(slave2MasterSize);

	
	for (;;)
	{
		// MPI_Recv
		// if TAG == DIETAG -> return
		// MPI_UNPACK
		// for pixels to calculate -> calculate
		// MPI_PACK reponse
		// MPI_Send to master

	}

	/*
	// temporary rubbish, MPI_Unpack jest ok
	//buffSend = (char*)malloc(sizeof(packageMaster2Slave));
	sumSize = sizeof(packageMaster2Slave);
	MPI_Recv(buffRecv, master2SlaveSize, MPI_PACKED, 0, 23, MPI_COMM_WORLD, &status);
	position = 0;
	MPI_Get_count(&status, MPI_PACKED, &msgSize);

	MPI_Unpack(buffRecv, msgSize, &position, &packageMaster2Slave.jobID, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, msgSize, &position, &packageMaster2Slave.x, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, msgSize, &position, &packageMaster2Slave.ymin, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, msgSize, &position, &packageMaster2Slave.ymax, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, msgSize, &position, &packageMaster2Slave.depth, 1, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, msgSize, &position, &packageMaster2Slave.colorR, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, msgSize, &position, &packageMaster2Slave.colorG, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
	MPI_Unpack(buffRecv, msgSize, &position, &packageMaster2Slave.colorB, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);

	*/
	
	
}
