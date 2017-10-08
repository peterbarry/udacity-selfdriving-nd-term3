# -----------
# User Instructions:
#
# Modify the the search function so that it returns
# a shortest path as follows:
#
# [['>', 'v', ' ', ' ', ' ', ' '],
#  [' ', '>', '>', '>', '>', 'v'],
#  [' ', ' ', ' ', ' ', ' ', 'v'],
#  [' ', ' ', ' ', ' ', ' ', 'v'],
#  [' ', ' ', ' ', ' ', ' ', '*']]
#
# Where '>', '<', '^', and 'v' refer to right, left,
# up, and down motions. Note that the 'v' should be
# lowercase. '*' should mark the goal cell.
#
# You may assume that all test cases for this function
# will have a path from init to goal.
# ----------

grid = [[0, 0, 1, 0, 0, 0],
        [0, 0, 0, 0, 0, 0],
        [0, 0, 1, 0, 1, 0],
        [0, 0, 1, 0, 1, 0],
        [0, 0, 1, 0, 1, 0]]
init = [0, 0]
goal = [len(grid)-1, len(grid[0])-1]
cost = 1

delta = [[-1, 0 ], # go up
         [ 0, -1], # go left
         [ 1, 0 ], # go down
         [ 0, 1 ]] # go right

delta_name = ['^', '<', 'v', '>']

def search(grid,init,goal,cost):
    # ----------------------------------------
    # modify code below
    # ----------------------------------------
    closed = [[0 for row in range(len(grid[0]))] for col in range(len(grid))]
    closed[init[0]][init[1]] = 1

    from_grid = [[[-1,-1] for row in range(len(grid[0]))] for col in range(len(grid))]
    expand = [[' ' for row in range(len(grid[0]))] for col in range(len(grid))]



    x = init[0]
    y = init[1]
    g = 0
    from_grid[x][y]=[x,y] # Start element

    open = [[g, x, y]]


    found = False  # flag that is set when search is complete
    resign = False # flag set if we can't find expand

    while not found and not resign:
        if len(open) == 0:
            resign = True
            return 'fail'
        else:
            open.sort()
            open.reverse()
            next = open.pop()
            x = next[1]
            y = next[2]
            g = next[0]



            if x == goal[0] and y == goal[1]:
                found = True
            else:
                for i in range(len(delta)):
                    x2 = x + delta[i][0]
                    y2 = y + delta[i][1]
                    if x2 >= 0 and x2 < len(grid) and y2 >=0 and y2 < len(grid[0]):
                        if closed[x2][y2] == 0 and grid[x2][y2] == 0:
                            g2 = g + cost
                            open.append([g2, x2, y2])
                            closed[x2][y2] = 1
                            from_grid[x2][y2] = [x,y]




    # met our goal
    expand[goal[0]][goal[1]] = '*'
    current_node = [goal[0],goal[1]]

    not_at_start = True
    while not_at_start is True:
        prev_node = from_grid[current_node[0]][current_node[1]]
        for dir in range(len(delta)):
            dirx = prev_node[0] + delta[dir][0]
            diry = prev_node[1] + delta[dir][1]
            if dirx == current_node[0] and diry == current_node[1]:
                #match the directions
                expand[prev_node[0]][prev_node[1]]=delta_name[dir]

        current_node = prev_node
        if current_node == init:
            not_at_start = False

    return expand # make sure you return the shortest path

expanded_list = search(grid,init,goal,cost)

print 'Expanded'
for pindex in range(len(expanded_list)):
    print expanded_list[pindex]
