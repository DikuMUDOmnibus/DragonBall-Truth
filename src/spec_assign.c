
/* ************************************************************************
*   File: spec_assign.c                                 Part of CircleMUD *
*  Usage: Functions to assign function pointers to objs/mobs/rooms        *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "db.h"
#include "interpreter.h"
#include "utils.h"

extern int top_of_world;
extern int dts_are_dumps;
extern int mini_mud;
extern struct room_data *world;
extern struct index_data *mob_index;
extern struct index_data *obj_index;

SPECIAL(dump);
SPECIAL(gamble_slot_machine);
SPECIAL(pet_shops);
SPECIAL(gamble_craps);
SPECIAL(postmaster);
SPECIAL(cityguard);
SPECIAL(receptionist);
SPECIAL(cryogenicist);
SPECIAL(guild_guard);
SPECIAL(guild);
SPECIAL(puff);
SPECIAL(fido);
SPECIAL(janitor);
SPECIAL(mayor);
SPECIAL(snake);
SPECIAL(icer);
SPECIAL(Human);
SPECIAL(bank);
SPECIAL(gen_board);
SPECIAL(meta_physician);
SPECIAL(pop_dispenser);
void assign_kings_castle(void);

/* local functions */
void assign_mobiles(void);
void assign_objects(void);
void assign_rooms(void);
void ASSIGNROOM(int room, SPECIAL(fname));
void ASSIGNMOB(int mob, SPECIAL(fname));
void ASSIGNOBJ(int obj, SPECIAL(fname));

/* functions to perform assignments */

void ASSIGNMOB(int mob, SPECIAL(fname))
{
  int rnum;

  if ((rnum = real_mobile(mob)) >= 0)
    mob_index[rnum].func = fname;
  else if (!mini_mud)
    log("SYSERR: Attempt to assign spec to non-existant mob #%d", mob);
}

void ASSIGNOBJ(int obj, SPECIAL(fname))
{
  if (real_object(obj) >= 0)
    obj_index[real_object(obj)].func = fname;
  else if (!mini_mud)
    log("SYSERR: Attempt to assign spec to non-existant obj #%d", obj);
}

void ASSIGNROOM(int room, SPECIAL(fname))
{
  if (real_room(room) >= 0)
    world[real_room(room)].func = fname;
  else if (!mini_mud)
    log("SYSERR: Attempt to assign spec to non-existant room #%d", room);
}


/* ********************************************************************
*  Assignments                                                        *
******************************************************************** */

/* assign special procedures to mobiles */
void assign_mobiles(void)
{
  assign_kings_castle();

  ASSIGNMOB(1, puff);

  /* Immortal Zone */
  ASSIGNMOB(1200, receptionist);
  ASSIGNMOB(1201, postmaster);
  ASSIGNMOB(2050, gamble_craps);
  ASSIGNMOB(1202, janitor);

  /* Midgaard */
  ASSIGNMOB(3005, receptionist);
  ASSIGNMOB(3010, postmaster);
  ASSIGNMOB(3020, guild);
  ASSIGNMOB(3021, guild);
  ASSIGNMOB(3022, guild);
  ASSIGNMOB(3023, guild);
  ASSIGNMOB(3024, guild_guard);
  ASSIGNMOB(3025, guild_guard);
  ASSIGNMOB(3026, guild_guard);
  ASSIGNMOB(3027, guild_guard);
  ASSIGNMOB(3059, cityguard);
  ASSIGNMOB(3060, cityguard);
  ASSIGNMOB(3061, janitor);
  ASSIGNMOB(3062, fido);
  ASSIGNMOB(3066, fido);
  ASSIGNMOB(3067, cityguard);
  ASSIGNMOB(3068, janitor);
  ASSIGNMOB(3095, cryogenicist);
  ASSIGNMOB(3105, mayor);

  /* MORIA */
  ASSIGNMOB(4000, snake);
  ASSIGNMOB(4001, snake);
  ASSIGNMOB(4053, snake);
  ASSIGNMOB(4100, Human);
  ASSIGNMOB(4102, snake);
  ASSIGNMOB(4103, icer);

  /* Redferne's */
  ASSIGNMOB(7900, cityguard);

  /* PYRAMID */
  ASSIGNMOB(5300, snake);
  ASSIGNMOB(5301, snake);
  ASSIGNMOB(5304, icer);
  ASSIGNMOB(5305, icer);
  ASSIGNMOB(5309, Human); /* should breath fire */
  ASSIGNMOB(5311, Human);
  ASSIGNMOB(5313, Human); /* should be a Namek */
  ASSIGNMOB(5314, Human); /* should be a Namek */
  ASSIGNMOB(5315, Human); /* should be a Namek */
  ASSIGNMOB(5316, Human); /* should be a Namek */
  ASSIGNMOB(5317, Human);

  /* High Tower Of Sorcery */
  ASSIGNMOB(2501, Human); /* should likely be Namek */
  ASSIGNMOB(2504, Human);
  ASSIGNMOB(2507, Human);
  ASSIGNMOB(2508, Human);
  ASSIGNMOB(2510, Human);
  ASSIGNMOB(2511, icer);
  ASSIGNMOB(2514, Human);
  ASSIGNMOB(2515, Human);
  ASSIGNMOB(2516, Human);
  ASSIGNMOB(2517, Human);
  ASSIGNMOB(2518, Human);
  ASSIGNMOB(2520, Human);
  ASSIGNMOB(2521, Human);
  ASSIGNMOB(2522, Human);
  ASSIGNMOB(2523, Human);
  ASSIGNMOB(2524, Human);
  ASSIGNMOB(2525, Human);
  ASSIGNMOB(2526, Human);
  ASSIGNMOB(2527, Human);
  ASSIGNMOB(2528, Human);
  ASSIGNMOB(2529, Human);
  ASSIGNMOB(2530, Human);
  ASSIGNMOB(2531, Human);
  ASSIGNMOB(2532, Human);
  ASSIGNMOB(2533, Human);
  ASSIGNMOB(2534, Human);
  ASSIGNMOB(2536, Human);
  ASSIGNMOB(2537, Human);
  ASSIGNMOB(2538, Human);
  ASSIGNMOB(2540, Human);
  ASSIGNMOB(2541, Human);
  ASSIGNMOB(2548, Human);
  ASSIGNMOB(2549, Human);
  ASSIGNMOB(2552, Human);
  ASSIGNMOB(2553, Human);
  ASSIGNMOB(2554, Human);
  ASSIGNMOB(2556, Human);
  ASSIGNMOB(2557, Human);
  ASSIGNMOB(2559, Human);
  ASSIGNMOB(2560, Human);
  ASSIGNMOB(2562, Human);
  ASSIGNMOB(2564, Human);

  /* SEWERS */
  ASSIGNMOB(7006, snake);
  ASSIGNMOB(7009, Human);
  ASSIGNMOB(7200, Human);
  ASSIGNMOB(7201, Human);
  ASSIGNMOB(7202, Human);

  /* FOREST */
  ASSIGNMOB(6112, Human);
  ASSIGNMOB(6113, snake);
  ASSIGNMOB(6114, Human);
  ASSIGNMOB(6115, Human);
  ASSIGNMOB(6116, Human); /* should be a Namek */
  ASSIGNMOB(6117, Human);

  /* ARACHNOS */
  ASSIGNMOB(6302, Human);
  ASSIGNMOB(6309, Human);
  ASSIGNMOB(6312, Human);
  ASSIGNMOB(6314, Human);
  ASSIGNMOB(6315, Human);

  /* Desert */
  ASSIGNMOB(5004, Human);
  ASSIGNMOB(5005, guild_guard); /* brass dragon */
  ASSIGNMOB(5010, Human);
  ASSIGNMOB(5014, Human);

  /* Drow City */
  ASSIGNMOB(5103, Human);
  ASSIGNMOB(5104, Human);
  ASSIGNMOB(5107, Human);
  ASSIGNMOB(5108, Human);

  /* Old Thalos */
  ASSIGNMOB(5200, Human);
  ASSIGNMOB(5201, Human);
  ASSIGNMOB(5209, Human);

  /* New Thalos */
/* 5481 - Namek (or Mage... but he IS a high priest... *shrug*) */
  ASSIGNMOB(5404, receptionist);
  ASSIGNMOB(5421, Human);
  ASSIGNMOB(5422, Human);
  ASSIGNMOB(5423, Human);
  ASSIGNMOB(5424, Human);
  ASSIGNMOB(5425, Human);
  ASSIGNMOB(5426, Human);
  ASSIGNMOB(5427, Human);
  ASSIGNMOB(5428, Human);
  ASSIGNMOB(5434, cityguard);
  ASSIGNMOB(5440, Human);
  ASSIGNMOB(5455, Human);
  ASSIGNMOB(5461, cityguard);
  ASSIGNMOB(5462, cityguard);
  ASSIGNMOB(5463, cityguard);
  ASSIGNMOB(5482, cityguard);

  /* ROME */
  ASSIGNMOB(12009, Human);
  ASSIGNMOB(12018, cityguard);
  ASSIGNMOB(12020, Human);
  ASSIGNMOB(12021, cityguard);
  ASSIGNMOB(12025, Human);
  ASSIGNMOB(12030, Human);
  ASSIGNMOB(12031, Human);
  ASSIGNMOB(12032, Human);

  /* King Welmar's castle (not covered in castle.c) */
  ASSIGNMOB(15015, icer);      /* Ergan... have a better idea? */
  ASSIGNMOB(15032, Human); /* Pit Fiend, have something better?  Use it */

  /* DWARVEN KINGDOM */
  ASSIGNMOB(6500, cityguard);
  ASSIGNMOB(6502, Human);
  ASSIGNMOB(6509, Human);
  ASSIGNMOB(6516, Human);


}

/* assign special procedures to objects */
void assign_objects(void)
{
  ASSIGNOBJ(3096, gen_board);	/* social board */
  ASSIGNOBJ(3097, gen_board);	/* freeze board */
  ASSIGNOBJ(3098, gen_board);	/* immortal board */
  ASSIGNOBJ(3099, gen_board);	/* mortal board */
  ASSIGNOBJ(252, gen_board);	/* coders board */
  ASSIGNOBJ(253, gen_board);	/* builders board */
  ASSIGNOBJ(254, gen_board);	/* enforcers board */
  ASSIGNOBJ(255, gen_board);    /* player relations board */
  ASSIGNOBJ(5892, gen_board);    /* player relations board */
  ASSIGNOBJ(5898, gen_board);
  ASSIGNOBJ(5899, gen_board);
  ASSIGNOBJ(5799, gen_board);
  ASSIGNOBJ(2010, gen_board);
  ASSIGNOBJ(16199, gen_board);

  ASSIGNOBJ(3034, bank);	/* atm */
  ASSIGNOBJ(3036, bank);	/* cashcard */
  ASSIGNOBJ(2050, gamble_slot_machine);
}



/* assign special procedures to rooms */
void assign_rooms(void)
{
  int i;

  ASSIGNROOM(3030, dump);
  ASSIGNROOM(3031, pet_shops);

  if (dts_are_dumps)
    for (i = 0; i < top_of_world; i++)
      if (ROOM_FLAGGED(i, ROOM_DEATH))
	world[i].func = dump;
}




