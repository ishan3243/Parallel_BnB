#include <cstdio>
#include <mpi.h>
//#include "problemDef.hpp"
#include <unistd.h>
#include "solverHybrid.hpp"
#include <vector>

using namespace std;



int main (int argc, char** argv)
{
	SolverHybrid solver;
	solver.probType = MAX;
	solver.initSetup(argc,argv);

	solver.solve();
	
}
