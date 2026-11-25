/*
 * STUDENT CODE - EDIT THIS FILE
 * Keep the class name and required interface.
 * Otherwise, add or change any data members or
 * member functions.
 */

#pragma once

#include "mothership.h"
#include "seafloor_survey.h"
#include "trees.h"

// #include "examples/robot_selector.h" // EXAMPLE

class Search
{
public:
  explicit Search(Mothership &mothership); // REQUIRED

  SearchResult findShipwreck(); // REQUIRED

private:
  Mothership &mothership; // REQUIRED

  int roboCount;
  AVLMap<int,int> abyss;
  AVLMap<int, std::vector<int>> debris;
  int nextRobot;

  int getLastSegment(int robot);
  void addDebris(int robot, int segment);
  int countDebris(int robot, int position);
  int signalStrength(int robot);
  bool isDone(int robot);
  int findBestRobot(int current);

};
