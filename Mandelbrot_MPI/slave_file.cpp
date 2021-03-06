#include "structs.h"
#include <iostream>
#include <mpi.h>
#include <cmath>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#define MANDELBROT 1
#define JULIA 2

int slave(int argc, char* argv[]);
void packSlave();
void unpackSlave();
void doMath(int x0, int y0, int depth);

float MIN_X = -2.5f;
float MAX_X = 2.5f;
float MIN_Y = -2.5f;
float MAX_Y = 2.5f;
float ZOOM_PLUS = 0.01f;
float ZOOM_MINUS = 0.01f;

int fraktal;
int temp;
float cx;
float cy;

//funkcja licz¹ca normê wektora [x, y]
float magnitude(double x, double y);

//funkcja licz¹ca odcieñ piksela (px,py), depth - liczba wyrazów ci¹gu branych pod uwagê przy sprawdzaniu zbie¿noœci
float convergenceMandelbrot(double px, double py, int depth);
float convergenceJulia(double px, double py, int depth);

int slave(int argc, char* argv[], int worldSize)
{
    	int y0, x0;
	//std::cout << "Slave argc: "<< argc << " argv[0]: " << argv[0] << "\n";

	if (argc != 14) return -1;
	int taskPerThread = atoi(argv[2]);
	packageMaster2Slave.x = atoi(argv[3]);
	x0 = atoi(argv[3]);
	y0 = atoi(argv[4]);
	
	fraktal = atoi(argv[9]);
	if (fraktal != 1 && fraktal != 2)
		return -1;
	cx = atof(argv[10]);
	cy = atof(argv[11]);
	ZOOM_MINUS = atof(argv[13]);

	
	float XX = MAX_X - MIN_X;
	float YY = MAX_Y - MIN_Y;
	float delta;
	if (x0 > y0)
	{
		//std::cout<<"lol\n";
		delta = (x0-y0)/(float)x0 * YY;
		//std::cout<<"delta: "<<delta<<"  YY: "<<YY<<" a: "<<abs(MIN_Y/YY)*delta<<"  b: "<<abs(MIN_Y/YY)<<" c: "<<(abs(MIN_Y/YY)*delta)<<"\n";
		if (MIN_Y/YY < 0) delta = -delta; // taki straszny workaround, bo nie wiem dlaczego abs(MIN_Y/YY) zwraca 0
		MIN_Y = MIN_Y + (MIN_Y/YY)*delta;
		if (MAX_Y/YY > 0) delta = -delta;
		MAX_Y = MAX_Y - (MAX_Y/YY)*delta;
	}
	else
	{
		//std::cout<<"lol2\n";
		delta = (y0-x0)/(float)y0 * XX;
		if (MIN_X/XX < 0) delta = -delta;
		MIN_X = MIN_X + abs(MIN_X/XX)*delta;
		if (MAX_X/XX > 0) delta = -delta;
		MAX_X = MAX_X - abs(MAX_X/XX)*delta;
	}

	//std::cout<<"MIN_X: "<<MIN_X<<"  MAX_X: "<<MAX_X<<"\n";
	//std::cout<<"MIN_Y: "<<MIN_Y<<"  MAX_Y: "<<MAX_Y<<"\n";


	int memberSize;
	int master2SlaveSize = 0;
	int slave2MasterSize = 0;
	int position = 0;

	int tasks = (worldSize - 1) * taskPerThread; // na ile podzadañ zostanie rozbite zadanie g³ówne
	if (tasks > y0) tasks = y0; // Zabezpieczenie przed zbyt dużą liczbą zadań
	temp = tasks;
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
		
	MPI_Pack_size(6, MPI_INT, MPI_COMM_WORLD, &memberSize);
	master2SlaveSize = memberSize; // master
	MPI_Pack_size(3, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
	master2SlaveSize += memberSize;
	buffRecv = (char*)malloc(master2SlaveSize);
	
	//------------------
	//----  okreœlenie potrzebnego rozmiaru bufora na komunikat slave2Master - bêdzie trzeba zrobiæ to jeszcze raz dla ostatniego taska (mo¿liwy inny rozmiar)
	MPI_Pack_size(3, MPI_INT, MPI_COMM_WORLD, &memberSize);
	slave2MasterSize = memberSize; // master
	MPI_Pack_size(3 * numOfPixels, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
	slave2MasterSize += memberSize;
	buffSend = (char*)malloc(slave2MasterSize);
	
	int prevNumOfFrame = 0;
	for (;;)
	{

		////std::cout<<"	SLAVE - wait for receive\n";
		MPI_Recv(buffRecv, master2SlaveSize, MPI_PACKED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
		////std::cout<<"	SLAVE -	received MPI_TAG: "<<status.MPI_TAG<<"\n";
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
		MPI_Unpack(buffRecv, master2SlaveSize, &position, &packageMaster2Slave.frameNumber, 1, MPI_INT, MPI_COMM_WORLD);

		if (packageMaster2Slave.frameNumber != prevNumOfFrame)
		{
			prevNumOfFrame = packageMaster2Slave.frameNumber;
			MIN_Y -= ZOOM_MINUS*MIN_Y;
			MIN_X -= ZOOM_MINUS*MIN_X;
			MAX_Y -= ZOOM_MINUS*MAX_Y;
			MAX_X -= ZOOM_MINUS*MAX_X;
			std::cout<<"SLAVE - next frame: "<<prevNumOfFrame<<"\n";
		}

		////std::cout<<"	SLAVE -	doMath()\n";
		// for pixels to calculate -> calculate
		doMath(x0, y0, packageMaster2Slave.depth);

		

		packageSlave2Master.jobID = packageMaster2Slave.jobID;
		packageSlave2Master.ymin = packageMaster2Slave.ymin;
		packageSlave2Master.ymax = packageMaster2Slave.ymax;		
	
		
		// MPI_PACK response
		////std::cout<<"	SLAVE -	packing results\n";
		position = 0;
		MPI_Pack(&packageSlave2Master.jobID, 1, MPI_INT, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageSlave2Master.ymin, 1, MPI_INT, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageSlave2Master.ymax, 1, MPI_INT, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);
		MPI_Pack(packageSlave2Master.colorR, numOfPixels, MPI_UNSIGNED_CHAR, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);
		MPI_Pack(packageSlave2Master.colorG, numOfPixels, MPI_UNSIGNED_CHAR, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);
		MPI_Pack(packageSlave2Master.colorB, numOfPixels, MPI_UNSIGNED_CHAR, buffSend, slave2MasterSize, &position, MPI_COMM_WORLD);

		// MPI_Send to master
		////std::cout<<"	SLAVE -	ready to send\n";
		MPI_Send(buffSend, position, MPI_PACKED, 0, WORKTAG, MPI_COMM_WORLD);
		////std::cout<<"	SLAVE -	sent\n";

	}

}


void doMath(int x0, int y0, int depth)
{
	double i, j, temp_MAX_Y;
	float shade;
	double  step;
	int x_pix;
	int y_pix;

	i = MIN_Y + (MAX_Y-MIN_Y)*packageMaster2Slave.ymin/y0;
	temp_MAX_Y = MIN_Y + (MAX_Y-MIN_Y)*packageMaster2Slave.ymax/y0;


	if (x0 > y0)
		step = (MAX_X - MIN_X) / x0;
	else
		step = (MAX_Y - MIN_Y) / y0;

	y_pix = packageMaster2Slave.ymin;
	////std::cout<<"temp_MAX_Y: "<<temp_MAX_Y<<"\n";
	while (i <= temp_MAX_Y && y_pix < packageMaster2Slave.ymax)
	{
		x_pix = 0;
		j = MIN_X;
		while (j <= MAX_X && x_pix < x0)
		{
			if (fraktal == MANDELBROT)
				shade = convergenceMandelbrot(j,i,depth);
			else if (fraktal == JULIA)
				shade = convergenceJulia(j,i,depth);


			packageSlave2Master.colorR[(y_pix-packageMaster2Slave.ymin)*x0+x_pix] = shade * packageMaster2Slave.colorR;
			packageSlave2Master.colorG[(y_pix-packageMaster2Slave.ymin)*x0+x_pix] = shade * packageMaster2Slave.colorG;
			packageSlave2Master.colorB[(y_pix-packageMaster2Slave.ymin)*x0+x_pix] = shade * packageMaster2Slave.colorB;
			j += step;
			++x_pix;
		}
		i += step;
		++y_pix;
		////std::cout<<"y_pix: "<<y_pix<<"  i: "<<i<<"\n";
	}
}


float magnitude(double x, double y)
{
	return pow(pow(x,2) + pow(y,2),0.5);
}

float convergenceJulia(double px, double py, int depth)
{
	////std::cout<<px<<"  "<<py<<"  "<<depth<<"\n";
	double zx = px, zy = py;
	double tempx, tempy;
	for (double a = 0; a < (double)depth; ++a)
	{

		//if (py > -0.5 && py < 0.5)
		////std::cout<<"a: "<<a<<"  zx: "<<zx<<"  zy: "<<zy<<"  px: "<<px<<"  py: "<<py<<"\n";
		if (magnitude(zx, zy) < 2.0f)
		{
			tempx = zx*zx - zy*zy + cx;
			tempy = 2.0f * zx * zy + cy;
			zx = tempx;
			zy = tempy;
		}
		else{
			////std::cout<<"\t"<<a/depth<<"  "<<a<<"  "<<depth<<"\n";
			return a/depth;	
		}
			
	}
	return 1;
}

float convergenceMandelbrot(double px, double py, int depth)
{
	////std::cout<<px<<"  "<<py<<"  "<<depth<<"\n";
	double zx = 0, zy = 0;
	double tempx, tempy;
	for (double a = 0; a < (double)depth; ++a)
	{

		//if (py > -0.5 && py < 0.5)
		////std::cout<<"a: "<<a<<"  zx: "<<zx<<"  zy: "<<zy<<"  px: "<<px<<"  py: "<<py<<"\n";
		if (magnitude(zx, zy) < 20.0f)
		{
			tempx = zx*zx - zy*zy + px;
			tempy = 2.0f * zx * zy + py;
			zx = tempx;
			zy = tempy;
		}
		else{
			////std::cout<<"\t"<<a/depth<<"  "<<a<<"  "<<depth<<"\n";
			return a/depth;	
		}
			
	}
	return 1;
}