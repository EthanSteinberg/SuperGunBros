import level

def build_hotel(lev):

    width = lev.width
    height = lev.height

    hotel_width = 800
    hotel_elevator_width = 120
    hotel_num_floors = 8
    hotel_floor_thickness = 60
    hotel_wall_thickness = 60
    hotel_ceiling_height = 250
    hotel_door_height = 140
    hotel_balcony_thickness = 30
    hotel_balcony_length = 150
    hotel_balcony_rail = 30
    hotel_roof_location = hotel_ceiling_height

    hotel_weapons = [["pierce"], None, ["rocket"], None, ["bounce"], None, ["flame"], None]

    for i in range(hotel_num_floors):

        floor_y = hotel_roof_location + i * (hotel_floor_thickness + hotel_ceiling_height)
        lobby = i == hotel_num_floors - 1
        balcony = 0 < i  < 4
        window = i < 3


        #Floor
        lev.add_obstacle(width - 20 - hotel_width,
                         width - 20 - hotel_elevator_width,
                         floor_y,
                         floor_y + hotel_floor_thickness)

        #Elevator Wall
        lev.add_obstacle(width - 20 - hotel_elevator_width - hotel_wall_thickness,
                         width - 20 - hotel_elevator_width,
                         floor_y + hotel_floor_thickness - 20,
                         height - 20 - 1.5 * hotel_door_height if lobby
                         else floor_y + hotel_floor_thickness + hotel_ceiling_height - hotel_door_height)

        #Outside wall
        if window or lobby:
            lev.add_obstacle(width - 20 - hotel_width,
                             width - 20 - hotel_width + hotel_wall_thickness,
                             floor_y + hotel_floor_thickness - 20,
                             height - 20 - 2.0 * hotel_door_height if lobby
                             else floor_y + hotel_floor_thickness + hotel_ceiling_height - 1.5 * hotel_door_height)
        else:
            lev.add_obstacle(width - 20 - hotel_width,
                             width - 20 - hotel_width + hotel_wall_thickness,
                             floor_y + hotel_floor_thickness - 20,
                             floor_y + hotel_floor_thickness + hotel_ceiling_height + 20)
        #Balcony
        if balcony:
            lev.add_obstacle(width - hotel_width - hotel_balcony_length - 20,
                             width - hotel_width,
                             floor_y,
                             floor_y + hotel_balcony_thickness)

            # lev.add_obstacle(width - hotel_width - hotel_balcony_length - 20,
            #                  width - hotel_width - hotel_balcony_length - 20 + hotel_balcony_thickness/2,
            #                  floor_y - hotel_balcony_rail,
            #                  floor_y + hotel_balcony_thickness)




        #Weapon
        if hotel_weapons[i] is not None:
            lev.add_box_spawn(width - 20 - hotel_elevator_width - (hotel_width - hotel_elevator_width)/2,
                              floor_y + hotel_ceiling_height,
                              hotel_weapons[i])