/*
 * File:   constraint_propagation.hpp
 *
 * This file is part of SudokuSolver.
 *
 * Author: Michael Morckos <mikey.morckos@gmail.com>
 *
 * This sudoku solving techique is based on the constraint programming based algorithm by
 * Peter Norvig.
 *
 * Reference: http://norvig.com/sudoku.html
 */

#ifndef CONSTRAINT_PROPAGATION_HPP
#define CONSTRAINT_PROPAGATION_HPP

#include <vector>

class Cell
{
public:
  /*! \brief Constructor of Cell. Cell represents the building block of a Sudoku puzzle. In a 9x9
   * puzzle, cell size is 3x3
   */
  Cell ();

  /*! \brief Returns the current status of a slot inside the cell.
   * 
   * \param i Index of slot of type int.
   *
   * \return Status of type bool.
   */
  bool is_on (const int i) const;

  /*! \brief Returns the count of active slots in a cell.
   *
   * \return Slots count.
   */
  int count () const;

  /*! \brief Eliminates a slot inside the cell from the search space.
   * 
   * \param i Index of slot of type int.
   */
  void eliminate (const int i);

  /*! \brief Returns the value of the cell.
   * 
   * \return Cell value of type int.
   */
  int get_value () const;

private:
  std::vector <bool> flags_;
};

//==================================================================================================
//==================================================================================================

class CSPSolver
{
public:
  /*! \brief Constructor of CSPSolver.
   *
   * \param input_grid Sudoku puzzle to solve of type std::vector <std::vector<int> >.
   */
  CSPSolver (const std::vector <std::vector<int> >& input_grid);

  /*! \brief Initializes internal state and global variables.
   */
  static void init ();

  /*! \brief Returns the validity of a given input puzzle.
   *
   * \return Validity of type bool.
   */
  bool is_valid () const;

  /*! \brief Returns the next cell to consider in the search process.
   *
   * \param i Index of cell of type int.
   * 
   * \return Next cell of type Cell.
   */
  Cell possible (const int i) const;

  /*! \brief Returns whether the puzzle was solved or not.
   * 
   * \return Status of type bool.
   */
  bool is_solved () const;

  /*! \brief Assigns a value to a cell. Used for populating internal constructs with the unsolved
   * puzzle data.
   *
   * \param k Cell index of type int.
   * \param value Value of type val.
   * 
   * \return Status of type bool.
   */
  bool assign (const int k, const int value);

  /*! \brief Returns the cell with the least number of available slots.
   *
   * \return ID of cell of type int.
   */
  int least_count () const;

  /*! \brief Copies the puzzle's solution to the final container.
   * 
   * \param output_grid Solved Sudoku puzzle of type std::vector <std::vector<int> >.
   */
  void output (std::vector <std::vector <int> >& output_grid) const;

private:
  std::vector <Cell> nodes_;
  bool valid_;
  static std::vector <std::vector<int> > group_;
  static std::vector <std::vector<int> > neighbors_;
  static std::vector <std::vector<int> > groups_of_;

  /*! \brief Eliminates a value from a cell, narrowing the search space.
   *
   * \param k Index of cell of type int.
   * \param value Value to be eliminated of type int.
   * 
   * \return Status of type bool. Indicates if the algorithm ended up in an invalid state.
   */
  bool eliminate (const int k, const int value);
};

/*! \brief Auxiliary function to be called to solve a puzzle.
 *
 * \param solver pointer of type CSPSolver.
 * 
 * \return pointer of type CSPSolver.
 */
std::unique_ptr<CSPSolver> solve_csp_aux (std::unique_ptr<CSPSolver> solver);

#endif // CONSTRAINT_PROPAGATION_HPP
