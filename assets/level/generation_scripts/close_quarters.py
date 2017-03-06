import level
import pre_builts

#Note
note = "generated by close_quarters.py"

#Settings
title = "Close Quarters"
width = 1440
height = 810
mirrored = False

#Init
lev = level.Level(title, width, height, mirrored)



def add_vertical_bar(xLeft, xRight, yBottom, mirrored = False):
    lev.add_obstacle(xLeft, xRight, yBottom, yBottom + 30, mirrored)


def add_horizontal_bar(xLeft, yBottom, yTop, mirrored = False):
    lev.add_obstacle(xLeft, xLeft + 30, yBottom, yTop, mirrored)
'''
Outer Bounds
'''
#Floor
lev.add_obstacle(-20, width + 20, height - 20, height + 20)
#Ceiling
lev.add_obstacle(-20, width + 20, -20, 20)
#Walls
lev.add_obstacle(-20, 20, -20, height + 20, mirror = True)


'''
Top portion
'''
add_vertical_bar(120, width - 120, 140)


add_vertical_bar(0, 400, 280, mirrored = True)


xBox = 200
wBox = 300
add_vertical_bar(xBox, xBox + wBox, 400, mirrored = True)

add_vertical_bar(xBox, xBox + wBox, 640, mirrored = True)

add_horizontal_bar(xBox, 400, 640 + 30, mirrored = True)

add_vertical_bar(570, width/2 + 30, 520, mirrored = True)


lev.add_box_spawn(width/2, 120, ["flame"])

#Spawns
for i in range(4):
    lev.add_player_spawn(width/2 + (1 if i%2 > 0 else -1) * (width/2 - 150), height - 150)
    #lev.add_player_spawn(width/2 + (1 if i > 0 else -1) * (width/2 - 150), 150)

print "Saving complex..."
lev.save("../close_quarters.json", note = note)