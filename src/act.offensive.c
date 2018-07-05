/* ************************************************************************
*   File: act.offensive.c                               Part of CircleMUD *
*  Usage: player-level commands of an offensive nature                    *
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

/* extern variables */
extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern int pk_allowed;
extern sh_int r_death_start_room;
extern sh_int r_mortal_start_room;
extern sh_int r_konack_start_room;

/* extern functions */
void raw_kill(struct char_data * ch, struct char_data * killer);
void check_killer(struct char_data * ch, struct char_data * vict);

/* local functions */
ACMD(do_assist);
ACMD(do_hit);
ACMD(do_kill);
ACMD(do_backstab);
ACMD(do_dodge);
ACMD(do_block);
ACMD(do_adef);
ACMD(do_order);
ACMD(do_flee);
ACMD(do_sweep);
ACMD(do_rescue);
ACMD(do_kick);
ACMD(do_tailwhip);
ACMD(do_forelock);
ACMD(do_knee);
ACMD(do_elbow);
ACMD(do_roundhouse);
ACMD(do_burning);
ACMD(do_genki);
ACMD(do_chikyuu);
ACMD(do_genocide);
ACMD(do_bbk);
ACMD(do_yoiko);
ACMD(do_spirit);
ACMD(do_scatter);
ACMD(do_hellspear);
ACMD(do_gigabeam);
ACMD(do_twin);
ACMD(do_dragon);
ACMD(do_kamehame);
ACMD(do_bigbang);
ACMD(do_finalflash);
ACMD(do_choukame);
ACMD(do_disk);
ACMD(do_masenko);
ACMD(do_renzo);
ACMD(do_makanko);
ACMD(do_kishot);
ACMD(do_havoc);
ACMD(do_crusher);
ACMD(do_eraser);
ACMD(do_bakuhatsuha);
ACMD(do_honoo);
ACMD(do_beam);
ACMD(do_tsuihidan);
ACMD(do_shogekiha);
ACMD(do_kikoho);
ACMD(do_kakusanha);
ACMD(do_pheonix);
ACMD(do_eclipse);
ACMD(do_hydro);
ACMD(do_dball);
ACMD(do_dball2);
ACMD(do_kousen);
ACMD(do_zanelb);
ACMD(do_hellf);
ACMD(do_galikgun);
ACMD(do_cosmic);
ACMD(do_heaven);
ACMD(do_heel);
ACMD(do_rogafufuken);
ACMD(do_vital);
ACMD(do_plasmabomb);

ACMD(do_rogafufuken)
{
  struct char_data *vict;
  int percent, prob, dam;
  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch,AFF_CHARGEL)){
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LLEG) && !PLR_FLAGGED(ch, PLR_RLEG)) {
     send_to_char("You have no limbs!\r\n", ch);
     return;
  }

  if (!GET_SKILL(ch, SPELL_ROGA)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60) {
     send_to_char("You don't have the Ki to do Rogafufuken!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
  } else {
      send_to_char("Hit who with Rogafufuken?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 100);
  prob = GET_LBS(ch);
  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/60) {
    damage(ch, vict, 0, SKILL_HEEL);
    act("&15You charge energy and prepare your wolf fang fist, but screw up the process and stop!", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n&15 charges energy and prepares $s wolf fang fist, but screws up and stops mid sequence!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n&15 charges energy and prepares $s wolf fang fist, but screws up and stops mid sequence!", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
    return;
  }
 else
    act("&16You slide one foot foreward as you cock your right hand and hold it out in front of your body. &14E&00&06n&00ergy &16wells up inside right before you launch yourself at &15$N &16while screaming '&16W&15o&00l&16f &00F&15a&16n&00g &00&06F&14i&16s&15t!&16', and sending palm &00&06s&14t&00&06r&14i&00&06k&14e&00&06s &16and &00&04k&12i&00&04c&12k&00&04s &16into &15$S &16body from many directions. &00&04B&12l&00&06u&14e &16streaks of &14k&00&06i &16can be seen &15f&16a&00di&16n&15g &16away after each hit, along with small &14e&00&06x&16p&15l&00os&15i&16o&00&06n&14s &16from each hit!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15$n &16moves into a &15f&00i&16ghti&00n&15g &16stance with one foot slide just before them and their right hand arched in front. Drawing &00&06e&14n&16er&14g&00&06y &16from &15d&00ee&15p &16within them they launch them self at you screaming '&16W&15o&00l&16f &00F&15a&16n&00g &00&06F&14i&16s&15t&16!!', and sending a &12f&00&04u&16r&15y &16of palm &00&06s&14t&00&06r&14i&00&06k&14e&00&06s &16and &00&04k&12i&00&04c&00&04k&12s &16into you from most directions. With each &00&06s&14t&16r&15i&00k&00&06e &16comes a &00&06b&12l&14u&00&06e &16streak of &00&06k&14i &16that is followed with a small &14e&00&06x&16p&15l&00os&15i&16o&00&06n&14s &16after each hit!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15$n &16prepares for &15$s &16next attack by sliding info a new &00f&15i&16ghti&15n&00g &16stance, sliding one foot ahead of them and raising &15$s &16right hand at an arch before them. &15$n &16beings to draw upon a &00&04e&12n&00&06e&14r&00&04g&12y &16deep within before launching them self at &15$N &16screaming '&16W&15o&00l&16f &00F&15a&16n&00g &00&06F&14i&16s&15t&16!', and sending a &00&04f&12u&16r&15y &16of palm &00&06s&14t&00&06r&14i&00&06k&14e&00&06s &16and &00&04k&12i&00&04c&12k&00&04s &16from most directions. Each &00&06s&14t&00&06r&14i&00&06k&14e &16is followed by a &14b&00&06l&00&04u&12e &16streak of &12k&00&04i &16that &14e&00&06x&16p&15l&16d&00&06e&14s &16on impact with $N!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      damage(ch, vict, GET_MAX_MANA(ch) / 50 + 500000, SKILL_HEEL);
      REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      dam = GET_MAX_MANA(ch) / 50 + 500000;

    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }

    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);

 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
}
 
ACMD(do_plasmabomb) {
  struct char_data *vict, *next_vict;
  int dam, num = 0;
  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!GET_SKILL(ch, SPELL_PBOMB)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do a Plasma Bomb!\r\n", ch);
     return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  else {                      /* no argument. clean out the room */
    act("&15Your right hand opens up, revealing a large barrel. Grabbing just below the barrel you begin to pull internal &00e&14n&00&06er&14g&00y&15 into the barrel. You leap into the air as the barrel continues to draw more &00e&14n&00&06er&14g&00y&15 into it, its bright &00&03y&11e&00ll&11o&00&03w&15 glow tinting the entire area the same color. As you reach a mile above the ground, you aim the cannon down, and fire a massive round blast down. The giant &00&03y&11e&00ll&11o&00&03w&15 blast swirling with &10g&00&02r&00e&10e&00&02n &00e&11n&00&03er&11g&00y&15 slams into the ground, covering the nearby area in a destructive blast!", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n's&15 right hand opens up, revealing a large barrel. Grabbing just below the barrel $e begins to pull internal &00e&14n&00&06er&14g&00y&15 into the barrel. $e leaps into the air as the barrel continues to draw more &00e&14n&00&06er&14g&00y&15 into it, its bright &00&03y&11e&00ll&11o&00&03w&15 glow tinting the entire area the same color. As $n reaches a mile above the ground, $e aims the cannon down, and fires a massive round blast down. The giant &00&03y&11e&00ll&11o&00&03w&15 blast swirling with &10g&00&02r&00e&10e&00&02n &00e&11n&00&03er&11g&00y&15 slams into the ground, covering the nearby area in a destructive blast!", FALSE, ch, 0, vict, TO_ROOM);
    if (!IS_NPC(ch)) {
     SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    }
    for (vict = world[ch->in_room].people; vict; vict = next_vict) {
      next_vict = vict->next_in_room;
      if (num >= 20) {
        continue;
      }
      if (vict == ch) {
        continue;
      }
      if (GET_MAX_HIT(vict) <= 4999999 && !IS_NPC(vict)) {
        continue;
      }
      if (CLN_FLAGGED(vict, CLN_ADEF)) {
        continue;
      }
      damage(ch, vict, GET_MAX_MANA(ch) / 30 + 325000, SKILL_HAVOC);
      num++;

      GET_POS(vict) = POS_SITTING;
      WAIT_STATE(vict, PULSE_VIOLENCE);
  }
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);
  if (!IS_NPC(ch)) {
   REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
  }
  dam = GET_MAX_MANA(ch) / 30 + 325000;
  if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
   sprintf(buf, "&15You hit everything in the room! &16[&12%d&16]&00", dam);
   sprintf(buf2, "&15You are hit by the attack!&16[&09%d&16]&00", dam);
   act(buf, FALSE, ch, NULL, vict, TO_CHAR);
   act(buf2, FALSE, ch, NULL, vict, TO_VICT);
GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
 }
}

ACMD(do_vital)
{
  struct char_data *vict;
  if (!GET_SKILL(ch, SPELL_VITAL)) {
       send_to_char("You don't know how to do that!\r\n", ch);
       return;
      }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 4 && !IS_NPC(ch)) {
     send_to_char("You don't have the Ki to do Vital Strike!\r\n", ch);
     return;
   }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Vital Strike who?\r\n", ch);
      return;
    }
  }
  if (IS_NPC(vict)) {
   send_to_char("Not mobs...\r\n", ch);
   return;
  }
  if (vict == ch) {
   send_to_char("Not yourself...\r\n", ch);
   return;
  }
  if (CLN_FLAGGED(vict, CLN_VITAL)) {
   send_to_char("You must wait a while before they can be stunned again!\r\n", ch);
   return;
  }
  if (AFF_FLAGGED(vict, AFF_CALM)) {
   act("&15You slam your elbow into &14$N&15's neck, but $e is unfazed.&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&09$n's&15 slams $s elbow into your neck, but you are unfazed.&00", FALSE, ch, 0, vict, TO_VICT);
   act("&09$n's&15 slams $s elbow into &14$N&15's neck, but $e is unfazed.&00", FALSE, ch, 0, vict, TO_NOTVICT);
   GET_MANA(ch) -= GET_MAX_MANA(ch) / 4;
   return;
  }
  if (GET_DEX(ch) <= GET_DEX(vict) + number(5, 20)) {
   act("&15You slam your elbow into &09$N&15's neck, but miss the vital point and fail to stun $M!", FALSE, ch, 0, vict, TO_CHAR);
   act("&09$n &15slams $s elbow into your neck, but misses the vital point and fails to stun you!", FALSE, ch, 0, vict, TO_VICT);
   act("&09$n &15slams $s elbow into &14$N&15's neck, but misses the vital point and fails to stun $M.", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) -= GET_MAX_MANA(ch) / 4;
    return;
  } 
  else {
   act("&15You quickly move, catching &14$N&15 offguard, and slamming your elbow into the back of $S neck you stun $S body, numbing it completely!", FALSE, ch, 0, vict, TO_CHAR);
   act("&14$n&15 quickly moves, catching you offguard, and slamming $s elbow into the back of your neck $e stuns your body, numbing it completely!!", FALSE, ch, 0, vict, TO_VICT);
   act("&14$n&15 quickly moves, catching &14$N&15 offguard, and slamming $s elbow into the back of $S neck $e stuns $N's&15 body, numbing it completely!", FALSE, ch, 0, vict, TO_NOTVICT);
   GET_POS(vict) = POS_STUNNED;
   WAIT_STATE(ch, PULSE_VIOLENCE * 1);
   GET_MANA(ch) -= GET_MAX_MANA(ch) / 4;
  }
}


ACMD(do_block)
{
 if (IS_NPC(ch)) {
  return;
 }
 if (!GET_SKILL(ch, SPELL_BLOCK)) {
       send_to_char("You don't know how to do that!\r\n", ch);
       return;
      }
 if (CLN_FLAGGED(ch, CLN_DODGE)) {
  send_to_char("You are concentrating on dodging too much!\r\n", ch);
  return;
 }
 if (CLN_FLAGGED(ch, CLN_BLOCK)) {
  send_to_char("You stop concentrating on blocking so much!\r\n", ch);
  REMOVE_BIT(CLN_FLAGS(ch), CLN_BLOCK);
  return;
 }
 if (CLN_FLAGGED(ch, CLN_ADEF)) {
  send_to_char("You are can't possibly put any more defense into your strategy!\r\n", ch);
  return;
 }
 if (GET_MANA(ch) < GET_MAX_MANA(ch) / 8) {
  send_to_char("You do not have enough ki to effectivly block hits.\r\n", ch);
  return;
 }
 else if (GET_MANA(ch) >= GET_MAX_MANA(ch) / 8) {
  act("&15You charge some ki and begin to move faster in an attempt to block more hits.", FALSE, ch, 0, 0, TO_CHAR);
  act("&14$n &15charges some ki and begins to move faster in an attempt to block more hits.", FALSE, ch, 0, 0, TO_ROOM);
  GET_MANA(ch) -= GET_MAX_MANA(ch) / 8;
  SET_BIT(CLN_FLAGS(ch), CLN_BLOCK);
  return;
 }
}

ACMD(do_dodge)
{
 if (IS_NPC(ch)) {
  return;
 }
 if (!GET_SKILL(ch, SPELL_DODGE)) {
       send_to_char("You don't know how to do that!\r\n", ch);
       return;
      }
 if (CLN_FLAGGED(ch, CLN_BLOCK)) {
  send_to_char("You are concentrating on blocking too much!\r\n", ch);
  return;
 }
 if (CLN_FLAGGED(ch, CLN_DODGE)) {
  send_to_char("You stop concentrating on dodging so much!\r\n", ch);
  REMOVE_BIT(CLN_FLAGS(ch), CLN_DODGE);
  return;
 }
 if (CLN_FLAGGED(ch, CLN_ADEF)) {
  send_to_char("You are can't possibly put any more defense into your strategy!\r\n", ch);
  return;
 }
 if (GET_MANA(ch) < GET_MAX_MANA(ch) / 8) {
  send_to_char("You do not have enough ki to effectivly dodge hits.\r\n", ch);
  return;
 }
 else if (GET_MANA(ch) >= GET_MAX_MANA(ch) / 8) {
  act("&15You charge some ki and begin to move faster in an attempt to dodge more hits.", FALSE, ch, 0, 0, TO_CHAR);
  act("&14$n &15charges some ki and begins to move faster in an attempt to dodge more hits.", FALSE, ch, 0, 0, TO_ROOM);
  GET_MANA(ch) -= GET_MAX_MANA(ch) / 8;
  SET_BIT(CLN_FLAGS(ch), CLN_DODGE);
  return;
 }
}

ACMD(do_adef)
{
 if (IS_NPC(ch)) {
  return;
 }
 if (!GET_SKILL(ch, SPELL_ADEF)) {
       send_to_char("You don't know how to do that!\r\n", ch);
       return;
      }
 if (CLN_FLAGGED(ch, CLN_BLOCK)) {
  send_to_char("You are concentrating on blocking too much!\r\n", ch);
  return;
 }
 if (CLN_FLAGGED(ch, CLN_DODGE)) {
  send_to_char("You are concentrating on blocking too much!\r\n", ch);
  return;
 }
 if (CLN_FLAGGED(ch, CLN_ADEF)) {
  send_to_char("You stop concentrating on all out defense!\r\n", ch);
  REMOVE_BIT(CLN_FLAGS(ch), CLN_ADEF);
  return;
 }
 if (GET_MANA(ch) < GET_MAX_MANA(ch) / 4) {
  send_to_char("You do not have enough ki to effectivly block hits.\r\n", ch);
  return;
 }
 else if (GET_MANA(ch) >= GET_MAX_MANA(ch) / 4) {
  act("&15You charge some ki and begin to move faster in an attempt to dodge and block more hits. like a total badass.", FALSE, ch, 0, 0, TO_CHAR);  
  act("&14$n &15charges some ki and begins to move faster in an attempt to dodge and block more hits, like a total badass.", FALSE, ch, 0, 0, TO_ROOM);
  GET_MANA(ch) -= GET_MAX_MANA(ch) / 4;
  SET_BIT(CLN_FLAGS(ch), CLN_ADEF);
  return;
 }
}
ACMD(do_assist)

{
  struct char_data *helpee, *opponent;

  if (FIGHTING(ch)) {
    send_to_char("You're already fighting!  How can you assist someone else?\r\n", ch);
    return;
  }
  one_argument(argument, arg);

  if (!*arg)
    send_to_char("Whom do you wish to assist?\r\n", ch);
  else if (!(helpee = get_char_room_vis(ch, arg)))
    send_to_char(NOPERSON, ch);
  else if (helpee == ch)
    send_to_char("You can't help yourself any more than this!\r\n", ch);
  else {
    /*
     * Hit the same enemy the person you're helping is.
     */
    if (FIGHTING(helpee))
      opponent = FIGHTING(helpee);
    else
      for (opponent = world[ch->in_room].people;
	   opponent && (FIGHTING(opponent) != helpee);
	   opponent = opponent->next_in_room)
		;

    if (!opponent)
      act("But nobody is fighting $M!", FALSE, ch, 0, helpee, TO_CHAR);
    else if (!CAN_SEE(ch, opponent))
      act("You can't see who is fighting $M!", FALSE, ch, 0, helpee, TO_CHAR);
    else if (!pk_allowed && !IS_NPC(opponent))	/* prevent accidental pkill */
      act("Use 'murder' if you really want to attack $N.", FALSE,
	  ch, 0, opponent, TO_CHAR);
    else {
      send_to_char("You join the fight!\r\n", ch);
      act("$N assists you!", 0, helpee, 0, ch, TO_CHAR);
      act("$n assists $N.", FALSE, ch, 0, helpee, TO_NOTVICT);
      hit(ch, opponent, TYPE_UNDEFINED);
    }
  }
}


ACMD(do_hit)
{
  struct char_data *vict;
  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return;
  }
  one_argument(argument, arg);
  if (!*arg)
    send_to_char("Hit who?\r\n", ch);
  else if (!(vict = get_char_room_vis(ch, arg)))
    send_to_char("They don't seem to be here.\r\n", ch);
  else if (vict == ch) {
    send_to_char("You hit yourself...OUCH!.\r\n", ch);
    act("$n hits $mself, and says OUCH!", FALSE, ch, 0, vict, TO_ROOM);
  } else if (AFF_FLAGGED(ch, AFF_MAJIN) && (ch->master == vict))
    act("$N is just such a good friend, you simply can't hit $M.", FALSE, ch, 0, vict, TO_CHAR);
  else {
    if (!pk_allowed) {
      if (!IS_NPC(vict) && !IS_NPC(ch)) {
	if (subcmd != SCMD_MURDER) {
	  send_to_char("Use 'murder' to hit another player.\r\n", ch);
	  return;
	} else {
	  check_killer(ch, vict);
	}
      }
     }
 if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
 if (GET_POS(ch) == POS_STANDING && GET_POS(vict) == POS_FLOATING) {
    send_to_char("They are up too high for you to reach!\r\n", ch);
    return;
    }
 else if (GET_POS(ch) == POS_FLOATING && GET_POS(vict) == POS_FLOATING) {
    hit(ch, vict, TYPE_UNDEFINED);
    WAIT_STATE(ch, PULSE_VIOLENCE + 2);
    }
 else if (GET_POS(ch) == POS_FLOATING && GET_POS(vict) == POS_STANDING) {
    send_to_char("You fly down and hit them!\r\n", ch);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_FLYING);
    act("&11$n&15 flies down and hits you!&00\r\n", TRUE, ch, 0, 0, TO_VICT);
    act("&11$n&15 flies down and hits &11$N&15!&00\r\n", FALSE, ch, 0, vict, TO_NOTVICT);
    GET_POS(ch) == POS_STANDING;
    hit(ch, vict, TYPE_UNDEFINED);
    WAIT_STATE(ch, PULSE_VIOLENCE + 2);
    }
 else if (GET_POS(ch) == POS_STANDING) {
      hit(ch, vict, TYPE_UNDEFINED);
      WAIT_STATE(ch, PULSE_VIOLENCE + 2);
      }
 }

}



ACMD(do_kill)
{
  struct char_data *vict;

  if ((GET_LEVEL(ch) < 1000000) || IS_NPC(ch)) {
    do_hit(ch, argument, cmd, subcmd);
    return;
  }
  one_argument(argument, arg);

  if (!*arg) {
    send_to_char("Kill who?\r\n", ch);
  } else {
    if (!(vict = get_char_room_vis(ch, arg)))
      send_to_char("They aren't here.\r\n", ch);
    else if (ch == vict)
      send_to_char("Your mother would be so sad.. :(\r\n", ch);
    else {
      act("You chop $M to pieces!  Ah!  The blood!", FALSE, ch, 0, vict, TO_CHAR);
      act("$N chops you to pieces!", FALSE, vict, 0, ch, TO_CHAR);
      act("$n brutally slays $N!", FALSE, ch, 0, vict, TO_NOTVICT);
      raw_kill(vict, ch);
      sprintf(buf, "&16[&10ALERT&16] &14%s &15has been &00&01SLAIN&00", GET_NAME(vict));
      REMOVE_BIT(PRF_FLAGS(vict), PRF_NOHASSLE);
      send_to_all(buf);
    }
  }
  *buf = '\0';
}



ACMD(do_backstab)
{
  struct char_data *vict;
  int percent, prob, dam;

  one_argument(argument, buf);
  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!GET_SKILL(ch, SKILL_BACKSTAB)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  if (!(vict = get_char_room_vis(ch, buf))) {
    send_to_char("Backstab who?\r\n", ch);
    return;
  }
  if (vict == ch) {
    send_to_char("How can you SWIFTNESS up on yourself?\r\n", ch);
    return;
  }
  if (!GET_EQ(ch, WEAR_WIELD)) {
    send_to_char("You need to wield a weapon to make it a success.\r\n", ch);
    return;
  }
  if (GET_OBJ_VAL(GET_EQ(ch, WEAR_WIELD), 3) != TYPE_SLASH - TYPE_HIT) {
    send_to_char("Only swords or daggers can be used for backstabbing.\r\n", ch);
    return;
  }

  if (MOB_FLAGGED(vict, MOB_AWARE)) {
    act("You notice $N lunging at you!", FALSE, vict, 0, ch, TO_CHAR);
    act("$e notices you lunging at $m!", FALSE, vict, 0, ch, TO_VICT);
    act("$n notices $N lunging at $m!", FALSE, vict, 0, ch, TO_NOTVICT);
    hit(vict, ch, TYPE_UNDEFINED);
    return;
  }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  percent = number(1, 101);	/* 101% is a complete failure */
  prob = GET_SKILL(ch, SKILL_BACKSTAB);
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }  
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (AWAKE(vict) && (percent > prob)) {
    damage(ch, vict, 0, SKILL_BACKSTAB);
    return;
    }
  else {
    if (GET_DEX(ch) >= number(80, 150) && GET_MANA(ch) >= GET_MAX_MANA(ch)/10 && GET_MAX_HIT(vict) <= GET_MAX_HIT(ch) * 2) {
     act("&14$N&15 gasps instantly as your sword emerges out through $S chest.", FALSE, ch, 0, vict, TO_CHAR);
     act("&14$n&15 stabs their sword right through your back and out your chest, and you are impaled upon the blade!&00", FALSE, ch, 0, vict, TO_VICT);
     act("&14$N&15 gasps instantly as &14$n&15's sword emerges out through $S chest.", FALSE, ch, 0, vict, TO_NOTVICT);
     
     GET_MANA(ch) -= GET_MAX_MANA(ch) / 10;
     GET_HIT(vict) -= GET_MAX_HIT(vict) - 1;
     if (PLR_FLAGGED(vict, PLR_KILLER) && !IS_NPC(vict)) {
       sprintf(buf2, "&16[&09Backstab!&16] &14%s &15dies, having been skewered by &12%s&15. This was also a &10Player &09K&00&01il&09l &11A&00&03ren&11a win!&00\r\n", GET_NAME(vict), GET_NAME(ch));
       send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
       char_from_room(vict);
       char_to_room(vict, r_death_start_room);
       REMOVE_BIT(PLR_FLAGS(vict), PLR_KILLER);
       REMOVE_BIT(PLR_FLAGS(ch), PLR_KILLER);
       char_from_room(ch);
       char_to_room(ch, r_konack_start_room );
       look_at_room(ch, 0);
       GET_POS(vict) = POS_STANDING;
       look_at_room(vict, 0);
      }
    else if (!PLR_FLAGGED(vict, PLR_KILLER) && !PLR_FLAGGED(vict, PLR_icer) && !IS_NPC(vict)) {
      sprintf(buf2, "&16[&09Backstab!&16] &14%s &15dies, having been skewered by &12%s&15.&00\r\n", GET_NAME(vict), GET_NAME(ch));
      send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
      char_from_room(vict);
      char_to_room(vict, r_death_start_room);
      SET_BIT(CLN_FLAGS(vict), CLN_Death);
      GET_DTIMER(vict) = 3600;
      GET_POS(vict) = POS_STANDING;
      look_at_room(vict, 0);
     }
    else if (PLR_FLAGGED(vict, PLR_icer) && !IS_NPC(vict)) {
      sprintf(buf2, "&16[&09Backstab!&16] &14%s &15dies, having been skewered by &12%s&15. &12%s &15also collected their bounty.&00\r\n", GET_NAME(vict), GET_NAME(ch), GET_NAME(ch));
      send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
      char_from_room(vict);
      REMOVE_BIT(PLR_FLAGS(vict), PLR_icer);
      send_to_char("You collected the 1.5mil bounty on their head. You receive a bonus from the gods for your excellent ability with the sword.\r\n ", ch);
      GET_GOLD(ch) += 1500000;
      char_to_room(vict, r_death_start_room);
      SET_BIT(CLN_FLAGS(vict), CLN_Death);
      GET_DTIMER(vict) = 3600;
      GET_POS(vict) = POS_STANDING;
      look_at_room(vict, 0);
     }
     if (!IS_NPC(ch) && !IS_NPC(vict)) {
      GET_HEIGHT(ch) = GET_HEIGHT(ch) + 1;
      GET_WEIGHT(vict) = GET_WEIGHT(vict) + 1;
      return;
     }
     if (IS_NPC(vict)) {
      damage(ch, vict, 1, TYPE_UNDEFINED);
     }
     }
    else {
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_HIT(ch) / 4, TYPE_UNDEFINED);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_HIT(ch) / 4;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 cries out in pain as they are stabbed&11!&16 [&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15You cry out as you are suddenly stabbed in the back&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act("&14$n &15stabs &09$N &15painfully in the back&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
     WAIT_STATE(ch, PULSE_VIOLENCE * 3);
     WAIT_STATE(vict, PULSE_VIOLENCE * 1); 
  }
 }
}



ACMD(do_order)
{
  char name[MAX_INPUT_LENGTH], message[MAX_INPUT_LENGTH];
  bool found = FALSE;
  int org_room;
  struct char_data *vict;
  struct follow_type *k;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  half_chop(argument, name, message);

  if (!*name || !*message)
    send_to_char("Order who to do what?\r\n", ch);
  else if (!(vict = get_char_room_vis(ch, name)) && !is_abbrev(name, "followers"))
    send_to_char("That person isn't here.\r\n", ch);
  else if (ch == vict)
    send_to_char("You obviously suffer from skitzofrenia.\r\n", ch);

  else {
    if (AFF_FLAGGED(ch, AFF_MAJIN)) {
      send_to_char("Your superior would not aprove of you giving orders.\r\n", ch);
      return;
    }
    if (vict) {
      sprintf(buf, "$N orders you to '%s'", message);
      act(buf, FALSE, vict, 0, ch, TO_CHAR);
      act("$n gives $N an order.", FALSE, ch, 0, vict, TO_ROOM);

      if ((vict->master != ch) || !AFF_FLAGGED(vict, AFF_MAJIN))
	act("$n has an indifferent look.", FALSE, vict, 0, 0, TO_ROOM);
      else {
	send_to_char(OK, ch);
	command_interpreter(vict, message);
      }
    } else {			/* This is order "followers" */
      sprintf(buf, "$n issues the order '%s'.", message);
      act(buf, FALSE, ch, 0, vict, TO_ROOM);

      org_room = ch->in_room;

      for (k = ch->followers; k; k = k->next) {
	if (org_room == k->follower->in_room)
	  if (AFF_FLAGGED(k->follower, AFF_MAJIN)) {
	    found = TRUE;
	    command_interpreter(k->follower, message);
	  }
      }
      if (found)
	send_to_char(OK, ch);
      else
	send_to_char("Nobody here is a loyal subject of yours!\r\n", ch);
    }
  }
}



ACMD(do_flee)
{
  int i, attempt;
  struct char_data *was_fighting;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
  if (!PLR_FLAGGED(ch, PLR_LLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You have no legs!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RLEG) && PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("What are ya gunna do hop on one leg!?\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("What are ya gunna do hop on one leg!?\r\n", ch);
     return;
     }
  if (GET_POS(ch) < POS_FIGHTING) {
    send_to_char("You are in pretty bad shape, unable to flee!\r\n", ch);
    return;
  }
  if (IS_AFFECTED(ch, AFF_MFROZEN)) {
    send_to_char("You would flee, but you can't move.\r\n", ch);
    return;
  }
  if (number(1, 4) <= 1) {
   send_to_char("You can't find an appropriate moment to escape!\r\n", ch);
   return;
  }

  for (i = 0; i < 6; i++) {
    attempt = number(0, NUM_OF_DIRS - 1);	/* Select a random direction */
    if (CAN_GO(ch, attempt) &&
	!ROOM_FLAGGED(EXIT(ch, attempt)->to_room, ROOM_DEATH)) {
      act("$n panics, and attempts to flee!", TRUE, ch, 0, 0, TO_ROOM);
      was_fighting = FIGHTING(ch);
      if (do_simple_move(ch, attempt, TRUE)) {
	send_to_char("You flee head over heels.\r\n", ch);
      } else {
	act("$n tries to flee, but can't!", TRUE, ch, 0, 0, TO_ROOM);
      }
      return;
    }
  }
  send_to_char("PANIC!  You couldn't escape!\r\n", ch);
}


ACMD(do_sweep)
{
  struct char_data *vict;
  int percent, prob;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  one_argument(argument, arg);
  if (!PLR_FLAGGED(ch, PLR_LLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You have no legs!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RLEG) && PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
     }
  if (!GET_SKILL(ch, SKILL_SWEEP)) {
    send_to_char("You have no idea how.\r\n", ch);
    return;
  }
  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return;
  }
  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Sweep who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  percent = number(1, 100);	/* 101% is a complete failure */
  prob = GET_LBS(ch);

  if (MOB_FLAGGED(vict, MOB_NOSWEEP)) {
    act("You can't sweep them!", FALSE, ch, 0, 0, TO_CHAR);
    return;
    }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (percent > prob) {
    act("&15You miss with your sweep!&00\r\n", TRUE, ch, 0, vict, TO_CHAR);
    act("&15$n misses with their sweep at you!&00\r\n", TRUE, ch, 0, vict, TO_VICT);
    act("&15$n misses with their sweep at $N!&00\r\n", TRUE, ch, 0, vict, TO_NOTVICT);
    damage(ch, vict, 1, SKILL_SWEEP);
    WAIT_STATE(ch, PULSE_VIOLENCE * 2);
    return;
    }
 else {
      damage(ch, vict, 1, SKILL_SWEEP);
      act("&15You sweep &14$N&15 off their &09feet&11!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$n&15 sweeps you off your &09feet&11!&00", FALSE, ch, 0, vict, TO_VICT);
      act("&14$n&15 sweeps &14$N&15 off their &09feet&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_POS(vict) = POS_SITTING;
      WAIT_STATE(vict, PULSE_VIOLENCE);
  }
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);
}

ACMD(do_rescue)
{
  struct char_data *vict, *tmp_ch;
  int percent, prob;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  one_argument(argument, arg);
  if (!PLR_FLAGGED(ch, PLR_LLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You have no legs!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RLEG) && PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
     }
  if (!(vict = get_char_room_vis(ch, arg))) {
    send_to_char("Whom do you want to rescue?\r\n", ch);
    return;
  }
  if (vict == ch) {
    send_to_char("What about fleeing instead?\r\n", ch);
    return;
  }
  if (FIGHTING(ch) == vict) {
    send_to_char("How can you rescue someone you are trying to kill?\r\n", ch);
    return;
  }
  for (tmp_ch = world[ch->in_room].people; tmp_ch &&
       (FIGHTING(tmp_ch) != vict); tmp_ch = tmp_ch->next_in_room);

  if (!tmp_ch) {
    act("But nobody is fighting $M!", FALSE, ch, 0, vict, TO_CHAR);
    return;
  }
  if (!GET_SKILL(ch, SKILL_RESCUE))
    send_to_char("But you have no idea how!\r\n", ch);
  else {
    percent = number(1, 101);	/* 101% is a complete failure */
    prob = GET_SKILL(ch, SKILL_RESCUE);

    if (percent > prob) {
      send_to_char("You fail the rescue!\r\n", ch);
      return;
    }
    send_to_char("Banzai!  To the rescue... You take damage in the process!\r\n", ch);
    act("&09You are rescued by $N, and you are confused&16!&00", FALSE, vict, 0, ch, TO_CHAR);
    act("&15$n heroically rescues $N! Taking damage in the process!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    sprintf(buf1, "(GC) %s has rescued %s &10from %s&10.", GET_NAME(ch), GET_NAME(vict), GET_NAME(tmp_ch));
             mudlog(buf1, BRF, LVL_GOD, TRUE);
    GET_HIT(ch) = GET_HIT(ch) - GET_MAX_HIT(ch) / 50;
    if (FIGHTING(vict) == tmp_ch)
      stop_fighting(vict);
    if (FIGHTING(tmp_ch))
      stop_fighting(tmp_ch);
    if (FIGHTING(ch))
      stop_fighting(ch);

    set_fighting(ch, tmp_ch);
    set_fighting(tmp_ch, ch);

    WAIT_STATE(vict, 2 * PULSE_VIOLENCE);
  }

}



ACMD(do_kick)
{
  struct char_data *vict;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  int MOVE = 0, HIT = 0;
  two_arguments(argument, arg1, arg2);
/*------------------------------No Limbs---------------------------*/  
  if (!PLR_FLAGGED(ch, PLR_LLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You have no legs!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RLEG) && PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
}
/*---------------------------Aiming Shots--------------------------*/
/* Kick to the Head */
  if (!(vict = get_char_room_vis(ch, arg1)) && !str_cmp(arg,"head") && GET_STR(ch)*2+GET_MANA(ch)/4*GET_LEVEL(ch)+GET_DAMROLL(ch) > GET_STR(vict)*2+GET_MANA(vict)/4*GET_LEVEL(vict)+GET_DAMROLL(vict)) {

  act("&15You swiftly swing a strong kick, hitting &11$N&15 in the head!&00", FALSE, ch, 0, vict, TO_CHAR);
  act("&12$n&15 smashes &11$N&15 in the head with a powerful kick!", TRUE, ch, 0, vict, TO_NOTVICT);
  act("&12$n&15 moves with great speed, smashing you in the head with a kick!&00", FALSE, ch, 0, vict, TO_VICT);  
  HIT = number(GET_LEVEL(ch)*10, GET_LEVEL(ch)*10);
  MOVE = number(5, 10);
  GET_HIT(vict) = GET_HIT(vict) - HIT;
  }
  
  if (GET_HIT(vict) <=0) {
  act("&15You kill &11$N&15 with the blow!&00", FALSE, ch, 0, vict, TO_CHAR);
  act("&12$n&15 kills &11$N with with blow!&00.", TRUE, ch, 0, vict, TO_NOTVICT);
  act("&12$n&15's kick kills you!&00", FALSE, ch, 0, vict, TO_VICT);
  char_from_room(vict);
  char_to_room(vict, r_death_start_room);
  GET_HIT(vict) = GET_MAX_HIT(vict)*0.1;
  GET_MANA(vict) = GET_MAX_MANA(vict)*0.1;
  GET_MOVE(vict) = GET_MOVE(ch);
  GET_POS(vict) = POS_STANDING;
  look_at_room(vict, 0);
  }
/*-----------------------------------------------------------------*/
  if (!(vict = get_char_room_vis(ch, arg1))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Kick who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
}

ACMD(do_forelock)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_FORELOCK)) {
     send_to_char("You don't have a Forelock!\r\n", ch);
     return;
     }
  if (!GET_SKILL(ch, SKILL_FORELOCK)) {
    send_to_char("You don't know how!\r\n", ch);
    return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Forelock whip who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  percent = number(1, 100);
  prob = GET_UBS(ch);

  if (percent > prob) {
    damage(ch, vict, 0, SKILL_FORELOCK);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    return;
  } else
    act("You &15wrap your forelock at &14$N&00&06'&15s throat, and slam them down&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("$n &15wraps their forelock around &14your&15 throat, and slams you down&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("$n &15wraps their forelock around &14$N&00&06'&15s throat, and slams them down&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_HIT(ch) /70 + (GET_UBS(ch) * GET_DEX(ch) * 10), SKILL_FORELOCK);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_HIT(ch) /70 + (GET_UBS(ch) * GET_DEX(ch) * 10);
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in pain from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
    GET_POS(vict) = POS_SITTING;
    WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);
}

ACMD(do_tailwhip)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';


  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_TAIL)) {
     send_to_char("You don't have a tail!\r\n", ch);
     return;
     }
  if (!GET_SKILL(ch, SKILL_TAILWHIP)) {
    send_to_char("You don't know how!\r\n", ch);
    return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Tailwhip who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  percent = number(1, 100);
  prob = GET_LBS(ch);

  if (percent > prob) {
    damage(ch, vict, 0, SKILL_TAILWHIP);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    return;
  } else
    act("You &15swing your tail at &14$N&00&06'&15s throat&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("$n &15swings their tail at &14your&15s throat&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("$n &15swings their tail at &14$N&00&06'&15s throat&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_HIT(ch) /70 + (GET_LBS(ch) * GET_DEX(ch) * 10), SKILL_TAILWHIP);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_HIT(ch) /70 + (GET_LBS(ch) * GET_DEX(ch) * 10);
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
    GET_POS(vict) = POS_SITTING;
    WAIT_STATE(vict, PULSE_VIOLENCE);
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);
}

ACMD(do_knee)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You have no legs!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RLEG) && PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
     }
  if (!GET_SKILL(ch, SKILL_KNEE)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Knee who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  percent = number(1, 100);
  prob = GET_LBS(ch);

  if (percent > prob) {
    damage(ch, vict, 0, SKILL_KNEE);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    return;
  } else {
    act("&11You &15crush your knee into &14$N's &15gut&11!", FALSE, ch, 0, vict, TO_CHAR);
    act("&11$n &15crushes their knee into &14your &15gut&11!", FALSE, ch, 0, vict, TO_VICT);
    act("&11$n &15crushes their knee into &14$N's &15gut&11!", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_HIT(ch) / 90 + (GET_LBS(ch) * GET_DEX(ch) * 10), SKILL_KNEE);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_HIT(ch) / 90 + (GET_LBS(ch) * GET_DEX(ch) * 10);
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
}
}

ACMD(do_elbow)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';


  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You have no legs!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RLEG) && PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
     }
  if (!GET_SKILL(ch, SKILL_ELBOW)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Elbow who?\r\n", ch);
      return;
    }
  }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  percent = number(1, 100);
  prob = GET_UBS(ch);

  if (percent > prob) {
    damage(ch, vict, 0, SKILL_ELBOW);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    return;
  } else {
    act("&15You spin and ram your &10elbow &15into &14$N's &09spine&11.&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15$n spins and rams their &10elbow &15into &14your &09spine&11.&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15$n spins and rams their &10elbow &15into &14$N's &09spine&11.&00", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_HIT(ch) /90 + (GET_UBS(ch) * GET_DEX(ch) * 10), SKILL_ELBOW);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_HIT(ch) /90 + (GET_UBS(ch) * GET_DEX(ch) * 10);
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
}
}

ACMD(do_roundhouse)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You have no legs!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RLEG) && PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
}
  if (PLR_FLAGGED(ch, PLR_RLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You only have one leg!\r\n", ch);
     return;
     }
  if (!GET_SKILL(ch, SKILL_ROUNDHOUSE)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Roundhouse who?\r\n", ch);
      return;
    }
  }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  percent = number(1, 100);
  prob = GET_LBS(ch);

  if (percent > prob) {
    damage(ch, vict, 0, SKILL_ROUNDHOUSE);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    return;
  } else {
    act("&11You &15leap into the air and spin doing a &12roundhouse&15 into the side of &14$N's head&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&11$n &15leaps into the air and spins doing a &12roundhouse&15 into the side of &14your head&00", FALSE, ch, 0, vict, TO_VICT);
    act("&11$n &15leaps into the air and spins doing a &12roundhouse&15 into the side of &14$N's head&00", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_HIT(ch) /60 + (GET_LBS(ch) * GET_DEX(ch) * 10), SKILL_ROUNDHOUSE);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_HIT(ch) /60 + (GET_LBS(ch) * GET_DEX(ch) * 10);
    
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
}
}

ACMD(do_twin)
{
  struct char_data *vict;
  int percent, prob, dam, r_num = 0;
  struct obj_data *obj;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_TWIN)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  if (!GET_EQ(ch, WEAR_WIELD)) {
    send_to_char("You need to wield a sword to use Twin Blade Slash.\r\n", ch);
    return;
  }
  if (GET_OBJ_VAL(GET_EQ(ch, WEAR_WIELD), 3) != TYPE_SLASH - TYPE_HIT) {
    send_to_char("Only you need a sword!.\r\n", ch);
    return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60) {
     send_to_char("You don't have the Ki to do a Twin Blade Slash!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Twin Blade Slash who?\r\n", ch);
      return;
    }
  }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (!IS_NPC(vict) && number(1, 20) >= 12 && percent < prob && GET_MAX_HIT(ch) >= ((GET_MAX_HIT(vict) / 100) * 80)) {
   switch(number(1, 5)) {
    case 1:
    if (PLR_FLAGGED(ch, PLR_RARM)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_RARM);
    act("&14$N&15 loses their &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2099;
    if ((r_num = real_object(2099)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_RARM);
     return;
    }
    }
    break;
    case 2:
    if (PLR_FLAGGED(ch, PLR_LARM)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_LARM);
    act("&14$N&15 loses their &14left arm&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14left arm&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14left&15 arm in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2096;
    if ((r_num = real_object(2096)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_LARM);
     return;
    }
    }
    break;
    case 3:
    if (PLR_FLAGGED(ch, PLR_RLEG)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_RLEG);
    act("&14$N&15 loses their &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2098;
    if ((r_num = real_object(2098)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_RLEG);
     return;
    }
    }
    break;
    case 4:
    if (PLR_FLAGGED(ch, PLR_LLEG)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_LLEG);
    act("&14$N&15 loses their &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2097;
    if ((r_num = real_object(2097)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_LLEG);
     return;
    }
    }
    break;
    case 5:
    if (PRF2_FLAGGED(ch, PRF2_HEAD) && number(1, 30) >= 24) {
    REMOVE_BIT(PRF2_FLAGS(vict), PRF2_HEAD);
    act("&14$N&15 loses their &14head&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14head&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14head&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    if (PLR_FLAGGED(vict, PLR_KILLER) && !IS_NPC(vict)) {
       sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15. This was also a &10Player &09K&00&01il&09l &11A&00&03ren&11a win!&00\r\n", GET_NAME(vict), GET_NAME(ch));
       send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
       char_from_room(vict);
       char_to_room(vict, r_death_start_room);
       REMOVE_BIT(PLR_FLAGS(vict), PLR_KILLER);
       REMOVE_BIT(PLR_FLAGS(ch), PLR_KILLER);
       char_from_room(ch);
       char_to_room(ch, r_mortal_start_room );
       look_at_room(ch, 0);
       GET_POS(vict) = POS_STANDING;
       look_at_room(vict, 0);
      }
    else if (!PLR_FLAGGED(vict, PLR_KILLER) && !PLR_FLAGGED(vict, PLR_icer) && !IS_NPC(vict)) {
      sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15.&00\r\n", GET_NAME(vict), GET_NAME(ch));
      send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
      char_from_room(vict);
      char_to_room(vict, r_death_start_room);
      SET_BIT(CLN_FLAGS(vict), CLN_Death);
      GET_DTIMER(vict) = 3600;
      GET_POS(vict) = POS_STANDING;
      look_at_room(vict, 0);
    }
    else if (PLR_FLAGGED(vict, PLR_icer) && !IS_NPC(vict)) {
      sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15. &12%s &15also collected their bounty.&00\r\n", GET_NAME(vict), GET_NAME(ch), GET_NAME(ch));
      send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
      char_from_room(vict);
      REMOVE_BIT(PLR_FLAGS(vict), PLR_icer);
      send_to_char("You collected the 1mil bounty on their head.\r\n ", ch);
      GET_GOLD(ch) += 1000000;
      char_to_room(vict, r_death_start_room);
      SET_BIT(CLN_FLAGS(vict), CLN_Death);
      GET_DTIMER(vict) = 3600;
      GET_POS(vict) = POS_STANDING;
      look_at_room(vict, 0);
     }
    if (!IS_NPC(ch) && !IS_NPC(vict)) {
      GET_HEIGHT(ch) = GET_HEIGHT(ch) + 1;
      GET_WEIGHT(vict) = GET_WEIGHT(vict) + 1;
     }
     r_num = 2095;
     if ((r_num = real_object(2095)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
     }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    *buf2 = '\0';
    sprintf(buf2, "&14%s's &01S&09e&16v&00e&16r&09e&00&01d &00&01H&09e&00a&16d&00", GET_NAME(vict));
    obj->short_description = str_dup(buf2);
    *buf2 = '\0';
    }
    }
    break;
   }
  }

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/60) {
    damage(ch, vict, 0, SKILL_TWIN);
    act("&16$N&16 jumps out of the way before you can strike them with &10T&16w&00&02i&10n &16B&15l&00a&16d&15e &16S&00&06l&00&02a&10s&16h&00&16!", FALSE, ch, 0, vict, TO_CHAR);
    act("&16You jump out of the way just in time to dodge $n&16's &10T&16w&00&02i&10n &16B&15l&00a&16d&15e &16S&00&06l&00&02a&10s&16h&00&16!", FALSE, ch, 0, vict, TO_VICT);
    act("&16$N&16 jumps out of the way of  $n&16's &10T&16w&00&02i&10n &16B&15l&00a&16d&15e &16S&00&06l&00&02a&10s&16h&00&16!", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
    if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else {
 act("&16You hold your swords down to your side, channeling &00&02k&10i&16 into the edges. A &00&04d&12a&16r&15k &10g&00&02r&10ee&00&02n&16 aura forms around the sharp blades. You fly forward, screaming '&10T&16w&00&02i&10n &16B&15l&00a&16d&15e &16S&00&06l&00&02a&10s&16h&00&16!', your body a blur as your swing your blades at $N&16, landing behind then, your blades crossed over your chest. A second later, $N&16 screams as thier chest glows &10g&00&02r&10ee&00&02n&16, then explodes with the &00e&14n&00&06er&14g&00y&16 that were focused into the blades!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&16$n&16 is holding there blades down to there sides, and you feel a build up of &00e&14n&00&06er&14g&00y&16. You notice a &00&04d&12a&16r&15k &10g&00&02r&10ee&00&02n&16 aura build up around the blades. You see them fly at $N&16, as they yell out, '&10T&16w&00&02i&10n &16B&15l&00a&16d&15e &16S&00&06l&00&02a&10s&16h&00&16!' They seem to vanish, then reappear behind $N&16. You see a &10g&00&02r&10ee&00&02n&16 cross on $N&16's chest, which explodes with &00e&14n&00&06er&14g&00y&16!", FALSE, ch, 0, vict, TO_NOTVICT);
 act("&16$n&16 is holding there blades down to there sides, and you feel a build up of &00e&14n&00&06er&14g&00y&16. You notice a &00&04d&12a&16r&15k &10g&00&02r&10ee&00&02n&16 aura build up around the blades. $n fly's at you, screaming, '&10T&16w&00&02i&10n &16B&15l&00a&16d&15e &16S&00&06l&00&02a&10s&16h&00&16!' and seemingly vanishes before your eyes. You notice a &10g&00&02r&10ee&00&02n&16 X on your chest, that suddenly explodes!", FALSE, ch, 0, vict, TO_VICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 20 + 100000, SKILL_TWIN);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_MANA(ch) / 20 + 100000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

    GET_POS(vict) = POS_SITTING;
    WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
}
}

ACMD(do_shogekiha)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}

  if (!GET_SKILL(ch, SKILL_SHOGEKIHA)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/ 100) {
     send_to_char("You don't have the Ki to fire a shogekiha!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Hit who with a shogekiha?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
 if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
 if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
 if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch) /100) {
    damage(ch, vict, 0, SKILL_SHOGEKIHA);
    act("&15$N somehow dodges your &15S&16h&00&06o&15g&16e&00&06k&15i!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/100;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  }
else {
   act("&15You charge a small amount of ki within your eyes and fire an invisible &15S&16h&00&06o&15g&16e&00&06k&15i at $N&15, catching them in the gut!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&15$n slams you with some kind of unseen attack!&00", FALSE, ch, 0, vict, TO_VICT);
   act("&15$N jerks back, slammed with an unknown attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
       SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
       damage(ch, vict, GET_MAX_MANA(ch) / 50 + 55000, SKILL_SHOGEKIHA);
       REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
       dam = GET_MAX_MANA(ch) / 50 + 55000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

 WAIT_STATE(ch, PULSE_VIOLENCE * 1);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/100;
}
}


ACMD(do_tsuihidan)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}

  if (!GET_SKILL(ch, SKILL_TSUIHIDAN)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/ 100) {
     send_to_char("You don't have the Ki to fire a tsuihidan!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Hit who with a tsuihidan?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
 if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
 if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
 if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/100) {
    damage(ch, vict, 0, SKILL_TSUIHIDAN);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/100;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  }
else {
   act("&15With arms fully extended, you place both hands palm out before you.  Focusing &00&03k&11i&15 into the air before the two, you cry out, &11T&09s&00&01u&00&03ihi&00&01d&09a&11n&15!.  A wide globe of &00&03k&11i&15 &00&03e&11n&15er&11g&00&03y&15 fires outward, trailed by a long comet tail of power.  As it flies forward, it moves to follow $N&15 until striking it in a blinding explosion.&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&15$n points both hands at you, palms out.  As you watch, a ball of energy forms in the air and is quickly released with a cry of, &11T&09s&00&01u&00&03ihi&00&01d&09a&11n&15!. The ball races at you followed by long tail of &00&03k&11i&15. You try to dodge out of the way, but the attack has a mind of its own and changes course to chase you.  There is no escape as it slams into you with a blinding explosion.", FALSE, ch, 0, vict, TO_VICT);
   act("&15$n points his palms towards $N&15 and forms a sphere of &00&03k&11i&15 in the air. With a single yelled word, &11T&09s&00&01u&00&03ihi&00&01d&09a&11n&15!, the sphere launches at its intended target.  As it snakes through the air, ball followed by tail, it arcs and weaves to chase down its prey.  Slamming into $N&15, there is an eruption of power and a blinding flash of light.", FALSE, ch, 0, vict, TO_NOTVICT);
       SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
       damage(ch, vict, GET_MAX_MANA(ch) / 50 + 60000, SKILL_TSUIHIDAN);
       REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
       dam = GET_MAX_MANA(ch) / 50 + 60000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

 WAIT_STATE(ch, PULSE_VIOLENCE * 1);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/100;
}
}


ACMD(do_gigabeam)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_GIGABEAM) && !IS_NPC(ch)) {
    send_to_char("You don't know how to do that!\r\n", ch);
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30 && !IS_NPC(ch)) {
     send_to_char("You don't have the Ki to do a Giga Beam!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Giga Beam who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30 && !IS_NPC(ch)) {
    damage(ch, vict, 0, SKILL_GIGABEAM);
    act("&15Your &16G&10i&00&02g&16a&10s &10B&00&02e&16a&10m&15 misses $N&15 by a long shot.&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15$n &16G&10i&00&02g&16a&10s &10B&00&02e&16a&10m&15 misses you by a long shot.&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15$n &16G&10i&00&02g&16a&10s &10B&00&02e&16a&10m&15 misses $N&15 by a long shot.&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else {
 act("&15You lean back as &16d&15a&00r&16k &00e&14n&00&06er&14g&00y&15 swells around your body. You bring your right fist up as your &00e&14n&00&06er&14g&00y&15 begins to pool there. Just as the attack is ready you bring it forward and fire your &16G&10i&00&02g&16a&10s &10B&00&02e&16a&10m&15 at $N&15, slamming into them!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&15$n&15 leans back as &16d&15a&00r&16k &00e&14n&00&06er&14g&00y&15 swells around thier body. $n&15 bring thier right fist up as the &00e&14n&00&06er&14g&00y&15 begins to pool there. Just as the attack is ready $n brings it forward and fires the &16G&10i&00&02g&16a&10s &10B&00&02e&16a&10m&15 at $N&15, slamming into them!&00", FALSE, ch, 0, vict, TO_NOTVICT);
 act("&15$n&15 leans back as &16d&15a&00r&16k &00e&14n&00&06er&14g&00y&15 swells around thier body. $n&15 bring thier right fist up as the &00e&14n&00&06er&14g&00y&15 begins to pool there. Just as the attack is ready $n&15 brings it forward and fires the &16G&10i&00&02g&16a&10s &10B&00&02e&16a&10m&15 at you&15, slamming into your chest!&00", FALSE, ch, 0, vict, TO_VICT);
    if (!IS_NPC(ch)) {
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 20 + 125000, SKILL_GIGABEAM);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_MANA(ch) / 20 + 125000;
    }
    if (IS_NPC(ch)) {
    damage(ch, vict, GET_LEVEL(ch), SKILL_GIGABEAM);
    dam = GET_LEVEL(ch);
    }
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
 
    GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);
if (!IS_NPC(ch)) {
 GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}
}
}


ACMD(do_hellspear)
{
  struct char_data *vict, *next_v;
  int dam, num = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_HELLSPEAR) && !IS_NPC(ch)) {
    send_to_char("You don't know how to do that!\r\n", ch);
   return;
  }
  if (IS_NPC(ch) && AFF_FLAGGED(ch, AFF_MAJIN)) {
   return;
  }
  if (!GET_EQ(ch, WEAR_WIELD) && !IS_NPC(ch)) {
    send_to_char("You need to wield a spear to use Hell Spear Blast.\r\n", ch);
    return;
  }
  if (!IS_NPC(ch)) {
   if ((GET_OBJ_VAL(GET_EQ(ch, WEAR_WIELD), 3) != TYPE_STAB - TYPE_HIT)) {
     send_to_char("Only you need a spear!.\r\n", ch);
     return;
   }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (!IS_NPC(ch)) {
   SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/25 && !IS_NPC(ch)) {
     send_to_char("You don't have the Ki to do a Hell Spear Blast!\r\n", ch);
     return;
  } 
 else {
 act("&11You &10grin &16e&13v&16i&13l&16l&13y&15 as you pull out your spear. You begin to spin your spear wickedly right as you begin to pump &00&06e&00&02n&10er&00&02g&00&06y&15 into it. After only a few moments the spear begins to radiate &09r&00&01e&09d&15. Right as it reaches the utmost capacity, you launch the &09H&00&01e&09l&00&01l &16S&15p&00&07e&15a&16r &10B&00&02l&10a&00&02s&10t &15down at the ground, and watch it explode into a fury of energy&11!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&11$n &10grins &16e&13v&16i&13l&16l&13y&15 as he pulls out his spear. &10$n&15 begins to spin his spear wickedly right as he begins to pump &00&06e&00&02n&10er&00&02g&00&06y&15 into it. After only a few moments the spear begins to radiate &09r&00&01e&09d&15. Right as it reaches the utmost capacity, &10$n&15 launches the &09H&00&01e&09l&00&01l &16S&15p&00&07e&15a&16r &10B&00&02l&10a&00&02s&10t &15at the ground, the spear explodes catching you in the explosion&11!&00", FALSE, ch, 0, vict, TO_ROOM);
 for (vict = world[ch->in_room].people; vict; vict = next_v) {
      next_v = vict->next_in_room;
      if (vict == ch) {
        continue;
      }
      if (num >= 20 && !IS_NPC(ch)) {
        continue;
      }
      if ((GET_MAX_HIT(vict) <= 4999999 && !IS_NPC(vict)) && !IS_NPC(ch)) {
        continue;
      }
      if (CLN_FLAGGED(vict, CLN_ADEF) && !IS_NPC(ch)) {
       continue;
      }
      if (!IS_NPC(ch)) {
       num += 1;
       damage(ch, vict, GET_MAX_MANA(ch) / 20 + 250000, SKILL_HELLSPEAR);
      }
      else if (IS_NPC(ch)) {
       damage(ch, vict, GET_LEVEL(ch), SKILL_HELLSPEAR);
      }
      GET_POS(vict) = POS_SITTING;
      WAIT_STATE(vict, PULSE_VIOLENCE);
      if (number(1, 4) == 2) {
        WAIT_STATE(vict, PULSE_VIOLENCE * 2);
      }
   if (!IS_NPC(ch)) {
    WAIT_STATE(ch, PULSE_VIOLENCE * 2);
   }
  }
    if (!IS_NPC(ch)) {
      dam = GET_MAX_MANA(ch) / 20 + 250000;
      }
    if (IS_NPC(ch)) {
      dam = GET_LEVEL(ch);
      }
      if (dam > 5000000) {
       dam = 5000000;
       }
      if (dam < 1) {
       dam = 1;
       }
      sprintf(buf, "&15You hit everything in the room! &16[&12%d&16]&00", dam);
      sprintf(buf2, "&15You are hit by the attack!&16[&09%d&16]&00", dam);
      act(buf, FALSE, ch, NULL, vict, TO_CHAR);
      act(buf2, FALSE, ch, NULL, vict, TO_ROOM);
  if (!IS_NPC(ch)) {
   GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/25;
  }
 }
if (!IS_NPC(ch)) {
   REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
  }
}

ACMD(do_scatter)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_SCATTER) && !IS_NPC(ch)) {
    send_to_char("You don't know how to do that!\r\n", ch);
   return;
  }
  if (IS_NPC(ch) && AFF_FLAGGED(ch, AFF_MAJIN)) {
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30 && !IS_NPC(ch)) {
     send_to_char("You don't have the Ki to do Scatter Shot!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Scattershot who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999 && !IS_NPC(ch)) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch) / 30 && !IS_NPC(ch)) {
    damage(ch, vict, 0, SKILL_SCATTER);
    act("$N&15 zanzokens away from your '&09S&00&01c&16a&15t&11t&00&03e&11r&15s&16h&00&01o&09t&15!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You zanzoken away from $n&15's '&09S&00&01c&16a&15t&11t&00&03e&11r&15s&16h&00&01o&09t&15!&00", FALSE, ch, 0, vict, TO_VICT);
    act("$N&15 zanzokens away from $n&15's '&09S&00&01c&16a&15t&11t&00&03e&11r&15s&16h&00&01o&09t&15!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else {
    act("$n&15 thrusts thier arms out, firing &11ki&00&03ba&11ll&15 after &11ki&00&03ba&11ll&15 around $N&15. $n keeps going, filling the skies with balls of &00e&14n&00&06er&14g&00y&15, the air crackling as &00e&15l&11e&00&03c&16t&15r&16i&00&03c&11i&15t&00y&15 jumps from one ball to another. As $n seems to finish firing the &11k&00&03i&15, they yell out, '&09S&00&01c&16a&15t&11t&00&03e&11r&15s&16h&00&01o&09t&15!' $n raises thier right hand at $N&15, and close thier hand, causing the &11ki&00&03ba&11ll&00&03s&15 to fly in and slam into $N&15 from all sides!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    act("$n&15 thrusts thier arms out, firing &11ki&00&03ba&11ll&15 after &11ki&00&03ba&11ll&15 around you. $n keeps going, filling the skies with balls of &00e&14n&00&06er&14g&00y&15, the air crackling as &00e&15l&11e&00&03c&16t&15r&16i&00&03c&11i&15t&00y&15 jumps from one ball to another. As $n seems to finish firing the &11k&00&03i&15, they yell out, '&09S&00&01c&16a&15t&11t&00&03e&11r&15s&16h&00&01o&09t&15!' $n raises thier right hand at $N&15, and close thier hand, causing the &11ki&00&03ba&11ll&00&03s&15 to fly in and slam into you from all sides!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15You charge &11k&00&03i&15 into your body, and then thrust your arms out, firing &11ki&00&03ba&11ll&15 after &11ki&00&03ba&11ll&15 around $N&15. You keep going, filling the skies with balls of &00e&14n&00&06er&14g&00y&15, the air crackling as &00e&15l&11e&00&03c&16t&15r&16i&00&03c&11i&15t&00y&15 jumps from one ball to another. As you finish firing the &11k&00&03i&15, you yell out, '&09S&00&01c&16a&15t&11t&00&03e&11r&15s&16h&00&01o&09t&15!' You raise your right hand at $N&15, and close your hand, causing the &11ki&00&03ba&11ll&00&03s&15 to fly in and slam into $N&15 from all sides!&00", FALSE, ch, 0, vict, TO_CHAR);
    if (!IS_NPC(ch)) {
     SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     damage(ch, vict, GET_MAX_MANA(ch) / 20 + 315000, SKILL_SCATTER);
     REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 20 + 315000;
    }
    else if (IS_NPC(ch)) {
     damage(ch, vict, GET_LEVEL(ch), SKILL_SCATTER);
     dam = GET_LEVEL(ch);
    }
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
     GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);
 if (!IS_NPC(ch)) {
  GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
 }
}
}


ACMD(do_spirit)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_SPIRITBALL)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do a Spirit Ball!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Spirit Ball who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30) {
    damage(ch, vict, 0, SKILL_SPIRITBALL);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else {
 act("&11You &15hold your right hand out, palm upward with your other hand gripping your wrist, and form a &09large&15 radiating &14S&12p&14i&12r&14i&12t&16-&11B&09a&11l&09l&15. You quickly launch it at $N&15. Bringing it down on them from above in a brilliant &09e&16x&09p&16l&09o&16s&09i&16o&09n&15!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&15$n holds their right hand out, palm upward with their other hand gripping their wrist, and form a &09large&15 radiating &14S&12p&14i&12r&14i&12t&16-&11B&09a&11l&09l&15. They quickly launch it at $N&15. Bringing it down on them from above in a brilliant &09e&16x&09p&16l&09o&16s&09i&16o&09n&15!&00", FALSE, ch, 0, vict, TO_NOTVICT);
 act("&15$n holds their right hand out, palm upward with their other hand gripping their wrist, and form a &09large&15 radiating &14S&12p&14i&12r&14i&12t&16-&11B&09a&11l&09l&15. They quickly launch it at you. Bringing it down on them from above in a brilliant &09e&16x&09p&16l&09o&16s&09i&16o&09n&15!&00", FALSE, ch, 0, vict, TO_VICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 20 + 345000, SKILL_SPIRITBALL);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_MANA(ch) / 20 + 345000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

    GET_POS(vict) = POS_SITTING;
    WAIT_STATE(vict, PULSE_VIOLENCE);
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}
}

ACMD(do_yoiko)
{
  struct char_data *vict;
  int percent, prob;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  one_argument(argument, arg);
  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!GET_SKILL(ch, SKILL_YOIKOMINMINKEN)) {
    send_to_char("You have no idea how.\r\n", ch);
    return;
  }
  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return;
  }
  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Sleep who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/40) {
     send_to_char("You don't have the Ki to do Yoikominminken!\r\n", ch);
    return;
  }

  percent = number(1, 60);     /* 101% is a complete failure */
  prob = GET_REINC(ch);

  
  if (percent > prob) {
    damage(ch, vict, 0, SKILL_YOIKOMINMINKEN);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/40;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else {
    if (damage(ch, vict, 1, SKILL_YOIKOMINMINKEN) > 0) { /* -1 = dead, 0 = miss */
      act("&15$n waves their arms in lulling motions as they focus ki into their eyes....slowly....putting...you to...sleeeeep...&00", FALSE, ch, 0, vict, TO_VICT);
      act("&15$n waves their arms in lulling motions as they focus ki into their eyes....slowly....putting...$N to...sleeeeep...", FALSE, ch, 0, vict, TO_NOTVICT);
      act("&15You wave your arms in lulling motions as you focus ki into your eyes....slowly....putting...$N to...sleeeeep...", FALSE, ch, 0, vict, TO_CHAR);
      GET_POS(vict) = POS_SLEEPING;
      WAIT_STATE(vict, PULSE_VIOLENCE);
    }
  }
  WAIT_STATE(ch, PULSE_VIOLENCE * 3);
GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/40;
}



ACMD(do_dragon)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_SEVENDRAGON)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do Dance of the Seven Dragons!\r\n", ch);
    return;
  }

 one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Dance of the Seven Dragons who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
 if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
 if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }  
 if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);
 
 if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30) {
    damage(ch, vict, 0, SKILL_SEVENDRAGON);
    act("$N&15 jumps out of range of your attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("You jump out of range of $n&15's attack!", FALSE, ch, 0, vict, TO_VICT);
    act("$N jumps out of range of $n&15's attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
 act("&15You run at $N&15, and jump into the air. As you draw energy into your body, you shout, &16D&00&15a&00&07n&00&15c&00&16e &00of the &06S&00&14e&00v&14e&00&06n &00&10D&00&02r&15ag&00&02o&00&10n&15s! And fly at your opponent, and deliver a hellacious combo of punches and kicks to their body in a blur of violence!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&15$n runs at $N&15, they jump majestically into the air, and glow a soft yellow, then scream out, &16D&00&15a&00&07n&00&15c&00&16e &00of the &06S&00&14e&00v&14e&00&06n &00&10D&00&02r&15ag&00&02o&00&10n&15s! They seemingly vanish, then the sounds of thudding impacts on $N&15 resonate through the area!&00", FALSE, ch, 0, vict, TO_NOTVICT);
 act("$n&15 runs at you, then jumps in the air while they start to glow. And as they scream, '&16D&00&15a&00&07n&00&15c&00&16e &00of the &06S&00&14e&00v&14e&00&06n &00&10D&00&02r&15ag&00&02o&00&10n&15s!' You feel your body become pummeled with punches and kicks that seem to come from no where!&00", FALSE, ch, 0, vict, TO_VICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 20 + 400000, SKILL_SEVENDRAGON);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_MANA(ch) / 20 + 400000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

    GET_POS(vict) = POS_SITTING;
    WAIT_STATE(vict, PULSE_VIOLENCE);
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}

ACMD(do_genocide)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_GENOCIDE)) {
    send_to_char("You don't know how to do that!\r\n", ch);
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do a Genocide!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Genocide?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch) /30) {
    damage(ch, vict, 0, SKILL_GENOCIDE);
    act("$N&16 somehow dodges your &00&05G&16e&13n&15o&00c&05i&16d&13e&16!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&16You somehow dodges $n&16's &00&05G&16e&13n&15o&00c&05i&16d&13e&16!&00", FALSE, ch, 0, vict, TO_VICT);
    act("$N&16 somehow dodges $n&16's &00&05G&16e&13n&15o&00c&05i&16d&13e&16!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
 act("&16You pull &00e&14n&00&06er&14g&00y&16 into your body, your body &15g&14l&00&06o&15w&00&03i&11n&15g&16 a soft &13p&00&05u&16r&13p&00&05l&16e. Swirls of &11k&00&03i&16 begin to wrap around your body, as you lift your right hand up into the air. The energy spins up around your arm, as it continues to build within your body. As you reach maximum charge, you yell out, '&00&05G&16e&13n&15o&00c&05i&16d&13e&16!' Stream after stream of &00e&14n&00&06er&14g&00y&16 fly up into the air, slamming into everything around, and a few blasts slam into $N&16's chest!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("$n&16's body begins &15g&14l&00&06o&15w&00&03i&11n&15g&16 a soft &13p&00&05u&16r&13p&00&05l&16e. Swirls of &11k&00&03i&16 begin to wrap around $n&16's body, as they lift thier right hand up into the air. The energy spins up around thier arm. $n&16 yells out, '&00&05G&16e&13n&15o&00c&05i&16d&13e&16!' Stream after stream of &00e&14n&00&06er&14g&00y&16 fly up into the air, slamming into everything around, and a few blasts slam into your chest!&00", FALSE, ch, 0, vict, TO_VICT);
 act("$n&16's body begins &15g&14l&00&06o&15w&00&03i&11n&15g&16 a soft &13p&00&05u&16r&13p&00&05l&16e. Swirls of &11k&00&03i&16 begin to wrap around $n&16's body, as they lift thier right hand up into the air. The energy spins up around thier arm. $n&16 yells out, '&00&05G&16e&13n&15o&00c&05i&16d&13e&16!' Stream after stream of &00e&14n&00&06er&14g&00y&16 fly up into the air, slamming into everything around, and a few blasts slam into $N&16's chest!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 20 + 330000, SKILL_GENOCIDE);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 20 + 330000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
     GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}


ACMD(do_chikyuu)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_CHIKYUU)) {
    send_to_char("You don't know how to do that!\r\n", ch);
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do a Chikyuu Geki Gama!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Chikyuu Geki Gama who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch) /30) {
    damage(ch, vict, 0, SKILL_CHIKYUU);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
 act("&15You raise your hand above your head, focusing &00&03k&11i&15 into your palm, as you begin pushing more energy into it, it begins to grow. The &00o&14r&00&06b&15 glows a soft &13p&00&05in&13k&15 as it grows, filling the area with its light. When it grows large enough to block out the sky, you yell out, '&13C&00&05h&16i&00&05k&16y&00&05u&13u G&00&05e&16k&13i G&16a&00&05m&13a&15!' And hurl the massive &00&06b&16a&00&06l&16l&15 of destructive energy at $N!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&14$n&15 raises their hand above their head, a small &00o&14r&00&06b&15 of glowing pink energy forming there. The &00o&14r&00&06b&15 suddenly grows to massive proportions, the energy in it ungodly. As it looks like it can't get any stronger, you hear them yell out '&13C&00&05h&16i&00&05k&16y&00&05u&13u G&00&05e&16k&13i G&16a&00&05m&13a&15!' and watch as the pink &00o&14r&00&06b&15 of energy comes at you, taking you and the entire area with it as it explodes!", FALSE, ch, 0, vict, TO_VICT);
 act("&14$n&15 raises their hand above their head, a small &00o&14r&00&06b&15 of glowing pink energy forming there. The &00o&14r&00&06b&15 suddenly grows to massive proportions, the energy in it ungodly. As it looks like it can't get any stronger, you hear them yell out '&13C&00&05h&16i&00&05k&16y&00&05u&13u G&00&05e&16k&13i G&16a&00&05m&13a&15!' and watch as the pink &00o&14r&00&06b&15 of energy flies at &09$N&15, the area exploding with energy!", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 20 + 375000, SKILL_CHIKYUU);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 20 + 375000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
     GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}



ACMD(do_genki)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_GENKI)) {
    send_to_char("You don't know how to do that!\r\n", ch);
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do a Genki Dama!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Genki Dama who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch) /30) {
    damage(ch, vict, 0, SKILL_GENKI);
    act("&15You stare in horror as the &14G&00&06e&00n&15k&14i &00&06D&00a&15m&14a&15 slides harmlessly past opponent, their laughter swallowed by the glow of wasted life.", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You breath a sigh of relief as opponent's &14G&00&06e&00n&15k&14i &00&06D&00a&15m&14a&15 passes you by, it's maelstrom of ki still held in delicate balance.", FALSE, ch, 0, vict, TO_VICT);
    act("$n&15's &14G&00&06e&00n&15k&14i &00&06D&00a&15m&14a&15 flys past $N, their form almost overtaken by the hurricane of ki that once was used to fuel life.", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
 act("&15You raise your hands above your head, and summon &00e&14n&00&06er&14g&00y&15 from every being in the universe.  The ki swirls, and condenses over above your hands to form a massive &12a&00&04z&00&06u&14r&00e o&14r&00&06b&15.  The hum of eternity engulfs your mind as you sense the life force of beings you will never know.  With a mighty heave, you hurl the &14G&00&06e&00n&15k&14i &00&06D&00a&15m&14a&15 with all your might at $N&15, obliterating them in a brilliant flash of &12b&00&04l&00&06u&14e&15 light!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&15$n raises their hands above their head, and summons &00e&14n&00&06er&14g&00y&15 from every being in the universe.  The ki swirls, and condenses over their hands to form a massive &12a&00&04z&00&06u&14r&00e o&14r&00&06b&15.  You can barely feel the life force of the numerous creatures that blessed their ki to $n&15.  With a mighty heave, $n&15 hurls the &14G&00&06e&00n&15k&14i &00&06D&00a&15m&14a&15 with all their might, and it bathes you in scorching &12b&00&04l&00&06u&14e &14f&00&06i&00&04r&12e&15!&00", FALSE, ch, 0, vict, TO_VICT);
 act("$n raises their hands above their head, and summons &00e&14n&00&06er&14g&00y&15 from every being in the universe.  The ki swirls, and condenses over their hands to form a massive &12a&00&04z&00&06u&14r&00e o&14r&00&06b&15.  You can barely feel the life force of the numerous creatures that blessed their ki to $n&15.  With a mighty heave, $n hurls the &14G&00&06e&00n&15k&14i &00&06D&00a&15m&14a&15 with all their might, and swallows $N&15 in it's wrathful maw!", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 20 + 450000, SKILL_GENKI);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 20 + 450000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
     GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}



ACMD(do_burning)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_BURNINGATTACK)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do a Burning Attack!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

   if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Burning Attack who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30) {
    damage(ch, vict, 0, SKILL_BURNINGATTACK);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
 act("&15You hold your arms straight out infront of you, your forearms crackling with &11energy&00. &15 Your arms move in a rythmic fashion, becoming a blur. As the energy reaches max, you throw your arms out and yell, &09B&00&01u&05rn&00&01in&09g &11A&00&03t&09t&00&01a&03c&00&11k&00!&15 and a flaming orb forms and flies at $N&15, engulfing them in an &01inferno!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&15$n begins to move their arms in a stylish fashion, the area around you beginning to heat up as you finally hear &09B&00&01u&00&05rn&00&01in&00&09g &00&11A&00&03t&00&09t&00&01a&00&03c&00&11k!&15 come from them. You watch a raging inferno engulf $N's&15 form as $n unleashes their firey orb!", FALSE, ch, 0, vict, TO_NOTVICT);
 act("&15$n begins to move their arms in a stylish fashion, the area around you beginning to heat up as you finally hear &09B&00&01u&00&05rn&00&01in&00&09g &00&11A&00&03t&00&09t&00&01a&00&03c&00&11k!&15 come from them. You feel a raging inferno engulf your form as they unleash their firey orb!", FALSE, ch, 0, vict, TO_VICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 20 + 355000, SKILL_BURNINGATTACK);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_MANA(ch) / 20 + 355000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
    if (number(1, 25) == 25) {
  if (!IS_NPC(vict)) {
   SET_BIT(PRF_FLAGS(ch), PRF_AUTOEXIT);
   act("&14$N &15has been &09burned&15 severely by the attack!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&15You have been &09burned&15 severely by the attack!", FALSE, ch, 0, vict, TO_VICT);
   act("&14$N &15has been &09burned&15 severely by the attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    GET_MANA(vict) -= GET_MANA(vict);
   }
   else {
    act("&14$N &15has been &09burned&15 severely by the attack!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You have been &09burned&15 severely by the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N &15has been &09burned&15 severely by the attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    GET_HIT(vict) -= GET_HIT(vict) / 4;
   }
  }
    GET_POS(vict) = POS_SITTING;
    WAIT_STATE(vict, PULSE_VIOLENCE);
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}

ACMD(do_bbk)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}

  if (!GET_SKILL(ch, SKILL_BBK)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/ 30) {
     send_to_char("You don't have the Ki to fire a Big Bang Kamehameha!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Hit who with a Big Bang Kamehameha?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
 if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
 if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
 if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30) {
    damage(ch, vict, 0, SKILL_BBK);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  }
else
   act("&16You pull your right and left hands by your side, grinding your teeth as you charge &00e&14n&00&06er&00&14g&00y&00 &16into your palms. The &00e&14n&00&06er&00&14g&00y&00 &16grows as well as your anger, your eyes going pure &00w&15hit&00e &16as you thrust your right fist out, a large &00o&00&14r&00&06b&00 &16of &00e&14n&00&06er&00&14g&00y&00 &16forming a foot from your palm. A ring of pure &00e&14n&00&06er&00&14g&00y&00 &16formed around the &00&03k&00&11i&00 &06b&00&16a&00&06l&00&16l, and you thrust out your other hand, the &00o&00&14r&00&06b&00 &16growing larger as it turns from a &00&06b&00&16a&00&06l&00&16l of &00&05p&00&13u&00rp&13l&00&05e &16ki to a soft &00&04b&00&12lu&00&04e&16. As you push all of your ki into it, you yell, &00&05B&00&13i&00g B&00&15a&00&13n&00&05g&00 &04K&00&12a&00m&00&14e&00&06ha&00&14m&00e&00&12h&00&04a&00&16! For what seems like an eternity, the ball seems to melt into a thick, wide &00&06b&00&14e&00a&14m&00&16, taking out most of the area before you as it slams into $N!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&16$n pulls back their hands by their sides, their face contorts as they grit their teeth. You feel mass amounts of &00e&14n&00&06er&00&14g&00y&00 &16welling up in their body as you see their hands start to form two &00&06b&00&16a&00&06l&00&16ls of different colored energies. They thrust their right hand out, a large &00&06b&00&16a&00&06l&00&16l of &00e&14n&00&06er&00&14g&00y&00 &16forming there, with a ring of &00e&14n&00&06er&00&14g&00y&00 &16forming around it, like a planets ring. They push out their other hand, pushing the other hands &00e&14n&00&06er&00&14g&00y&00 &16into the other hands &00e&14n&00&06er&00&14g&00y&00 &16, making it change a completely different color. The earth quakes like the its the end of days as they yell out, &00&05B&00&13i&00g B&00&15a&00&13n&00&05g&00 &04K&00&12a&00m&00&14e&00&06ha&00&14m&00e&00&12h&00&04a&00&16 The &00&06b&00&16a&00&06l&00&16l explodes into a brilliant &00&04b&00&12lu&00&04e &00w&15hit&00e &00&06b&00&14e&00a&14m&00&16 as it slams into you!&00", FALSE, ch, 0, vict, TO_VICT);
   act("&16$n pulls back their hands by their sides, their face contorts as they grit their teeth. You feel mass amounts of &00e&14n&00&06er&00&14g&00y&00 &16welling up in their body as you see their hands start to form two &00&06b&00&16a&00&06l&00&16ls of different colored energies. They thrust their right hand out, a large &00&06b&00&16a&00&06l&00&16l of &00e&14n&00&06er&00&14g&00y&00 &16forming there, with a ring of &00e&14n&00&06er&00&14g&00y&00 &16forming around it, like a planets ring. They push out their other hand, pushing the other hands &00e&14n&00&06er&00&14g&00y&00 &16into the other hands &00e&14n&00&06er&00&14g&00y&00 &16, making it change a completely different color. The earth quakes like the its the end of days as they yell out, &00&05B&00&13i&00g B&00&15a&00&13n&00&05g&00 &04K&00&12a&00m&00&14e&00&06ha&00&14m&00e&00&12h&00&04a&00&16 The &00&06b&00&16a&00&06l&00&16l explodes into a brilliant &00&04b&00&12lu&00&04e &00w&15hit&00e &00&06b&00&14e&00a&14m&00&16 as it slams into $N!&00", FALSE, ch, 0, vict, TO_NOTVICT);
       SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
       damage(ch, vict, GET_MAX_MANA(ch) / 20 + 750000, SKILL_BBK);
       REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
       dam = GET_MAX_MANA(ch) / 20 + 750000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

       GET_POS(vict) = POS_SITTING;
    WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}


ACMD(do_kamehame)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (IS_AFFECTED(ch, AFF_MFROZEN)) {
    send_to_char("You struggle against your mind freeze...\r\n", ch);
    return;
  }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!GET_SKILL(ch, SKILL_KAMEHAMEHA) && !IS_NPC(ch)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  if (IS_NPC(ch) && AFF_FLAGGED(ch, AFF_MAJIN)) {
   return;
  }
 else  if (GET_MANA(ch) < GET_MAX_MANA(ch)/100  && !IS_NPC(ch)) {
     send_to_char("You don't have the Ki to do a Kamehameha!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Kamehameha who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999  && !IS_NPC(ch)) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/100  && !IS_NPC(ch)) {
    damage(ch, vict, 0, SKILL_KAMEHAMEHA);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/100;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
 act("&15You thrust your hands to your side, forming a &14b&12l&00&06u&00&04e&15 ball of &00&03k&11i&15 between your palms. As you push &00e&14n&00&06er&00&14g&00y&15 into the ball, you thrust your hands forward as you yell out '&12K&14a&00&06m&00&04e&12ha&00&04m&00&06e&14h&12a&15!' unleashing a thick beam of &00&03k&11i&15 at $N&15, slamming into their chest!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&15$n&15 thrust their hands to $e&15 side, forming a &14b&12l&00&06u&00&04e&15 ball of &00&03k&11i&15 between $e&15 palms. As $n&15 pushes &00e&14n&00&06er&00&14g&00y&15 into the ball, $n&15 thrust thier hands forward as $e yells out '&12K&14a&00&06m&00&04e&12ha&00&04m&00&06e&14h&12a&15!' unleashing a thick beam of &00&03k&11i&15 at you, slamming into your chest!&00", FALSE, ch, 0, vict, TO_VICT);
 act("&15$n&15 thrust their hands to $e&15 side, forming a &14b&12l&00&06u&00&04e&15 ball of &00&03k&11i&15 between $e&15 palms. As $n&15 pushes &00e&14n&00&06er&00&14g&00y&15 into the ball, $n&15 thrust thier hands forward as $e yells out '&12K&14a&00&06m&00&04e&12ha&00&04m&00&06e&14h&12a&15!' unleashing a thick beam of &00&03k&11i&15 at $N&15, slamming into their chest!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    if (!IS_NPC(ch)) {
     SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     damage(ch, vict, GET_MAX_MANA(ch) / 50 + 80000, SKILL_KAMEHAMEHA);
     REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 50 + 80000;
    }
    else if (IS_NPC(ch)) {
     damage(ch, vict, GET_LEVEL(ch), SKILL_KAMEHAMEHA);
     dam = GET_LEVEL(ch);
    }
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
 if (!IS_NPC(ch)) {
  GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/100;
 }
}


ACMD(do_bigbang)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!GET_SKILL(ch, SKILL_BIGBANG)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60) {
     send_to_char("You don't have the Ki to do a Big Bang Attack!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Big Bang who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/60) {
    damage(ch, vict, 0, SKILL_BIGBANG);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
    act("&15You turn sideways to &14$N&15, smirking as you lift your left arm up, with your palm pointed at your opponent, thumb tucked in. Your body starts to crackle with &00e&14n&00&06er&00&14g&00y&00 &15as you start to charge ki into your hand. As you feel the &00e&14n&00&06er&00&14g&00y&00 &15rise within your arm, you yell out, &00&05B&00&13i&00g B&00&15a&00&13n&00&05g&00 &03A&00&11t&16ta&11c&00&03k&15! A large &00&06b&14e&00a&14m&00 &15of &00e&14n&00&06er&00&14g&00y&00 &15flies towards &14$N,&15 slamming into them when it hits.&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n&15 turns sideways to you, smirking as they point their left hand at you. You can feel the &00e&14n&00&06er&00&14g&00y&00 &15well up inside them as you hear, &00&05B&00&13i&00g B&00&15a&00&13n&00&05g&00 &03A&00&11t&16ta&11c&00&03k&15! You feel the air heat up as the &00&06b&00&14e&00a&14m&00 &15slams into your body!&00", FALSE, ch, 0, vict, TO_VICT); 
    act("&14$n&15 turns sideways to &14$N, smirking as they point their left hand at &14$N&15. You can feel the &00e&14n&00&06er&00&14g&00y&00 &15well up inside them as you hear, &00&05B&00&13i&00g B&00&15a&00&13n&00&05g&00 &03A&00&11t&16ta&11c&00&03k&15! You feel the air heat up as the &00&06b&00&14e&00a&14m&00 &15slams into &14$N&15!&00", FALSE, ch, 0, vict, TO_NOTVICT);  
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 30 + 175000, SKILL_BIGBANG);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_MANA(ch) / 30 + 175000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
}


ACMD(do_finalflash)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!GET_SKILL(ch, SKILL_FINALFLASH) && !IS_NPC(ch)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  if (IS_NPC(ch) && AFF_FLAGGED(ch, AFF_MAJIN)) {
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 60 && !IS_NPC(ch)) {
     send_to_char("You don't have the Ki to do a Final Flash!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Final Flash who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch) / 60) {
    damage(ch, vict, 0, SKILL_FINALFLASH);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
    act("&15You thrust your arms forward, wrists together, palms open. You push &00e&14n&00&06er&00&14g&00y&00&15 into your palms as &00li&00&14gh&00&06t&00&14ni&00ng&15 crackles around you. As you feel you have enough &00e&14n&00&06er&00&14g&00y&00&15 into your hands, you yell, &11F&00&03i&16n&00&03a&11l&00 &16F&00&03l&11a&00&03s&16h&00&15! And unleash a hellacious blast at &14$N&15!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15$n thrusts their arms out at you, their palms pointing straight out at you. As you feel the &00e&14n&00&06er&00&14g&00y&00&15 well within them, they scream, &11F&00&03i&16n&00&03a&11l&00 &16F&00&03l&11a&00&03s&16h&00&15! And your body feels like its on fire as a massive &00&06b&14e&00a&14m&00 slams into you!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15$n thrusts their arms out at you, their palms pointing straight out at you. As you feel the &00e&14n&00&06er&00&14g&00y&00&15 well within them, they scream, &11F&00&03i&16n&00&03a&11l&00 &16F&00&03l&11a&00&03s&16h&00&15! And your body feels like its on fire as a massive &00&06b&14e&00a&14m&00 slams into $N!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    if (!IS_NPC(ch)) {
     SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     damage(ch, vict, GET_MAX_MANA(ch) / 30 + 165000, SKILL_FINALFLASH);
     REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 30 + 165000;
    }
    else if (IS_NPC(ch)) {
     damage(ch, vict, GET_LEVEL(ch), SKILL_KAMEHAMEHA);
     dam = GET_LEVEL(ch);
    }
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
 if (!IS_NPC(ch)) {
  GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 60;
 }
}

ACMD(do_galikgun)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_GALIKGUN)) {
    send_to_char("You don't know how to do that!\r\n", ch);
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/100) {
     send_to_char("You don't have the Ki to do a Galik Gun!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Galik Gun who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch) / 100) {
    damage(ch, vict, 0, SKILL_GALIKGUN);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/100;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
 act("&16You fly up into the air, twisting your body as you begin to charge &00e&14n&00&06er&00&14g&00y&16 into your palms. As you continue to charge, a large &00o&14r&00&06b&00&16 of &00e&14n&00&06er&00&14g&00y&16 encases you, and as you yell, &00&05G&13a&00&16l&13i&00&05k&00 &16G&15u&00n&16! You throw your hands forward, and the &00o&14r&00&06b&00&16 around you explodes towards &14$N&16 in a large &00&06b&14e&00a&14m&16, smashing into them!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&16$n flies up into the air, twisting their body as you feel the &00e&14n&00&06er&00&14g&00y&16 well up in them. Suddenly a large &13p&00&05urpl&13e&00 &00o&14r&00&06b&00&16 surrounds their form, and as you hear them scream &00&05G&13a&00&16l&13i&00&05k&00 &16G&15u&00n&16! You watch as a massive &00&06b&14e&00a&14m&16 slams into you!&00", FALSE, ch, 0, vict, TO_VICT);
 act("&16$n flies up into the air, twisting their body as you feel the &00e&14n&00&06er&00&14g&00y&16 well up in them. Suddenly a large &13p&00&05urpl&13e&00 &00o&14r&00&06b&00&16 surrounds their form, and as you hear them scream &00&05G&13a&00&16l&13i&00&05k&00 &16G&15u&00n&16! You watch as a massive &00&06b&14e&00a&14m&16 slams into &14$N&15!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 50 + 90000, SKILL_GALIKGUN);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 50 + 90000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
   WAIT_STATE(ch, PULSE_VIOLENCE * 1);

 GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/100;
}


ACMD(do_choukame)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!GET_SKILL(ch, SKILL_CHOUKAME)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60) {
     send_to_char("You don't have the Ki to do a Chou Kamehameha!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Chou Kamehameha who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 6);
                                                                 
  prob = 5;

  if (percent > prob) {
    damage(ch, vict, 0, SKILL_CHOUKAME);
   act("&15Your &14C&00&06h&16o&15u &12K&14a&00&06m&00&04e&12ha&00&04m&00&06e&14h&12a&15 turns upwards at the last moment, missing $N&15!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("$n&15's &14C&00&06h&16o&15u &12K&14a&00&06m&00&04e&12ha&00&04m&00&06e&14h&12a&15 turns upwards at the last moment, missing you!&00", FALSE, ch, 0, vict, TO_VICT);
   act("$n&15's &14C&00&06h&16o&15u &12K&14a&00&06m&00&04e&12ha&00&04m&00&06e&14h&12a&15 turns upwards at the last moment, missing $N&15!&00", FALSE, ch, 0, vict, TO_NOTVICT);
   WAIT_STATE(ch, PULSE_VIOLENCE * 1);
   GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
   return;
  } else 
 act("&15You bend your knees, bracing your feet, as you pull your hands to your side. The area is bathed in a &12b&00&04l&15u&00e &11l&00&03i&00g&00&03h&11t&15 as the ground around you begins to shudder and &00&03q&16u&15a&00&03k&16e&15. Chunks of rocks and stone begin to fly high into the &00a&14i&15r as &00e&14n&00&06er&14g&00y&15 begins to pour into the space between your hands, forming into a bright &12b&00&04l&15u&00e &16b&00&06a&16l&00&06l &15of &11k&00&03i&15. &00E&15le&11c&00&03tric&11i&15t&00y&15 starts to crackle around your form, the air shaking with the sheer force of your building attack. As you pump as much &11k&00&03i&15 as you can into your attack, you thrust your hands forward, yelling loudly, '&14C&00&06h&16o&15u &12K&14a&00&06m&00&04e&12ha&00&04m&00&06e&14h&12a&15!' The bright beam of enormous energy flies at $N&15, encompassing thier entire body as it hits!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("$n&15 bends thier knees, pulling thier hands to the side. The area is bathed in a &12b&00&04l&15u&00e &11l&00&03i&00g&00&03h&11t&15 as the ground around you begins to shudder and &00&03q&16u&15a&00&03k&16e&15. Chunks of rocks and stone begin to fly high into the &00a&14i&15r as &00e&14n&00&06er&14g&00y&15 begins to pour into the space between thier hands, forming into a bright &12b&00&04l&15u&00e &16b&00&06a&16l&00&06l &15of &11k&00&03i&15. &00E&15le&11c&00&03tric&11i&15t&00y&15 starts to crackle around thier body, the air shaking with the sheer force of the building attack. $n thrusts thier hands forward, yelling loudly, '&14C&00&06h&16o&15u &12K&14a&00&06m&00&04e&12ha&00&04m&00&06e&14h&12a&15!' The bright beam of enormous energy flies at you, encompassing your entire body as it hits!&00", FALSE, ch, 0, vict, TO_VICT);
 act("$n&15 bends thier knees, pulling thier hands to the side. The area is bathed in a &12b&00&04l&15u&00e &11l&00&03i&00g&00&03h&11t&15 as the ground around you begins to shudder and &00&03q&16u&15a&00&03k&16e&15. Chunks of rocks and stone begin to fly high into the &00a&14i&15r as &00e&14n&00&06er&14g&00y&15 begins to pour into the space between thier hands, forming into a bright &12b&00&04l&15u&00e &16b&00&06a&16l&00&06l &15of &11k&00&03i&15. &00E&15le&11c&00&03tric&11i&15t&00y&15 starts to crackle around thier body, the air shaking with the sheer force of the building attack. $n thrusts thier hands forward, yelling loudly, '&14C&00&06h&16o&15u &12K&14a&00&06m&00&04e&12ha&00&04m&00&06e&14h&12a&15!' The bright beam of enormous energy flies at $N&15, encompassing thier entire body as it hits!&00", FALSE, ch, 0, vict, TO_NOTVICT);
   if (!IS_BIODROID(ch)) {
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 30 + 205000, SKILL_CHOUKAME);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_MANA(ch) / 30 + 205000;
   }
   else if (IS_BIODROID(ch)) {
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 20 + 405000, SKILL_CHOUKAME);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_MANA(ch) / 20 + 405000;
   }
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

   GET_POS(vict) = POS_SITTING;
   WAIT_STATE(vict, PULSE_VIOLENCE);
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
}



ACMD(do_disk)
{
  struct char_data *vict;
  int percent, prob, dam, r_num = 0;
  struct obj_data *obj;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (IS_AFFECTED(ch, AFF_MFROZEN)) {
    send_to_char("You struggle against your mind freeze...\r\n", ch);
    return;
  }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!GET_SKILL(ch, SKILL_DISK) && !IS_NPC(ch)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  if (IS_NPC(ch) && AFF_FLAGGED(ch, AFF_MAJIN)) {
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60 && !IS_NPC(ch)) {
     send_to_char("You don't have the Ki to do a Destructo Disk!\r\n", ch);
     return;
   }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Destructo Disk who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/90 && !IS_NPC(ch)) {
    damage(ch, vict, 0, SKILL_DISK);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  }
 if (!IS_NPC(vict) && number(1, 20) >= 12 && percent < prob && GET_MAX_HIT(ch) >= ((GET_MAX_HIT(vict) / 100) * 80)) {
   switch(number(1, 5)) {
    case 1:
    if (PLR_FLAGGED(ch, PLR_RARM)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_RARM);
    act("&14$N&15 loses their &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2099;
    if ((r_num = real_object(2099)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_RARM);
     return;
    }
    break;
    case 2:
    if (PLR_FLAGGED(ch, PLR_LARM)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_LARM);
    act("&14$N&15 loses their &14left arm&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14left arm&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14left&15 arm in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2096;
    if ((r_num = real_object(2096)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_LARM);
     return;
    }
    break;
    case 3:
    if (PLR_FLAGGED(ch, PLR_RLEG)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_RLEG);
    act("&14$N&15 loses their &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2098;
    if ((r_num = real_object(2098)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_RLEG);
     return;
    }
    }
    break;
    case 4:
    if (PLR_FLAGGED(ch, PLR_LLEG)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_LLEG);
    act("&14$N&15 loses their &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2097;
    if ((r_num = real_object(2097)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_LLEG);
     return;
    }
    }
    break;
    case 5:
    if (PRF2_FLAGGED(ch, PRF2_HEAD) && number(1, 30) >= 24) {
    REMOVE_BIT(PRF2_FLAGS(vict), PRF2_HEAD);
    act("&14$N&15 loses their &14head&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14head&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14head&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    if (PLR_FLAGGED(vict, PLR_KILLER) && !IS_NPC(vict)) {
       sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15. This was also a &10Player &09K&00&01il&09l &11A&00&03ren&11a win!&00\r\n", GET_NAME(vict), GET_NAME(ch));
       send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
       char_from_room(vict);
       char_to_room(vict, r_death_start_room);
       REMOVE_BIT(PLR_FLAGS(vict), PLR_KILLER);
       REMOVE_BIT(PLR_FLAGS(ch), PLR_KILLER);
      char_from_room(ch);
       char_to_room(ch, r_mortal_start_room );
       look_at_room(ch, 0);
       GET_POS(vict) = POS_STANDING;
       look_at_room(vict, 0);
      }
    else if (!PLR_FLAGGED(vict, PLR_KILLER) && !PLR_FLAGGED(vict, PLR_icer) && !IS_NPC(vict)) {
      sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15.&00\r\n", GET_NAME(vict), GET_NAME(ch));
      send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
      char_from_room(vict);
      char_to_room(vict, r_death_start_room);
      SET_BIT(CLN_FLAGS(vict), CLN_Death);
      GET_DTIMER(vict) = 3600;
      GET_POS(vict) = POS_STANDING;
      look_at_room(vict, 0);
    }
    else if (PLR_FLAGGED(vict, PLR_icer) && !IS_NPC(vict)) {
      sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15. &12%s &15also collected their bounty.&00\r\n", GET_NAME(vict), GET_NAME(ch), GET_NAME(ch));
      send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
      char_from_room(vict);
      REMOVE_BIT(PLR_FLAGS(vict), PLR_icer);
      send_to_char("You collected the 1mil bounty on their head.\r\n ", ch);
      GET_GOLD(ch) += 1000000;
      char_to_room(vict, r_death_start_room);
      SET_BIT(CLN_FLAGS(vict), CLN_Death);
      GET_DTIMER(vict) = 3600;
      GET_POS(vict) = POS_STANDING;
      look_at_room(vict, 0);
     }
    if (!IS_NPC(ch) && !IS_NPC(vict)) {
      GET_HEIGHT(ch) = GET_HEIGHT(ch) + 1;
      GET_WEIGHT(vict) = GET_WEIGHT(vict) + 1;
     }
    r_num = 2095;
    if ((r_num = real_object(2095)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    *buf2 = '\0';
    sprintf(buf2, "&14%s's &01S&09e&16v&00e&16r&09e&00&01d &00&01H&09e&00a&16d&00", GET_NAME(vict));
    obj->short_description = str_dup(buf2);
    *buf2 = '\0';
    }
    }
    break;
   }
  }
  if (IS_MAJIN(vict) && PLR_FLAGGED(vict, PLR_FORELOCK) && !IS_NPC(ch)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_FORELOCK);
    act("&15You cut $N's forelock off&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15$n cuts your forelock off&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15$n cuts $N's forelock off&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    return;
    }
  if (IS_saiyan(vict) && PLR_FLAGGED(vict, PLR_STAIL) && !IS_NPC(ch) && !PRF2_FLAGGED(ch, PRF2_GO)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_STAIL);
    act("&15You cut $N's tail off&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15$n cuts your tail off&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15$n cuts $N's tail off&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    if (IS_saiyan(vict) && PRF_FLAGGED(vict, PRF_OOZARU)) {
     REMOVE_BIT(PRF_FLAGS(vict), PRF_OOZARU);
     act("&12Your rage calms down and you begin to slowly shrink as the hair on your body disolves and you pass out on the ground.&00\n\r", FALSE, ch, 0, vict, TO_VICT);
     act("&12$N's rage calms down and $N begins to slowly shrink as the hair on $N's body disolves and they pass out on the ground.&00\n\r",  FALSE, ch, 0, vict, TO_NOTVICT);
     act("&12$N's rage calms down and $N begins to slowly shrink as the hair on $N's body disolves and they pass out on the ground.&00\n\r",  FALSE, ch, 0, vict, TO_CHAR);
     GET_MAX_HIT(vict) -= 1000000;
     GET_MAX_MANA(vict) -= 1000000;
     }
    return;
    }
  else if (IS_HALF_BREED(vict) && PLR_FLAGGED(vict, PLR_STAIL) && !IS_NPC(ch) && PRF2_FLAGGED(ch, PRF2_GO)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_STAIL);
    act("&15You cut $N's tail off&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15$n cuts your tail off&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15$n cuts $N's tail off&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    if (IS_HALF_BREED(vict) && PRF_FLAGGED(vict, PRF_OOZARU)) {
     REMOVE_BIT(PRF_FLAGS(vict), PRF_OOZARU);
     act("&12Your rage calms down and you begin to slowly shrink as the hair on your body disolves and you pass out on the ground.&00\n\r", FALSE, ch, 0, 0, TO_VICT);
     act("&12$N's rage calms down and $N begins to slowly shrink as the hair on $N's body disolves and they pass out on the ground.&00\n\r",  FALSE, ch, 0, vict, TO_NOTVICT);
     act("&12$N's rage calms down and $N begins to slowly shrink as the hair on $N's body disolves and they pass out on the ground.&00\n\r",  FALSE, ch, 0, vict, TO_CHAR);
     GET_MAX_HIT(vict) -= 1000000;
     GET_MAX_MANA(vict) -= 1000000;
     }
    return;
    }
  else if (IS_BIODROID(vict) && PLR_FLAGGED(vict, PLR_TAIL) && !IS_NPC(ch)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_TAIL);
    act("&15You cut $N's tail off&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15$n cuts your tail off&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15$n cuts $N's tail off&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    return;
    }
  else if (IS_icer(vict) && PLR_FLAGGED(vict, PLR_TAIL) && !IS_NPC(ch)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_TAIL);
    act("&15You cut $N's tail off&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15$n cuts your tail off&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15$n cuts $N's tail off&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    REMOVE_BIT(PLR_FLAGS(vict), PLR_TAIL);
    return;
    }
  else if ((IS_NPC(vict) && GET_MAX_MANA(ch)) >= 500000 && (number(1, 250) >= 230)) {
    act("&15Your &11destructo &00&07disk&15 cuts &14$N&15 in half!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &11destructo &00&07disk&15 cuts &14$N&15 in half!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    act("&14$n &11destructo &00&07disk&15 cuts you&15 in half!&00", FALSE, ch, 0, vict, TO_VICT);
    GET_POS(vict) == POS_DEAD;
  }
    else
     act("&15You hold one hand above you, the palm facing the sky and crying out, '&13D&15e&00&05s&16tru&00&05c&15t&13o &00&05D&13i&15s&00k&15!'  As you concentrate, &00&03e&11n&15er&11g&00&03y&15 gathers and swirls about a single point until being forced into a flat disc that spins and hums like a tormented saw blade.  With only the slightest of gestures, you hurl the disc forward.  It flies straight at its target, slicing through everything unlucky enough to be caught in its wake, until at last it slices through $N.&00", FALSE, ch, 0, vict, TO_CHAR);
     act("&15$n&15 holds one hand in the air palm up.  With but a single word, '&13D&15e&00&05s&16tru&00&05c&15t&13o &00&05D&13i&15s&00k&15!', a flat whirling disc forms in the air inches from the palm.  The hand comes down, the disc is flung and comes straight at you.  Nothing can protect you as it slices through everything in its path, the loud hum filling your ears before it cuts right through you.&00", FALSE, ch, 0, vict, TO_VICT);
     act("&15$n&15 holds up one hand and calls out, '&13D&15e&00&05s&16tru&00&05c&15t&13o &00&05D&13i&15s&00k&15!'.  A flat disc of raw power is called into being and then cast at $N&15 with a mere movement of the arm.  It hums loudly, slicing cleanly through everything it passes.  There is no explosion or sound other than that hum as it slices right through $n.&15", FALSE, ch, 0, vict, TO_NOTVICT);
    if (!IS_NPC(ch)) {
     SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     damage(ch, vict, GET_MAX_MANA(ch) / 30 + 200000, SKILL_DISK);
     REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 30 + 200000;
    }
    else if (IS_NPC(ch)) {
     damage(ch, vict, GET_LEVEL(ch), SKILL_DISK);
     dam = GET_LEVEL(ch);
    }
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
 if (!IS_NPC(ch)) {
  GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 60;
 }
}

ACMD(do_masenko)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!GET_SKILL(ch, SKILL_MASENKO) && !IS_NPC(ch)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60 && !IS_NPC(ch)) {
     send_to_char("You don't have the Ki to do a Masenko!\r\n", ch);
     return; 
  }
  if (IS_NPC(ch) && AFF_FLAGGED(ch, AFF_MAJIN)) {
   return;
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Masenko who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999 && !IS_NPC(ch)) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/60 && !IS_NPC(ch)) {
    damage(ch, vict, 0, SKILL_MASENKO);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
    act("&16You raise your hands to your forehead, placing them one behind the other, palms wide open. You start pushing &00&03k&11i&16 into your hands, the entire area glowing a light teal color. As a large ball of &00e&14n&00&06er&00&14g&00y&16 forms in your hands, you aim them at $N as you scream, '&14M&00&06a&16s&14e&00&06n&16k&14o&16!' A thick &14b&00&06e&00a&14m&16 fires from the ball of &00e&14n&00&06er&00&14g&00y&16, slamming into $N!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&16$n&16 raise $s&16 hands to $s&16 forehead, placing them one behind the other, palms wide open. $n start pushing &00&03k&11i&16 into $e&16 hands, the entire area glowing a light &14t&00&06e&00a&14l&16 color. As a large ball of &00e&14n&00&06er&00&14g&00y&16 forms in $n&16 hands, $e&16 aim them at $N as $e&16 screams, '&14M&00&06a&16s&14e&00&06n&16k&14o&16!' A thick &14b&00&06e&00a&14m&16 fires from the ball of &00e&14n&00&06er&00&14g&00y&16, slamming into you!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&16$n&16 raise $s&16 hands to $s&16 forehead, placing them one behind the other, palms wide open. $n&16 start pushing &00&03k&11i&16 into $e&16 hands, the entire area glowing a light teal color. As a large ball of &00e&14n&00&06er&00&14g&00y&16 forms in $n&16 hands, $e&16 aim them at $N as $e&16 screams, '&14M&00&06a&16s&14e&00&06n&16k&14o&16!' A thick &14b&00&06e&00a&14m&16 fires from the ball of &00e&14n&00&06er&00&14g&00y&16, slamming into $N!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    if (!IS_NPC(ch)) {
     SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     damage(ch, vict, GET_MAX_MANA(ch) / 30 + 185000, SKILL_MASENKO);
     REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 30 + 185000;
    }
    else if (IS_NPC(ch)) {
     damage(ch, vict, GET_LEVEL(ch), SKILL_MASENKO);
     dam = GET_LEVEL(ch);
    }
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

   GET_POS(vict) = POS_SITTING;
   WAIT_STATE(vict, PULSE_VIOLENCE);
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);
 if (!IS_NPC(ch)) {
  GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 60;
 }
}

ACMD(do_renzo)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!GET_SKILL(ch, SKILL_RENZO)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60) {
     send_to_char("You don't have the Ki to do a Renzokou Energy Dan!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Renzokou Energy Dan who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/60) {
    damage(ch, vict, 0, SKILL_RENZO);
    act("$N&15 defly navigates your maze of &00e&14n&00&06er&14g&00y&15 shots, causing you to growl in frustration.&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You swiftly dodge out of the path of the incoming shots, but take a moment to admire the sight of &00&03g&11o&00&03ld&11e&00&03n&15, shooting stars.&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15$N's quick motions allow them the room to slip past the seemingly impenetrable &00R&15e&14n&00&06z&00&03o&11k&15u &00E&14n&00&06er&14g&00y &00&06D&00&03a&11n&15!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
    act("&15You extend your hand towards $N&15, and smirk slightly as a ball of &00&03g&11o&00&03ld&11e&00&03n&15 ki takes shape in your vertical, open hand.  Briefly bringing your charge back to your side, you thrust your palm forward, and burst the small &00o&14r&00&06b&15 into hundreds of small, lightning fast ki blasts.  The &00R&15e&14n&00&06z&00&03o&11k&15u &00E&14n&00&06er&14g&00y &00&06D&00&03a&11n&15 rockets forward, and explode all over $N&15's smoking body!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15$n&15 extends their hand towards you, and smirks slightly as a ball of golden ki takes shape in their vertical, open hand.  Briefly bringing their charge back to their side, User thrusts their palm forward, and bursts the small orb into hundreds of small, lightning fast ki blasts.  The &00R&15e&14n&00&06z&00&03o&11k&15u &00E&14n&00&06er&14g&00y &00&06D&00&03a&11n&15 rockets forward, and explode all over your smoking body!", FALSE, ch, 0, vict, TO_VICT);
    act("&15$n&15 extends their hand towards opponent, and smirks slightly as a ball of golden ki takes shape in their vertical, open hand.  Briefly bringing their charge back to their side, $n thrusts their palm forward, and bursts the small orb into hundreds of small, lightning fast ki blasts.  The &00R&15e&14n&00&06z&00&03o&11k&15u &00E&14n&00&06er&14g&00y &00&06D&00&03a&11n&15 rockets forward, and explode all over $N&15's smoking body!", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 30 + 168000, SKILL_RENZO);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_MANA(ch) / 30 + 168000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  WAIT_STATE(ch, PULSE_VIOLENCE * 1);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
}
ACMD(do_makanko)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (IS_AFFECTED(ch, AFF_MFROZEN)) {
    send_to_char("You struggle against your mind freeze...\r\n", ch);
    return;
  }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM) && !IS_NPC(ch)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!GET_SKILL(ch, SKILL_MAKANKO) && !IS_NPC(ch)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  if (IS_NPC(ch) && AFF_FLAGGED(ch, AFF_MAJIN)) {
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60 && !IS_NPC(ch)) {
     send_to_char("You don't have the Ki to do a Makankosappo!\r\n", ch);
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Makankosappo who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);  
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/60 && !IS_NPC(ch)) {
    damage(ch, vict, 0, SKILL_MAKANKO);
    act("$N&15 ducks under your &11S&00&03p&16e&15c&00i&03a&11l &00&01B&09e&15a&00m &16C&15a&00nn&15o&16n&15!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("You duck under $n&15's &11S&00&03p&16e&15c&00i&03a&11l &00&01B&09e&15a&00m &16C&15a&00nn&15o&16n&15!&00", FALSE, ch, 0, vict, TO_VICT);
    act("$N&15 ducks under $n&15's &11S&00&03p&16e&15c&00i&03a&11l &00&01B&09e&15a&00m &16C&15a&00nn&15o&16n&15!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
    act("&15You press your fingers to your forehead, charging &00e&14n&00&06er&14g&00y&15 into the tips of them. Viens within your body begin to pulsate with &09p&00&01o&16w&00&01e&09r&15 as the area around your fingers begin to &14g&00&06l&00&03o&11w&15 and &11c&00&03r&16a&15c&00k&11l&00&03e&15 with &00e&14n&00&06er&14g&00y&15. The ground beneath you begins to break up as thrust your fingers forward, screaming, '&11S&00&03p&16e&15c&00i&03a&11l &00&01B&09e&15a&00m &16C&15a&00nn&15o&16n&15!' A straight &14b&00&06e&15a&00m&15 of energy flies from one finger while a beam of spinning energy swirls around the first. The beams of energy slams into $N&15, cutting into thier skin!", TRUE, ch, 0, vict, TO_CHAR);
    act("$n&15 press thier fingers to thier forehead, charging &00e&14n&00&06er&14g&00y&15 into the tips of them. The area around the fingers begin to &14g&00&06l&00&03o&11w&15 and &11c&00&03r&16a&15c&00k&11l&00&03e&15 with &00e&14n&00&06er&14g&00y&15. The ground beneath you begins to break up as thrust thier fingers forward, screaming, '&11S&00&03p&16e&15c&00i&03a&11l &00&01B&09e&15a&00m &16C&15a&00nn&15o&16n&15!' A straight &14b&00&06e&15a&00m&15 of energy flies from one finger while a beam of spinning energy swirls around the first. The beams of energy slams into $N&15, cutting into your skin!&00", TRUE, ch, 0, vict, TO_VICT);
    act("$n&15 press thier fingers to thier forehead, charging &00e&14n&00&06er&14g&00y&15 into the tips of them. The area around the fingers begin to &14g&00&06l&00&03o&11w&15 and &11c&00&03r&16a&15c&00k&11l&00&03e&15 with &00e&14n&00&06er&14g&00y&15. The ground beneath you begins to break up as thrust thier fingers forward, screaming, '&11S&00&03p&16e&15c&00i&03a&11l &00&01B&09e&15a&00m &16C&15a&00nn&15o&16n&15!' A straight &14b&00&06e&15a&00m&15 of energy flies from one finger while a beam of spinning energy swirls around the first. The beams of energy slams into $N&15, cutting into thier skin!&00", TRUE, ch, 0, vict, TO_NOTVICT);
    if (IS_NPC(ch)) {
     SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     damage(ch, vict, GET_LEVEL(ch), SKILL_MAKANKO);
     REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_LEVEL(ch);
    }
    if (!IS_NPC(ch)) {
     damage(ch, vict, GET_MAX_MANA(ch) / 30 + 198000, SKILL_MAKANKO);
     dam = GET_MAX_MANA(ch) / 30 + 198000;
    }
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
 if (!IS_NPC(ch)) {
  GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
 }
}

ACMD(do_beam)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}

  if (!GET_SKILL(ch, SKILL_BEAM)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/ 200) {
     send_to_char("You don't have the Ki to fire a beam!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Hit who with a beam?\r\n", ch);
      return;
    }
  }
 if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/200) {
    damage(ch, vict, 0, SKILL_BEAM);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/200;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  }
   if (IS_ANDROID(vict) && (GET_SKILL(vict, SKILL_ABSORB) || IS_BIODROID(vict)) && GET_SKILL(vict, SKILL_ABSORB)) {
       switch (number(1, 10)) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
         break;
        case 8:
         act("&15You &11absorbs &14$n's &15attack!&00", TRUE, ch, 0, vict, TO_VICT);
         act("&15$N &11absorbs &14$n's &15attack!&00", TRUE, ch, 0, vict, TO_NOTVICT);
         act("&15$N &11absorbs &14your &15attack!&00", TRUE, ch, 0, vict, TO_CHAR);
         GET_MANA(vict) = GET_MANA(vict) + dam;
         return;
         break;
        case 9:
        case 10:
         break;
       }
      }
  else
   act("&15$n charges some &00e&14n&00&06er&00&14g&00y&15 into their hands and fires a &00&06b&00&14e&00a&14m&15 of &00&03k&11i&15 at $N!", FALSE, ch, 0, vict, TO_NOTVICT);
   act("&15You charge some &00e&14n&00&06er&00&14g&00y&15 into your hands and fire a &00&06b&00&14e&00a&14m&15 of &00&03k&11i&15 at $N!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&15$n charges some &00e&14n&00&06er&00&14g&00y&15 into their hands and fires a &00&06b&00&14e&00a&14m&15 of &00&03k&11i&15 that hits you in the chest!&00", FALSE, ch, 0, vict, TO_VICT);
       SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
       damage(ch, vict, GET_MAX_MANA(ch) / 70 + 35000, SKILL_BEAM);
       REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
       dam = GET_MAX_MANA(ch) / 70 + 35000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
 WAIT_STATE(ch, PULSE_VIOLENCE * 1);
GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/200;
}


ACMD(do_kishot)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!GET_SKILL(ch, SKILL_KISHOT)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/200) {
     send_to_char("You don't have the Ki to do a Kishot!\r\n", ch);
     return;  
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Kishot who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60); 
  prob = GET_REINC(ch);
  
  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/200) {
    damage(ch, vict, 0, SKILL_KISHOT);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/200;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
    act("&16$n charges some &00&03k&11i&16 into their hand and fires a &00&03k&11i&14s&00&06h&14o&00t&16 at $N!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    act("&16You charge some &00&03k&11i&16 into your hand and fire a &00&03k&11i&14s&00&06h&14o&00t&16 at $N!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&16$n charges some &00&03k&11i&16 into their hand and fires it at you!&00", FALSE, ch, 0, vict, TO_VICT);
    dam = GET_MAX_MANA(ch) / 70 + 25000;
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 70 + 25000, SKILL_KISHOT);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    if (IS_ANDROID(vict) && (GET_SKILL(vict, SKILL_ABSORB) || IS_BIODROID(vict)) && GET_SKILL(vict, SKILL_ABSORB)) {
       switch (number(1, 10)) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
         break;
        case 8:
         act("&15You &11absorbs &14$n's &15attack!&00", TRUE, ch, 0, vict, TO_VICT);
         act("&15$N &11absorbs &14$n's &15attack!&00", TRUE, ch, 0, vict, TO_NOTVICT);
         act("&15$N &11absorbs &14your &15attack!&00", TRUE, ch, 0, vict, TO_CHAR);
         GET_MANA(vict) = GET_MANA(vict) + dam;
         return;
         break;
        case 9:
        case 10:
         break;
       }
      }
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/200;
}

ACMD(do_honoo)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_HONOO)) {
    send_to_char("You don't know how to do that!\r\n", ch);
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60) {
     send_to_char("You don't have the Ki to do a Honoo!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Honoo who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch) / 60) {
    damage(ch, vict, 0, SKILL_HONOO);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
 act("&16You smirk at $N&16, and start to push ki into your stomach. You lean your head back as you push the ki upwards, then open your mouth and unleash a&00 &09m&00&01a&15s&16s&00&15i&00&01v&09e&16 blast of &09f&00&03i&00&11r&00e&16 at $N!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&16$n smirks at you as they start to push ki into their stomach. You see curls of &09f&00&03i&00&11r&15e&16 lick at the edges of their lips as they open their mouth fully and fire a hellacious blast of &09f&00&03i&00&11r&15e&16 at you!", FALSE, ch, 0, vict, TO_VICT);
 act("&16$n smirks at $N&16 as they start to push ki into their stomach. You see curls of &09f&00&03i&00&11r&15e&16 lick at the edges of their lips as they open their mouth fully and fire a hellacious blast of &09f&00&03i&00&11r&15e&16 at $N!", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 30 + 225000, SKILL_HONOO);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 30 + 225000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
    if (number(1, 25) == 25) {
  if (!IS_NPC(vict)) {
   SET_BIT(PRF_FLAGS(ch), PRF_AUTOEXIT);
   act("&14$N &15has been &09burned&15 severely by the attack!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&15You have been &09burned&15 severely by the attack!", FALSE, ch, 0, vict, TO_VICT);
   act("&14$N &15has been &09burned&15 severely by the attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    GET_MANA(vict) -= GET_MANA(vict);
   }
   else {
    act("&14$N &15has been &09burned&15 severely by the attack!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You have been &09burned&15 severely by the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N &15has been &09burned&15 severely by the attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    GET_HIT(vict) -= GET_HIT(vict) / 4;
   }
  }
    GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
}

ACMD(do_eraser)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!GET_SKILL(ch, SKILL_ERASER)) {
    send_to_char("You don't know how to do that!\r\n", ch);
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60) {
     send_to_char("You don't have the Ki to do an eraser cannon!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Eraser cannon who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/60) {
    damage(ch, vict, 0, SKILL_ERASER);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
 act("&00&05You push ki deep down into your body, as you cross your arms over you forehead. As the &00e&14n&00&06er&00&14g&00y&05 grows as much as possible, you yell, &00&02E&00&10r&15a&16s&00&02e&10r&00 &16C&15a&00nn&15o&16n&00&05! And fire a &09m&00&01a&15s&16s&00&15i&00&01v&09e &00&06b&14e&00a&14m&00&05 of ki from your mouth at $N!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&00&05$n crosses their arms over their forehead, and as you feel the &00e&14n&00&06er&00&14g&00y&05 build up within them, they yell out &00&02E&00&10r&15a&16s&00&02e&10r&00 &16C&15a&00nn&15o&16n&00&05! And unleash a &09m&00&01a&15s&16s&00&15i&00&01v&09e &00&06b&14e&00a&14m&00&05 at you!&00", FALSE, ch, 0, vict, TO_VICT);
 act("&00&05$n crosses their arms over their forehead, and as you feel the &00e&14n&00&06er&00&14g&00y&05 build up within them, they yell out &00&02E&00&10r&15a&16s&00&02e&10r&00 &16C&15a&00nn&15o&16n&00&05! And unleash a &09m&00&01a&15s&16s&00&15i&00&01v&09e &00&06b&14e&00a&14m&00&05 at $N!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 30 + 178000, SKILL_ERASER);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 30 + 178000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
     
 WAIT_STATE(ch, PULSE_VIOLENCE * 1);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
}


ACMD(do_crusher)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }
  if (!GET_SKILL(ch, SKILL_CRUSHER)) {
    send_to_char("You don't know how to do that!\r\n", ch);
   return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do a crusher ball!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
   if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Crusher Ball who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30) {
    damage(ch, vict, 0, SKILL_CRUSHER);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
 act("&00&06You fly above the ground, your left hand held up as you start to charge ki into your palm. As you feel the &00e&14n&00&06er&00&14g&00y&06 rise, a small glowing &00o&14r&00&06b&00&06 forms there. As you force as much ki into it as you can, you give it a small bump upwards with your palm, than smash it towards $N&00&06 with your right hand! The ball impacts $N&00&06 in the chest!&00", FALSE, ch, 0, vict, TO_CHAR);
 act("&00&06$n flies above the ground, raising their left hand towards the sky. You feel them start to fill with ki, and a small glowing &00o&14r&00&06b&00&06 of &00e&14n&00&06er&00&14g&00y&06 forms in their palm. As they finish forming the &00o&14r&00&06b&00&06, they push it up then slap it towards you, which hits your body right in the chest!&00", FALSE, ch, 0, vict, TO_VICT);
 act("&00&06$n flies above the ground, raising their left hand towards the sky. You feel them start to fill with ki, and a small glowing &00o&14r&00&06b&00&06 of &00e&14n&00&06er&00&14g&00y&06 forms in their palm. As they finish forming the &00o&14r&00&06b&00&06, they push it up then slap it towards $N&00&06, which hits them in the chest!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 20 + 310000, SKILL_CRUSHER);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 20 + 310000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
     GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}


ACMD(do_havoc)
{
  struct char_data *vict, *next_vict;
  int dam, num = 0;
  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!GET_SKILL(ch, SKILL_HAVOC)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/20) {
     send_to_char("You don't have the Ki to do a Havoc Blast!\r\n", ch);
     return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  else {                      /* no argument. clean out the room */
    act("&15You crouch down, your hands down by your side as you start to charge &00e&14n&00&06er&00&14g&00y&15 into your body. You continue to pump the ki into your body as your muscles start to bulge, expand, and ache from all the ki stored in your form. As you cant take anymore pressure, you throw your arms out as you scream, &10H&16a&15v&00o&02c&00 B&15l&16a&15s&00t&15! The entire area explodes for what seems to be like miles.&00", FALSE, ch, 0, 0, TO_CHAR);
    act("&15$n crouches down, their hands held tightly by their waist as the earth beneath them starts to quiver and shake. As the land beneath your feet starts to fly up past your head, you hear the words, &10H&16a&15v&00o&02c&00 B&15l&16a&15s&00t&15! You feel your body burn with intense &00e&14n&00&06er&00&14g&00y&15 as you fly back for hundreds of yards!&00", FALSE, ch, 0, 0, TO_ROOM);
    if (!IS_NPC(ch)) {
     SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    }
    for (vict = world[ch->in_room].people; vict; vict = next_vict) {
      next_vict = vict->next_in_room;
      if (num >= 20) {
        continue;
      }
      if (vict == ch) {
        continue;
      }
      if (GET_MAX_HIT(vict) <= 4999999 && !IS_NPC(vict)) {
        continue;
      }
      if (CLN_FLAGGED(vict, CLN_ADEF)) {
        continue;
      }
      damage(ch, vict, GET_MAX_MANA(ch) / 20 + 365000, SKILL_HAVOC);
      num++;

      GET_POS(vict) = POS_SITTING;
      WAIT_STATE(vict, PULSE_VIOLENCE);
  }
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);
  if (!IS_NPC(ch)) {
   REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
  }
  dam = GET_MAX_MANA(ch) / 20 + 365000;
  if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
   sprintf(buf, "&15You hit everything in the room! &16[&12%d&16]&00", dam);
   sprintf(buf2, "&15You are hit by the attack!&16[&09%d&16]&00", dam);
   act(buf, FALSE, ch, NULL, vict, TO_CHAR);
   act(buf2, FALSE, ch, NULL, vict, TO_VICT);
GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/20;
 }   
}

ACMD(do_bakuhatsuha)
{
  struct char_data *vict, *next_v;
  int dam, num = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!GET_SKILL(ch, SKILL_BAKU) && !IS_NPC(ch)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
}
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60 && !IS_NPC(ch)) {
     send_to_char("You don't have the Ki to do a Bakuhatsuha!\r\n", ch);
     return;
  }
  if (IS_NPC(ch) && AFF_FLAGGED(ch, AFF_MAJIN)) {
   return;
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  else {                      /* no argument. clean out the room */
   if (!IS_NPC(ch)) {
     SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    }
    act("&00&01You stand there, charging &00e&14n&00&06er&14g&00y&01 into your form as you scan the horizon. And as you feel you &00e&14n&00&06er&14g&00y&01 well up, you thrust two fingers towards the sky as you yell, &11B&00&03a&16k&00&01u&09h&15a&09t&00&01s&16u&00&03h&11a&00&01! The entire area within one square mile explodes in a blast of pure &00e&14n&00&06er&00&14g&00y&01!", FALSE, ch, 0, vict, TO_CHAR);
    act("&00&01$n stands there, looking around theirself. As you start to feel the &00e&14n&00&06er&14g&00y&01 within them well up, they yell, &11B&00&03a&16k&00&01u&09h&15a&09t&00&01s&16u&00&03h&11a&00&01! Everywhere around you explodes as you feel &00e&14n&00&06er&00&14g&00y&01 rack your body with pain!&00", FALSE, ch, 0, vict, TO_ROOM);
    for (vict = world[ch->in_room].people; vict; vict = next_v) {
      next_v = vict->next_in_room;
    if (vict == ch) {
        continue;
      }
    if (num >= 20) {
        continue;
      }
    if (GET_MAX_HIT(vict) <= 4999999 && !IS_NPC(vict)) {
        continue;
      }
    if (CLN_FLAGGED(vict, CLN_ADEF) && !IS_NPC(ch)) {
      continue;
     }
    if (IS_NPC(ch)) {
     damage(ch, vict, GET_LEVEL(ch), SKILL_BAKU);
    }
    else if (!IS_NPC(ch)) {
     damage(ch, vict, GET_MAX_MANA(ch) / 30 + 187000, SKILL_BAKU);
     num += 1;
     }
    
    WAIT_STATE(ch, PULSE_VIOLENCE * 2);
  }
    if (!IS_NPC(ch)) {
    dam = GET_MAX_MANA(ch) / 30 + 187000;
    }
    else if (IS_NPC(ch)) {
    dam = GET_LEVEL(ch);
    }
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&15You hit everything in the room! &16[&12%d&16]&00", dam);
    sprintf(buf2, "&15You are hit by the attack!&16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
 if (!IS_NPC(ch)) {
  GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
  }
 }
 if (!IS_NPC(ch)) {
   REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
  }
}


ACMD(do_kikoho)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_RARM) && PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You only have one arm!\r\n", ch);
     return;
}
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/100) {
     send_to_char("You don't have the Ki to do a Kikoho!\r\n", ch);
     return;
     }

   
  if (!GET_SKILL(ch, SKILL_KIKOHO)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Kikoho who?\r\n", ch);
      return;
    }
  }
  if (CLN_FLAGGED(ch, CLN_ADEF)) {
    send_to_char("You are too busy defending!\r\n", ch);
    return;
   }
  if (CLN_FLAGGED(vict, CLN_ADEF)) {
     send_to_char("It is impossible to hit them while they are using all out defense!\r\n", ch);
     return;
    }
  if (GET_MANA(vict) >= GET_MAX_MANA(vict) / 75 && PLR_FLAGGED(vict, PLR_DEFLECT)) {
     switch (number(1, 3)) {
     case 1:
     case 2:
      break;
     case 3:
      act("&15You &11deflect &15$n's attack!\r\n", FALSE, ch, 0, vict, TO_VICT);
      act("&14$N &11deflects &15your attack!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N &11deflects &15$n's attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      GET_MANA(ch) -= GET_MAX_MANA(ch) / 200;
      GET_MANA(vict) -= GET_MAX_MANA(vict) / 75;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
      break;
    }
   }
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/100) {
    damage(ch, vict, 0, SKILL_KIKOHO);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/100;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
    act("&15$n starts charging &00e&14n&00&06er&00&14g&00y&15 into $s hands as $e forms a circle with their $s hands. &00El&14ec&00&06tri&14ci&00ty&15 starts crackling around $'s forearms as $n yells out, 'Kikoho!' A thick &00&06b&00&14e&00a&14m&15 of &00&03k&11i&15 flies down at $N&15, slamming into them!", FALSE, ch, 0, vict, TO_NOTVICT);
    act("&15You start charging &00e&14n&00&06er&00&14g&00y&15 into your hands as you form a circle with your hands. &00El&14ec&00&06tri&14ci&00ty&15 starts crackling around your forearms as you yell out, '&13K&00&05i&16k&15o&00h&00&05o&15!' A thick &00&06b&00&14e&00a&14m&15 of &00&03k&11i&15 flies down at $N&15, slamming into them!&00 ", FALSE, ch, 0, vict, TO_CHAR);
    act("&15$n starts charging &00e&14n&00&06er&00&14g&00y&15 into $s hands as $e forms a circle with $s hands. &00El&14ec&00&06tri&14ci&00ty&15 starts crackling around $n's forearms as $n yells out, 'Kikoho!' A thick &00&06b&00&14e&00a&14m&15 of &00&03k&11i&15 flies down at you, slamming into your chest!&00", FALSE, ch, 0, vict, TO_VICT);
     SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     damage(ch, vict, GET_MAX_MANA(ch) / 50 + 60000, SKILL_KIKOHO);
     REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 50 + 60000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  WAIT_STATE(ch, PULSE_VIOLENCE * 1);

GET_HIT(ch) = GET_HIT(ch) - GET_MAX_MANA(ch)/100;
}

ACMD(do_dball2)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (IS_AFFECTED(ch, AFF_MFROZEN)) {
    send_to_char("You struggle against your mind freeze...\r\n", ch);
    return;
  }
  if (!IS_NPC(ch)) {
     send_to_char("Only mobs can use this attack!\r\n", ch);
     return;
    }
  
  one_argument(argument, arg);

 if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Hit who with a Big Bang Kamehameha?\r\n", ch);
      return;
    }
  }
 if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 5);
  prob = 4;

  if (percent > prob && GET_MANA(ch) >= 1) {
    damage(ch, vict, 0, SKILL_DBALL2);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    return;
  }
else
   act("&16You raise your hand above your hand, your fingers curled except for your pointer and middle finger, which are raised straight up. You begin to charge &00e&14n&00&06er&14g&00y&16 into the tips of your fingers, forcing it to form into a b&00&06a&16l&00&06l&16 above you. You keep pumping as much &00&03k&11i&16 as you can into it, until it reaches massive proportions.  With a yell, '&09D&16e&00&01a&09t&15h&00&01B&16a&09l&16l!' you thrust the gigantic &00o&14r&00&06b&16 downwards towards $N&16, smashing them into the ground!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&16$n raises thier hand above thier hand, fingers curled except for the pointer and middle finger, which are raised straight up. You begin to feel the charge of &00e&14n&00&06er&14g&00y&16 pour into thier body, watching it as they form into a b&00&06a&16l&00&06l&16 above them. $n&16 keeps pumping as much &00&03k&11i&16 as they can into it, until it reaches massive proportions.  With a yell, '&09D&16e&00&01a&09t&15h&00&01B&16a&09l&16l!' With a thrust downwards, the gigantic &00o&14r&00&06b&16 flies downwards towards you, smashing your body into the ground!&00", FALSE, ch, 0, vict, TO_VICT);
   act("&16$n raises thier hand above thier hand, fingers curled except for the pointer and middle finger, which are raised straight up. You begin to feel the charge of &00e&14n&00&06er&14g&00y&16 pour into thier body, watching it as they form into a b&00&06a&16l&00&06l&16 above them. $n&16 keeps pumping as much &00&03k&11i&16 as they can into it, until it reaches massive proportions.  With a yell, '&09D&16e&00&01a&09t&15h&00&01B&16a&09l&16l!' With a thrust downwards, the gigantic &00o&14r&00&06b&16 flies downwards towards $N&16, smashing them into the ground!&00", FALSE, ch, 0, vict, TO_NOTVICT);
       SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
       damage(ch, vict, GET_MAX_HIT(ch) / 100, SKILL_DBALL2);
       REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
       dam = GET_MAX_HIT(ch) / 100;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
  
       GET_POS(vict) = POS_SITTING;
    WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE);
}


ACMD(do_dball)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!GET_SKILL(ch, SKILL_DBALL)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
   }
    

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do a Death Ball!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Death Ball who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30) {
    damage(ch, vict, 0, SKILL_DBALL);
    act("$N&16 quickly zanzokens away from the massive &09D&16e&00&01a&09t&15h&00&01B&16a&09l&16l!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("$N&16 quickly zanzokens away from the massive &09D&16e&00&01a&09t&15h&00&01B&16a&09l&16l!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    act("&16You quickly zanzoken away from the massive &09D&16e&00&01a&09t&15h&00&01B&16a&09l&16l!&00", FALSE, ch, 0, vict, TO_VICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } 
 else 
   act("&16You raise your hand above your hand, your fingers curled except for your pointer and middle finger, which are raised straight up. You begin to charge &00e&14n&00&06er&14g&00y&16 into the tips of your fingers, forcing it to form into a b&00&06a&16l&00&06l&16 above you. You keep pumping as much &00&03k&11i&16 as you can into it, until it reaches massive proportions.  With a yell, '&09D&16e&00&01a&09t&15h&00&01B&16a&09l&16l!' you thrust the gigantic &00o&14r&00&06b&16 downwards towards $N&16, smashing them into the ground!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&16$n raises thier hand above thier hand, fingers curled except for the pointer and middle finger, which are raised straight up. You begin to feel the charge of &00e&14n&00&06er&14g&00y&16 pour into thier body, watching it as they form into a b&00&06a&16l&00&06l&16 above them. $n&16 keeps pumping as much &00&03k&11i&16 as they can into it, until it reaches massive proportions.  With a yell, '&09D&16e&00&01a&09t&15h&00&01B&16a&09l&16l!' With a thrust downwards, the gigantic &00o&14r&00&06b&16 flies downwards towards $N&16, smashing them into the ground!&00", FALSE,  ch, 0, vict, TO_NOTVICT);
   act("&16$n raises thier hand above thier hand, fingers curled except for the pointer and middle finger, which are raised straight up. You begin to feel the charge of &00e&14n&00&06er&14g&00y&16 pour into thier body, watching it as they form into a b&00&06a&16l&00&06l&16 above them. $n&16 keeps pumping as much &00&03k&11i&16 as they can into it, until it reaches massive proportions.  With a yell, '&09D&16e&00&01a&09t&15h&00&01B&16a&09l&16l!' With a thrust downwards, the gigantic &00o&14r&00&06b&16 flies downwards towards you, smashing your body into the ground!&00", FALSE,  ch, 0, vict, TO_VICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_MANA(ch) / 20 + 380000, SKILL_DBALL);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_MANA(ch) / 20 + 380000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  if (!IS_NPC(vict)) {
   SET_BIT(PRF_FLAGS(ch), PRF_AUTOEXIT);
   act("&14$N &15is &16crushed&15 under the massive weight!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&15You are &16crushed&15 under the massive weight!", FALSE, ch, 0, vict, TO_VICT);
   act("&14$N &15is &16crushed&15 under the massive weight!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    GET_MANA(vict) -= GET_MANA(vict);
   }
   else {
    act("&14$N &15is &16crushed&15 under the massive weight!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You are &16crushed&15 under the massive weight!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N &15is &16crushed&15 under the massive weight!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    GET_HIT(vict) -= GET_HIT(vict) / 4;
   }


  GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);  
 WAIT_STATE(ch, PULSE_VIOLENCE * 2); 
     
    
GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}
ACMD(do_cosmic)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch,AFF_CHARGEL)){
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }

  if (!GET_SKILL(ch, SKILL_COSMIC)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do Cosmic Halo!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
  } else {
      send_to_char("Hit who with Cosmic Halo?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 5);
  prob = 4;
  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30) {
    damage(ch, vict, 0, SKILL_HYDRO);
    act("&16$N&16 throws a large &00&03k&11i&14s&00&06h&15o&00t&16 at your &00&04C&12o&14s&00&06m&15i&00c &03H&11a&15l&00o&16, deflecting the attack!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&16You throw a large &00&03k&11i&14s&00&06h&15o&00t&16 at $N&16's &00&04C&12o&14s&00&06m&15i&00c &03H&11a&15l&00o&16, deflecting $s&16 attack!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&16$N&16 throws a large &00&03k&11i&14s&00&06h&15o&00t&16 at $n&16's &00&04C&12o&14s&00&06m&15i&00c &03H&11a&15l&00o&16, deflecting the attack!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/70;
    return;
  }
 else
    act("&16You give your &00w&15i&14n&00g&15s&16 a hard flap, as your outstretched hand begins to &00&06g&14l&11o&00&03w&16 a soft &00&03g&11o&15l&00d&03e&11n&16 hue. As you raise your hand above your head, the &15e&11n&00&03er&11g&15y&16 travels up your palm into your finger, which &00c&15r&11a&00&03ck&11l&15e&00s&16 violently with &00&03k&11i&16. You slowly draw a circle over and over, the &00&03k&11i&16 building up in the &00s&15k&14y&16. As you finish, you form a &00&06g&14l&11o&00&03wi&11n&00&06g &00&03h&11a&15l&00o&16 in the &00a&15i&14r&16, and then throw it downwards at $N&16, the &00&04C&12o&14s&00&06m&15i&00c &03H&11a&15l&00o&16 wrapping around $S&16 body and squeezing them tight!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&16$N&16 gives a hard flap of $s&16 &00w&15i&14n&00g&15s&16, $s outstretched hand &00&06g&14l&11o&00&03w&16 a soft &00&03g&11o&15l&00d&03e&11n&16 hue. As $e&16 raises $s&16 hand over $s&16 head, the &00&06g&14l&11o&00&03w&16 travels up $s&16 into the palm into $s&16 fingertip. $N&16 slowly draws a circle in the &00a&15i&14r&16, over and over, forming a large circle of &00&03k&11i&16. With a downward thrust of $s&16 hand, the &00&04C&12o&14s&00&06m&15i&00c &03H&11a&15l&00o&16 flies down and wraps around your body, squeezing you tightly!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&16$N&16 gives a hard flap of $s&16 &00w&15i&14n&00g&15s&16, $s outstretched hand &00&06g&14l&11o&00&03w&16 a soft &00&03g&11o&15l&00d&03e&11n&16 hue. As $e&16 raises $s&16 hand over $s&16 head, the &00&06g&14l&11o&00&03w&16 travels up $s&16 into the palm into $s&16 fingertip. $N&16 slowly draws a circle in the &00a&15i&14r&16, over and over, forming a large circle of &00&03k&11i&16. With a downward thrust of $s&16 hand, the &00&04C&12o&14s&00&06m&15i&00c &03H&11a&15l&00o&16 flies down and wraps around $N&16's body, squeezing $S&16 tightly!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      damage(ch, vict, GET_MAX_MANA(ch) / 20 + 335000, SKILL_COSMIC);
      REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      dam = GET_MAX_MANA(ch) / 20 + 335000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }

    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);

 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}
ACMD(do_heaven)
{
  struct char_data *vict;
  int percent, prob, dam, r_num = 0;
  struct obj_data *obj;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch,AFF_CHARGEL)){
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_RARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }

  if (!GET_SKILL(ch, SKILL_HEAVEN)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do Heaven's rain!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
  } else {
      send_to_char("Hit who with Heaven's Rain?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);
  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30) {
    damage(ch, vict, 0, SKILL_HEEL);
    act("$N&16 throws up a quick barrier, narrowly dodging your &11H&00&03e&15a&00v&15e&00&03n&11s &00R&15a&11i&00&03n&16!", FALSE, ch, 0, vict, TO_CHAR);
    act("&16You throw your arms up, conjuring a barrier, managing to escape from &11H&00&03e&15a&00v&15e&00&03n&11s &00R&15a&11i&00&03n&16!", FALSE, ch, 0, vict, TO_VICT);
    act("$N&16 throws up a quick barrier, narrowly dodging $n&16's &11H&00&03e&15a&00v&15e&00&03n&11s &00R&15a&11i&00&03n&16!", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
    return;
  }
 if (!IS_NPC(vict) && number(1, 40) >= 30 && percent < prob && GET_MAX_HIT(ch) >= ((GET_MAX_HIT(vict) / 100) * 80)) {
   switch(number(1, 5)) {
    case 1:
    if (PLR_FLAGGED(ch, PLR_RARM)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_RARM);
    act("&14$N&15 loses their &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2099;
    r_num = 2099;
    if ((r_num = real_object(2099)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_RARM);
     return;
    }
    break;
    case 2:
    if (PLR_FLAGGED(ch, PLR_LARM)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_LARM);
    act("&14$N&15 loses their &14left arm&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14left arm&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14left&15 arm in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2096;
    if ((r_num = real_object(2096)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_LARM);
     return;
    }
    break;
    case 3:
    if (PLR_FLAGGED(ch, PLR_RLEG)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_RLEG);
    act("&14$N&15 loses their &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2098;
    if ((r_num = real_object(2098)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_RLEG);
     return;
    }
    }
    break;
    case 4:
    if (PLR_FLAGGED(ch, PLR_LLEG)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_LLEG);
    act("&14$N&15 loses their &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2097;
    if ((r_num = real_object(2097)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_LLEG);
     return;
    }
    }
    break;
    case 5:
    if (PRF2_FLAGGED(ch, PRF2_HEAD) && number(1, 30) >= 24) {
    REMOVE_BIT(PRF2_FLAGS(vict), PRF2_HEAD);
    act("&14$N&15 loses their &14head&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14head&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14head&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    if (PLR_FLAGGED(vict, PLR_KILLER) && !IS_NPC(vict)) {
       sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15. This was also a &10Player &09K&00&01il&09l &11A&00&03ren&11a win!&00\r\n", GET_NAME(vict), GET_NAME(ch));
       send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
       char_from_room(vict);
       char_to_room(vict, r_death_start_room);
       REMOVE_BIT(PLR_FLAGS(vict), PLR_KILLER);
       REMOVE_BIT(PLR_FLAGS(ch), PLR_KILLER);
      char_from_room(ch);
       char_to_room(ch, r_mortal_start_room );
       look_at_room(ch, 0);
       GET_POS(vict) = POS_STANDING;
       look_at_room(vict, 0);
      }
    else if (!PLR_FLAGGED(vict, PLR_KILLER) && !PLR_FLAGGED(vict, PLR_icer) && !IS_NPC(vict)) {
      sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15.&00\r\n", GET_NAME(vict), GET_NAME(ch));
      send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
      char_from_room(vict);
      char_to_room(vict, r_death_start_room);
      SET_BIT(CLN_FLAGS(vict), CLN_Death);
      GET_DTIMER(vict) = 3600;
      GET_POS(vict) = POS_STANDING;
      look_at_room(vict, 0);
    }
    else if (PLR_FLAGGED(vict, PLR_icer) && !IS_NPC(vict)) {
      sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15. &12%s &15also collected their bounty.&00\r\n", GET_NAME(vict), GET_NAME(ch), GET_NAME(ch));
      send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
      char_from_room(vict);
      REMOVE_BIT(PLR_FLAGS(vict), PLR_icer);
      send_to_char("You collected the 1mil bounty on their head.\r\n ", ch);
      GET_GOLD(ch) += 1000000;
      char_to_room(vict, r_death_start_room);
      SET_BIT(CLN_FLAGS(vict), CLN_Death);
      GET_DTIMER(vict) = 3600;
      GET_POS(vict) = POS_STANDING;
      look_at_room(vict, 0);
     }
    if (!IS_NPC(ch) && !IS_NPC(vict)) {
      GET_HEIGHT(ch) = GET_HEIGHT(ch) + 1;
      GET_WEIGHT(vict) = GET_WEIGHT(vict) + 1;
     }
    r_num = 2095;
    if ((r_num = real_object(2095)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    *buf2 = '\0';
    sprintf(buf2, "&14%s's &01S&09e&16v&00e&16r&09e&00&01d &00&01H&09e&00a&16d&00", GET_NAME(vict));
    obj->short_description = str_dup(buf2);
    *buf2 = '\0';
    }
    }
    break;
   }
  }
      act("&16The winds whip up beneath you as you fly high into the &00s&15k&14y&16, the &00c&15l&14o&00u&15d&14s&16 seemingly making way for your presence. Your &00&06m&16e&15t&00al&15l&16i&00&06c &00w&15i&14n&00g&15s&16 start to &14g&00&06l&00&03o&11w&16 a hazy &00g&15o&11l&00&03d&16, as long forgotten &14r&00&06u&00&02ne&11s&16 start to appear on each feather. You completely spread your wings out, the &11s&09u&00&01n&16 glistening off the tips. The quills of your feathers release from their bindings, and start to hover around you. As you feel the &00e&15n&14e&00&06rg&14i&15e&00s&16 within the loose feathers reach their peak, you whisper as you point down to $N&16, .&11H&00&03e&15a&00v&15e&00&03n&11s &00R&15a&11i&00&03n&16.. The &16m&15e&00t&15a&16l feathers scream down from the sky, creating a beautiful trail of &00g&15o&11l&00&03d&16 in their wake. As the feathers reach their mark, some stab into the ground, while the rest rip into $N&16.s flesh, showering the ground in the victims &00&01b&09l&15o&00od&16.&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&16$n&16 flies up into the &00a&15i&14r&16, the clouds parting from their presence. They begin to &14g&00&06l&00&03o&11w&16 a soft hazy &00g&15o&11l&00&03d&16, as the &11s&09u&00&01n&16 seemingly brightens behind them. They fan out their &00w&15i&14n&00g&15s&16, and multitudes of feathers come loose from their holdings, and begin to hover around $n. You barely see $n&16.s lips move before the now glowing feathers descend upon you, tearing into your flesh!&00", FALSE, ch, 0, vict, TO_VICT);
      act("&16$n&16 flies up into the &00a&15i&14r&16, the clouds parting from their presence. They begin to &14g&00&06l&00&03o&11w&16 a soft hazy &00g&15o&11l&00&03d&16, as the &11s&09u&00&01n&16 seemingly brightens behind them. They fan out their &00w&15i&14n&00g&15s&16, and multitudes of feathers come loose from their holdings, and begin to hover around $n. You barely see $n&16.s lips move before the now glowing feathers descend upon $N, tearing into their flesh!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      damage(ch, vict, GET_MAX_MANA(ch) / 18 + 350000, SKILL_HEEL);
      REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      dam = GET_MAX_MANA(ch) / 18 + 350000;

    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }

    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);

 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}
 
ACMD(do_hydro) {
  struct char_data *vict;
  int percent, prob, dam, r_num = 0;
  struct obj_data *obj;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch,AFF_CHARGEL)){
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }

  if (!GET_SKILL(ch, SKILL_HYDRO)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/70) {
     send_to_char("You don't have the Ki to do Hydro Blade!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
  } else {
      send_to_char("Hit who with Hydro Blade?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);
  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30) {
    damage(ch, vict, 0, SKILL_HYDRO);
    act("&15The air is to dry to form the &00&04H&00&06y&12d&14r&15o &00&06B&00&04l&16a&15d&00e&15!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&15The air is to dry for $n to form the &00&04H&00&06y&12d&14r&15o &00&06B&00&04l&16a&15d&00e&15!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&15The air is to dry for $n to form the &00&04H&00&06y&12d&14r&15o &00&06B&00&04l&16a&15d&00e&15!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/70;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  }
 else
  if (!IS_NPC(vict) && number(1, 20) >= 12 && percent < prob && GET_MAX_HIT(ch) >= ((GET_MAX_HIT(vict) / 100) * 80)) {
   switch(number(1, 5)) {
    case 1:
    if (PLR_FLAGGED(ch, PLR_RARM)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_RARM);
    act("&14$N&15 loses their &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14right arm&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2099;
    if ((r_num = real_object(2099)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_RARM);
     return;
    }
    }
    break;
    case 2:
    if (PLR_FLAGGED(ch, PLR_LARM)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_LARM);
    act("&14$N&15 loses their &14left arm&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14left arm&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14left&15 arm in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2096;
    if ((r_num = real_object(2096)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_LARM);
     return;
    }
    }
    break;
    case 3:
    if (PLR_FLAGGED(ch, PLR_RLEG)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_RLEG);
    act("&14$N&15 loses their &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14right leg&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2098;
    if ((r_num = real_object(2098)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_RLEG);
     return;
    }
    }
    break;
    case 4:
    if (PLR_FLAGGED(ch, PLR_LLEG)) {
    REMOVE_BIT(PLR_FLAGS(vict), PLR_LLEG);
    act("&14$N&15 loses their &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14left leg&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    r_num = 2097;
    if ((r_num = real_object(2097)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    }
    if (AFF_FLAGGED(vict, AFF_IREG)) {
      act("&15Your limb quickly regrows!&00", FALSE, ch, 0, vict, TO_VICT);
      act("14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$N's&15 limb quickly regrows!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PLR_FLAGS(vict), PLR_LLEG);
     return;
    }
    }
    break;
    case 5:
    if (PRF2_FLAGGED(ch, PRF2_HEAD) && number(1, 30) >= 24) {
    REMOVE_BIT(PRF2_FLAGS(vict), PRF2_HEAD);
    act("&14$N&15 loses their &14head&15 in the attack!", FALSE, ch, 0, vict, TO_CHAR);
    act("&15You lose your &14head&15 in the attack!", FALSE, ch, 0, vict, TO_VICT);
    act("&14$N&15 loses their &14head&15 in the attack!", FALSE, ch, 0, vict, TO_NOTVICT);
    if (PLR_FLAGGED(vict, PLR_KILLER) && !IS_NPC(vict)) {
       sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15. This was also a &10Player &09K&00&01il&09l &11A&00&03ren&11a win!&00\r\n", GET_NAME(vict), GET_NAME(ch));
       send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
       char_from_room(vict);
       char_to_room(vict, r_death_start_room);
       REMOVE_BIT(PLR_FLAGS(vict), PLR_KILLER);
       REMOVE_BIT(PLR_FLAGS(ch), PLR_KILLER);
       char_from_room(ch);
       char_to_room(ch, r_mortal_start_room );
       look_at_room(ch, 0);
       GET_POS(vict) = POS_STANDING;
       look_at_room(vict, 0);
      }
    else if (!PLR_FLAGGED(vict, PLR_KILLER) && !PLR_FLAGGED(vict, PLR_icer) && !IS_NPC(vict)) {
      sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15.&00\r\n", GET_NAME(vict), GET_NAME(ch));
      send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
      char_from_room(vict);
      char_to_room(vict, r_death_start_room);
      SET_BIT(CLN_FLAGS(vict), CLN_Death);
      GET_DTIMER(vict) = 3600;
      GET_POS(vict) = POS_STANDING;
      look_at_room(vict, 0);
    }
    else if (PLR_FLAGGED(vict, PLR_icer) && !IS_NPC(vict)) {
      sprintf(buf2, "&16[&09Beheaded!&16] &14%s &15dies, having their head removed by &12%s&15. &12%s &15also collected their bounty.&00\r\n", GET_NAME(vict), GET_NAME(ch), GET_NAME(ch));
      send_to_all(buf2);
       stop_fighting(vict);
       stop_fighting(ch);
      char_from_room(vict);
      REMOVE_BIT(PLR_FLAGS(vict), PLR_icer);
      send_to_char("You collected the 1mil bounty on their head.\r\n ", ch);
      GET_GOLD(ch) += 1000000;
      char_to_room(vict, r_death_start_room);
      SET_BIT(CLN_FLAGS(vict), CLN_Death);
      GET_DTIMER(vict) = 3600;
      GET_POS(vict) = POS_STANDING;
      look_at_room(vict, 0);
     }
    if (!IS_NPC(ch) && !IS_NPC(vict)) {
      GET_HEIGHT(ch) = GET_HEIGHT(ch) + 1;
      GET_WEIGHT(vict) = GET_WEIGHT(vict) + 1;
     }
    r_num = 2095;
    if ((r_num = real_object(2095)) < 0) {
      send_to_char("Error, report to immortal.\r\n", ch);
    }
    else {
    obj = read_object(r_num, REAL);
    obj_to_room(obj, ch->in_room);
    *buf2 = '\0';
    sprintf(buf2, "&14%s's &01S&09e&16v&00e&16r&09e&00&01d &00&01H&09e&00a&16d&00", GET_NAME(vict));
    obj->short_description = str_dup(buf2);
    *buf2 = '\0';
    }
    }
    break;
   }
  }
    act("&15You draw your hand to the side, concentrating as you pull the &14w&12a&00&06t&12e&14r&15 from the &00a&14i&15r and form it into a &16b&00&06a&16l&00&06l&15. You pump &11k&00&03i&15 into it as well, and as it starts to &14g&00&06l&00&03o&11w&15 a bright &12b&00&04l&14u&00e&15 hue, you thrust your arm around your body, pulling the ball of &14w&12a&00&06te&12r&14y &11k&00&03i&15 out into a &10c&00&02r&00&06e&15s&00c&10e&00&02n&00&06t&15 shape. As you reach the apex of your swing, you yell out, '&00&04H&00&06y&12d&14r&15o &00&06B&00&04l&16a&15d&00e&15!' sending the blade of charged at $N&15, slamming it into thier chest and cutting them deep!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("$n&15 draws thier hand to the side, pulling the &14w&12a&00&06t&12e&14r&15 from the &00a&14i&15r and forming it into a &16b&00&06a&16l&00&06l&15. It starts to &14g&00&06l&00&03o&11w&15 a bright &12b&00&04l&14u&00e&15 hue, and $n thrusts thier arm around thier body, pulling the ball of &14w&12a&00&06te&12r&14y &11k&00&03i&15 out into a &10c&00&02r&00&06e&15s&00c&10e&00&02n&00&06t&15 shape. As $n reaches the apex of thier swing, they yell out, '&00&04H&00&06y&12d&14r&15o &00&06B&00&04l&16a&15d&00e&15!' sending the blade of charged at you, slamming it into your chest and cutting you deeply!&00", FALSE, ch, 0, vict, TO_VICT);
    act("$n&15 draws thier hand to the side, pulling the &14w&12a&00&06t&12e&14r&15 from the &00a&14i&15r and forming it into a &16b&00&06a&16l&00&06l&15. It starts to &14g&00&06l&00&03o&11w&15 a bright &12b&00&04l&14u&00e&15 hue, and $n thrusts thier arm around thier body, pulling the ball of &14w&12a&00&06te&12r&14y &11k&00&03i&15 out into a &10c&00&02r&00&06e&15s&00c&10e&00&02n&00&06t&15 shape. As $n reaches the apex of thier swing, they yell out, '&00&04H&00&06y&12d&14r&15o &00&06B&00&04l&16a&15d&00e&15!' sending the blade of charged at $N&15, slamming it into thier chest and cutting them deep!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      damage(ch, vict, GET_MAX_MANA(ch) / 30 + 185000, SKILL_HYDRO);
      REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      dam = GET_MAX_MANA(ch) / 30 + 185000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }

    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/70;
}


ACMD(do_eclipse)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch,AFF_CHARGEL)){
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }

  if (!GET_SKILL(ch, SKILL_ECLIPSE)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/30) {
     send_to_char("You don't have the Ki to do Hydro Eclipse Cannon!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
  } else {
      send_to_char("Hit who with Hydro Eclipse Cannon?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);
  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30) {
    damage(ch, vict, 0, SKILL_ECLIPSE);
    act("&16The air is to dry for your &00&04H&00&06y&12d&14r&15o &16E&00&04c&00&06l&12i&14p&15s&00e &00&06C&14a&15n&00n&00&04o&12n&16!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&16The air is to dry for $n&16 to fire thier &00&04H&00&06y&12d&14r&15o &16E&00&04c&00&06l&12i&14p&15s&00e &00&06C&14a&15n&00n&00&04o&12n&16!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&16The air is to dry for $n&16 to fire thier &00&04H&00&06y&12d&14r&15o &16E&00&04c&00&06l&12i&14p&15s&00e &00&06C&14a&15n&00n&00&04o&12n&16!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  }
 else
   act("&16You hold your hands out infront of you, charging a small b&00&06a&16l&00&06l&16 of &00&03k&11i&16 in your palms. As the &00&03k&11i&16 reaches the size of a basketball, you bring it to your side, and start concentrating on the &00a&14i&15r&16 around you. The moisture in the &00a&14i&15r&16 seems to dissipate, becoming snake like tendrils of &00&04w&12a&14t&12e&00&04r&16 wrapping around the b&00&06a&16l&00&06l&16 of &00&03k&11i&16, swirling around it as it makes the ball grow more massive. As you draw as much water from the air as possible, you thrust the ball of watery ki forward and yell, '&00&04H&00&06y&12d&14r&15o &16E&00&04c&00&06l&12i&14p&15s&00e &00&06C&14a&15n&00n&00&04o&12n&16!' A thick beam of ki escapes from the ball, with four water spouts snaking around it, slamming full force into $N&16's chest!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("$n&16 hold thier hands out infront of them, charging a small b&00&06a&16l&00&06l&16 of &00&03k&11i&16 in thier palms. As the &00&03k&11i&16 reaches the size of a basketball, they bring it to thier side, and start concentrating on the &00a&14i&15r&16 around you. The moisture in the &00a&14i&15r&16 seems to dissipate, becoming snake like tendrils of &00&04w&12a&14t&12e&00&04r&16 wrapping around the b&00&06a&16l&00&06l&16 of &00&03k&11i&16, swirling around it as it makes the ball grow more massive. As they draw as much water from the air as possible, $n thrusts the ball of watery ki forward and yell, '&00&04H&00&06y&12d&14r&15o &16E&00&04c&00&06l&12i&14p&15s&00e &00&06C&14a&15n&00n&00&04o&12n&16!' A thick beam of ki escapes from the ball, with four water spouts snaking around it, slamming full force into your chest!", FALSE, ch, 0, vict, TO_VICT);
   act("$n&16 hold thier hands out infront of them, charging a small b&00&06a&16l&00&06l&16 of &00&03k&11i&16 in thier palms. As the &00&03k&11i&16 reaches the size of a basketball, they bring it to thier side, and start concentrating on the &00a&14i&15r&16 around you. The moisture in the &00a&14i&15r&16 seems to dissipate, becoming snake like tendrils of &00&04w&12a&14t&12e&00&04r&16 wrapping around the b&00&06a&16l&00&06l&16 of &00&03k&11i&16, swirling around it as it makes the ball grow more massive. As they draw as much water from the air as possible, $n thrusts the ball of watery ki forward and yell, '&00&04H&00&06y&12d&14r&15o &16E&00&04c&00&06l&12i&14p&15s&00e &00&06C&14a&15n&00n&00&04o&12n&16!' A thick beam of ki escapes from the ball, with four water spouts snaking around it, slamming full force into $N&16's chest!", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      damage(ch, vict, GET_MAX_MANA(ch) / 20 + 335000, SKILL_ECLIPSE);
      REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      dam = GET_MAX_MANA(ch) / 20 + 335000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }

    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/30;
}


ACMD(do_pheonix)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }

  if (!GET_SKILL(ch, SKILL_PHEONIX)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  if (!GET_EQ(ch, WEAR_WIELD) || !GET_EQ(ch, WEAR_DUAL)) {
    send_to_char("You need to wield two swords to use Rising Pheonix Slash.\r\n", ch);
    return;
  }
  if ((GET_OBJ_VAL(GET_EQ(ch, WEAR_WIELD), 3) != TYPE_SLASH - TYPE_HIT) || (GET_OBJ_VAL(GET_EQ(ch, WEAR_DUAL), 3) != TYPE_SLASH - TYPE_HIT)) {
    send_to_char("Only you need two swords\r\n", ch);
    return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/40) {
     send_to_char("You don't have the Ki to do Rising Pheonix Slash!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
  } else {
      send_to_char("Hit who with Rising Pheonix Slash?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);
  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/60) {
    damage(ch, vict, 0, SKILL_PHEONIX);
    act("&16You begin to call upon the &00&01p&00&09h&11o&00&03e&16n&00&01i&09x&16, but it fails to answer!", FALSE, ch, 0, vict, TO_CHAR);
    act("&16$n&16 eyes go blank as they stare at thier swords!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&16$n&16 eyes go blank as they stare at thier swords!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/40;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  }
 else
   act("&16You hold your blades down to your side as you rise into the &00a&15i&14r&16, charging &00e&14n&00&06er&14g&00y&16 into your body. Your body begins to glow a soft &00&01c&16r&00&01i&09m&15s&00&01o&16n as &09r&00&01e&15d&16 and &11y&00&03e&11ll&00&03o&11w &00f&15e&14a&00t&15h&14e&00r&15s&16 of ki begin to float around your form. Soon a pair of bright, &00f&15e&14a&00t&15h&14e&00r&15y&16 &14w&00&06i&15n&16g&00s&16 seemingly grow from your shoulders. As you gaze down to $N&16, you push the &00e&14n&00&06er&14g&00y&16 within your body down to your swords, the feathery &11k&00&03i &14w&00&06i&15n&16g&00s&16 moving with the energy. You raise the blades high above your head and shout, '&00&01R&09i&11s&00&03i&00&01n&09g &00&01P&00&09h&11e&00&03o&16n&00&01i&09x &00&01S&09l&11a&00&03s&16h!' You thrust the blades downwards, and the image of a beautiful &00&01p&00&09h&11o&00&03e&16n&00&01i&09x&16  flies from them, flying down towards the ground then slams hard into $N&16's gut!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&16$n holds thier blades down to the side as they rise into the &00a&15i&14r&16. $n's body begins to glow a soft &00&01c&16r&00&01i&09m&15s&00&01o&16n as &09r&00&01e&15d&16 and &11y&00&03e&11ll&00&03o&11w &00f&15e&14a&00t&15h&14e&00r&15s&16 of ki begin to float around thier form. Soon a pair of bright, &00f&15e&14a&00t&15h&14e&00r&15y&16 &14w&00&06i&15n&16g&00s&16 seemingly grow from thier back. As you gaze up to $n&16, you see the feathery &11k&00&03i &14w&00&06i&15n&16g&00s&16 moving down the blades. $n raises the blades high above thier head and shout, '&00&01R&09i&11s&00&03i&00&01n&09g &00&01P&00&09h&11o&00&03e&16n&00&01i&09x &00&01S&09l&11a&00&03s&16h!' They thrust the blades downwards, and the image of a beautiful &00&01p&00&09h&11e&00&03o&16n&00&01i&09x&16  flies from them, flying down towards the ground then slams hard into your gut!&00", FALSE, ch, 0, vict, TO_VICT);
   act("&16$n holds thier blades down to the side as they rise into the &00a&15i&14r&16. $n's body begins to glow a soft &00&01c&16r&00&01i&09m&15s&00&01o&16n as &09r&00&01e&15d&16 and &11y&00&03e&11ll&00&03o&11w &00f&15e&14a&00t&15h&14e&00r&15s&16 of ki begin to float around thier form. Soon a pair of bright, &00f&15e&14a&00t&15h&14e&00r&15y&16 &14w&00&06i&15n&16g&00s&16 seemingly grow from thier back. As you gaze up to $n&16, you see the feathery &11k&00&03i &14w&00&06i&15n&16g&00s&16 moving down the blades. $n raises the blades high above thier head and shout, '&00&01R&09i&11s&00&03i&00&01n&09g &00&01P&00&09h&11o&00&03e&16n&00&01i&09x &00&01S&09l&11a&00&03s&16h!' They thrust the blades downwards, and the image of a beautiful &00&01p&00&09h&11e&00&03o&16n&00&01i&09x&16  flies from them, flying down towards the ground then slams hard into $N&16's gut!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      damage(ch, vict, GET_MAX_MANA(ch) / 20 + 655000, SKILL_PHEONIX);
      REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      dam = GET_MAX_MANA(ch) / 20 + 655000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }

    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/40;
}
 
ACMD(do_kakusanha) {
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
  }

  if (!GET_SKILL(ch, SKILL_KAKUSANHA)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60) {
     send_to_char("You don't have the Ki to do Kakusanha!\r\n", ch);
     return;
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
  } else {
      send_to_char("Hit who with Kakusanha?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/60) {
    damage(ch, vict, 0, SKILL_KAKUSANHA);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  }
 else
   act("&00&15You crouch down, bracing your legs as you start to charge &00&03k&00&11i&00 &15into your forearms. &00El&15e&00&11c&00&03tri&00&11c&00&15i&00ty&15 crackles down your arms as you throw your arms up to the sky and yell, &00&11K&00&03ak&16usa&00&03nh&00&11a&00&15! You unleash a large &00&16b&00&06a&00&16l&00&06l&00 &15from your hands, and flies high above $N&15! Then in a crescendo, you throw your arms downward, the ball splitting apart into five thick&00 &06b&00&14e&00a&14m&00&06s&00 &15and slamming into $N&15 and surrounding area!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&00&15$n crouches down, bracing their legs as the &00a&12i&00&14r&00 &15starts to crackle with &00e&11n&00&03er&00&11g&00y&15. They then throw their arms up to the sky, firing a large &00o&00&14r&00&06b&00 &15of &00&03k&00&11i&00 &15into the air as the words &00&11K&00&03ak&16usa&00&03nh&00&11a&00! &15shatter the air! As they throw their arms down, five large&00 &06b&00&14e&00a&14m&00&06s&00 &15fly down and smash into $N&15!&00", FALSE, ch, 0, vict, TO_NOTVICT);
   act("&00&15$n crouches down, bracing their legs as the &00a&12i&00&14r&00 &15starts to crackle with &00e&11n&00&03er&00&11g&00y&15. They then throw their arms up to the sky, firing a large &00o&00&14r&00&06b&00 &15of &00&03k&00&11i&00 &15into the air as the words &00&11K&00&03ak&16usa&00&03nh&00&11a&00! &15shatter the air! As they throw their arms down, five large&00 &06b&00&14e&00a&14m&00&06s&00 &15fly down and smash into you!!&00", FALSE, ch, 0, vict, TO_VICT);
      SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      damage(ch, vict, GET_MAX_MANA(ch) / 30 + 205000, SKILL_KAKUSANHA);
      REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      dam = GET_MAX_MANA(ch) / 30 + 205000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);
 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
}


ACMD(do_kousen)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }

  if (!GET_SKILL(ch, SKILL_KOUSEN) && !IS_NPC(ch)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60 && !IS_NPC(ch)) {
     send_to_char("You don't have the Ki to do a Kousengan!\r\n", ch);
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Kousengan who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob && !IS_NPC(ch)) {
    damage(ch, vict, 0, SKILL_KOUSEN);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
    if (!IS_NPC(ch)) {
     SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     damage(ch, vict, GET_MAX_MANA(ch) / 30 + 175000, SKILL_KOUSEN);
     REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
     dam = GET_MAX_MANA(ch) / 30 + 175000;
    }
    else if (IS_NPC(ch)) {
     damage(ch, vict, GET_LEVEL(ch), SKILL_KOUSEN);
     dam = GET_LEVEL(ch);
    }
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    act("&16You stare at your opponent, you eyes &09b&00&01u&00&03r&11n&00&03i&00&01n&09g&16 with &00e&14n&00&06er&14g&00y&16 as you begin to charge. You deftly jump backwards as you fire two thick beams of ki at $N&16, slamming the &00K&15o&14u&00&06s&15e&11n&00&03g&15a&00n&16 into there chest!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&16$n&16 stares at you, and a welling up of &00e&14n&00&06er&14g&00y&16 can be felt. $n&16 jumps back quickly, and a bright &00&01r&09e&15d&16 beam explodes from their eyes at you, slamming into your chest!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&16$n&16 stares at $N&16, and a welling up of &00e&14n&00&06er&14g&00y&16 can be felt. $n&16 jumps back quickly, and a bright &00&01r&09e&15d&16 beam explodes from their eyes at $n, slamming into $N's chest!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
 if (!IS_NPC(ch)) {
  GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
 }
}

ACMD(do_zanelb)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!PLR_FLAGGED(ch, PLR_LLEG) && !PLR_FLAGGED(ch, PLR_LLEG)) {
     send_to_char("You have no legs!\r\n", ch);
     return;
}

  if (!GET_SKILL(ch, SKILL_ZANELB)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Zanzoken Elbow who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 100);
  prob = GET_UBS(ch);
  if (!IS_NPC(vict) && GET_MAX_HIT(vict) <= 4999999) {
    send_to_char("The newbie shield protects them right now.", ch);
    return;
    }
  if (percent > prob) {
    damage(ch, vict, 0, SKILL_ZANELB);
    act("&15You miss with your attack at $N&11!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15misses with their attack at you&11!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15misses with their attack at $N&11!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    return;
  } else
    act("&15You disappear, only reappearing behind &14$N&15 where you smash your elbow into the base of their &12neck!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&11$n &15disappears, only to reappear behind &14$N&15, where they smash their elbow into the base of &14$N'&15s &12neck&11!&00", FALSE, ch, 0, vict, TO_ROOM);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, GET_MAX_HIT(ch) /70 + (GET_UBS(ch) * GET_DEX(ch) * 10), SKILL_ZANELB);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = GET_MAX_HIT(ch) /70 + (GET_UBS(ch) * GET_DEX(ch) * 10);
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
    GET_POS(vict) = POS_SITTING;
    WAIT_STATE(vict, PULSE_VIOLENCE);
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);
}
ACMD(do_heel)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch,AFF_CHARGEL)){
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LLEG) && !PLR_FLAGGED(ch, PLR_RLEG)) {
     send_to_char("You have no legs!\r\n", ch);
     return;
  }

  if (!GET_SKILL(ch, SKILL_HEEL)) {
   send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }

  if (GET_MANA(ch) < GET_MAX_MANA(ch)/100) {
     send_to_char("You don't have the Ki to do Heel Strike!\r\n", ch);
     return;
  }
  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
  } else {
      send_to_char("Hit who with Heel Strike?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 100);
  prob = GET_LBS(ch);
  if (percent > prob && GET_MANA(ch) >= GET_MAX_MANA(ch)/30) {
    damage(ch, vict, 0, SKILL_HEEL);
    act("&15You spin around, an arc of blue energy following after your outstretched heel. &14$N&15 dodges out of the way at the last minute!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15spins around, an arc of blue energy following after $s outstretched heel. you dodge out of the way at the last minute!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&14$n &15spins around, an arc of blue energy following after $s outstretched heel. &14$N&15 dodges out of the way at the last minute!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/100;
    return;
  }
 else
      act("&15You begin to charge some ki into your foot, causing a &14b&00&06l&12u&00&04e &10g&00&02lo&10w&15 to surround it in seconds. With a grin you then leap into the air and spin, smashing your heel into &14$N's&15 stomach! A sudden explosion erupts there and throws &14$N&15 to the ground!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&14$n &15begins to charge some ki into $s foot, causing a &14b&00&06l&12u&00&04e &10g&00&02lo&10w&15 to surround it in seconds. With a grin $e then leaps into the air and spins, smashing $s heel into your stomach! A sudden explosion erupts there and throws you to the ground!&00", FALSE, ch, 0, vict, TO_VICT);
      act("&14$n &15begins to charge some ki into $s foot, causing a &14b&00&06l&12u&00&04e &10g&00&02lo&10w&15 to surround it in seconds. With a grin $e then leaps into the air and spins, smashing $s heel into &14$N's&15 stomach! A sudden explosion erupts there and throws &14$N&15 to the ground!&00", FALSE, ch, 0, vict, TO_NOTVICT);
      SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      damage(ch, vict, GET_MAX_HIT(ch) / 30 + 90000, SKILL_HEEL);
      REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
      dam = GET_MAX_HIT(ch) / 30 + 90000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }

    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);

  GET_POS(vict) = POS_SITTING;
  WAIT_STATE(vict, PULSE_VIOLENCE);

 WAIT_STATE(ch, PULSE_VIOLENCE * 2);

GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/100;
}


ACMD(do_hellf)
{
  struct char_data *vict;
  int percent, prob, dam;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("Violence is not allowed in here!", ch);
    return;
  }
  if (AFF_FLAGGED(ch, AFF_CHARGEH) || AFF_FLAGGED(ch, AFF_CHARGEM) || AFF_FLAGGED(ch, AFF_CHARGEL)) {
    send_to_char("You would blow up if you tried that while charged!\r\n", ch);
    return;
   }
  if (!PLR_FLAGGED(ch, PLR_LARM) && !PLR_FLAGGED(ch, PLR_LARM)) {
     send_to_char("You have no arms!\r\n", ch);
     return;
}
  if (!GET_SKILL(ch, SKILL_HFLASH)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch)/60) {
     send_to_char("You don't have the Ki to do Hellsflash!\r\n", ch);
     return;
  }


  one_argument(argument, arg);

  if (!(vict = get_char_room_vis(ch, arg))) {
    if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
    } else {
      send_to_char("Hell's Flash who?\r\n", ch);
      return;
    }
  }
  if (vict == ch) {
    send_to_char("Aren't we funny today...\r\n", ch);
    return;
  }
  percent = number(1, 60);
  prob = GET_REINC(ch);

  if (percent > prob) {
    damage(ch, vict, 0, SKILL_HFLASH);
    act("&16You thrust your arm out but your &00&01H&16e&09l&15l&00&01s &11F&00&03l&16a&00&03s&11h&16 jams up!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&16$n thrust thier arm out but thier &00&01H&16e&09l&15l&00&01s &11F&00&03l&16a&00&03s&11h&16 jams up!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&16$n thrust thier arm out but thier &00&01H&16e&09l&15l&00&01s &11F&00&03l&16a&00&03s&11h&16 jams up!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE * 1);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
     if (number(1, 200) == 100 && GET_REINC(ch) >= 0) {
     send_to_char("&16[&09Ki Skill Loss&16]&00\r\n", ch);
     GET_REINC(ch) -= 1;
    }
    return;
  } else
    act("&16You thrust your right hand forward, a devilish grin crossing your lips. The clothing on your arm &09e&00&01x&16p&15lo&16d&00&01e&09s&16, &14b&00&06l&00&04u&12e &11l&00&03i&00g&11h&00&03t&16 shining through your arm in an intricate design as your arm splits apart, small &16m&15e&00t&16al&00l&15i&16c arms holding the chunks of your flesh, revealing a small, three barrelled ma&15c&00h&15i&16ne g&15u&00n&16, hidden in your arm. The barrels begin to spin as &00e&14l&00&06e&16c&00&03t&11r&00&03i&16c&00&06i&14t&00y&16 travels down the length of your arm. A maniacal look glazes over your eyes as you yell out '&00&01H&16e&09l&15l&00&01s &11F&00&03l&16a&00&03s&11h&16!', unleashing a machine gun blast of &00&03k&11i&16 bullets at $N&16, riddling thier body with holes!&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&16$n&16 thrust thier right hand forward, a devilish grin crossing thier lips. The clothing on thier left arm &09e&00&01x&16p&15lo&16d&00&01e&09s&16, &14b&00&06l&00&04u&12e &11l&00&03i&00g&11h&00&03t&16 shining through thier arm in an intricate design as $n&16's arm splits apart, small &16m&15e&00t&16al&00l&15i&16c arms holding the chunks of flesh, revealing a small, three barrelled ma&15c&00h&15i&16ne g&15u&00n&16, hidden in $n&16's arm. The barrels begin to spin as &00e&14l&00&06e&16c&00&03t&11r&00&03i&16c&00&06i&14t&00y&16 travels down the length of thier arm. A maniacal look glazes over thier eyes as $n&16 yells out '&00&01H&16e&09l&15l&00&01s &11F&00&03l&16a&00&03s&11h&16!', unleashing a machine gun blast of &00&03k&11i&16 bullets at you, riddling your body with &09r&00&01e&16d &09h&00&03o&00&01t &00&03k&11i&16 blasts!&00", FALSE, ch, 0, vict, TO_VICT);
    act("&16$n&16 thrust thier right hand forward, a devilish grin crossing thier lips. The clothing on thier left arm &09e&00&01x&16p&15lo&16d&00&01e&09s&16, &14b&00&06l&00&04u&12e &11l&00&03i&00g&11h&00&03t&16 shining through thier arm in an intricate design as $n&16's arm splits apart, small &16m&15e&00t&16al&00l&15i&16c arms holding the chunks of flesh, revealing a small, three barrelled ma&15c&00h&15i&16ne g&15u&00n&16, hidden in $n&16's arm. The barrels begin to spin as &00e&14l&00&06e&16c&00&03t&11r&00&03i&16c&00&06i&14t&00y&16 travels down the length of thier arm. A maniacal look glazes over thier eyes as $n&16 yells out '&00&01H&16e&09l&15l&00&01s &11F&00&03l&16a&00&03s&11h&16!', unleashing a machine gun blast of &00&03k&11i&16 bullets at $N&16, riddling thier body with holes!&00", FALSE, ch, 0, vict, TO_NOTVICT);
    SET_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    damage(ch, vict, (GET_MAX_MANA(ch) / 20 + 290000) + GET_UBS(ch) * 1000, SKILL_HFLASH);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_NOHASSLE);
    dam = (GET_MAX_MANA(ch) / 20 + 290000) + GET_UBS(ch) * 1000;
    if (dam > 5000000) {
       dam = 5000000;
       }
    if (dam < 1) {
       dam = 1;
       }
    sprintf(buf, "&14%s&15 writhes in &09pain&15 from the attack&12! &16[&12%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf1, "&15You sense the amount of energy that hit &14%s&12! &16[&10%d&16]&00", GET_NAME(vict), dam);
    sprintf(buf2, "&15Your body is wracked with &11pain&12! &16[&09%d&16]&00", dam);
    act(buf, FALSE, ch, NULL, vict, TO_CHAR);
    act(buf2, FALSE, ch, NULL, vict, TO_VICT);
    act(buf1, FALSE, ch, NULL, vict, TO_NOTVICT);
    GET_POS(vict) = POS_SITTING;
    WAIT_STATE(vict, PULSE_VIOLENCE);
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);
GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/60;
}
