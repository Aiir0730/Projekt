#include <iostream>
#include <cstdlib>
#include <cmath>
#include <mpi.h>

const int WORKTAG = 0;
const int DIETAG = -1;

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

//funkcja licz¹ca normê wektora [x, y]
float magnitude(double x, double y);

//funkcja licz¹ca odcieñ piksela (px,py), depth - liczba wyrazów ci¹gu branych pod uwagê przy sprawdzaniu zbie¿noœci	
int convergence(double px, double py, int depth);


void renderScene(double x, double y, int depth);	// Wspó³rzêdne przy rysowaniu rozpatrujemy w zakresie x(MIN_X,MAX_X) y(MIN_Y,MAX_Y)

// G³ówny punkt wej¶cia programu. Program dzia³a w trybie konsoli
int main(int argc, char *argv[])	// argv: depth, taskPerThread, x, y, colorR, colorG, colorB
{
	// parametry generowanego fraktala zostan¹ przekazane przez kolejne argumenty wywo³ania
	if (argc != 8) return -1; 
	packageMaster2Slave.jobID = 0;
	packageMaster2Slave.depth = atoi(argv[1]);
	int taskPerThread = atoi(argv[2]);
	packageMaster2Slave.x = atoi(argv[3]);
	packageMaster2Slave.ymin = 0;
	packageMaster2Slave.ymax = 0;
	int y0 = atoi(argv[4]);
	packageMaster2Slave.colorR = (unsigned char)atoi(argv[5]);
	packageMaster2Slave.colorG = (unsigned char)atoi(argv[6]);
	packageMaster2Slave.colorB = (unsigned char)atoi(argv[7]);
	
	
	int memberSize, msgSize, worldSize, worldRank;
	int master2SlaveSize = 0;
	int slave2MasterSize = 0;
	int position = 0;
	
	int tasks = (worldSize - 1) * taskPerThread; // na ile podzadañ zostanie rozbite zadanie g³ówne
	
	int rowNo = 0; // numery wierszy obecnie rozpatrywanych, przy podziale na taski, licz¹c od góry
	int numberOfRowsPerTask = y0/tasks;
	int sizeOfLastTask;
	int slaveWithLastTask;
	
	char* buffSend;
	char* buffRecv;
	MPI_Status status;
	
	int numOfPixels = numberOfRowsPerTask*packageMaster2Slave.x;
	
	buffRecv = (char*)malloc (sumSize)); // 
			
	
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
	
	char* packageSlave2Master.colorR = (char*)malloc(numOfPixels*sizeof(char));
	char* packageSlave2Master.colorG = (char*)malloc(numOfPixels*sizeof(char));
	char* packageSlave2Master.colorB = (char*)malloc(numOfPixels*sizeof(char));
			
		
	if (worldRank == 0)
	{	
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
		master2SlaveSize += memberSize; // master
		MPI_Pack_size(3*numOfPixels, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
		master2SlaveSize += memberSize;
		buffRecv = (char*)malloc(slave2MasterSize);
		
		//---------------------
		//----  przydzielenie po jednym zadaniu ka¿demu slave
		//---------------------
		int i = 0; // bedziemy tez traktowac jako jobID
		for (; i < worldSize - 1; i++)
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
			MPI_Pack(&packageMaster2Slave.colorR, 1, MPI_UNSIGNED_CHAR, master2SlaveSize, sumSize, &position, MPI_COMM_WORLD);
			MPI_Pack(&packageMaster2Slave.colorG, 1, MPI_UNSIGNED_CHAR, master2SlaveSize, sumSize, &position, MPI_COMM_WORLD);
			MPI_Pack(&packageMaster2Slave.colorB, 1, MPI_UNSIGNED_CHAR, master2SlaveSize, sumSize, &position, MPI_COMM_WORLD);
		
			MPI_Send(buffSend, position, MPI_PACKED, i + 1, WORKTAG, MPI_COMM_WORLD);
			--tasks;
		}
		//---------------------
		//----  odbieranie rezultatów kolejnych podzadañ i przydzielanie podzadañ wolnym slave'om
		//---------------------
		while(tasks > 0)
		{
			
				
			
			//recv od slave'ów
			
			MPI_Recv(buffRecv, sumSize, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			position = 0;
			MPI_Get_count(&status, MPI_PACKED, &msgSize);
			
			
			MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.jobID, 1, MPI_INT, MPI_COMM_WORLD);
			MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.ymin, 1, MPI_INT, MPI_COMM_WORLD);
			MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.ymax, 1, MPI_INT, MPI_COMM_WORLD);
			MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.colorR, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
			MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.colorG, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
			MPI_Unpack(buffRecv, slave2MasterSize, &position, &packageSlave2Master.colorB, numOfPixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
			

			//ustalenie nastêpnego taska
			position = 0;
			packageMaster2Slave.jobID = i;
			packageMaster2Slave.ymin = rowNo;
			if (tasks == 1) // w wyliczaniu numberOfRowsPerTask zaokr¹glamy w dól, wiêc bêdziemy przetwarzaæ mniej wierszy ni¿ powinniœmy
							// zatem przy ostatnim tasku niech policzy wszystkie pozosta³e
			{
				packageMaster2Slave.ymax = y0;
				sizeOfLastTask = y0 - rowNo; // bedzie potrzebne by zaalokowac odpowiednio duzo bufora przy MPI_Recv
				slaveWithLastTask = status.MPI_SOURCE;
			}
			else
				packageMaster2Slave.ymax = rowNo + numberOfRowsPerTask;
			rowNo += numberOfRowsPerTask;
			//wys³anie nastêpnego taska
			MPI_Pack(&packageMaster2Slave.jobID, 1, MPI_INT, buff, sumSize, &position, MPI_COMM_WORLD);
			MPI_Pack(&packageMaster2Slave.x, 1, MPI_INT, buff, sumSize, &position, MPI_COMM_WORLD);
			MPI_Pack(&packageMaster2Slave.ymin, 1, MPI_INT, buff, sumSize, &position, MPI_COMM_WORLD);
			MPI_Pack(&packageMaster2Slave.ymax, 1, MPI_INT, buff, sumSize, &position, MPI_COMM_WORLD);
			MPI_Pack(&packageMaster2Slave.depth, 1, MPI_INT, buff, sumSize, &position, MPI_COMM_WORLD);
			MPI_Pack(&packageMaster2Slave.colorR, 1, MPI_UNSIGNED_CHAR, buff, sumSize, &position, MPI_COMM_WORLD);
			MPI_Pack(&packageMaster2Slave.colorG, 1, MPI_UNSIGNED_CHAR, buff, sumSize, &position, MPI_COMM_WORLD);
			MPI_Pack(&packageMaster2Slave.colorB, 1, MPI_UNSIGNED_CHAR, buff, sumSize, &position, MPI_COMM_WORLD);
		
			MPI_Send(buff, position, MPI_PACKED, status.MPI_SOURCE, WORKTAG, MPI_COMM_WORLD);
			
			//
			//	przetwarzanie odebranenych rezultatów, wstawienie do tablicy bitmapy w odpowiednim miejscu
			//
			
			--tasks;
			++i;
		}
		
		
		//-------------
		//----  oczekiwanie na zakoñczenie ostatnich podzadañ, po rozdaniu wszystkich podzadañ z puli, 
		for (int rank = 1; rank <= worldSize; rank++)
		{
			
			if (rank == slaveWithLastTask)
			{
				
			}
			else
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
			}
			//
			//	przetwarzanie odebranenych rezultatów, wstawienie do tablicy bitmapy w odpowiednim miejscu
			//
			
		}
		
		//--------------
		//----  wyslanie ¿adania zakonczenia pracy slave'om
		//--------------
		for (int rank = 1; rank < worldSize; rank++)
		{
			MPI_Send(0, 0, MPI_PACKED, rank, DIETAG, MPI_COMM_WORLD);
		}
	}
	else
	{		
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
		master2SlaveSize += memberSize; // master
		MPI_Pack_size(3*numOfPixels, MPI_CHAR, MPI_COMM_WORLD, &memberSize);
		master2SlaveSize += memberSize;
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
		
		std::cout << packageMaster2Slave.jobID << std::endl;
		std::cout << packageMaster2Slave.x << std::endl;
		std::cout << packageMaster2Slave.ymin << std::endl;
		std::cout << packageMaster2Slave.ymax << std::endl;
		std::cout << packageMaster2Slave.depth << std::endl;
		std::cout << (int)packageMaster2Slave.colorR << std::endl;
		std::cout << (int)packageMaster2Slave.colorG << std::endl;
		std::cout << (int)packageMaster2Slave.colorB << std::endl;
		
		//RenderScene(depth);
	}
	
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

void renderScene(double x, double y, int depth)
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

