#include <cstdio>
#include <omp.h>
#include "problemDefKnap.hpp"
#include <unistd.h>
#include <vector>
#define MAX 1
#define MIN 0

const int maxthread = 5;

class SolverMP{
public:
	int probType;


	int solve()
	{
		queue<Subproblem> bqueue;
		Bound bstemp = probType==MAX?(-inf):inf;
		Solution bestsol;
		Subproblem data[maxthread];

		Bound high, low;

		Subproblem auxsp;

		Problem pbm;
		pbm.initProblem();

		auxsp.initSubProblem(pbm);

		bqueue.push(auxsp);

		int nn,nt;

		while(!bqueue.empty())
		{
			nn = bqueue.size();
			nt = nn>maxthread ? maxthread : nn;

			for(int i=0;i<nt;i++)
			{
				data[i] = bqueue.front();
				bqueue.pop();
			}

			#pragma omp parallel for num_threads(nt) private(high, low)

			for(int i=0;i<nt;i++)
			{
				//printf("thread => %d\n",omp_get_thread_num());
				if(probType == MAX)
				{

					if(data[i].feasibility(pbm)==1)
					{
						#pragma omp critical
						{
							if(bstemp < data[i].getSolutionMeasure(pbm))
							{
								bstemp = data[i].getSolutionMeasure(pbm);
								bestsol = data[i].sol;
							}
						}
					}
					else
					{
						high = data[i].upper_bound(pbm);
						if(high > bstemp)
						{
							#pragma omp critical
							{
								data[i].branch(pbm, bqueue);
							}
							
						}
					}
				}
				else
				{
					if(data[i].feasibility(pbm)==1)
					{
						#pragma omp critical
						{
							if(bstemp > data[i].getSolutionMeasure(pbm))
							{
								bstemp = data[i].getSolutionMeasure(pbm);
								bestsol = data[i].sol;
							}
						}
					}
					else
					{
						low = data[i].lower_bound(pbm);
						if(low < bstemp)
						{
							#pragma omp critical
							{
								data[i].branch(pbm, bqueue);
							}
							
						}
					}
				}
			}
		}

		bestsol.printSolution(pbm);

	}
};