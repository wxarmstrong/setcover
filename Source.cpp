#include "astar.h"
#include "opus.h"

int main()
{
	std::ifstream in("opus1.txt");
	Opus s(in);
	AStar solver;
	solver.solve(&s);
	system("pause");
}