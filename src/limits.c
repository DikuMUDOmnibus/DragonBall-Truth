/* ************************************************************************
*   File: limits.c                                      Part of CircleMUD *
*  Usage: limits & gain funcs for HMV, exp, hunger/thirst, idle time      *
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
#include "spells.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "dg_scripts.h"

extern struct char_data *character_list;
extern struct obj_data *object_list;
extern struct room_data *world;
extern int max_exp_gain;
extern int max_exp_loss;
extern int idle_rent_time;
extern int idle_max_level;
extern int idle_void;
extern int use_autowiz;
extern long long min_wizlist_lev;
extern int free_rent;

/* local functions */
int graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6);
void check_autowiz(struct char_data * ch);

void Crash_rentsave(struct char_data *ch, int cost);
int level_exp(int chclass, int level);
char *title_male(int chclass, int level);
char *title_female(int chclass, int level);
void update_char_objects(struct char_data * ch);	/* handler.c */

/* When age < 15 return the value p0 */
/* When age in 15..29 calculate the line between p1 & p2 */
/* When age in 30..44 calculate the line between p2 & p3 */
/* When age in 45..59 calculate the line between p3 & p4 */
/* When age in 60..79 calculate the line between p4 & p5 */
/* When age >= 80 return the value p6 */
int graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6)
{

  if (age < 15)
    return (p0);		/* < 15   */
  else if (age <= 29)
    return (int) (p1 + (((age - 15) * (p2 - p1)) / 15));	/* 15..29 */
  else if (age <= 44)
    return (int) (p2 + (((age - 30) * (p3 - p2)) / 15));	/* 30..44 */
  else if (age <= 59)
    return (int) (p3 + (((age - 45) * (p4 - p3)) / 15));	/* 45..59 */
  else if (age <= 79)
    return (int) (p4 + (((age - 60) * (p5 - p4)) / 20));	/* 60..79 */
  else
    return (p6);		/* >= 80 */
}


/*
 * The hit_limit, mana_limit, and move_limit functions are gone.  They
 * added an unnecessary level of complexity to the internal structure,
 * weren't particularly useful, and led to some annoying bugs.  From the
 * players' point of view, the only difference the removal of these
 * functions will make is that a character's age will now only affect
 * the HMV gain per tick, and _not_ the HMV maximums.
 */

/* manapoint gain pr. game hour */
int mana_gain(struct char_data * ch)
{
  int gain;

  if (IS_NPC(ch)) {
    /* Neat and fast */
    gain = GET_MAX_MANA(ch);
  } else {
    gain = number(100, 100);
  
    /* Class calculations */

    /* Skill/Spell calculations */

    /* Position calculations    */
    switch (GET_POS(ch)) {
    case POS_SLEEPING:
      gain += GET_MAX_MANA(ch)/8;
      break;
    case POS_RESTING:
      gain += GET_MAX_MANA(ch)/12;	/* Divide by 2 */
      break;
    case POS_SITTING:
      gain += GET_MAX_MANA(ch)/20;	/* Divide by 4 */
      break;
    case POS_STANDING:
      gain -= 0;      /* Divide by 4 */
      break;
    }
    if (IS_Namek(ch)) {
      gain += GET_MAX_MANA(ch)/4;
      }
    if ((SECT(ch->in_room) == SECT_WATER_NOSWIM) && IS_KANASSAN(ch)) {
      gain += GET_MAX_MANA(ch) /4;
     }
    if (((GET_COND(ch, FULL) <= 5) && (GET_COND(ch, FULL) > 0)) || ((GET_COND(ch, THIRST) <= 5) && (GET_COND(ch, THIRST) > 0)))
      gain = GET_MAX_MANA(ch)/80;
    
    if ((GET_COND(ch, FULL) == 0) || (GET_COND(ch, THIRST) == 0))
      gain = GET_MAX_MANA(ch)/100;
  }
  if (!IS_NPC(ch) && CLN_FLAGGED(ch, CLN_ADEF)) {
   switch (number(1, 5)) {
   case 1:
   act("&15You slow down, catching your breath from the intense defending!", FALSE, ch, 0, 0, TO_CHAR);
   act("&14$n &15slows down, panting from the intense defending.", FALSE, ch, 0, 0, TO_ROOM);
   REMOVE_BIT(CLN_FLAGS(ch), CLN_ADEF);
   break;
   case 2:
   case 3:
   case 4:
   case 5:
   break;
   }
  }
  if (!IS_NPC(ch) && CLN_FLAGGED(ch, CLN_DODGE)) {
   switch(number(1, 5)) {
   case 1:
   act("&15You slow down, catching your breath from the intense dodging!", FALSE, ch, 0, 0, TO_CHAR);
   act("&14$n &15slows down, panting from the intense dodging.", FALSE, ch, 0, 0, TO_ROOM);
   REMOVE_BIT(CLN_FLAGS(ch), CLN_DODGE);
   break;
   case 2:
   case 3:
   case 4:
   case 5:
   break;
   }
  }
  if (!IS_NPC(ch) && CLN_FLAGGED(ch, CLN_BLOCK)) {
   switch (number(1, 5)) {
   case 1:
   act("&15You slow down, catching your breath from the intense blocking!", FALSE, ch, 0, 0, TO_CHAR);
   act("&14$n &15slows down, panting from the intense blocking.", FALSE, ch, 0, 0, TO_ROOM);
   REMOVE_BIT(CLN_FLAGS(ch), CLN_BLOCK);
   break;
   case 2:
   case 3:
   case 4:
   case 5:
   break;
   }
  }
  if (!IS_NPC(ch) && AFF_FLAGGED(ch, AFF_POISON)) {
    GET_HIT(ch) -= GET_MAX_HIT(ch) / 200;
    sprintf(buf, "&16(&10Poison&16)&15=========================== &16[&09%d&16]&00\n", GET_MAX_HIT(ch) / 200);
    send_to_char(buf, ch);
    act("&14$n &15pukes, as they seem to be poisoned....&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  }
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DRAIN) && !IS_NPC(ch)) {
    if (GET_MANA(ch) >= GET_MAX_MANA(ch)/20) {
    send_to_char("&15The room &09drains &15your &10life &12force&15!&00\r\n", ch);
     gain -= GET_MAX_MANA(ch)/20;
     }
    }
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GOOD_REGEN) && GET_POS(ch) != POS_STANDING)
    gain += GET_MAX_MANA(ch)/5;
  if (PRF_FLAGGED(ch, PRF_CAMP))
    gain += GET_MAX_MANA(ch)/5;
  if (IS_icer(ch) && number(1, 30) == 15) {
    SET_BIT(PLR_FLAGS(ch), PLR_TAIL);
  }
  if (IS_BIODROID(ch) && number(1, 30) == 15) {
    SET_BIT(PLR_FLAGS(ch), PLR_TAIL);
  }
  if (IS_saiyan(ch) && number(1, 30) == 15) {
    SET_BIT(PLR_FLAGS(ch), PLR_STAIL);
  }
  if (IS_HALF_BREED(ch) && number(1, 30) == 15) {
    SET_BIT(PLR_FLAGS(ch), PLR_STAIL);
  }
  if (!IS_NPC(ch) && GET_LEVEL(ch) >= 1 && !PRF2_FLAGGED(ch, PRF2_HEAD)) {
   send_to_char("The gods have blessed you with a new head.\r\n", ch);
   SET_BIT(PRF2_FLAGS(ch), PRF2_HEAD);
  }
  if (!IS_NPC(ch) && GET_LEVEL(ch) >= 1 && number(1, 12) == 6) {
   SET_BIT(PLR_FLAGS(ch), PLR_RARM);
   SET_BIT(PLR_FLAGS(ch), PLR_LARM);
   SET_BIT(PLR_FLAGS(ch), PLR_RLEG);
   SET_BIT(PLR_FLAGS(ch), PLR_LLEG);
  }
  if (!IS_NPC(ch) && CLN_FLAGGED(ch, CLN_VITAL) && number(1, 3) == 3) {
   REMOVE_BIT(CLN_FLAGS(ch), CLN_VITAL);
  }
  if (!IS_NPC(ch) && GET_POS(ch) == POS_STUNNED && number(1, 2) == 2) {
    act("&15You are no longer stunned!", TRUE, ch, 0, 0, TO_CHAR);
    act("&14$n&15 is no longer stunned!", TRUE, ch, 0, 0, TO_ROOM);
    GET_POS(ch) = POS_SITTING;
    SET_BIT(CLN_FLAGS(ch), CLN_VITAL);
  }
  if (!IS_NPC(ch) && IS_KONATSU(ch)) {
   REMOVE_BIT(PRF2_FLAGS(ch), PRF2_MUGG);
  }
  if (!IS_NPC(ch) && GET_LOADROOM(ch) >= 1) {
   if (PLR_FLAGGED(ch, PLR_LOADROOM)) {
   REMOVE_BIT(PLR_FLAGS(ch), PLR_LOADROOM);
   }
   GET_LOADROOM(ch) = -1;
  }
  if (GET_COND(ch, DRUNK) > 5) {
     switch (number(1, 10)) {
      case 1:
       act("You bend over, puking all over your shoes.", TRUE, ch, 0, 0, TO_CHAR);
       act("$n bends over, puking all over their shoes.", TRUE, ch, 0, 0, TO_ROOM);
       break;
      case 2:
       act("You swing your fist at the loud mouthed circus clown.", TRUE, ch, 0, 0, TO_CHAR);
       act("$n swings their fist at the loud mouthed circus clown.", TRUE, ch, 0, 0, TO_ROOM);
       break;
      case 3:
       act("You begin peeing right in the middle of the room.", TRUE, ch, 0, 0, TO_CHAR);
       act("$n begins peeing right in the middle of the room.", TRUE, ch, 0, 0, TO_ROOM);
       break;
      case 4:
       act("You scratch your ass while complaining about your problems.", TRUE, ch, 0, 0, TO_CHAR);
       act("$n scratches their ass while complaining about their problems.", TRUE, ch, 0, 0, TO_ROOM);
       break;
      case 5:
       act("You begin ranting about the pixies and their special drink.", TRUE, ch, 0, 0, TO_CHAR);
       act("$n begins ranting about the pixies and their special drink.", TRUE, ch, 0, 0, TO_ROOM);
       break;
      case 6:
       act("You wobble around, licking your lips just before mouthing off to everyone in the room.", TRUE, ch, 0, 0, TO_CHAR);
       act("$n wobbles around, licking their lips just before mouthing off to everyone in the room.", TRUE, ch, 0, 0, TO_ROOM);
       break;
      case 7:
       act("You burp loudly.", TRUE, ch, 0, 0, TO_CHAR);
       act("$n burps loudly.", TRUE, ch, 0, 0, TO_ROOM);
       break;
      case 8:
       act("You begin to think the coat rack is sexy, and likes you.", TRUE, ch, 0, 0, TO_CHAR);
       act("$n starts hitting on the coat rack.", TRUE, ch, 0, 0, TO_ROOM);
       break;
      case 9:
       act("You yell at the bottle to stop whispering!", TRUE, ch, 0, 0, TO_CHAR);
       act("$n yells loudly at their bottle to stop it's whispering to them!", TRUE, ch, 0, 0, TO_ROOM);
       break;
      case 10:
       act("You must really be hammered, you are trying to hump the wall, and failing.", TRUE, ch, 0, 0, TO_CHAR);
       act("$n must really be hammered, they are trying to hump the wall, and failing.", TRUE, ch, 0, 0, TO_ROOM);
       break;
     }
  }
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_HTANK))
    gain += (gain * 20);
  if (GET_MANA(ch) >= GET_MAX_MANA(ch)) {
    gain == 0;
  }    
  if (GET_POS(ch) == POS_STANDING) {
    gain -= gain;
   }
  if (!IS_NPC(ch) && number(1, 6) == 3 && PRF_FLAGGED(ch, PRF_AUTOEXIT)) {
   REMOVE_BIT(PRF_FLAGS(ch), PRF_AUTOEXIT);
   act("&15You are no longer burned!&00", FALSE, ch, 0, 0, TO_CHAR);
   act("&14$n &15is no longer burned!&00", FALSE, ch, 0, 0, TO_ROOM);
  }
  if (!PRF_FLAGGED(ch, PRF_AUTOEXIT)) {
  return (gain);
  }
}


/* Hitpoint gain pr. game hour */
int hit_gain(struct char_data * ch)
{
  int gain;

  if (IS_NPC(ch)) {
    /* Neat and fast */
    gain = GET_LEVEL(ch);
  } else {

    gain = number(100, 100);

    /* Class/Level calculations */

    /* Skill/Spell calculations */

    /* Position calculations    */

    switch (GET_POS(ch)) {
    case POS_SLEEPING:
      gain += GET_MAX_HIT(ch)/8;	/* Divide by 2 */
      break;
    case POS_RESTING:
      gain += GET_MAX_HIT(ch)/12;	/* Divide by 4 */
      break;
    case POS_SITTING:
      gain += GET_MAX_HIT(ch)/18;	/* Divide by 8 */
      break;
    case POS_STANDING:
      gain -= 0;      /* Divide by 4 */
      break;
    }
    if (IS_Namek(ch)) {
      gain += GET_MAX_HIT(ch)/4;
      }    
    if (((GET_COND(ch, FULL) <= 5) && (GET_COND(ch, FULL) > 0)) || ((GET_COND(ch, THIRST) <= 5) && (GET_COND(ch, THIRST) > 0)))
      gain = GET_MAX_HIT(ch)/80;
   

    if ((GET_COND(ch, FULL) == 0) || (GET_COND(ch, THIRST) == 0))
      gain = GET_MAX_HIT(ch)/100;
   }
    if ((SECT(ch->in_room) == SECT_WATER_NOSWIM) && IS_KANASSAN(ch)) {
      gain += GET_MAX_HIT(ch) /4;
     }
  if (AFF_FLAGGED(ch, AFF_POISON))
    gain = GET_MAX_HIT(ch)/220;
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DRAIN) && !IS_NPC(ch)) {
   if (GET_HIT(ch) >= GET_MAX_HIT(ch)/20) {
    gain -= GET_MAX_HIT(ch)/20;
   }
  }
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GOOD_REGEN) && GET_POS(ch) != POS_STANDING)
    gain += GET_MAX_HIT(ch)/5;  

  if (PRF_FLAGGED(ch, PRF_CAMP))
    gain += GET_MAX_HIT(ch)/5;
  if (GET_HIT(ch) >= GET_MAX_HIT(ch)) {
    gain == 0;
  }
  if (GET_POS(ch) == POS_STANDING) {
    gain -= 0;
   }
  return (gain);
}



/* move gain pr. game hour */
int move_gain(struct char_data * ch)
{
  int gain;

  if (IS_NPC(ch)) {
    /* Neat and fast */
    gain = GET_LEVEL(ch);
  } else {
    gain = 0;

    /* Class/Level calculations */

    /* Skill/Spell calculations */


    /* Position calculations    */
    switch (GET_POS(ch)) {
    case POS_SLEEPING:
      gain += 0;	/* Divide by 2 */
      break;
    case POS_RESTING:
      gain += 0;	/* Divide by 4 */
      break;
    case POS_SITTING:
      gain += 0;	/* Divide by 8 */
      break;
    }

    if ((GET_COND(ch, FULL) == 0) || (GET_COND(ch, THIRST) == 0))
      gain += 0;
  }

  if (AFF_FLAGGED(ch, AFF_POISON))
    gain += 0;
  
  if (PRF_FLAGGED(ch, PRF_CAMP))
    gain += 0;

  return gain;
}



void set_title(struct char_data * ch, char *title)
{
  if (title == NULL) {
    if (GET_SEX(ch) == SEX_FEMALE)
      title = title_female(GET_CLASS(ch), GET_LEVEL(ch));
    else
      title = title_male(GET_CLASS(ch), GET_LEVEL(ch));
  }

  if (strlen(title) > MAX_TITLE_LENGTH)
    title[MAX_TITLE_LENGTH] = '\0';

  if (GET_TITLE(ch) != NULL)
    free(GET_TITLE(ch));

  GET_TITLE(ch) = str_dup(title);
}


void check_autowiz(struct char_data * ch)
{
#ifndef CIRCLE_UNIX
  return;
#else
  char buf[100];

  if (use_autowiz && GET_MAX_HIT(ch) >= 1000015) {
    sprintf(buf, "nice ../bin/autowiz %Ld %s %Ld %s %d &", min_wizlist_lev,
	    WIZLIST_FILE, LVL_IMMORT, IMMLIST_FILE, (int) getpid());
    mudlog("Initiating autowiz.", CMP, LVL_IMMORT, FALSE);
    system(buf);
  }
#endif /* CIRCLE_UNIX */
}



void gain_exp(struct char_data * ch, int gain)
{
  int is_altered = FALSE;
  int num_levels = 0;
  char buf[128];
    
  if (!IS_NPC(ch) && ((GET_LEVEL(ch) < 1 || GET_LEVEL(ch) >= LVL_IMMORT)))
    return;

  if (IS_NPC(ch)) {
    return;
  }
  if (gain > 0) {
    gain = MIN(max_exp_gain, gain);	/* put a cap on the max gain per kill */
    GET_EXP(ch) += gain;
    if (GET_EXP(ch) >= GET_LEVEL(ch) *2000 && GET_LEVEL(ch) <= 999998 && !CLN_FLAGGED(ch, CLN_Death)) {
	GET_LEVEL(ch) = GET_LEVEL(ch) + 1;
	GET_EXP(ch) = GET_EXP(ch) * .05;
	advance_level(ch);
	is_altered = TRUE;
	num_levels = 1;
    }
    else if (GET_EXP(ch) >= GET_LEVEL(ch)*1500 && GET_LEVEL(ch) <= 999998 && CLN_FLAGGED(ch, CLN_Death)) {
        GET_LEVEL(ch) = GET_LEVEL(ch) + 1;
        GET_EXP(ch) = GET_EXP(ch) * .05;
        advance_level(ch);
        is_altered = TRUE;
        num_levels = 1;
    }    
    else if (GET_EXP(ch) >= GET_LEVEL(ch)*2000 && GET_LEVEL(ch) >= 999999) {
        GET_LEVEL(ch) = GET_LEVEL(ch) - 1;
        GET_EXP(ch) = GET_EXP(ch) * .05;
        advance_level(ch);
        is_altered = TRUE;
        num_levels = 1;
    }
  } else if (gain < 0) {
    gain = MAX(-max_exp_loss, gain);	/* Cap max exp lost per death */
    GET_EXP(ch) += gain;
    if (GET_EXP(ch) < 0) {
      GET_EXP(ch) = 0;
    }
  }
}


void gain_exp_regardless(struct char_data * ch, int gain)
{
  int is_altered = FALSE;
  int num_levels = 0;

  GET_EXP(ch) += gain;
  if (GET_EXP(ch) < 0) {
    GET_EXP(ch) = 0;
  }
  if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000) {
      GET_LEVEL(ch) += 1;
      GET_EXP(ch) = 1;
      num_levels++;
      advance_level(ch);
      is_altered = TRUE;
    }

    if (is_altered) {
      if (num_levels == 1)
        send_to_char("&10You feel your power increase substantially.&00\r\n", ch);
      else {
	sprintf(buf, "You rise %d levels!\r\n", num_levels);
	send_to_char(buf, ch);
      }
      set_title(ch, NULL);
      check_autowiz(ch);
    }
}



void gain_condition(struct char_data * ch, int condition, int value)
{
  bool intoxicated;

  if (IS_NPC(ch) || GET_COND(ch, condition) == -1)	/* No change */
    return;

  intoxicated = (GET_COND(ch, DRUNK) > 0);

  GET_COND(ch, condition) += value;

  GET_COND(ch, condition) = MAX(0, GET_COND(ch, condition));
  GET_COND(ch, condition) = MIN(24, GET_COND(ch, condition));

  if (GET_COND(ch, condition) || PLR_FLAGGED(ch, PLR_WRITING))
    return;

  switch (condition) {
  case FULL:
    send_to_char("You are hungry.\r\n", ch);
    return;
  case THIRST:
    send_to_char("You are thirsty.\r\n", ch);
    return;
  case DRUNK:
    if (intoxicated)
      send_to_char("You are now sober.\r\n", ch);
    return;
  default:
    break;
  }

}


void check_idling(struct char_data * ch)
{
    struct obj_data *obj;
  for (obj = ch->carrying; obj; obj = obj->next_content) {
       if (IS_OBJ_STAT(obj, ITEM_FLAG)) {
        return;
       }
      }
  if (PRF_FLAGGED(ch, PRF_NOGRATZ)) {
    return;
  }
  else if (++(ch->char_specials.timer) > idle_void) {
     if (GET_WAS_IN(ch) == NOWHERE && ch->in_room != NOWHERE) {
      GET_WAS_IN(ch) = ch->in_room;
      if (FIGHTING(ch)) {
	stop_fighting(FIGHTING(ch));
	stop_fighting(ch);
      }
      act("$n disappears into the void.", TRUE, ch, 0, 0, TO_ROOM);
      send_to_char("You have been idle, and are pulled into a void.\r\n", ch);
      save_char(ch, NOWHERE);
      Crash_crashsave(ch);
      char_from_room(ch);
      char_to_room(ch, 1);
    } else if (ch->char_specials.timer > idle_rent_time) {
      if (ch->in_room != NOWHERE)
	char_from_room(ch);
      char_to_room(ch, 3);
      if (ch->desc) {
	STATE(ch->desc) = CON_DISCONNECT;
	/*
	 * For the 'if (d->character)' test in close_socket().
	 * -gg 3/1/98 (Happy anniversary.)
	 */
	ch->desc->character = NULL;
	ch->desc = NULL;
      }
      if (free_rent)
	Crash_rentsave(ch, 0);
      else
	Crash_idlesave(ch);
      sprintf(buf, "%s force-rented and extracted (idle).", GET_NAME(ch));
      mudlog(buf, CMP, LVL_GOD, TRUE);
      extract_char(ch, FALSE);
    }
  }
}



/* Update PCs, NPCs, and objects */
void point_update(void)
{
  struct char_data *i, *next_char;
  struct obj_data *j, *next_thing, *jj, *next_thing2;
  struct char_data * ch;

  /* characters */
  for (i = character_list; i; i = next_char) {
    next_char = i->next;
	
    gain_condition(i, FULL, -1);
    gain_condition(i, DRUNK, -1);
    gain_condition(i, THIRST, -1);
    if (GET_POS(i) >= POS_STUNNED) {
       GET_HIT(i) = MIN(GET_HIT(i) + hit_gain(i), GET_MAX_HIT(i));
       GET_MANA(i) = MIN(GET_MANA(i) + mana_gain(i), GET_MAX_MANA(i));
      if (AFF_FLAGGED(i, AFF_POISON))
	if (damage(i, i, 1, SPELL_POISON) == -1)
	  continue;	/* Oops, they died. -gg 6/24/98 */
      if (GET_POS(i) <= POS_STUNNED)
	update_pos(i);
    } else if (GET_POS(i) == POS_INCAP) {
      if (damage(i, i, 1, TYPE_SUFFERING) == -1)
	continue;
    } else if (GET_POS(i) == POS_MORTALLYW) {
      if (damage(i, i, 2, TYPE_SUFFERING) == -1)
	continue;
    }
    if (!IS_NPC(i)) {
      update_char_objects(i);
      if (GET_LEVEL(i) < idle_max_level)
	check_idling(i);
    }
  }

  /* objects */
  for (j = object_list; j; j = next_thing) {
    next_thing = j->next;	/* Next in object list */
    if (GET_OBJ_TYPE(j) == ITEM_TRAP || GET_OBJ_TYPE(j) == ITEM_FLOOD) {
      if (GET_OBJ_TIMER(j) > 0)
          GET_OBJ_TIMER(j)--;
      if (!GET_OBJ_TIMER(j)) {
       if ((j->in_room != NOWHERE) && (world[j->in_room].people) && GET_OBJ_TYPE(j) != ITEM_FLOOD) {
        act("A glowing portal fades from existance.", TRUE, world[j->in_room].people, j, 0, TO_ROOM);
        act("A glowing portal fades from existance.", TRUE, world[j->in_room].people, j, 0, TO_CHAR);
        extract_obj(j);
       }
       if ((j->in_room != NOWHERE) && (world[j->in_room].people) && GET_OBJ_TYPE(j) == ITEM_FLOOD) {
        act("The raging waters slowly begin to seep away... The room has been cleared!", TRUE, world[j->in_room].people, j, 0, TO_ROOM);
        act("The raging waters slowly begin to seep away... The room has been cleared!", TRUE, world[j->in_room].people, j, 0, TO_CHAR);
        extract_obj(j);
       }
      }
    }
    /* If this is a corpse */
    if (IS_CORPSE(j)) {
      /* timer count down */
      if (GET_OBJ_TIMER(j) > 0)
	GET_OBJ_TIMER(j)--;

      if (!GET_OBJ_TIMER(j)) {

	if (j->carried_by)
	  act("$p decays in your hands.", FALSE, j->carried_by, j, 0, TO_CHAR);
	else if ((j->in_room != NOWHERE) && (world[j->in_room].people)) {
	  act("A quivering horde of maggots consumes $p.",
	      TRUE, world[j->in_room].people, j, 0, TO_ROOM);
	  act("A quivering horde of maggots consumes $p.",
	      TRUE, world[j->in_room].people, j, 0, TO_CHAR);
	}
	for (jj = j->contains; jj; jj = next_thing2) {
	  next_thing2 = jj->next_content;	/* Next in inventory */
	  obj_from_obj(jj);

	  if (j->in_obj)
	    obj_to_obj(jj, j->in_obj);
	  else if (j->carried_by)
	    obj_to_room(jj, j->carried_by->in_room);
	  else if (j->in_room != NOWHERE)
	    obj_to_room(jj, j->in_room);
	  else
	    core_dump();
	}
	extract_obj(j);
      }
    }
    /* If the timer is set, count it down and at 0, try the trigger */
    /* note to .rej hand-patchers: make this last in your point-update() */
    else if (GET_OBJ_TIMER(j)>0) {
      GET_OBJ_TIMER(j)--; 
      if (!GET_OBJ_TIMER(j))
        timer_otrigger(j);
    }
  }
}
