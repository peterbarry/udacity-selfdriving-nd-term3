# ----------
# User Instructions:
#
# Define a function, search() that returns a list
# in the form of [optimal path length, row, col]. For
# the grid shown below, your function should output
# [11, 4, 5].
#
# If there is no valid path from the start point
# to the goal, your function should return the string
# 'fail'
# ----------

# Grid format:
#   0 = Navigable space
#   1 = Occupied space

grid = [[0, 0, 1, 0, 0, 0],
        [0, 0, 1, 0, 0, 0],
        [0, 0, 0, 0, 1, 0],
        [0, 0, 1, 1, 1, 0],
        [0, 0, 0, 0, 1, 0]]
init = [0, 0]
goal = [len(grid)-1, len(grid[0])-1]
cost = 1

delta = [[-1, 0], # go up
         [ 0,-1], # go left
         [ 1, 0], # go down
         [ 0, 1]] # go right

delta_name = ['^', '<', 'v', '>']

def search(grid,init,goal,cost):
    # ----------------------------------------
    # insert code here
    # ----------------------------------------


    print 'example from videos :'
    closed = [[0 for row in range(len(grid[0]))] for col in range(len(grid))]
    closed[init[0]][init[1]]

    x=init[0]
    y=init[1]
    g = 0

    open = [[g,x,y]]
    found = False
    resign = False

    while found is False and resign is False:
        if len(open) == 0:
            resign = True
            print 'Fail - no search path.'
        else:
            open.sort()
            open.reverse()
            next = open.pop()
            x = next[1]
            y = next[2]
            g = next[0]

            if x == goal[0] and y == goal[1]:
                found = True
                print next
                print 'Found goal !!!! '
            else:
                for i in range(len(delta)):
                    x2 = x + delta[i][0]
                    y2 = y + delta[i][1]
                    if x2 >= 0 and x2 < len(grid) and y2 >= 0 and y2 < len(grid[1]) :
                        if closed[x2][y2] == 0 and grid[x2][y2] == 0:
                            g2 = g + cost
                            open.append([g2,x2,y2])
                            closed[x2][y2] = 1


    print 'Finished example from videos :'



    class grid_state_c:
        def __init__(self,initial_grid):
            #print 'init grid_state_c'
            self.grid = initial_grid
            self.grid_max_x = len(self.grid)-1 # num of elemnts should be <= this
            self.grid_max_y = len(self.grid[0])-1 # num of elemnts should be <= this
            #self.open = [[True for x in range(len(self.grid[0])-1)] for y in range(len(self.grid))]
            self.checked = [[False for x in range(len(self.grid[0]))] for y in range(len(self.grid))]
            #self.open_list = [[[] for x in range(len(self.grid[0])-1)] for y in range(len(self.grid))] #add tupple here x,y,g
            self.g_init=0

        def set_check(self,location):
                self.checked[location[0]][location[1]] = True
        def is_checked(self,location):
            return self.checked[location[0]][location[1]]

        def set_cost(self,cost):
                self.cost = cost

        def dump_state(self):
            print 'Grid'
            print self.grid
            #print 'open'
            #print self.open
            print 'checked'
            print self.checked
            #print 'open list'
            #print self.open_list
            print 'g_init'
            print self.g_init
            print 'grid_max_x'
            print self.grid_max_x
            print 'grid_max_y'
            print self.grid_max_y


        def gen_open_list_and_check(self,location):
            #Search around the location
            open_list = []
            for i in range(len(delta)):
                new_x = location[0] + delta[i][0]
                new_y = location[1] + delta[i][1]
                if new_x >= 0 and new_x <= self.grid_max_x and new_y >= 0 and new_y <= self.grid_max_y :
                    #ensure we are in range still
                    #now check if its an open space
                    #print 'new_x'
                    #print new_x
                    #print 'new_y'
                    #print new_y
                    if self.grid[new_x][new_y] == 0 :
                        # The slot is empty
                        if self.checked[new_x][new_y] is False :
                            open_list.append([new_x,new_y,location[2]+self.cost])
                            self.checked[new_x][new_y] = True

            #print 'Open Cells around this cell'
            #print 'location:'
            #print location
            #print 'around'
            #print open_list
            return open_list



    grid_state = grid_state_c(grid)
    grid_state.set_cost(cost)

    result = ''
    expansion_count = 0
    continue_search = True

    open_list = []
    current_node = init
    current_node.append(expansion_count) # open - x,y,expansion_count
    grid_state.set_check(current_node)

    print ' Search using second algorithm'

    while continue_search is True:
        #print 'current node'
        #print current_node
        if current_node[0] == goal[0] and current_node[1] == goal[1] :
            #print 'goal'
            tmp = [current_node[2],current_node[0],current_node[1]]
            print tmp
            #res = tmp

            continue_search = False
        else:
            open_list_tmp = grid_state.gen_open_list_and_check(current_node)
            #print 'open list tmp'
            #print open_list_tmp

            for elem in range(len(open_list_tmp)):
                open_list.append(open_list_tmp[elem])
            #print 'open list'
            #print open_list
            exp_count_list = [row[2] for row in open_list]
            if exp_count_list == []:
                print 'fail'
                result = 'fail'
                continue_search = False
            else:
                x = min(exp_count_list)
                index_min = exp_count_list.index(x)
                current_node = open_list[index_min]
                del open_list[index_min]
                #print 'open list with currnet node removed'
                #print open_list



        #print 'dumping state in search'
        #grid_state.dump_state()

    return result

search(grid,init,goal,cost)
