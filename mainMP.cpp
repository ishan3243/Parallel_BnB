#include <cstdio>
#include <unistd.h>
#include "solverMP.hpp"
#include <vector>

using namespace std;



int main (int argc, char** argv)
{
	SolverMP solver;
	solver.probType = MAX;
	//solver.initSetup(argc,argv);

	solver.solve();
	
}
