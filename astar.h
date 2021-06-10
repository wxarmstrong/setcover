#ifndef ASTAR_H
#define ASTAR_H

#include "search.h"
#include <map>
#include <iostream>
#include <cassert>


struct AStar : public SearchAlg {

	// Nodes for the priority queue
	struct Node {
	public:
		Node();
		Node(const Node& n);
		Node(SearchState* _s, int _f);
		~Node();
		Node& operator=(Node other) {
			release();
			this->state = other.state->clone();
			this->f = other.f;
			return *this;
		}
		SearchState* state = nullptr;
		int f = 0;
	private:
		void release();
	};

	// Comparator for the priority queue
	struct Compare {
		bool operator()(Node lhs, Node rhs) const {
			if (lhs.f > rhs.f) return true;
			if (lhs.f == rhs.f) return (lhs.state->g < rhs.state->g);
			return false;
		}
	};

	// Tuple for State Table data 
	struct StateData {
		StateData() {};
		StateData(int _g, std::string _p, std::string _a);
		int g = 0;
		std::string parent = "";
		std::string action = "";
	};

	// Some variants need to access the underlying container of the PQ,
	//  so we make this wrapper class to grant access to it.
	struct PQ : public std::priority_queue<Node, std::deque<Node>, Compare>
	{
	public:
		std::deque<Node> get_c() {
			return c;
		}
	};

public:
	void solve(SearchSpace* dom);
	int cost = INT_MAX;
};

void AStar::Node::release()
{
	if (state != nullptr) delete state;
}

AStar::Node::Node()
{
	state = nullptr;
	f = 0;
}

AStar::Node::Node(const Node& n)
{
	release();
	state = n.state->clone();
	f = n.f;
}

AStar::Node::Node(SearchState* _s, int _f)
{
	state = _s;
	f = _f;
}

AStar::Node::~Node()
{
	release();
}

AStar::StateData::StateData(int _g, std::string _p, std::string _a)
{
	g = _g;
	parent = _p;
	action = _a;
}

void AStar::solve(SearchSpace* dom)
{
	std::cout << "AStar::solve" << std::endl;
	PQ fringe;
	std::map<std::string, StateData> StateTable;

	Node begin(dom->init->clone(), dom->init->h());
	begin.state->print();

	std::string initHash = (begin.state)->hash();
	StateData initData(begin.state->g, "", "");
	StateTable[initHash] = initData;
	fringe.push(begin);

	while (!fringe.empty())
	{
 		Node n = fringe.top();
		fringe.pop();
		SearchState* s = n.state;
		std::string hash = s->hash();

		if (StateTable.count(hash) == 1 && s->g > StateTable[hash].g)
			continue;

		if (s->goal())
		{
			std::cout << "Found a solution! (Cost: " << s->g << ")" << std::endl;
			std::string path = "";
			StateData data = StateTable[hash];
			std::string act = data.action;

			while (act != "")
			{
				path = act + " " + path;
				//std::cout << "Current path: " << path << std::endl;
				hash = data.parent;
				data = StateTable[hash];
				act = data.action;
			}
			std::cout << path << std::endl;
			cost = s->g;
			return;
		}

		std::queue<std::pair<std::string, SearchState*>> kids = s->expand();
		while (!kids.empty())
		{

			std::pair<std::string, SearchState*> p = kids.front();
			kids.pop();
			std::string a = p.first;
			SearchState* k = p.second;

			std::string kHash = k->hash();
			bool found = (StateTable.count(kHash) == 1);

			if (!found || StateTable[kHash].g > k->g)
			{
				StateData newData(k->g, hash, a);
				StateTable[kHash] = newData;
				Node newNode(k->clone(), k->g + k->h());
				fringe.push(newNode);
			}
			delete k;
		}

	}

	std::cout << "No solution found!" << std::endl;

}

#endif ASTAR_H