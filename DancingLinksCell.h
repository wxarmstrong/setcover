#ifndef DANCINGLINKSCELL_H
#define DANCINGLINKSCELL_H

#include "DancingLinksColumn.h"

 // 1-elements in the adjacency list
class DancingLinksCell {
protected:
	// Neighbours
	DancingLinksCell* left_ptr_, * right_ptr_, * up_ptr_, * down_ptr_;

	// Pointer to the current column
	DancingLinksColumn* column_ptr_;

	// ID of the current row
	int row_id_;

public:
	// Constructor
	DancingLinksCell();

	// Accessors
	DancingLinksCell* left_ptr() { return left_ptr_; }
	DancingLinksCell* right_ptr() { return right_ptr_; }
	DancingLinksCell* up_ptr() { return up_ptr_; }
	DancingLinksCell* down_ptr() { return down_ptr_; }

	DancingLinksColumn* column_ptr() { return column_ptr_; }

	int row_id() { return row_id_; }

	// Mutators
	void set_left_ptr(DancingLinksCell* ptr) { left_ptr_ = ptr; }
	void set_right_ptr(DancingLinksCell* ptr) { right_ptr_ = ptr; }
	void set_up_ptr(DancingLinksCell* ptr) { up_ptr_ = ptr; }
	void set_down_ptr(DancingLinksCell* ptr) { down_ptr_ = ptr; }

	void set_column_ptr(DancingLinksColumn* ptr) { column_ptr_ = ptr; }
	void set_row_id(int row_id) { row_id_ = row_id; }

	// Remove the cell from the linked list in vertical direction
	void RemoveCell();

	// Put the cell back in the linked list in vertical direction
	void UnremoveCell();

	// Remove a cell from the board entirely
	void Erase();
};

#endif
