#ifndef DANCINGLINKS_H
#define DANCINGLINKS_H

#include "DancingLinksColumn.h"
#include <vector>

 // Forward declaration for all the classes
class DancingLinksCell;
class DancingLinksColumn;

// Head cell for the dancing links implementation
class DancingLinks {
private:
    // Dummy column to ease horizontal movement
    DancingLinksColumn* dummycolumn_;

    // Pointers to store column and cell arrays
    DancingLinksColumn* columns_;
    DancingLinksCell* cells_;

public:
    // Constructor
    DancingLinks();

    // Destructor
    ~DancingLinks();

    // Create link structure from adjacency matrix
    void Create(int R, int C1, int C2, char* mat);

    // Clear the link structure
    void Destroy();

    // Solve the problem
    bool solve(std::vector<std::vector<int> >& allsolutions,
        std::vector<int>& cursolution, bool allneeded = false);
};

#endif DANCINGLINKS_H