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

struct packMasterToSlave
{
	int jobID;
	int x;
	int ymin;
	int ymax;
	int depth;
	char colorR;
	char colorG;
	char colorB;
} packMasterToSlave;

struct packSlaveToMaster
{
	int jobID;
	int ymin;
	int ymax;
	char* colorR;
	char* colorG;
	char* colorB;
} packSlaveToMaster;

float magnitude(double x, double y);
int convergence(double px, double py, int depth);
void RenderScene(int depth);	// Wspó³rzêdne przy rysowaniu rozpatrujemy w zakresie x(MIN_X,MAX_X) y(MIN_Y,MAX_Y)

// G³ówny punkt wej¶cia programu. Program dzia³a w trybie konsoli
int main(int argc, char *argv[])
{
	int depth = 32;
	if (argc > 1) depth = atoi(argv[1]);
	
	int worldSize, worldRank;
	
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
	
	if (worldRank == 0)
	{
		// master
	}
	else
	{
		// slave
	}
	
	RenderScene(depth);
	
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

