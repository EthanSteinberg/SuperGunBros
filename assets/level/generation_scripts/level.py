import json
import io
from collections import OrderedDict

class Level():

    def __init__(self, title, width, height, mirrored = False):
        self.width = width
        self.height = height

        self.dict = OrderedDict([
            ("title", title),
            ("width", width),
            ("height" , height),
            ("mirrored" , mirrored),
            ("obstacles" , []),
            ("boxSpawnLocations" , []),
            ("spawnLocations", []),
            ("killboxes", [])
        ])

    def add_obstacle(self, xl, xr, yt, yb, mirror=None, pierceable=None):
        obst = {"xLeft": xl, "xRight": xr, "yTop": yt, "yBottom": yb}
        if mirror is not None:
            obst["mirrored"] = mirror
        if pierceable is not None:
            obst["pierceable"] = pierceable
        self.dict["obstacles"].append(obst)

    def add_box_spawn(self, x, y, weapons, isd=120, rd=600, mirror=None):
        box = {"x" : x, "y": y, "weapons" : weapons, "initialSpawnDelay": isd, "respawnDelay": rd}
        if mirror is not None:
            box["mirrored"] = mirror
        self.dict["boxSpawnLocations"].append(box)

    def add_player_spawn(self, x, y, mirror=None):
        spawn = {"x" : x, "y": y}
        if mirror is not None:
            spawn["mirrored"] = mirror
        self.dict["spawnLocations"].append(spawn)

    def add_kill_box(self, xl, xr, yt, yb, mirror=None):
        kb = {"xLeft": xl, "xRight": xr, "yTop": yt, "yBottom": yb}
        if mirror is not None:
            kb["mirrored"] = mirror
        self.dict["killboxes"].append(kb)

    def add_background(self, img, xl, xr, yt, yb):
        if "background" not in self.dict:
            self.dict["background"] = []
        bg = {"img" : img, "xLeft": xl, "xRight": xr, "yTop": yt, "yBottom": yb}
        self.dict["background"].append(bg)

    def save(self, filename, note = None):
        with io.open(filename, 'w', encoding='utf-8') as f:
            if note:
                f.write(unicode("//" + note + "\n"))
            f.write(unicode(json.dumps(self.dict, indent = 4)))
        print "Level successfully saved"
