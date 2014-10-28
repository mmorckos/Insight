/*
 * File:   main.cpp
 *
 * This file is part of SudokuSolver.
 *
 * Author: Michael Morckos <mikey.morckos@gmail.com>
 */

#include <string.h>

#include "sudoku_solver.hpp"

void display_usage ()
{
  std::cout << "SudokuSolver" << std::endl;
  std::cout << "Usage" << std::endl;
  std::cout << "  SudokuSolver [options] -f <input-file-name>" << std::endl << std::endl;
  std::cout << "Options" << std::endl;
  std::cout << "  -o <output-file-name>     = Solved puzzle(s) output file." << std::endl;
  std::cout << "  -t [1|2]                  = Technique used to solve puzzles." << std::endl;
  std::cout << "  -p                        = Enable recording of processing time." << std::endl;
  std::cout << "  -d                        = Enable display of solved puzzle(s) on terminal." \
  << std::endl;
}

int main (int argc, char** argv)
{
  SudokuSolver solver;
  std::string infile;
  std::string outfile;
  int i = 1;
  int technique = -1;
  
  if (argc <= 2 || (argc == 2 && (strcmp (argv[1], "-h") == 0 || strcmp (argv[1], "--help") == 0)))
  {
    display_usage ();
    return 0;
  }
  else
  {
    while (i < argc)
    {
      if ((strcmp (argv[i], "-p") == 0 || strcmp (argv[i], "--processing") == 0))
      {
        solver.toggle_print_time (true);
      }
      else if ((strcmp (argv[i], "-d") == 0 || strcmp (argv[i], "--display") == 0))
      {
        solver.toggle_terminal_output (true);
      }
      else if ((strcmp (argv[i], "-f") == 0 || strcmp (argv[i], "--file") == 0))
      {
        if (i + 1 == argc)
        {
          std::cout << "Missing input filename" << std::endl;
          display_usage ();
          return 0;
        }
        infile = argv [i + 1];
        ++i;
      }
      else if ((strcmp (argv[i], "-o") == 0 || strcmp (argv[i], "--output") == 0))
      {
        if (i + 1 == argc)
        {
          std::cout << "Missing output filename" << std::endl;
          display_usage ();
          return 0;
        }
        outfile = argv [i + 1];
        ++i;
      }
      else if ((strcmp (argv[i], "-t") == 0 || strcmp (argv[i], "--technique") == 0))
      {
        if (i + 1 == argc)
        {
          std::cout << "Missing technique code" << std::endl;
          display_usage ();
          return 0;
        }
        technique = atoi (argv [i + 1]);
        ++i;
      }
      else
      {
        display_usage ();
        return 0;
      }
      ++i;
    }
  }

  if (infile.empty ())
  {
    display_usage ();
    return 0;
  }
  solver.init ();
  if (technique != -1)
  {
    solver.set_technique (technique);
  }
  if (!outfile.empty ())
  {
    solver.solve (infile, outfile);
  }
  else
  {
    solver.solve (infile);
  }
  return 0;
}
