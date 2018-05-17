#pragma once
#include<random>
using namespace std;

class RandGen{
private:
	typedef std::mt19937 MyRNG;  // the Mersenne Twister with a popular choice of parameters
	//uint32_t seed_val;           // populate somehow

	MyRNG rng;                   // e.g. keep one global instance (per thread)
	random_device rd;
public:
	int RandGen::randInt(int felsokorlat); //0 és felsõkorlát között ad
	RandGen::RandGen();
	
};

