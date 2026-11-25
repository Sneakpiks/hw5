/*
 * STUDENT CODE - EDIT THIS FILE
 */

#include "search.h"

#include <algorithm>

Search::Search(Mothership &mothership)
    : mothership(mothership), roboCount(mothership.getRobotCount())
{
  for(const Location &location : mothership.getAbysses()){
    if(abyss.find(location.robot)== abyss.end()){
      abyss[location.robot] = location.segment;
    }
    else if(location.segment < abyss.at(location.robot)){
      abyss[location.robot] = location.segment;
    }
  }
}

int Search::getLastSegment(int robot){
  if(abyss.find(robot)!=abyss.end()){
    return std::min(TETHER_LENGTH, abyss.at(robot)-1);
  }
  return TETHER_LENGTH;
}

void Search::addDebris(int robot, int segment){
  debris[robot].push_back(segment);
}

int Search::countDebris(int robot, int position){
  if(debris.find(robot)==debris.end()){
    return 0;
  }

  int count = 0; 
  for(int segment : debris.at(robot)){
    if(segment >= position - SIGNAL_RANGE && segment <= position + SIGNAL_RANGE){
      count++;
    }
  }
  return count;
}

int Search::signalStrength(int robot){
  int position = mothership.getRobotPosition(robot);
  int total = 0;

  for(int offset = -1; offset <= 1; ++offset){
    int neighbor = (robot+offset+roboCount) % roboCount;
    total += countDebris(neighbor,position);
  }
  return total;
}

bool Search::isDone(int robot){
  return mothership.getRobotPosition(robot) >= getLastSegment(robot);
}

int Search::findBestRobot(){
  int best = -1;
  int bestSignal = -1;
  for(int robot = 0; robot < roboCount; ++robot){
    if(isDone(robot)){
      continue;
    }
    int signal = signalStrength(robot);
    if (signal>bestSignal){
      best = robot;
      bestSignal = signal;
    }
  }
  return best;
}

SearchResult Search::findShipwreck()
{
 int current = mothership.getActiveRobot();
 int noDebrisDrives = 0;

 while(true){
  if(!isDone(current)){
    int safeDistance = getLastSegment(current) - mothership.getRobotPosition(current);
    DriveResult result = mothership.driveActiveRobot(std::min(MAX_SEGMENTS_PER_DRIVE,safeDistance));

    if(result.reason == DriveStopReason::ShipwreckFound){
      return SearchResult{true, Location{current,mothership.getRobotPosition(current)}};
    }

    if(result.reason == DriveStopReason::DebrisFound){
      addDebris(current,mothership.getRobotPosition(current));
      noDebrisDrives = 0;
    }
    else{
      noDebrisDrives++;
    }
  }
  if(isDone(current)|| noDebrisDrives >= 3){
  int best = findBestRobot();
  if(best == -1){
    return SearchResult{false, Location{}};
  }

  if(isDone(current)|| signalStrength(best)>signalStrength(current)+2){
    current = best;
    mothership.switchToRobot(current);
  }
  noDebrisDrives = 0;
 }
}
}
