#include "stdafx.h"
#include "RandGen.h"


//használat: RandGen rg; egyszer, majd: rg.randInt(n);

// [0,felsokorlat[
int RandGen::randInt(int felsokorlat) {
	if (felsokorlat > 0)
		felsokorlat--;
	uniform_int_distribution<int> intUb(0, felsokorlat);
	return intUb(rng);
}
RandGen::RandGen() {	
	rng.seed(rd());
}