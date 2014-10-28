/*
 * File:   sudoku_solver.cpp
 *
 * This file is part of SudokuSolver.
 *
 * Author: Michael Morckos <mikey.morckos@gmail.com>
 */

#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <memory>

#include "sudoku_solver.hpp"
#include "constraint_propagation.hpp"

const static int CSP_TECH = 1; 
const static int DLX_TECH = 2;

SudokuSolver::SudokuSolver ():
  print_time_ (false),
  technique_ (CSP_TECH),
  grid_size_ (9),
  ready_ (false),
  display_ (false)
{}

bool SudokuSolver::init ()
{
  if (!ec_solver_.init (grid_size_))
  {
    std::cout << "ERROR! Could not initialize Sudoko DLX solver." << std::endl;
    return false;
  }
  CSPSolver::init ();
  ready_ = true;
  
  return true;
}

bool SudokuSolver::is_ready () const
{
  return ready_;
}

void SudokuSolver::solve (std::string infile, std::string outfile)
{
  std::vector <Puzzle> puzzles;
  std::ofstream out;
  int win_count = 0;
  /// Check if ready
  if (!ready_)
  {
    std::cout << "WARNING! Sodoku solver is not initialized. Halted." << std::endl;
    return;
  }
  /// Validate input
  if (!validate_input (infile, puzzles))
  {
    return;
  }
  /// Solve puzzle(s) using the selected technique
  for (unsigned int i = 0; i < puzzles.size (); ++i)
  {
    std::cout << "Solving puzzle: " << i + 1 << std::endl;
    if (technique_ == DLX_TECH || grid_size_ > 9)
    {
      solve_EC (puzzles[i]);
    }
    else if (technique_ == CSP_TECH)
    {
      sovle_CSP (puzzles[i]);
    }
  }
  /// Output puzzle(s)
  out.open (outfile);
  for (unsigned int i = 0; i < puzzles.size (); ++i)
  {
    if (puzzles[i].solved)
    {
      output_puzzle (puzzles[i], out);
      ++win_count;
    }
    else
    {
      out << "+++++ Could not solve puzzle. +++++\n";
      if (display_)
      {
        std::cout << "Could not solve puzzle." << std::endl;
      }
    }
    out << "\n";
    if (display_)
    {
      std::cout << std::endl;
    }
  }
  out.close ();
  std::cout << "Solved " << win_count << " puzzle(s)" << std::endl;
}

void SudokuSolver::toggle_print_time (const bool flag)
{
  print_time_ = flag;
}

void SudokuSolver::toggle_terminal_output (const bool flag)
{
  display_ = flag;
}

void SudokuSolver::set_technique (const int technique)
{
  if (technique != CSP_TECH && technique != DLX_TECH)
  {
    std::cout << "WARNING! Invalid technique code. Resorting to default technique." << std::endl;
    return;
  }
  technique_ = technique;
}

void SudokuSolver::set_grid_size (const int size)
{
  grid_size_ = size;
}

bool SudokuSolver::validate_input (const std::string& infile, std::vector <Puzzle>& puzzles)
{
  std::ifstream in;
  std::string line;
  int count = 0;
  Puzzle curr_puzzle;
  std::vector <int> tmp_list;

  if (infile.empty ())
  {
    std::cerr << "ERROR! Empty filename." << std::endl;
    return false;
  }

  try
  {
    in.open (infile);
    if (!in.is_open ())
    {
      std::cerr << "ERROR! Nonexistent or corrupted input file." << std::endl;
      return false;
    }
    while (!in.eof ())
    {
      std::getline (in, line);
      if (!line.empty () && !blank (line))
      {
        /// Completed a grid
        if (count != 0 && count % grid_size_ == 0)
        {
          curr_puzzle.solved = false;
          curr_puzzle.output_grid = curr_puzzle.input_grid;
          puzzles.push_back (curr_puzzle);
          curr_puzzle.clear ();
          count = 0;
        }
        /// Validate line
        curr_puzzle.input_grid.push_back (tmp_list);
        if (!validate_line (line, curr_puzzle, count))
        {
          return false;
        }
        ++count;
      }
    }
    if (count % grid_size_ != 0)
    {
      std::cerr << "ERROR! One or more incomplete puzzles in input file." << std::endl;
      return false;
    }
    else if (count == grid_size_)
    {
      curr_puzzle.solved = false;
      curr_puzzle.output_grid = curr_puzzle.input_grid;
      puzzles.push_back (curr_puzzle);
    }
    return true;
  }
  catch (std::ifstream::failure e)
  {
    std::cerr << "ERROR! Nonexistent or corrupted input file." << std::endl;
  }
  
  return false;
}

bool SudokuSolver::validate_line (std::string& line, Puzzle& puzzle, const int count)
{
  char tmp[line.size () + 1];
  char* token = NULL;
  int num = 0;

  for (unsigned int i = 0; i < line.size (); ++i)
  {
    tmp[i] = line[i];
  }
  tmp[line.size ()] = '\0';
  token = strtok (tmp, " ,;.");
  while (token != NULL)
  {
    num = atoi (token);
    if (num == 0 && (token[0] != '0' || strlen (token) > 1))
    {
      std::cerr << "ERROR! Erroneous data in input file: " << token << std::endl;
      return false;
    }
    puzzle.input_grid[count].push_back (num);
    token = strtok (NULL, " ,;.");
  }
  if ((int) puzzle.input_grid[count].size () != grid_size_)
  {
    std::cerr << "ERROR! One or more incomplete puzzles in file." << std::endl;
    return false;
  }
  
  return true;
}

bool SudokuSolver::blank (const std::string& line)
{
  for (unsigned int i = 0; i < line.size (); ++i)
  {
    if (line[i] != ' ' && line[i] != '\t' && line[i] != '\b')
    {
      return false;
    }
  }
  
  return true;
}

void SudokuSolver::solve_EC (Puzzle& puzzle)
{
  struct timeval then;
  struct timeval now;

  gettimeofday (&then, NULL);
  ec_solver_.solve (puzzle.input_grid);
  if (ec_solver_.is_solved ())
  {
    ec_solver_.output (puzzle.output_grid);
    puzzle.solved = true;
  }
  else
  {
    puzzle.solved = false;
  }
  gettimeofday (&now, NULL);
  puzzle.proc_time = (now.tv_sec - then.tv_sec + (1e-6 * (now.tv_usec - then.tv_usec)));
}

void SudokuSolver::sovle_CSP (Puzzle& puzzle) const
{
  struct timeval then;
  struct timeval now;
  
  gettimeofday (&then, NULL);
  if (auto csp = solve_csp_aux (std::unique_ptr<CSPSolver> (new CSPSolver(puzzle.input_grid))))
  {
    if (!csp->is_valid ())
    {
      return;
    }
    csp->output (puzzle.output_grid);
  }
  else
  {
    std::cout << "ERROR! Could not initialize Sudoko CSP solver." << std::endl;
    return;
  }
  gettimeofday (&now, NULL);
  puzzle.solved = true;
  puzzle.proc_time = (now.tv_sec - then.tv_sec + (1e-6 * (now.tv_usec - then.tv_usec)));
}

void SudokuSolver::output_puzzle (Puzzle& puzzle, std::ofstream& out)
{
  /// Output execution time if option is selected
  if (print_time_)
  {
    out << "Processing time: " << std::to_string (puzzle.proc_time) << " s" << "\n";
    if (display_)
    {
      std::cout << "Processing time: " << std::to_string (puzzle.proc_time) << " s" << "\n";
    }
  }
  for (unsigned int i = 0; i < puzzle.output_grid.size (); ++i)
  {
    for (unsigned int j = 0; j < puzzle.output_grid[i].size (); ++j)
    {
      out << puzzle.output_grid[i][j];
      if (display_)
      {
        std::cout << puzzle.output_grid[i][j];
      }
      if (j < puzzle.output_grid[i].size () - 1)
      {
        out << ", ";
        if (display_)
        {
          std::cout << ", ";
        }
      }
    }
    out << "\n";
    if (display_)
    {
      std::cout << std::endl;
    }
  }
}
