/* ************************************************************************
*   File: act.informative.c                             Part of CircleMUD *
*  Usage: Player-level commands of an informative nature                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

//#include "clan.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "screen.h"
#include "constants.h"
#include "dg_scripts.h"

/* extern variables */
extern int top_of_helpt;
extern struct help_index_element *help_table;
extern char *help;
extern struct time_info_data time_info;
extern const char *weekdays[];
extern const char *month_name[];
extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern struct char_data *character_list;
extern struct obj_data *object_list;
extern int top_of_world;
extern struct player_index_element *player_table;
extern int top_of_p_table;

extern char *credits;
extern char *news;
extern char *info;
extern char *motd;
extern char *imotd;
extern char *wizlist;
extern char *immlist;
extern char *policies;
extern char *handbook;
extern struct spell_entry spells[];
extern char *class_abbrevs[];
extern char *class_abbrevs2[];

/* extern functions */
ACMD(do_action);
long find_class_bitvector(char arg);
long long level_exp(int chclass, long long level);
char *title_male(int chclass, int level);
char *title_female(int chclass, int level);
extern struct help_index_element *help_table;

struct time_info_data *real_time_passed(time_t t2, time_t t1);

/* local functions */
void print_object_location(int num, struct obj_data * obj, struct char_data * ch, int recur);
void show_obj_to_char(struct obj_data * object, struct char_data * ch, int mode);
void list_obj_to_char(struct obj_data * list, struct char_data * ch, int mode, int show);
ACMD(do_look);
ACMD(do_examine);
ACMD(do_gold);
ACMD(do_score);
ACMD(do_inventory);
ACMD(do_equipment);
ACMD(do_time);
ACMD(do_weather);
ACMD(do_pushup);
ACMD(do_situp);
ACMD(do_help);
ACMD(do_who);
ACMD(do_finger);
ACMD(do_users);
ACMD(do_gen_ps);
void perform_mortal_where(struct char_data * ch, char *arg);
void perform_immort_where(struct char_data * ch, char *arg);
ACMD(do_where);
ACMD(do_levels);
ACMD(do_consider);
ACMD(do_diagnose);
ACMD(do_affects);
ACMD(do_color);
ACMD(do_toggle);
void sort_commands(void);
ACMD(do_commands);
void diag_char_to_char(struct char_data * i, struct char_data * ch);
void look_at_char(struct char_data * i, struct char_data * ch);
void list_one_char(struct char_data * i, struct char_data * ch);
void list_char_to_char(struct char_data * list, struct char_data * ch);
void do_auto_exits(struct char_data * ch);
ACMD(do_exits);
void look_in_direction(struct char_data * ch, int dir);
void look_in_obj(struct char_data * ch, char *arg);
char *find_exdesc(char *word, struct extra_descr_data * list);
void look_at_target(struct char_data * ch, char *arg);
ACMD(do_benchpress);

/*
 * This function screams bitvector... -gg 6/45/98
 */
void show_obj_to_char(struct obj_data * object, struct char_data * ch,
			int mode)
{
  bool found;

  *buf = '\0';
  if ((mode == 0) && object->description)
    strcpy(buf, object->description);
  else if (object->short_description && ((mode == 1) ||
				 (mode == 2) || (mode == 3) || (mode == 4)))
    strcpy(buf, object->short_description);
  else if (mode == 5) {
    if (GET_OBJ_TYPE(object) == ITEM_NOTE) {
      if (object->action_description) {
	strcpy(buf, "There is something written upon it:\r\n\r\n");
	strcat(buf, object->action_description);
	page_string(ch->desc, buf, 1);
      } else
	act("It's blank.", FALSE, ch, 0, 0, TO_CHAR);
      return;
    } else if (GET_OBJ_TYPE(object) != ITEM_DRINKCON) {
      strcpy(buf, "You see nothing special..");
    } else			/* ITEM_TYPE == ITEM_DRINKCON||FOUNTAIN */
      strcpy(buf, "It looks like a drink container.");
  }
  if (mode != 3) {
    found = FALSE;
    if (IS_OBJ_STAT(object, ITEM_INVISIBLE)) {
      strcat(buf, " (invisible)");
      found = TRUE;
    }
    if (IS_OBJ_STAT(object, ITEM_ZANZOKEN) && AFF_FLAGGED(ch, AFF_DETECT_ALIGN)) {
      strcat(buf, " ..It glows blue!");
      found = TRUE;
    }
    if (IS_OBJ_STAT(object, ITEM_MAGIC) && AFF_FLAGGED(ch, AFF_DETECT_MAGIC)) {
      strcat(buf, " ..It glows yellow!");
      found = TRUE;
    }
    if (IS_OBJ_STAT(object, ITEM_GLOW)) {
      strcat(buf, " (&03Glowing&00)");
      found = TRUE;
    }
    if (IS_OBJ_STAT(object, ITEM_HUM)) {
      strcat(buf, " (&05Humming&00)");
      found = TRUE;
    }
  }
  strcat(buf, "\r\n");
  page_string(ch->desc, buf, TRUE);
}


void list_obj_to_char(struct obj_data * list, struct char_data * ch, int mode,
		           int show)
{
  struct obj_data *i;
  bool found;

  found = FALSE;
  for (i = list; i; i = i->next_content) {
    if (CAN_SEE_OBJ(ch, i)) {
      show_obj_to_char(i, ch, mode);
      found = TRUE;
    }
  }
  if (!found && show)
    send_to_char(" Nothing.\r\n", ch);
}


void diag_char_to_char(struct char_data * i, struct char_data * ch)
{
  int percent;

  if (GET_MAX_HIT(i) > 0)
    percent = (100 * GET_HIT(i)) / GET_MAX_HIT(i);
  else
    percent = -1;		/* How could MAX_HIT be < 1?? */

  strcpy(buf, PERS(i, ch));
  CAP(buf);

  if (percent >= 100)
    strcat(buf, " is in excellent condition.\r\n");
  else if (GET_LEVEL(i) > 1000000)
    strcat(buf, " is in excellent condition.\r\n");
  else if (percent >= 90)
    strcat(buf, " has a few scratches.\r\n");
  else if (percent >= 75)
    strcat(buf, " has some small wounds and bruises.\r\n");
  else if (percent >= 50)
    strcat(buf, " has quite a few wounds.\r\n");
  else if (percent >= 30)
    strcat(buf, " has some big nasty wounds and scratches.\r\n");
  else if (percent >= 15)
    strcat(buf, " looks pretty hurt.\r\n");
  else if (percent >= 0)
    strcat(buf, " is in awful condition.\r\n");
  else
    strcat(buf, " is bleeding awfully from big wounds.\r\n");

  send_to_char(buf, ch);
}


void look_at_char(struct char_data * i, struct char_data * ch)
{
  int j, found;
  struct obj_data *tmp_obj;

  if (!ch->desc)
    return;

   if (i->player.description)
    send_to_char(i->player.description, ch);
  else
    act("You see nothing special about $m.", FALSE, i, 0, ch, TO_VICT);

  diag_char_to_char(i, ch);

  found = FALSE;
  for (j = 0; !found && j < NUM_WEARS; j++)
    if (GET_EQ(i, j) && CAN_SEE_OBJ(ch, GET_EQ(i, j)))
      found = TRUE;

  if (found) {
    act("\r\n$n is using:", FALSE, i, 0, ch, TO_VICT);
    for (j = 0; j < NUM_WEARS; j++)
      if (GET_EQ(i, j) && CAN_SEE_OBJ(ch, GET_EQ(i, j))) {
	send_to_char(where[j], ch);
	show_obj_to_char(GET_EQ(i, j), ch, 1);
      }
  }
  if (ch != i && (IS_icer(ch) || GET_LEVEL(ch) >= LVL_IMMORT)) {
    found = FALSE;
    act("\r\nYou attempt to peek at $s inventory:", FALSE, i, 0, ch, TO_VICT);
    for (tmp_obj = i->carrying; tmp_obj; tmp_obj = tmp_obj->next_content) {
      if (CAN_SEE_OBJ(ch, tmp_obj) && (number(0, 20) < GET_LEVEL(ch))) {
	show_obj_to_char(tmp_obj, ch, 1);
	found = TRUE;
      }
    }

    if (!found)
      send_to_char("You can't see anything.\r\n", ch);
  }
}


void list_one_char(struct char_data * i, struct char_data * ch)
{

struct player_special_data;

  const char *positions[] = {
    " is lying here, dead.",
    " is lying here, mortally wounded.",
    " is lying here, incapacitated.",
    " is lying here, stunned.",
    " is sleeping here.",
    " is resting here.",
    " is sitting here.",
    "!FIGHTING!",
    " is standing here.",
    " is floating here."
  };

  if (IS_NPC(i) && i->player.long_descr && GET_POS(i) == GET_DEFAULT_POS(i)) {
    if (AFF_FLAGGED(i, AFF_INVISIBLE))
      strcpy(buf, "*");
    else
      *buf = '\0';

    if (AFF_FLAGGED(ch, AFF_DETECT_ALIGN)) {
      if (IS_EVIL(i))
	strcat(buf, "(Red Aura) ");
      else if (IS_GOOD(i))
	strcat(buf, "(Blue Aura) ");
    }
    strcat(buf, i->player.long_descr);
    send_to_char(buf, ch);

    if (AFF_FLAGGED(i, AFF_BARRIER))
      act("&12...$e is engulfed in a globe of energy.&00", FALSE, i, 0, ch, TO_VICT);
    if (AFF_FLAGGED(i, AFF_SOLAR_FLARE))
      act("...$e is groping around blindly!", FALSE, i, 0, ch, TO_VICT);
    return;
  }
  if (IS_NPC(i)) {
    strcpy(buf, i->player.short_descr);
    CAP(buf);
  }

  else if (PLR_FLAGGED(i, PLR_trans4) && (IS_saiyan(i) || IS_HALF_BREED(i)))
//    sprintf(buf, "&14%s &15the %s %s %s &15with &16black &15eyes lined in &09red&15, long, &16black &15hair, and a &16black&15 aura", i->player.name, build[(int) i->player.build], genders[(int) i->player.sex], class_abbrevs[(int) i->player.chclass] );  
    sprintf(buf, "&14%s &15%s &16(&09SSJ4&16)&15", i->player.name, i->player.title);
  else if (PLR_FLAGGED(i, PLR_trans3) && (IS_saiyan(i) || IS_HALF_BREED(i)))
//    sprintf(buf, "&14%s &15the %s %s %s &15with&10 emerald &15eyes, waist-length &11golden &15hair, and a &11golden&15 aura", i->player.name, build[(int) i->player.build], genders[(int) i->player.sex], class_abbrevs[(int) i->player.chclass] );
    sprintf(buf, "&14%s &15%s &16(&11SSJ3&16)&15", i->player.name, i->player.title);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_saiyan(i) || IS_HALF_BREED(i)))
    sprintf(buf, "&14%s &15%s &16(&12SSJ2&16)&15", i->player.name, i->player.title);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)))
//    sprintf(buf, "&14%s &15the %s %s %s &15with&00&10 emerald&00 &15eyes, %s &11golden&00 &15hair, and a &11golden &15aura", i->player.name, build[(int) i->player.build], genders[(int) i->player.sex], class_abbrevs[(int) i->player.chclass], hl[(int) i->player.hairl] );    
    sprintf(buf, "&14%s &15%s &16(&03SSJ1&16)&15", i->player.name, i->player.title);
  else if (PRF_FLAGGED(i, PRF_MYSTIC))
//    sprintf(buf, "&14%s &15the %s %s %s &15with %s &15eyes, %s %s &15hair and a silver &15aura", i->player.name, build[(int) i->player.build], genders[(int) i->player.sex], class_abbrevs[(int) i->player.chclass], eyes[(int) i->player.eye], hl[(int) i->player.hairl], hc[(int) i->player.hairc] );    
    sprintf(buf, "&14%s &15%s &16(&14Mystic&16)&15", i->player.name, i->player.title);
//  else if (GET_LEVEL(ch) == LVL_IMPL)
//    sprintf(buf, "&14%s&00 %s&15", i->player.name, i->player.title);
  else
//    sprintf(buf, "&14%s &15the %s %s %s &15with %s &15eyes, %s %s &15hair and a %s &15aura", i->player.name, build[(int) i->player.build], genders[(int) i->player.sex], class_abbrevs[(int) i->player.chclass], eyes[(int) i->player.eye], hl[(int) i->player.hairl], hc[(int) i->player.hairc], aura[(int) i->player.aura] );
    sprintf(buf, "&14%s &15%s", i->player.name, i->player.title);

  if (AFF_FLAGGED(i, AFF_INVISIBLE))
    strcat(buf, " (invisible)");
  if (AFF_FLAGGED(i, AFF_HIDE))
    strcat(buf, " (hidden)");
  if (!IS_NPC(i) && !i->desc)
    strcat(buf, " (linkless)");
  if (PLR_FLAGGED(i, PLR_WRITING))
    strcat(buf, " (writing)");

  if (GET_POS(i) != POS_FIGHTING)
    strcat(buf, positions[(int) GET_POS(i)]);
  else {
    if (FIGHTING(i)) {
      strcat(buf, " is here, fighting ");
      if (FIGHTING(i) == ch)
	strcat(buf, "YOU!");
      else {
	if (i->in_room == FIGHTING(i)->in_room)
	  strcat(buf, PERS(FIGHTING(i), ch));
	else
	  strcat(buf, "someone who has already left");
	strcat(buf, "!");
      }
    } else			/* NIL fighting pointer */
      strcat(buf, " is here struggling with thin air.");
  }

  if (AFF_FLAGGED(ch, AFF_DETECT_ALIGN)) {
    if (IS_EVIL(i))
      strcat(buf, " (Red Aura)");
    else if (IS_GOOD(i))
      strcat(buf, " (Blue Aura)");
  }
  strcat(buf, "\r\n");
  send_to_char(buf, ch);

  if (AFF_FLAGGED(i, AFF_BARRIER))
    act("&12...$e is engulfed in a globe of energy.&00", FALSE, i, 0, ch, TO_VICT);
}



void list_char_to_char(struct char_data * list, struct char_data * ch)
{
  struct char_data *i;

  for (i = list; i; i = i->next_in_room)
    if (ch != i) {
      if (CAN_SEE(ch, i))
	list_one_char(i, ch);
      else if (IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch) &&
	       AFF_FLAGGED(i, AFF_INFRAVISION))
	send_to_char("You see a pair of glowing red eyes looking your way.\r\n", ch);
    }
}


void do_auto_exits(struct char_data * ch)
{
  int door;

  *buf = '\0';

  for (door = 0; door < NUM_OF_DIRS; door++)
    if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE &&
	!EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED)) {
      if (GET_LEVEL(ch) >= LVL_IMMORT)
	sprintf(buf2, "%-5s &09- &10[&11%5d&10 ] &15%s&15\r\n", dirs[door],
		GET_ROOM_VNUM(EXIT(ch, door)->to_room),
		world[EXIT(ch, door)->to_room].name);
      else {
	sprintf(buf2, "%-5s&00 &09-&15 ", dirs[door]);
	if (IS_DARK(EXIT(ch, door)->to_room) && !CAN_SEE_IN_DARK(ch))
	  strcat(buf2, "");
	else {
	  strcat(buf2, world[EXIT(ch, door)->to_room].name);
	  strcat(buf2, "\r\n");
	}
      }
      strcat(buf, CAP(buf2));
    }
  send_to_char("&15Obvious exits:&15\r\n", ch);

  if (*buf)
    send_to_char(buf, ch);
  else
    send_to_char(" &15None.&00\r\n", ch);
}

ACMD(do_exits)
{
  int door;

  *buf = '\0';

  if (AFF_FLAGGED(ch, AFF_SOLAR_FLARE)) {
    send_to_char("You can't see a damned thing, you're SOLAR_FLARE!\r\n", ch);
    return;
  }
  for (door = 0; door < NUM_OF_DIRS; door++)
    if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE &&
	!EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED)) {
      if (GET_LEVEL(ch) >= LVL_IMMORT)
	sprintf(buf2, "%-5s - [%5d] %s\r\n", dirs[door],
		GET_ROOM_VNUM(EXIT(ch, door)->to_room),
		world[EXIT(ch, door)->to_room].name);
      else {
	sprintf(buf2, "%-5s - ", dirs[door]);
	if (IS_DARK(EXIT(ch, door)->to_room) && !CAN_SEE_IN_DARK(ch))
	  strcat(buf2, "Too dark to tell\r\n");
	else {
	  strcat(buf2, world[EXIT(ch, door)->to_room].name);
	  strcat(buf2, "\r\n");
	}
      }
      strcat(buf, CAP(buf2));
    }
  send_to_char("Obvious exits:\r\n", ch);

  if (*buf)
    send_to_char(buf, ch);
  else
    send_to_char(" None.\r\n", ch);
}



void look_at_room(struct char_data * ch, int ignore_brief)
{
  if (!ch->desc)
    return;

  if (IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch)) {
    send_to_char("It is pitch black...\r\n", ch);
    return;
  } else if (AFF_FLAGGED(ch, AFF_SOLAR_FLARE)) {
    send_to_char("You see nothing but infinite darkness...\r\n", ch);
    return;
  }
  send_to_char(CCCYN(ch, C_NRM), ch);
  if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_ROOMFLAGS)) {
    sprintbit(ROOM_FLAGS(ch->in_room), room_bits, buf);
    sprintf(buf2, "[%5d] [ %s]\r\n", GET_ROOM_VNUM(IN_ROOM(ch)), buf);
    send_to_char(buf2, ch);
   
  } else if (!PRF_FLAGGED(ch, PRF_ROOMFLAGS))
  send_to_char(world[ch->in_room].name, ch);
  send_to_char(CCNRM(ch, C_NRM), ch);
  send_to_char("\r\n", ch);
  
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10)) {
  send_to_char("&15Gravity:&11 10 times normal gravity.&00\r\n", ch);
  }
  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100)) {
  send_to_char("&15Gravity:&11 100 times normal gravity.&00\r\n", ch);
  
  }
  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200)) {
  send_to_char("&15Gravity:&11 200 times normal gravity.&00\r\n", ch);
  
  }
  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300)) {
  send_to_char("&15Gravity:&11 300 times normal gravity.&00\r\n", ch);
  
  }
  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400)) {
  send_to_char("&15Gravity:&11 400 times normal gravity.&00\r\n", ch);
  
  }
  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_SPIRIT)) {
  send_to_char("&15Gravity:&11 Unknown.&00\r\n", ch);
  
  }
  else
  send_to_char("&15Gravity:&11 Normal.&00\r\n", ch);
  
  
  if ((!IS_NPC(ch) && !PRF_FLAGGED(ch, PRF_BRIEF)) || ignore_brief ||
      ROOM_FLAGGED(ch->in_room, ROOM_DEATH))
    send_to_char(world[ch->in_room].description, ch);

  /* autoexits */
  if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_AUTOEXIT))
    do_auto_exits(ch);

  /* now list characters & objects */
  send_to_char(CCGRN(ch, C_NRM), ch);
  list_obj_to_char(world[ch->in_room].contents, ch, 0, FALSE);
  send_to_char(CCYEL(ch, C_NRM), ch);
  list_char_to_char(world[ch->in_room].people, ch);
  send_to_char(CCNRM(ch, C_NRM), ch);
}



void look_in_direction(struct char_data * ch, int dir)
{
  if (EXIT(ch, dir)) {
    if (EXIT(ch, dir)->general_description)
      send_to_char(EXIT(ch, dir)->general_description, ch);
    else
      send_to_char("You don't see anything special.\r\n", ch);

    if (EXIT_FLAGGED(EXIT(ch, dir), EX_CLOSED) && EXIT(ch, dir)->keyword) {
      sprintf(buf, "The %s is closed.\r\n", fname(EXIT(ch, dir)->keyword));
      send_to_char(buf, ch);
    } else if (EXIT_FLAGGED(EXIT(ch, dir), EX_ISDOOR) && EXIT(ch, dir)->keyword) {
      sprintf(buf, "The %s is open.\r\n", fname(EXIT(ch, dir)->keyword));
      send_to_char(buf, ch);
    }
  } else
    send_to_char("Nothing special there...\r\n", ch);
}



void look_in_obj(struct char_data * ch, char *arg)
{
  struct obj_data *obj = NULL;
  struct char_data *dummy = NULL;
  int amt, bits;

  if (!*arg)
    send_to_char("Look in what?\r\n", ch);
  else if (!(bits = generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM |
				 FIND_OBJ_EQUIP, ch, &dummy, &obj))) {
    sprintf(buf, "There doesn't seem to be %s %s here.\r\n", AN(arg), arg);
    send_to_char(buf, ch);
  } else if ((GET_OBJ_TYPE(obj) != ITEM_DRINKCON) &&
	     (GET_OBJ_TYPE(obj) != ITEM_FOUNTAIN) &&
	     (GET_OBJ_TYPE(obj) != ITEM_CONTAINER))
    send_to_char("There's nothing inside that!\r\n", ch);
  else {
    if (GET_OBJ_TYPE(obj) == ITEM_CONTAINER) {
      if (OBJVAL_FLAGGED(obj, CONT_CLOSED))
	send_to_char("It is closed.\r\n", ch);
      else {
	send_to_char(fname(obj->name), ch);
	switch (bits) {
	case FIND_OBJ_INV:
	  send_to_char(" (carried): \r\n", ch);
	  break;
	case FIND_OBJ_ROOM:
	  send_to_char(" (here): \r\n", ch);
	  break;
	case FIND_OBJ_EQUIP:
	  send_to_char(" (used): \r\n", ch);
	  break;
	}

	list_obj_to_char(obj->contains, ch, 2, TRUE);
      }
    } else {		/* item must be a fountain or drink container */
      if (GET_OBJ_VAL(obj, 1) <= 0)
	send_to_char("It is empty.\r\n", ch);
      else {
	if (GET_OBJ_VAL(obj,0) <= 0 || GET_OBJ_VAL(obj,1)>GET_OBJ_VAL(obj,0)) {
	  sprintf(buf, "Its contents seem somewhat murky.\r\n"); /* BUG */
	} else {
	  amt = (GET_OBJ_VAL(obj, 1) * 3) / GET_OBJ_VAL(obj, 0);
	  sprinttype(GET_OBJ_VAL(obj, 2), color_liquid, buf2);
	  sprintf(buf, "It's %sfull of a %s liquid.\r\n", fullness[amt], buf2);
	}
	send_to_char(buf, ch);
      }
    }
  }
}



char *find_exdesc(char *word, struct extra_descr_data * list)
{
  struct extra_descr_data *i;

  for (i = list; i; i = i->next)
    if (isname(word, i->keyword))
      return (i->description);

  return NULL;
}


/*
 * Given the argument "look at <target>", figure out what object or char
 * matches the target.  First, see if there is another char in the room
 * with the name.  Then check local objs for exdescs.
 */

/*
 * BUG BUG: If fed an argument like '2.bread', the extra description
 *          search will fail when it works on 'bread'!
 * -gg 6/24/98 (I'd do a fix, but it's late and non-critical.)
 */
void look_at_target(struct char_data * ch, char *arg)
{
  int bits, found = FALSE, j;
  struct char_data *found_char = NULL;
  struct obj_data *obj = NULL, *found_obj = NULL;
  char *desc;

  if (!ch->desc)
    return;

  if (!*arg) {
    send_to_char("Look at what?\r\n", ch);
    return;
  }
  bits = generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP |
		      FIND_CHAR_ROOM, ch, &found_char, &found_obj);

  /* Is the target a character? */
  if (found_char != NULL) {
    look_at_char(found_char, ch);
    if (ch != found_char) {
      if (CAN_SEE(found_char, ch))
	act("$n looks at you.", TRUE, ch, 0, found_char, TO_VICT);
      act("$n looks at $N.", TRUE, ch, 0, found_char, TO_NOTVICT);
    }
    return;
  }
  /* Does the argument match an extra desc in the room? */
  if ((desc = find_exdesc(arg, world[ch->in_room].ex_description)) != NULL) {
    page_string(ch->desc, desc, FALSE);
    return;
  }
  /* Does the argument match an extra desc in the char's equipment? */
  for (j = 0; j < NUM_WEARS && !found; j++)
    if (GET_EQ(ch, j) && CAN_SEE_OBJ(ch, GET_EQ(ch, j)))
      if ((desc = find_exdesc(arg, GET_EQ(ch, j)->ex_description)) != NULL) {
	send_to_char(desc, ch);
	found = TRUE;
      }
  /* Does the argument match an extra desc in the char's inventory? */
  for (obj = ch->carrying; obj && !found; obj = obj->next_content) {
    if (CAN_SEE_OBJ(ch, obj))
      if ((desc = find_exdesc(arg, obj->ex_description)) != NULL) {
	send_to_char(desc, ch);
	found = TRUE;
      }
  }

  /* Does the argument match an extra desc of an object in the room? */
  for (obj = world[ch->in_room].contents; obj && !found; obj = obj->next_content)
    if (CAN_SEE_OBJ(ch, obj))
      if ((desc = find_exdesc(arg, obj->ex_description)) != NULL) {
	send_to_char(desc, ch);
	found = TRUE;
      }

  /* If an object was found back in generic_find */
  if (bits) {
    if (!found)
      show_obj_to_char(found_obj, ch, 5);	/* Show no-description */
    else
      show_obj_to_char(found_obj, ch, 6);	/* Find hum, glow etc */
  } else if (!found)
    send_to_char("You do not see that here.\r\n", ch);
}


ACMD(do_look)
{
  char arg2[MAX_INPUT_LENGTH];
  int look_type;
  if (!ch->desc)
    return;

  if (GET_POS(ch) < POS_SLEEPING)
    send_to_char("You can't see anything but stars!\r\n", ch);
  else if (AFF_FLAGGED(ch, AFF_SOLAR_FLARE))
    send_to_char("You can't see a damned thing, you're blind!\r\n", ch);
  else if (IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch)) {
    send_to_char("It is pitch black...\r\n", ch);
    list_char_to_char(world[ch->in_room].people, ch);	/* glowing red eyes */
  } else {
    half_chop(argument, arg, arg2);

    if (subcmd == SCMD_READ) {
      if (!*arg)
	send_to_char("Read what?\r\n", ch);
      else
	look_at_target(ch, arg);
      return;
    }
      if (!*arg)			/* "look" alone, without an argument at all */

      look_at_room(ch, 1);
    else if (is_abbrev(arg, "in"))
      look_in_obj(ch, arg2);
    /* did the char type 'look <direction>?' */
    else if ((look_type = search_block(arg, dirs, FALSE)) >= 0)
      look_in_direction(ch, look_type);
    else if (is_abbrev(arg, "at"))
      look_at_target(ch, arg2);
    else
      look_at_target(ch, arg);
  }
}


ACMD(do_examine)
{
  int bits;
  struct char_data *tmp_char;
  struct obj_data *tmp_object;

  one_argument(argument, arg);

  if (!*arg) {
    send_to_char("Examine what?\r\n", ch);
    return;
  }
  look_at_target(ch, arg);

  bits = generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_CHAR_ROOM |
		      FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);

  if (tmp_object) {
    if ((GET_OBJ_TYPE(tmp_object) == ITEM_DRINKCON) ||
	(GET_OBJ_TYPE(tmp_object) == ITEM_FOUNTAIN) ||
	(GET_OBJ_TYPE(tmp_object) == ITEM_CONTAINER)) {
      send_to_char("When you look inside, you see:\r\n", ch);
      look_in_obj(ch, arg);
    }
  }
}



ACMD(do_gold)
{
  if (GET_GOLD(ch) == 0)
    send_to_char("You're broke!\r\n", ch);
  else if (GET_GOLD(ch) == 1)
    send_to_char("You have one miserable little gold coin.\r\n", ch);
  else {
    sprintf(buf, "You have %d gold coins.\r\n", GET_GOLD(ch));
    send_to_char(buf, ch);
  }
}

ACMD(do_score)
{
  sprintf(buf,"                    &14Information for:&00 &15%s&00\r\n", GET_NAME(ch));
  sprintf(buf, "%s&15o&12------------------------------------------------------------&15o&00\r\n", buf);
  sprintf(buf, "%s&10Level:&00 &15%Ld&00\r\n", buf, GET_LEVEL(ch));
  sprintf(buf, "%s&10Race:&00 %s     \r\n&10Sex: &00 ", buf,
               CLASS_ABBR(ch));
  switch (GET_SEX(ch)) {
    case SEX_MALE: strcat(buf, "&12Male&00\r\n"); break;
    case SEX_FEMALE: strcat(buf, "&13Female&00\r\n"); break;
    default: strcat(buf, "Neutral\r\n"); break;
  }
  sprintf(buf, "%s&09Health&15: &15%d&12/&15%d&00\r\n", buf, GET_HIT(ch), GET_MAX_HIT(ch));
  sprintf(buf, "%s&10Energy&15: &15%d&12/&15%d&00\r\n", buf, GET_MOVE(ch), GET_MAX_MOVE(ch));
  sprintf(buf, "%s&14Ki&15:     &15%d&12/&15%d&00\r\n", buf, GET_MANA(ch), GET_MAX_MANA(ch));
  sprintf(buf, "%s&09Maximum Powerlevel:&00 &15%Ld&00\r\n", buf, GET_STR(ch)*2+GET_MAX_MANA(ch)/4*GET_LEVEL(ch)+GET_DAMROLL(ch));
  sprintf(buf, "%s&09Current Powerlevel:&00 &15%Ld&00\r\n", buf, GET_STR(ch)*2+GET_MANA(ch)/4*GET_LEVEL(ch)+GET_DAMROLL(ch));
  sprintf(buf, "%s&12Strength&15:           &00 &15%d&00     &12Dexterity&15:       &00 &15%d&00\r\n", buf, GET_STR(ch), GET_DEX(ch));
  sprintf(buf, "%s&12Upper Body Strength&15:&00 &15%d&00     &12Constitution&15:    &00 &15%d&00\r\n", buf, GET_UBS(ch), GET_CON(ch));
  sprintf(buf, "%s&12Lower Body Strength&15:&00 &15%d&00     &12Hitroll&15:         &00 &15%d&00\r\n", buf, GET_LBS(ch), (ch)->points.hitroll);
  sprintf(buf, "%s&12Wisdom&15:&00              &15%d&00     &12Damroll&15:         &00 &15%d&00\r\n", buf,  GET_WIS(ch), (ch)->points.damroll);
  sprintf(buf, "%s&12Inteligence&15:&00         &15%d&00     &12Rage&15:            &00 &15%d&00\r\n", buf, GET_INT(ch), GET_RAGE(ch));
  sprintf(buf, "%s&12Age&15:&00                 &15%d&00     &12Height&15:          &00 &15%d&00\r\n", buf, GET_AGE(ch), GET_HEIGHT(ch));
  sprintf(buf, "%s&12Charisma&15:&00            &15%d&00\r\n", buf, GET_CHA(ch));
  sprintf(buf, "%s&12Zenni Carried&15: &00  &11%d zenni&00\r\n", buf, GET_GOLD(ch));
  sprintf(buf, "%s&12Zenni In bank&15: &00  &11%d zenni&00\r\n", buf, GET_BANK_GOLD(ch));
  sprintf(buf, "%s&12Experience&15:&00      &15%Ld&00\r\n", buf, GET_EXP(ch));
  sprintf(buf, "%s&12Experience TNL&15:&00  &15%Ld&00\r\n", buf, GET_LEVEL(ch)*2000-GET_EXP(ch));
  sprintf(buf, "%s&12Clan&15:&00       %s\r\n", buf, clan[(int) ch->player.clan]);
  sprintf(buf, "%s&12God&15:&00        %s\r\n", buf, god[(int) ch->player.god]);
  sprintf(buf, "%s&09See 'Status' for more information.&00\r\n", buf);
  sprintf(buf, "%s&11Armor Class&15:     &15%d&00\r\n", buf, GET_AC(ch));
  sprintf(buf, "%s&11Alignment&15:       &15%d&00\r\n", buf, GET_ALIGNMENT(ch));
  sprintf(buf, "%s&11Str+&15:            &15%d&00\r\n", buf, GET_ADD(ch));
  sprintf(buf, "%s&15o&12------------------------------------------------------------&15o&00\r\n", buf);

  send_to_char(buf, ch);
}


ACMD(do_inventory)
{
  send_to_char("&15You are carrying:&00\r\n", ch);
  list_obj_to_char(ch->carrying, ch, 1, TRUE);
}


ACMD(do_equipment)
{
  int i, found = 0;

  send_to_char("&15You are using:&00\r\n", ch);
  for (i = 0; i < NUM_WEARS; i++) {
    if (GET_EQ(ch, i)) {
      if (CAN_SEE_OBJ(ch, GET_EQ(ch, i))) {
	send_to_char(where[i], ch);
	show_obj_to_char(GET_EQ(ch, i), ch, 1);
	found = TRUE;
      } else {
	send_to_char(where[i], ch);
	send_to_char("Something.\r\n", ch);
	found = TRUE;
      }
    }
  }
  if (!found) {
    send_to_char(" Nothing.\r\n", ch);
  }
}


ACMD(do_time)
{
  const char *suf;
  int weekday, day;

  sprintf(buf, "It is %d o'clock %s, on ",
	  ((time_info.hours % 12 == 0) ? 12 : ((time_info.hours) % 12)),
	  ((time_info.hours >= 12) ? "pm" : "am"));

  /* 35 days in a month */
  weekday = ((35 * time_info.month) + time_info.day + 1) % 7;

  strcat(buf, weekdays[weekday]);
  strcat(buf, "\r\n");
  send_to_char(buf, ch);

  day = time_info.day + 1;	/* day in [1..35] */

  if (day == 1)
    suf = "st";
  else if (day == 2)
    suf = "nd";
  else if (day == 3)
    suf = "rd";
  else if (day < 20)
    suf = "th";
  else if ((day % 10) == 1)
    suf = "st";
  else if ((day % 10) == 2)
    suf = "nd";
  else if ((day % 10) == 3)
    suf = "rd";
  else
    suf = "th";

  sprintf(buf, "The %d%s Day of the %s, Year %d.\r\n",
	  day, suf, month_name[(int) time_info.month], time_info.year);

  send_to_char(buf, ch);
}


ACMD(do_weather)
{
  const char *sky_look[] = {
    "cloudless",
    "cloudy",
    "rainy",
    "lit by flashes of lightning"
  };

  if (OUTSIDE(ch)) {
    sprintf(buf, "The sky is %s and %s.\r\n", sky_look[weather_info.sky],
	    (weather_info.change >= 0 ? "you feel a warm wind from south" :
	     "your foot tells you bad weather is due"));
    send_to_char(buf, ch);
  } else
    send_to_char("You have no feeling about the weather at all.\r\n", ch);
}


struct help_index_element *find_help(char *keyword)
{
  int i;

  for (i = 0; i < top_of_helpt; i++)
    if (isname(keyword, help_table[i].keywords))
      return (help_table + i);

  return NULL;
}


ACMD(do_help)
{
  struct help_index_element *this_help;
  char entry[MAX_STRING_LENGTH];

  if (!ch->desc)
    return;

  skip_spaces(&argument);

  if (!*argument) {
    page_string(ch->desc, help, 0);
    return;
  }
  if (!help_table) {
    send_to_char("No help available.\r\n", ch);
    return;
  }

  if (!(this_help = find_help(argument))) {
      send_to_char("There is no help on that word.\r\n", ch);
    sprintf(buf, "HELP: %s tried to get help on %s", GET_NAME(ch), argument);
    log(buf);
      return;
    }

  if (this_help->min_level > GET_LEVEL(ch)) {
    send_to_char("There is no help on that word.\r\n", ch);
    return;
  }

  sprintf(entry, "%s\r\n%s", this_help->keywords, this_help->entry);
  
  page_string(ch->desc, entry, 0);

}

/*                                                          *
 * This hardly deserves it's own function, but I see future *
 * expansion capabilities...                                *
 *                                               TR 8-18-98 *
 *                                          Fixed:  8-25-98 */
int show_on_who_list(struct descriptor_data *d)
{
  if ((STATE(d) != CON_PLAYING) && (STATE(d) != CON_MEDIT) &&
	 (STATE(d) != CON_OEDIT) && (STATE(d) != CON_REDIT) &&
	 (STATE(d) != CON_SEDIT) && (STATE(d) != CON_ZEDIT) &&
	 (STATE(d) != CON_HEDIT) && (STATE(d) != CON_AEDIT) &&
	 (STATE(d) != CON_TEXTED) && (STATE(d) != CON_TRIGEDIT))
    return 0;
  else
    return 1;
}


#define WHO_FORMAT \
"format: who [minlev[-maxlev]] [-n name] [-c classlist] [-s] [-o] [-q] [-r] [-z]\r\n"

ACMD(do_who)
{
  struct descriptor_data *d;
  struct char_data *tch;
  char name_search[MAX_INPUT_LENGTH];
  char mode;
  size_t i;
  int low = 0, high = LVL_TUSKINO, localwho = 0, questwho = 0;
  int showclass = 0, short_list = 0, outlaws = 0, num_can_see = 0;
  int who_room = 0;

  skip_spaces(&argument);
  strcpy(buf, argument);
  name_search[0] = '\0';

  while (*buf) {
    half_chop(buf, arg, buf1);
    if (isdigit(*arg)) {
      sscanf(arg, "%d-%d", &low, &high);
      strcpy(buf, buf1);
    } else if (*arg == '-') {
      mode = *(arg + 1);       /* just in case; we destroy arg in the switch */
      switch (mode) {
      case 'o':
      case 'k':
	outlaws = 1;
	strcpy(buf, buf1);
	break;
      case 'z':
	localwho = 1;
	strcpy(buf, buf1);
	break;
      case 'q':
	questwho = 1;
	strcpy(buf, buf1);
	break;
      case 'l':
	half_chop(buf1, arg, buf);
	sscanf(arg, "%d-%d", &low, &high);
	break;
      case 'n':
	half_chop(buf1, name_search, buf);
	break;
      case 'r':
	who_room = 1;
	strcpy(buf, buf1);
	break;
      case 'c':
	half_chop(buf1, arg, buf);
	for (i = 0; i < strlen(arg); i++)
	  showclass |= find_class_bitvector(arg[i]);
	break;
      default:
	send_to_char(WHO_FORMAT, ch);
	return;
      }				/* end of switch */

    } else {			/* endif */
      send_to_char(WHO_FORMAT, ch);
      return;
    }
  }				/* end while (parser) */

  send_to_char("\r\n      &15Players on &11Drag&12(&09*&12)&11nball - &10Struggle &15for &09Power&00\r\n\r\n", ch);

  for (d = descriptor_list; d; d = d->next) {
    if (d->original)
      tch = d->original;
    else if (!(tch = d->character))
      continue;

    if (!show_on_who_list(d))
      continue;
    if (*name_search && str_cmp(GET_NAME(tch), name_search) &&
	!strstr(GET_TITLE(tch), name_search))
      continue;
    if (!CAN_SEE(ch, tch) || GET_LEVEL(tch) < low || GET_LEVEL(tch) > high)
      continue;
    if (outlaws && !PLR_FLAGGED(tch, PLR_KILLER) &&
	!PLR_FLAGGED(tch, PLR_icer))
      continue;
    if (questwho && !PRF_FLAGGED(tch, PRF_QUEST))
      continue;
    if (localwho && world[ch->in_room].zone != world[tch->in_room].zone)
      continue;
    if (who_room && (tch->in_room != ch->in_room))
      continue;
    if (showclass && !(showclass & (1 << GET_CLASS(tch))))
      continue;
    if (short_list) {
      sprintf(buf, "%s[%2Ld %s] %-12.12s%s%s",
	      (GET_LEVEL(tch) >= LVL_IMMORT ? CCYEL(ch, C_SPR) : ""),
	      GET_LEVEL(tch), CLASS_ABBR(tch), GET_NAME(tch),
	      (GET_LEVEL(tch) >= LVL_IMMORT ? CCNRM(ch, C_SPR) : ""),
	      ((!(++num_can_see % 4)) ? "\r\n" : ""));
      send_to_char(buf, ch);
    } else {
      num_can_see++;
      sprintf(buf, "&05-&03(&00%s&03)&05-&00 &03%s&00 %s",
	      CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch));
      if (GET_SEX(tch) == SEX_MALE)
//        sprintf(buf, "%s &16[&12M&16]&00   &06%s&00 %s&00 %s", 
        sprintf(buf, "&15[&00%s&15 ] {&12M&15}&00&06 %s &00%s",
 	      CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch));
//	      CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch), clan[(int) tch->player.clan]);
      if (GET_SEX(tch) == SEX_FEMALE)
//        sprintf(buf, "%s &16[&13F&16]&00   &06%s&00 %s&00 %s",
        sprintf(buf, "&15[&00%s&15 ] {&13F&15}&00 &05 %s &00%s",
 	      CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch));
//	      CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch), clan[(int) tch->player.clan]);
      
      /* Imms */
/*      if ((GET_LEVEL(tch) == LVL_IMMORT && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, "[&12Immortal&00     ]&15[&12M&15] &10%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_IMMORT && GET_SEX(tch) == SEX_FEMALE))
      	sprintf(buf, "[&12Immortal&00     ]&15[&13F&15] &10%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_LSSRGOD && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, "[&12Lesser God   &00]&15[&12M&15] &10%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_LSSRGOD && GET_SEX(tch) == SEX_FEMALE))
      	sprintf(buf, "[&12Lesser Goddess&00]&15[&13F&15] &10%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_GOD && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, "[&15God&00          ]&16[&12M&16] &10%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_GOD && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, "[&15Godess&00       ]&16[&13F&16] &10%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_ARCANE && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, "[&10Arcane&00       ]&15[&12M&15] &10%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_ARCANE && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, "[&10Arcane&00       ]&15[&13F&15] &10%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_GRGOD && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, "[&14Greater God&00  ]&15[&12M&15] &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_GRGOD && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, "[&14Gr. Goddess&00  ]&15[&13F&15] &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_COIMP && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, "[&11Co-Imp&00       ]&15[&12M&15] &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_COIMP && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, "[&11Co-Imp&00       ]&15[&13F&15] &12%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_IMPL && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, "[&09Implementer&00  ]&15[&12M&15] &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_IMPL && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, "[&09Implementress&00]&15[&13F&15] &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch)); */
/*My Who*/
/* Immortal */
      if ((GET_LEVEL(tch) == LVL_IMMORT) && GET_SEX(tch) == SEX_MALE)
        sprintf(buf, "&15[  &12Immortal&00   &15] &15{&12M&15} &25&14%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_IMMORT) && GET_SEX(tch) == SEX_FEMALE)
        sprintf(buf, "&15[  &12Immortal&00   &15] &15{&13F&15} &25&14%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
/* Lesser God */
      else if ((GET_LEVEL(tch) == LVL_LSSRGOD) && GET_SEX(tch) == SEX_MALE)
        sprintf(buf, "&15[ &05Lesser God&00  &15] &15{&12M&15} &25&14%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_LSSRGOD) && GET_SEX(tch) == SEX_FEMALE)
        sprintf(buf, "&15[ &05Lesser God&00  &15] &15(&13F&15) &25&14%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
/* God */
      else if ((GET_LEVEL(tch) == LVL_GOD) && GET_SEX(tch) == SEX_MALE)
        sprintf(buf, "&15[     &13God&00     &15] &15{&12M&15} &25&14%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_GOD) && GET_SEX(tch) == SEX_FEMALE)
        sprintf(buf, "&15[     &13God&00     &15] &15{&13F&15} &25&14%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
/* Creator */
      else if ((GET_LEVEL(tch) == LVL_ARCANE) && GET_SEX(tch) == SEX_MALE)
        sprintf(buf, "&15[   &11Creator&00   &15] &15{&12M&15} &25&14%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_ARCANE) && GET_SEX(tch) == SEX_FEMALE)
        sprintf(buf, "&15[   &11Creator&00   &15] &15{&13F&15} &25&14%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
/* Greater God */
      else if ((GET_LEVEL(tch) == LVL_GRGOD) && GET_SEX(tch) == SEX_MALE)
        sprintf(buf, "&15[ &14Greater God&00 &15] &15{&12M&15} &25&14%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_GRGOD) && GET_SEX(tch) == SEX_FEMALE)
        sprintf(buf, "&15[ &14Greater God&00 &15] &15{&13F&15} &25&14%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
/* Co-Imp */
      else if ((GET_LEVEL(tch) == LVL_COIMP) && GET_SEX(tch) == SEX_MALE)
        sprintf(buf, "&15[&14----&09Co-Imp&14---&15] &15{&12M&15} &25&14%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_COIMP) && GET_SEX(tch) == SEX_FEMALE)
        sprintf(buf, "&15[&14----&09Co-Imp&14---&15] &15{&13F&15} &25&15%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
/* IMP */
      else if ((GET_LEVEL(tch) == LVL_PIMPL) && GET_SEX(tch) == SEX_MALE)
        sprintf(buf, "&15[&09-----&16IMP&09-----&15] &15{&12M&15} &25&15%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch)); 
      else if ((GET_LEVEL(tch) == LVL_PIMPL) && GET_SEX(tch) == SEX_FEMALE)
        sprintf(buf, "&15[&09-----&16IMP&09-----&15] &15{&13F&15} &25&15%s&00&11 %s&00",
              GET_NAME(tch), GET_TITLE(tch));
/* Owner */
       if (!str_cmp(tch->player.name, "Tuskino"))
        sprintf(buf, "&15[&09    &09O&16w&09n&16e&09r&16    &15] &15{&12M&15} &16T&09u&16s&09k&16i&09n&16o &09K&16a&09m&16n&09i&00&11 &00 "
              );
       if (!str_cmp(tch->player.name, "Gohorin"))
        sprintf(buf, "&15[&09    &09K&10i&11t&12t&13y    &15] &15{&12M&15} &25&09T&10h&11e &12b&13e&14a&15u&16t&09i&10f&11u&12l &13o&14n&15e&16:&00&11 %s&00"
              );
       if (!str_cmp(tch->player.name, "Heaven"))
        sprintf(buf, "&15[&16Tuskino's Girl&15] &15{&13F&15} &15A little peice of &15H&14e&15a&14v&15e&14n&00"
              );
        
      if (GET_INVIS_LEV(tch))
	sprintf(buf + strlen(buf), " &09(%Ld)&00", GET_INVIS_LEV(tch));
      else if (AFF_FLAGGED(tch, AFF_INVISIBLE))
	strcat(buf, " &09(Invis)&00");

      if (PLR_FLAGGED(tch, PLR_MAILING))
	strcat(buf, " &15(Mailing)&00");
      else if ((STATE(d) >= CON_OEDIT) && (STATE(d) <= CON_TEXTED))
        strcat(buf, " &12(BUILDING)&00");
      else if (PLR_FLAGGED(tch, PLR_WRITING))
	strcat(buf, " &15(Writing)&00");

/*clan*/
/*      *buf = '\0'; */

      /*. Add the clan title to the title between []. Think about no allowing
          brakets in titles to prevent simulating clan titles .*/
/*
      if ((clan_num=find_clan_by_id(GET_CLAN(wch)))>=0&&clan_num<num_of_clans)
{
        if(GET_CLAN_RANK(wch)>0)
          sprintf(buf, "%s [%s of %s]", buf,


clan[clan_num].rank_name[GET_CLAN_RANK(wch) -1],
                clan[clan_num].name);
        else
          sprintf(buf, "%s [Applying to %s]", buf, clan[clan_num].name);
       }*/
/*end clan*/
      if (PRF_FLAGGED(tch, PRF_NOTELL))
	strcat(buf, " &11(No Tell)&00");
      if (PRF_FLAGGED(tch, PRF_QUEST))
	strcat(buf, " &15(&14RP&15)&00");
      if (PLR_FLAGGED(tch, PLR_KILLER))
	strcat(buf, " &11(&09PK&11)&00");
	if (PRF_FLAGGED(tch, PRF_AFK))
        strcat(buf, " &15(&09Away&15)&00");
      if (PRF_FLAGGED(tch, PRF_OOZARU))
        strcat(buf, " &09(Oozaru)&00");
      if (PRF_FLAGGED(tch, PRF_NOGOSS))
        strcat(buf, " &12(No Roleplay)&00");
      if (PRF_FLAGGED(tch, PRF_NOooc))
        strcat(buf, " &12(No OOC)&00");
      if (PLR_FLAGGED(tch, PLR_trans4) && IS_BIODROID(tch))
        strcat(buf, " &10(Super Perfect)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && IS_BIODROID(tch))
        strcat(buf, " &10(Perfect)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_BIODROID(tch))
        strcat(buf, " &20(&00&02Imperfect&00&20)&00");
      else if (PLR_FLAGGED(tch, PLR_trans4) && (IS_saiyan(tch) || IS_HALF_BREED(tch)))
        strcat(buf, " &11(&09SSJ4&11)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && (IS_saiyan(tch) || IS_HALF_BREED(tch)))
        strcat(buf, " &11(SSJ3)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && (IS_saiyan(tch) || IS_HALF_BREED(tch)))
        strcat(buf, " &11(&12SSJ2&11)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && (IS_saiyan(tch) || IS_HALF_BREED(tch)))
        strcat(buf, " &11(&00&03SSJ&00&11)&00");
      else if (PLR_FLAGGED(tch, PLR_trans4) && IS_icer(tch))
        strcat(buf, " &11(&12Icer 4&11)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && IS_icer(tch))
        strcat(buf, " &11(&12Icer 3&11)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_icer(tch))
        strcat(buf, " &11(&12Icer 2&11)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_icer(tch))
        strcat(buf, " &11(&12Icer 1&11)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_MAJIN(tch))
        strcat(buf, " &14(&13Good&14)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_MAJIN(tch))
        strcat(buf, " &14(&13Evil&14)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && IS_MAJIN(tch))
        strcat(buf, " &14(&13Super&14)&00");
      else if (PLR_FLAGGED(tch, PLR_trans4) && IS_MAJIN(tch))
        strcat(buf, " &14(&13Panks&14)&00");
      else if (PRF_FLAGGED(tch, PRF_MYSTIC))
        strcat(buf, " &15(&14Mystic&15)&00");

      if (GET_LEVEL(tch) >= LVL_IMMORT)
	strcat(buf, CCNRM(ch, C_SPR));
      strcat(buf, "\r\n");
      send_to_char(buf, ch);
    }				/* endif shortlist */
  }				/* end of for */
  if (short_list && (num_can_see % 4))
    send_to_char("\r\n", ch);
  if (num_can_see == 0)
    sprintf(buf, "\r\n&15No-one at all!&00\r\n");
  else if (num_can_see == 1)
    sprintf(buf, "\r\n    &13One &15lonely warrior in the &10Struggle&15.&00\r\n");
  else
    sprintf(buf, "\r\n    &14%d&00 &15warriors in the &10Struggle&15.&00\r\n", num_can_see);
  send_to_char(buf, ch);
//  send_to_char("\r\n      &15Players on &11Drag&12(&09*&12)&11nball - &10Struggle &15for &09Power&00\r\n\r\n", ch);
}




#define USERS_FORMAT \
"format: users [-l minlevel[-maxlevel]] [-n name] [-h host] [-c classlist] [-o] [-p]\r\n"

ACMD(do_users)
{
  const char *format = "%3d %-7s %-12s %-14s %-3s %-8s ";
  char line[200], line2[220], idletime[10], classname[20];
  char state[30], *timeptr, mode;
  char name_search[MAX_INPUT_LENGTH], host_search[MAX_INPUT_LENGTH];
  struct char_data *tch;
  struct descriptor_data *d;
  size_t i;
  int low = 0, high = LVL_IMPL -1, num_can_see = 0;
  int showclass = 0, outlaws = 0, playing = 0, deadweight = 0;

  host_search[0] = name_search[0] = '\0';

  strcpy(buf, argument);
  while (*buf) {
    half_chop(buf, arg, buf1);
    if (*arg == '-') {
      mode = *(arg + 1);  /* just in case; we destroy arg in the switch */
      switch (mode) {
      case 'o':
      case 'k':
	outlaws = 1;
	playing = 1;
	strcpy(buf, buf1);
	break;
      case 'c':
        half_chop(buf1, arg, buf);
        for (i = 0; i < strlen(arg); i++)
          showclass |= find_class_bitvector(arg[i]);
        break;
      case 'p':
	playing = 1;
	strcpy(buf, buf1);
	break;
      case 'd':
	deadweight = 1;
	strcpy(buf, buf1);
	break;
      case 'l':
	playing = 1;
	half_chop(buf1, arg, buf);
	sscanf(arg, "%d-%d", &low, &high);
	break;
      case 'n':
	playing = 1;
	half_chop(buf1, name_search, buf);
	break;
      case 'h':
	playing = 1;
	half_chop(buf1, host_search, buf);
	break;
      default:
	send_to_char(USERS_FORMAT, ch);
	return;
      }				/* end of switch */

    } else {			/* endif */
      send_to_char(USERS_FORMAT, ch);
      return;
    }
  }				/* end while (parser) */
  strcpy(line,
	 "Num Level   Name         State          Idl Login@   Site\r\n");
  strcat(line,
	 "--- ------- ------------ -------------- --- -------- ---------------------\r\n");
  send_to_char(line, ch);

  one_argument(argument, arg);

  for (d = descriptor_list; d; d = d->next) {
    if (STATE(d) != CON_PLAYING && playing)
      continue;
    if (STATE(d) == CON_PLAYING && deadweight)
      continue;
    if (STATE(d) == CON_PLAYING) {
      if (d->original)
	tch = d->original;
      else if (!(tch = d->character))
	continue;

      if (*host_search && !strstr(d->host, host_search))
	continue;
      if (*name_search && str_cmp(GET_NAME(tch), name_search))
	continue;
      if (!CAN_SEE(ch, tch) || GET_LEVEL(tch) < low || GET_LEVEL(tch) > high)
	continue;
      if (outlaws && !PLR_FLAGGED(tch, PLR_KILLER) &&
	  !PLR_FLAGGED(tch, PLR_icer))
	continue;
      if (GET_INVIS_LEV(ch) > GET_LEVEL(ch))
	continue;
      if (d->original)
        sprintf(classname, "[%2Ld]", GET_LEVEL(d->original));
      else
        sprintf(classname, "[%2Ld]", GET_LEVEL(d->character));
        } else
      strcpy(classname, "   -   ");

    timeptr = asctime(localtime(&d->login_time));
    timeptr += 11;
    *(timeptr + 8) = '\0';

    if (STATE(d) == CON_PLAYING && d->original)
      strcpy(state, "Switched");
    else
      strcpy(state, connected_types[STATE(d)]);

    if (d->character && STATE(d) == CON_PLAYING && GET_LEVEL(d->character) < LVL_GOD)
      sprintf(idletime, "%3d", d->character->char_specials.timer *
	      SECS_PER_MUD_HOUR / SECS_PER_REAL_MIN);
    else
      strcpy(idletime, "");

    if (d->character && d->character->player.name) {
      if (d->original)
	sprintf(line, format, d->desc_num, classname,
		d->original->player.name, state, idletime, timeptr);
      else
	sprintf(line, format, d->desc_num, classname,
		d->character->player.name, state, idletime, timeptr);
    } else
      sprintf(line, format, d->desc_num, "   -   ", "UNDEFINED",
	      state, idletime, timeptr);

    if (d->host && *d->host)
      sprintf(line + strlen(line), "[%s]\r\n", d->host);
    else
      strcat(line, "[Hostname unknown]\r\n");

    if (STATE(d) != CON_PLAYING) {
      sprintf(line2, "%s%s%s", CCGRN(ch, C_SPR), line, CCNRM(ch, C_SPR));
      strcpy(line, line2);
    }
    if (STATE(d) != CON_PLAYING ||
		(STATE(d) == CON_PLAYING && CAN_SEE(ch, d->character))) {
      send_to_char(line, ch);
      num_can_see++;
    }
  }

  sprintf(line, "\r\n%d visible sockets connected.\r\n", num_can_see);
  send_to_char(line, ch);
}



/* Generic page_string function for displaying text */
ACMD(do_gen_ps)
{
  switch (subcmd) {
  case SCMD_CREDITS:
    page_string(ch->desc, credits, 0);
    break;
  case SCMD_NEWS:
    page_string(ch->desc, news, 0);
    break;
  case SCMD_INFO:
    page_string(ch->desc, info, 0);
    break;
  case SCMD_WIZLIST:
    page_string(ch->desc, wizlist, 0);
    break;
  case SCMD_IMMLIST:
    page_string(ch->desc, immlist, 0);
    break;
  case SCMD_HANDBOOK:
    page_string(ch->desc, handbook, 0);
    break;
  case SCMD_POLICIES:
    page_string(ch->desc, policies, 0);
    break;
  case SCMD_MOTD:
    page_string(ch->desc, motd, 0);
    break;
  case SCMD_IMOTD:
    page_string(ch->desc, imotd, 0);
    break;
  case SCMD_CLEAR:
    send_to_char("\033[H\033[J", ch);
    break;
  case SCMD_WHOAMI:
    send_to_char(strcat(strcpy(buf, GET_NAME(ch)), "\r\n"), ch);
    break;
  default:
    return;
  }
}


void perform_mortal_where(struct char_data * ch, char *arg)
{
  register struct char_data *i;
  register struct descriptor_data *d;

  if (!*arg) {
    send_to_char("Players in your Zone\r\n--------------------\r\n", ch);
    for (d = descriptor_list; d; d = d->next) {
      if (STATE(d) != CON_PLAYING || d->character == ch)
	continue;
      if ((i = (d->original ? d->original : d->character)) == NULL)
	continue;
      if (i->in_room == NOWHERE || !CAN_SEE(ch, i))
	continue;
      if (world[ch->in_room].zone != world[i->in_room].zone)
	continue;
      sprintf(buf, "%-20s - %s\r\n", GET_NAME(i), world[i->in_room].name);
      send_to_char(buf, ch);
    }
  } else {			/* print only FIRST char, not all. */
    for (i = character_list; i; i = i->next) {
      if (i->in_room == NOWHERE || i == ch)
	continue;
      if (!CAN_SEE(ch, i) || world[i->in_room].zone != world[ch->in_room].zone)
	continue;
      if (!isname(arg, i->player.name))
	continue;
      sprintf(buf, "%-25s - %s\r\n", GET_NAME(i), world[i->in_room].name);
      send_to_char(buf, ch);
      return;
    }
    send_to_char("No-one around by that name.\r\n", ch);
  }
}


void print_object_location(int num, struct obj_data * obj, struct char_data * ch,
			        int recur)
{
  if (num > 0)
    sprintf(buf, "O%3d. %-25s - ", num, obj->short_description);
  else
    sprintf(buf, "%33s", " - ");

  if (obj->in_room > NOWHERE) {
    sprintf(buf + strlen(buf), "[%5d] %s\r\n",
	    GET_ROOM_VNUM(IN_ROOM(obj)), world[obj->in_room].name);
    send_to_char(buf, ch);
  } else if (obj->carried_by) {
    sprintf(buf + strlen(buf), "carried by %s\r\n",
	    PERS(obj->carried_by, ch));
    send_to_char(buf, ch);
  } else if (obj->worn_by) {
    sprintf(buf + strlen(buf), "worn by %s\r\n",
	    PERS(obj->worn_by, ch));
    send_to_char(buf, ch);
  } else if (obj->in_obj) {
    sprintf(buf + strlen(buf), "inside %s%s\r\n",
	    obj->in_obj->short_description, (recur ? ", which is" : " "));
    send_to_char(buf, ch);
    if (recur)
      print_object_location(0, obj->in_obj, ch, recur);
  } else {
    sprintf(buf + strlen(buf), "in an unknown location\r\n");
    send_to_char(buf, ch);
  }
}



void perform_immort_where(struct char_data * ch, char *arg)
{
  register struct char_data *i;
  register struct obj_data *k;
  struct descriptor_data *d;
  int num = 0, found = 0;

  if (!*arg) {
    send_to_char("Players\r\n-------\r\n", ch);
    for (d = descriptor_list; d; d = d->next)
      if (STATE(d) == CON_PLAYING) {
	i = (d->original ? d->original : d->character);
	if (i && CAN_SEE(ch, i) && (i->in_room != NOWHERE)) {
	  if (d->original)
	    sprintf(buf, "%-20s - [%5d] %s (in %s)\r\n",
		    GET_NAME(i), GET_ROOM_VNUM(IN_ROOM(d->character)),
		 world[d->character->in_room].name, GET_NAME(d->character));
	  else
	    sprintf(buf, "%-20s - [%5d] %s\r\n", GET_NAME(i),
		    GET_ROOM_VNUM(IN_ROOM(i)), world[i->in_room].name);
	  send_to_char(buf, ch);
	}
      }
  } else {
    for (i = character_list; i; i = i->next)
      if (CAN_SEE(ch, i) && i->in_room != NOWHERE && isname(arg, i->player.name)) {
	found = 1;
	sprintf(buf, "M%3d. %-25s - [%5d] %s\r\n", ++num, GET_NAME(i),
		GET_ROOM_VNUM(IN_ROOM(i)), world[IN_ROOM(i)].name);
	send_to_char(buf, ch);
      }
    for (num = 0, k = object_list; k; k = k->next)
      if (CAN_SEE_OBJ(ch, k) && isname(arg, k->name)) {
	found = 1;
	print_object_location(++num, k, ch, TRUE);
      }
    if (!found)
      send_to_char("Couldn't find any such thing.\r\n", ch);
  }
}


ACMD(do_where)
{
  one_argument(argument, arg);

  if (GET_LEVEL(ch) >= LVL_IMMORT)
    perform_immort_where(ch, arg);
  else
    perform_mortal_where(ch, arg);
}



ACMD(do_consider)
{
  struct char_data *victim;
  int diff;

  one_argument(argument, buf);

  if (!(victim = get_char_room_vis(ch, buf))) {
    send_to_char("Consider killing who?\r\n", ch);
    return;
  }
  if (victim == ch) {
    send_to_char("Easy!  Very easy indeed!\r\n", ch);
    return;
  }
  diff = ((GET_STR(victim)*2+GET_MAX_MANA(victim)*GET_LEVEL(victim)+GET_DAMROLL(victim)) - (GET_STR(ch)*2+GET_MAX_MANA(ch)*GET_LEVEL(ch)+GET_DAMROLL(ch)));

  if (diff <= -1000000)
    send_to_char("Now where did that chicken go?\r\n", ch);
  else if (diff <= -100000)
    send_to_char("You could do it with a needle!\r\n", ch);
  else if (diff <= -10000)
    send_to_char("Easy.\r\n", ch);
  else if (diff <= -1000)
    send_to_char("Fairly easy.\r\n", ch);
  else if (diff == 0)
    send_to_char("The perfect match!\r\n", ch);
  else if (diff <= 100)
    send_to_char("You would need some luck!\r\n", ch);
  else if (diff <= 1000)
    send_to_char("You would need a lot of luck!\r\n", ch);
  else if (diff <= 10000)
    send_to_char("You would need a lot of luck and great equipment!\r\n", ch);
  else if (diff <= 100000)
    send_to_char("Do you feel lucky, punk?\r\n", ch);
  else if (diff <= 500000)
    send_to_char("Are you mad!?\r\n", ch);
  else if (diff <= 1000000)
    send_to_char("You ARE mad!\r\n", ch);

}



ACMD(do_diagnose)
{
  struct char_data *vict;

  one_argument(argument, buf);

  if (*buf) {
    if (!(vict = get_char_room_vis(ch, buf))) {
      send_to_char(NOPERSON, ch);
      return;
    } else
      diag_char_to_char(vict, ch);
  } else {
    if (FIGHTING(ch))
      diag_char_to_char(FIGHTING(ch), ch);
    else
      send_to_char("Diagnose who?\r\n", ch);
  }
}


const char *ctypes[] = {
  "off", "sparse", "normal", "complete", "\n"
};

ACMD(do_color)
{
  int tp;

  if (IS_NPC(ch))
    return;

  one_argument(argument, arg);

  if (!*arg) {
    sprintf(buf, "Your current color level is %s.\r\n", ctypes[COLOR_LEV(ch)]);
    send_to_char(buf, ch);
    return;
  }
  if (((tp = search_block(arg, ctypes, FALSE)) == -1)) {
    send_to_char("Usage: color { Off | Sparse | Normal | Complete }\r\n", ch);
    return;
  }
  REMOVE_BIT(PRF_FLAGS(ch), PRF_COLOR_1 | PRF_COLOR_2);
  SET_BIT(PRF_FLAGS(ch), (PRF_COLOR_1 * (tp & 1)) | (PRF_COLOR_2 * (tp & 2) >> 1));

  sprintf(buf, "Your %scolor%s is now %s.\r\n", CCRED(ch, C_SPR),
	  CCNRM(ch, C_OFF), ctypes[tp]);
  send_to_char(buf, ch);
}


ACMD(do_toggle)
{
  if (IS_NPC(ch))
    return;
  if (GET_WIMP_LEV(ch) == 0)
    strcpy(buf2, "OFF");
  else
    sprintf(buf2, "%-3d", GET_WIMP_LEV(ch));

  sprintf(buf,
	  "Hit Pnt Display: %-3s    "
	  "     Brief Mode: %-3s    "
	  " Summon Protect: %-3s\r\n"

	  "   Move Display: %-3s    "
	  "   Compact Mode: %-3s    "
	  "       On Quest: %-3s\r\n"

	  "     Ki Display: %-3s    "
	  "         NoTell: %-3s    "
	  "   Repeat Comm.: %-3s\r\n"

	  " Auto Show Exit: %-3s    "
	  "           Deaf: %-3s    "
	  "     Wimp Level: %-3s\r\n"

	  " Telepathy Channel: %-3s    "
	  "    OOC Channel: %-3s    "
	  "  Grats Channel: %-3s\r\n"
	  
	  "    Color Level: %s\r\n",

	  ONOFF(PRF_FLAGGED(ch, PRF_DISPHP)),
	  ONOFF(PRF_FLAGGED(ch, PRF_BRIEF)),
	  ONOFF(!PRF_FLAGGED(ch, PRF_SUMMONABLE)),

	  ONOFF(PRF_FLAGGED(ch, PRF_DISPMOVE)),
	  ONOFF(PRF_FLAGGED(ch, PRF_COMPACT)),
	  YESNO(PRF_FLAGGED(ch, PRF_QUEST)),

	  ONOFF(PRF_FLAGGED(ch, PRF_DISPMANA)),
	  ONOFF(PRF_FLAGGED(ch, PRF_NOTELL)),
	  YESNO(!PRF_FLAGGED(ch, PRF_NOREPEAT)),

	  ONOFF(PRF_FLAGGED(ch, PRF_AUTOEXIT)),
	  YESNO(PRF_FLAGGED(ch, PRF_DEAF)),
	  buf2,

	  ONOFF(!PRF_FLAGGED(ch, PRF_NOGOSS)),
	  ONOFF(!PRF_FLAGGED(ch, PRF_NOooc)),
	  ONOFF(!PRF_FLAGGED(ch, PRF_NOGRATZ)),

	  ctypes[COLOR_LEV(ch)]);

  send_to_char(buf, ch);
}


struct sort_struct {
  int sort_pos;
  byte is_social;
} *cmd_sort_info = NULL;

int num_of_cmds;


void sort_commands(void) {
  int a, b, tmp;

  num_of_cmds = 0;

  /*
   * first, count commands (num_of_commands is actually one greater than the
   * number of commands; it inclues the '\n'.
   */
  while (*complete_cmd_info[num_of_cmds].command != '\n')
    num_of_cmds++;

   /* check if there was an old sort info.. then free it -- aedit -- M. Scott*/
   if (cmd_sort_info) free(cmd_sort_info);
  /* create data array */
  CREATE(cmd_sort_info, struct sort_struct, num_of_cmds);

  /* initialize it */
  for (a = 1; a < num_of_cmds; a++) {
    cmd_sort_info[a].sort_pos = a;
      cmd_sort_info[a].is_social = (complete_cmd_info[a].command_pointer == do_action);
  }

  /* the infernal special case */
  cmd_sort_info[find_command("insult")].is_social = TRUE;

  /* Sort.  'a' starts at 1, not 0, to remove 'RESERVED' */
  for (a = 1; a < num_of_cmds - 1; a++)
    for (b = a + 1; b < num_of_cmds; b++)
     if (strcmp(complete_cmd_info[cmd_sort_info[a].sort_pos].command,
		complete_cmd_info[cmd_sort_info[b].sort_pos].command) > 0) {
	tmp = cmd_sort_info[a].sort_pos;
	cmd_sort_info[a].sort_pos = cmd_sort_info[b].sort_pos;
	cmd_sort_info[b].sort_pos = tmp;
      }
}



ACMD(do_commands)
{
  int no, i, cmd_num;
  int wizhelp = 0, socials = 0;
  struct char_data *vict;

  one_argument(argument, arg);

  if (*arg) {
    if (!(vict = get_char_vis(ch, arg)) || IS_NPC(vict)) {
      send_to_char("Who is that?\r\n", ch);
      return;
    }
    if (GET_LEVEL(ch) < GET_LEVEL(vict)) {
      send_to_char("You can't see the commands of people above your level.\r\n", ch);
      return;
    }
  } else
    vict = ch;

  if (subcmd == SCMD_SOCIALS)
    socials = 1;
  else if (subcmd == SCMD_WIZHELP)
    wizhelp = 1;

  sprintf(buf, "The following %s%s are available to %s:\r\n",
	  wizhelp ? "privileged " : "",
	  socials ? "socials" : "commands",
	  vict == ch ? "you" : GET_NAME(vict));

  /* cmd_num starts at 1, not 0, to remove 'RESERVED' */
  for (no = 1, cmd_num = 1; cmd_num < num_of_cmds; cmd_num++) {
    i = cmd_sort_info[cmd_num].sort_pos;
    if (complete_cmd_info[i].minimum_level >= 0 &&
	GET_LEVEL(vict) >= complete_cmd_info[i].minimum_level &&
	(complete_cmd_info[i].minimum_level >= LVL_IMMORT) == wizhelp &&
	(wizhelp || socials == cmd_sort_info[i].is_social)) {
      sprintf(buf + strlen(buf), "%-11s", complete_cmd_info[i].command);
      if (!(no % 7))
	strcat(buf, "\r\n");
      no++;
    }
  }

  strcat(buf, "\r\n");
  send_to_char(buf, ch);
}


ACMD(do_pushup)	
   {   

int is_altered = FALSE;
int num_levels = 0;
   int MOVE = 0, EXP = 0;
    if (IS_ANDROID(ch)) {
    send_to_char("&15Androids have no use for pushups, you won't get stronger.&00\r\n", ch);
    return;
}
          if (number(1, 100) < 10 && GET_MOVE(ch) >= 50 && GET_LBS(ch) > 10) {
send_to_char("You feel your upper body strenghtening!\r\n", ch);
         ch->real_abils.ubs += 1;
         ch->real_abils.lbs -= 1;
         affect_total(ch);
        }

if (PRF_FLAGGED(ch, PRF_CAMP)) {
      send_to_char("You can't do this while camped!\r\n", ch);
      return;
    }
if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000) {
GET_LEVEL(ch) += 1;
GET_EXP(ch) = 1;
num_levels++;                                                                   advance_level(ch);                                                           
is_altered = TRUE;
send_to_char("You seem to have grown surprisingly stronger with that pushup!\r\n", ch);
}
   if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10) && GET_MOVE(ch) >= 51)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(50, 50);
  EXP = number(GET_LEVEL(ch)*5+100, GET_LEVEL(ch)*40+100);
  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) && GET_MOVE(ch) >= 101)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(150, 150);
  EXP = number(GET_LEVEL(ch)*10+100, GET_LEVEL(ch)*100+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200) && GET_MOVE(ch) >= 201)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(250, 250);
  EXP = number(GET_LEVEL(ch)*20+100, GET_LEVEL(ch)*200+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300) && GET_MOVE(ch) >= 301)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(350, 350);
  EXP = number(GET_LEVEL(ch)*30+100, GET_LEVEL(ch)*300+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400) && GET_MOVE(ch) >= 401)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(450, 450);
  EXP = number(GET_LEVEL(ch)*40+100, GET_LEVEL(ch)*400+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) && GET_MOVE(ch) >= 501)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(550, 550);
  EXP = number(GET_LEVEL(ch)*100+100, GET_LEVEL(ch)*800+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
    }
 else if (GET_MOVE(ch) >= 16 &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_SPIRIT))
   {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(10, 10);
  EXP = number(GET_LEVEL(ch)+100, GET_LEVEL(ch)+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
    }

  else
  {
        act("Your too tired to do pushups.", TRUE, ch, 0, 0, TO_CHAR);
        return;
        }

   }

ACMD(do_situp)	
   {

   int is_altered = FALSE;
   int num_levels = 0;
   int MOVE = 0, EXP = 0;
       if (IS_ANDROID(ch)) {
    send_to_char("&15Androids have no use for situps, you won't get stronger.&00\r\n", ch);
    return;
}
if (PRF_FLAGGED(ch, PRF_CAMP)) {
      send_to_char("You can't do this while camped!\r\n", ch);
      return;
    }
if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000) {
GET_LEVEL(ch) += 1;
GET_EXP(ch) = 1;
num_levels++;                                                                   advance_level(ch);                                                           
is_altered = TRUE;
send_to_char("You seem to have grown surprisingly stronger with that pushup!\r\n", ch);
}
          if (number(1, 100) < 10 && GET_MOVE(ch) >= 50 && GET_LBS(ch) > 10) {
send_to_char("You feel your upper body strenghtening!\r\n", ch);
         ch->real_abils.ubs += 1;
         ch->real_abils.lbs -= 1;
         affect_total(ch);
        }
   if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10) && GET_MOVE(ch) >= 16)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(10, 10);
  EXP = number(GET_LEVEL(ch)*5+100, GET_LEVEL(ch)*5+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) && GET_MOVE(ch) >= 51)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(30, 30);
  EXP = number(GET_LEVEL(ch)*10+100, GET_LEVEL(ch)*10+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200) && GET_MOVE(ch) >= 101)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(40, 40);
  EXP = number(GET_LEVEL(ch)*20+100, GET_LEVEL(ch)*20+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300) && GET_MOVE(ch) >= 201)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(80, 80);
  EXP = number(GET_LEVEL(ch)*30+100, GET_LEVEL(ch)*30+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400) && GET_MOVE(ch) >= 301)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(150, 150);
  EXP = number(GET_LEVEL(ch)*40+00, GET_LEVEL(ch)*40+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) && GET_MOVE(ch) >= 401)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(200, 200);
  EXP = number(GET_LEVEL(ch)*100+100, GET_LEVEL(ch)*100+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
    }
   else if (GET_MOVE(ch) >= 16 &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_SPIRIT))
   {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some &12situps&00.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(10, 10);
  EXP = number(GET_LEVEL(ch)+100, GET_LEVEL(ch)+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
    }
  else
  {
        act("Your too tired to do situps.", TRUE, ch, 0, 0, TO_CHAR);
        return;
        }

   }

ACMD(do_scan)
{
  struct char_data *i;
  int is_in, dir, dis, maxdis, found = 0;


  const char *distance[] = {
    "&09right here",
    "&09immediately ",
    "&09nearby ",
    "&09a ways ",
    "&09far ",
    "&09very far ",
    "&09extremely far ",
    "&09impossibly far ",
  };

 
 if (!GET_SKILL(ch, SKILL_SCAN)) {
  send_to_char("You can't do that!\r\n", ch);
  return;
}


  if (IS_AFFECTED(ch, AFF_SOLAR_FLARE)) {
    act("You can't see anything, you're blind!", TRUE, ch, 0, 0, TO_CHAR);
    return;
  }
  if ((GET_MOVE(ch) < 3) && (GET_LEVEL(ch) < LVL_IMMORT)) {
    act("You are too exhausted.", TRUE, ch, 0, 0, TO_CHAR);
    return;
  }

  maxdis = (1 + ((GET_SKILL(ch, SKILL_SCAN) * 5) / 100));
  if (GET_LEVEL(ch) >= LVL_IMMORT)
    maxdis = 7;

  act("&15You quickly scan the area and see:&00", TRUE, ch, 0, 0, TO_CHAR);
  act("$n quickly scans the area.", FALSE, ch, 0, 0, TO_ROOM);
  if (GET_LEVEL(ch) < LVL_IMMORT)
    GET_MOVE(ch) -= 3;

  is_in = ch->in_room;
  for (dir = 0; dir < NUM_OF_DIRS; dir++) {
    ch->in_room = is_in;
    for (dis = 0; dis <= maxdis; dis++) {
      if (((dis == 0) && (dir == 0)) || (dis > 0)) {
        for (i = world[ch->in_room].people; i; i = i->next_in_room) {
          if ((!((ch == i) && (dis == 0))) && CAN_SEE(ch, i)) {
            sprintf(buf, "%33s: %s%s%s%s&00", GET_NAME(i), distance[dis],
                    ((dis > 0) && (dir < (NUM_OF_DIRS - 2))) ? "&15to the&09 " : "",
                    (dis > 0) ? dirs[dir] : "",
                    ((dis > 0) && (dir > (NUM_OF_DIRS - 3))) ? "&15wards&00" : "");
            act(buf, TRUE, ch, 0, 0, TO_CHAR);
            found++;
          }
        }
      }
      if (!CAN_GO(ch, dir) || (world[ch->in_room].dir_option[dir]->to_room
== is_in))
        break;
      else
        ch->in_room = world[ch->in_room].dir_option[dir]->to_room;
    }
  }
  if (found == 0)
    act("Nobody anywhere near you.", TRUE, ch, 0, 0, TO_CHAR);
  ch->in_room = is_in;
}

ACMD(do_finger)
{
	
  char value[MAX_INPUT_LENGTH], birth[80];
  struct char_file_u vbuf;
	one_argument(argument, arg);
if (!*argument) {
      send_to_char("A name would help.\r\n", ch);
      return;
    }

    if (load_char(value, &vbuf) < 0) {
      send_to_char("There is no such player.\r\n", ch);
      return;
    }
    else
    sprintf(buf, "Player: %-12s (%s) [%s] They have %s eyes, and %s %s hair.\r\n", vbuf.name,
      genders[(int) vbuf.sex], class_abbrevs[(int) vbuf.chclass], eyes[(int) vbuf.eye], hl[(int) vbuf.hairl], hc[(int) vbuf.hairc] );
    sprintf(buf,
	 "%sAu: %-8d  Bal: %-8d  Exp: %-10Ld  Align: %-5d  Lessons: %-3d\r\n",
	    buf, vbuf.points.gold, vbuf.points.bank_gold, vbuf.points.exp,
	    vbuf.char_specials_saved.alignment,
	    vbuf.player_specials_saved.spells_to_learn);
    strcpy(birth, ctime(&vbuf.birth));
    sprintf(buf,
	    "%sStarted: %-20.16s  Last: %-20.16s  Played: %3dh %2dm\r\n",
	    buf, birth, ctime(&vbuf.last_logon), (int) (vbuf.played / 3600),
	    (int) (vbuf.played / 60 % 60));
    send_to_char(buf, ch);

}
ACMD(do_benchpress)

   {
int is_altered = FALSE;
int num_levels = 0;
   int MOVE = 0, EXP = 0;
    if (IS_ANDROID(ch)) {
    send_to_char("&15Androids have no use for benchpresses, you won't get stronger.&00\r\n", ch);
    return;
}
          if (number(1, 100) < 10 && GET_MOVE(ch) >= 50 && GET_LBS(ch) > 10) {
send_to_char("You feel your upper body strenghtening!\r\n", ch);
         ch->real_abils.ubs += 1;
         ch->real_abils.lbs -= 1;
         affect_total(ch);
        }

if (PRF_FLAGGED(ch, PRF_CAMP)) {
      send_to_char("You can't do this while camped!\r\n", ch);
      return;
    }
if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000) {
GET_LEVEL(ch) += 1;
GET_EXP(ch) = 1;
num_levels++;                                                                   advance_level(ch);                           
is_altered = TRUE;
send_to_char("You seem to have grown surprisingly stronger with that pushup!\r\n", ch);
}
   if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10) && GET_MOVE(ch) >= 51)
      {
send_to_char("You do some benchpresses and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some benchpresses.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(50, 50);
  EXP = number(GET_LEVEL(ch)*5+100, GET_LEVEL(ch)*40+100);
  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) && GET_MOVE(ch) >= 101)
      {
send_to_char("You do some benchpresses and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some benchpresses.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(150, 150);
  EXP = number(GET_LEVEL(ch)*10+100, GET_LEVEL(ch)*100+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200) && GET_MOVE(ch) >= 201)
      {
send_to_char("You do some benchpresses and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some benchpresses.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(250, 250);
  EXP = number(GET_LEVEL(ch)*20+100, GET_LEVEL(ch)*200+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300) && GET_MOVE(ch) >= 301)
      {
send_to_char("You do some benchpresses and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some benchpresses.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(350, 350);
  EXP = number(GET_LEVEL(ch)*30+100, GET_LEVEL(ch)*300+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400) && GET_MOVE(ch) >= 401)
      {
send_to_char("You do some benchpresses and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some benchpresses.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(450, 450);
  EXP = number(GET_LEVEL(ch)*40+100, GET_LEVEL(ch)*400+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_SPIRIT) && GET_MOVE(ch) >= 501)
      {
send_to_char("You do some benchpresses and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some benchpresses.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(550, 550);
  EXP = number(GET_LEVEL(ch)*100+100, GET_LEVEL(ch)*800+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_SPIRIT) && GET_MOVE(ch) >= 501)
      {
send_to_char("You do some benchpresses and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some benchpresses.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(550, 550);
  EXP = number(GET_LEVEL(ch)*100+100, GET_LEVEL(ch)*800+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
    }
 else if (GET_MOVE(ch) >= 16 &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_SPIRIT))
   {
send_to_char("You do some benchpresses and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some benchpresses.", TRUE, ch, 0, 0, TO_ROOM);

  MOVE = number(10, 10);
  EXP = number(GET_LEVEL(ch)+100, GET_LEVEL(ch)+100);

  GET_MOVE(ch) = GET_MOVE(ch) - MOVE;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  return;
    }

  else
  {
        act("Your too tired to do benchpresses.", TRUE, ch, 0, 0, TO_CHAR);
        return;
        }
   }



ACMD(do_players)
{
  int i, count = 0;
  *buf = 0;

  for (i = 0; i <= top_of_p_table; i++) {
    sprintf(buf, "%s  %-20.20s", buf, (player_table + i)->name);
    count++;
    if (count == 3) {
      count = 0;
      strcat(buf, "\r\n");
    }
  }
  page_string(ch->desc, buf, 1);
}


