/*
 * File:   constraint_propagation.cpp
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

#include <algorithm>
#include <memory>
#include <iostream>
#include <math.h>

#include "constraint_propagation.hpp"

static const int GRID_SIZE = 9;
static const int TUPLE_SIZE = GRID_SIZE / 3;

Cell::Cell():
  flags_(GRID_SIZE, true)
{}

bool Cell::is_on (const int i) const
{
  return flags_[i-1];
}

int Cell::count () const
{
  return std::count (flags_.begin (), flags_.end (), true);
}

void Cell::eliminate (const int i)
{
  flags_[i-1] = false;
}

int Cell::get_value () const
{
  auto it = find (flags_.begin (), flags_.end (), true);
  
  return (it != flags_.end () ? 1 + (it - flags_.begin ()) : -1);
}

//==================================================================================================
//==================================================================================================

std::vector <std::vector <int> > CSPSolver::group_ (GRID_SIZE * TUPLE_SIZE);
std::vector <std::vector <int> > CSPSolver::neighbors_ (pow (GRID_SIZE, 2));
std::vector <std::vector <int> > CSPSolver::groups_of_ (pow (GRID_SIZE, 2));

CSPSolver::CSPSolver (const std::vector <std::vector<int> >& input_grid):
  nodes_ (pow (GRID_SIZE, 2)),
  valid_ (true)
{
  int counter = 0;
  
  for (unsigned int i = 0; i < input_grid.size (); ++i)
  {
    for (unsigned int j = 0; j < input_grid[i].size (); ++j)
    {
      if (input_grid[i][j] != 0)
      {
        if (!assign (counter, input_grid[i][j]))
        {
          std::cerr << "ERROR! Repeated or invalid value '" << input_grid[i][j] \
          << "' in puzzle at row: " << i + 1 << ", column: " << j + 1 << "." << std::endl;
          valid_ = false;
          return;
        }
      }
      ++counter;
    }
  }
}

void CSPSolver::init ()
{
  int k = 0;
  int val = 0;
  
  for (int i = 0; i < GRID_SIZE; ++i)
  {
    for (int j = 0; j < GRID_SIZE; ++j)
    {
      const int x[TUPLE_SIZE] = {i, GRID_SIZE + j, 18 + (i/TUPLE_SIZE) * TUPLE_SIZE + j/TUPLE_SIZE};

      k = i * GRID_SIZE + j;
      for (int g = 0; g < TUPLE_SIZE; ++g)
      {
        group_[x[g]].push_back (k);
        groups_of_[k].push_back (x[g]);
      }
    }
  }
  for (unsigned int i = 0; i < neighbors_.size (); ++i)
  {
    for (unsigned int j = 0; j < groups_of_[i].size (); ++j)
    {
      for (int k = 0; k < GRID_SIZE; ++k)
      {
        val = group_[groups_of_[i][j]][k];
        if (val != (int) i)
        {
          neighbors_[i].push_back (val);
        }
      }
    }
  }
}

bool CSPSolver::is_valid () const
{
  return valid_;
}

Cell CSPSolver::possible (const int i) const
{
  return nodes_[i];
}

bool CSPSolver::is_solved () const
{
  for (unsigned int i = 0; i < nodes_.size (); ++i)
  {
    if (nodes_[i].count () != 1)
    {
      return false;
    }
  }
  
  return true;
}

bool CSPSolver::assign (const int k, const int value)
{
  for (int i = 1; i <= GRID_SIZE; ++i)
  {
    if (i != value)
    {
      if (!eliminate (k, i))
      {
        return false;  
      }
    }
  }
  
  return true;
}

bool CSPSolver::eliminate (const int k, const int value)
{
  if (!nodes_[k].is_on (value))
  {
    return true;
  }
  nodes_[k].eliminate (value);
  const int N = nodes_[k].count ();

  if (N == 0)
  {
    return false;
  }
  else if (N == 1)
  {
    const int v = nodes_[k].get_value ();
    
    for (unsigned int i = 0; i < neighbors_[k].size (); ++i)
    {
      if (!eliminate (neighbors_[k][i], v))
      {
        return false;
      }
    }
  }
  for (unsigned int i = 0; i < groups_of_[k].size (); ++i)
  {
    const int x = groups_of_[k][i];
    int n = 0;
    int ks = 0;
    
    for (int j = 0; j < GRID_SIZE; ++j)
    {
      const int p = group_[x][j];
      
      if (nodes_[p].is_on (value))
      {
        ++n;
        ks = p;
      }
    }
    if (n == 0)
    {
      return false;
    }
    else if (n == 1)
    {
      if (!assign (ks, value))
      {
        return false;
      }
    }
  }
  
  return true;
}

int CSPSolver::least_count () const
{
  int k = -1;
  int min = 0;
  
  for (unsigned int i = 0; i < nodes_.size (); ++i)
  {
    const int m = nodes_[i].count ();
    
    if (m > 1 && (k == -1 || m < min))
    {
      min = m;
      k = i;
    }
  }
  
  return k;
}

void CSPSolver::output (std::vector <std::vector <int> >& output_grid) const
{
  for (int i = 0; i < GRID_SIZE; ++i)
  {
    for (int j = 0; j < GRID_SIZE; ++j)
    {
      output_grid[i][j] = nodes_[i * GRID_SIZE + j].get_value ();
    }
  }
}

std::unique_ptr<CSPSolver> solve_csp_aux (std::unique_ptr<CSPSolver> solver)
{
  int k = 0;
  Cell cell;
  
  if (solver == nullptr || !solver->is_valid () || solver->is_solved ())
  {
    return solver;
  }
  k = solver->least_count ();
  cell = solver->possible (k);
  for (int i = 1; i <= GRID_SIZE; i++)
  {
    if (cell.is_on (i))
    {
      std::unique_ptr<CSPSolver> solver_0 (new CSPSolver (*solver));
      
      if (solver_0->assign (k, i))
      {
        if (auto solver_1 = solve_csp_aux (std::move (solver_0)))
        {
          return solver_1;
        }
      }
    }
  }
  
  return {};
}
