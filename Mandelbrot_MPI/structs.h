#ifndef __STRUCTS
#define __STRUCTS

struct master2Slave
{
	int jobID;
	int x;
	int ymin;
	int ymax;
	int depth;
	unsigned char colorR;
	unsigned char colorG;
	unsigned char colorB;
};

struct slave2Master
{
	int jobID;
	int ymin;
	int ymax;
	unsigned char* colorR;
	unsigned char* colorG;
	unsigned char* colorB;
};


extern struct master2Slave packageMaster2Slave;
extern struct slave2Master packageSlave2Master;

extern int DIETAG;
extern int WORKTAG;

extern int worldSize;
extern int worldRank;


#endif
