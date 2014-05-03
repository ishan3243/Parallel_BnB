#include <cstdio>
#include <mpi.h>
#include "problemDefKnap.hpp"
#include <unistd.h>
#include <vector>
#define MAX 1
#define MIN 0

typedef enum {PBM_TAG=50000, BNB_TAG, END_TAG, SOLVE_TAG, DEFAULT_TAG, DONE_TAG, IDLE_TAG, BCAST_TAG} tag;

class Solver
{
public:
	int probType;
	//int nProcs;

	int argc;
	char** argv;

	void initSetup(int argc, char** argv)
	{
		this->argc = argc;
		this->argv = argv;
	}

	int solve()
	{
		int rank,size;
		Bound bestSol;


		MPI_Init (&argc, &argv);
		MPI_Comm_rank (MPI_COMM_WORLD, &rank);
		MPI_Comm_size (MPI_COMM_WORLD, &size);
		int nProcs = size;
		Problem pbm;
		int dummy;

		MPI_Status status;
		int source;
		tag Tag;
		
		Solution sol;

		int nslaves;
		Bound high,low;
		int total;
		int j;

		Bound tempBestSol;
		//int arrTemp[nItem];
		int sendslaves[nProcs];

		Solution tempsol;

		queue<Subproblem> bqueue;

		if(rank==0)			//master code
		{
			int i;
			pbm.initProblem();
			for(i=1;i<nProcs;i++)
			{
				MPI_Send(&pbm, sizeof(pbm), MPI_BYTE, i, BCAST_TAG, MPI_COMM_WORLD);
			}

			bestSol = this->probType==MAX?-(inf):inf;
			int busy[nProcs];
			busy[0] = 1;
			

			for(i=1;i<nProcs;i++)
			{
				busy[i] = 0;
			}
			
			int idle = nProcs - 1;
			int firstIdle = 1;

			Subproblem auxSp;
			auxSp.initSubProblem(pbm);
			
			MPI_Send(&dummy, 1, MPI_INT, firstIdle, 
				PBM_TAG, MPI_COMM_WORLD);

			MPI_Send(&bestSol, sizeof(bestSol), MPI_BYTE, firstIdle, 
				PBM_TAG, MPI_COMM_WORLD);

			MPI_Send(&auxSp, sizeof(auxSp), MPI_BYTE, firstIdle, PBM_TAG, MPI_COMM_WORLD);

			idle--;

			busy[firstIdle] = 1;


			int donetag = -1;




			while(idle < (nProcs - 1))		//If all are idle, end
			{
				MPI_Recv(&dummy, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
					MPI_COMM_WORLD, &status);

				source = status.MPI_SOURCE;
				Tag = (tag)status.MPI_TAG;

				switch(Tag)
				{
					case SOLVE_TAG:

						MPI_Recv(&tempBestSol, sizeof(tempBestSol), MPI_BYTE, status.MPI_SOURCE, SOLVE_TAG,
							MPI_COMM_WORLD, &status);
						MPI_Recv(&tempsol, sizeof(tempsol), MPI_BYTE, status.MPI_SOURCE, SOLVE_TAG, MPI_COMM_WORLD, &status);

						if((this->probType==MAX && tempBestSol>=bestSol) || (this->probType==MIN && tempBestSol<=bestSol))
						{
							
							bestSol = tempBestSol;

							sol = tempsol;

						}
					break;

					case BNB_TAG:
						MPI_Recv(&nslaves, 1, MPI_INT, status.MPI_SOURCE, BNB_TAG,
							MPI_COMM_WORLD, &status);

						j=0;
						total = (nslaves<=idle)?nslaves:idle;
						for(i=1;i<nProcs && j<nslaves;i++)
						{
							if(busy[i]==0)
							{
								sendslaves[j] = i;
								j++;
								busy[i]=1;
								idle--;
							}
						}

						MPI_Send(&total, 1, MPI_INT, status.MPI_SOURCE, 
							DEFAULT_TAG, MPI_COMM_WORLD);
						MPI_Send(&bestSol, sizeof(bestSol), MPI_BYTE, status.MPI_SOURCE, 
							DEFAULT_TAG, MPI_COMM_WORLD);
						MPI_Send(sendslaves, nProcs, MPI_INT, status.MPI_SOURCE, 
							DEFAULT_TAG, MPI_COMM_WORLD);

					break;

					case IDLE_TAG:

						idle++;
						busy[status.MPI_SOURCE] = 0;
					break;
				} 
				
			}

			for(i=1;i<nProcs;i++)
			{
				MPI_Send(&dummy, 1, MPI_INT, i, 
					END_TAG, MPI_COMM_WORLD);
			}

			printf("master => solution\n");


			sol.printSolution(pbm);

/*
			for(i=0;i<nItem;i++)
			{
				printf("%d ",sol.chosen[i]);
			}
			printf("\n");
*/			//sleep(5);
			
		}

		else
		{
			char c[100];
			gethostname(c,100);
			Subproblem subp;
			Solution auxSol;
			int rslaves;
			int nfslaves;
			printf("rank=> %d  host name=>%s\n ", rank, c);
			MPI_Recv(&pbm, sizeof(pbm), MPI_BYTE, 0, BCAST_TAG,
				MPI_COMM_WORLD, &status);

			
			//printf("\n\n");

			while(true)
			{
				MPI_Recv(&dummy, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
					MPI_COMM_WORLD, &status);

				Tag = (tag)status.MPI_TAG;
				switch(Tag)
				{
					case PBM_TAG:
						MPI_Recv(&bestSol, sizeof(bestSol), MPI_BYTE, status.MPI_SOURCE, PBM_TAG,
							MPI_COMM_WORLD, &status);
						MPI_Recv(&subp, sizeof(subp), MPI_BYTE, status.MPI_SOURCE, PBM_TAG,
							MPI_COMM_WORLD, &status);

						bqueue.push(subp);

						while(!bqueue.empty()) 
						{


							/*if(bqueue.size()<=0)
							{
								printf("GRAVE Error\n");
								break;

							}*/

							subp = bqueue.front();

							bqueue.pop();

							if(subp.feasibility(pbm) == 1)
							{
								MPI_Send(&dummy, 1, MPI_INT, 0, 
									SOLVE_TAG, MPI_COMM_WORLD);
								
								Bound temp = (float)subp.getSolutionMeasure(pbm);
								//printf("solved, %f\n",temp);
								
								MPI_Send(&temp, sizeof(temp), MPI_BYTE, 0, 
									SOLVE_TAG, MPI_COMM_WORLD);
								MPI_Send(&subp.sol, sizeof(subp.sol), MPI_BYTE, 0, 
									SOLVE_TAG, MPI_COMM_WORLD);
							}
							else
							{
								if(this->probType == MAX)
								{
									high = subp.upper_bound(pbm);
									if(high>bestSol)
									{
										rslaves = subp.getNumberOfNodes(pbm);
										MPI_Send(&dummy, 1, MPI_INT, 0, 
											BNB_TAG, MPI_COMM_WORLD);

										MPI_Send(&rslaves, 1, MPI_INT, 0, 
											BNB_TAG, MPI_COMM_WORLD);

										MPI_Recv(&nfslaves, 1, MPI_INT, 0, DEFAULT_TAG,
											MPI_COMM_WORLD, &status);

										MPI_Recv(&bestSol, sizeof(bestSol), MPI_BYTE, 0, DEFAULT_TAG,
											MPI_COMM_WORLD, &status);

										MPI_Recv(sendslaves, nProcs, MPI_INT, 0, DEFAULT_TAG,
											MPI_COMM_WORLD, &status);

										
										subp.branch(pbm, bqueue);

										for(int i=0;i<nfslaves;i++)
										{
											subp = bqueue.front();
											bqueue.pop();
											MPI_Send(&dummy, 1, MPI_INT, sendslaves[i], 
												PBM_TAG, MPI_COMM_WORLD);
											MPI_Send(&bestSol, sizeof(bestSol), MPI_BYTE, sendslaves[i], 
												PBM_TAG, MPI_COMM_WORLD);
											MPI_Send(&subp, sizeof(subp), MPI_BYTE, sendslaves[i], PBM_TAG, MPI_COMM_WORLD);
										
										}
									}								
								}
								else  //its a minimization problem
								{
									low = subp.lower_bound(pbm);
									if(low<bestSol)
									{
										rslaves = subp.getNumberOfNodes(pbm);
										MPI_Send(&dummy, 1, MPI_INT, 0, 
											BNB_TAG, MPI_COMM_WORLD);

										MPI_Send(&rslaves, 1, MPI_INT, 0, 
											BNB_TAG, MPI_COMM_WORLD);

										MPI_Recv(&nfslaves, 1, MPI_INT, 0, DEFAULT_TAG,
											MPI_COMM_WORLD, &status);

										MPI_Recv(&bestSol, sizeof(bestSol), MPI_BYTE, 0, DEFAULT_TAG,
											MPI_COMM_WORLD, &status);

										MPI_Recv(sendslaves, nProcs, MPI_INT, 0, DEFAULT_TAG,
											MPI_COMM_WORLD, &status);

										
										subp.branch(pbm, bqueue);

										for(int i=0;i<nfslaves;i++)
										{
											subp = bqueue.front();
											bqueue.pop();
											MPI_Send(&dummy, 1, MPI_INT, sendslaves[i], 
												PBM_TAG, MPI_COMM_WORLD);
											MPI_Send(&bestSol, sizeof(bestSol), MPI_BYTE, sendslaves[i], 
												PBM_TAG, MPI_COMM_WORLD);
											MPI_Send(&subp, sizeof(subp), MPI_BYTE, sendslaves[i], PBM_TAG, MPI_COMM_WORLD);
										
										}
									}
								}

							}
						}

						MPI_Send(&dummy, 1, MPI_INT, 0, 
							IDLE_TAG, MPI_COMM_WORLD);

					break;

					case END_TAG:
					MPI_Finalize();
					return 0;
					break;
				}
			}
		}
		
		MPI_Finalize();
		return 0;
	}
};
