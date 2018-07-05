



/* ************************************************************************
*   File: magic.c                                       Part of CircleMUD *
*  Usage: low-level functions for magic; spell template code              *
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
#include "dg_scripts.h"

extern struct room_data *world;
extern struct obj_data *object_list;
extern struct char_data *character_list;
extern struct index_data *obj_index;

extern struct descriptor_data *descriptor_list;
extern struct zone_data *zone_table;

extern int mini_mud;
extern int pk_allowed;
extern char *spell_wear_off_msg[];
extern struct default_mobile_stats *mob_defaults;
extern struct apply_mod_defaults *apmd;
extern sh_int r_death_start_room;

void clearMemory(struct char_data * ch);
void weight_change_object(struct obj_data * obj, int weight);
void add_follower(struct char_data * ch, struct char_data * leader);
extern struct spell_info_type spell_info[];

/* local functions */
int mag_materials(struct char_data * ch, int item0, int item1, int item2, int extract, int verbose);
void perform_mag_groups(int level, struct char_data * ch, struct char_data * tch, int spellnum, int savetype);
int mag_savingthrow(struct char_data * ch, int type);
void affect_update(void);

/*
 * Saving throws are now in class.c (bpl13)
 */

int mag_savingthrow(struct char_data * ch, int type)
{
  int save;

  /* negative apply_saving_throw values make saving throws better! */

  /* throwing a 0 is always a failure */
  if (MAX(1, 100) < number(99, 0))
    return TRUE;
  else
    return FALSE;
}


/* affect_update: called from comm.c (causes spells to wear off) */
void affect_update(void)
{
  struct affected_type *af, *next;
  struct char_data *i;

  for (i = character_list; i; i = i->next)
    for (af = i->affected; af; af = next) {
      next = af->next;
      if (af->duration >= 1)
	af->duration--;
      else {
	if ((af->type > 0) && (af->type <= MAX_SPELLS))
	  if (!af->next || (af->next->type != af->type) ||
	      (af->next->duration > 0))
	    if (*spell_wear_off_msg[af->type]) {
              send_to_char(spell_wear_off_msg[af->type], i);
	      send_to_char("\r\n", i);
	    }
	affect_remove(i, af);
      }
    }
}


/*
 *  mag_materials:
 *  Checks for up to 3 vnums (spell reagents) in the player's inventory.
 *
 * No spells implemented in Circle 3.0 use mag_materials, but you can use
 * it to implement your own spells which require ingredients (i.e., some
 * heal spell which requires a rare herb or some such.)
 */
int mag_materials(struct char_data * ch, int item0, int item1, int item2,
		      int extract, int verbose)
{
  struct obj_data *tobj;
  struct obj_data *obj0 = NULL, *obj1 = NULL, *obj2 = NULL;

  for (tobj = ch->carrying; tobj; tobj = tobj->next_content) {
    if ((item0 > 0) && (GET_OBJ_VNUM(tobj) == item0)) {
      obj0 = tobj;
      item0 = -1;
    } else if ((item1 > 0) && (GET_OBJ_VNUM(tobj) == item1)) {
      obj1 = tobj;
      item1 = -1;
    } else if ((item2 > 0) && (GET_OBJ_VNUM(tobj) == item2)) {
      obj2 = tobj;
      item2 = -1;
    }
  }
  if ((item0 > 0) || (item1 > 0) || (item2 > 0)) {
    if (verbose) {
      switch (number(0, 2)) {
      case 0:
	send_to_char("A wart sprouts on your nose.\r\n", ch);
	break;
      case 1:
	send_to_char("Your hair falls out in clumps.\r\n", ch);
	break;
      case 2:
	send_to_char("A huge corn develops on your big toe.\r\n", ch);
	break;
      }
    }
    return (FALSE);
  }
  if (extract) {
    if (item0 < 0) {
      obj_from_char(obj0);
      extract_obj(obj0);
    }
    if (item1 < 0) {
      obj_from_char(obj1);
      extract_obj(obj1);
    }
    if (item2 < 0) {
      obj_from_char(obj2);
      extract_obj(obj2);
    }
  }
  if (verbose) {
    send_to_char("A puff of smoke rises from your pack.\r\n", ch);
    act("A puff of smoke rises from $n's pack.", TRUE, ch, NULL, NULL, TO_ROOM);
  }
  return (TRUE);
}




/*
 * Every spell that does damage comes through here.  This calculates the
 * amount of damage, adds in any modifiers, determines what the saves are,
 * tests for save and calls damage().
 *
 * -1 = dead, otherwise the amount of damage done.
 */
int mag_damage(int level, struct char_data * ch, struct char_data * victim,
		     int spellnum, int savetype)
{
  int dam = 0;

  if (victim == NULL || ch == NULL)
    return 0;

  switch (spellnum) {

  case SPELL_DISPEL_GOOD:
	 dam = dice(6, 8) + 6;
	 if (IS_GOOD(ch)) {
		victim = ch;
		dam = GET_HIT(ch) - 1;
	 } else if (IS_EVIL(victim)) {
		act("The gods protect $N.", FALSE, ch, 0, victim, TO_CHAR);
		dam = 0;
		return 0;
	 }
	 break;
 
  } /* switch(spellnum) */


  /* divide damage by two if victim makes his saving throw */
  if (mag_savingthrow(victim, savetype))
	 dam /= 2;

  /* and finally, inflict the damage */
  return damage(ch, victim, dam, spellnum);
}


/*
 * Every spell that does an affect comes through here.  This determines
 * the effect, whether it is added or replacement, whether it is legal or
 * not, etc.
 *
 * affect_join(vict, aff, add_dur, avg_dur, add_mod, avg_mod)
*/

#define MAX_SPELL_AFFECTS 7	/* change if more needed */

void mag_affects(int level, struct char_data * ch, struct char_data * victim,
		      int spellnum, int savetype)
{
    
  struct affected_type af[MAX_SPELL_AFFECTS];
  bool accum_affect = FALSE, accum_duration = FALSE;
  const char *to_vict = NULL, *to_room = NULL;
  int i;
  int spell = 0;

  if (victim == NULL || ch == NULL)
    return;
  if (GET_MANA(ch) < GET_MAX_MANA(ch)){
    switch (spellnum) {
     case SPELL_PLATED:
     act("You don't have enough ki!", FALSE, ch, 0, 0, TO_CHAR);
     return;
     break;
   }
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)){
    switch (spellnum) {
     case SPELL_FRENZY:
     act("You don't have enough ki!", FALSE, ch, 0, 0, TO_CHAR);
     return;
     break;
   }
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 8){
    switch (spellnum) {
     case SPELL_CALM:
     act("You don't have enough ki!", FALSE, ch, 0, 0, TO_CHAR);
     return;
     break;
   }
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 4){
    switch (spellnum) {
     case SPELL_IREG:
     act("You don't have enough ki!", FALSE, ch, 0, 0, TO_CHAR);
     return;
     break;
   }
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 80){
    switch (spellnum) {
     case SPELL_armor:
     case SPELL_HASSHUKEN:
     case SPELL_ZANZOKEN:
     case SPELL_FOCUS:
     case SPELL_TSURUGI:
     case SPELL_MAGIC:
     case SPELL_BLADEMORPH:
     case SPELL_MUTATE:
     case SPELL_SOLAR_FLARE:
     case SPELL_DETECT_INVISIBILTY:
     case SPELL_CURSE:
     case SPELL_INFRAVISION:
     case SPELL_INVISIBLE:
     case SPELL_POISON:
     case SPELL_BARRIER:
     case SPELL_YOIKOMINMINKEN:
     act("You don't have enough ki!", FALSE, ch, 0, 0, TO_CHAR);
     return;
     break;
    }
   }
  for (i = 0; i < MAX_SPELL_AFFECTS; i++) {
    af[i].type = spellnum;
    af[i].bitvector = 0;
    af[i].modifier = 0;
    af[i].location = APPLY_NONE;
  }
  switch (spellnum) {

  case SPELL_armor:
    af[0].location = APPLY_AC;
    af[0].modifier = -20;
    af[0].duration = 24;
    accum_duration = TRUE;
    to_vict = "You start glowing with a blue outline of ki.";
    break;
  
  case SPELL_HASSHUKEN:
    af[0].location = APPLY_RAGE;
    af[0].modifier = 20;
    af[0].duration = 6;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;

    accum_duration = TRUE;
    to_room = "&14$n &15moves $s &15arms so fast they look like 8.&00";
    to_vict = "&15You move your arms at such speed they look like 8 arms.&00";
    break;
  case SPELL_STONED:
    af[0].duration = 5;
    af[0].bitvector = AFF_STONED;

    af[0].location = APPLY_INT;
    af[0].modifier = -5;
    af[0].duration = 5;
    af[0].location = APPLY_WIS;
    af[0].modifier = 5;
    af[0].duration = 5;
    accum_duration = FALSE;
    send_to_char("&15You feel really stoned now...&00", ch);
    break;
  case SPELL_FOCUS:
    af[0].location = APPLY_KISKILL;
    af[0].modifier = 5;
    af[0].duration = 10;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;

    accum_duration = TRUE;
    to_room = "&14$n's&15 eyes flash a bright blue for a moment as they concentrate on their aim.&00";
    to_vict = "&15Your eyes flash a bright blue for a moment as he concentrate on their aim.&00";
    break;
  case SPELL_ZANZOKEN:
    af[0].location = APPLY_DEX;
    af[0].location = APPLY_RAGE;
    af[0].modifier = 20;
    af[0].duration = 14;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;

    accum_duration = TRUE;
    to_room = "&14$n &15begins moving at &09u&16n&15b&00&07e&00&01l&16i&00&01e&00&07v&15a&16bl&09e &16s&00&02p&10ee&00&02d&16s&15.&00";
    to_vict = "&15You &15begin moving at &09u&16n&15b&00&07e&00&01l&16i&00&01e&00&07v&15a&16bl&09e &16s&00&02p&10ee&00&02d&16s&15.&00";
    break;
  
  case SPELL_TSURUGI:
    af[0].location = APPLY_CON;
    af[0].modifier = 1;    
    af[0].duration = 8;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;
    
    accum_duration = FALSE;
    to_room = "&14$n hands &10glow &11yellow as &12ki &16b&15l&00&07a&15d&16e&15s form around them.&00";
    to_vict = "&15Your hands begin to glow yellow as &12ki &16b&15l&00&07a&15d&16e&15s form around them.&00";
    break;
   case SPELL_MAGIC:
    af[0].location = APPLY_INT;
    af[0].modifier = 2;
    af[0].duration = 8;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;

    accum_duration = FALSE;
    to_room = "&14$n &15chants as they raise a hand and a &11b&00&03e&00&07a&11m&15 of &00&07l&15i&16g&15h&00&07t&15 envelopes their body&00";
    to_vict = "&15You chant and raise a hand, a &11b&00&03e&00&07a&11m&15 of &00&07l&15i&16g&15h&00&07t&15 envelopes your body and bestows you with more intelligence.&00";
    break;
  case SPELL_BLADEMORPH:
    af[0].location = APPLY_HITROLL;
    af[0].modifier = 20;
    af[0].duration = 8;
    
    af[1].location = APPLY_DAMROLL;
    af[1].modifier = 20;
    af[1].duration = 8;
    
    accum_duration = TRUE;
    to_room = "&14$n's&15 hands grow &16b&15l&00&07a&15d&16e&15 like claws!&00";
    to_vict = "&15Your hands grow &16b&15l&00&07a&15d&16e&15 like claws!&00";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;
    break;
  case SPELL_PLATED:
    af[0].duration = 1;
    af[0].bitvector = AFF_PLATED;

    accum_duration = FALSE;
    to_vict = "&15Your skin begins to grow hard bone plates!&00";
    to_room = "&14$n's&15 skin begins to grow hard bone plates!&00";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch);
    break;
  case SPELL_CALM:
    af[0].duration = 3;
    af[0].bitvector = AFF_CALM;

    accum_duration = FALSE;
    to_vict = "&15You stop and beging to concentrate, bringing the depths of your mind to complete peace.&00";
    to_room = "&14$n&15 stops suddenly, and begins to concentrate. $s eyes &12g&00&04l&16o&12w&15 for a few moments before a grin crosses $s face.&00";
    GET_MANA(ch) = GET_MANA(ch) - (GET_MAX_MANA(ch) / 8);
    break;
  case SPELL_FRENZY:
    af[0].duration = 1;
    af[0].bitvector = AFF_FRENZY;

    accum_duration = FALSE;
    to_vict = "&15Your mind snaps and you pour every last bit of energy into a blood thirsty frenzy!&00";
    to_room = "&14$n's&15 mind snaps and $e pours every last bit of $s energy into a blood thirsty frenzy!&00";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch);
    break;
  case SPELL_MUTATE:
    af[0].duration = 4;
    af[0].bitvector = AFF_BARRIER;
    
    af[0].location = APPLY_AC;
    af[0].modifier = -20;
    af[0].duration = 10;
    accum_duration = FALSE;
    to_vict = "&15You begin to &00&05mutate&15 your body in a defensive state.&00";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;
    break;
  case SPELL_KYODAIKA:
    af[0].location = APPLY_HIT;
    af[0].modifier = +50;
    af[0].duration = 8;
    
    af[1].location = APPLY_MANA;
    af[1].modifier = +50;
    af[1].duration = 8;

    af[2].location = APPLY_STR;
    af[2].modifier = +2;
    af[2].duration = 8;

    accum_duration = TRUE;
    to_room = "$n begins to ripple with bulging muscles!";
    to_vict = "You begin to ripple with bulging muscles and grow larger in size!.";
    break;

  case SPELL_SOLAR_FLARE:
    if (MOB_FLAGGED(victim,MOB_NOSOLAR_FLARE) || mag_savingthrow(victim, savetype)) {
      send_to_char("You fail.\r\n", ch);
      return;
    }
    if (!IS_NPC(victim) && GET_MAX_HIT(victim) <= 4999999) {
      return;
    }
    if (!IS_NPC(ch) && GET_MAX_HIT(ch) <= 4999999) {
      return;
    }
    af[0].location = APPLY_HITROLL;
    af[0].modifier = -4;
    af[0].duration = 2;
    af[0].bitvector = AFF_SOLAR_FLARE;

    af[1].location = APPLY_AC;
    af[1].modifier = 40;
    af[1].duration = 2;
    af[1].bitvector = AFF_SOLAR_FLARE;
   
    act("&14$N&15 yells &00&07'&00&03S&11o&00&07l&11a&00&03r &16f&00&01l&09a&00&01r&16e&15!&00'&15 and suddenly you can't see!&00", FALSE, victim, 0, ch, TO_CHAR);
    act("&15You yell &00&07'&00&03S&11o&00&07l&11a&00&03r &16f&00&01l&09a&00&01r&16e&15!&00' &15and blind $n!&00", FALSE, victim, 0, ch, TO_VICT);
    act("&14$n&15 is blinded by a &00&03f&11l&00&07a&11s&00&03h&15 of light as &10$N&15 yells &00&07'&00&03S&11o&00&07l&11a&00&03r &16f&00&01l&09a&00&01r&16e&15!&00'&00", FALSE, victim, 0, ch, TO_NOTVICT);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;
    break;

  case SPELL_CURSE:
    if (mag_savingthrow(victim, savetype)) {
      send_to_char(NOEFFECT, ch);
      return;
    }
    
    if (AFF_FLAGGED(victim, AFF_PROTECT_EVIL)) {
      send_to_char("&15They are protected from evil powers!&00\r\n", ch);
      act("&14$n&15's curse is reflected off of your protection enchantment.&00", FALSE, ch, 0, victim, TO_VICT);
      act("&14$n&15's curse is reflected off of &10$N&15's protection enchantment&00", FALSE, ch, 0, victim, TO_NOTVICT);
      return;
    }
    af[0].location = APPLY_DEX;
    af[0].duration = 12;
    af[0].modifier = -30;
    af[0].bitvector = AFF_CURSE;

    af[1].location = APPLY_RAGE;
    af[1].duration = 12;
    af[1].modifier = -30;
    af[1].bitvector = AFF_CURSE;

    accum_duration = FALSE;
    accum_affect = FALSE;
    to_room = "&14$n &15briefly &11g&00&03low&11s &09r&00&01e&09d&15!&00";
    to_vict = "&15You feel very uncomfortable..... You've been &16c&00&01u&09r&15s&00&01e&16d!&00";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;
    break;

  
  case SPELL_DETECT_INVISIBILTY:
    af[0].duration = 10;
    af[0].bitvector = AFF_DETECT_INVISIBILTY;
    accum_duration = TRUE;
    to_vict = "&15Your eyes &10t&00&02i&00&06ng&00&02l&10e&15.&00";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;
    break;

  case SPELL_IREG:
    af[0].duration = 4;
    af[0].bitvector = AFF_IREG;
    accum_duration = FALSE;
    to_vict = "&15You concentrate and prepare your metabolism to regrow limbs instantly.&00";
    to_room = "&15$n concentrates and prepares $s metabolism to regrow limbs instantly.&00&00";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 4;
    break;

  case SPELL_INFRAVISION:
    af[0].duration = 12;
    af[0].bitvector = AFF_INFRAVISION;
    accum_duration = TRUE;
    to_vict = "&15Your eyes &11g&00&03lo&11w &09r&00&01e&09d&15.&00";
    to_room = "&14$n&15's eyes &11g&00&03lo&11w &09r&00&01e&09d.&00";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;
    break;

  case SPELL_INVISIBLE:
    if (!victim)
      victim = ch;

    af[0].duration = 10;
    af[0].modifier = -40;
    af[0].location = APPLY_AC;
    af[0].bitvector = AFF_INVISIBLE;
    accum_duration = TRUE;
    to_vict = "&15Your body wavers as you &00&07v&15a&16ni&15s&00&07h from sight&15.&00";
    to_room = "&14$n &15slowly fades out of existence.&00";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;
    break;

  case SPELL_POISON:
    if (mag_savingthrow(victim, savetype)) {
      send_to_char(NOEFFECT, ch);
      return;
    }

    af[0].location = APPLY_CON;
    af[0].duration = 30;
    af[0].modifier = -5;
    af[0].bitvector = AFF_POISON;
    to_vict = "&15You feel very &10sick.&00";
    to_room = "&14$n &15gets violently ill!&00";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;
    break;

  case SPELL_PROT_FROM_EVIL:
    af[0].duration = 10;
    af[0].bitvector = AFF_PROTECT_EVIL;
    accum_duration = TRUE;
    to_vict = "&15You feel &16in&15vu&00&07lner&15ab&16le &15to curses&00!\r\n";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;
    break;

  case SPELL_BARRIER:
    af[0].duration = 8;
    af[0].bitvector = AFF_BARRIER;

    accum_duration = TRUE;
    to_vict = "&15A &12globe of &10e&00&02n&10e&00&02r&10g&00&02y &15engulfs your body!&00\r\n";
    to_room = "&15A &12globe of &10e&00&02n&10e&00&02r&10g&00&02y &15engulfs &14$n's&15 body!&00\r\n";
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 80;
    break;

  case SPELL_YOIKOMINMINKEN:
    if (!pk_allowed && !IS_NPC(ch) && !IS_NPC(victim))
      return;
    if (AFF_FLAGGED(victim, AFF_CALM)  && !IS_NPC(victim)) {
     act("&14$N &15shrugs off the effects of the Yoikominminken.&00", FALSE, ch, 0, victim, TO_CHAR);
     act("&15You &15shrug off the effects of &14$n's&15 Yoikominminken.&00", FALSE, ch, 0, victim, TO_VICT);
     act("&14$N &15shrugs off the effects of &14$n's&15 Yoikominminken.&00", FALSE, ch, 0, victim, TO_NOTVICT);
     return;
    }
    if (!IS_NPC(victim) && GET_MAX_HIT(victim) <= 4999999)
      return;
    if (!IS_NPC(ch) && GET_MAX_HIT(ch) <= 4999999)
      return;
    if (MOB_FLAGGED(victim, MOB_NOYOIKOMINMINKEN))
      return;
    if (mag_savingthrow(victim, savetype))
      return;
    GET_MANA(ch) -= GET_MAX_MANA(ch) / 80;
    af[0].duration = 10;
    af[0].bitvector = AFF_YOIKOMINMINKEN;

    if (GET_POS(victim) > POS_SLEEPING) {
      act("&15You charge ki into your eyes and hands and lull &14$N&15 to sleep&00\r\n", FALSE, ch, 0, victim, TO_CHAR);
      act("&15You feel very &12sleepy&15 as you see &14$n &15wave $s hands slightly, while &10ki &11flows &15through $s hands.&00.", TRUE, ch, 0, victim, TO_VICT);
      act("&09$N &15goes to &12sleep&15 as &14$n &15waves $s hands slowly, while &10ki &11flows &15through $s hands.&00", TRUE, ch, 0, victim, TO_NOTVICT);
      GET_POS(victim) = POS_SLEEPING;
    }
    break;

  case SPELL_SENSE_LIFE:
    to_vict = "Your feel your awareness improve.";
    af[0].duration = GET_LEVEL(ch);
    af[0].bitvector = AFF_SENSE_LIFE;
    accum_duration = TRUE;
    break;
  case SPELL_MIND_FREEZE:
      if (GET_MANA(ch) < GET_MAX_MANA(ch) / 10) {
        send_to_char("Not enough ki!", ch);
        return;
      }
      if (MOB_FLAGGED(victim,MOB_NOMIND_FREEZE) || mag_savingthrow(victim, savetype) || (!IS_NPC(victim)) || GET_MAX_HIT(victim) >= GET_MAX_HIT(ch) * 10 || GET_MAX_HIT(victim) >= 90000000) {
      act("You shrug off the attempt of mind freeze from &01$n&15.&00",
          FALSE, ch, 0, victim, TO_VICT);
      act("$N shrugs off the attempt of mind freeze from &01you&15.&00",
          FALSE, ch, 0, victim, TO_CHAR);
      act("&01$n&15's pathetic attempt to mind freeze &12$N&15 fails.&00", TRUE, ch, 0, victim, TO_NOTVICT);
      return;
    }
    af[0].duration = 0;
    af[0].bitvector = AFF_MFROZEN;
    to_vict = "&01$N&15 concentrates and suddenlly you can't move!&00";
    act("&00&01$n&15 concentrates and mind freezes &12$N&00.", FALSE, ch, 0, victim, TO_NOTVICT);
    act("&00&01You&15 concentrate and mind freezes &12$N&00.", FALSE, ch, 0, victim, TO_CHAR);
    GET_HIT(victim) = GET_HIT(victim) - GET_MAX_MANA(ch)/80;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/10;
    if (GET_POS(ch) > POS_STUNNED) {
      if (!(FIGHTING(ch)))
        set_fighting(ch, victim);

      if (IS_NPC(ch) && IS_NPC(victim) && victim->master &&
          !number(0, 10) && IS_AFFECTED(victim, AFF_MAJIN) &&
          (victim->master->in_room == ch->in_room)) {
        if (FIGHTING(ch))
          stop_fighting(ch);
        hit(ch, victim->master, TYPE_UNDEFINED);
        return;
      }
    }
    if (GET_POS(victim) > POS_STUNNED && !FIGHTING(victim)) {
      set_fighting(victim, ch);
      if (MOB_FLAGGED(victim, MOB_MEMORY) && !IS_NPC(ch) &&
          (GET_LEVEL(ch) < LVL_IMMORT))
        remember(victim, ch);
    }

    break;
    

  }
  /*
   * If this is a mob that has this affect set in its mob file, do not
   * perform the affect.  This prevents people from un-sancting mobs
   * by sancting them and waiting for it to fade, for example.
   */
  if (IS_NPC(victim) && !affected_by_spell(victim, spellnum))
    for (i = 0; i < MAX_SPELL_AFFECTS; i++)
      if (AFF_FLAGGED(victim, af[i].bitvector)) {
	send_to_char(NOEFFECT, ch);
	return;
      }

  /*
   * If the victim is already affected by this spell, and the spell does
   * not have an accumulative effect, then fail the spell.
   */
  if (affected_by_spell(victim,spellnum) && !(accum_duration||accum_affect)) {
    send_to_char(NOEFFECT, ch);
    return;
  }

  for (i = 0; i < MAX_SPELL_AFFECTS; i++)
    if (af[i].bitvector || (af[i].location != APPLY_NONE))
      affect_join(victim, af+i, accum_duration, FALSE, accum_affect, FALSE);

  if (to_vict != NULL)
    act(to_vict, FALSE, victim, 0, ch, TO_CHAR);
  if (to_room != NULL)
    act(to_room, TRUE, victim, 0, ch, TO_ROOM);
}


/*
 * This function is used to provide services to mag_groups.  This function
 * is the one you should change to add new group spells.
 */

void perform_mag_groups(int level, struct char_data * ch,
			struct char_data * tch, int spellnum, int savetype)
{
  switch (spellnum) {
   case SPELL_GROUP_HEAL:
    mag_points(level, ch, tch, SPELL_HEAL, savetype);
    break;
   case SPELL_GROUP_armor:
    mag_affects(level, ch, tch, SPELL_BARRIER, savetype);
    break;
   case SPELL_GROUP_BOOST:
    mag_points(level, ch, tch, SPELL_BOOST, savetype);
    break;
   case SPELL_GROUP_INVISIBLE:
    mag_affects(level, ch, tch, SPELL_INVISIBLE, savetype);
    break;
   case SPELL_GROUP_RECALL:
    spell_recall(level, ch, tch, NULL);
    break;
  }
}


/*
 * Every spell that affects the group should run through here
 * perform_mag_groups contains the switch statement to send us to the right
 * magic.
 *
 * group spells affect everyone grouped with the focuser who is in the room,
 * focuser last.
 *
 * To add new group spells, you shouldn't have to change anything in
 * mag_groups -- just add a new case to perform_mag_groups.
 */

void mag_groups(int level, struct char_data * ch, int spellnum, int savetype)
{
  struct char_data *tch, *k;
  struct follow_type *f, *f_next;

  if (ch == NULL)
    return;

  if (!AFF_FLAGGED(ch, AFF_GROUP))
    return;
  if (ch->master != NULL)
    k = ch->master;
  else
    k = ch;
  for (f = k->followers; f; f = f_next) {
    f_next = f->next;
    tch = f->follower;
    if (tch->in_room != ch->in_room)
      continue;
    if (!AFF_FLAGGED(tch, AFF_GROUP))
      continue;
    if (ch == tch)
      continue;
    perform_mag_groups(level, ch, tch, spellnum, savetype);
  }

  if ((k != ch) && AFF_FLAGGED(k, AFF_GROUP))
    perform_mag_groups(level, ch, k, spellnum, savetype);
  perform_mag_groups(level, ch, ch, spellnum, savetype);
}


/*
 * mass spells affect every creature in the room except the focuser.
 *
 * No spells of this class currently implemented as of Circle 3.0.
 */

void mag_masses(int level, struct char_data * ch, int spellnum, int savetype)
{
  struct char_data *tch, *tch_next;

  for (tch = world[ch->in_room].people; tch; tch = tch_next) {
    tch_next = tch->next_in_room;
    if (tch == ch)
      continue;

    switch (spellnum) {
    }
  }
}


/*
 * Every spell that affects an area (room) runs through here.  These are
 * generally offensive spells.  This calls mag_damage to do the actual
 * damage -- all spells listed here must also have a case in mag_damage()
 * in order for them to work.
 *
 *  area spells have limited targets within the room.
*/

void mag_areas(int level, struct char_data * ch, int spellnum, int savetype)
{
  struct char_data *tch, *next_tch;
  const char *to_char = NULL, *to_room = NULL;

  if (ch == NULL)
    return;

  /*
   * to add spells to this fn, just add the message here plus an entry
   * in mag_damage for the damaging part of the spell.
   */
  switch (spellnum) {
  }

  if (to_char != NULL)
    act(to_char, FALSE, ch, 0, 0, TO_CHAR);
  if (to_room != NULL)
    act(to_room, FALSE, ch, 0, 0, TO_ROOM);
  

  for (tch = world[ch->in_room].people; tch; tch = next_tch) {
    next_tch = tch->next_in_room;

    /*
     * The skips: 1: the focuser
     *            2: immortals
     *            3: if no pk on this mud, skips over all players
     *            4: pets (MAJINed NPCs)
     * players can only hit players in CRIMEOK rooms 4) players can only hit
     * MAJINed mobs in CRIMEOK rooms
     */

    if (tch == ch)
      continue;
    if (!IS_NPC(tch) && GET_LEVEL(tch) >= LVL_IMMORT)
      continue;
    if (!pk_allowed && !IS_NPC(ch) && !IS_NPC(tch))
      continue;
    if (!IS_NPC(ch) && IS_NPC(tch) && AFF_FLAGGED(tch, AFF_MAJIN))
      continue;

    /* Doesn't matter if they die here so we don't check. -gg 6/24/98 */
    mag_damage(GET_LEVEL(ch), ch, tch, spellnum, 1);
  }
}


/*
 *  Every spell which summons/gates/conjours a mob comes through here.
 *
 *  None of these spells are currently implemented in Circle 3.0; these
 *  were taken as examples from the JediMUD code.  Summons can be used
 *  for spells like MULTIFORM, ariel servant, etc.
 *
 * 10/15/97 (gg) - Implemented Animate Dead and MULTIFORM.
 */

/*
 * These use act(), don't put the \r\n.
 */
const char *mag_summon_msgs[] = {
  "\r\n",
  "$n makes a strange magical gesture; you feel a strong breeze!",
  "&14$n&15 animates a &16c&00&02o&10r&15p&00&02s&16e&15!&00",
  "$N appears from a cloud of thick blue smoke!",
  "$N appears from a cloud of thick green smoke!",
  "$N appears from a cloud of thick red smoke!",
  "$N disappears in a thick black cloud!"
  "As $n makes a strange magical gesture, you feel a strong breeze.",
  "As $n makes a strange magical gesture, you feel a searing heat.",
  "As $n makes a strange magical gesture, you feel a sudden chill.",
  "As $n makes a strange magical gesture, you feel the dust swirl.",
  "&14$n &15concentrates and &00&07d&15i&16vid&15e&00&07s!&00",
  "&14$n &15pours some liquid on to the seed, and a little &10g&00&02ree&10n &15alien come out of the ground!&00",
  "&14$n &15tumbles over in pain as $s spits out a large &00&07e&15g&00&07g&15 that hatches into a little &10n&00&02ame&10k&00!"
};

/*
 * Keep the \r\n because these use send_to_char.
 */
const char *mag_summon_fail_msgs[] = {
  "\r\n",
  "There are no such creatures.\r\n",
  "You failed\r\n",
  "You failed pretty bad.\r\n",
  "Oh shit! Yeah, you messed up, try again...\r\n",
  "Your body resists! Concentrate harder!\r\n",
  "You failed.\r\n",
  "You plant them wrong and they don't grow.\r\n"
};

/* These mobiles do not exist. */
#define MOB_MONSUM_I		130
#define MOB_MONSUM_II		140
#define MOB_MONSUM_III		150
#define MOB_GATE_I		160
#define MOB_GATE_II		170
#define MOB_GATE_III		180

/* Defined mobiles. */
#define MOB_ELEMENTAL_BASE	20	/* Only one for now. */
#define MOB_MULTIFORM		10
#define MOB_MULTIFORM2          20000
#define MOB_MULTIFORM3          20001
#define MOB_SAIBA		4599
#define MOB_ZOMBIE		11
#define MOB_GHOUL               20002
#define MOB_IMP                 20003
#define MOB_AERIALSERVANT	19
#define MOB_REPRO       	4598
#define MOB_REPRO2              20004
#define MOB_REPRO3              20005


void mag_summons(int level, struct char_data * ch, struct obj_data * obj,
		      int spellnum, int savetype)
{
  struct char_data *mob = NULL;
  struct obj_data *tobj, *next_obj;
  int	pfail = 0, msg = 0, fmsg = 0, mob_num = 0,
	num = 1, handle_corpse = FALSE, i;

  if (ch == NULL)
    return;
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 30) {
   act("You don't have enough ki!", FALSE, ch, 0, 0, TO_CHAR);
   return;
  }

  switch (spellnum) {
  case SPELL_MULTIFORM:
    msg = 10;
    fmsg = number(2, 3);	/* Random fail message. */
    mob_num = MOB_MULTIFORM;
    pfail = 50;	/* 50% failure, should be based on something later. */
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
    break;
  case SPELL_SAIBA:
    msg = 11;
    fmsg = number(7, 7);	/* Random fail message. */
    mob_num = MOB_SAIBA;
    pfail = 1;	/* 50% failure, should be based on something later. */
    break;
  case SPELL_REPRODUCE:
    msg = 12;
    fmsg = number(7, 7);	/* Random fail message. */
    mob_num = MOB_REPRO;
    pfail = 1;	/* 50% failure, should be based on something later. */
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
    break;
  case SPELL_ANIMATE_DEAD:
    if (obj == NULL || !IS_CORPSE(obj)) {
      act(mag_summon_fail_msgs[7], FALSE, ch, 0, 0, TO_CHAR);
      return;
    }
    handle_corpse = TRUE;
    msg = 2;
    fmsg = number(2, 6);	/* Random fail message. */
    mob_num = MOB_ZOMBIE;
    pfail = 10;	/* 10% failure, should vary in the future. */
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
    break;

  default:
    return;
  }
  if (GET_MAX_HIT(ch) >= 1499999) {
   switch (spellnum) {

   case SPELL_MULTIFORM:
     mob_num = MOB_MULTIFORM2;
     msg = 10;
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 40;
     break;
   case SPELL_ANIMATE_DEAD:
     msg = 2;
     mob_num = MOB_GHOUL;
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 40;
     break;
   case SPELL_REPRODUCE:
     msg = 12;
     mob_num = MOB_REPRO2;
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 40;
     break;
   }  
 }
  if (GET_MAX_HIT(ch) >= 9999999) {
   switch (spellnum) {

   case SPELL_MULTIFORM:
     msg = 10;
     mob_num = MOB_MULTIFORM3;
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 30;
     break;
   case SPELL_ANIMATE_DEAD:
     msg = 2;
     mob_num = MOB_IMP;
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 30;
     break;
   case SPELL_REPRODUCE:
     msg = 12;
     mob_num = MOB_REPRO3;
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 30;
     break;
   }
  }
  
  if (AFF_FLAGGED(ch, AFF_MAJIN)) {
    send_to_char("You cannot have any followers.\r\n", ch);
    return;
  }
  if (number(0, 101) < pfail) {
    send_to_char(mag_summon_fail_msgs[fmsg], ch);
    return;
  }
  for (i = 0; i < num; i++) {
    if (!(mob = read_mobile(mob_num, VIRTUAL))) {
      send_to_char("You don't quite remember how to make that creature.\r\n", ch);
      return;
    }
    char_to_room(mob, ch->in_room);
    IS_CARRYING_W(mob) = 0;
    IS_CARRYING_N(mob) = 0;
    SET_BIT(AFF_FLAGS(mob), AFF_MAJIN);
    if (spellnum == SPELL_MULTIFORM) {	/* Don't mess up the proto with strcpy. */
      mob->player.name = str_dup(GET_NAME(ch));
      mob->player.short_descr = str_dup(GET_NAME(ch));
      sprintf(buf2, "&14%s stands here.&00\r\n", GET_NAME(ch));
      mob->player.long_descr = str_dup(buf2);
      *buf2 = '\0';
      sprintf(buf2, "&15%s looks kind of absent minded, as if waiting for some sort of command.&00\r\n", GET_NAME(ch));
      mob->player.description = str_dup(buf2);
    }
    act(mag_summon_msgs[msg], FALSE, ch, 0, mob, TO_ROOM);
    GET_HIT(mob) = GET_MAX_HIT(ch) / 5;
    GET_MAX_HIT(mob) = GET_MAX_HIT(ch) / 5;
    load_mtrigger(mob);
    add_follower(mob, ch);
  }
  if (handle_corpse) {
    for (tobj = obj->contains; tobj; tobj = next_obj) {
      next_obj = tobj->next_content;
      obj_from_obj(tobj);
      obj_to_char(tobj, mob);
    }
    extract_obj(obj);
  }
}


void mag_points(int level, struct char_data * ch, struct char_data * victim,
		     int spellnum, int savetype)
{
  int hit = 0;
  int mana = 0;
  int move = 0;
  int mhit = 0;
  int mmana = 0;

  if (victim == NULL)
    return;
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 40 && !IS_NPC(ch)){
   switch (spellnum) {
    case SPELL_HEAL:
     act("Not enough ki!", TRUE, ch, 0, 0, TO_CHAR);
     return;
     break;
   }
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 20 && !IS_NPC(ch)){
   switch (spellnum) {
    case SPELL_LIMBREGO:
     act("Not enough ki!", TRUE, ch, 0, 0, TO_CHAR);
     return;
     break;
    case SPELL_RESTORE:
     act("Not enough ki!", TRUE, ch, 0, 0, TO_CHAR);
     return;
     break;
   }
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 5 || GET_HIT(ch) < GET_MAX_HIT(ch) / 5) {
   switch (spellnum) {
    case SPELL_BOOST:
     act("Not enough ki, or pl!\r\n", TRUE, ch, 0, 0, TO_CHAR);
     return;
     break;
   }
  }
  switch (spellnum) {
  case SPELL_CURE_CRITIC:
    hit = dice(3, 8) + 3 + (level / 4);
    send_to_char("You feel a lot better!\r\n", victim);
    break;
  case SPELL_BOOST:
     if (ch != victim) {
      GET_MANA(victim) += GET_MAX_MANA(victim)/10;
      send_to_char("&15You feel &10e&00&02n&10e&00&02r&10g&00&02y&15 flow through your body&16.&00\r\n", victim);
      }
     else {
      GET_MANA(ch) -= GET_MAX_MANA(ch)/5;
      GET_HIT(ch) -= GET_MAX_HIT(ch)/5;
      act("&15Your body slows down as you pass your &10e&00&02n&10e&00&02r&10g&00&02y&15 to the party.&00\r\n", FALSE, ch, 0, 0, TO_CHAR);
      WAIT_STATE(ch, PULSE_VIOLENCE * 3);
      }
    break;
  case SPELL_RESTORE:
     hit = MIN(GET_MAX_MANA(ch) / 8, MAX(1, 2000000));
     sprintf(buf1, "&16[&13Restore&11: &15%d&16]\r\n", hit);
     send_to_char(buf1, victim);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/20;
    send_to_char("&15The magical spell fills you with &13g&00&05o&16o&13d &10h&00&02e&16a&15l&00&07th&16.&00\r\n", victim);
    break;
  case SPELL_LIMBREGO:
   if (IS_NPC(victim)) {
    send_to_char("Not mobs...\r\n", ch);
    return;
   }
   SET_BIT(PLR_FLAGS(victim), PLR_RARM);
   SET_BIT(PLR_FLAGS(victim), PLR_LARM);
   SET_BIT(PLR_FLAGS(victim), PLR_RLEG);
   SET_BIT(PLR_FLAGS(victim), PLR_LLEG);
   act("&15You put your hands over &14$N&15 and $e glow with a &12b&00&04l&16u&12e&15 light as $s limbs regrow.&00", FALSE, ch, 0, victim, TO_CHAR);
   act("&14$n&15 puts $s hands over you and you glow with a &12b&00&04l&16u&12e&15 light as your limbs regrow.&00", FALSE, ch, 0, victim, TO_VICT);
   act("&14$n&15 puts $s hands over &14$N&15 and $e glow with a &12b&00&04l&16u&12e&15 light as $s limbs regrow.&00", FALSE, ch, 0, victim, TO_NOTVICT);
   GET_MANA(ch) -= GET_MAX_MANA(ch) / 20;
   break;
  case SPELL_HEAL:
    if (IS_NPC(ch)) {
     hit = GET_MAX_HIT(victim) / 220;
    }
    else if (!IS_NPC(ch)) {
     hit = MIN(GET_MAX_MANA(ch) / 15, MAX(1, 1000000));
     sprintf(buf1, "&16[&13Heal&11: &15%d&16]\r\n", hit);
     send_to_char(buf1, victim);
    }
    if (!IS_NPC(ch)) {
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/40;
    }
    send_to_char("&15A &09warm &15feeling floods your body&16.&00\r\n", victim);
    break;
  case SPELL_HOLYWATER:
   if (!IS_ANDROID(ch)) {
    mhit = number(50000, 125000);
    mmana = number(50000, 125000);
    send_to_char("You feel your bodily strength, and spirit increase from consumption of the water!\r\n\r\n", victim);
    }
   else if (IS_ANDROID(ch)) {
    if (GET_MAX_HIT(ch) >= 15000000) {
    send_to_char("&15The information bean increases your upgrade data!&00", ch);
    GET_HOME(ch) += 200;
    }
    else {
     send_to_char("&15Your internal systems are not advanced enough to process the bean, you register that you need at least 15,000,000 pl to be able to do so. Your money has been refunded.", ch);
     GET_GOLD(ch) += 2000000;
     }
   }
    break;
  case SPELL_NANO:
    if (!PRF_FLAGGED(ch, PRF_AUTOEXIT)) {
    mana = GET_MAX_MANA(ch);
    }
    move = GET_MAX_MOVE(ch);
    send_to_char("Charging batteries...\r\n\r\n", victim);
    break;
  case SPELL_SENZU:
    if (!PRF_FLAGGED(ch, PRF_AUTOEXIT)) {
     mana = GET_MAX_MANA(ch);
    }
    hit = GET_MAX_HIT(ch);
    send_to_char("You have been fully healed!\r\n", victim);
    break;
  case SPELL_LIMB:
    SET_BIT(PLR_FLAGS(ch), PLR_LARM);
    SET_BIT(PLR_FLAGS(ch), PLR_RARM);
    SET_BIT(PLR_FLAGS(ch), PLR_LLEG);
    SET_BIT(PLR_FLAGS(ch), PLR_RLEG);
    act("&15You regrow your arms and legs after swallowing the pill!&00", FALSE, ch, 0, 0, TO_CHAR);
    act("$n regrows their arms and legs after swallowing the pill.", FALSE, ch, 0, 0, TO_NOTVICT);
    break;
  }
  GET_HIT(victim) = MIN(GET_MAX_HIT(victim), GET_HIT(victim) + hit);
  GET_MANA(victim) = MIN(GET_MAX_MANA(victim), GET_MANA(victim) + mana);
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + mhit;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + mmana;
  update_pos(victim);
}


void mag_unaffects(int level, struct char_data * ch, struct char_data * victim,
		        int spellnum, int type)
{
  int spell = 0;
  const char *to_vict = NULL, *to_room = NULL;

  if (victim == NULL)
    return;

  switch (spellnum) {
  case SPELL_CURE_SOLAR_FLARE:
  case SPELL_HEAL:
  case SPELL_SENZU:
    spell = SPELL_SOLAR_FLARE;
    to_vict = "Your vision returns!";
    to_room = "&15There's a momentary &14gleam in &12$n's&15 eyes.&00";
    break;
  case SPELL_REMOVE_POISON:
    spell = SPELL_POISON;
    to_vict = "A warm feeling runs through your body!";
    to_room = "$n looks better.";
    break;
  case SPELL_REMOVE_CURSE:
    spell = SPELL_CURSE;
    to_room = "&14$n &15has had their curse removed!&00";
    to_vict = "&15You don't feel so unlucky anymore.&00";
    break;
  default:
    log("SYSERR: unknown spellnum %d passed to mag_unaffects.", spellnum);
    return;
  }

  if (!affected_by_spell(victim, spell) && spellnum != SPELL_HEAL) {
    send_to_char(NOEFFECT, ch);
    return;
  }

  affect_from_char(victim, spell);
  if (to_vict != NULL)
    act(to_vict, FALSE, victim, 0, ch, TO_CHAR);
  if (to_room != NULL)
    act(to_room, TRUE, victim, 0, ch, TO_ROOM);

}


void mag_alter_objs(int level, struct char_data * ch, struct obj_data * obj,
		         int spellnum, int savetype)
{
  const char *to_char = NULL, *to_room = NULL;
  int spell = 0;
  if (obj == NULL)
    return;

  switch (spellnum) {
    case SPELL_ZANZOKEN:
      if (!IS_OBJ_STAT(obj, ITEM_ZANZOKEN) &&
	  (GET_OBJ_WEIGHT(obj) <= 5 * GET_LEVEL(ch))) {
	SET_BIT(GET_OBJ_EXTRA(obj), ITEM_ZANZOKEN);
	to_char = "$p glows briefly.";
      }
      break;
    case SPELL_CURSE:
      if (GET_OBJ_TYPE(obj) == ITEM_dball1 || GET_OBJ_TYPE(obj) == ITEM_dball2 || GET_OBJ_TYPE(obj) == ITEM_dball3 || GET_OBJ_TYPE(obj) == ITEM_dball4 || GET_OBJ_TYPE(obj) == ITEM_dball5 || GET_OBJ_TYPE(obj) == ITEM_dball6 || GET_OBJ_TYPE(obj) == ITEM_dball7) {
       send_to_char("No cursing dragonballs!\r\n", ch);
       return;
      }
      if (!IS_OBJ_STAT(obj, ITEM_NODROP)) {
	SET_BIT(GET_OBJ_EXTRA(obj), ITEM_NODROP);
	if (GET_OBJ_TYPE(obj) == ITEM_WEAPON)
	  GET_OBJ_VAL(obj, 2)--;
	to_char = "$p briefly glows red.";
      }
      break;
    case SPELL_INVISIBLE:
      if (!IS_OBJ_STAT(obj, ITEM_NOINVIS | ITEM_INVISIBLE)) {
        SET_BIT(obj->obj_flags.extra_flags, ITEM_INVISIBLE);
        to_char = "$p vanishes.";
      }
      break;
    case SPELL_POISON:
      if (((GET_OBJ_TYPE(obj) == ITEM_DRINKCON) ||
         (GET_OBJ_TYPE(obj) == ITEM_FOUNTAIN) ||
         (GET_OBJ_TYPE(obj) == ITEM_FOOD)) && !GET_OBJ_VAL(obj, 3)) {
      GET_OBJ_VAL(obj, 3) = 1;
      to_char = "$p steams briefly.";
      }
      break;
    case SPELL_REMOVE_CURSE:
      if (IS_OBJ_STAT(obj, ITEM_NODROP)) {
        REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);
        if (GET_OBJ_TYPE(obj) == ITEM_WEAPON)
          GET_OBJ_VAL(obj, 2)++;
        to_char = "$p briefly glows blue.";
      }
      break;
    case SPELL_REMOVE_POISON:
      if (((GET_OBJ_TYPE(obj) == ITEM_DRINKCON) ||
         (GET_OBJ_TYPE(obj) == ITEM_FOUNTAIN) ||
         (GET_OBJ_TYPE(obj) == ITEM_FOOD)) && GET_OBJ_VAL(obj, 3)) {
        GET_OBJ_VAL(obj, 3) = 0;
        to_char = "$p steams briefly.";
      }
      break;
  }

  if (to_char == NULL)
    send_to_char(NOEFFECT, ch);
  else
    act(to_char, TRUE, ch, obj, 0, TO_CHAR);

  if (to_room != NULL)
    act(to_room, TRUE, ch, obj, 0, TO_ROOM);
  else if (to_char != NULL)
    act(to_char, TRUE, ch, obj, 0, TO_ROOM);

}



void mag_creations(int level, struct char_data * ch, int spellnum)
{
  struct obj_data *tobj;
  int z;

  if (ch == NULL)
    return;
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 2) {
   switch (spellnum) {
    case SPELL_BUILDNANO:
     send_to_char("Not enough ki to manufacture a nano bean!", ch);
     return;
     break;
    }
   }
  if (IS_MAJIN(ch) && !PLR_FLAGGED(ch, PLR_FORELOCK)) {
   switch (spellnum) {
   case SPELL_CANDYBEAM:
    act("You need to have a forelock to use candybeam!", FALSE, ch, 0, 0, TO_CHAR);
    return;
    break;
    }
   }
  if (IS_MAJIN(ch) && GET_COND(ch, FULL) >= 20) {
   switch (spellnum) {
   case SPELL_CANDYBEAM:
    act("You are to full to desire to make candy, wait a while.!", FALSE, ch, 0, 0, TO_CHAR);
    return;
    break;
    }
   }
  if (GET_MANA(ch) < GET_MAX_MANA(ch) && GET_HIT(ch) >= GET_MAX_HIT(ch) / 2) {
   switch (spellnum) {
   case SPELL_CANDYBEAM:
    act("You need to have full ki and at least half your pl to make some majin candy!", FALSE, ch, 0, 0, TO_CHAR);
    return;
    break;
    }
   }
  if (GET_MANA(ch) >= GET_MAX_MANA(ch) / 5) {
   switch (spellnum) {
    case SPELL_BUILDNANO:
     z = 2150;
     GET_MANA(ch) -= GET_MAX_MANA(ch) / 2;
     act("&15You slowly put together the parts to make a nano bean, and charging it full of energy complete the product.&00", FALSE, ch, 0, 0, TO_CHAR);
     act("&14$n &15slowly puts together the parts to make a nano bean, and charging it full of energy completes the product.&00", FALSE, ch, 0, 0, TO_NOTVICT);
     break;
    }
   }
  if (GET_MANA(ch) >= GET_MAX_MANA(ch)) {
   switch (spellnum) {
   case SPELL_CANDYBEAM:
    z = 20009;
    GET_MANA(ch) = 0;
    GET_COND(ch, FULL) = 20;
    act("&14You pick up a &15rock&14 and aim your &00&05forelock&14 at it&16.&14 Firing a &13pink&14 beam you change it into some &13majin &12candy&16!&00", FALSE, ch, 0, 0, TO_CHAR);
    act("&11$n &14picks up a &15rock&14 and aims their &00&05forelock&14 at it&16.&14 Firing a &13pink&14 beam they change it into some &13majin &12candy&16!&00", FALSE, ch, 0, 0, TO_ROOM);
    break;
    }
   }

  else if (GET_MANA(ch) < GET_MAX_MANA(ch) / 3) {
   act("You don't have enough ki!", FALSE, ch, 0, 0, TO_CHAR);
   return;
  }
  level = MAX(MIN(level, LVL_IMPL), 1);

  switch (spellnum) {
  case SPELL_BUILDNANO:
    z = 2150;
    break;
  case SPELL_FORMWRIST:
    z = 22;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 100;
    break;
  case SPELL_FORMPANTS:
    z = 23;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 100;
    break;
  case SPELL_CREATE_FOOD:
    z = 21;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 100;
    break;
  case SPELL_CANDYBEAM:
    z = 20009;
    GET_MANA(ch) = 0;
    break;
  case SPELL_BUILDSHIELD:
    z = 20005;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 100;
    break;
  case SPELL_SPEAR:
    z = 20000;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 100;
    break;
  case SPELL_SWORD:
    z = 4565;
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 100;
    break;
  case SPELL_STASIS:
    if (CLN_FLAGGED(ch, CLN_STASIS)) {
     send_to_char("You no longer have room in your body to do it.\r\n", ch);
     return;
    }
    z = 2091;
    act("$n falls to $s knees and groans as $e isolates part of $s body making it stasis cells!", FALSE, ch, tobj, 0, TO_ROOM);
    act("You fall to your knees and groan as you isolate part of your body making it stasis cells!", FALSE, ch, tobj, 0, TO_CHAR);
    SET_BIT(CLN_FLAGS(ch), CLN_STASIS);
    break;
  default:
    send_to_char("Spell unimplemented, it would seem.\r\n", ch);
    return;
  }
  if (GET_MAX_HIT(ch) >= 1499999) {
   switch (spellnum) {
  
   case SPELL_SPEAR:
     z = 20001;
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 5;
     break;
   case SPELL_SWORD:
     z = 20003;
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 5;
     break;
   }
  }
  if (GET_MAX_HIT(ch) >= 9999999) {
   switch (spellnum) {

   case SPELL_SPEAR:
     z = 20002;
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 3;
     break;
   case SPELL_SWORD:
     z = 20004;
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 3;
     break;
   }
  }
  if (!(tobj = read_object(z, VIRTUAL))) {
    send_to_char("I seem to have goofed.\r\n", ch);
    log("SYSERR: spell_creations, spell %d, obj %d: obj not found",
	    spellnum, z);
    return;
  }
  obj_to_char(tobj, ch);
  act("$n creates $p.", FALSE, ch, tobj, 0, TO_ROOM);
  act("You create $p.", FALSE, ch, tobj, 0, TO_CHAR);
  load_otrigger(tobj);
}
