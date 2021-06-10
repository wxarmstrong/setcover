#ifndef OPUS_H
#define OPUS_H

#include "search.h"

//#include "islands.h"
#include <iostream>
#include <iomanip>
#include <stack>
#include <cassert>
#include "DancingLinks.h"

#define ROW 20
#define COL 16

std::vector<std::string> permitted_actions;

// A function to check if a given
// cell (row, col) can be included in DFS
int isSafea(bool M[COL][ROW], int row, int col,
	int visited[COL][ROW])
{
	// row number is in range, column
	// number is in range and value is 1
	// and not yet visited
	return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL) && (!M[row][col] && visited[row][col] == -1);
}

// A utility function to do DFS for a
// 2D boolean matrix. It only considers
// the 4 neighbours as adjacent vertices
void DFS(bool M[COL][ROW], int row, int col,
	int visited[COL][ROW], int isleNum, int& static_count)
{
	
	static_count++;
//	std::cout << "DFS(" << col << "," << row << std::endl;
	// These arrays are used to get
	// row and column numbers of 4
	// neighbours of a given cell
	static int rowNbr[] = { -1,  1,  0, 0 };
	static int colNbr[] = { 0, 0, -1, 1 };

	// Mark this cell as visited
	visited[row][col] = isleNum;

	// Recur for all connected neighbours
	for (int k = 0; k < 4; ++k)
		if (isSafea(M, row + rowNbr[k], col + colNbr[k], visited))
		{
			//            std::cout << "Moving recusively to neighbor " << row + rowNbr[k] << "," << col + colNbr[k] << std::endl;
			DFS(M, row + rowNbr[k], col + colNbr[k], visited, isleNum, static_count);
		}
}

// The main function that returns
// count of islands in a given boolean
// 2D matrix
bool islands(bool M[COL][ROW])
{

/*
	for (int i = 0; i < COL; i++)
	{
		for (int j = 0; j < ROW; j++)
		{
			std::cout << (M[i][j]) << " ";
		}
		std::cout << std::endl;
	}
	system("pause");
*/


	// Make a bool array to mark visited cells.
	// Initially all cells are unvisited
	int visited[COL][ROW];
	memset(visited, -1, sizeof(visited));

	// Initialize count as 0 and
	// travese through the all cells of
	// given matrix
	int static_count;
	int isleNum = 0;
	for (int i = 0; i < COL; ++i)
		for (int j = 0; j < ROW; ++j)

			// If a cell with value 1 is not
		{
			if (!M[i][j] && (visited[i][j] == -1)) {
				//               std::cout << i << "," << j << " not yet visited." << std::endl;
							   // visited yet, then new island found
							   // Visit all cells in this island.
				static_count = 0;
				DFS(M, i, j, visited, isleNum++, static_count);
				//                std::cout << "Size of that isle: " << static_count << std::endl;

				if (static_count % 5 != 0)
				{
//					std::cout << static_count << std::endl;
//					std::cout << "Detected an island of size % 5 != 0. Rejected!" << std::endl;
					return false;
				}
					
			}
		}


	return true;
}

struct BlockData
{
	int size = 0;
	int configs = 0;
	bool*** maps = nullptr;
	std::pair<int, int>* endpos = nullptr;
};

BlockData* dataArr = nullptr;

struct Opus : public SearchSpace {

	struct State : public SearchState
	{
		struct Block
		{
			std::pair<int, int> pos;
			int type;
			int config;
		};

	public:
		State(std::ifstream& in);
		State(State& s);
		~State() {};
		SearchState* clone();
		void print();

		int get_remaining() { return remaining; };
		int get_num_blocks() { return blocks.size(); };
		bool board[16][20];
	private:
		std::pair<int, int> pos = std::pair<int, int>(0,0);
		std::vector<Block> blocks;
//		bool board[16][20];
		int remaining = 0;

		void place_block(int blocknum, int config, std::pair<int, int> new_pos, int totaldistance);

		int distance_to_block(int blocknum);
		int distance_to_place(std::pair<int, int> pos1, std::pair<int, int> pos2);

		int h();
		bool goal();
		std::string hash();
		std::queue<std::pair<std::string, SearchState*>> expand();
	};

public:
	Opus();
	Opus(SearchState* s);
	Opus(std::ifstream& in);
private:
	static void printBlock(int type, int config);
};

Opus::Opus()
{
	std::cout << "Loading blockdata.txt..." << std::endl;
	std::ifstream blockdata("blockdata.txt");
	int num_blocks;
	blockdata >> num_blocks;
	dataArr = new BlockData[num_blocks];

//	BlockData* temp = data;

	for (int i = 0; i < num_blocks; i++)
	{
		BlockData* cur = &dataArr[i];
		blockdata >> dataArr[i].size;
		blockdata >> dataArr[i].configs;
		dataArr[i].maps = new bool**[dataArr[i].configs];
		for (int j = 0; j < dataArr[i].configs; j++)
		{
			dataArr[i].maps[j] = new bool*[dataArr[i].size];
			for (int k = 0; k < dataArr[i].size; k++)
			{
				dataArr[i].maps[j][k] = new bool[dataArr[i].size];
			}
		}
		dataArr[i].endpos = new std::pair<int, int>[dataArr[i].configs];

		for (int j = 0; j < dataArr[i].configs; j++)
		{
			for (int k = 0; k < dataArr[i].size; k++)
			{
				for (int l = 0; l < dataArr[i].size; l++)
				{
					char c;
					blockdata >> c;
					dataArr[i].maps[j][k][l] = (c == '0' ? false : true);
				}
			}
			blockdata >> dataArr[i].endpos[j].first;
			blockdata >> dataArr[i].endpos[j].second;

			assert(dataArr[i].endpos[j].first >= 0);
			assert(dataArr[i].endpos[j].second >= 0);
		}

		
	}

	// Check validity of block data
/*
	std::cout << num_blocks << " blocks loaded:" << std::endl;
	for (int i = 0; i < num_blocks; i++)
	{
		std::cout << "Block type " << i << ":" << std::endl;
		std::cout << " Size: " << data[i].size << std::endl;
		int num_configs = data[i].configs;
		for (int j = 0; j < num_configs; j++)
		{
			std::cout << " Config #" << j << std::endl;
			int size = data[i].size;
			for (int k = 0; k < size; k++)
			{
				std::cout << " ";
				for (int l = 0; l < size; l++)
				{
					std::cout << data[i].maps[j][k][l];
				}
				std::cout << std::endl;
			}
		}
	}
	*/
}


Opus::Opus(std::ifstream& in): Opus()
{
	if (init != nullptr) delete init;
	init = new State(in);

	//
	std::queue<std::pair<std::string, SearchState*>> init_moves = init->expand();
	int R = init_moves.size();
	State* temp = (State*)init;
	int C1 = temp->get_remaining();
	int C2 = temp->get_num_blocks();
	int C = C1 + C2;
	std::cout << R << " moves available at start: " << std::endl;

	char* mat;

	mat = new char[R * C];
	memset(mat, 0, R * C);
	std::cout << "Matrix created of size " << R * C << std::endl;

	int movenum = 0;

	std::vector<std::string> move_names;

	while (!init_moves.empty())
	{
		std::pair<std::string, SearchState*> cur_move = init_moves.front();
		init_moves.pop();
		std::string a = cur_move.first;
		State* s = (State*)cur_move.second;
		
		move_names.push_back(a);
		std::cout << a << std::endl;

		int blocknum = a[0] - '0';
		std::cout << "Placing nonessential block flag in position " << movenum * C + C1 + blocknum << std::endl;
		mat[movenum * C + C1 + blocknum] = 1;

		// Now work out which blocks were filled
		int block_idx = 0;

		for (int i = 0; i < COL; i++)
			for (int j = 0; j < COL; j++)
				if (temp->board[i][j] == false)
				{
					if (s->board[i][j] == true)
					{
						std::cout << "Placing essential target flag in position " << movenum * C + block_idx << std::endl;
						mat[movenum * C + block_idx] = 1;
					}
						
					block_idx++;
				}

		delete s;
		movenum++;
	}

	int* testCount = new int[R];
	memset(testCount, 0, sizeof(int)*R);
	for (int i = 0; i < R; i++)
	{
		for (int j = 0; j < C; j++)
			if (mat[i*C+j]) testCount[j] += 1;
	}

	for (int i = 0; i < C; i++)
	{
		std::cout << "Col " << i << " size: " << testCount[i] << std::endl;
	}

	// Produce a printout of the matrix so that
	//  I can follow along manually to find the bug

	for (int i = 0; i < R; i++)
	{
		std::cout << std::setw(10) << std::left <<  move_names[i] + ";";
		for (int j = 0; j < C; j++)
		{
			std::cout << (mat[i * C + j]==true?'1':'0') << ";";
		}
		std::cout << std::endl;
	}













	//Now solve
	DancingLinks D;
	D.Create(R, C1, C2, mat);
	std::vector<std::vector<int>> all;
	std::vector<int> cur;

	D.solve(all, cur, true);

	//Number of solutions
	std::cout << all.size() << " solutions found" << std::endl;
	

	system("pause");
}

Opus::Opus(SearchState* s): Opus()
{
	State* s2 = (State*)s;
	init = new State(*s2);
}

Opus::State::State(std::ifstream& in) {
	int num_blocks;

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			char g;
			in >> g;
			if (g == '0')
			{
				board[i][j] = false;
				remaining++;
			}
			else
			{
				board[i][j] = true;
			}
		}
	}

	std::cout << "Remaining: " << remaining << std::endl;

	in >> pos.first;
	in >> pos.second;
//	std::cout << pos.first << "," << pos.second << std::endl;
	
	in >> num_blocks;
	for (int i = 0; i < num_blocks; i++)
	{
		Block newblock;
		in >> newblock.type;
		in >> newblock.config;
		in >> newblock.pos.first;
		in >> newblock.pos.second;
		blocks.push_back(newblock);
	}
	
	g = 0;
}

Opus::State::State(State& s) : SearchState(s) {
	pos = s.pos;
	blocks = s.blocks;
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 20; j++)
			board[i][j] = s.board[i][j];
	remaining = s.remaining;
	g = s.g;
}

SearchState* Opus::State::clone() {
	return new State(*this);
}

void Opus::State::print() {
	std::cout << "Current pos: " << pos.first << ", " << pos.second << std::endl;
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			std::cout << board[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << blocks.size() << " blocks:" << std::endl;
	for (int i = 0; i < blocks.size(); i++)
	{
		Block curblock = blocks[i];
		std::cout << " Type " << curblock.type << ", config " << curblock.config << ", pos " << curblock.pos.first << ", " << curblock.pos.second << std::endl;
		printBlock(curblock.type, curblock.config);
//		std::cout << " Distance: " << distance_to_block(i) << std::endl;
	}
}

void Opus::printBlock(int type, int config)
{
	BlockData curblock = dataArr[type];
	int size = curblock.size;
	bool** curmap = curblock.maps[config];
	for (int i = 0; i < size; i++)
	{
		std::cout << " ";
		for (int j = 0; j < size; j++)
		{
			std::cout << curmap[i][j];
		}
		std::cout << std::endl;
	}

}

int Opus::State::distance_to_block(int blocknum)
{
//	std::cout << "distance_to_block(" << blocknum << ")" << std::endl;

	Block block = blocks[blocknum];
	int player_x = pos.first;
	int player_y = pos.second;
	int block_x = block.pos.first;
	int block_y = block.pos.second;
	
//	std::cout << "player_x: " << player_x << std::endl;
//	std::cout << "player_y: " << player_y << std::endl;
//	std::cout << "block_x: " << block_x << std::endl;
//	std::cout << "block_y: " << block_y << std::endl;

	int min_dist = INT_MAX;

	int type = block.type;
	int config = block.config;

	BlockData curdata = dataArr[type];
	int size = curdata.size;
	int configs = curdata.configs;
	bool*** curmaps = curdata.maps;
	bool** config_map = curmaps[config];
	for (int cur_y = 0; cur_y < size; cur_y++)
		for (int cur_x = 0; cur_x < size; cur_x++)
		{
			if (config_map[cur_y][cur_x]==true)
			{
//				std::cout << "Grabbable section found at:" << block_x + cur_x << ", " << block_y + cur_y << std::endl;
				int dist_x = abs((block_x + cur_x) - player_x);
				int dist_y = abs((block_y + cur_y) - player_y);
				int dist = std::max(dist_x, dist_y);
				dist = 1 + 2 * (dist - 1);
				assert(dist > 0);
//				std::cout << "dist: " << dist << std::endl;
				if (dist < min_dist)
				{
//					std::cout << "New closest segment found" << std::endl;
					min_dist = dist;
				}
					
			}
		}

	assert(min_dist > 0);
	return min_dist;
}

int Opus::State::distance_to_place(std::pair<int, int> pos1, std::pair<int, int> pos2)
{
	int pos1_x = pos1.first;
	int pos1_y = pos1.second;

	int pos2_x = pos2.first;
	int pos2_y = pos2.second;

	int dist_x = abs(pos1_x - pos2_x);
	int dist_y = abs(pos1_y - pos2_y);
	int dist = std::max(dist_x, dist_y);
	int final_dist = 1 + 2 * (dist - 1);
	assert(final_dist > 0);
	return final_dist;
}

int Opus::State::h()
{
	//Very naive: just count # of unplaced blocks
	//return blocks.size();

	int total = 0;
	for (int i = 0; i < blocks.size(); i++)
		total += distance_to_block(i);

	return total;

	/*
	if (blocks.empty()) return 0;

	int smallest = INT_MAX;
	for (int i = 0; i < blocks.size(); i++)
	{
		int cur_dist = distance_to_block(i);
		if (cur_dist < smallest)
			smallest = cur_dist;
	}

	return smallest + (blocks.size() - 1);
	*/
}

bool Opus::State::goal()
{
	return remaining == 0;
}

std::string Opus::State::hash()
{
	std::string s = "";

	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 20; j++)
			s += std::to_string(board[i][j]);
	s += ".";
	s += pos.first;
	s += ",";
	s += pos.second;
	for (int i = 0; i < blocks.size(); i++)
	{
		Block curblock = blocks[i];
		s += ",";
		s += curblock.type;
		s += ",";
		s += curblock.config;
		s += ",";
		s += curblock.pos.first;
		s += ",";
		s += curblock.pos.second;
	}

	return s;
}

void Opus::State::place_block(int blocknum, int config, std::pair<int, int> new_pos, int totaldistance)
{
	Block curblock = blocks[blocknum];
	int type = curblock.type;
	BlockData curdata = dataArr[type];
	int size = curdata.size;
	bool** curmap = curdata.maps[config];

	int new_x = new_pos.first;
	int new_y = new_pos.second;

	for (int cur_y = 0; cur_y < size; cur_y++)
		for (int cur_x = 0; cur_x < size; cur_x++)
			if (curmap[cur_y][cur_x] == true)
			{
				board[new_y + cur_y][new_x + cur_x] = true;
				remaining--;
			}
				

	// Set new pos
	std::pair<int, int> offset = curdata.endpos[config];
	int x_offset = offset.first;
	int y_offset = offset.second;
	pos = std::pair<int, int>(new_x + x_offset,new_y + y_offset);
	assert(pos.first > 0);
	assert(pos.second > 0);

	// Remove block from blocks vector
	blocks.erase(blocks.begin() + blocknum);

	g += totaldistance;
	assert(g > 0);
}

std::queue<std::pair<std::string, SearchState*>> Opus::State::expand()
{
//	std::cout << "expand()" << std::endl;
	std::queue<std::pair<std::string, SearchState*>> q;

	for (int blocknum = 0; blocknum < blocks.size(); blocknum++)
	{
		Block block = blocks[blocknum];
		int grab_dist = distance_to_block(blocknum);
		// Check all possible configurations
		int type = block.type;
		BlockData cur_data = dataArr[type];
		int size = cur_data.size;
		int num_configs = cur_data.configs;

		for (int config_num = 0; config_num < num_configs; config_num++)
		{
			bool** cur_map = cur_data.maps[config_num];
			for (int cur_y = 0; cur_y < 16 - size + 1; cur_y++)
				for (int cur_x = 0; cur_x < 20 - size + 1; cur_x++)
				{
					// Iterate over all sections of block map
					bool fits = true;
					for (int block_y = 0; block_y < size; block_y++)
					{
						for (int block_x = 0; block_x < size; block_x++)
						{
							// Confirm that block fits there
//							std::cout << "Checking that block fits there" << std::endl;
							if (cur_map[block_y][block_x] == true && board[cur_y + block_y][cur_x + block_x] == true)
								fits = false;
							if (!fits) break;
						}
						if (!fits) break;
					}
					if (fits)
					{
						// Create new action consisting of placing block at that spot
						std::pair<int, int> new_pos = std::pair<int, int>(cur_x, cur_y);
						int total_distance = grab_dist + distance_to_place(block.pos, new_pos);

						assert(total_distance > 0);

						//total_distance = 0;

						State* newState = new State(*this);
						newState->place_block(blocknum, config_num, new_pos, total_distance);

						/*
						newState->print();
						system("pause");
						*/

						if (islands(newState->board))
						{
							std::string action_name = std::to_string(type) + "/" + std::to_string(config_num) + "->" + std::to_string(new_pos.first) + "," + std::to_string(new_pos.second);
							//						std::cout << "Action found: " << action_name << std::endl;
							q.push(std::pair<std::string, SearchState*>(action_name, newState));
						}
						else
						{
							delete newState;
						}
							

					}

				}
		}

	}

	return q;
}

#endif OPUS_H