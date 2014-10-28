/*
 * File:   sudoku_solver.hpp
 *
 * This file is part of SudokuSolver.
 *
 * Author: Michael Morckos <mikey.morckos@gmail.com>
 */

#ifndef SUDOKU_SOLVER_HPP
#define SUDOKU_SOLVER_HPP

#include <vector>
#include <iostream>
#include <fstream>

#include "exact_cover.hpp"

struct Puzzle
{
  std::vector <std::vector <int> > input_grid;
  std::vector <std::vector <int> > output_grid;
  double proc_time;
  bool solved;

  void clear ()
  {
    input_grid.clear ();
    output_grid.clear ();
    proc_time = 0.0;
    solved = false;
  }
};

//==================================================================================================
//==================================================================================================

class SudokuSolver
{
public:
  SudokuSolver ();

  /*! \brief Initialize solver.
   *
   * \return true if initialization is success, false otherwise.
   */
  bool init ();

  /*! \brief Returns the readiness status of the solver.
   * 
   * \return true if solver is ready, false otherwise.
   */
  bool is_ready () const;

  /*! \brief Solves a puzzle given input and output files.
   * 
   * \param infile Input file of type string.
   * \param infile Output file of type string.
   */
  void solve (std::string infile, std::string outfile = "sudoku_output.txt");

  /*! \brief Enable/disable record of processing time.
   * 
   * \param flag Toggle flag.
   */
  void toggle_print_time (const bool flag);

  /*! \brief Enable/disable terminal output.
   * 
   * \param flag Toggle flag.
   */
  void toggle_terminal_output (const bool flag);

  /*! \brief Set sudoku solving technique.
   * 
   * \param technique Technique ID of type int.
   */
  void set_technique (const int technique);

  /*! \brief Set puzzle grid size.
   * 
   * \param size Puzzle size of type int.
   */
  void set_grid_size (const int size);

private:
  bool print_time_;
  int technique_;
  int grid_size_;
  bool ready_;
  bool display_;
  ExactCoverSolver ec_solver_;

  /*! \brief Validates input.
   * 
   * \param infile Input file of type string.
   * \param puzzles List of puzzles of type std::vector <Puzzle>.
   *
   * \return true if validation is success, false otherwise
   */
  bool validate_input (const std::string& infile, std::vector <Puzzle>& puzzles);

  /*! \brief Validates input line.
   * 
   * \param line Input line of type string.
   * \param puzzle Puzzle.
   * \param count Data location in current puzzle.
   *
   * \return true if validation is success, false otherwise
   */
  bool validate_line (std::string& line, Puzzle& puzzle, const int count);

  /*! \brief Checks if input line contains characters other than whitespaces.
   *
   * \return true if line is composed of one or more whitespaces, false otherwise.
   */
  bool blank (const std::string& line);

  /*! \brief Solves a puzzle using Algorithm X.
   * 
   * \param puzzle Input puzzle.
   */
  void solve_EC (Puzzle& puzzle);

  /*! \brief Solves a puzzle using CSP.
   * 
   * \param puzzle Input puzzle.
   */
  void sovle_CSP (Puzzle& puzzle) const;

  /*! \brief Outputs a solved puzzle.
   * 
   * \param puzzle Solved puzzle.
   * \param out Output stream.
   */
  void output_puzzle (Puzzle& puzzle, std::ofstream& out);
};

#endif /// SUDOKU_SOLVER_HPP
