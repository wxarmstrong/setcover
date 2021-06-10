#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include <queue>
#include <fstream>

struct SearchState {
public:
	SearchState() {};
	SearchState(SearchState& s) { g = s.g; };
	virtual ~SearchState() {};
	virtual SearchState* clone() = 0;
	virtual void print() = 0;

	virtual int h() = 0;
	virtual bool goal() = 0;
	virtual std::string hash() = 0;
	virtual std::queue<std::pair<std::string, SearchState*>> expand() = 0;

	int g = 0;
};

struct SearchSpace {
public:
	SearchSpace() {};
	SearchSpace(std::ifstream in) {};
	~SearchSpace();
	SearchState* init = nullptr;
};

SearchSpace::~SearchSpace()
{
	if (init != nullptr) delete init;
}

struct SearchAlg {
	virtual void solve(SearchSpace* dom) = 0;
};

#endif SEARCH_H