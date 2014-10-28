/*
 * File:   exact_cover.cpp
 *
 * This file is part of SudokuSolver.
 *
 * Author: Michael Morckos <mikey.morckos@gmail.com>
 *
 * This sudoku solving techique employs Algorithm X for solving the exact cover problem using
 * dancing links (DLX), by Donald Knuth.
 *
 * Reference: http://en.wikipedia.org/wiki/Knuth's_Algorithm_X
 *            http://en.wikipedia.org/wiki/Dancing_Links
 */

#include <math.h>
#include "exact_cover.hpp"

ExactCoverSolver::ExactCoverSolver ():
  solved_(false),
  total_competition_ (0),
  GRID_SIZE_ (0),
  ROW_OFFSET_ (0),
  COL_OFFSET_ (0),
  CELL_OFFSET_ (0),
  BOX_OFFSET_ (0),
  MAX_COLS_ (0),
  MAX_ROWS_ (0),
  COL_BOX_DIV_ (0),
  ROW_BOX_DIV_ (0)
{
  root_ = new Node<int>;
  root_->header_ = true;
  root_->right_ = root_;
  root_->left_ = root_;
  root_->top_ = root_;
  root_->bottom_ = root_;
  solved_ = false;
}

ExactCoverSolver::~ExactCoverSolver ()
{
  cleanup ();
  delete root_;
}

bool ExactCoverSolver::init (const int grid_size)
{
  GRID_SIZE_ = grid_size;
  ROW_OFFSET_ = 0;
  COL_OFFSET_ = pow (grid_size, 2);
  CELL_OFFSET_ = COL_OFFSET_ * 2;
  BOX_OFFSET_ = COL_OFFSET_ * 3;
  MAX_COLS_ = COL_OFFSET_ * 4;
  MAX_ROWS_ = COL_OFFSET_ * GRID_SIZE_;
  
  if (grid_size == 9)
  {
    COL_BOX_DIV_ = 3;
    ROW_BOX_DIV_ = 3;
  }
  else if (grid_size == 10)
  {
    COL_BOX_DIV_ = 2;
    ROW_BOX_DIV_ = 5;
  }
  else if (grid_size == 12)
  {
    COL_BOX_DIV_ = 4;
    ROW_BOX_DIV_ = 3;
  }
  else if (grid_size == 16)
  {
    COL_BOX_DIV_ = 4;
    ROW_BOX_DIV_ = 4;
  }
  else
  {
    return false;
  }
  
  Node<int>* matrix[MAX_ROWS_][MAX_COLS_];
  int row = 0;
  int tmp = 0;
  Node<int>* row_node = NULL;
  Node<int>* col_node = NULL;
  Node<int>* cell_node = NULL;
  Node<int>* box_node = NULL;
  //initialize matrix
   for (int i = 0; i < MAX_ROWS_; ++i)
   {
     for (int j = 0; j < MAX_COLS_; ++j)
     {
       matrix[i][j] = NULL;
     }
   }
    
  for (int i = 0; i < GRID_SIZE_; ++i)
  {
    for (int j = 0; j < GRID_SIZE_; ++j)
    {
      for (int k = 0; k < GRID_SIZE_; ++k)
      {
        row = (i * COL_OFFSET_ + j * GRID_SIZE_ + k);
        row_node = new Node<int> (i, j, k);
        matrix[row][ROW_OFFSET_ + (i * GRID_SIZE_ + k)] = row_node;
        
        col_node = new Node<int> (i, j, k);
        matrix[row][COL_OFFSET_ + (j * GRID_SIZE_ + k)] = col_node;
        
        cell_node = new Node<int> (i, j, k);
        matrix[row][CELL_OFFSET_ + (i * GRID_SIZE_ + j)] = cell_node;
        
        tmp = BOX_OFFSET_ + ((i / ROW_BOX_DIV_ + j / COL_BOX_DIV_ * COL_BOX_DIV_) * GRID_SIZE_ + k);
        box_node = matrix[row][tmp] = new Node<int> (i, j, k);
        /// Link nodes
        row_node->right_ = col_node;
        row_node->left_ = box_node;
        
        col_node->left_ = row_node;
        col_node->right_ = cell_node;
        
        cell_node->left_ = col_node;
        cell_node->right_ = box_node;
        
        box_node->left_ = cell_node;
        box_node->right_ = row_node;
      }
    }
  }
  
  Node<int>* next_col_header;
  Node<int>* next_col_row;
  /// Link columns to construct dancing links
  for (int j = 0; j < MAX_COLS_; ++j)
  {
    next_col_header = new Node<int>;
    next_col_header->header_ = true;
    next_col_header->top_ = next_col_header;
    next_col_header->bottom_ = next_col_header;
    next_col_header->left_ = next_col_header;
    next_col_header->right_ = next_col_header;
    next_col_header->col_header_ = next_col_header;
    next_col_row = next_col_header;
    for (int i = 0; i < MAX_ROWS_; ++i)
    {
      if (matrix[i][j] != NULL)
      {
        //search down rows to add to column
        matrix[i][j]->top_ = next_col_row;
        next_col_row->bottom_ = matrix[i][j];
        matrix[i][j]->bottom_ = next_col_header;
        next_col_header->top_ = matrix[i][j];
        matrix[i][j]->col_header_ = next_col_header;
        next_col_row = matrix[i][j];
      }
    }
    if (next_col_header->bottom_ == next_col_header)
    {
      return false;
    }
    
    if (!create_col (next_col_header))
    {
      /// Cleanup
      for (int i = 0; i < MAX_ROWS_; ++i)
      {
        for (int j = 0; j < MAX_COLS_; ++j)
        {
          if (matrix[i][j] != NULL)
          {
            delete matrix[i][j];  
          }
        }
      }
      return false;
    }
  }
  return true;
}

void ExactCoverSolver::solve (const std::vector <std::vector <int> >& input_grid)
{
  Node <int>* to_find = NULL;
  Node <int>* insert_next = NULL;
  std::stack <Node <int>*> puzzle_nodes;
  Node <int>* col_node;
  Node <int>* next_row_in_col;
  Node <int>* row_node;
  int val = 0;
  
  solved_ = false;
  total_competition_ = 0;
//  running_sol_.clear ();
  while (!running_sol_.empty ())
  {
    running_sol_.pop();
  }

  for (int i = 0; i < GRID_SIZE_; ++i)
  {
    for (int j = 0; j < GRID_SIZE_; ++j)
    {
      val = input_grid[i][j];
      if (val > GRID_SIZE_ || val < 0)
      {
        std::cout << "ERROR! Invalid puzzle specified." << std::endl;
        return;
      }
      
      if (val != 0)
      {
        to_find = new Node<int> (i, j, val - 1);
        insert_next = find (to_find);
        if (insert_next == NULL)
        {
          std::cerr << "ERROR! Repeated or invalid value '" << val \
          << "' in puzzle at row: " << i + 1 << ", column: " << j + 1 << "." << std::endl;
          return;
        }
        col_node = insert_next->col_header_;
        next_row_in_col = insert_next;
        cover (col_node);
        row_node = next_row_in_col->right_;
        while (row_node != next_row_in_col)
        {
          cover (row_node->col_header_);
          row_node = row_node->right_;
        }
        puzzle_nodes.push (insert_next);
        running_sol_.push (insert_next);
        delete to_find;
        to_find = NULL;
      }
    }
  }

  if (!solve ())
  {
    std::cout << "Puzzle is not solvable or multiple solutions exists." << std::endl; 
  }
  /// Restore initial state to prepare for next puzzle
  while (!puzzle_nodes.empty())
  {
    col_node = (puzzle_nodes.top ())->col_header_;
    next_row_in_col = (puzzle_nodes.top ());
    row_node = next_row_in_col->right_;
    while (row_node != next_row_in_col)
    {
      uncover (row_node->col_header_);
      row_node = row_node->right_;
    }
    uncover (col_node);
    puzzle_nodes.pop();
  }
}

bool ExactCoverSolver::is_solved () const
{
  return solved_;
}

void ExactCoverSolver::output (std::vector <std::vector <int> >& output_grid)
{
  Node<int>* next = NULL;
  
  if (!running_sol_.empty ())
  {
    while (!running_sol_.empty ())
    {
      next = running_sol_.top ();
      output_grid[next->row_][next->col_] = next->value_ + 1;
      running_sol_.pop ();
    }
  }
}

bool ExactCoverSolver::solve ()
{
  int cols_count;
  Node<int>* next_col = NULL;
  Node<int>* next_row_in_col = NULL;
  Node<int>* row_node = NULL;

  if (empty ())
  {
    return true;
  }
  next_col = pick_next_col (cols_count);
  if (cols_count < 1)
  {
    return false;
  }
  total_competition_ += cols_count;
  next_row_in_col = next_col->bottom_;
  cover (next_col);
  while (next_row_in_col != next_col && !solved_)
  {
    running_sol_.push (next_row_in_col);
    row_node = next_row_in_col->right_;
    while (row_node != next_row_in_col)
    {
      cover (row_node->col_header_);
      row_node = row_node->right_;
    }
    solved_= solve ();
    if (!solved_)
    {
      running_sol_.pop ();
    }
    row_node = next_row_in_col->right_;
    while (row_node != next_row_in_col)
    {
      uncover (row_node->col_header_);
      row_node = row_node->right_;
    }
    next_row_in_col = next_row_in_col->bottom_;
  }
  uncover (next_col);

  return solved_;
}

bool ExactCoverSolver::create_col (Node<int>* new_node)
{
  if (!new_node || !new_node->header_)
  {
    return false;
  }

  return create_col_helper (new_node, root_);
}

bool ExactCoverSolver::create_col_helper (Node<int>* new_node, Node<int>* curr_root)
{
  if (curr_root->right_ == root_ && curr_root != new_node)
  {
    curr_root->right_->left_ = new_node;
    new_node->right_ = curr_root->right_;
    new_node->left_ = curr_root;
    curr_root->right_ = new_node;
    
    return true;
  }
  else if (curr_root == new_node)
  {
    return false;  
  }
  else
  {
    return create_col_helper (new_node, curr_root->right_);
  }
}

bool ExactCoverSolver::empty ()
{
//  assert (root_ != NULL);
  return (root_->right_ == root_);
}

void ExactCoverSolver::cover (Node<int>* node)
{
  Node<int>* row_node = NULL;
  Node<int>* right_node = NULL;
  Node<int>* col_node = node->col_header_;
  
  col_node->right_->left_ = col_node->left_;
  col_node->left_->right_ = col_node->right_;
  for (row_node = col_node->bottom_; row_node != col_node; row_node = row_node->bottom_)
  {
    for (right_node = row_node->right_; right_node != row_node; right_node = right_node->right_)
    {
      right_node->top_->bottom_ = right_node->bottom_;
      right_node->bottom_->top_ = right_node->top_;
    }
  }
}

void ExactCoverSolver::uncover (Node<int>* node)
{
  Node<int>* row_node = NULL;
  Node<int>* left_node = NULL;
  Node<int>* col_node = node->col_header_;
  
  for (row_node = col_node->top_; row_node != col_node; row_node = row_node->top_)
  {
    for (left_node = row_node->left_; left_node != row_node; left_node = left_node->left_)
    {
      left_node->top_->bottom_ = left_node;
      left_node->bottom_->top_ = left_node;
    }
  }
  col_node->right_->left_ = col_node;
  col_node->left_->right_ = col_node;
}

Node<int>* ExactCoverSolver::find (Node<int>* node)
{
  Node<int>* right_node = root_->right_;
  Node<int>* bottom_node = NULL;
  
  while (right_node != root_)
  {
    bottom_node = right_node->bottom_;
    while (bottom_node != right_node)
    {
      if (bottom_node->row_ == node->row_ && bottom_node->col_ == node->col_
        && bottom_node->value_ == node->value_)
      {
        return bottom_node;
      }
      bottom_node = bottom_node->bottom_;
    }
    right_node = right_node->right_;
  }
  
  return NULL;
}

Node<int>* ExactCoverSolver::pick_next_col (int& count)
{
  Node<int>* curr_best = root_->right_;
  Node<int>* next = curr_best->bottom_;
  Node<int>* next_col = curr_best;
  int best = -1;
  int tmp_count = 0;

  while (next_col != root_)
  {
    next = next_col->bottom_;
    tmp_count = 0;
    while (next != next_col)
    {
//      if (next == next->bottom_)
//      {
//        std::cout<<"Err!" << std::endl;
//      }
      ++tmp_count;
      next = next->bottom_;
    }
    if (tmp_count < best || best == -1)
    {
      curr_best = next_col;
      best = tmp_count;
    }
    next_col = next_col->right_;
  }
//  if (curr_best == root_)
//  {
//    exit (0);
//  }
  count = best;

  return curr_best;
}

void ExactCoverSolver::cleanup ()
{
  Node<int>* col_del = root_->right_;
  Node<int>* row_del;
  Node<int>* tmp;
  while (col_del != root_)
  {
    row_del = col_del->bottom_;
    while (!row_del->header_)
    {
      tmp = row_del->bottom_;
      delete row_del;
      row_del = tmp;
    }
    tmp = col_del->right_;
    delete col_del;
    col_del = tmp;
  }
}
