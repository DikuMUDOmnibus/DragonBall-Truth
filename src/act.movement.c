/* ************************************************************************
*   File: act.movement.c                                Part of CircleMUD *
*  Usage: movement commands, door handling, & sleep/rest/etc state        *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "house.h"
#include "constants.h"
#include "dg_scripts.h"

/* external vars  */
extern struct room_data *world;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct index_data *obj_index;
extern int top_of_world;
extern int max_regen_time;

/* external functs */
void add_follower(struct char_data *ch, struct char_data *leader);
int special(struct char_data *ch, int cmd, char *arg);
void death_cry(struct char_data *ch);
int find_eq_pos(struct char_data * ch, struct obj_data * obj, char *arg);

/* local functions */
int has_boat(struct char_data *ch);
int find_door(struct char_data *ch, const char *type, char *dir, const char *cmdname);
int has_key(struct char_data *ch, int key);
void do_doorcmd(struct char_data *ch, struct obj_data *obj, int door, int scmd);int ok_pick(struct char_data *ch, int keynum, int pickproof, int scmd);
ACMD(do_gen_door);
ACMD(do_enter);
ACMD(do_leave);
ACMD(do_stand);
ACMD(do_sit);
ACMD(do_rest);
ACMD(do_sleep);
ACMD(do_wake);
ACMD(do_push);
ACMD(do_follow);

/* simple function to determine if char can walk on water */
int has_boat(struct char_data *ch)
{
  struct obj_data *obj;
  int i;
/*
  if (ROOM_IDENTITY(ch->in_room) == DEAD_SEA)
    return 1;
*/

  /* non-wearable boats in inventory will do it */

  /* and any boat you're wearing will do it too */

  return 0;
}

  

/* do_simple_move assumes
 *    1. That there is no master and no followers.
 *    2. That the direction exists.
 *
 *   Returns :
 *   1 : If succes.
 *   0 : If fail
 */
int do_simple_move(struct char_data *ch, int dir, int need_specials_check)
{
    struct obj_data *obj, *next_obj;
    int was_in, need_movement;
    int MOVE = 0, EXP = 0;
    int is_altered = FALSE; 
    int num_levels = 0; 
  /*
   * Check for special routines (North is 1 in command list, but 0 here) Note
   * -- only check if following; this avoids 'double spec-proc' bug
   */
  if (need_specials_check && special(ch, dir + 1, "")) /* XXX: Evaluate NULL */
    return 0;

  /* MAJINed? */
  if (AFF_FLAGGED(ch, AFF_MAJIN) && ch->master && ch->in_room == ch->master->in_room) {
    send_to_char("You cannot leave your master.\r\n", ch);
    return 0;
  }

  if (PRF_FLAGGED(ch, PRF_CAMP)) {
    send_to_char("You cannot leave while camped.\r\n", ch);
    return 0;
  }
  
  if (AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You cannot leave with all this charged energy!\r\n", ch);
    return 0;
  }
  
  if (AFF_FLAGGED(ch, AFF_CHARGEM)) {
    send_to_char("You cannot leave with all this charged energy!\r\n", ch);
    return 0;
  }
  
  if (AFF_FLAGGED(ch, AFF_CHARGEH)) {
    send_to_char("You cannot leave with all this charged energy!\r\n", ch);
    return 0;
  }
    
  if (AFF_FLAGGED(ch, AFF_HALTED)) {
    send_to_char("You only paused the battle.\r\n", ch);
    return 0;
  }

  if (AFF_FLAGGED(ch, AFF_JOG) && GET_MANA(ch) <= GET_MAX_MANA(ch)/100) {
     send_to_char("Your too tired to jog.\r\n", ch);
     return 0;
  }
/* Missing legs? */
// if (!PLR_FLAGGED(ch, PLR_RLEG) && !PLR_FLAGGED(ch, PLR_LLEG) && !IS_NPC(ch)) {
if (!PLR_FLAGGED(ch, PLR_RLEG) && !PLR_FLAGGED(ch, PLR_LLEG) && !IS_NPC(ch)) {      
     send_to_char("You crawl slowly.\r\n", ch);      
     WAIT_STATE(ch, PULSE_VIOLENCE * 3);
}
 if (MOB_FLAGGED(ch, MOB_SENTINEL)) {      
    send_to_char("You can't move!\r\n", ch); 
}
  if (!PLR_FLAGGED(ch, PLR_RLEG) && PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You hop around on one leg.\r\n", ch);
}
  if (PLR_FLAGGED(ch, PLR_RLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You hop around on one leg.\r\n", ch);
     }
  if (AFF_FLAGGED(ch, AFF_SPAR)) {
    send_to_char("You cannot leave while sparing.\r\n", ch);
    return 0;
  }
  for (obj = world[ch->in_room].contents; obj; obj = next_obj) {
      next_obj = obj->next_content;
      if (CAN_SEE_OBJ(ch, obj)) {
        if (GET_OBJ_TYPE(obj) == ITEM_FLOOD && !IS_KANASSAN(ch)) {
          send_to_char("You can't move! The room is filled with raging waters!\r\n", ch);
          return;
         }
        }
      else {
       continue;
      }
  }
  if (AFF_FLAGGED(ch, AFF_JOG)) {
    send_to_char("You jog around a bit.\r\n", ch);
    EXP = number(GET_HIT(ch)/100, GET_HIT(ch)/100);
    MOVE = number(GET_MAX_MANA(ch)/100, GET_MAX_MANA(ch)/100);
    GET_EXP(ch) = GET_EXP(ch) + EXP;
    GET_MANA(ch) = GET_MANA(ch) - MOVE;
    if (number(1, 30) == 15) {
     GET_MAX_HIT(ch) += number(9, 20);
     send_to_char("&15Your bodily strength increased with that jog.&00\r\n", ch);
    }
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
  }
  if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000 && AFF_FLAGGED(ch, AFF_JOG)) {     
   GET_LEVEL(ch) += 1;                                                             
   GET_EXP(ch) = 1;                                                                
   num_levels++;                                                                   advance_level(ch);                                                              
   is_altered = TRUE;                                                            
   send_to_char("You seem to have grown surprisingly stronger from jogging!\r\n", ch);
}    
  
  /* if this room or the one we're going to needs a boat, check for one */
  if (((SECT(ch->in_room) == SECT_WATER_NOSWIM) && !IS_KANASSAN(ch)) || ((SECT(EXIT(ch, dir)->to_room) == SECT_WATER_NOSWIM) && !IS_KANASSAN(ch))) {
      send_to_char("&15You struggle for &09air&15! Some of your energy drains as you hold your breath.&00\r\n", ch);
      act("&14$n &15swims into the room&00\r\n", FALSE, ch, 0, 0, TO_ROOM);
   if (!IS_NPC(ch)) {
    if (GET_HIT(ch) < GET_MAX_HIT(ch)/200) {
       GET_HIT(ch) = 1;
      }
    else   
      GET_HIT(ch) -= GET_MAX_HIT(ch)/200;
    }
  }
  
  if ((SECT(EXIT(ch, dir)->to_room) == SECT_FLYING) && !IS_NPC(ch)) {
    if (GET_POS(ch) != POS_FLOATING) {
      send_to_char("You need to fly to go there.\r\n", ch);
      return 0;
    }
  }
  /* move points needed is avg. move loss for src and destination sect type */
  need_movement = (movement_loss[SECT(ch->in_room)] +
		   movement_loss[SECT(EXIT(ch, dir)->to_room)]) / 2;

  if (ROOM_FLAGGED(ch->in_room, ROOM_ATRIUM)) {
    if (!House_can_enter(ch, GET_ROOM_VNUM(EXIT(ch, dir)->to_room))) {
  if (GET_LEVEL(ch) < LVL_IMMORT) {
      send_to_char("That's private property -- no trespassing!\r\n", ch);
      return 0;
    }
  }
 }
  if (ROOM_FLAGGED(EXIT(ch, dir)->to_room, ROOM_TUNNEL) &&
      num_pc_in_room(&(world[EXIT(ch, dir)->to_room])) > 0) {
    send_to_char("There isn't enough room there for more than one person!\r\n", ch);
    return 0;
  }
  /* Mortals and low level gods cannot enter greater god rooms. */
  if (ROOM_FLAGGED(EXIT(ch, dir)->to_room, ROOM_GODROOM) &&
	GET_LEVEL(ch) < LVL_GRGOD) {
    send_to_char("You aren't godly enough to use that room!\r\n", ch);
    return 0;
  }
  if (GET_COND(ch, DRUNK) > 5) {
    switch (number(1, 10)) {
     case 1:
      act("&15You wobble around, slamming into a wall!", TRUE, ch, 0, 0, TO_CHAR);
      act("&15$n wobbles around, slamming into a wall!", TRUE, ch, 0, 0, TO_ROOM);
      if (GET_HIT(ch) >= GET_MAX_HIT(ch) / 20) {
       GET_HIT(ch) = GET_HIT(ch) - GET_MAX_HIT(ch) / 20;
      }
      return;
      break;
     case 2:
     case 3:
     case 4:
      break;
     case 5:
      act("&15You trip over a pink elephant and fall on your ass!", TRUE, ch, 0, 0, TO_CHAR);
      act("&15$n trips over a box and falls on their ass!", TRUE, ch, 0, 0, TO_ROOM);
      GET_POS(ch) = POS_SITTING;
      return;
      break;
     case 6:
     case 7:
     case 8:
     case 9:
      break;
     case 10:
      act("&15You bend over and puke up all your stomach's contents, and then pass out on the puke!", TRUE, ch, 0, 0, TO_CHAR);
      act("&15$n bends over and puke up all their stomach's contents, and then pass out on the puke!", TRUE, ch, 0, 0, TO_ROOM);
      if (GET_MANA(ch) >= GET_MAX_MANA(ch) / 20) {
       GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 20;
      }
      GET_POS(ch) = POS_SLEEPING;
      GET_COND(ch, DRUNK) -= 1;
      return;
      break;
     }
  }
  /* Now we know we're allow to go into the room. */
  if (GET_LEVEL(ch) < LVL_IMMORT && !IS_NPC(ch))
    GET_MOVE(ch) == GET_MOVE(ch);
  if (GET_POS(ch) == POS_FLOATING) {
    sprintf(buf2, "&14$n &15flies &00&07%s&00.", dirs[dir]);
    act(buf2, TRUE, ch, 0, 0, TO_ROOM);
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/1000 && !IS_ANDROID(ch) && !!IS_ANGEL(ch)) {
    send_to_char("You fall out of the sky, unable to support yourself enough to fly anymore.\r\n", ch); 
    act("&11$n &15falls out of the sky, unable to fly anymore!&00", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_STANDING;
    }
   if (IS_ANDROID(ch) || IS_ANGEL(ch)) {
     GET_MANA(ch) = GET_MANA(ch);
     }
   else if (!IS_NPC(ch) && !IS_ANDROID(ch) && !IS_ANGEL(ch)) {
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/1000;
    }
   }
  if (!AFF_FLAGGED(ch, AFF_SWIFTNESS) && GET_POS(ch) == POS_STANDING && (PLR_FLAGGED(ch, PLR_RLEG) || !PLR_FLAGGED(ch, PLR_LLEG))) {
    sprintf(buf2, "$n leaves %s.", dirs[dir]);
    act(buf2, TRUE, ch, 0, 0, TO_ROOM);
  }
  if (!PLR_FLAGGED(ch, PLR_RLEG) && !PLR_FLAGGED(ch, PLR_LLEG) && !IS_NPC(ch) && !AFF_FLAGGED(ch, AFF_SWIFTNESS)) {
    sprintf(buf2, "$n crawls slowly %s.", dirs[dir]);
    act(buf2, TRUE, ch, 0, 0, TO_ROOM);
  }

  /* see if an entry trigger disallows the move */
  if (!entry_mtrigger(ch))
    return 0;
  if (!enter_wtrigger(&world[EXIT(ch, dir)->to_room], ch, dir))
    return 0;

  was_in = ch->in_room;
  char_from_room(ch);
  char_to_room(ch, world[was_in].dir_option[dir]->to_room);
  if (GET_POS(ch) == POS_FLOATING && !IS_ANGEL(ch)) {
    char buf2[MAX_STRING_LENGTH];
    sprintf(buf2, "$n &14flies&15 in from &14%s%s.&00",
          ((dir == UP || dir == DOWN) ? "" : "the "),
          (dir == UP ? "below": dir == DOWN ? "above" : dirs[rev_dir[dir]]));
    act(buf2, TRUE, ch, 0, 0, TO_ROOM);
  }
  else if (GET_POS(ch) == POS_FLOATING && IS_ANGEL(ch)) {
    char buf2[MAX_STRING_LENGTH];
    sprintf(buf2, "$n &14soars&15 in from &14%s%s.&00",
          ((dir == UP || dir == DOWN) ? "" : "the "),
          (dir == UP ? "below": dir == DOWN ? "above" : dirs[rev_dir[dir]]));
    act(buf2, TRUE, ch, 0, 0, TO_ROOM);
  }
  else if (!AFF_FLAGGED(ch, AFF_SWIFTNESS) && GET_POS(ch) == POS_STANDING) {
    char buf2[MAX_STRING_LENGTH];
    sprintf(buf2, "$n &15arrives from &14%s%s.&00",
          ((dir == UP || dir == DOWN) ? "" : "the "),
          (dir == UP ? "below": dir == DOWN ? "above" : dirs[rev_dir[dir]]));
    act(buf2, TRUE, ch, 0, 0, TO_ROOM);
  }
  if (ch->desc != NULL)
    look_at_room(ch, 0);

  if (ROOM_FLAGGED(ch->in_room, ROOM_DEATH) && GET_LEVEL(ch) < LVL_IMMORT) {
    log_death_trap(ch);
    death_cry(ch);
    extract_char(ch, TRUE);
    return 0;
  }

  entry_memory_mtrigger(ch);
  if (!greet_mtrigger(ch, dir)) {
    char_from_room(ch);
    char_to_room(ch, was_in);
    look_at_room(ch, 0);
  } else greet_memory_mtrigger(ch);
  return 1;
}


int perform_move(struct char_data *ch, int dir, int need_specials_check)
{
  int was_in;
  struct follow_type *k, *next;

  if (ch == NULL || dir < 0 || dir >= NUM_OF_DIRS || FIGHTING(ch))
    return 0;
  else if (!EXIT(ch, dir) || EXIT(ch, dir)->to_room == NOWHERE)
    send_to_char("Alas, you cannot go that way...\r\n", ch);
  else if (EXIT_FLAGGED(EXIT(ch, dir), EX_CLOSED)) {
    if (EXIT(ch, dir)->keyword) {
      sprintf(buf2, "The %s seems to be closed.\r\n", fname(EXIT(ch, dir)->keyword));
      send_to_char(buf2, ch);
    } else
      send_to_char("It seems to be closed.\r\n", ch);
  } else {
    if (!ch->followers)
      return (do_simple_move(ch, dir, need_specials_check));
    was_in = ch->in_room;
    if (!do_simple_move(ch, dir, need_specials_check))
      return 0;

    for (k = ch->followers; k; k = next) {
      next = k->next;
      if ((k->follower->in_room == was_in) &&
	  (GET_POS(k->follower) >= POS_STANDING)) {
	act("You follow $N.\r\n", FALSE, k->follower, 0, ch, TO_CHAR);
	perform_move(k->follower, dir, 1);
      }
    }
    return 1;
  }
  return 0;
}


ACMD(do_move)
{
  /*
   * This is basically a mapping of cmd numbers to perform_move indices.
   * It cannot be done in perform_move because perform_move is called
   * by other functions which do not require the remapping.
   */
  perform_move(ch, subcmd - 1, 0);
}


int find_door(struct char_data *ch, const char *type, char *dir, const char *cmdname)
{
  int door;

  if (*dir) {			/* a direction was specified */
    if ((door = search_block(dir, dirs, FALSE)) == -1) {	/* Partial Match */
      send_to_char("That's not a direction.\r\n", ch);
      return -1;
    }
    if (EXIT(ch, door)) {	/* Braces added according to indent. -gg */
      if (EXIT(ch, door)->keyword) {
	if (isname(type, EXIT(ch, door)->keyword))
	  return door;
	else {
	  sprintf(buf2, "I see no %s there.\r\n", type);
	  send_to_char(buf2, ch);
	  return -1;
        }
      } else
	return door;
    } else {
      sprintf(buf2, "I really don't see how you can %s anything there.\r\n", cmdname);
      send_to_char(buf2, ch);
      return -1;
    }
  } else {			/* try to locate the keyword */
    if (!*type) {
      sprintf(buf2, "What is it you want to %s?\r\n", cmdname);
      send_to_char(buf2, ch);
      return -1;
    }
    for (door = 0; door < NUM_OF_DIRS; door++)
      if (EXIT(ch, door))
	if (EXIT(ch, door)->keyword)
	  if (isname(type, EXIT(ch, door)->keyword))
	    return door;

    sprintf(buf2, "There doesn't seem to be %s %s here.\r\n", AN(type), type);
    send_to_char(buf2, ch);
    return -1;
  }
}


int has_key(struct char_data *ch, int key)
{
  struct obj_data *o;

  for (o = ch->carrying; o; o = o->next_content)
    if (GET_OBJ_VNUM(o) == key)
      return 1;

  if (GET_EQ(ch, WEAR_HOLD))
    if (GET_OBJ_VNUM(GET_EQ(ch, WEAR_HOLD)) == key)
      return 1;

  return 0;
}



#define NEED_OPEN	1
#define NEED_CLOSED	2
#define NEED_UNLOCKED	4
#define NEED_LOCKED	8

const char *cmd_door[] =
{
  "open",
  "close",
  "unlock",
  "lock",
  "pick"
};

const int flags_door[] =
{
  NEED_CLOSED | NEED_UNLOCKED,
  NEED_OPEN,
  NEED_CLOSED | NEED_LOCKED,
  NEED_CLOSED | NEED_UNLOCKED,
  NEED_CLOSED | NEED_LOCKED
};


#define EXITN(room, door)		(world[room].dir_option[door])
#define OPEN_DOOR(room, obj, door)	((obj) ?\
		(TOGGLE_BIT(GET_OBJ_VAL(obj, 1), CONT_CLOSED)) :\
		(TOGGLE_BIT(EXITN(room, door)->exit_info, EX_CLOSED)))
#define LOCK_DOOR(room, obj, door)	((obj) ?\
		(TOGGLE_BIT(GET_OBJ_VAL(obj, 1), CONT_LOCKED)) :\
		(TOGGLE_BIT(EXITN(room, door)->exit_info, EX_LOCKED)))

void do_doorcmd(struct char_data *ch, struct obj_data *obj, int door, int scmd)
{
  int other_room = 0;
  struct room_direction_data *back = 0;

  sprintf(buf, "$n %ss ", cmd_door[scmd]);
  if (!obj && ((other_room = EXIT(ch, door)->to_room) != NOWHERE))
    if ((back = world[other_room].dir_option[rev_dir[door]]))
      if (back->to_room != ch->in_room)
	back = 0;

  switch (scmd) {
  case SCMD_OPEN:
  case SCMD_CLOSE:
    OPEN_DOOR(ch->in_room, obj, door);
    if (back)
      OPEN_DOOR(other_room, obj, rev_dir[door]);
    send_to_char(OK, ch);
    break;
  case SCMD_UNLOCK:
  case SCMD_LOCK:
    LOCK_DOOR(ch->in_room, obj, door);
    if (back)
      LOCK_DOOR(other_room, obj, rev_dir[door]);
    send_to_char("*Click*\r\n", ch);
    break;
  case SCMD_PICK:
    LOCK_DOOR(ch->in_room, obj, door);
    if (back && GET_MANA(ch) >= GET_MAX_MANA(ch) / 5) 
      LOCK_DOOR(other_room, obj, rev_dir[door]);
      send_to_char("The lock quickly yields to your skills.\r\n", ch);
      strcpy(buf, "$n skillfully picks the lock on ");
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 10;
    break;
  }

  /* Notify the room */
  sprintf(buf + strlen(buf), "%s%s.", ((obj) ? "" : "the "), (obj) ? "$p" :
	  (EXIT(ch, door)->keyword ? "$F" : "door"));
  if (!(obj) || (obj->in_room != NOWHERE))
    act(buf, FALSE, ch, obj, obj ? 0 : EXIT(ch, door)->keyword, TO_ROOM);

  /* Notify the other room */
  if ((scmd == SCMD_OPEN || scmd == SCMD_CLOSE) && back) {
    sprintf(buf, "The %s is %s%s from the other side.\r\n",
	 (back->keyword ? fname(back->keyword) : "door"), cmd_door[scmd],
	    (scmd == SCMD_CLOSE) ? "d" : "ed");
    if (world[EXIT(ch, door)->to_room].people) {
      act(buf, FALSE, world[EXIT(ch, door)->to_room].people, 0, 0, TO_ROOM);
      act(buf, FALSE, world[EXIT(ch, door)->to_room].people, 0, 0, TO_CHAR);
    }
  }
}


int ok_pick(struct char_data *ch, int keynum, int pickproof, int scmd)
{
  int percent;

  percent = number(1, 101);

  if (scmd == SCMD_PICK) {
    if (keynum < 0)
      send_to_char("Odd - you can't seem to find a keyhole.\r\n", ch);
    else if (!GET_SKILL(ch, SKILL_PICK_LOCK))
     send_to_char("You don't know how to do that!\r\n", ch);
    else if (GET_MANA(ch) < GET_MAX_MANA(ch) / 5)
      send_to_char("You do not have enough ki for the concentration required!", ch);
    else if (pickproof)
      send_to_char("It resists your attempts to pick it.\r\n", ch);
    else if (percent >= GET_DEX(ch)) {
      send_to_char("You failed to pick the lock.\r\n", ch);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 5;
      }
    else
      return (1);
    return (0);
  }
  return (1);
}


#define DOOR_IS_OPENABLE(ch, obj, door)	((obj) ? \
			((GET_OBJ_TYPE(obj) == ITEM_CONTAINER) && \
			OBJVAL_FLAGGED(obj, CONT_CLOSEABLE)) :\
			(EXIT_FLAGGED(EXIT(ch, door), EX_ISDOOR)))
#define DOOR_IS_OPEN(ch, obj, door)	((obj) ? \
			(!OBJVAL_FLAGGED(obj, CONT_CLOSED)) :\
			(!EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED)))
#define DOOR_IS_UNLOCKED(ch, obj, door)	((obj) ? \
			(!OBJVAL_FLAGGED(obj, CONT_LOCKED)) :\
			(!EXIT_FLAGGED(EXIT(ch, door), EX_LOCKED)))
#define DOOR_IS_PICKPROOF(ch, obj, door) ((obj) ? \
			(OBJVAL_FLAGGED(obj, CONT_PICKPROOF)) : \
			(EXIT_FLAGGED(EXIT(ch, door), EX_PICKPROOF)))

#define DOOR_IS_CLOSED(ch, obj, door)	(!(DOOR_IS_OPEN(ch, obj, door)))
#define DOOR_IS_LOCKED(ch, obj, door)	(!(DOOR_IS_UNLOCKED(ch, obj, door)))
#define DOOR_KEY(ch, obj, door)		((obj) ? (GET_OBJ_VAL(obj, 2)) : \
					(EXIT(ch, door)->key))
#define DOOR_LOCK(ch, obj, door)	((obj) ? (GET_OBJ_VAL(obj, 1)) : \
					(EXIT(ch, door)->exit_info))

ACMD(do_gen_door)
{
  int door = -1, keynum;
  char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH];
  struct obj_data *obj = NULL;
  struct char_data *victim = NULL;

  skip_spaces(&argument);
  if (!*argument) {
    sprintf(buf, "%s what?\r\n", cmd_door[subcmd]);
    send_to_char(CAP(buf), ch);
    return;
  }
  two_arguments(argument, type, dir);
  if (!generic_find(type, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &victim, &obj))
    door = find_door(ch, type, dir, cmd_door[subcmd]);

  if ((obj) || (door >= 0)) {
    keynum = DOOR_KEY(ch, obj, door);
    if (!(DOOR_IS_OPENABLE(ch, obj, door)))
      act("You can't $F that!", FALSE, ch, 0, cmd_door[subcmd], TO_CHAR);
    else if (!DOOR_IS_OPEN(ch, obj, door) &&
	     IS_SET(flags_door[subcmd], NEED_OPEN))
      send_to_char("But it's already closed!\r\n", ch);
    else if (!DOOR_IS_CLOSED(ch, obj, door) &&
	     IS_SET(flags_door[subcmd], NEED_CLOSED))
      send_to_char("But it's currently open!\r\n", ch);
    else if (!(DOOR_IS_LOCKED(ch, obj, door)) &&
	     IS_SET(flags_door[subcmd], NEED_LOCKED))
      send_to_char("Oh.. it wasn't locked, after all..\r\n", ch);
    else if (!(DOOR_IS_UNLOCKED(ch, obj, door)) &&
	     IS_SET(flags_door[subcmd], NEED_UNLOCKED))
      send_to_char("It seems to be locked.\r\n", ch);
    else if (!has_key(ch, keynum) && (GET_LEVEL(ch) < LVL_GOD) &&
	     ((subcmd == SCMD_LOCK) || (subcmd == SCMD_UNLOCK)))
      send_to_char("You don't seem to have the proper key.\r\n", ch);
    else if (ok_pick(ch, keynum, DOOR_IS_PICKPROOF(ch, obj, door), subcmd))
      do_doorcmd(ch, obj, door, subcmd);
  }
  return;
}



ACMD(do_enter)
{
  struct obj_data *obj = NULL;
  int door;

  one_argument(argument, buf);

  if (*buf) {			/* an argument was supplied, search for door
				 * keyword */

   if ((obj = get_obj_in_list_vis(ch, buf, world[ch->in_room].contents))) {
      if (CAN_SEE_OBJ(ch, obj)) {
        if (GET_OBJ_TYPE(obj) == ITEM_TRAP) {
          if (GET_OBJ_VAL(obj, 0) != NOWHERE) {
             act("&14$n &15jumps into the glowing portal, disappearing in a flash of light!&00", TRUE, ch, 0, 0, TO_ROOM);
            char_from_room(ch);
            char_to_room(ch, GET_OBJ_VAL(obj, 0));
             act("&15The portal warps the world around you, spitting you out the other end..&00", FALSE, ch, 0, 0, TO_CHAR);
             act("&14$n &15pops out of the portal!&00", TRUE, ch, 0, 0, TO_ROOM);
          } else if (real_room(GET_OBJ_VAL(obj, 1)) != NOWHERE) {
            char_from_room(ch);
            char_to_room(ch, real_room(GET_OBJ_VAL(obj, 1)));
          }
          look_at_room(ch, 1);
          return;
        }
      }
    }

    for (door = 0; door < NUM_OF_DIRS; door++)
      if (EXIT(ch, door))
	if (EXIT(ch, door)->keyword)
	  if (!str_cmp(EXIT(ch, door)->keyword, buf)) {
	    perform_move(ch, door, 1);
	    return;
	  }
    sprintf(buf2, "There is no %s here.\r\n", buf);
    send_to_char(buf2, ch);
  } else if (ROOM_FLAGGED(ch->in_room, ROOM_INDOORS))
    send_to_char("You are already indoors.\r\n", ch);
  else {
    /* try to locate an entrance */
    for (door = 0; door < NUM_OF_DIRS; door++)
      if (EXIT(ch, door))
	if (EXIT(ch, door)->to_room != NOWHERE)
	  if (!EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED) &&
	      ROOM_FLAGGED(EXIT(ch, door)->to_room, ROOM_INDOORS)) {
	    perform_move(ch, door, 1);
	    return;
	  }
    send_to_char("You can't seem to find anything to enter.\r\n", ch);
  }
}


ACMD(do_leave)
{
  int door;

  if (!ROOM_FLAGGED(ch->in_room, ROOM_INDOORS))
    send_to_char("You are outside.. where do you want to go?\r\n", ch);
  else {
    for (door = 0; door < NUM_OF_DIRS; door++)
      if (EXIT(ch, door))
	if (EXIT(ch, door)->to_room != NOWHERE)
	  if (!EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED) &&
	    !ROOM_FLAGGED(EXIT(ch, door)->to_room, ROOM_INDOORS)) {
	    perform_move(ch, door, 1);
	    return;
	  }
    send_to_char("I see no obvious exits to the outside.\r\n", ch);
  }
}


ACMD(do_stand)
{
  if (PRF2_FLAGGED(ch, PRF2_FISHING)) {
   send_to_char("You are too busy fishing.\r\n", ch);
   return;
  }
  switch (GET_POS(ch)) {
  case POS_STANDING:
    act("You are already standing.", FALSE, ch, 0, 0, TO_CHAR);
    break;
  case POS_SITTING:
    if (FIGHTING(ch)) {
     if (number(1, 4) > 1) {
     act("You quickly move to your feet.", FALSE, ch, 0, 0, TO_CHAR);
     act("$n quickly moves to $s feet.", TRUE, ch, 0, 0, TO_ROOM);
     GET_POS(ch) = POS_FIGHTING;
     }
     else {
     act("You try to get up but collapse momentarily in pain.", FALSE, ch, 0, 0, TO_CHAR);
     act("$n to stand but $e slumps over in pain.", TRUE, ch, 0, 0, TO_ROOM);
     WAIT_STATE(ch, PULSE_VIOLENCE * 1);
     }
    }
    else {
    act("You quickly move to your feet.", FALSE, ch, 0, 0, TO_CHAR);
    act("$n quickly moves to $s feet.", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_STANDING;
    }
    break;
  case POS_RESTING:
    act("You stop resting, and quickly move to your feet", FALSE, ch, 0, 0, TO_CHAR);
    act("$n stops resting, and quickly moves to $s feet.", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_STANDING;
    break;
  case POS_SLEEPING:
    act("You have to wake up first!", FALSE, ch, 0, 0, TO_CHAR);
    break;
  case POS_FIGHTING:
    act("Do you not consider fighting as standing?", FALSE, ch, 0, 0, TO_CHAR);
    break;
  default:
    act("You stop floating around, and put your feet on the ground.",
	FALSE, ch, 0, 0, TO_CHAR);
    act("$n stops floating around, and puts $s feet on the ground.",
	TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_STANDING;
    break;
  }
}


ACMD(do_sit)
{
  switch (GET_POS(ch)) {
  case POS_STANDING:
    act("You sit down.", FALSE, ch, 0, 0, TO_CHAR);
    act("$n sits down.", FALSE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_SITTING;
    break;
  case POS_SITTING:
    send_to_char("You're sitting already.\r\n", ch);
    break;
  case POS_RESTING:
    act("You stop resting, and sit up.", FALSE, ch, 0, 0, TO_CHAR);
    act("$n stops resting.", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_SITTING;
    break;
  case POS_SLEEPING:
    act("You have to wake up first.", FALSE, ch, 0, 0, TO_CHAR);
    break;
  case POS_FIGHTING:
    act("Sit down while fighting? are you MAD?", FALSE, ch, 0, 0, TO_CHAR);
    break;
  default:
    act("You stop floating around, and sit down.", FALSE, ch, 0, 0, TO_CHAR);
    act("$n stops floating around, and sits down.", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_SITTING;
    break;
  }
}

ACMD(do_rest)
{
  switch (GET_POS(ch)) {
  case POS_STANDING:
    act("You sit down and lie back to rest.", FALSE, ch, 0, 0, TO_CHAR);
    act("$n sits down and lies back to rest.", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_RESTING;
    break;
  case POS_SITTING:
    act("You lie back to rest.", FALSE, ch, 0, 0, TO_CHAR);
    act("$n lies back to rests.", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_RESTING;
    break;
  case POS_RESTING:
    act("You are already resting.", FALSE, ch, 0, 0, TO_CHAR);
    break;
  case POS_SLEEPING:
    act("You have to wake up first.", FALSE, ch, 0, 0, TO_CHAR);
    break;
  case POS_FIGHTING:
    act("Rest while fighting?  Are you MAD?", FALSE, ch, 0, 0, TO_CHAR);
    break;
  default:
    act("You stop floating around, and stop to rest your tired bones.",
	FALSE, ch, 0, 0, TO_CHAR);
    act("$n stops floating around, and rests.", FALSE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_SITTING;
    break;
  }
}


ACMD(do_sleep)
{
  if (PRF2_FLAGGED(ch, PRF2_FISHING)) {
   send_to_char("You are too busy fishing.\r\n", ch);
   return;
  }
  switch (GET_POS(ch)) {
  case POS_STANDING:
    send_to_char("You lie down and drift into slumber.\r\n", ch);
    act("$n lies down and drifts into slumber.", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_SLEEPING;
    break;
  case POS_SITTING:
    send_to_char("You lie back and drift into slumber.\r\n", ch);
    act("$n lies back and drifts into slumber.", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_SLEEPING;
    break;
  case POS_RESTING:
    send_to_char("You slowly drift into slumber.\r\n", ch);
    act("$n slowly drifts into slumber.", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_SLEEPING;
    break;
  case POS_SLEEPING:
    send_to_char("You are already sound asleep.\r\n", ch);
    break;
  case POS_FIGHTING:
    send_to_char("Sleep while fighting?  Are you MAD?\r\n", ch);
    break;
  default:
    act("You stop floating around, and lie down to sleep.",
	FALSE, ch, 0, 0, TO_CHAR);
    act("$n stops floating around, and lie down to sleep.",
	TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_SLEEPING;
    break;
  }
}


ACMD(do_wake)
{
  struct char_data *vict;
  int self = 0;

  one_argument(argument, arg);
  if (*arg) {
    if (GET_POS(ch) == POS_SLEEPING)
      send_to_char("Maybe you should wake yourself up first.\r\n", ch);
    else if ((vict = get_char_room_vis(ch, arg)) == NULL)
      send_to_char(NOPERSON, ch);
    else if (vict == ch)
      self = 1;
    else if (GET_POS(vict) > POS_SLEEPING)
      act("$E is already awake.", FALSE, ch, 0, vict, TO_CHAR);
    else if (AFF_FLAGGED(vict, AFF_YOIKOMINMINKEN))
      act("You can't wake $M up!", FALSE, ch, 0, vict, TO_CHAR);
    else if (GET_POS(vict) < POS_SLEEPING)
      act("$E's in pretty bad shape!", FALSE, ch, 0, vict, TO_CHAR);
    else {
      act("You wake $M up.", FALSE, ch, 0, vict, TO_CHAR);
      act("You are awakened by $n.", FALSE, ch, 0, vict, TO_VICT | TO_SLEEP);
      GET_POS(vict) = POS_SITTING;
    }
    if (!self)
      return;
  }
  if (AFF_FLAGGED(ch, AFF_YOIKOMINMINKEN))
    send_to_char("You can't wake up!\r\n", ch);
  else if (GET_POS(ch) > POS_SLEEPING)
    send_to_char("You are already awake...\r\n", ch);
  else {
    if (AFF_FLAGGED(ch, AFF_STONED)) {
     send_to_char("&15You snore loudly, burnt out for the night...\r\n", ch);
     return;
    }
    send_to_char("You awaken, and stand up.\r\n", ch);
    act("$n awakens.", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_STANDING;
  }
}
ACMD(do_push)
{
  struct char_data *leader;
  struct char_data *vict;

  one_argument(argument, arg);
  if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char(NOPERSON, ch);
      return;
    }
  if (vict->master != ch) {
   send_to_char("That person isn't following you!\r\n", ch);
  }
  else if (vict->master == ch) {
   act("You push $N away.", FALSE, ch, 0, vict, TO_CHAR);
   act("$n pushes you away.", FALSE, ch, 0, vict, TO_NOTVICT);
   act("$N pushes $N away.", FALSE, ch, 0, vict, TO_VICT);
   stop_follower(vict);
  }
}
ACMD(do_follow)
{
  struct char_data *leader;

  one_argument(argument, buf);

  if (*buf) {
    if (!(leader = get_char_room_vis(ch, buf))) {
      send_to_char(NOPERSON, ch);
      return;
    }
    if (AFF_FLAGGED(ch, AFF_JOG)) {
     send_to_char("You can't follow someone while jogging.", ch);
     return;
    }
  } else {
    send_to_char("Whom do you wish to follow?\r\n", ch);
    return;
  }

  if (ch->master == leader) {
    act("You are already following $M.", FALSE, ch, 0, leader, TO_CHAR);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_MAJIN) && (ch->master)) {
    act("But you only feel like following $N!", FALSE, ch, 0, ch->master, TO_CHAR);
  } else {			/* Not MAJINed follow person */
    if (leader == ch) {
      if (!ch->master) {
	send_to_char("You are already following yourself.\r\n", ch);
	return;
      }
      stop_follower(ch);
    } else {
      if (circle_follow(ch, leader)) {
	act("Sorry, but following in loops is not allowed.", FALSE, ch, 0, 0, TO_CHAR);
	return;
      }
      if (ch->master)
	stop_follower(ch);
      REMOVE_BIT(AFF_FLAGS(ch), AFF_GROUP);
      add_follower(ch, leader);
    }
  }
}
