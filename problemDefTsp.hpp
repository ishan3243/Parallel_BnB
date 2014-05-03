/**
problem structure - problem
max/min = enum
*/
#include <queue>
#include <cstdio>
#include <vector>
#include <algorithm>

using namespace std;


const int NOEDGE = 10000000;
const int inf = 1000000000;
const int nNode =5;

typedef float Bound;


class AdjList
{
public:
	int node;
	int weight;

	bool operator < (const AdjList& other) const
	{
		return this->weight < other.weight;
	}
};

class Problem
{
public:
	int adjMat[nNode][nNode];

	int min[nNode][2];

	void initProblem()
	{
		FILE* fp = fopen("inputTsp.txt","r");
		int x;
		int y;
		for(x=0;x<nNode;x++)
		{
			for(y=0;y<nNode;y++)
			{
				adjMat[x][y] =NOEDGE;
			}
		}

		int node;
		int adj,w;
		int nedge;
		for(x=0;x<nNode;x++)
		{
			fscanf(fp, "%d",&node);
			fscanf(fp, "%d",&nedge);
			vector<int> temp;
			while(nedge--)
			{
				fscanf(fp, "%d %d", &adj, &w);
				adjMat[node-1][adj-1] = w;
				temp.push_back(w);
			}
			sort(temp.begin(), temp.end());
			min[node-1][0] = temp[0];
			min[node-1][1] = temp[1];
		}
/*
		for(x=0;x<nNode;x++)
		{
			for(y=0;y<nNode;y++)
			{
				printf("%d ",adjMat[x][y]);
			}
			printf("\n");
		}*/

	}
	//~Problem();
};

class Solution
{
public:
	int chosen[nNode];

	void initSolution()
	{
		int x;
		for(x=0;x<nNode;x++)
		{
			chosen[x] = -1;
		}
	}

	void printSolution(Problem pbm)
	{
		//printf("cost %d\n",curCost);
		int x;
		printf("Solution : \n");
		for(x=0;x<nNode;x++)
		{
			printf("%d ",this->chosen[x]);
		}
		printf("\n");

		int cost = 0;
		for(x=0;x<nNode-1;x++)
		{
			cost = cost+ pbm.adjMat[chosen[x]][chosen[x+1]];
		}
		cost += pbm.adjMat[chosen[0]][chosen[nNode-1]];

		printf("cost %d\n",cost);

	}
	//Solution(void);
	//~Solution();
};

class Subproblem
{
public:
	int nVertices;
	int consideredVertices[nNode];
	Solution sol;
	int curCost;

	void initSubProblem(int nVertices, int consideredVertices[nNode], int cost)
	{
		this->nVertices = nVertices;
		int x;
		for(x=0;x<nNode;x++)
		{
			this->consideredVertices[x] = consideredVertices[x];
		}
		this->curCost = cost;
	}
	
	//~Subproblem();

	Bound getSolutionMeasure(Problem pbm)
	{
		return curCost + pbm.adjMat[this->sol.chosen[nNode-1]][this->sol.chosen[0]];
	}



	void initSubProblem(const Problem& pbm);
	Bound upper_bound(const Problem& pbm);
	Bound lower_bound(const Problem& pbm);
	void branch(const Problem& pbm, queue<Subproblem> & subqueue);
	int feasibility(Problem pbm);
	int getNumberOfNodes(Problem pbm);
};

int Subproblem::getNumberOfNodes(Problem pbm)
{
	int curnode = this->sol.chosen[nVertices-1];
	int x;
	int count = 0;
	for(x=0;x<nNode;x++)
	{
		if(pbm.adjMat[curnode][x] != NOEDGE && this->consideredVertices[x]==0)
		{
			count++;
		}
	}
	return count;
}

int Subproblem::feasibility(Problem pbm)
{
	if(nVertices==nNode)
	{
		if(pbm.adjMat[this->sol.chosen[nNode-1]][this->sol.chosen[0]] != NOEDGE)
		{
			return 1;
		}
	}
	return 0;
}

void Subproblem::initSubProblem(const Problem& pbm)
{
	this->nVertices = 0;
	int x;
	for(x=0;x<nNode;x++)
	{
		this->consideredVertices[x] = 0;
	}

	this->sol.initSolution();
	this->curCost = 0;
}

Bound Subproblem::upper_bound(const Problem& pbm)
{
	return inf;
}

Bound Subproblem::lower_bound(const Problem& pbm)
{
	int x;
	int tempw;
	int bound=this->curCost;
	for(x=0;x<nNode;x++)
	{
		if(this->consideredVertices[x]==0)
		{
			bound+= ((float)((pbm.min[x][0] + pbm.min[x][1])))/2;
		}
		else if(this->consideredVertices[x] == 1)
		{
			if(this->sol.chosen[0]==x)
			{
				tempw = pbm.adjMat[x][this->sol.chosen[1]];
				
				if(tempw == pbm.min[x][0]) //|| tempw == pbm.min[x][1])
				{
					bound+= ((float) (pbm.min[x][1]))/2;
				}
				else
				{
					bound+= ((float) (pbm.min[x][0]))/2;
				}
				
			}
			else if(this->sol.chosen[nVertices-1] == x)
			{
				tempw = pbm.adjMat[x][this->sol.chosen[nVertices-2]];
				
				if(tempw == pbm.min[x][0]) //|| tempw == pbm.min[x][1])
				{
					bound+= ((float) (pbm.min[x][1]))/2;
				}
				else
				{
					bound+= ((float) (pbm.min[x][0]))/2;
				}
			}
		}
	}
	return bound;
}

void Subproblem::branch(const Problem& pbm, queue<Subproblem> & subqueue)
{
	int x,i;
	int curnode;
	


	if(nVertices == 0)
	{
		//first case
		for(x=0;x<nNode;x++)
		{
			if(pbm.adjMat[0][x] != NOEDGE)
			{
				Subproblem sp;
				sp.initSubProblem(pbm);
				sp.nVertices = 2;
				sp.consideredVertices[0] = 1;
				sp.consideredVertices[x] = 1;
				sp.curCost = pbm.adjMat[0][x];
				sp.sol.chosen[0] = 0;
				sp.sol.chosen[1] = x;

				subqueue.push(sp);
			}
		}
	}
	else
	{
		
		curnode = this->sol.chosen[this->nVertices-1];
		for(x=0;x<nNode;x++)
		{
			//printf("========> x  %d curnode %d nVertices %d\n",x,curnode,nVertices);
			if(pbm.adjMat[curnode][x] != NOEDGE && consideredVertices[x]==0)
			{
				Subproblem sp;
				sp.initSubProblem(this->nVertices, this->consideredVertices, this->curCost);
				//sp.initSubProblem(pbm);
				sp.nVertices +=1;
				sp.consideredVertices[x] = 1;
				sp.consideredVertices[this->sol.chosen[this->nVertices-1]] = 2;
				sp.curCost += pbm.adjMat[curnode][x];

				for(i=0;i<nNode;i++)
				{
					sp.sol.chosen[i] = this->sol.chosen[i];
				}

				sp.sol.chosen[this->nVertices] = x;

				subqueue.push(sp);
			}
		}
		
	}
	for(x=0;x<nNode;x++)
	{
		//printf("%d ",this->consideredVertices[x]);
	}

	//printf("%f %d\n",this->lower_bound(pbm),this->curCost);
	
}
