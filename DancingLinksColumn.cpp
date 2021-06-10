#include "DancingLinksColumn.h"
#include "DancingLinksCell.h"
#include <cstdlib>
#include <cassert>
#include <iostream>

DancingLinksColumn::DancingLinksColumn() {
    left_ptr_ = right_ptr_ = NULL;
    dummycell_ = new DancingLinksCell;
    size_ = 0;
    essential_ = true;
}

DancingLinksColumn::~DancingLinksColumn() { delete dummycell_; }

void DancingLinksColumn::RemoveColumn() {
    std::cout << "RemoveColumn()" << std::endl;
    // Remove the column from the column list
    left_ptr_->set_right_ptr(right_ptr_);
    right_ptr_->set_left_ptr(left_ptr_);

    // Find the 1-cells in the column and remove the other 1-cells in those rows
    //  from their respective columns
    for (DancingLinksCell* ptr = dummycell_->down_ptr(); ptr != dummycell_;
        ptr = ptr->down_ptr()) {
        // Found a 1-cell in the column
        // Erase the row by finding its row neighbours and removing their
        //  vertical links
        std::cout << "Clearing row id " << ptr->row_id() << std::endl;
        for (DancingLinksCell* ptr2 = ptr->right_ptr(); ptr2 != ptr;
            ptr2 = ptr2->right_ptr()) {
            // 1-cell in the row
            // Remove vertical links
            ptr2->RemoveCell();
        }
    }
}

void DancingLinksColumn::UnremoveColumn() {
    std::cout << "UnremoveColumn()" << std::endl;
    // Put back the cells in the reverse order as they were removed
    for (DancingLinksCell* ptr = dummycell_->up_ptr(); ptr != dummycell_;
        ptr = ptr->up_ptr()) {
        std::cout << "New iteration: ptr = " << ptr << std::endl;
        std::cout << "Restoring row id " << ptr->row_id() << std::endl;
        for (DancingLinksCell* ptr2 = ptr->left_ptr(); ptr2 != ptr;
            ptr2 = ptr2->left_ptr()) {
            ptr2->UnremoveCell();
        }
    }

    // Insert the column to the column list
    left_ptr_->set_right_ptr(this);
    right_ptr_->set_left_ptr(this);
}

