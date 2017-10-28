#include <iostream>
#include "vehicle.h"
#include <iostream>
#include <math.h>
#include <map>
#include <string>
#include <iterator>

//
// Series of cost functons for vehicle.
//

enum TrajectoryData  {
  proposed_lane,
  avg_speed,
  max_acceleration,
  rms_acceleration,
  closest_approach,
  end_distance_to_goal,
  end_lanes_from_goal,
  collides
};

typedef struct trajectory_data_s {
  int proposed_lane;
  float avg_speed;
  int max_accel;
  float rms_acceleration;
  int closest_approach;
  int end_distance_to_goal;
  int end_lanes_from_goal;
  bool collides;
  int collides_at;
}trajectory_data_s;



#define COST_COLLISION   (1000000) //( 10 ^ 6)
#define COST_DANGER      (100000)
#define COST_REACH_GOAL  (100000)
#define COST_COMFORT     (1000)
#define COST_EFFICIENCY  (100)

#define DESIRED_BUFFER (1.5)
#define PLANNING_HORIZON (2)
#define COST_DEBUG TRUE
