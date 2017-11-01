# CarND-Path-Planning-Project
Self-Driving Car Engineer Nanodegree Program



### Goals
In this project your goal is to safely navigate around a virtual highway with other traffic that is driving +-10 MPH of the 50 MPH speed limit. You will be provided the car's localization and sensor fusion data, there is also a sparse map list of waypoints around the highway. The car should try to go as close as possible to the 50 MPH speed limit, which means passing slower traffic when possible, note that other cars will try to change lanes too. The car should avoid hitting other cars at all cost as well as driving inside of the marked road lanes at all times, unless going from one lane to another. The car should be able to make one complete loop around the 6946m highway. Since the car is trying to go 50 MPH, it should take a little over 5 minutes to complete 1 loop. Also the car should not experience total acceleration over 10 m/s^2 and jerk that is greater than 50 m/s^3.


### Project Solution Overview.

Credits: The project made extensive use of the overview session provided by Aaron Brown and David Silver. The project has made modifications in detection of vehicles prior to making a maneuver  and also some minor modifications for max acceleration.

#### Path Generation

 The path generator is responsible for producing the vehicle trajectory. The trajectory is calculated every time an event is received from the simulator. The Planner provides 50 x/y ahead every time it is called. The  path made up of (x,y) points that the car will visit sequentially every .02 seconds. When there is no vehicle in front of ego vehicle the default behavior is to track in lane and increase speed to max 50 ( target 49.5mph to avoid overshoot).

Step 1:
  Check the closest vehicle in this lane. This is done in fresnet coordinates. This is simply  done by iterating through all vehicle in the sensor data set and checking if the difference in s between ego vehicle and any vehicle in ego lane is less than 30m
  The code iterating  through the sensor fusion list is in main.cpp Line 298 to 315.

Step 2:
  If there is a vehicle too close in ego lane we must slow down or change lane if safe to do so. The model checks the adjacent lane(s) , and verifies the minimum gap in front of the vehicle and the minimum gap behind the vehicle in a potential target lane. This check is carried out in main.cpp Lines 340-365.
  Its important to note, the vehicle distance is a predicted value based on the velocity of the vehicles in the other lanes. It is not simply a snapshot of the current s values. This calculation  in in main.cpp line 351.

  If there is a safe gap in an adjacent lane and a vehicle is too close ahead we set the target lane to be the newly identified safe lane to change to. The updated future lane is set in main.cpp line 373. The my_lane value is used in the trajectory generation. All future trajectory points will be in using the target lane to generate the future fresent d values.

Step 3:
  If the vehicle is too close to the vehicle ahead, the model reduce the reference velocity. This is carried out in main.cpp line 386. The deceleration is limited by the max longitudinal jerk allowed. It starts off at 0.5 the maximum jerk, but if the gap to the vehicle does not reduce fast enough the deceleration rate is increased.

  If there is not a vehicle too close in lane we accelerate. The code is at main.cpp lines 388-396.


Step 4:

  The model provides 50 future (x,y) points to the simulator. However the simulator will only execute a small number of the trajectory points before generating a new telemetry event to the model. The event will indicate how much progress the car has made on along the previous submitted path.

  *     First , two (x/y) points that make up tangent to the  previously submitted trajectory are added to an array of x/y points. See main.ccp Line 422-436.
  *     Three Target x/y values are calculated  for 30,60 and 90 meters ahead. The d values use the map provided center road d values and updated based on the target lane -   Line 440-451. The future x/y values are updated based on map based translation.
  *    The x/y coordinates are rotated to align with a reference angle of zero degrees (makes math easier later)

  At this point we have 5 x/y points that follow on smoothly from the previous trajectory.

  We generate a spline using these 5 x/y values.

  The previous unused trajectory points are added to the new trajectory in main.cpp line 471-476.

  The remaining trajectory points are calculated  using the spline to ensure we submit 50points each time.

  The velocity of the ego vehicle is controlled  by the spacing of the x points in the trajectory, as each point represents 0.02sec.
  The distance between x points is calculated in main.cpp line 479-481 and line 488. The associated y point for each x point is calculated using the spline.

  The coordinates are rotated back by the ref_yaw (to reverse earlier transform)

Step 5:  
   The trajectory is ready for submission to the simulator.



##### Future work
 The model provide has not made extensive use of the class material in reality.

 The spine generated is not checked to see of it exceeds the jerk limits. It has been worked out empirically to avoid lateral jerk but is is not computationally guaranteed to not exceed the limit. The project should calculate the maximum rate of change on the lateral direction. If the jerk is exceed an alternative spline could be calculated or the maneuver avoided at that time.

 A number of potential trajectories should be generated and each with an associated cost function. Cost examples are level of jerk laterally and longitudinally.

 The project would benefit from using the behavioral planner in earlier classes. The cost function could include a benefit to picking a lane with overall less traffic ahead. In many cases the existing simplistic model will move left if its available, wheres moving right would be the better decision for forward progress.

## Original readme follows..
 ### Simulator.
 You can download the Term3 Simulator which contains the Path Planning Project from the [releases tab (https://github.com/udacity/self-driving-car-sim/releases).

#### The map of the highway is in data/highway_map.txt
Each waypoint in the list contains  [x,y,s,dx,dy] values. x and y are the waypoint's map coordinate position, the s value is the distance along the road to get to that waypoint in meters, the dx and dy values define the unit normal vector pointing outward of the highway loop.

The highway's waypoints loop around so the frenet s value, distance along the road, goes from 0 to 6945.554.

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./path_planning`.

Here is the data provided from the Simulator to the C++ Program

#### Main car's localization Data (No Noise)

["x"] The car's x position in map coordinates

["y"] The car's y position in map coordinates

["s"] The car's s position in frenet coordinates

["d"] The car's d position in frenet coordinates

["yaw"] The car's yaw angle in the map

["speed"] The car's speed in MPH

#### Previous path data given to the Planner

//Note: Return the previous list but with processed points removed, can be a nice tool to show how far along
the path has processed since last time.

["previous_path_x"] The previous list of x points previously given to the simulator

["previous_path_y"] The previous list of y points previously given to the simulator

#### Previous path's end s and d values

["end_path_s"] The previous list's last point's frenet s value

["end_path_d"] The previous list's last point's frenet d value

#### Sensor Fusion Data, a list of all other car's attributes on the same side of the road. (No Noise)

["sensor_fusion"] A 2d vector of cars and then that car's [car's unique ID, car's x position in map coordinates, car's y position in map coordinates, car's x velocity in m/s, car's y velocity in m/s, car's s position in frenet coordinates, car's d position in frenet coordinates.

## Details

1. The car uses a perfect controller and will visit every (x,y) point it recieves in the list every .02 seconds. The units for the (x,y) points are in meters and the spacing of the points determines the speed of the car. The vector going from a point to the next point in the list dictates the angle of the car. Acceleration both in the tangential and normal directions is measured along with the jerk, the rate of change of total Acceleration. The (x,y) point paths that the planner recieves should not have a total acceleration that goes over 10 m/s^2, also the jerk should not go over 50 m/s^3. (NOTE: As this is BETA, these requirements might change. Also currently jerk is over a .02 second interval, it would probably be better to average total acceleration over 1 second and measure jerk from that.

2. There will be some latency between the simulator running and the path planner returning a path, with optimized code usually its not very long maybe just 1-3 time steps. During this delay the simulator will continue using points that it was last given, because of this its a good idea to store the last points you have used so you can have a smooth transition. previous_path_x, and previous_path_y can be helpful for this transition since they show the last points given to the simulator controller with the processed points already removed. You would either return a path that extends this previous path or make sure to create a new path that has a smooth transition with this last path.

## Tips

A really helpful resource for doing this project and creating smooth trajectories was using http://kluge.in-chemnitz.de/opensource/spline/, the spline function is in a single hearder file is really easy to use.

---

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `install-mac.sh` or `install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets
    cd uWebSockets
    git checkout e94b6e1
    ```

## Editor Settings

We've purposefully kept editor configuration files out of this repo in order to
keep it as simple and environment agnostic as possible. However, we recommend
using the following settings:

* indent using spaces
* set tab width to 2 spaces (keeps the matrices in source code aligned)

## Code Style

Please (do your best to) stick to [Google's C++ style guide](https://google.github.io/styleguide/cppguide.html).

## Project Instructions and Rubric

Note: regardless of the changes you make, your project must be buildable using
cmake and make!


## Call for IDE Profiles Pull Requests

Help your fellow students!

We decided to create Makefiles with cmake to keep this project as platform
agnostic as possible. Similarly, we omitted IDE profiles in order to ensure
that students don't feel pressured to use one IDE or another.

However! I'd love to help people get up and running with their IDEs of choice.
If you've created a profile for an IDE that you think other students would
appreciate, we'd love to have you add the requisite profile files and
instructions to ide_profiles/. For example if you wanted to add a VS Code
profile, you'd add:

* /ide_profiles/vscode/.vscode
* /ide_profiles/vscode/README.md

The README should explain what the profile does, how to take advantage of it,
and how to install it.

Frankly, I've never been involved in a project with multiple IDE profiles
before. I believe the best way to handle this would be to keep them out of the
repo root to avoid clutter. My expectation is that most profiles will include
instructions to copy files to a new location to get picked up by the IDE, but
that's just a guess.

One last note here: regardless of the IDE used, every submitted project must
still be compilable with cmake and make./

## How to write a README
A well written README file can enhance your project and portfolio.  Develop your abilities to create professional README files by completing [this free course](https://www.udacity.com/course/writing-readmes--ud777).
