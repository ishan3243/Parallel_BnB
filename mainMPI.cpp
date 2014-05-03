#include <cstdio>
#include <mpi.h>
//#include "problemDef.hpp"
#include <unistd.h>
#include "solverMPI.hpp"
#include <vector>

using namespace std;



int main (int argc, char** argv)
{
	Solver solver;
	solver.probType = MAX;
	solver.initSetup(argc,argv);

	solver.solve();
	
}
