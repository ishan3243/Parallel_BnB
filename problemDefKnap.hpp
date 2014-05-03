/**
problem structure - problem
max/min = enum
*/
#include <queue>
#include <cstdio>
#include <vector>
#include <algorithm>
using namespace std;



const int inf = 1000000000;
const int nItem =15;

typedef float Bound;


class Data
{
public:
	int price;
	int weight;

	bool operator < (const Data& other) const
	{
		float f1,f2;
		f1 = ((float)this->price)/this->weight;
		f2 = ((float)other.price)/other.weight;
		return f1>f2;
	}
};


class Problem
{
public:
	int capacity;
	int nItems;
	int price[nItem];
	int weight[nItem];

	
	void initProblem()
	{
		FILE* fp = fopen("inputKnap.txt","r");
		printf("Enter the details of the knapsack\n");
		fscanf(fp,"%d",&(this->capacity));
		this->nItems = nItem;
		int x;

		for(x=0;x<nItem;x++)
		{
			fscanf(fp, "%d",&(this->price[x]));
		}
		for(x=0;x<nItem;x++)
		{
			fscanf(fp, "%d",&(this->weight[x]));
		}

		vector<Data> temp;
		Data t2;
		for(x=0;x<nItem;x++)
		{
			t2.price = this->price[x];
			t2.weight = this->weight[x];
			temp.push_back(t2);
		}
		sort(temp.begin(), temp.end());

		for(x=0;x<nItem;x++)
		{
			this->price[x] = temp[x].price;
			this->weight[x] = temp[x].weight;
		}

		for(x=0;x<nItem;x++)
		{
			printf("%d %d\n",this->price[x],this->weight[x]);
		}

	}

	//~Problem();
};

class Solution
{
public:
	int chosen[nItem];

	void initialize()
	{
		int x;
		for(x=0;x<nItem;x++)
		{
			chosen[x] = 0;
		}
	}

	void printSolution(Problem pbm)
	{
		printf("The items selected are\n");
		int x;
		int profit = 0;
		for(x=0;x<nItem;x++)
		{
			if(chosen[x]==1)
			{
				printf("Price %d Weight %d\n",pbm.price[x], pbm.weight[x]);
				profit+= pbm.price[x];
			}
		}
		printf("%d\n",profit);
	}

	//Solution(void);
	//~Solution();
};

class Subproblem
{
public:
	int curCapacity;
	int nextObj;
	int curProfit;
	Solution sol;

	void initSubProblem(int curCapacity, int nextObj, int curProfit)
	{
		this->curCapacity = curCapacity;
		this->nextObj = nextObj;
		this->curProfit = curProfit;
	}
	
	//~Subproblem();

	Bound getSolutionMeasure(Problem pbm)
	{
		return curProfit;
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
	int remCap = this->curCapacity - pbm.weight[this->nextObj];
	if(remCap>=0)
	{
		return 2;
	}
	return 1;
	//return 2;
	//return 1;
}

int Subproblem::feasibility(Problem pbm)
{
	
	if(this->nextObj==nItem)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	//return 1;
}

void Subproblem::initSubProblem(const Problem& pbm)
{
	this->curCapacity = pbm.capacity;
	this->nextObj = 0;
	this->curProfit = 0;
	this->sol.initialize();
}

Bound Subproblem::upper_bound(const Problem& pbm)
{
	int x;
	int weight;
	float price =0 ;
	weight = pbm.capacity - this->curCapacity;
	for(x=this->nextObj;x<nItem;x++)
	{
		if(weight+pbm.weight[x] <= pbm.capacity)
		{
			price += pbm.price[x];
			weight = weight + pbm.weight[x];
		}
		else
		{
			price = price + ((float)(pbm.capacity-weight)/pbm.weight[x])*pbm.price[x];
			weight = pbm.capacity;
		}
	}
	return (price+(this->curProfit));
}

Bound Subproblem::lower_bound(const Problem& pbm)
{
	return (Bound)inf;
}

void Subproblem::branch(const Problem& pbm, queue<Subproblem> & subqueue)
{
	Subproblem spnew;
	spnew.initSubProblem(this->curCapacity, this->nextObj+1, this->curProfit);

	int x;

	for(x=0;x<nItem;x++)
	{
		spnew.sol.chosen[x] = this->sol.chosen[x];
	}

	subqueue.push(spnew);
	
	int remCap = this->curCapacity - pbm.weight[this->nextObj];
	if(remCap>=0)
	{
		Subproblem spnew2;
		spnew2.initSubProblem(remCap, this->nextObj+1, this->curProfit + pbm.price[this->nextObj]);
		for(x=0;x<nItem;x++)
		{
			spnew2.sol.chosen[x] = this->sol.chosen[x];
		}
		spnew2.sol.chosen[this->nextObj] = 1;

		subqueue.push(spnew2);
	}
}
