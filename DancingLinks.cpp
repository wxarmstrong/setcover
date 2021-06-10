#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <set>
#include <iostream>

#include "DancingLinks.h"
#include "DancingLinksCell.h"
#include "DancingLinksColumn.h"

DancingLinks::DancingLinks() {
    dummycolumn_ = new DancingLinksColumn;
    dummycolumn_->set_essential(false);
    columns_ = NULL;
    cells_ = NULL;
}

DancingLinks::~DancingLinks() {
    Destroy();
    delete dummycolumn_;
}

void DancingLinks::Create(int R, int C1, int C2, char* mat) {
    // Create linked structure
    assert((R >= 0) && (C1 >= 0) && (C2 >= 0));
    int C = C1 + C2;

    // Start by destroying existing structure, if any
    Destroy();

    // Allocate memory for the full structure
    // Being lavish here, essentially treating it like an adjacency matrix
    //  C DancingLinksColumns for columns and R*C DancingLinksCells for cells_
    //  TODO: Convert to an adjacency list maybe

    // First, the C columns
    DancingLinksColumn* columns_ = new DancingLinksColumn[C];

    // Set left and right links in both directions for the head
    dummycolumn_->set_right_ptr(&columns_[0]);
    dummycolumn_->set_left_ptr(&columns_[C - 1]);
    columns_[0].set_left_ptr(dummycolumn_);
    columns_[C - 1].set_right_ptr(dummycolumn_);

    // Set left and right pointers for remaining column elements
    for (int i = 0; i < C - 1; i++) {
        columns_[i].set_right_ptr(&columns_[i + 1]);
        columns_[i + 1].set_left_ptr(&columns_[i]);
    }

    // Set size and essentiality values for each column
    for (int i = 0; i < C; i++) {
        columns_[i].set_size(R);
        assert(R >= 0);
        columns_[i].set_essential(i < C1);
    }

    // Now the cells
    DancingLinksCell* cells_ = new DancingLinksCell[R * C];

    // Assign row and column IDs and neighbours for all cells
    for (int i = 0, k = 0; i < R; i++) {
        for (int j = 0; j < C; j++, k++) {
            cells_[k].set_row_id(i);
            cells_[k].set_column_ptr(&columns_[j]);

            if (k % C == 0)
                cells_[k].set_left_ptr(&cells_[k + C - 1]);
            else
                cells_[k].set_left_ptr(&cells_[k - 1]);

            if (k % C == C - 1)
                cells_[k].set_right_ptr(&cells_[k - C + 1]);
            else
                cells_[k].set_right_ptr(&cells_[k + 1]);

            if (k < C) {
                cells_[k].set_up_ptr(columns_[j].dummycell());
                columns_[j].dummycell()->set_down_ptr(&cells_[k]);
            }
            else
                cells_[k].set_up_ptr(&cells_[k - C]);

            if (k >= (R - 1) * C) {
                cells_[k].set_down_ptr(columns_[j].dummycell());
                columns_[j].dummycell()->set_up_ptr(&cells_[k]);
            }
            else
                cells_[k].set_down_ptr(&cells_[k + C]);
        }
    }

    // If a matrix element is 0, delete it by changing neighbours' pointers
    for (int i = 0, k = 0; i < R; i++) {
        for (int j = 0; j < C; j++, k++) {
            if (mat[k] == 0) cells_[k].Erase();
        }
    }
}

void DancingLinks::Destroy() {
    // Delete memory allocated to the children at the header
    // Of course, use this only at the header and only after allocation has
    //  been done. Don't blame me if you screw up
    if (columns_ != NULL) {
        delete[] columns_;
        columns_ = NULL;
    }
    if (cells_ != NULL) {
        delete[] cells_;
        cells_ = NULL;
    }
    dummycolumn_->set_left_ptr(NULL);
    dummycolumn_->set_right_ptr(NULL);
}

bool DancingLinks::solve(std::vector<std::vector<int> >& allsolutions,
    std::vector<int>& cursolution, bool allneeded) {
    std::cout << "solve()" << std::endl;

    std::cout << "Column sizes before column selection: ";
    for (DancingLinksColumn* tempCol = dummycolumn_->right_ptr(); tempCol != dummycolumn_; tempCol = tempCol->right_ptr())
    {
        std::cout << tempCol->size() << " ";
    }
    std::cout << std::endl;

    if (dummycolumn_->right_ptr()->essential() == false) {
        // No more constraints left to be satisfied. Success
        std::cout << "Solution found" << std::endl;
        allsolutions.push_back(cursolution);
        return true;
    }

    // Find the essential column with the lowest degree
    DancingLinksColumn* chosencolumn = dummycolumn_->right_ptr();
    int mincnt = chosencolumn->size();
    assert(mincnt >= 0);

    for (DancingLinksColumn* ptr = chosencolumn->right_ptr(); ptr->essential();
        ptr = ptr->right_ptr()) {
        if (ptr->size() < mincnt) {
            chosencolumn = ptr;
            mincnt = ptr->size();
            assert(mincnt >= 0);
        }
    }

    // No way to satisfy some constraint. Failure
    if (mincnt == 0) return false;
    bool flag = false;

    std::cout << "Selected an essential column of minimal size " << mincnt << std::endl;

    // Remove the chosen column
    int originalsize = chosencolumn->size();
    chosencolumn->RemoveColumn();

    std::cout << "Now looping row selection: " << std::endl;
    for (DancingLinksCell* ptr = chosencolumn->dummycell()->down_ptr();
        ptr != chosencolumn->dummycell(); ptr = ptr->down_ptr()) {
        // Pick this row in candidate solution

        std::cout << "Column sizes before row selection: ";
        for (DancingLinksColumn* tempCol = dummycolumn_->right_ptr(); tempCol != dummycolumn_; tempCol = tempCol->right_ptr())
        {
            std::cout << tempCol->size() << " ";
        }
        std::cout << std::endl;

        int nextsize = chosencolumn->right_ptr()->size();

        // Remove columns for all other cells in this row
        cursolution.push_back(ptr->row_id());
        for (DancingLinksCell* ptr2 = ptr->right_ptr(); ptr2 != ptr;
            ptr2 = ptr2->right_ptr()) {
            ptr2->column_ptr()->RemoveColumn();
        }

        // Recurse to solve the modified board
        std::cout << "Selected row #" << ptr->row_id() << std::endl;
        if (solve(allsolutions, cursolution, allneeded)) flag = true;

        // Unremove all those columns
        for (DancingLinksCell* ptr2 = ptr->right_ptr(); ptr2 != ptr;
            ptr2 = ptr2->right_ptr()) {
            ptr2->column_ptr()->UnremoveColumn();
        }
        cursolution.pop_back();
        std::cout << "Backtracked..." << std::endl;

        //Check sizes of all columns after restoration
        std::cout << "Column sizes after recursive call: ";
        for (DancingLinksColumn* tempCol = dummycolumn_->right_ptr(); tempCol != dummycolumn_; tempCol = tempCol->right_ptr())
        {
            std::cout << tempCol->size() << " ";
        }
        std::cout << std::endl;

        // If only one solution needs to be found and we have found one, don't
        // bother
        // checking for the rest
        if ((flag == true) && (allneeded == false)) {
            chosencolumn->UnremoveColumn();
            return flag;
        }

        assert(chosencolumn->right_ptr()->size() == nextsize);

    }
    // Unremove the chosen column
    std::cout << "Unremoving the chosen column" << std::endl;
    chosencolumn->UnremoveColumn();
    assert(chosencolumn->size() == originalsize);
   
    return flag;
}

