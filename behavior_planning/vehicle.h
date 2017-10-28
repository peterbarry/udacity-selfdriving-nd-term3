#ifndef VEHICLE_H
#define VEHICLE_H
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <math.h>
#include <vector>
#include <map>
#include <string>
#include <iterator>
#include "cost_functions.h"

using namespace std;

#define PRED_LANE_INDEX (0)
#define PRED_S_INDEX (1)

class Vehicle {
public:

  struct collider{

    bool collision ; // is there a collision?
    int  time; // time collision happens

  };


  int L = 1;

  int preferred_buffer = 6; // impacts "keep lane" behavior.

  int prediction_horizon=5;

  typedef struct state_snapshot {
    int lane;
    int s;
    int v;
    int a;
    string state;
  }state_snapshot;

  int lane;

  int s;

  int v;

  int a;

  int target_speed;

  int lanes_available;

  int max_acceleration;

  int goal_lane;

  int goal_s;

  string state;

  /**
  * Constructor
  */
  Vehicle(int lane, int s, int v, int a);

  /**
  * Destructor
  */
  virtual ~Vehicle();

  void update_state(map<int, vector <vector<int> > > predictions);

  string  _get_next_state( map<int, vector <vector<int> > > predictions);

  vector<state_snapshot>  _trajectory_for_state(string state,map<int, vector <vector<int> > >  predictions,int horizon);

  float calculate_cost (vector<state_snapshot> trajectory,
                                          map<int, vector <vector<int> > >  predictions);

  void _get_helper_data(vector<state_snapshot> trajectory,
                        map<int, vector <vector<int> > >  predictions,
                        trajectory_data_s &ret_data);
  bool _check_collision(state_snapshot *snapshot, int s_previous, int s_now);


  void _filter_predictions_by_lane(map<int, vector <vector<int> > >  predictions,
    int proposed_lane,
    map<int, vector <vector<int> > >  &filtered_predictions);


  void _get_state_snapshot( state_snapshot &state_s);
  void _set_state_snapshot( state_snapshot state_s);

  void configure(vector<int> road_data);

  string display();

  void increment(int dt);

  vector<int> state_at(int t);

  bool collides_with(Vehicle other, int at_time);

  collider will_collide_with(Vehicle other, int timesteps);

  void realize_state(map<int, vector < vector<int> > > predictions);

  void realize_constant_speed();

  int _max_accel_for_lane(map<int,vector<vector<int> > > predictions, int lane, int s);

  void realize_keep_lane(map<int, vector< vector<int> > > predictions);

  void realize_lane_change(map<int,vector< vector<int> > > predictions, string direction);

  void realize_prep_lane_change(map<int,vector< vector<int> > > predictions, string direction);

  vector<vector<int> > generate_predictions(int horizon);

  // All Cost funtions are here.

  float _distance_from_goal_lane_cost(vector<state_snapshot> trajectory,
                        map<int, vector <vector<int> > >  predictions,
                        trajectory_data_s data);

  float _inefficiency_cost(vector<state_snapshot> trajectory,
                        map<int, vector <vector<int> > >  predictions,
                        trajectory_data_s data);


  float _collision_cost(vector<state_snapshot> trajectory,
                        map<int, vector <vector<int> > >  predictions,
                        trajectory_data_s data);

  float _buffer_cost(vector<state_snapshot> trajectory,
                        map<int, vector <vector<int> > >  predictions,
                        trajectory_data_s data);

  float _change_lane_cost(vector<state_snapshot> trajectory,
                        map<int, vector <vector<int> > >  predictions,
                        trajectory_data_s data);


};

#endif
