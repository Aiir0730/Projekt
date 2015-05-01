#pragma once

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

