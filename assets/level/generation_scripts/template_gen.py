import level

#Note
note = "generated by template_gen.py"

#Settings
title = "Template"
width = 1280
height = 720
mirrored = False

#Init
lev = level.Level(title, width, height, mirrored)

#Floor
lev.add_obstacle(-20, width + 20, height - 20, height + 20)
#Ceiling
lev.add_obstacle(-20, width + 20, -20, 20)
#Walls
lev.add_obstacle(-20, 20, -20, height + 20, mirror = True)


#Weapons
lev.add_box_spawn(width/2, height/2, ["rocket", "flame", "pierce", "bounce"])

#Spawns
for i in range(2):
    lev.add_player_spawn(width/2 + (1 if i > 0 else -1) * width/3, height - 150)
    lev.add_player_spawn(width/2 + (1 if i > 0 else -1) * width/3, 150)

lev.save("../"+title+".json", note = note)