
#include <iostream>
#include "vehicle.h"
#include <iostream>
#include <math.h>
#include <map>
#include <string>
#include <iterator>
#include <limits>

/**
 * Initializes Vehicle
 */
Vehicle::Vehicle(int lane, int s, int v, int a) {

    this->lane = lane;
    this->s = s;
    this->v = v;
    this->a = a;
    state = "CS";
    max_acceleration = -1;

}

void print_vec_str(const std::vector<string> &vec)
{
  for (const auto& i: vec)
    std::cout << i << ' ';
  std::cout << std::endl;
}
void print_vec_int(const std::vector<int> &vec)
{
  for (const auto& i: vec)
    std::cout << i << ' ';
  std::cout << std::endl;
}

Vehicle::~Vehicle() {}

// TODO - Implement this method.
void Vehicle::update_state(map<int,vector < vector<int> > > predictions) {
	/*
    Updates the "state" of the vehicle by assigning one of the
    following values to 'self.state':

    "KL" - Keep Lane
     - The vehicle will attempt to drive its target speed, unless there is
       traffic in front of it, in which case it will slow down.

    "LCL" or "LCR" - Lane Change Left / Right
     - The vehicle will IMMEDIATELY change lanes and then follow longitudinal
       behavior for the "KL" state in the new lane.

    "PLCL" or "PLCR" - Prepare for Lane Change Left / Right
     - The vehicle will find the nearest vehicle in the adjacent lane which is
       BEHIND itself and will adjust speed to try to get behind that vehicle.

    INPUTS
    - predictions
    A dictionary. The keys are ids of other vehicles and the values are arrays
    where each entry corresponds to the vehicle's predicted location at the
    corresponding timestep. The FIRST element in the array gives the vehicle's
    current position. Example (showing a car with id 3 moving at 2 m/s):

    {
      3 : [
        {"s" : 4, "lane": 0},
        {"s" : 6, "lane": 0},
        {"s" : 8, "lane": 0},
        {"s" : 10, "lane": 0},
      ]
    }

    */
    state = _get_next_state(predictions); // this is an example of how you change state.

}

string Vehicle::_get_next_state(map<int, vector <vector<int> > > predictions)
{
  vector<string> states = {"KL","LCL","LCR"};
  vector< vector <int> > states_cost;
  vector<struct state_snapshot> trajectory;
  int cost;
  float min_cost= std::numeric_limits<float>::max();
  string min_state = states[0];

  //print_vec_str(states);
  if ( lane == 0 ) {
    // remove left lane`
    states.erase( std::remove( states.begin(), states.end(), "LCR" ), states.end() );
    states.erase( std::remove( states.begin(), states.end(), "PLCR" ), states.end() );

  }
  if ( lane == lanes_available-1) {
    states.erase( std::remove( states.begin(), states.end(), "LCL" ), states.end() );
    states.erase( std::remove( states.begin(), states.end(), "PLCL" ), states.end() );

  }
  print_vec_str(states);

  for ( int state_id=0 ;state_id < states.size();++state_id)
  {

    map<int, vector <vector<int> > > predictions_copy = predictions;
    //cout << "Processing State:" << states[state_id] << endl;
    trajectory = _trajectory_for_state(states[state_id],predictions_copy,prediction_horizon);
    //cout << "Here.." << endl;

    cost = calculate_cost(trajectory, predictions);
    cout << "Cost:" << cost << endl;
    if ( cost <= min_cost)
    {
      min_cost = cost;
        min_state = states[state_id];
    }
  }

  cout << "Next State:" << min_state << endl;

  return(min_state);
}

vector< Vehicle::state_snapshot>  Vehicle::_trajectory_for_state(string  state_proposed,map<int, vector <vector<int> > >  predictions,int horizon)
{
  std::vector<state_snapshot> trajectory;
  state_snapshot snapshot;

  _get_state_snapshot(snapshot); // get current snapshot.

  // pretend to be in new propoed state
  state = state_proposed;
  trajectory.push_back(snapshot);

  for ( int i =0; i < horizon;++i)
  {
      _set_state_snapshot(snapshot);
      state = state_proposed;
      realize_state(predictions);
      if ( lane < 0 || lane >= lanes_available)
      {
        cout << "ERROR: Lanes out of bounds:" << lane << endl;
      }
      increment(1);

      state_snapshot update_snapshot;
      _get_state_snapshot(update_snapshot); // get current snapshot.
      trajectory.push_back(update_snapshot);

      //need to remove first prediction for each vehicle. c++ 11.
      for(auto const& x : predictions)
      {
        vector<vector<int> > new_predictions;

//        cout << "Processing vehicle:id " << x.first << endl;
//        cout << "Has :" << x.second.size() << "predictions" << endl;
//        cout << "Removing first element" << endl;

          bool skiped=false;
          for ( const auto& y : x.second)
          {
              //cout << y.size();
              if ( skiped ) // did we drop the first one.
                  new_predictions.push_back(y);
              else
                  skiped = true;
          }

          //for( vector<vector<int> > v : x.second )
          //std::cout << v[0] << ","<< v[1] "  " ;

        //new_predictions = x.second;

       //new_predictions.erase(x.second.begin());
          //predictions.erase(x.first);
          //predictions.swap ( std::pair<int, vector <vector<int> > >(x.first,
          //new_predictions) );

          predictions[x.first] = new_predictions;

          //cout << "Now Has :" << new_predictions.size() << "predictions" << endl;
      }

//      cout << "**************************************" << endl;
//      for(auto const& x : predictions)
//      {
//          vector<vector<int> > new_predictions;
//
//          cout << "Post Processed vehicle:id" << x.first << endl;
//          cout << "Has :" << x.second.size() << "predictions" << endl;
//      }



  }

  _set_state_snapshot(snapshot);
  return (trajectory);

}

void Vehicle::_get_state_snapshot(  state_snapshot &state_s)
{
  state_s.lane = lane;
  state_s.s = s;
  state_s.v = v;
  state_s.a = a;
  state_s.state = state;
}

void Vehicle::_set_state_snapshot( state_snapshot state_s)
{
  lane = state_s.lane;
  s = state_s.s;
  v = state_s.v;
  a = state_s.a;
  state = state_s.state;
}



//string _get_next_state(vector <vector<int> > > predictions);
//  return("KL");


// todo change from void.
//void  _trajectory_for_state(self,string state,vector <vector<int> > > predictions, int horizon)


void Vehicle::configure(vector<int> road_data) {
	/*
    Called by simulator before simulation begins. Sets various
    parameters which will impact the ego vehicle.
    */
    target_speed = road_data[0];
    lanes_available = road_data[1];
    goal_s = road_data[2];
    goal_lane = road_data[3];
    max_acceleration = road_data[4];
}

string Vehicle::display() {

	ostringstream oss;

	oss << "s:    " << this->s << "\n";
    oss << "lane: " << this->lane << "\n";
    oss << "v:    " << this->v << "\n";
    oss << "a:    " << this->a << "\n";

    return oss.str();
}

void Vehicle::increment(int dt = 1) {

	this->s += this->v * dt;
    this->v += this->a * dt;
}

vector<int> Vehicle::state_at(int t) {

	/*
    Predicts state of vehicle in t seconds (assuming constant acceleration)
    */
    int s = this->s + this->v * t + this->a * t * t / 2;
    int v = this->v + this->a * t;
    return {this->lane, s, v, this->a};
}

bool Vehicle::collides_with(Vehicle other, int at_time) {

	/*
    Simple collision detection.
    */
    vector<int> check1 = state_at(at_time);
    vector<int> check2 = other.state_at(at_time);
    return (check1[0] == check2[0]) && (abs(check1[1]-check2[1]) <= L);
}

Vehicle::collider Vehicle::will_collide_with(Vehicle other, int timesteps) {

	Vehicle::collider collider_temp;
	collider_temp.collision = false;
	collider_temp.time = -1;

	for (int t = 0; t < timesteps+1; t++)
	{
      	if( collides_with(other, t) )
      	{
			collider_temp.collision = true;
			collider_temp.time = t;
        	return collider_temp;
    	}
	}

	return collider_temp;
}

void Vehicle::realize_state(map<int,vector < vector<int> > > predictions) {

	/*
    Given a state, realize it by adjusting acceleration and lane.
    Note - lane changes happen instantaneously.
    */
    string state = this->state;
    if(state.compare("CS") == 0)
    {
    	realize_constant_speed();
    }
    else if(state.compare("KL") == 0)
    {
    	realize_keep_lane(predictions);
    }
    else if(state.compare("LCL") == 0)
    {
    	realize_lane_change(predictions, "L");
    }
    else if(state.compare("LCR") == 0)
    {
    	realize_lane_change(predictions, "R");
    }
    else if(state.compare("PLCL") == 0)
    {
    	realize_prep_lane_change(predictions, "L");
    }
    else if(state.compare("PLCR") == 0)
    {
    	realize_prep_lane_change(predictions, "R");
    }

}

void Vehicle::realize_constant_speed() {
	a = 0;
}

int Vehicle::_max_accel_for_lane(map<int,vector<vector<int> > > predictions, int lane, int s) {

	int delta_v_til_target = target_speed - v;
    int max_acc = min(max_acceleration, delta_v_til_target);

    map<int, vector<vector<int> > >::iterator it = predictions.begin();
    vector<vector<vector<int> > > in_front;
    while(it != predictions.end())
    {

    	int v_id = it->first;

        vector<vector<int> > v = it->second;

        if((v[0][0] == lane) && (v[0][1] > s))
        {
        	in_front.push_back(v);

        }
        it++;
    }

    if(in_front.size() > 0)
    {
    	int min_s = 1000;
    	vector<vector<int>> leading = {};
    	for(int i = 0; i < in_front.size(); i++)
    	{
    		if((in_front[i][0][1]-s) < min_s)
    		{
    			min_s = (in_front[i][0][1]-s);
    			leading = in_front[i];
    		}
    	}

    	int next_pos = leading[1][1];
    	int my_next = s + this->v;
    	int separation_next = next_pos - my_next;
    	int available_room = separation_next - preferred_buffer;
    	max_acc = min(max_acc, available_room);
    }

    return max_acc;

}

void Vehicle::realize_keep_lane(map<int,vector< vector<int> > > predictions) {
	this->a = _max_accel_for_lane(predictions, this->lane, this->s);
}

void Vehicle::realize_lane_change(map<int,vector< vector<int> > > predictions, string direction) {
	int delta = -1;
    if (direction.compare("L") == 0)
    {
    	delta = 1;
    }
    this->lane += delta;
    int lane = this->lane;
    int s = this->s;
    this->a = _max_accel_for_lane(predictions, lane, s);
}

void Vehicle::realize_prep_lane_change(map<int,vector<vector<int> > > predictions, string direction) {
	int delta = -1;
    if (direction.compare("L") == 0)
    {
    	delta = 1;
    }
    int lane = this->lane + delta;

    map<int, vector<vector<int> > >::iterator it = predictions.begin();
    vector<vector<vector<int> > > at_behind;
    while(it != predictions.end())
    {
    	int v_id = it->first;
        vector<vector<int> > v = it->second;

        if((v[0][0] == lane) && (v[0][1] <= this->s))
        {
        	at_behind.push_back(v);

        }
        it++;
    }
    if(at_behind.size() > 0)
    {

    	int max_s = -1000;
    	vector<vector<int> > nearest_behind = {};
    	for(int i = 0; i < at_behind.size(); i++)
    	{
    		if((at_behind[i][0][1]) > max_s)
    		{
    			max_s = at_behind[i][0][1];
    			nearest_behind = at_behind[i];
    		}
    	}
    	int target_vel = nearest_behind[1][1] - nearest_behind[0][1];
    	int delta_v = this->v - target_vel;
    	int delta_s = this->s - nearest_behind[0][1];
    	if(delta_v != 0)
    	{

    		int time = -2 * delta_s/delta_v;
    		int a;
    		if (time == 0)
    		{
    			a = this->a;
    		}
    		else
    		{
    			a = delta_v/time;
    		}
    		if(a > this->max_acceleration)
    		{
    			a = this->max_acceleration;
    		}
    		if(a < -this->max_acceleration)
    		{
    			a = -this->max_acceleration;
    		}
    		this->a = a;
    	}
    	else
    	{
    		int my_min_acc = max(-this->max_acceleration,-delta_s);
    		this->a = my_min_acc;
    	}

    }

}

vector<vector<int> > Vehicle::generate_predictions(int horizon = 10) {

	vector<vector<int> > predictions;
    for( int i = 0; i < horizon; i++)
    {
      vector<int> check1 = state_at(i);
      vector<int> lane_s = {check1[0], check1[1]};
      predictions.push_back(lane_s);
  	}
    return predictions;

}
