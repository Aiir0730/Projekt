#include <iostream>
#include <cstdlib>
#include <cmath>
#include <mpi.h>

float MIN_X = -7.0f;
float MAX_X = 7.0f;
float MIN_Y = -7.0f;
float MAX_Y = 7.0f;
float ZOOM_PLUS = 0.3f;
float ZOOM_MINUS = 0.3f;

int horiz;
int vert;

struct masterToSlave
{
	int jobID;
	int x;
	int ymin;
	int ymax;
	int depth;
	unsigned char colorR;
	unsigned char colorG;
	unsigned char colorB;
} packageMaster2Slave;

struct slaveToMaster
{
	int jobID;
	int ymin;
	int ymax;
	unsigned char* colorR;
	unsigned char* colorG;
	unsigned char* colorB;
} packageSlave2Master;

float magnitude(double x, double y);
int convergence(double px, double py, int depth);
void RenderScene(int depth);	// Wspó³rzêdne przy rysowaniu rozpatrujemy w zakresie x(MIN_X,MAX_X) y(MIN_Y,MAX_Y)

// G³ówny punkt wej¶cia programu. Program dzia³a w trybie konsoli
int main(int argc, char *argv[])
{
	/*int depth = 32;
	if (argc > 1) depth = atoi(argv[1]);
	*/
	int worldSize, worldRank;
	
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
	
	int memberSize, sumSize = 0, position = 0;
	char* buff;
	int msgSize;
	MPI_Status status;
	
		
	if (worldRank == 0)
	{	
		MPI_Pack_size(5, MPI_INT, MPI_COMM_WORLD, &memberSize);
		sumSize += memberSize; // master
		MPI_Pack_size(3, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
		sumSize += memberSize;
		buff = (char*)malloc(sumSize);
		
		packageMaster2Slave.jobID = 123;
		packageMaster2Slave.x = 1920;
		packageMaster2Slave.ymin = 1;
		packageMaster2Slave.ymax = 10;
		packageMaster2Slave.depth = 5;
		packageMaster2Slave.colorR = 64;
		packageMaster2Slave.colorG = 128;
		packageMaster2Slave.colorB = 192;
		
		MPI_Pack(&packageMaster2Slave.jobID, 1, MPI_INT, buff, sumSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.x, 1, MPI_INT, buff, sumSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.ymin, 1, MPI_INT, buff, sumSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.ymax, 1, MPI_INT, buff, sumSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.depth, 1, MPI_INT, buff, sumSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.colorR, 1, MPI_UNSIGNED_CHAR, buff, sumSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.colorG, 1, MPI_UNSIGNED_CHAR, buff, sumSize, &position, MPI_COMM_WORLD);
		MPI_Pack(&packageMaster2Slave.colorB, 1, MPI_UNSIGNED_CHAR, buff, sumSize, &position, MPI_COMM_WORLD);
		
		MPI_Send(buff, position, MPI_PACKED, 1, 23, MPI_COMM_WORLD);
		
	}
	else
	{		
		std::cout<<"LOL "<<worldRank<<"\n";	
		buff = (char*)malloc(sizeof(packageMaster2Slave));
		sumSize = sizeof(packageMaster2Slave);
		MPI_Recv(buff, sumSize, MPI_PACKED, 0, 23, MPI_COMM_WORLD, &status);
		position = 0;
		MPI_Get_count(&status, MPI_PACKED, &msgSize);
		
		MPI_Unpack(buff, msgSize, &position, &packageMaster2Slave.jobID, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buff, msgSize, &position, &packageMaster2Slave.x, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buff, msgSize, &position, &packageMaster2Slave.ymin, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buff, msgSize, &position, &packageMaster2Slave.ymax, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buff, msgSize, &position, &packageMaster2Slave.depth, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buff, msgSize, &position, &packageMaster2Slave.colorR, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
		MPI_Unpack(buff, msgSize, &position, &packageMaster2Slave.colorG, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
		MPI_Unpack(buff, msgSize, &position, &packageMaster2Slave.colorB, 1, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
		
		std::cout << packageMaster2Slave.jobID << std::endl;
		std::cout << packageMaster2Slave.x << std::endl;
		std::cout << packageMaster2Slave.ymin << std::endl;
		std::cout << packageMaster2Slave.ymax << std::endl;
		std::cout << packageMaster2Slave.depth << std::endl;
		std::cout << (int)packageMaster2Slave.colorR << std::endl;
		std::cout << (int)packageMaster2Slave.colorG << std::endl;
		std::cout << (int)packageMaster2Slave.colorB << std::endl;
		
	
	
		// slave
	}
	
	//RenderScene(depth);
	
	MPI_Finalize();
	return 0;
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

void RenderScene(int depth)
{
	double i = MIN_X, j = MIN_Y;
	float shade;
	double  step;
	
	if (horiz>vert)
		step = (MAX_X - MIN_X) / horiz;
	else
		step = (MAX_Y - MIN_Y) / vert;

	while (i <= MAX_X)
	{
		j = MIN_Y;
		while (j <= MAX_Y)
		{
			shade = 8 * magnitude(i, j) / 256.0f;
			j += step;
		}
		i += step;
	}
}

