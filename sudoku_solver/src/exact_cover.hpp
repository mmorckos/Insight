/*
 * File:   exact_cover.hpp
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

#ifndef EXACT_COVER_HPP
#define EXACT_COVER_HPP

#include <vector>
#include <stack>
#include <iostream>

class ExactCoverSolver;

template <typename T>
class Node
{
public:
  /*! \brief Constructor of Node. Node is the building block for dancing links, a modified
   * doubly linked-list that can have horizontal and vertical neighbors.
   */
  Node ();

  /*! \brief Constructor of Node.
   *
   * \param r Row index of type int.
   * \param c Column index of type int.
   * \param r Node value.
   */
  Node (const int r, const int c, const T v);

private:
  friend class ExactCoverSolver;
  T value_;
  int row_;
  int col_;
  bool header_;
  Node<T>* left_;
  Node<T>* right_;
  Node<T>* top_;
  Node<T>* bottom_;
  Node<T>* col_header_;
};

template <typename T>
Node<T>::Node ():
  row_ (-1),
  col_ (-1),
  header_ (false),
  left_ (NULL),
  right_ (NULL),
  top_ (NULL),
  bottom_ (NULL),
  col_header_ (NULL)
{}

template <typename T>
Node<T>::Node (const int r, const int c, const T v):
  value_ (v),
  row_ (r),
  col_ (c),
  header_ (false),
  left_ (NULL),
  right_ (NULL),
  top_ (NULL),
  bottom_ (NULL),
  col_header_ (NULL)
{}

//==================================================================================================
//==================================================================================================

class ExactCoverSolver
{
public:
  ExactCoverSolver ();
  
  ~ExactCoverSolver ();
  
  /*! \brief Initializes solver with grid size.
   *
   * \param grid_size Grid size of type int.
   * 
   * \return Outcome of the process of type bool.
   */
  bool init (const int grid_size);

  /*! \brief Solves a sudoku puzzle.
   * 
   * \param input_grid Sudoku puzzle to solve of type std::vector <std::vector<int> >.
   */
  void solve (const std::vector <std::vector <int> >& input_grid);

  /*! \brief Returns the status of the current puzzle.
   * 
   * \return Returns true if puzzle was successfully solved, false otherwise.
   */
  bool is_solved () const;

  /*! \brief Copies the puzzle's solution to the final container.
   * 
   * \param output_grid Solved Sudoku puzzle of type std::vector <std::vector<int> >.
   */
  void output (std::vector <std::vector <int> >& output_grid);

private:
  Node <int>* root_;
  std::stack<Node <int>* > running_sol_;
  bool solved_;
  int total_competition_;
  int GRID_SIZE_;
  int ROW_OFFSET_;
  int COL_OFFSET_;
  int CELL_OFFSET_;
  int BOX_OFFSET_;
  int MAX_COLS_;
  int MAX_ROWS_;
  int COL_BOX_DIV_;
  int ROW_BOX_DIV_;
  
  /*! \brief Solves a given puzzle.
   * 
   * \return Outcome of the process of type bool.
   */
  bool solve ();

  /*! \brief Creates a column in the DLX structure.
   *
   * \param new_node New node to be added.
   * 
   * \return Outcome of the process of type bool.
   */
  bool create_col (Node<int>* new_node);

  /*! \brief Helper function for creating a new column.
   * 
   * \param new_node New node to be added.
   * \param new_node New node's parent in the hierarchy.
   *
   * \return Outcome of the process of type bool.
   */
  bool create_col_helper (Node<int>* new_node, Node<int>* r);
  
  bool empty ();

  /*! \brief Removes a certain node from the search space.
   * 
   * \param node Designated node to be removed.
   */
  void cover (Node<int>* node);

  /*! \brief Restores a certain node into the search space.
   * 
   * \param node Designated node to be restored.
   */
  void uncover (Node<int>* node);

  /*! \brief Find a certain node given its parent in the hierarchy.
   * 
   * \param node Parent node.
   *
   * \return Pointer to found node or NULL otherwise.
   */
  Node<int>* find (Node<int>* node);

  /*! \brief Pick next column for search. Store its score.
   * 
   * \param count Reference to resulting node score.
   *
   * \return Pointer to found node or NULL otherwise.
   */
  Node<int>* pick_next_col (int& count);

  /*! \brief Memory management function for releasing resources.
   */
  void cleanup ();
};

#endif /// EXACT_COVER_HPP
