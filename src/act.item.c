/* ************************************************************************
*   File: act.item.c                                    Part of CircleMUD *
*  Usage: object handling routines -- get/drop and container handling     *
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
#include "dg_scripts.h"

/* extern variables */
extern sh_int donation_room_1;
#if 0
extern sh_int donation_room_2;  /* uncomment if needed! */
extern sh_int donation_room_3;  /* uncomment if needed! */
#endif
extern struct obj_data *obj_proto;
extern struct obj_data *obj_proto;
extern char *drinknames[];
extern struct str_app_type str_app[];
extern struct room_data *world;
extern char *drinks[];
extern int drink_aff[][3];
extern int auto_save;
extern int top_of_world;

/* local functions */
int can_take_obj(struct char_data * ch, struct obj_data * obj);
void get_check_money(struct char_data * ch, struct obj_data * obj);
int perform_get_from_room(struct char_data * ch, struct obj_data * obj);
void get_from_room(struct char_data * ch, char *arg);
void perform_give_gold(struct char_data * ch, struct char_data * vict, int amount);
void perform_give(struct char_data * ch, struct char_data * vict, struct obj_data * obj);
int perform_drop(struct char_data * ch, struct obj_data * obj, byte mode, const char *sname, sh_int RDR);
void perform_drop_gold(struct char_data * ch, int amount, byte mode, sh_int RDR);
struct char_data *give_find_vict(struct char_data * ch, char *arg);
void weight_change_object(struct obj_data * obj, int weight);
void perform_put(struct char_data * ch, struct obj_data * obj, struct obj_data * cont);
void name_from_drinkcon(struct obj_data * obj);
void get_from_container(struct char_data * ch, struct obj_data * cont, char *arg, int mode);
void name_to_drinkcon(struct obj_data * obj, int type);
void wear_message(struct char_data * ch, struct obj_data * obj, int where);
void perform_wear(struct char_data * ch, struct obj_data * obj, int where);
int find_eq_pos(struct char_data * ch, struct obj_data * obj, char *arg);
void perform_get_from_container(struct char_data * ch, struct obj_data * obj, struct obj_data * cont, int mode);
void perform_remove(struct char_data * ch, int pos);
ACMD(do_remove);
ACMD(do_put);
ACMD(do_get);
ACMD(do_drop);
ACMD(do_give);
ACMD(do_package);
ACMD(do_drink);
ACMD(do_eat);
ACMD(do_pour);
ACMD(do_wear);
ACMD(do_wield);
ACMD(do_grab);


void perform_put(struct char_data * ch, struct obj_data * obj,
		      struct obj_data * cont)
{
  if (!drop_otrigger(obj, ch))
    return;
  if (IS_OBJ_STAT(obj, ITEM_FLAG)) {
     send_to_char("You can't bag Flags!\r\n", ch);
     return;
    }
  /*if (GET_OBJ_WEIGHT(cont) + GET_OBJ_WEIGHT(obj) > GET_OBJ_VAL(cont, 0))
    act("$p won't fit in $P.", FALSE, ch, obj, cont, TO_CHAR);
  */
  if (GET_OBJ_WEIGHT(cont) >= 101) {
           send_to_char("That container can't hold anymore!\r\n", ch);
             return;
          }
  if (GET_OBJ_TYPE(obj) == ITEM_CONTAINER) {
    send_to_char("Not bags in bags!\r\n", ch);
    return;
   }
  if (IS_OBJ_STAT(cont, ITEM_NODONATE) && GET_OBJ_WEIGHT(cont) >= 7) {
    send_to_char("Your cells can't hold anymore!\r\n", ch);
             return;
          }
  if (IS_OBJ_STAT(obj, ITEM_NODONATE)) {
   send_to_char("Your cells can't be bagged!\r\n", ch);
             return;
          }
  if (IS_OBJ_STAT(cont, ITEM_NODONATE) && GET_OBJ_TYPE(obj) == ITEM_CONTAINER) {
   send_to_char("You can't put containers in the cells!\r\n", ch);
             return;
          }
  else {
    GET_OBJ_WEIGHT(cont) += 1;
    obj_from_char(obj);
    obj_to_obj(obj, cont);
    act("You put $p in $P.", FALSE, ch, obj, cont, TO_CHAR);
    act("$n puts $p in $P.", TRUE, ch, obj, cont, TO_ROOM);
  }
}


/* The following put modes are supported by the code below:

	1) put <object> <container>
	2) put all.<object> <container>
	3) put all <container>

	<container> must be in inventory or on ground.
	all objects to be put into container must be in inventory.
*/

ACMD(do_put)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  struct obj_data *obj, *next_obj, *cont;
  struct char_data *tmp_char;
  int obj_dotmode, cont_dotmode, found = 0;

  two_arguments(argument, arg1, arg2);
  obj_dotmode = find_all_dots(arg1);
  cont_dotmode = find_all_dots(arg2);
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RARM) && PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
     }
  if (!*arg1)
    send_to_char("Put what in what?\r\n", ch);
  else if (cont_dotmode != FIND_INDIV)
    send_to_char("You can only put things into one container at a time.\r\n", ch);
  else if (!*arg2) {
    sprintf(buf, "What do you want to put %s in?\r\n",
	    ((obj_dotmode == FIND_INDIV) ? "it" : "them"));
    send_to_char(buf, ch);
    strcat(buf, "\r\n");
  } else {
    generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &tmp_char, &cont);
    if (!cont) {
      sprintf(buf, "You don't see %s %s here.\r\n", AN(arg2), arg2);
      send_to_char(buf, ch);
      strcat(buf, "\r\n");
    } else if (GET_OBJ_TYPE(cont) != ITEM_CONTAINER)
      act("$p is not a container.", FALSE, ch, cont, 0, TO_CHAR);
    else if (OBJVAL_FLAGGED(cont, CONT_CLOSED))
      send_to_char("You'd better open it first!\r\n", ch);
    else {
      if (obj_dotmode == FIND_INDIV) {	/* put <obj> <container> */
	if (!(obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
	  sprintf(buf, "You aren't carrying %s %s.\r\n", AN(arg1), arg1);
	  send_to_char(buf, ch);
          strcat(buf, "\r\n");
	} else if (obj == cont)
	  send_to_char("You attempt to fold it into itself, but fail.\r\n", ch);
	else {
	  perform_put(ch, obj, cont);
        }
      } else {
	for (obj = ch->carrying; obj; obj = next_obj) {
	  next_obj = obj->next_content;
	  if (obj != cont && CAN_SEE_OBJ(ch, obj) &&
	      (obj_dotmode == FIND_ALL || isname(arg1, obj->name))) {
	    found = 1;
	    perform_put(ch, obj, cont);
	  }
	}
	if (!found) {
	  if (obj_dotmode == FIND_ALL)
	    send_to_char("You don't seem to have anything to put in it.\r\n", ch);
	  else {
	    sprintf(buf, "You don't seem to have any %ss.\r\n", arg1);
	    send_to_char(buf, ch);
            strcat(buf, "\r\n");
	  }
	}
      }
    }
  }
}



int can_take_obj(struct char_data * ch, struct obj_data * obj)
{
  if (IS_CARRYING_N(ch) >= 400) {
    act("$p: you can't carry that many items.", FALSE, ch, obj, 0, TO_CHAR);
    return 0;
  } else if ((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) > CAN_CARRY_W(ch)) {
    act("$p: you can't carry that much weight.", FALSE, ch, obj, 0, TO_CHAR);
    return 0;
  } else if (!(CAN_WEAR(obj, ITEM_WEAR_TAKE))) {
    act("$p: you can't take that!", FALSE, ch, obj, 0, TO_CHAR);
    return 0;
  }
  if (GET_OBJ_TYPE(obj) == ITEM_MONEY){
    return 1;
    }
  return 1;
}


void get_check_money(struct char_data * ch, struct obj_data * obj)
{
  if ((GET_OBJ_TYPE(obj) == ITEM_MONEY) && (GET_OBJ_VAL(obj, 0) > 0)) {
    obj_from_char(obj);
    if (GET_OBJ_VAL(obj, 0) > 1) {
      sprintf(buf, "There were %d coins.\r\n", GET_OBJ_VAL(obj, 0));
      send_to_char(buf, ch);
    }
    GET_GOLD(ch) += GET_OBJ_VAL(obj, 0);
    extract_obj(obj);
  }
}


void perform_get_from_container(struct char_data * ch, struct obj_data * obj,
				     struct obj_data * cont, int mode)
{
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RARM) && PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
     }
  if (mode == FIND_OBJ_INV || can_take_obj(ch, obj)) {
    if (IS_CARRYING_N(ch) >= 400 && !IS_NPC(ch))
      act("$p: you can't hold any more items.", FALSE, ch, obj, 0, TO_CHAR);
    else if (get_otrigger(obj, ch)) {
      obj_from_obj(obj);
      obj_to_char(obj, ch);
      if (!IS_NPC(ch)) {
       GET_OBJ_WEIGHT(cont) -= 1;
       act("You get $p from $P.", FALSE, ch, obj, cont, TO_CHAR);
       act("$n gets $p from $P.", TRUE, ch, obj, cont, TO_ROOM);
      }
      get_check_money(ch, obj);
    }
  }
}


void get_from_container(struct char_data * ch, struct obj_data * cont,
			     char *arg, int mode)
{
  struct obj_data *obj, *next_obj;
  int obj_dotmode, found = 0;

  obj_dotmode = find_all_dots(arg);

  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RARM) && PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
     }

  if (OBJVAL_FLAGGED(cont, CONT_CLOSED))
    act("$p is closed.", FALSE, ch, cont, 0, TO_CHAR);
  else if (obj_dotmode == FIND_INDIV) {
    if (!(obj = get_obj_in_list_vis(ch, arg, cont->contains))) {
      sprintf(buf, "There doesn't seem to be %s %s in $p.", AN(arg), arg);
      act(buf, FALSE, ch, cont, 0, TO_CHAR);
    } else
      perform_get_from_container(ch, obj, cont, mode);
  } else {
    if (obj_dotmode == FIND_ALLDOT && !*arg) {
      send_to_char("Get all of what?\r\n", ch);
      return;
    }
    for (obj = cont->contains; obj; obj = next_obj) {
      next_obj = obj->next_content;
      if (CAN_SEE_OBJ(ch, obj) &&
	  (obj_dotmode == FIND_ALL || isname(arg, obj->name))) {
	found = 1;
	perform_get_from_container(ch, obj, cont, mode);
      }
    }
    if (!found) {
      if (obj_dotmode == FIND_ALL)
	act("$p seems to be empty.", FALSE, ch, cont, 0, TO_CHAR);
      else {
	sprintf(buf, "You can't seem to find any %ss in $p.", arg);
	act(buf, FALSE, ch, cont, 0, TO_CHAR);
      }
    }
  }
}


int perform_get_from_room(struct char_data * ch, struct obj_data * obj)
{
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return 0;
}
  if (can_take_obj(ch, obj) && get_otrigger(obj, ch)) {
    obj_from_room(obj);
    obj_to_char(obj, ch);
    act("You get $p.", FALSE, ch, obj, 0, TO_CHAR);
    act("$n gets $p.", TRUE, ch, obj, 0, TO_ROOM);
    get_check_money(ch, obj);
    return 1;
  }
  return 0;
}


void get_from_room(struct char_data * ch, char *arg)
{
  struct obj_data *obj, *next_obj;
  int dotmode, found = 0;

  dotmode = find_all_dots(arg);
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }

  if (dotmode == FIND_INDIV) {
    if (!(obj = get_obj_in_list_vis(ch, arg, world[ch->in_room].contents))) {
      sprintf(buf, "You don't see %s %s here.\r\n", AN(arg), arg);
      send_to_char(buf, ch);
    } else
      perform_get_from_room(ch, obj);
  } else {
    if (dotmode == FIND_ALLDOT && !*arg) {
      send_to_char("Get all of what?\r\n", ch);
      return;
    }
    for (obj = world[ch->in_room].contents; obj; obj = next_obj) {
      next_obj = obj->next_content;
      if (CAN_SEE_OBJ(ch, obj) &&
	  (dotmode == FIND_ALL || isname(arg, obj->name))) {
	found = 1;
	perform_get_from_room(ch, obj);
      }
    }
    if (!found) {
      if (dotmode == FIND_ALL)
	send_to_char("There doesn't seem to be anything here.\r\n", ch);
      else {
	sprintf(buf, "You don't see any %ss here.\r\n", arg);
	send_to_char(buf, ch);
      }
    }
  }
}



ACMD(do_get)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];

  int cont_dotmode, found = 0, mode;
  struct obj_data *cont;
  struct char_data *tmp_char;

  two_arguments(argument, arg1, arg2);
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }

  if (IS_CARRYING_N(ch) >= 400)
    send_to_char("Your arms are already full!\r\n", ch);
  else if (!*arg1)
    send_to_char("Get what?\r\n", ch);
  else if (!*arg2)
    get_from_room(ch, arg1);
  else {
    cont_dotmode = find_all_dots(arg2);
    if (cont_dotmode == FIND_INDIV) {
      mode = generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &tmp_char, &cont);
      if (!cont) {
	sprintf(buf, "You don't have %s %s.\r\n", AN(arg2), arg2);
	send_to_char(buf, ch);
      } else if (GET_OBJ_TYPE(cont) != ITEM_CONTAINER)
	act("$p is not a container.", FALSE, ch, cont, 0, TO_CHAR);
      else 
	get_from_container(ch, cont, arg1, mode);
    } else {
      if (cont_dotmode == FIND_ALLDOT && !*arg2) {
	send_to_char("Get from all of what?\r\n", ch);
	return;
      }
      for (cont = ch->carrying; cont; cont = cont->next_content) {
	  if (CAN_SEE_OBJ(ch, cont) &&
	    (cont_dotmode == FIND_ALL || isname(arg2, cont->name))) {
	  if (GET_OBJ_TYPE(cont) == ITEM_CONTAINER) {
	    found = 1;
	    get_from_container(ch, cont, arg1, FIND_OBJ_INV);
	  } else if (cont_dotmode == FIND_ALLDOT) {
	    found = 1;
	    act("$p is not a container.", FALSE, ch, cont, 0, TO_CHAR);
	  }
	}
      }
      for (cont = world[ch->in_room].contents; cont; cont = cont->next_content) {
	if (CAN_SEE_OBJ(ch, cont) &&
	    (cont_dotmode == FIND_ALL || isname(arg2, cont->name))) {
	  if (GET_OBJ_TYPE(cont) == ITEM_CONTAINER) {
	    get_from_container(ch, cont, arg1, FIND_OBJ_ROOM);
	    found = 1;
	  } else if (cont_dotmode == FIND_ALLDOT) {
	    act("$p is not a container.", FALSE, ch, cont, 0, TO_CHAR);
	    found = 1;
	  }
	}
      }
      if (!found) {
	if (cont_dotmode == FIND_ALL)
	  send_to_char("You can't seem to find any containers.\r\n", ch);
	else {
	  sprintf(buf, "You can't seem to find any %ss here.\r\n", arg2);
	  send_to_char(buf, ch);
	}
      }
    }
  }
}


void perform_drop_gold(struct char_data * ch, int amount,
		            byte mode, sh_int RDR)
{
  struct obj_data *obj;

  if (amount <= 0)
    send_to_char("Heh heh heh.. we are jolly funny today, eh?\r\n", ch);
  else if (GET_GOLD(ch) < amount)
    send_to_char("You don't have that many coins!\r\n", ch);
  else {
    if (mode != SCMD_JUNK) {
      WAIT_STATE(ch, PULSE_VIOLENCE);	/* to prevent coin-bombing */
      obj = create_money(amount);
      if (mode == SCMD_DONATE) {
	send_to_char("You throw some gold into the air where it disappears in a puff of smoke!\r\n", ch);
	act("$n throws some gold into the air where it disappears in a puff of smoke!",
	    FALSE, ch, 0, 0, TO_ROOM);
	obj_to_room(obj, RDR);
	act("$p suddenly appears in a puff of orange smoke!", 0, 0, obj, 0, TO_ROOM);
      } else {
        if (!drop_wtrigger(obj, ch)) {
          extract_obj(obj);
          return;
        }
	send_to_char("You drop some gold.\r\n", ch);
	sprintf(buf, "$n drops %s.", money_desc(amount));
	act(buf, TRUE, ch, 0, 0, TO_ROOM);
	obj_to_room(obj, ch->in_room);
      }
    } else {
      sprintf(buf, "$n drops %s which disappears in a puff of smoke!",
	      money_desc(amount));
      act(buf, FALSE, ch, 0, 0, TO_ROOM);
      send_to_char("You drop some gold which disappears in a puff of smoke!\r\n", ch);
    }
    GET_GOLD(ch) -= amount;
  }
  if (auto_save && !IS_NPC(ch)) {
        save_char(ch, NOWHERE);
        Crash_crashsave(ch);
        if (ROOM_FLAGGED(ch->in_room, ROOM_HOUSE_CRASH))
        House_crashsave(GET_ROOM_VNUM(IN_ROOM(ch)));
  }
}


#define VANISH(mode) ((mode == SCMD_DONATE || mode == SCMD_JUNK) ? \
		      "  It vanishes in a puff of smoke!" : "")

int perform_drop(struct char_data * ch, struct obj_data * obj,
		     byte mode, const char *sname, sh_int RDR)
{
  int value;
  DYN_DEFINE;
  DYN_CREATE;
  *dynbuf = 0;

  if (!drop_otrigger(obj, ch))
    return 0;
  if ((mode == SCMD_DROP) && !drop_wtrigger(obj, ch))
    return 0;

  if (IS_OBJ_STAT(obj, ITEM_NODROP) && !IS_OBJ_STAT(obj, ITEM_NODONATE)) {
    sprintf(buf, "You can't %s $p, it must be CURSED!", sname);
    act(buf, FALSE, ch, obj, 0, TO_CHAR);
    return 0;
  }
  else if (IS_OBJ_STAT(obj, ITEM_NODROP) && IS_OBJ_STAT(obj, ITEM_NODONATE)) {
    sprintf(buf, "You can't %s $p, they are a part of your body!", sname);
    act(buf, FALSE, ch, obj, 0, TO_CHAR);
    return 0;
  }
  sprintf(buf, "You %s $p.%s", sname, VANISH(mode));
  act(buf, FALSE, ch, obj, 0, TO_CHAR);
  sprintf(buf, "$n %ss $p.%s", sname, VANISH(mode));
  act(buf, TRUE, ch, obj, 0, TO_ROOM);
  obj_from_char(obj);
  if ((mode == SCMD_DONATE) && IS_OBJ_STAT(obj, ITEM_NODONATE))
    mode = SCMD_JUNK;

  switch (mode) {
  case SCMD_DROP:
    obj_to_room(obj, ch->in_room);
    return 0;
  case SCMD_DONATE:
    obj_to_room(obj, RDR);
    act("$p suddenly appears in a puff a smoke!", FALSE, 0, obj, 0, TO_ROOM);
    return 0;
  case SCMD_JUNK:
    value = MAX(1, MIN(200, GET_OBJ_COST(obj) / 16));
    extract_obj(obj);
    return value;
  default:
    log("SYSERR: Incorrect argument %d passed to perform_drop.", mode);
    break;
  }
  if (auto_save && !IS_NPC(ch)) {
        save_char(ch, NOWHERE);
        Crash_crashsave(ch);
        if (ROOM_FLAGGED(ch->in_room, ROOM_HOUSE_CRASH))
        House_crashsave(GET_ROOM_VNUM(IN_ROOM(ch)));
  }
  return 0;
}



ACMD(do_drop)
{
  struct obj_data *obj, *next_obj;
  sh_int RDR = 0;
  byte mode = SCMD_DROP;
  int dotmode, amount = 0;
  const char *sname;
  DYN_DEFINE;
  
  DYN_CREATE;
  *dynbuf = 0;

  switch (subcmd) {
  case SCMD_JUNK:
    sname = "junk";
    mode = SCMD_JUNK;
    break;
  case SCMD_DONATE:
    sname = "donate";
    mode = SCMD_DONATE;
    switch (number(0, 2)) {
    case 0:
      mode = SCMD_JUNK;
      break;
    case 1:
    case 2:
      RDR = real_room(donation_room_1);
      break;
/*    case 3: RDR = real_room(donation_room_2); break;
      case 4: RDR = real_room(donation_room_3); break;
*/
    }
    if (RDR == NOWHERE) {
      send_to_char("Sorry, you can't donate anything right now.\r\n", ch);
      return;
    }
    break;
  default:
    sname = "drop";
    break;
  }

  argument = one_argument(argument, arg);

  if (!*arg) {
    sprintf(buf, "What do you want to %s?\r\n", sname);
    send_to_char(buf, ch);
    strcat(buf, "\r\n");
    DYN_RESIZE(buf);
    return;
  } else if (is_number(arg)) {
    amount = atoi(arg);
    argument = one_argument(argument, arg);
    if (!str_cmp("zenni", arg) || !str_cmp("coin", arg) || !str_cmp("coins", arg))
      perform_drop_gold(ch, amount, mode, RDR);
    else {
      /* code to drop multiple items.  anyone want to write it? -je */
      send_to_char("Sorry, you can't do that to more than one item at a time.\r\n", ch);
    }
    return;
  } else {
    dotmode = find_all_dots(arg);

    /* Can't junk or donate all */
    if (dotmode == FIND_ALL) {
      if (!ch->carrying)
	send_to_char("You don't seem to be carrying anything.\r\n", ch);
      else
	for (obj = ch->carrying; obj; obj = next_obj) {
	  next_obj = obj->next_content;
	  amount += perform_drop(ch, obj, mode, sname, RDR);
	}
    } else if (dotmode == FIND_ALLDOT) {
      if (!*arg) {
	sprintf(buf, "What do you want to %s all of?\r\n", sname);
	send_to_char(buf, ch);
	return;
      }
      if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
	sprintf(buf, "You don't seem to have any %ss.\r\n", arg);
	send_to_char(buf, ch);
      }
      while (obj) {
	next_obj = get_obj_in_list_vis(ch, arg, obj->next_content);
	amount += perform_drop(ch, obj, mode, sname, RDR);
	obj = next_obj;
      }
    } else {
      if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
        sprintf(buf, "You don't seem to have %s %s.\r\n", AN(arg), arg);
	send_to_char(buf, ch);
      } else
	amount += perform_drop(ch, obj, mode, sname, RDR);
    }
  }

  if (amount && (subcmd == SCMD_JUNK)) {
    send_to_char("Your God kindly rewards you for your donation! With 5 zenni\r\n", ch);
    act("$n has kindly been rewarded by $s god.", TRUE, ch, 0, 0, TO_ROOM);
    GET_GOLD(ch) = GET_GOLD(ch) + 5;
  }
  if (auto_save && !IS_NPC(ch)) {
        save_char(ch, NOWHERE);
        Crash_crashsave(ch);
        if (ROOM_FLAGGED(ch->in_room, ROOM_HOUSE_CRASH))
        House_crashsave(GET_ROOM_VNUM(IN_ROOM(ch)));
  }
}


void perform_give(struct char_data * ch, struct char_data * vict,
		       struct obj_data * obj)
{
  if (IS_OBJ_STAT(obj, ITEM_NODROP)) {
    act("You can't let go of $p!!  Yeech!", FALSE, ch, obj, 0, TO_CHAR);
    return;
  }
  if (IS_CARRYING_N(vict) >= 400) {
    act("$N seems to have $S hands full.", FALSE, ch, 0, vict, TO_CHAR);
    return;
  }
  if (GET_OBJ_WEIGHT(obj) + IS_CARRYING_W(vict) > CAN_CARRY_W(vict)) {
    act("$E can't carry that much weight.", FALSE, ch, 0, vict, TO_CHAR);
    return;
  }
  if (GET_OBJ_TYPE(obj) == ITEM_KEY && IS_NPC(vict)) {
       send_to_char("No giving keys to mobs!\r\n", ch);
       return;
      }
  if (!give_otrigger(obj, ch, vict) || !receive_mtrigger(vict, ch, obj))
    return;

  obj_from_char(obj);
  obj_to_char(obj, vict);
  act("You give $p to $N.", FALSE, ch, obj, vict, TO_CHAR);
  act("$n gives you $p.", FALSE, ch, obj, vict, TO_VICT);
  act("$n gives $p to $N.", TRUE, ch, obj, vict, TO_NOTVICT);
}

/* utility function for give */
struct char_data *give_find_vict(struct char_data * ch, char *arg)
{
  struct char_data *vict;

  if (!*arg) {
    send_to_char("To who?\r\n", ch);
    return NULL;
  } else if (!(vict = get_char_room_vis(ch, arg))) {
    send_to_char(NOPERSON, ch);
    return NULL;
  } else if (vict == ch) {
    send_to_char("What's the point of that?\r\n", ch);
    return NULL;
  } else
    return vict;
}


void perform_give_gold(struct char_data * ch, struct char_data * vict,
		            int amount)
{
  if (amount <= 0) {
    send_to_char("Heh heh heh ... we are jolly funny today, eh?\r\n", ch);
    return;
  }
  if ((GET_GOLD(ch) < amount) && (IS_NPC(ch) || (GET_LEVEL(ch) < LVL_GOD))) {
    send_to_char("You don't have that many coins!\r\n", ch);
    return;
  }
  send_to_char(OK, ch);
  sprintf(buf, "$n gives you %d gold coin%s.", amount, amount == 1 ? "" : "s");
  act(buf, FALSE, ch, 0, vict, TO_VICT);
  sprintf(buf, "$n gives %s to $N.", money_desc(amount));
  act(buf, TRUE, ch, 0, vict, TO_NOTVICT);
  if (IS_NPC(ch) || (GET_LEVEL(ch) < LVL_GOD))
    GET_GOLD(ch) -= amount;
  GET_GOLD(vict) += amount;

  bribe_mtrigger(vict, ch, amount);
}

ACMD(do_package)
{
  struct obj_data *obj;
  struct char_data *vict;
  two_arguments(argument, buf, arg);
  
  if (!*buf) {
   send_to_char("You need to specify a person to send your package to, as well as\r\nwhat you are sending. \r\n&11Example&16: &15package carl tofu.\r\n", ch);
   return;
  }
  else if (!(vict = get_char_vis(ch, buf))) {
    send_to_char(NOPERSON, ch);
    return;
  }
  if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
        sprintf(buf1, "You don't seem to have that.\r\n");
        send_to_char(buf1, ch);
        return;
  }
  if (GET_OBJ_TYPE(obj) == ITEM_KEY) {
       send_to_char("No packaging keys!\r\n", ch);
       return;
      }
  if (IS_OBJ_STAT(obj, ITEM_NODONATE)) {
      send_to_char("No packaging that!\r\n", ch);
       return;
      }
  if (GET_GOLD(ch) <= 24999) {
   send_to_char("You need at least 25000 zenni to send a package to someone.\r\n", ch);
   return;
  }
  else {
   sprintf(buf1, "&16[&15You mail &12%s &15to &09%s&16]&00\r\n", obj->name, GET_NAME(vict));
   sprintf(buf2, "&16[&14Package&16] &09%s &15has mailed &12%s&15 to you.&00\r\n", GET_NAME(ch), obj->name);
   send_to_char(buf1, ch);
   send_to_char(buf2, vict);
   obj_from_char(obj);
   obj_to_char(obj, vict);
   GET_GOLD(ch) -= 25000;
   return;
  }
}
ACMD(do_give)
{
  int amount, dotmode;
  struct char_data *vict;
  struct obj_data *obj, *next_obj;

  argument = one_argument(argument, arg);
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}

  if (!*arg)
    send_to_char("Give what to who?\r\n", ch);
  else if (is_number(arg)) {
    amount = atoi(arg);
    argument = one_argument(argument, arg);
    if (!str_cmp("zenni", arg) || !str_cmp("coin", arg) || !str_cmp("coins", arg)) {
      argument = one_argument(argument, arg);
      if ((vict = give_find_vict(ch, arg)))
	perform_give_gold(ch, vict, amount);
      if (auto_save && !IS_NPC(ch)) {
        save_char(ch, NOWHERE);
        Crash_crashsave(ch);
        if (ROOM_FLAGGED(ch->in_room, ROOM_HOUSE_CRASH))
        House_crashsave(GET_ROOM_VNUM(IN_ROOM(ch)));
      }
      return;
    } else {
      /* code to give multiple items.  anyone want to write it? -je */
      send_to_char("You can't give more than one item at a time.\r\n", ch);
      return;
    }
  } else {
    one_argument(argument, buf1);
    if (!(vict = give_find_vict(ch, buf1)))
      return;
    dotmode = find_all_dots(arg);
    if (dotmode == FIND_INDIV) {
      if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
	sprintf(buf, "You don't seem to have %s %s.\r\n", AN(arg), arg);
	send_to_char(buf, ch);
      } else
	perform_give(ch, vict, obj);
        if (auto_save && !IS_NPC(ch)) {
        save_char(ch, NOWHERE);
        Crash_crashsave(ch);
        if (ROOM_FLAGGED(ch->in_room, ROOM_HOUSE_CRASH))
        House_crashsave(GET_ROOM_VNUM(IN_ROOM(ch)));
        }
    } else {
      if (dotmode == FIND_ALLDOT && !*arg) {
	send_to_char("All of what?\r\n", ch);
	return;
      }
      if (!ch->carrying)
	send_to_char("You don't seem to be holding anything.\r\n", ch);
      else
	for (obj = ch->carrying; obj; obj = next_obj) {
	  next_obj = obj->next_content;
	  if (CAN_SEE_OBJ(ch, obj) &&
	      ((dotmode == FIND_ALL || isname(arg, obj->name))))
	    perform_give(ch, vict, obj);
	}
       if (auto_save && !IS_NPC(ch)) {
             save_char(ch, NOWHERE);
             Crash_crashsave(ch);
             if (ROOM_FLAGGED(ch->in_room, ROOM_HOUSE_CRASH))
             House_crashsave(GET_ROOM_VNUM(IN_ROOM(ch)));
           }
    }
  }
}



void weight_change_object(struct obj_data * obj, int weight)
{
  struct obj_data *tmp_obj;
  struct char_data *tmp_ch;

  if (obj->in_room != NOWHERE) {
    GET_OBJ_WEIGHT(obj) += weight;
  } else if ((tmp_ch = obj->carried_by)) {
    obj_from_char(obj);
    GET_OBJ_WEIGHT(obj) += weight;
    obj_to_char(obj, tmp_ch);
  } else if ((tmp_obj = obj->in_obj)) {
    obj_from_obj(obj);
    GET_OBJ_WEIGHT(obj) += GET_OBJ_WEIGHT(tmp_obj);
    obj_to_obj(obj, tmp_obj);
  } else {
    log("SYSERR: Unknown attempt to subtract weight from an object.");
  }
}



void name_from_drinkcon(struct obj_data * obj)
{
  int i;
  char *new_name;

  for (i = 0; (*((obj->name) + i) != ' ') && (*((obj->name) + i) != '\0'); i++);

  if (*((obj->name) + i) == ' ') {
    new_name = str_dup((obj->name) + i + 1);
    if (GET_OBJ_RNUM(obj) < 0 || obj->name != obj_proto[GET_OBJ_RNUM(obj)].name)
      free(obj->name);
    obj->name = new_name;
  }
}



void name_to_drinkcon(struct obj_data * obj, int type)
{
  char *new_name;

  CREATE(new_name, char, strlen(obj->name) + strlen(drinknames[type]) + 2);
  sprintf(new_name, "%s %s", drinknames[type], obj->name);
  if (GET_OBJ_RNUM(obj) < 0 || obj->name != obj_proto[GET_OBJ_RNUM(obj)].name)
    free(obj->name);
  obj->name = new_name;
}



ACMD(do_drink)
{
  struct obj_data *temp;
  struct affected_type af;
  int amount, weight;
  int on_ground = 0;

  one_argument(argument, arg);
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}

  if (IS_NPC(ch))	/* Cannot use GET_COND() on mobs. */
    return;

  if (!*arg) {
    send_to_char("Drink from what?\r\n", ch);
    return;
  }
  if (!(temp = get_obj_in_list_vis(ch, arg, ch->carrying))) {
    if (!(temp = get_obj_in_list_vis(ch, arg, world[ch->in_room].contents))) {
      act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
      return;
    } else
      on_ground = 1;
  }
  if ((GET_OBJ_TYPE(temp) != ITEM_DRINKCON) &&
      (GET_OBJ_TYPE(temp) != ITEM_FOUNTAIN)) {
    send_to_char("You can't drink from that!\r\n", ch);
    return;
  }
  if (on_ground && (GET_OBJ_TYPE(temp) == ITEM_DRINKCON)) {
    send_to_char("You have to be holding that to drink from it.\r\n", ch);
    return;
  }
  if (GET_COND(ch, DRUNK) > 4) {
    send_to_char("You feel pretty crocked.\r\n", ch);
  }
  if (GET_COND(ch, DRUNK) >= 40) {
    /* The pig is drunk */
    send_to_char("You can't seem to get close enough to your mouth.\r\n", ch);
    act("$n tries to drink but misses $s mouth!", TRUE, ch, 0, 0, TO_ROOM);
    return;
  }
  if ((GET_COND(ch, FULL) > 20) && (GET_COND(ch, THIRST) > 0)) {
    send_to_char("Your stomach can't contain anymore!\r\n", ch);
    return;
  }
  if (!GET_OBJ_VAL(temp, 1)) {
    send_to_char("It's empty.\r\n", ch);
    return;
  }
  if (subcmd == SCMD_DRINK) {
    sprintf(buf, "$n drinks %s from $p.", drinks[GET_OBJ_VAL(temp, 2)]);
    act(buf, TRUE, ch, temp, 0, TO_ROOM);

    sprintf(buf, "You drink the %s.\r\n", drinks[GET_OBJ_VAL(temp, 2)]);
    send_to_char(buf, ch);

    if (drink_aff[GET_OBJ_VAL(temp, 2)][DRUNK] > 0)
      amount = (25 - GET_COND(ch, THIRST)) / drink_aff[GET_OBJ_VAL(temp, 2)][DRUNK];
    else
      amount = number(3, 10);

  } else {
    act("$n sips from $p.", TRUE, ch, temp, 0, TO_ROOM);
    sprintf(buf, "It tastes like %s.\r\n", drinks[GET_OBJ_VAL(temp, 2)]);
    send_to_char(buf, ch);
    amount = 1;
  }

  amount = MIN(amount, GET_OBJ_VAL(temp, 1));

  /* You can't subtract more than the object weighs */
  weight = MIN(amount, GET_OBJ_WEIGHT(temp));

  weight_change_object(temp, -weight);	/* Subtract amount */

  gain_condition(ch, DRUNK,
	 (int) ((int) drink_aff[GET_OBJ_VAL(temp, 2)][DRUNK] * amount) / 4);

  gain_condition(ch, FULL,
	  (int) ((int) drink_aff[GET_OBJ_VAL(temp, 2)][FULL] * amount) / 4);

  gain_condition(ch, THIRST,
	(int) ((int) drink_aff[GET_OBJ_VAL(temp, 2)][THIRST] * amount) / 4);

  if (GET_COND(ch, DRUNK) > 10)
    send_to_char("You feel drunk.\r\n", ch);

  if (GET_COND(ch, THIRST) > 20)
    send_to_char("You don't feel thirsty any more.\r\n", ch);

  if (GET_COND(ch, FULL) > 20)
    send_to_char("You are full.\r\n", ch);

  if (GET_OBJ_VAL(temp, 3)) {	/* The shit was poisoned ! */
    send_to_char("Oops, it tasted rather strange!\r\n", ch);
    act("$n chokes and utters some strange sounds.", TRUE, ch, 0, 0, TO_ROOM);

    af.type = SPELL_POISON;
    af.duration = amount * 3;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_POISON;
    affect_join(ch, &af, FALSE, FALSE, FALSE, FALSE);
  }
  /* empty the container, and no longer poison. */
  GET_OBJ_VAL(temp, 1) -= amount;
  if (!GET_OBJ_VAL(temp, 1)) {	/* The last bit */
    GET_OBJ_VAL(temp, 2) = 0;
    GET_OBJ_VAL(temp, 3) = 0;
    name_from_drinkcon(temp);
  }
  return;
}



ACMD(do_eat)
{
  struct obj_data *food;
  struct affected_type af;
  int amount;

  one_argument(argument, arg);
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}

  if (IS_NPC(ch))	/* Cannot use GET_COND() on mobs. */
    return;

  if (!*arg) {
    send_to_char("Eat what?\r\n", ch);
    return;
  }
  if (!(food = get_obj_in_list_vis(ch, arg, ch->carrying))) {
    sprintf(buf, "You don't seem to have %s %s.\r\n", AN(arg), arg);
    send_to_char(buf, ch);
    return;
  }
  if (subcmd == SCMD_TASTE && ((GET_OBJ_TYPE(food) == ITEM_DRINKCON) ||
			       (GET_OBJ_TYPE(food) == ITEM_FOUNTAIN))) {
    do_drink(ch, argument, 0, SCMD_SIP);
    return;
  }
  if ((GET_OBJ_TYPE(food) != ITEM_FOOD) && (GET_LEVEL(ch) < LVL_GOD)) {
    send_to_char("You can't eat THAT!\r\n", ch);
    return;
  }
  if (GET_COND(ch, FULL) > 20) {/* Stomach full */
    act("You are too full to eat more!", FALSE, ch, 0, 0, TO_CHAR);
    return;
  }
  if (subcmd == SCMD_EAT) {
    act("You eat the $o.", FALSE, ch, food, 0, TO_CHAR);
    act("$n eats $p.", TRUE, ch, food, 0, TO_ROOM);
  } else {
    act("You nibble a little bit of the $o.", FALSE, ch, food, 0, TO_CHAR);
    act("$n tastes a little bit of $p.", TRUE, ch, food, 0, TO_ROOM);
  }

  amount = (subcmd == SCMD_EAT ? GET_OBJ_VAL(food, 0) : 1);

  gain_condition(ch, FULL, amount);

  if (GET_COND(ch, FULL) > 20)
    act("You are full.", FALSE, ch, 0, 0, TO_CHAR);

  if (GET_OBJ_VAL(food, 3) && (GET_LEVEL(ch) < LVL_IMMORT)) {
    /* The shit was poisoned ! */
    send_to_char("Oops, that tasted rather strange!\r\n", ch);
    act("$n coughs and utters some strange sounds.", FALSE, ch, 0, 0, TO_ROOM);

    af.type = SPELL_POISON;
    af.duration = amount * 2;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_POISON;
    affect_join(ch, &af, FALSE, FALSE, FALSE, FALSE);
  }
  if (subcmd == SCMD_EAT)
    extract_obj(food);
  else {
    if (!(--GET_OBJ_VAL(food, 0))) {
      send_to_char("There's nothing left now.\r\n", ch);
      extract_obj(food);
    }
  }
}


ACMD(do_pour)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  struct obj_data *from_obj = NULL, *to_obj = NULL;
  int amount;

  two_arguments(argument, arg1, arg2);
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RARM) && PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
     }
  if (subcmd == SCMD_POUR) {
    if (!*arg1) {		/* No arguments */
      act("From what do you want to pour?", FALSE, ch, 0, 0, TO_CHAR);
      return;
    }
    if (!(from_obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
      act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
      return;
    }
    if (GET_OBJ_TYPE(from_obj) != ITEM_DRINKCON) {
      act("You can't pour from that!", FALSE, ch, 0, 0, TO_CHAR);
      return;
    }
  }
  if (subcmd == SCMD_FILL) {
    if (!*arg1) {		/* no arguments */
      send_to_char("What do you want to fill?  And what are you filling it from?\r\n", ch);
      return;
    }
    if (!(to_obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
      send_to_char("You can't find it!", ch);
      return;
    }
    if (GET_OBJ_TYPE(to_obj) != ITEM_DRINKCON) {
      act("You can't fill $p!", FALSE, ch, to_obj, 0, TO_CHAR);
      return;
    }
    if (!*arg2) {		/* no 2nd argument */
      act("What do you want to fill $p from?", FALSE, ch, to_obj, 0, TO_CHAR);
      return;
    }
    if (!(from_obj = get_obj_in_list_vis(ch, arg2, world[ch->in_room].contents))) {
      sprintf(buf, "There doesn't seem to be %s %s here.\r\n", AN(arg2), arg2);
      send_to_char(buf, ch);
      return;
    }
    if (GET_OBJ_TYPE(from_obj) != ITEM_FOUNTAIN) {
      act("You can't fill something from $p.", FALSE, ch, from_obj, 0, TO_CHAR);
      return;
    }
  }
  if (GET_OBJ_VAL(from_obj, 1) == 0) {
    act("The $p is empty.", FALSE, ch, from_obj, 0, TO_CHAR);
    return;
  }
  if (subcmd == SCMD_POUR) {	/* pour */
    if (!*arg2) {
      act("Where do you want it?  Out or in what?", FALSE, ch, 0, 0, TO_CHAR);
      return;
    }
    if (!str_cmp(arg2, "out")) {
      act("$n empties $p.", TRUE, ch, from_obj, 0, TO_ROOM);
      act("You empty $p.", FALSE, ch, from_obj, 0, TO_CHAR);

      weight_change_object(from_obj, -GET_OBJ_VAL(from_obj, 1)); /* Empty */

      GET_OBJ_VAL(from_obj, 1) = 0;
      GET_OBJ_VAL(from_obj, 2) = 0;
      GET_OBJ_VAL(from_obj, 3) = 0;
      name_from_drinkcon(from_obj);

      return;
    }
    if (!(to_obj = get_obj_in_list_vis(ch, arg2, ch->carrying))) {
      act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
      return;
    }
    if ((GET_OBJ_TYPE(to_obj) != ITEM_DRINKCON) &&
	(GET_OBJ_TYPE(to_obj) != ITEM_FOUNTAIN)) {
      act("You can't pour anything into that.", FALSE, ch, 0, 0, TO_CHAR);
      return;
    }
  }
  if (to_obj == from_obj) {
    act("A most unproductive effort.", FALSE, ch, 0, 0, TO_CHAR);
    return;
  }
  if ((GET_OBJ_VAL(to_obj, 1) != 0) &&
      (GET_OBJ_VAL(to_obj, 2) != GET_OBJ_VAL(from_obj, 2))) {
    act("There is already another liquid in it!", FALSE, ch, 0, 0, TO_CHAR);
    return;
  }
  if (!(GET_OBJ_VAL(to_obj, 1) < GET_OBJ_VAL(to_obj, 0))) {
    act("There is no room for more.", FALSE, ch, 0, 0, TO_CHAR);
    return;
  }
  if (subcmd == SCMD_POUR) {
    sprintf(buf, "You pour the %s into the %s.",
	    drinks[GET_OBJ_VAL(from_obj, 2)], arg2);
    send_to_char(buf, ch);
  }
  if (subcmd == SCMD_FILL) {
    act("You gently fill $p from $P.", FALSE, ch, to_obj, from_obj, TO_CHAR);
    act("$n gently fills $p from $P.", TRUE, ch, to_obj, from_obj, TO_ROOM);
  }
  /* New alias */
  if (GET_OBJ_VAL(to_obj, 1) == 0)
    name_to_drinkcon(to_obj, GET_OBJ_VAL(from_obj, 2));

  /* First same type liq. */
  GET_OBJ_VAL(to_obj, 2) = GET_OBJ_VAL(from_obj, 2);

  /* Then how much to pour */
  GET_OBJ_VAL(from_obj, 1) -= (amount =
			 (GET_OBJ_VAL(to_obj, 0) - GET_OBJ_VAL(to_obj, 1)));

  GET_OBJ_VAL(to_obj, 1) = GET_OBJ_VAL(to_obj, 0);

  if (GET_OBJ_VAL(from_obj, 1) < 0) {	/* There was too little */
    GET_OBJ_VAL(to_obj, 1) += GET_OBJ_VAL(from_obj, 1);
    amount += GET_OBJ_VAL(from_obj, 1);
    GET_OBJ_VAL(from_obj, 1) = 0;
    GET_OBJ_VAL(from_obj, 2) = 0;
    GET_OBJ_VAL(from_obj, 3) = 0;
    name_from_drinkcon(from_obj);
  }
  /* Then the poison boogie */
  GET_OBJ_VAL(to_obj, 3) =
    (GET_OBJ_VAL(to_obj, 3) || GET_OBJ_VAL(from_obj, 3));

  /* And the weight boogie */
  weight_change_object(from_obj, -amount);
  weight_change_object(to_obj, amount);	/* Add weight */

  return;
}



void wear_message(struct char_data * ch, struct obj_data * obj, int where)
{
  const char *wear_messages[][2] = {
    {"$n lights $p and holds it.",
    "You light $p and hold it."},

    {"$n slides $p on to $s left ring finger.",
    "You slide $p on to your left ring finger."},

    {"$n slides $p on to $s right ring finger.",
    "You slide $p on to your right ring finger."},

    {"$n wears $p around $s neck.",
    "You wear $p around your neck."},

    {"$n wears $p around $s neck.",
    "You wear $p around your neck."},

    {"$n wears $p on $s body.",
    "You wear $p on your body.",},

    {"$n wears $p on $s head.",
    "You wear $p on your head."},

    {"$n puts $p on $s legs.",
    "You put $p on your legs."},

    {"$n wears $p on $s feet.",
    "You wear $p on your feet."},

    {"$n puts $p on $s hands.",
    "You put $p on your hands."},

    {"$n wears $p on $s arms.",
    "You wear $p on your arms."},

    {"$n fastens $p on $s right eye.",
    "You fasten $p on your eye."},

    {"$n wears $p about $s body.",
    "You wear $p around your body."},

    {"$n wears $p around $s waist.",
    "You wear $p around your waist."},

    {"$n puts $p on around $s right wrist.",
    "You put $p on around your right wrist."},

    {"$n puts $p on around $s left wrist.",
    "You put $p on around your left wrist."},

    {"$n wields $p.",
    "You wield $p."},

    {"$n grabs $p.",
    "You grab $p."},

    {"$n float $p over $s head.",
    "You float $p over your head."},

    {"$n wears $p on $s tail!",
    "You wear $p on your tail!"},

    {"$n wears $p on $s left ear.",
    "You wear $p on your left ear."},

    {"$n wears $p on $s right ear.",
    "You wear $p on your right ear."},

    {"$n screams in agony as $p bores into $s forehead!",
     "You scream in agony as $p bores into your forehead!"},

    {"$n clips $p onto $s right shoulder.",
     "You clip $p onto your right shoulder."},
    
    {"$n slides $p onto $s forelock.",
     "You slide $p onto your forelock."},
   
    {"$n dual wields $p.",
     "You dual wield $p."},

  };

  act(wear_messages[where][0], TRUE, ch, obj, 0, TO_ROOM);
  act(wear_messages[where][1], FALSE, ch, obj, 0, TO_CHAR);
}



void perform_wear(struct char_data * ch, struct obj_data * obj, int where)
{
  /*
   * ITEM_WEAR_TAKE is used for objects that do not require special bits
   * to be put into that position (e.g. you can hold any object, not just
   * an object with a HOLD bit.)
   */

  int wear_bitvectors[] = {
    ITEM_WEAR_TAKE, ITEM_WEAR_FINGER, ITEM_WEAR_FINGER, 
    ITEM_WEAR_NECK, ITEM_WEAR_NECK, ITEM_WEAR_BODY,
    ITEM_WEAR_HEAD, ITEM_WEAR_LEGS, ITEM_WEAR_FEET,
    ITEM_WEAR_HANDS, ITEM_WEAR_ARMS, ITEM_WEAR_SHIELD,
    ITEM_WEAR_ABOUT, ITEM_WEAR_WAIST, ITEM_WEAR_WRIST,
    ITEM_WEAR_WRIST, ITEM_WEAR_WIELD, ITEM_WEAR_TAKE,
    ITEM_WEAR_FLOAT, ITEM_WEAR_AURA, ITEM_WEAR_LEAR, 
    ITEM_WEAR_REAR, ITEM_WEAR_FORE, ITEM_WEAR_WEDDING, 
    ITEM_WEAR_FORELOCK, ITEM_WEAR_DUAL 
  };

  const char *already_wearing[] = {
    "You're already using a light.\r\n",
    "YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\r\n",
    "You're already wearing something on both of your ring fingers.\r\n",
    "YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\r\n",
    "You can't wear anything else around your neck.\r\n",
    "You're already wearing something on your body.\r\n",
    "You're already wearing something on your head.\r\n",
    "You're already wearing something on your legs.\r\n",
    "You're already wearing something on your feet.\r\n",
    "You're already wearing something on your hands.\r\n",
    "You're already wearing something on your arms.\r\n",
    "You're already using a scouter.\r\n",
    "You're already wearing something about your body.\r\n",
    "You already have something around your waist.\r\n",
    "YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\r\n",
    "You're already wearing something around both of your wrists.\r\n",
    "You're already wielding a weapon.\r\n",
    "You're already holding something.\r\n",
    "You're already floating something over your head.\r\n",
    "You already have something on your tail.\r\n",
    "You already have a left earring.\r\n",
    "You already have a right earring.\r\n",
    "You already have something bored into your head!\r\n"
    "You already have a Clan Insignia.\r\n",
    "You already have a Forelock.\r\n",
    "You already are dual wielding!\r\n",

  };

  /* first, make sure that the wear position is valid. */
  if (!CAN_WEAR(obj, wear_bitvectors[where])) {
    act("You can't wear $p there.", FALSE, ch, obj, 0, TO_CHAR);
    return;
  }
  /* for neck, finger, and wrist, try pos 2 if pos 1 is already full */
  if ((where == WEAR_FINGER_R) || (where == WEAR_NECK_1) || (where == WEAR_WRIST_R))
    if (GET_EQ(ch, where))
      where++;

  if (GET_EQ(ch, where)) {
    send_to_char(already_wearing[where], ch);
    return;
  }

  if (!wear_otrigger(obj, ch, where))
    return;
  wear_message(ch, obj, where);
  obj_from_char(obj);
  equip_char(ch, obj, where);
}



int find_eq_pos(struct char_data * ch, struct obj_data * obj, char *arg)
{
  int where = -1;

  const char *keywords[] = {	/* \n to prevent explicit wearing. */
    "!RESERVED!",
    "finger",
    "!RESERVED!",
    "neck",
    "!RESERVED!",
    "body",
    "head",
    "legs",
    "feet",
    "hands",
    "arms",
    "scouter",
    "about",
    "waist",
    "wrist",
    "!RESERVED!",
    "!RESERVED!",
    "!RESERVED!",
    "float",
    "tail",
    "lear",
    "rear",
    "fore",
    "wedding",
    "forelock",
    "dual",
    "\n"
  };

  if (!arg || !*arg) {
    if (CAN_WEAR(obj, ITEM_WEAR_FINGER))      where = WEAR_FINGER_R;
    if (CAN_WEAR(obj, ITEM_WEAR_NECK))        where = WEAR_NECK_1;
    if (CAN_WEAR(obj, ITEM_WEAR_BODY))        where = WEAR_BODY;
    if (CAN_WEAR(obj, ITEM_WEAR_HEAD))        where = WEAR_HEAD;
    if (CAN_WEAR(obj, ITEM_WEAR_LEGS))        where = WEAR_LEGS;
    if (CAN_WEAR(obj, ITEM_WEAR_FEET))        where = WEAR_FEET;
    if (CAN_WEAR(obj, ITEM_WEAR_HANDS))       where = WEAR_HANDS;
    if (CAN_WEAR(obj, ITEM_WEAR_ARMS))        where = WEAR_ARMS;
    if (CAN_WEAR(obj, ITEM_WEAR_SHIELD))      where = WEAR_SHIELD;
    if (CAN_WEAR(obj, ITEM_WEAR_ABOUT))       where = WEAR_ABOUT;
    if (CAN_WEAR(obj, ITEM_WEAR_WAIST))       where = WEAR_WAIST;
    if (CAN_WEAR(obj, ITEM_WEAR_WRIST))       where = WEAR_WRIST_R;
    if (CAN_WEAR(obj, ITEM_WEAR_FLOAT))       where = WEAR_FLOAT;
    if (CAN_WEAR(obj, ITEM_WEAR_AURA) && (IS_icer(ch) || IS_saiyan(ch) || IS_HALF_BREED(ch) || IS_BIODROID(ch)))        where = WEAR_AURA;
    if (CAN_WEAR(obj, ITEM_WEAR_LEAR))        where = WEAR_LEAR;
    if (CAN_WEAR(obj, ITEM_WEAR_REAR))        where = WEAR_REAR;
    if (CAN_WEAR(obj, ITEM_WEAR_WEDDING))     where = WEAR_WEDDING;
    if (CAN_WEAR(obj, ITEM_WEAR_FORE) && IS_TRUFFLE(ch))        where = WEAR_FORE;
    if (CAN_WEAR(obj, ITEM_WEAR_FORELOCK) && IS_MAJIN(ch))        where = WEAR_FORELOCK;
  } else {
    if (((where = search_block(arg, keywords, FALSE)) < 0) ||
        (*arg=='!')) {
      sprintf(buf, "'%s'?  What part of your body is THAT?\r\n", arg);
      send_to_char(buf, ch);
      return -1;
    }
  }

  return where;
}



ACMD(do_wear)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  struct obj_data *obj, *next_obj;
  int where = -1, dotmode, items_worn = 0;

  two_arguments(argument, arg1, arg2);
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}

  if (!*arg1) {
    send_to_char("Wear what?\r\n", ch);
    return;
  }
  dotmode = find_all_dots(arg1);

  if (*arg2 && (dotmode != FIND_INDIV)) {
    send_to_char("You can't specify the same body location for more than one item!\r\n", ch);
    return;
  }
  if (dotmode == FIND_ALL) {
    for (obj = ch->carrying; obj; obj = next_obj) {
      next_obj = obj->next_content;
      if (CAN_SEE_OBJ(ch, obj) && (where = find_eq_pos(ch, obj, 0)) >= 0) {
	items_worn++;
	perform_wear(ch, obj, where);
      }
    }
    if (!items_worn)
      send_to_char("You don't seem to have anything wearable.\r\n", ch);
  } else if (dotmode == FIND_ALLDOT) {
    if (!*arg1) {
      send_to_char("Wear all of what?\r\n", ch);
      return;
    }
    if (!(obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
      sprintf(buf, "You don't seem to have any %ss.\r\n", arg1);
      send_to_char(buf, ch);
    } else
      while (obj) {
	next_obj = get_obj_in_list_vis(ch, arg1, obj->next_content);
	if ((where = find_eq_pos(ch, obj, 0)) >= 0)
		  perform_wear(ch, obj, where);
	else 
	  act("You can't wear $p.", FALSE, ch, obj, 0, TO_CHAR);
	obj = next_obj;
      }
  } else {
    if (!(obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
      sprintf(buf, "You don't seem to have %s %s.\r\n", AN(arg1), arg1);
      send_to_char(buf, ch);
    } else {
      if ((where = find_eq_pos(ch, obj, arg2)) >= 0) {
	perform_wear(ch, obj, where);
      }
      else if (!*arg2)
	act("You can't wear $p.", FALSE, ch, obj, 0, TO_CHAR);
    }
  }
}

ACMD(do_wield)
{
  struct obj_data *obj;

  one_argument(argument, arg);
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RARM) && PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
     }
  if (!*arg)
    send_to_char("Wield what?\r\n", ch);
  else if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
    sprintf(buf, "You don't seem to have %s %s.\r\n", AN(arg), arg);
    send_to_char(buf, ch);
  } else {
    if (GET_OBJ_WEIGHT(obj) > str_app[STRENGTH_APPLY_INDEX(ch)].wield_w)
      send_to_char("It's too heavy for you to use.\r\n", ch);
    if (!GET_SKILL(ch, SKILL_DUALWIELD) && GET_EQ(ch, WEAR_WIELD)) {
      send_to_char("You can't dual wield.\r\n", ch);
      return;
     }
    else if (!CAN_WEAR(obj, ITEM_WEAR_DUAL) && GET_EQ(ch, WEAR_WIELD)) {
      send_to_char("You can't wield that.\r\n", ch);
      return;
    }
    else if (GET_EQ(ch, WEAR_WIELD) && (CAN_WEAR(GET_EQ(ch, WEAR_WIELD), ITEM_WEAR_DUAL) && CAN_WEAR(obj, ITEM_WEAR_DUAL))) {
      perform_wear(ch, obj, WEAR_DUAL);
      return;
    }
    else if (!CAN_WEAR(obj, ITEM_WEAR_WIELD))
      send_to_char("You can't wield that.\r\n", ch);
    else if (GET_EQ(ch, WEAR_DUAL))
      send_to_char("Remove your dual wielded weapon fire..\r\n", ch);
    else if (CAN_WEAR(obj, ITEM_WEAR_WIELD)) {
      perform_wear(ch, obj, WEAR_WIELD);
      return;
    }
  }
}



ACMD(do_grab)
{
  struct obj_data *obj;

  one_argument(argument, arg);
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RARM) && PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
     }
  if (!*arg)
    send_to_char("Hold what?\r\n", ch);
  else if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
    sprintf(buf, "You don't seem to have %s %s.\r\n", AN(arg), arg);
    send_to_char(buf, ch);
  } else {
    if (GET_OBJ_TYPE(obj) == ITEM_LIGHT)
      perform_wear(ch, obj, WEAR_LIGHT);
    else {
      if (!CAN_WEAR(obj, ITEM_WEAR_HOLD) && GET_OBJ_TYPE(obj) != ITEM_STAFF && GET_OBJ_TYPE(obj) != ITEM_SENZU &&
	  GET_OBJ_TYPE(obj) != ITEM_SAIBAKIT)
	send_to_char("You can't hold that.\r\n", ch);
      else
	perform_wear(ch, obj, WEAR_HOLD);
    }
  }
}



void perform_remove(struct char_data * ch, int pos)
{
  struct obj_data *obj;
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}

  if (!(obj = GET_EQ(ch, pos)))
    log("Error in perform_remove: bad pos %d passed.", pos);
  else if (IS_OBJ_STAT(obj, ITEM_NODROP))
    act("You can't remove $p, it must be CURSED!", FALSE, ch, obj, 0, TO_CHAR);
  else if (IS_CARRYING_N(ch) >= 400)
    act("$p: you can't carry that many items!", FALSE, ch, obj, 0, TO_CHAR);
  else {
    if (!remove_otrigger(obj, ch))
      return;
    obj_to_char(unequip_char(ch, pos), ch);
    act("You stop using $p.", FALSE, ch, obj, 0, TO_CHAR);
    act("$n stops using $p.", TRUE, ch, obj, 0, TO_ROOM);
  }
}



ACMD(do_remove)
{
  struct obj_data *obj;
  int i, dotmode, found;

  one_argument(argument, arg);
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}

  if (!*arg) {
    send_to_char("Remove what?\r\n", ch);
    return;
  }
  dotmode = find_all_dots(arg);

  if (dotmode == FIND_ALL) {
    found = 0;
    for (i = 0; i < NUM_WEARS; i++)
      if (GET_EQ(ch, i)) {
	perform_remove(ch, i);
	found = 1;
      }
    if (!found)
      send_to_char("You're not using anything.\r\n", ch);
  } else if (dotmode == FIND_ALLDOT) {
    if (!*arg)
      send_to_char("Remove all of what?\r\n", ch);
    else {
      found = 0;
      for (i = 0; i < NUM_WEARS; i++)
         if (GET_EQ(ch, i) && CAN_SEE_OBJ(ch, GET_EQ(ch, i)) &&
	    isname(arg, GET_EQ(ch, i)->name)) {
	  perform_remove(ch, i);
	  found = 1;
	}
      if (!found) {
	sprintf(buf, "You don't seem to be using any %ss.\r\n", arg);
	send_to_char(buf, ch);
      }
    }
  } else {
    if (!(obj = get_object_in_equip_vis(ch, arg, ch->equipment, &i))) {
      sprintf(buf, "You don't seem to be using %s %s.\r\n", AN(arg), arg);
      send_to_char(buf, ch);
    } else
      perform_remove(ch, i);
  }
}
