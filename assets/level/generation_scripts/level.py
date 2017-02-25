import json
import io
from collections import OrderedDict

class Level():

    def __init__(self, title, width, height, mirrored = False):
        self.dict = OrderedDict([
            ("title", title),
            ("width", width),
            ("height" , height),
            ("mirrored" , mirrored),
            ("obstacles" , []),
            ("boxSpawnLocations" , []),
            ("spawnLocations", [])
        ])

    def add_obstacle(self, xl, xr, yt, yb, mir=None):
        obst = {"xLeft": xl, "xRight": xr, "yTop": yt, "yBottom": yb}
        if mir is not None:
            obst["mirrored"] = mir
        self.dict["obstacles"].append(obst)

    def add_box_spawn(self, x, y, weapons, isd=15, rd=30, mir=None):
        box = {"x" : x, "y": y, "weapons" : weapons, "initialSpawnDelay": isd, "respawnDelay": rd}
        if mir is not None:
            box["mirrored"] = mir
        self.dict["boxSpawnLocations"].append(box)

    def add_player_spawn(self, x, y, mir=None):
        spawn = {"x" : x, "y": y}
        if mir is not None:
            spawn["mirrored"] = mir
        self.dict["spawnLocations"].append(spawn)

    def save(self, filename, note = None):
        with io.open(filename, 'w', encoding='utf-8') as f:
            if note:
                f.write(unicode("//" + note + "\n"))
            f.write(unicode(json.dumps(self.dict, indent = 4)))