#include "structs.h"
#include "bitmap_image.hpp"
#include <mpi.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h> 
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h>
#include <arpa/inet.h>
#include <zmq.h>
#include <assert.h>

const int KROKI = 10;
const int PORT = 9999;
#define SRV_IP "127.0.0.1"

struct msgt
{
	char task_id[50];
	int progress;
	bool is_done;
};

void *context;
void *responder;

int numOfTasks;
int numOfFrames;
int fifo_handler;
int doneTasks = 0;
bitmap_image image;
int master(int argc, char** argv);

sockaddr_in adr_moj, adr_serw, adr_x; 
int s, i, slen = sizeof(adr_serw), snd, blen = sizeof(msgt), rec;
msgt msg;

void packMaster();
void unpackMaster();
void doNiceStuff(int x0, int y0, int ymin, int ymax);
void blad(char *s);

int master(int argc, char** argv, int worldSize)
{

	// s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	// if(s < 0) blad("socket"); 

	//s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//if(s < 0) blad("socket"); 

	// printf("Gniazdko %d utworzone\n", s); 
	// memset((char *) &adr_serw, 0, sizeof(adr_serw)); 
	// adr_serw.sin_family = AF_INET; 
	// adr_serw.sin_port = htons(PORT); 
	// if (inet_aton(SRV_IP, &adr_serw.sin_addr) == 0)	// Tu zmienione z argv[1] na SRV_IP
	// { 
	// 	fprintf(stderr, "inet_aton() failed\n"); 
	// 	_exit(1); 
	// }


	// Socket to talk to clients
	
	context = zmq_ctx_new ();
	responder = zmq_socket (context, ZMQ_REP);
	int rc = zmq_bind (responder, "tcp://*:9999");
	assert (rc == 0);
	 

    int y0, x0;

	if (argc != 14) return -1;
		
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
	numOfFrames = atoi(argv[12]);
	image = bitmap_image(packageMaster2Slave.x, y0);
	char filename[50];
	sprintf(filename,"%s.bmp", argv[8]);
	sprintf(msg.task_id,"%s",argv[8]);
	int memberSize, msgSize;
	int master2SlaveSize = 0;
	int slave2MasterSize = 0;
	int position = 0;

	int tasks = (worldSize - 1) * taskPerThread; // na ile podzadañ zostanie rozbite zadanie g³ówne
	if (tasks > y0) tasks = y0; // Zabezpieczenie przed zbyt dużą liczbą zadań

	int rowNo = 0; // numery wierszy obecnie rozpatrywanych, przy podziale na taski
	int numberOfRowsPerTask = y0 / tasks;
	tasks = ceil((double)y0/(double)numberOfRowsPerTask); // psotor - prosty sposob zeby policzyc tyle taskow by bylo ok
	numOfTasks = tasks;

	char* buffSend;
	char* buffRecv;
	MPI_Status status;

	int numOfPixels = numberOfRowsPerTask * x0;

 	packageSlave2Master.colorR = (unsigned char*)malloc(numOfPixels*sizeof(char));
	packageSlave2Master.colorG = (unsigned char*)malloc(numOfPixels*sizeof(char));
	packageSlave2Master.colorB = (unsigned char*)malloc(numOfPixels*sizeof(char));

	//------------------
	//----  okreœlenie potrzebnego rozmiaru bufora na komunikat master2Slave
	MPI_Pack_size(6, MPI_INT, MPI_COMM_WORLD, &memberSize);
	master2SlaveSize = memberSize; // master
	MPI_Pack_size(3, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
	master2SlaveSize += memberSize;
	buffSend = (char*)malloc(master2SlaveSize);
	//------------------
	//----  okreœlenie potrzebnego rozmiaru bufora na komunikat slave2Master - bêdzie trzeba zrobiæ to jeszcze raz dla ostatniego taska (mo¿liwy inny rozmiar)
	MPI_Pack_size(3, MPI_INT, MPI_COMM_WORLD, &memberSize);
	slave2MasterSize = memberSize; // master
	MPI_Pack_size(3 * numOfPixels, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
	slave2MasterSize += memberSize;
	buffRecv = (char*)malloc(slave2MasterSize);

	for (int j = 0; j< numOfFrames;++j)
	{
		rowNo = 0;
		packageMaster2Slave.frameNumber = j;
		image = bitmap_image(packageMaster2Slave.x, y0);
		tasks = numOfTasks;
		for (i = 0; i < worldSize - 1; i++)
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
			MPI_Pack(&packageMaster2Slave.frameNumber, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);

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
			MPI_Pack(&packageMaster2Slave.frameNumber, 1, MPI_INT, buffSend, master2SlaveSize, &position, MPI_COMM_WORLD);
			MPI_Send(buffSend, position, MPI_PACKED, status.MPI_SOURCE, WORKTAG, MPI_COMM_WORLD);
			//
			//	przetwarzanie odebranenych rezultatów, wstawienie do tablicy bitmapy w odpowiednim miejscu
			//
			doNiceStuff(x0, y0, packageSlave2Master.ymin, packageSlave2Master.ymax);

			--tasks;
			++i;
		}
		//-------------
		//----  oczekiwanie na zakoñczenie ostatnich podzadañ, po rozdaniu wszystkich podzadañ z puli,
		for (int rank = 1; rank < worldSize; rank++)
		{
			MPI_Recv(buffRecv, slave2MasterSize, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			position = 0;
			MPI_Get_count(&status, MPI_PACKED, &msgSize);
			MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.jobID, 1, MPI_INT, MPI_COMM_WORLD);
			MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.ymin, 1, MPI_INT, MPI_COMM_WORLD);
			MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.ymax, 1, MPI_INT, MPI_COMM_WORLD);
			MPI_Unpack(buffRecv, slave2MasterSize, &position, packageSlave2Master.colorR, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
			MPI_Unpack(buffRecv, slave2MasterSize, &position, packageSlave2Master.colorG, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
			MPI_Unpack(buffRecv, slave2MasterSize, &position, packageSlave2Master.colorB, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
			doNiceStuff(x0, y0, packageSlave2Master.ymin, packageSlave2Master.ymax);
		}
		char buf[50];
		sprintf(buf, "%s_%d.bmp",argv[8],j);
		image.save_image(buf);
	}
	for (int rank = 1; rank < worldSize; rank++)
	{
		MPI_Send(0, 0, MPI_PACKED, rank, DIETAG, MPI_COMM_WORLD);
	}
	
	close(s); 
}

void doNiceStuff(int x0, int y0, int ymin, int ymax)
{
	static int done_tasks = 0;
	unsigned int i, j;
	unsigned char r, g, b;
	//std::cout<<"frame: "<<packageMaster2Slave.frameNumber<<"  ymin: "<<ymin<<"  ymax: "<<ymax<<"\n";
	for (i = ymin; (i < ymax) && (i < y0); ++i)  //  i<y0 zapewni w ostatnim tasku że będzie ok
	{
		for (j = 0; j < x0; ++j)
		{
			r = packageSlave2Master.colorR[(i-ymin)*x0+j];
			g = packageSlave2Master.colorG[(i-ymin)*x0+j];
			b = packageSlave2Master.colorB[(i-ymin)*x0+j];
			image.set_pixel(j,i,r,g,b);
		}
	}
	++done_tasks;
	//msg.task_id = argv[8];
	msg.progress = (done_tasks * 100)/(numOfFrames * numOfTasks);
	if (done_tasks == numOfTasks) msg.is_done = true;
	else msg.is_done = false;


	char message[20];//moje
	sprintf(message, "%s:%d:%d", msg.task_id, msg.progress, msg.is_done);//moje
	zmq_send (responder, message, 20, 0);
	//snd = sendto(s, &msg, blen, 0,(struct sockaddr *) &adr_serw, (socklen_t) slen);
	//snd = sendto(s, message, strlen(message), 0, (struct sockaddr*) &adr_serw, (socklen_t) slen);
	if(snd < 0) blad("sendto()"); 

}
 
void blad(char *s)
{ 
	perror(s); 
	_exit(1); 
}
