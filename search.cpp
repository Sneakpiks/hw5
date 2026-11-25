/*
 * STUDENT CODE - EDIT THIS FILE
 */

#include "search.h"

#include <algorithm>

const int BUST = 1000;

Search::Search(Mothership &mothership)
    : mothership(mothership), roboCount(mothership.getRobotCount()), nextRobot(0)
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

  const std::vector<int> &list = debris.at(robot);
  std::vector<int>::const_iterator low = std::lower_bound(list.begin(), list.end(), position - SIGNAL_RANGE);
  std::vector<int>::const_iterator high = std::upper_bound(list.begin(), list.end(), position + SIGNAL_RANGE);

  return static_cast<int>(high-low);
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

int Search::findBestRobot(int current){
  int best = -1;
  int bestSignal = 0;
  int offset[3] = {0,-1,1};



  for(int i = 0; i < 3; ++i){
    int robot = (current + offset[i] + roboCount)%roboCount;
    if(isDone(robot)){
      continue;
    }
    int signal = signalStrength(robot);
    if (signal>bestSignal){
      best = robot;
      bestSignal = signal;
    }
  }

  if(best != -1){
    return best;
  }

  for(int tries = 0; tries < roboCount; ++tries){
    nextRobot = (nextRobot+1) % roboCount;
    if(!isDone(nextRobot)){
      return nextRobot;
    }
  }

  return -1;
  
}

SearchResult Search::findShipwreck()
{
 int current = mothership.getActiveRobot();
 
 while(true){
  int last = getLastSegment(current);
  int turnStart = mothership.getRobotPosition(current);

  while(mothership.getRobotPosition(current)<last && mothership.getRobotPosition(current)-turnStart<BUST){
    int safeDistance = last-mothership.getRobotPosition(current);
    DriveResult result = mothership.driveActiveRobot(std::min(MAX_SEGMENTS_PER_DRIVE,safeDistance));

    if(result.reason == DriveStopReason::ShipwreckFound){
      return SearchResult{true, Location{current, mothership.getRobotPosition(current)}};
    }

    if(result.reason == DriveStopReason::DebrisFound){
      addDebris(current,mothership.getRobotPosition(current));
    }
  }

  int best = findBestRobot(current);
  if(best == -1){
    return SearchResult{false, Location{}};
  }

  if(best != current){
    current = best;
    mothership.switchToRobot(current);
  }
 }
}
