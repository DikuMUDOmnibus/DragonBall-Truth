/*** BEGIN VEHICLE.C ***/

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "screen.h"
#include "house.h"

extern struct room_data *world;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct index_data *obj_index;
extern int rev_dir[];
extern char *dirs[];
extern char *room_bits[];

struct obj_data *find_vehicle(int roomNum);

typedef struct {

int dirNum;

char* dirCmd;
} dirParseStruct;

#define DRIVE_DIRS 4
#define PILOT_DIRS 6

#ifndef EXITN
#  define EXITN(room, door)

(world[room].dir_option[door])
#endif

struct obj_data *find_vehicle_by_vnum(int vnum) {

extern struct obj_data * object_list;

struct obj_data * i;

for (i = object_list; i; i = i->next)


if (GET_OBJ_TYPE(i) == ITEM_VEHICLE)



if (GET_OBJ_VNUM(i) == vnum)




return i;

return 0;
}


void auto_exits_by_rnum(struct char_data * ch, int is_in)
{
  int door;

  *buf = '\0';

  for (door = 0; door < NUM_OF_DIRS; door++)
    if (EXITN(is_in, door) && EXITN(is_in, door)->to_room != NOWHERE &&

!IS_SET(EXITN(is_in, door)->exit_info, EX_CLOSED))
      sprintf(buf, "%s%c ", buf, LOWER(*dirs[door]));

  sprintf(buf2, "%s[ Exits: %s]%s\r\n", CCCYN(ch, C_NRM),

  *buf ? buf : "None! ", CCNRM(ch, C_NRM));

  send_to_char(buf2, ch);
}


void view_room_by_rnum(struct char_data * ch, int is_in) {

if (IS_DARK(is_in) && !CAN_SEE_IN_DARK(ch)) {


send_to_char("It is pitch black...\r\n", ch);



return;

}

send_to_char(CCCYN(ch, C_NRM), ch);

if (PRF_FLAGGED(ch, PRF_ROOMFLAGS)) {


sprintbit((long) ROOM_FLAGS(is_in), room_bits, buf);


sprintf(buf2, "[%5d] %s [ %s]", world[is_in].number,


world[is_in].name, buf);


send_to_char(buf2, ch);

} else


send_to_char(world[is_in].name, ch);




send_to_char(CCNRM(ch, C_NRM), ch);

send_to_char("\r\n", ch);


send_to_char(world[is_in].description, ch);


/* autoexits */

if (PRF_FLAGGED(ch, PRF_AUTOEXIT))


auto_exits_by_rnum(ch, is_in);


/* now list characters & objects */

send_to_char(CCGRN(ch, C_NRM), ch);

list_obj_to_char(world[is_in].contents, ch, 0, FALSE);

send_to_char(CCYEL(ch, C_NRM), ch);

list_char_to_char(world[is_in].people, ch);

send_to_char(CCNRM(ch, C_NRM), ch);
}


ACMD(do_drive) {

int x, dir;

const dirParseStruct dirParse[6] = {


{SCMD_NORTH - 1, "north"},


{SCMD_SOUTH - 1, "south"},


{SCMD_EAST - 1,  "east"},


{SCMD_WEST - 1,  "west"},


{SCMD_UP - 1,    "up"},


{SCMD_DOWN - 1,  "down"}

};

struct obj_data *vehicle, *controls, *vehicle_in_out;


controls = get_obj_in_list_type(ITEM_V_CONTROLS,
world[ch->in_room].contents);

if (!controls) {


send_to_char("ERROR!  Vehicle controls present yet not present!\r\n",
ch);


return;

}

vehicle = find_vehicle_by_vnum(GET_OBJ_VAL(controls, 0));

if (!vehicle) {


send_to_char("ERROR!  Vehicle has been lost somehow!\r\n", ch);


return;

}



if (IS_AFFECTED(ch, AFF_BLIND)) {


/* Blind characters can't drive! */


send_to_char("You can't see the controls!\r\n", ch);


return;

}



two_arguments(argument, arg, buf2);


/* Gotta give us a direction... */

if (!*arg) {


send_to_char("Drive which direction?\r\n", ch);


return;

}



/* Driving Into another Vehicle */

if (is_abbrev(arg, "into")) {


int was_in, is_in, is_going_to;





if (!*buf2) {



send_to_char("Drive into what?\r\n", ch);



return;


}





vehicle_in_out = get_obj_in_list_vis(ch, buf2,
world[vehicle->in_room].contents);


if (!vehicle_in_out) {



send_to_char("Nothing here by that name!\r\n", ch);



return;


}





if (GET_OBJ_TYPE(vehicle_in_out) != ITEM_VEHICLE) {



send_to_char("Thats not a vehicle.\r\n", ch);



return;


}





is_going_to = real_room(GET_OBJ_VAL(vehicle_in_out, 0));





if (!IS_SET(ROOM_FLAGS(is_going_to), ROOM_VEHICLE)) {



send_to_char("That vehicle can't carry other vehicles.", ch);



return;


}



sprintf(buf, "%s enters %s.\n\r", vehicle->short_description,
vehicle_in_out->short_description);


send_to_room(buf, vehicle->in_room);



was_in = vehicle->in_room;


obj_from_room(vehicle);


obj_to_room(vehicle, is_going_to);







is_in = vehicle->in_room;





if (ch->desc != NULL)



view_room_by_rnum(ch, is_in);








sprintf(buf, "%s enters.\r\n", vehicle->short_description);


send_to_room(buf, is_in);



return;

} else if (is_abbrev(arg, "out")) {


struct obj_data *hatch;





hatch = get_obj_in_list_type(ITEM_V_HATCH,
world[vehicle->in_room].contents);


if (!hatch) {



send_to_char("Nowhere to drive out of.\r\n", ch);



return;


}





vehicle_in_out = find_vehicle_by_vnum(GET_OBJ_VAL(hatch, 0));


if (!vehicle_in_out) {



send_to_char("ERROR!  Vehicle to drive out of doesn't exist!\r\n", ch);



return;


}





sprintf(buf, "%s exits %s.\r\n", vehicle->short_description,




vehicle_in_out->short_description);


send_to_room(buf, vehicle->in_room);





obj_from_room(vehicle);


obj_to_room(vehicle, vehicle_in_out->in_room);





if (ch->desc != NULL)



view_room_by_rnum(ch, vehicle->in_room);





sprintf(buf, "%s drives out of %s.\r\n", vehicle->short_description,




vehicle_in_out->short_description);


send_to_room(buf, vehicle->in_room);





return;

} else for(x = 0; x < (GET_OBJ_VAL(vehicle, 1) ? PILOT_DIRS :
DRIVE_DIRS); x++)

/* Drive in a direction... */


if (is_abbrev(arg, dirParse[x].dirCmd)) {



dir = dirParse[x].dirNum;



/* Ok we found the direction! */



if (ch == NULL || dir < 0 || dir >= NUM_OF_DIRS)




/* But something is invalid */




return;



else if (!EXIT(vehicle, dir) || EXIT(vehicle, dir)->to_room ==
NOWHERE) {




/* But there is no exit that way */




send_to_char("Alas, you cannot go that way...\r\n", ch);




return;



} else if (IS_SET(EXIT(vehicle, dir)->exit_info, EX_CLOSED)) {




/* But the door is closed */




if (EXIT(vehicle, dir)->keyword) {





sprintf(buf2, "The %s seems to be closed.\r\n",







fname(EXIT(vehicle, dir)->keyword));





send_to_char(buf2, ch);




} else





send_to_char("It seems to be closed.\r\n", ch);




return;



} else if (!IS_SET(ROOM_FLAGS(EXIT(vehicle, dir)->to_room),
ROOM_VEHICLE)) {




/* But the vehicle can't go that way*/




send_to_char("The vehicle can't manage that terrain.\r\n", ch);




return;



} else {




/* But nothing!  Let's go that way! */




int was_in, is_in;









sprintf(buf, "%s leaves %s.\n\r", vehicle->short_description,
dirs[dir]);




send_to_room(buf, vehicle->in_room);





was_in = vehicle->in_room;




obj_from_room(vehicle);




obj_to_room(vehicle, world[was_in].dir_option[dir]->to_room);









is_in = vehicle->in_room;










if (ch->desc != NULL)





view_room_by_rnum(ch, is_in);










sprintf(buf, "%s enters from the %s.\r\n",






vehicle->short_description, dirs[rev_dir[dir]]);




send_to_room(buf, is_in);





return;



}


}

send_to_char("Thats not a valid direction.\r\n", ch);

return;
}


SPECIAL(vehicle) {

struct obj_data * obj;

if (CMD_IS("enter")) {


one_argument(argument, arg);





if (!*arg) {



send_to_char("Enter what?\r\n", ch);



return 1;


}





obj = get_obj_in_list_vis(ch, arg, world[ch->in_room].contents);





if (!obj) {



send_to_char("Nothing by that name is here to enter!\r\n", ch);



return 1;


}





act("You climb into $o.", TRUE, ch, obj, 0, TO_CHAR);


act("$n climbs into $o", TRUE, ch, obj, 0, TO_ROOM);


char_from_room(ch);


char_to_room(ch, real_room(GET_OBJ_VAL(obj, 0)));


act("$n climbs in.", TRUE, ch, 0, 0, TO_ROOM);


do_look(ch, "", "", 0);


return 1;

}

return 0;
}


SPECIAL(vehicle_controls) {

if (CMD_IS("drive") || CMD_IS("pilot")) {


do_drive(ch, argument, cmd, 0);


return 1;

}

return 0;
}


SPECIAL(vehicle_hatch) {

struct obj_data * hatch, * vehicle;

if (CMD_IS("leave")) {


hatch = get_obj_in_list_type(ITEM_V_HATCH, world[ch->in_room].contents);


if (!hatch) {



send_to_char("ERROR!  Hatch is there, yet it isn't!\r\n", ch);



return 1;


}


vehicle = find_vehicle_by_vnum(GET_OBJ_VAL(hatch, 0));


if (!vehicle) {



send_to_char("ERROR!  Vehicle has been lost somehow!\r\n", ch);



return 1;


}


act("$n leaves $o", TRUE, ch, vehicle, 0, TO_ROOM);


act("You climb out of $o.", TRUE, ch, vehicle, 0, TO_CHAR);


char_from_room(ch);


char_to_room(ch, vehicle->in_room);


act("$n climbs out of $o", TRUE, ch, vehicle, 0, TO_ROOM);





/* Now show them the room */


do_look(ch, "", "", 0);





return 1;

}

return 0;
}


SPECIAL(vehicle_window) {

struct obj_data * viewport, * vehicle;

if (CMD_IS("look")) {


one_argument(argument, arg);


if (is_abbrev(arg, "out")) {



viewport = get_obj_in_list_type(ITEM_V_WINDOW,
world[ch->in_room].contents);



if (!viewport) {




send_to_char("ERROR!  Viewport present, yet not present!\r\n", ch);




return 1;



}



vehicle = find_vehicle_by_vnum(GET_OBJ_VAL(viewport, 0));



if (!vehicle) {




send_to_char("ERROR!  Vehicle has been lost somehow!\r\n", ch);




return 1;



}



view_room_by_rnum(ch, vehicle->in_room);



return 1;


}

}

return 0;
}

/* assign special procedures to vehicular objects */
void assign_vehicles(void) {

SPECIAL(vehicle_controls);

SPECIAL(vehicle_window);

SPECIAL(vehicle_hatch);

SPECIAL(vehicle);



/* Assign the generic tank! */

ASSIGNOBJ(200, vehicle);

ASSIGNOBJ(201, vehicle_controls);

ASSIGNOBJ(202, vehicle_hatch);

ASSIGNOBJ(203, vehicle_window);
}

/*** END VEHICLES.C ***/

