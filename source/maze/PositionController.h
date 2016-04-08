#ifndef PositionController_h
#define PositionController_h

#include "Maze.h"

class PositionController {
private:
  position_t pos;           /**< current position of the mouse */
  direction_e dir; 			/**< direction of the mouse */
  inline void setPosition(position_t posToSet){
	  pos = posToSet;
  }
  inline void setDirection(direction_e dirToSet){
	  dir = dirToSet;
  }

public:
  PositionController(int row, int col, direction_e dirTo);
  PositionController(struct position_t pos, direction_e dirTo);
  PositionController();
  virtual void turnRight();
  virtual void turnLeft();
  virtual int goForward();
  inline direction_e getCurrentDirection(){
    return dir;
  }
  inline position_t getCurrentPosition(){
    return pos;
  }
  position_t getNextPosition(direction_e dirTo);

  inline position_t getNextPosition(){
	  return PositionController::getNextPosition(dir);
  }

  bool isInGoal();

  bool operator==(PositionController& rVal)
  {
  	return (getCurrentPosition() == rVal.getCurrentPosition());
  }
};

#endif
