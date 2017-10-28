		#include <iostream>
#include "vehicle.h"
#include <iostream>
#include <math.h>
#include <map>
#include <string>
#include <iterator>




bool Vehicle::_check_collision(state_snapshot *snapshot, int s_previous, int s_now)
{
  int s = snapshot->s;
  int v = snapshot->v;

  int v_target = s_now - s_previous;

  if (s_previous < s)
  {
    if (s_now >= s)
        return true;
    else
        return false;
  }
  if (s_previous > s)
  {
    if (s_now <= s)
        return true;
    else
        return false;
  }

  if (s_previous == s)
  {
    if (v_target > v)
        return false;
    else
        return true;
  }
  cout << "VALUE Error in check collision!!" << endl;
  return true;

}


float Vehicle::calculate_cost (vector<state_snapshot> trajectory,
                              map<int, vector <vector<int> > >  predictions)
{
    trajectory_data_s trajectory_data;

    _get_helper_data(trajectory, predictions,trajectory_data);
    float cost=0.0;
    float tmp_c;

    //todo: Build up funciton pointer list.
    tmp_c = _distance_from_goal_lane_cost(trajectory,predictions,trajectory_data);
    cout << "Cost:_distance_from_goal_lane_cost from goal:" << tmp_c<<endl;
    cost += tmp_c;
    
    tmp_c = _inefficiency_cost(trajectory,predictions,trajectory_data);
    cout << "Cost:_inefficiency_cost:" << tmp_c<<endl;
    cost += tmp_c;
    
    tmp_c = _collision_cost(trajectory,predictions,trajectory_data);
    cout << "Cost:_collision_cost:" << tmp_c<<endl;
    cost += tmp_c;
    
    tmp_c = _buffer_cost(trajectory,predictions,trajectory_data);
    cout << "Cost:_buffer_cost:" << tmp_c<<endl;
    cost += tmp_c;
    
    tmp_c = _change_lane_cost(trajectory,predictions,trajectory_data);
    cout << "Cost:_change_lane_cost:" << tmp_c<<endl;
    cost += tmp_c;
    
    cout << "Total cost:" << cost <<endl;

    return cost;
}



float Vehicle::_change_lane_cost(vector<state_snapshot> trajectory,
                      map<int, vector <vector<int> > >  predictions,
                      trajectory_data_s data)
{


	//Penalizes lane changes AWAY from the goal lane and rewards
	//lane changes TOWARDS the goal lane.
	 int proposed_lanes = data.end_lanes_from_goal;
   int cur_lanes = trajectory[0].lane;
   float cost = 0.0;

   if (proposed_lanes > cur_lanes)
 		cost = COST_COMFORT;
 	 if (proposed_lanes < cur_lanes)
 		cost = -1 * COST_COMFORT;

 	 if (cost != 0)
 		 cout <<  "Cost for lane change is :" << cost << endl;

   return cost;

 }


float Vehicle::_distance_from_goal_lane_cost(vector<state_snapshot> trajectory,
                      map<int, vector <vector<int> > >  predictions,
                      trajectory_data_s data)
{
   float cost = 0.0;

    int distance = abs(data.end_distance_to_goal);

  if (distance < 1.0)
    distance = 1.0;

	float time_to_goal = float(distance) / data.avg_speed;
    int lanes = data.end_lanes_from_goal;
    float multiplier = float(5 * lanes / time_to_goal);
    cost = multiplier * COST_REACH_GOAL;

	return cost;

}

float Vehicle::_inefficiency_cost(vector<state_snapshot> trajectory,
                      map<int, vector <vector<int> > >  predictions,
                               trajectory_data_s data)
{

  float speed = data.avg_speed;
  float diff = (float)(target_speed) - speed;
  float pct = float(diff) / target_speed;
  float multiplier = pow(pct,2);
  return multiplier * COST_EFFICIENCY;

}

float Vehicle::_collision_cost(vector<state_snapshot> trajectory,
                      map<int, vector <vector<int> > >  predictions,
                             trajectory_data_s data)
{
  if (data.collides)
  {
		int time_til_collision = data.collides_at;
		float exponent = pow(float(time_til_collision) ,2);
		float mult = exp(-exponent);
		return mult * COST_COLLISION;
  }
	return 0;


}


float Vehicle::_buffer_cost(vector<state_snapshot> trajectory,
                      map<int, vector <vector<int> > >  predictions,
                          trajectory_data_s data)
{
    int closest = data.closest_approach;
  if (closest == 0)
    return 10 * COST_DANGER;

  float timesteps_away = (float)closest / data.avg_speed;
  if (timesteps_away > DESIRED_BUFFER)
      return 0.0;

  float multiplier = 1.0 - pow((timesteps_away / DESIRED_BUFFER),2);
  return multiplier * COST_DANGER;
}



// End of cost functions.

void Vehicle::_get_helper_data(vector<state_snapshot> trajectory,
                      map<int, vector <vector<int> > >  predictions,
                      trajectory_data_s &ret_data)
{

// todo : remove later.
  ret_data.proposed_lane = -1;
  ret_data.avg_speed = -1;
  ret_data.max_accel = -1.0;
  ret_data.rms_acceleration = -1;
  ret_data.closest_approach = -1;
  ret_data.end_distance_to_goal = -1;
  ret_data.end_lanes_from_goal = -1;
  ret_data.collides = false;
  ret_data.collides_at = 0;

  vector<state_snapshot> t = trajectory;

  state_snapshot current_snapshot = t[0];
  state_snapshot first = t[1];
  state_snapshot last = t[t.size()-1];

  ret_data.end_distance_to_goal = goal_s - last.s;
  ret_data.end_lanes_from_goal = abs( goal_lane - last.lane);
  float dt = (float) trajectory.size();
  ret_data.proposed_lane = first.lane;
  ret_data.avg_speed = (last.s - current_snapshot.s) / dt;


  std::vector<int> accels;
  ret_data.closest_approach = 999999;
  ret_data.collides = false;
  state_snapshot last_snap = trajectory[0];

  map<int, vector <vector<int> > >  filtered_predictions;
    _filter_predictions_by_lane(predictions, ret_data.proposed_lane,filtered_predictions);

    for ( int i=1; i < PLANNING_HORIZON+1;++i)
    {
      state_snapshot snapshot = t[i];
      accels.push_back(abs(snapshot.a));

      for(auto const& x : filtered_predictions)
      {
          vector<vector<int> > new_predictions;

          vector<vector<int> > v = x.second;
          std::vector<int> state = v[i];
          std::vector<int> last_state = v[i-1];
          bool vehicle_collides = _check_collision(&snapshot, last_state[PRED_S_INDEX], state[PRED_S_INDEX]);
          if (vehicle_collides)
          {
            ret_data.collides = true;
            ret_data.collides_at = i;
          }
          int dist = abs(state[PRED_S_INDEX] - snapshot.s );
          if ( dist < ret_data.closest_approach)
                  ret_data.closest_approach = dist;
      }
      last_snap = snapshot;
    }
    auto it = max_element(std::begin(accels),std::end(accels));
    ret_data.max_accel = *it;
    for (int i ; i < accels.size();++i)
    {
      accels[i] = pow(accels[i],2);
    }
    int sum_accels = std::accumulate(accels.begin(), accels.end(), 0);
    ret_data.rms_acceleration = float(sum_accels) / accels.size();

      //int proposed_lane;
      //float avg_speed;
      //float max_accel;
      //float rms_acceleration;
      //float closest_approach;
      //int end_distance_to_goal;
      //int end_lanes_from_goal;
      //bool collides;


    }



void Vehicle::_filter_predictions_by_lane(map<int, vector <vector<int> > >  predictions,
  int proposed_lane,
  map<int, vector <vector<int> > >  &filtered_predictions)
{
        //cout << "**************************************" << endl;
        for(auto const& x : predictions)
        {
            vector<vector<int> > new_predictions;

            vector<vector<int> > pred = x.second;
            if (pred[0][0] 	== proposed_lane && x.first != -1)
            {
                // Add to list
                filtered_predictions.insert(x);
            }
        }

    return;

}
