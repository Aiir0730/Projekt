#pragma once

#include"structs.h"
#include"bitmap_image.hpp"
#include<mpi.h>
#include<cmath>
#include<iostream>
#include<stdlib.h>
#define WORKTAG 1
#define DIETAG 2
struct master2Slave packageMaster2Slave;
struct slave2Master packageSlave2Master;

bitmap_image image;
int y0;
int x0;
int master(int argc, char** argv);
void packMaster();
void unpackMaster();
void doNiceStuff();


int master(int argc, char** argv)
{
	std::cout<<"Master argc: "<<argc<<"  argv[0]: "<<argv[0]<<"\n";
	if (argc != 8) return -1;
	
	packageMaster2Slave.depth = atoi(argv[1]);
	int taskPerThread = atoi(argv[2]);
	packageMaster2Slave.x = atoi(argv[3]);
	packageMaster2Slave.ymin = 0;
	packageMaster2Slave.ymax = 0;
	x0 = atoi(argv[3]);	
	y0 = atoi(argv[4]);
	packageMaster2Slave.colorR = (unsigned char)atoi(argv[5]);
	packageMaster2Slave.colorG = (unsigned char)atoi(argv[6]);
	packageMaster2Slave.colorB = (unsigned char)atoi(argv[7]);
	
	image = bitmap_image(packageMaster2Slave.x, y0);
	
	int memberSize, msgSize, worldSize, worldRank;
	int master2SlaveSize = 0;
	int slave2MasterSize = 0;
	int position = 0;

	
	int tasks; // skoro wysylamy zadania az do (rowNo < y0) 
	//int tasks = (worldSize - 1) * taskPerThread; // na ile podzadañ zostanie rozbite zadanie g³ówne
	//if (tasks > y0) tasks = y0; // Zabezpieczenie przed zbyt dużą liczbą zadań

	int rowNo = 0; // numery wierszy obecnie rozpatrywanych, przy podziale na taski
	int numberOfRowsPerTask = y0 / tasks;
	tasks = ceil(y0/numberOfRowsPerTask); // psotor - prosty sposob zeby policzyc tyle taskow by bylo ok

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
	buffSend = (char*)malloc(master2SlaveSize);

	//------------------
	//----  okreœlenie potrzebnego rozmiaru bufora na komunikat slave2Master - bêdzie trzeba zrobiæ to jeszcze raz dla ostatniego taska (mo¿liwy inny rozmiar)
	MPI_Pack_size(3, MPI_INT, MPI_COMM_WORLD, &memberSize);
	slave2MasterSize += memberSize; // master
	MPI_Pack_size(3 * numOfPixels, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
	slave2MasterSize += memberSize;
	buffRecv = (char*)malloc(slave2MasterSize);

	int i = 0; // bedziemy tez traktowac jako jobID
	for (i; i < worldSize - 1; i++)
	{
		packageMaster2Slave.jobID = i;
		packageMaster2Slave.ymin = rowNo;
		packageMaster2Slave.ymax = rowNo + numberOfRowsPerTask;

		rowNo += numberOfRowsPerTask;
		position = 0;
		MPI_Pack(&packageMaster2Slave.jobID, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.x, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.ymin, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.ymax, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.depth, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.colorR, 1, MPI_UNSIGNED_CHAR, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.colorG, 1, MPI_UNSIGNED_CHAR, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.colorB, 1, MPI_UNSIGNED_CHAR, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);

		MPI_Send(buffSend, position, MPI_PACKED, i + 1, WORKTAG, MPI_COMM_WORLD);
		tasks--;
	}
	
	while(tasks > 0)
	{
		//recv od slave'ów

		MPI_Recv(buffRecv, slave2MasterSize, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		position = 0;
		MPI_Get_count(&status, MPI_PACKED, &msgSize);

		MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.jobID, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.ymin, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.ymax, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, slave2MasterSize, &position, packageSlave2Master.colorR, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, slave2MasterSize, &position, packageSlave2Master.colorG, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, slave2MasterSize, &position, packageSlave2Master.colorB, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
		
		//ustalenie nastêpnego taska
		position = 0;
		packageMaster2Slave.jobID = i;
		packageMaster2Slave.ymin = rowNo;
		packageMaster2Slave.ymax = rowNo + numberOfRowsPerTask;
		
		rowNo += numberOfRowsPerTask;
			
		
		//wys³anie nastêpnego taska
		MPI_Pack(&packageMaster2Slave.jobID, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.x, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.ymin, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.ymax, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.depth, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.colorR, 1, MPI_UNSIGNED_CHAR, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.colorG, 1, MPI_UNSIGNED_CHAR, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.colorB, 1, MPI_UNSIGNED_CHAR, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);

		MPI_Send(buffSend, position, MPI_PACKED, status.MPI_SOURCE, WORKTAG, MPI_COMM_WORLD);

		//
		//	przetwarzanie odebranenych rezultatów, wstawienie do tablicy bitmapy w odpowiednim miejscu
		//
		doNiceStuff(); // 

		--tasks;
		++i;
	}
	//-------------
	//----  oczekiwanie na zakoñczenie ostatnich podzadañ, po rozdaniu wszystkich podzadañ z puli,
	for (int rank = 1; rank <= worldSize; rank++)
	{
		
		//recv od slave'ów
		MPI_Recv(buffRecv, slave2MasterSize, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		position = 0;
		MPI_Get_count(&status, MPI_PACKED, &msgSize);

		MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.jobID, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.ymin, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.ymax, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.colorR, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.colorG, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
		MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.colorB, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
		//
		//	przetwarzanie odebranenych rezultatów, wstawienie do tablicy bitmapy w odpowiednim miejscu
		//
		doNiceStuff();
	}
	for (int rank = 1; rank <= worldSize; rank++)
	{
		MPI_Send(0, 0, MPI_PACKED, rank, DIETAG, MPI_COMM_WORLD);
	}
}

void doNiceStuff()
{
	unsigned int i,j;
	unsigned char r,g,b;
	for (i=packageSlave2Master.ymin;i<packageSlave2Master.ymax && i < y0;++i)  //  i<y0 zapewni w ostatnim tasku że będzie ok
	{
		for (j = 0; j < x0; ++j)
		{
			r = packageSlave2Master.colorR[(i-ymin)*x0+j];
			g = packageSlave2Master.colorG[(i-ymin)*x0+j];
			b = packageSlave2Master.colorB[(i-ymin)*x0+j];
			image.set_pixel(j,i,r,g,b);
		}

	}		
	
}






















