/* ************************************************************************
*   File: spells.c                                      Part of CircleMUD *
*  Usage: Implementation of "manual spells".  Circle 2.2 spell compat.    *
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
#include "spells.h"
#include "handler.h"
#include "db.h"
#include "constants.h"

extern sh_int r_mortal_start_room;
extern int top_of_world;
extern struct spell_entry spells[];
extern struct room_data *world;
extern struct obj_data *object_list;
extern struct char_data *character_list;
extern struct index_data *obj_index;
extern struct descriptor_data *descriptor_list;
extern struct zone_data *zone_table;

extern int mini_mud;
extern int pk_allowed;

extern struct default_mobile_stats *mob_defaults;
extern struct apply_mod_defaults *apmd;

void clearMemory(struct char_data * ch);
void weight_change_object(struct obj_data * obj, int weight);
void add_follower(struct char_data * ch, struct char_data * leader);
int mag_savingthrow(struct char_data * ch, int type);
void name_to_drinkcon(struct obj_data * obj, int type);
void name_from_drinkcon(struct obj_data * obj);


/*
 * Special spells appear below.
 */

ASPELL(spell_create_water)
{
  int water;

  if (ch == NULL || obj == NULL)
    return;
  level = MAX(MIN(level, LVL_IMPL), 1);

  if (GET_OBJ_TYPE(obj) == ITEM_DRINKCON) {
    if ((GET_OBJ_VAL(obj, 2) != LIQ_WATER) && (GET_OBJ_VAL(obj, 1) != 0)) {
      name_from_drinkcon(obj);
      GET_OBJ_VAL(obj, 2) = LIQ_SLIME;
      name_to_drinkcon(obj, LIQ_SLIME);
    } else {
      water = MAX(GET_OBJ_VAL(obj, 0) - GET_OBJ_VAL(obj, 1), 0);
      if (water > 0) {
	if (GET_OBJ_VAL(obj, 1) >= 0)
	  name_from_drinkcon(obj);
	GET_OBJ_VAL(obj, 2) = LIQ_WATER;
	GET_OBJ_VAL(obj, 1) += water;
	name_to_drinkcon(obj, LIQ_WATER);
	weight_change_object(obj, water);
	act("$p is filled.", FALSE, ch, obj, 0, TO_CHAR);
      }
    }
  }
}

ASPELL(spell_gate)
{
  /* create a magic portal */
  struct obj_data *tmp_obj, *tmp_obj2;
  struct extra_descr_data *ed;
  struct room_data *rp, *nrp;
  struct char_data *tmp_ch = (struct char_data *) victim;
  char buf[512];

  assert(ch);
  assert((level >= 0) && (level <= 1000015));


  /*
    check target room for legality.
   */
  rp = &world[ch->in_room];
  tmp_obj = read_object(20, VIRTUAL);
  if (!rp || !tmp_obj) {
    send_to_char("The magic fails\n\r", ch);
    extract_obj(tmp_obj);
    return;
  }
  if (IS_SET(rp->room_flags, ROOM_TUNNEL)) {
    send_to_char("There is no room in here to summon!\n\r", ch);
    extract_obj(tmp_obj);
    return;
  }

  if (!(nrp = &world[tmp_ch->in_room])) {
    char str[180];
    sprintf(str, "%s not in any room", GET_NAME(tmp_ch));
    log(str);
    send_to_char("The magic cannot locate the target\n", ch);
    extract_obj(tmp_obj);
    return;
  }

  if (ROOM_FLAGGED(tmp_ch->in_room, ROOM_NOMAGIC)) {
    send_to_char("Your target is protected against your magic.\r\n", ch);
    extract_obj(tmp_obj);
    return;
  }
  else if (PLR_FLAGGED(tmp_ch, PLR_KILLER)) {
   send_to_char("You can't enter the arena that way.", ch);
    extract_obj(tmp_obj);
   return;
  }
  else if (PLR_FLAGGED(ch, PLR_KILLER)) {
   send_to_char("You can't portal out of the arena.", ch);
    extract_obj(tmp_obj);
   return;
  }
  else if (GET_LEVEL(tmp_ch) >= 1000000) {
   send_to_char("You can't portal to immortals...", ch);
   extract_obj(tmp_obj);
   return;
  }
  else if (MOB_FLAGGED(tmp_ch, MOB_NOSOLAR_FLARE)) {
   send_to_char("You can not portal to that kind of mob chump.\r\n", ch);
   extract_obj(tmp_obj);
   return;
  }
  else if (GET_MANA(ch) < GET_MAX_MANA(ch)) {
   send_to_char("You do not have enough ki, you need full ki.", ch);
   extract_obj(tmp_obj);
   return;
  }
  else if (number(1, 20) <= 5) {
   send_to_char("Summoning the portal fails...", ch);
   GET_MANA(ch) -= GET_MANA(ch);
   extract_obj(tmp_obj);
   return;
  }
  sprintf(buf, "&15Through the mists of the portal, you can faintly see %s",nrp->name);
  GET_MANA(ch) -= GET_MAX_MANA(ch);
  CREATE(ed , struct extra_descr_data, 1);
  ed->next = tmp_obj->ex_description;
  tmp_obj->ex_description = ed;
  CREATE(ed->keyword, char, strlen(tmp_obj->name) + 1);
  strcpy(ed->keyword, tmp_obj->name);
  ed->description = str_dup(buf);

  tmp_obj->obj_flags.value[0] = tmp_ch->in_room;
  GET_OBJ_TIMER(tmp_obj) = 2;
  GET_OBJ_TYPE(tmp_obj2) == ITEM_TRAP;
  obj_to_room(tmp_obj,ch->in_room);
  
  act("$p &15suddenly appears.\r\n",TRUE,ch,tmp_obj,0,TO_ROOM);
  act("$p &15suddenly appears.\r\n",TRUE,ch,tmp_obj,0,TO_CHAR);

/* Portal at other side */
   rp = &world[ch->in_room];
   tmp_obj2 = read_object(20, VIRTUAL);
   if (!rp || !tmp_obj2) {
     send_to_char("The magic fails\n\r", ch);
     extract_obj(tmp_obj2);
     return;
   }
  sprintf(buf,"&15Through the mists of the portal, you can faintly see %s\r\n", rp->name);

  CREATE(ed , struct extra_descr_data, 1);
  ed->next = tmp_obj2->ex_description;
  tmp_obj2->ex_description = ed;
  CREATE(ed->keyword, char, strlen(tmp_obj2->name) + 1);
  strcpy(ed->keyword, tmp_obj2->name);
  ed->description = str_dup(buf);
  tmp_obj2->obj_flags.value[0] = ch->in_room;
  GET_OBJ_TIMER(tmp_obj2) = 2;
  GET_OBJ_TYPE(tmp_obj2) == ITEM_TRAP;
  obj_to_room(tmp_obj2,tmp_ch->in_room);
  act("$p &15suddenly appears.\r\n", TRUE, tmp_ch, tmp_obj2, 0, TO_ROOM);
  act("$p &15suddenly appears.\r\n", TRUE, tmp_ch, tmp_obj2, 0, TO_CHAR);
}



ASPELL(spell_recall)
{
  if (victim == NULL || IS_NPC(victim))
    return;

  act("$n disappears.", TRUE, victim, 0, 0, TO_ROOM);
  char_from_room(victim);
  char_to_room(victim, r_mortal_start_room);
  act("$n appears in the middle of the room.", TRUE, victim, 0, 0, TO_ROOM);
  look_at_room(victim, 0);
}


ASPELL(spell_teleport)
{
  int to_room;

  if (victim == NULL || IS_NPC(victim))
    return;
  if (MOB_FLAGGED(victim, MOB_NOSOLAR_FLARE)){
    act("You fail, the gods forbid teleporting that entity!", FALSE, ch, 0, 0, TO_CHAR);
    return;
    }

  do {
    to_room = number(0, top_of_world);
  } while (ROOM_FLAGGED(to_room, ROOM_PRIVATE | ROOM_DEATH));

  act("$n disappears in a puff of smoke.",
      FALSE, victim, 0, 0, TO_ROOM);
  char_from_room(victim);
  char_to_room(victim, to_room);
  act("$n slowly fades into existence.", FALSE, victim, 0, 0, TO_ROOM);
  look_at_room(victim, 0);

}

#define SUMMON_FAIL "You failed.\r\n"

ASPELL(spell_summon)
{
  if (ch == NULL || victim == NULL)
    return;

  if (GET_LEVEL(victim) > MIN(LVL_IMMORT - 1, level + 3)) {
    send_to_char(SUMMON_FAIL, ch);
    return;
  }
  if (MOB_FLAGGED(victim, MOB_NOSOLAR_FLARE)){
    send_to_char(SUMMON_FAIL, ch);
    return;
    }
  if (!pk_allowed) {
    if (MOB_FLAGGED(victim, MOB_AGGRESSIVE)) {
      act("As the words escape your lips and $N travels\r\n"
	  "through time and space towards you, you realize that $E is\r\n"
	  "aggressive and might harm you, so you wisely send $M back.",
	  FALSE, ch, 0, victim, TO_CHAR);
      return;
    }
  }

  act("$n disappears suddenly.", TRUE, victim, 0, 0, TO_ROOM);

  char_from_room(victim);
  char_to_room(victim, ch->in_room);

  act("$n arrives suddenly.", TRUE, victim, 0, 0, TO_ROOM);
  act("$n has summoned you!", FALSE, ch, 0, victim, TO_VICT);
  GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/80;
  look_at_room(victim, 0);
}



ASPELL(spell_locate_object)
{
  struct obj_data *i;
  char name[MAX_INPUT_LENGTH];
  int j;

  /*
   * FIXME: This is broken.  The spell parser routines took the argument
   * the player gave to the spell and located an object with that keyword.
   * Since we're passed the object and not the keyword we can only guess
   * at what the player originally meant to search for. -gg
   */
  strcpy(name, fname(obj->name));
  j = level / 2;

  for (i = object_list; i && (j > 0); i = i->next) {
    if (!isname(name, i->name))
      continue;

    if (i->carried_by)
      sprintf(buf, "%s is being carried by %s.\r\n",
	      i->short_description, PERS(i->carried_by, ch));
    else if (i->in_room != NOWHERE)
      sprintf(buf, "%s is in %s.\r\n", i->short_description,
	      world[i->in_room].name);
    else if (i->in_obj)
      sprintf(buf, "%s is in %s.\r\n", i->short_description,
	      i->in_obj->short_description);
    else if (i->worn_by)
      sprintf(buf, "%s is being worn by %s.\r\n",
	      i->short_description, PERS(i->worn_by, ch));
    else
      sprintf(buf, "%s's location is uncertain.\r\n",
	      i->short_description);

    CAP(buf);
    send_to_char(buf, ch);
    j--;
  }

  if (j == level / 2)
    send_to_char("You sense nothing.\r\n", ch);
}



ASPELL(spell_MAJIN)
{
  struct affected_type af;

  if (victim == NULL || ch == NULL)
    return;
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 20) {
   act("You don't have enough ki!", FALSE, ch, 0, 0, TO_CHAR);
   return;
  }
  if(!IS_NPC(victim)) { 
    act("You can't majinize players!", FALSE, ch, 0, 0, TO_CHAR);
    return;
  }
  if (victim == ch)
    send_to_char("You like yourself even better!\r\n", ch);
  else if (AFF_FLAGGED(victim, AFF_BARRIER))
    send_to_char("Your victim is protected by barrier!\r\n", ch);
  else if (MOB_FLAGGED(victim, MOB_NOMAJIN))
    send_to_char("Your victim resists!\r\n", ch);
  else if (GET_MAX_HIT(victim) >= GET_MAX_HIT(ch))
    send_to_char("Your victim is to powerful for you!\r\n", ch);
  else if (AFF_FLAGGED(ch, AFF_MAJIN))
    send_to_char("You can't have any followers of your own!\r\n", ch);
  else if (AFF_FLAGGED(victim, AFF_MAJIN) || level < GET_LEVEL(victim))
    send_to_char("You fail.\r\n", ch);
  /* player MAJINing another player - no legal reason for this */
  else if (!pk_allowed && !IS_NPC(victim))
    send_to_char("You fail - shouldn't be doing it anyway.\r\n", ch);
  else if (circle_follow(victim, ch))
    send_to_char("Sorry, following in circles can not be allowed.\r\n", ch);
  else if (mag_savingthrow(victim, SAVING_PARA))
    send_to_char("Your victim resists!\r\n", ch);
  else {
    if (victim->master)
      stop_follower(victim);

    add_follower(victim, ch);
    act("They now follow you!", FALSE, ch, 0, 0, TO_CHAR);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/20;
    af.type = SPELL_MAJIN;

    if (GET_INT(victim))
      af.duration = 24 * 18 / GET_INT(victim);
    else
      af.duration = 24 * 18;

    af.modifier = 0;
    af.location = 0;
    af.bitvector = AFF_MAJIN;
    affect_to_char(victim, &af);

    act("$n concentrates and an &09M&00 appears on your forehead.", FALSE, ch, 0, victim, TO_VICT);
    if (IS_NPC(victim)) {
      REMOVE_BIT(MOB_FLAGS(victim), MOB_AGGRESSIVE);
      REMOVE_BIT(MOB_FLAGS(victim), MOB_SPEC);
    }
  }
}



ASPELL(spell_IDENTIFY)
{
  int i;
  int found;

  if (obj) {
    send_to_char("You feel informed:\r\n", ch);
    sprintf(buf, "Object '%s', Item type: ", obj->short_description);
    sprinttype(GET_OBJ_TYPE(obj), item_types, buf2);
    strcat(buf, buf2);
    strcat(buf, "\r\n");
    send_to_char(buf, ch);

    if (obj->obj_flags.bitvector) {
      send_to_char("Item will give you following abilities:  ", ch);
      sprintbit(obj->obj_flags.bitvector, affected_bits, buf);
      strcat(buf, "\r\n");
      send_to_char(buf, ch);
    }
    send_to_char("Item is: ", ch);
    sprintbit(GET_OBJ_EXTRA(obj), extra_bits, buf);
    strcat(buf, "\r\n");
    send_to_char(buf, ch);

    sprintf(buf, "Weight: %d, Value: %d, Rent: %d\r\n",
	    GET_OBJ_WEIGHT(obj), GET_OBJ_COST(obj), GET_OBJ_RENT(obj));
    send_to_char(buf, ch);

    switch (GET_OBJ_TYPE(obj)) {
    case ITEM_SENZU:
    case ITEM_NANO:
    case ITEM_SAIBAKIT:
      sprintf(buf, "This %s focuss: ", item_types[(int) GET_OBJ_TYPE(obj)]);

      if (GET_OBJ_VAL(obj, 1) >= 1)
	sprintf(buf + strlen(buf), " %s", spells[GET_OBJ_VAL(obj, 1)].name);
      if (GET_OBJ_VAL(obj, 2) >= 1)
	sprintf(buf + strlen(buf), " %s", spells[GET_OBJ_VAL(obj, 2)].name);
      if (GET_OBJ_VAL(obj, 3) >= 1)
	sprintf(buf + strlen(buf), " %s", spells[GET_OBJ_VAL(obj, 3)].name);
      strcat(buf, "\r\n");
      send_to_char(buf, ch);
      break;
    case ITEM_SCOUTER:
    case ITEM_STAFF:
      sprintf(buf, "This %s focus': ", item_types[(int) GET_OBJ_TYPE(obj)]);
      sprintf(buf + strlen(buf), " %s\r\n", spells[GET_OBJ_VAL(obj, 3)].name);
      sprintf(buf + strlen(buf), "It has %d maximum charge%s and %d remaining.\r\n",
	      GET_OBJ_VAL(obj, 1), GET_OBJ_VAL(obj, 1) == 1 ? "" : "s",
	      GET_OBJ_VAL(obj, 2));
      send_to_char(buf, ch);
      break;
    case ITEM_WEAPON:
      sprintf(buf, "Damage Dice is '%dD%d'", GET_OBJ_VAL(obj, 1),
	      GET_OBJ_VAL(obj, 2));
      sprintf(buf + strlen(buf), " for an average per-round damage of %.1f.\r\n",
	      (((GET_OBJ_VAL(obj, 2) + 1) / 2.0) * GET_OBJ_VAL(obj, 1)));
      send_to_char(buf, ch);
      break;
    case ITEM_ARMOR:
      sprintf(buf, "AC-apply is %d\r\n", GET_OBJ_VAL(obj, 0));
      send_to_char(buf, ch);
      break;
    }
    found = FALSE;
    for (i = 0; i < MAX_OBJ_AFFECT; i++) {
      if ((obj->affected[i].location != APPLY_NONE) &&
	  (obj->affected[i].modifier != 0)) {
	if (!found) {
	  send_to_char("Can affect you as :\r\n", ch);
	  found = TRUE;
	}
	sprinttype(obj->affected[i].location, apply_types, buf2);
	sprintf(buf, "   Affects: %s By %d\r\n", buf2, obj->affected[i].modifier);
	send_to_char(buf, ch);
      }
    }
  } else if (victim) {		/* victim */
    sprintf(buf, "Name: %s\r\n", GET_NAME(victim));
    send_to_char(buf, ch);
    if (!IS_NPC(victim)) {
      sprintf(buf, "%s is %d years, %d months, %d days and %d hours old.\r\n",
	      GET_NAME(victim), age(victim)->year, age(victim)->month,
	      age(victim)->day, age(victim)->hours);
      send_to_char(buf, ch);
    }
    sprintf(buf, "Height %d cm, Weight %d pounds\r\n",
	    GET_HEIGHT(victim), GET_WEIGHT(victim));
    sprintf(buf + strlen(buf), "Level: %Ld, Hits: %d, Mana: %d\r\n",
	    GET_LEVEL(victim), GET_HIT(victim), GET_MANA(victim));
    sprintf(buf + strlen(buf), "AC: %d, Hitroll: %d, Damroll: %d\r\n",
	    GET_AC(victim), GET_HITROLL(victim), GET_DAMROLL(victim));
    sprintf(buf + strlen(buf), "Str: %d/%d, Int: %d, Wis: %d, Dex: %d, Con: %d, Cha: %d\r\n",
	GET_STR(victim), GET_ADD(victim), GET_INT(victim),
	GET_WIS(victim), GET_DEX(victim), GET_CON(victim), GET_CHA(victim));
    send_to_char(buf, ch);

  }
}


ASPELL(spell_POWER_SENSE)
{
   if (IS_ANDROID(victim)) {
    send_to_char("You can't use scouters on androids!\r\n", ch);
    return;
   }
  else
   if (victim) {		/* victim */
act("&10 _____________________     &00", FALSE, ch, 0, victim, TO_CHAR);
act("&10//&20                    &00", FALSE, ch, 0, victim, TO_CHAR);
act("&10|]&20                    &00", FALSE, ch, 0, victim, TO_CHAR);
act("&10|]&20                    &00", FALSE, ch, 0, victim, TO_CHAR);
act("&10|]&20&09Loading&16...       &10   &00", FALSE, ch, 0, victim, TO_CHAR);
act("&10|]&20                    &00", FALSE, ch, 0, victim, TO_CHAR);
act("&10|]&20____________________&00  ", FALSE, ch, 0, victim, TO_CHAR);
act("&09...................... []&00", FALSE, ch, 0, victim, TO_CHAR);
    sprintf(buf, "&10PL&15: &11%d&15&00\r\n", GET_HIT(victim));
     send_to_char(buf, ch);
    sprintf(buf, "&10Ki&15: &11%d&15&00\r\n", GET_MANA(victim));
     send_to_char(buf, ch);
   if (!IS_NPC(victim)) {
    sprintf(buf, "&10Str&15: &11%d&15&00\r\n", GET_STR(victim));
     send_to_char(buf, ch);
    sprintf(buf, "&10Con&15: &11%d&15&00\r\n", GET_CON(victim));
     send_to_char(buf, ch);
    sprintf(buf, "&10Int&15: &11%d&15&00\r\n", GET_INT(victim));
     send_to_char(buf, ch);
    sprintf(buf, "&10Wis&15: &11%d&15&00\r\n", GET_WIS(victim));
     send_to_char(buf, ch);
    sprintf(buf, "&10Dex&15: &11%d&15&00\r\n", GET_DEX(victim));
     send_to_char(buf, ch);
    sprintf(buf, "&10Speed&15: &11%d&15&00\r\n", GET_RAGE(victim));
     send_to_char(buf, ch);
    }
   }
}


ASPELL(spell_BURNING_SOULS)
{
  int i;

  if (ch == NULL || obj == NULL)
    return;

}

ASPELL(spell_detect_poison)
{
  if (victim) {
    if (victim == ch) {
      if (AFF_FLAGGED(victim, AFF_POISON))
        send_to_char("You can sense poison in your blood.\r\n", ch);
      else
        send_to_char("You feel healthy.\r\n", ch);
    } else {
      if (AFF_FLAGGED(victim, AFF_POISON))
        act("You sense that $E is poisoned.", FALSE, ch, 0, victim, TO_CHAR);
      else
        act("You sense that $E is healthy.", FALSE, ch, 0, victim, TO_CHAR);
    }
  }

  if (obj) {
    switch (GET_OBJ_TYPE(obj)) {
    case ITEM_DRINKCON:
    case ITEM_FOUNTAIN:
    case ITEM_FOOD:
      if (GET_OBJ_VAL(obj, 3))
	act("You sense that $p has been contaminated.",FALSE,ch,obj,0,TO_CHAR);
      else
	act("You sense that $p is safe for consumption.", FALSE, ch, obj, 0,
	    TO_CHAR);
      break;
    default:
      send_to_char("You sense that it should not be consumed.\r\n", ch);
    }
  }
}
