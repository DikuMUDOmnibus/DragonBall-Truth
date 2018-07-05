/* ************************************************************************
*   File: act.other.c                                   Part of CircleMUD *
*  Usage: Miscellaneous player-level commands                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __ACT_OTHER_C__

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "screen.h"
#include "house.h"
#include "dg_scripts.h"

/* extern variables */
extern char arg1 [MAX_INPUT_LENGTH];
extern char arg2 [MAX_INPUT_LENGTH];
extern int spell_sort_info[MAX_SKILLS+1];
extern struct str_app_type str_app[];
extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern struct dex_skill_type dex_app_skill[];
extern struct spell_info_type spell_info[];
extern struct index_data *mob_index;
extern char *class_abbrevs[];
extern struct spell_entry spells[];
extern int free_rent;
extern int pt_allowed;
extern int max_filesize;
extern int nameserver_is_slow;
extern int top_of_world;
extern int auto_save;
extern sh_int r_trans_space;
extern sh_int r_trans_earth;
extern sh_int r_trans_vegeta;
extern sh_int r_trans_frigid;
extern sh_int r_trans_namek;
extern sh_int r_trans_orainas;
extern sh_int r_trans_konack;
extern sh_int r_death_start_room;
extern sh_int r_mortal_start_room;
extern sh_int r_immort_start_room;
extern sh_int r_demon_start_room;
extern sh_int r_saiyan_start_room;
extern sh_int r_angel_start_room;
extern sh_int r_konack_start_room;
extern sh_int r_majin_start_room;
extern sh_int r_pk1_room;
extern sh_int r_pk2_room;
extern sh_int r_pk3_room;
extern sh_int r_pk4_room;
extern sh_int r_icer_start_room;
extern sh_int r_truffle_start_room;
extern sh_int r_namek_start_room;
extern int dotmode;
extern int value1;
extern int value2;

/* extern procedures */
void list_skills(struct char_data * ch);
void appear(struct char_data * ch);
void perform_immort_vis(struct char_data *ch);
SPECIAL(shop_keeper);
ACMD(do_gen_comm);
void die(struct char_data * ch, struct char_data * killer);
void Crash_rentsave(struct char_data * ch, int cost);
void make_corpse(struct char_data * ch);

/* local functions */
ACMD(do_quit);
ACMD(do_save);
ACMD(do_fly);
ACMD(do_land);
ACMD(do_not_here);
ACMD(do_SWIFTNESS);
ACMD(do_hide);
ACMD(do_mug);
ACMD(do_practice);
ACMD(do_visible);
ACMD(do_title);
int perform_group(struct char_data *ch, struct char_data *vict);
void print_group(struct char_data *ch);
ACMD(do_group);
ACMD(do_ungroup);
ACMD(do_inplant);
ACMD(do_report);
ACMD(do_sbreed);
ACMD(do_split);
ACMD(do_ssj);
ACMD(do_kaioken);
ACMD(do_kyodaika);
ACMD(do_spar);
ACMD(do_sparkick);
ACMD(do_sparpunch);
ACMD(do_sparelbow);
ACMD(do_sparknee);
ACMD(do_sparkiblast);
ACMD(do_transform);
ACMD(do_learn);
ACMD(do_use);
ACMD(do_repair);
ACMD(do_wimpy);
ACMD(do_display);
ACMD(do_stealth);
ACMD(do_gen_write);
ACMD(do_powerup);
ACMD(do_meditate);
ACMD(do_powerdown);
ACMD(do_gen_tog);
ACMD(do_ingest);
ACMD(do_deflect);
ACMD(do_upgrade);
ACMD(do_mystic);
ACMD(do_perfection);
ACMD(do_heal);
ACMD(do_heal2);
ACMD(do_pk);
ACMD(do_majinmorph);
ACMD(do_innerfire);
ACMD(do_clanlist);
ACMD(do_clanjoin);
ACMD(do_clanresign);
ACMD(do_rlist);
ACMD(do_mlist);
ACMD(do_olist);
ACMD(do_whois);
ACMD(do_mutate);
ACMD(do_shadow);
ACMD(do_augment);
ACMD(do_reincarnate);
ACMD(do_lp);
ACMD(do_potential);
ACMD(do_majinize);
ACMD(do_majinyes);
ACMD(do_mindread);
ACMD(do_skanassan);
ACMD(do_training);
ACMD(do_scouter);
ACMD(do_clan);
ACMD(do_race);
ACMD(do_roleplay);
ACMD(do_quote);
ACMD(do_bounty);
ACMD(do_password);
ACMD(do_trade);
ACMD(do_suicide);
ACMD(do_drop);
ACMD(do_wish);
ACMD(do_calc);
ACMD(do_sangel);
ACMD(do_transport);
ACMD(do_recruit);
ACMD(do_boot);
ACMD(do_demote);
ACMD(do_promote);
ACMD(do_elect);
ACMD(do_cquit);
ACMD(do_moonburst);

/* Do not remove or alter text in version unless you want to admit to being
* a lying douche bag. You may add to it, but do not alter or remove any of
* the text.
*/
ACMD(do_version)
{
 send_to_char("Circlemud: 3.0\r\n", ch);
 send_to_char("Based on Diku: Gamma 0.0\r\n", ch);
 send_to_char("Dragonball Truth: Version 1.8\r\n", ch);
 send_to_char("Dragonball Truth Credits:     Code by      - Iovan\r\n"
              "                              Server Admin - Kuvoc\r\n"
              "                              Building by  - Iovan,\r\n"
              "Solbet, Kuvoc, Xyron, Shinigami, Mhurzin, Shigeru,\r\n"
              "Eragon, Emerai, Necrid, and many others.\r\n", ch);
}
/* for growth tokens */

ACMD(do_growth)
{
  struct obj_data *obj, *next_obj;
  int found = FALSE, num = 0;

  for (obj = ch->carrying; obj; obj = next_obj) {
          next_obj = obj->next_content;
   if (IS_OBJ_STAT(obj, ITEM_NOINVIS)) {
     found = TRUE;
     if (GET_LEVEL(ch) >= 20000) {
     num += 9;
     extract_obj(obj);
     }
     else if (GET_LEVEL(ch) >= 15000) {
     num += 7;
     extract_obj(obj);
     }
     else if (GET_LEVEL(ch) >= 12500) {
     num += 5;
     extract_obj(obj);
     }
     else if (GET_LEVEL(ch) >= 10000) {
     num += 3;
     extract_obj(obj);
     }
     else if (GET_LEVEL(ch) >= 7500) {
     num += 1;
     extract_obj(obj);
     }
     else if (GET_LEVEL(ch) < 7500) {
     send_to_char("&15You cannot do that until you have &0915,000,000 &15TNB!&00\r\n", ch);
     }
    }
   
  }
   if (found == TRUE) {
    sprintf(buf, "You have gained %d levels!\r\n", num);
    send_to_char(buf, ch);
     while (num > 0) {
       if (CLN_FLAGGED(ch, CLN_Death)) {
       --num;
       gain_exp(ch, GET_LEVEL(ch) * 1500);
       }
       else {
       --num;
       gain_exp(ch, GET_LEVEL(ch) * 2000);
       }
     }
     return;
   }
  else {
   send_to_char("You don't have any growth tokens.\r\n", ch);
   return;
  }
}

/* For fishing */
ACMD(do_reel)
{
  if (!PRF2_FLAGGED(ch, PRF2_FISHING)) {
   send_to_char("You are not even fishing!\r\n", ch);
   return;
  }
  if (PRF2_FLAGGED(ch, PRF2_REELING)) {
   send_to_char("You are already reeling in!\r\n", ch);
   return;
  }
  if (!PRF2_FLAGGED(ch, PRF2_CAUGHT)) {
   send_to_char("You don't even have a fish on the line!\r\n", ch);
   return;
  }
  else {
    act("You start reeling in.", TRUE, ch, 0, 0, TO_CHAR);
    act("$n starts reeling in.", TRUE, ch, 0, 0, TO_ROOM);
    SET_BIT(PRF2_FLAGS(ch), PRF2_REELING);
    REMOVE_BIT(PRF2_FLAGS(ch), PRF2_CAUGHT);
  }
}
ACMD(do_fish)
{
  *buf = '\0';
  one_argument(argument, buf);
  if (!ROOM_FLAGGED(ch->in_room, ROOM_SHORE)) {
    send_to_char("You are not near water!\r\n", ch);
    return;
  }
  if (!GET_EQ(ch, WEAR_HOLD)) {
    send_to_char("You do not have a fishing pole!\r\n", ch);
    return;
  }
  if (!IS_OBJ_STAT(GET_EQ(ch, WEAR_HOLD), ITEM_ROD)) {
    send_to_char("You are not holding a fishing pole!\r\n", ch);
    return;
  }
  if (PRF2_FLAGGED(ch, PRF2_CAUGHT) || PRF2_FLAGGED(ch, PRF2_REELING)) {
    send_to_char("You have a fish on the pole!\r\n", ch);
    return;
  }
  if (PRF2_FLAGGED(ch, PRF2_FISHING)) {
    act("You decide to give up and reel in the line.", TRUE, ch, 0, 0, TO_CHAR);
    act("$n gives up and reels in their fishing line.", TRUE, ch, 0, 0, TO_ROOM);
    REMOVE_BIT(PRF2_FLAGS(ch), PRF2_FISHING);
    if (GET_SKILL(ch, SPELL_FISHING) >= 70) {
    REMOVE_BIT(PRF2_FLAGS(ch), PRF2_FISH4);
    }
    else if (GET_SKILL(ch, SPELL_FISHING) >= 40) {
    REMOVE_BIT(PRF2_FLAGS(ch), PRF2_FISH3);
    }
    else if (GET_SKILL(ch, SPELL_FISHING) >= 21) {
    REMOVE_BIT(PRF2_FLAGS(ch), PRF2_FISH2);
    }
    else if (GET_SKILL(ch, SPELL_FISHING) <= 20) {
    REMOVE_BIT(PRF2_FLAGS(ch), PRF2_FISH1);
    }

    return;
  }
  if (GET_POS(ch) = POS_STANDING) {
    GET_POS(ch) = POS_RESTING;
    act("You lie back and cast your line out into the water.", TRUE, ch, 0, 0, TO_CHAR);
    act("$n lays back and cast their fishing line out into the water.", TRUE, ch, 0, 0, TO_ROOM);
    SET_BIT(PRF2_FLAGS(ch), PRF2_FISHING);
    if (GET_SKILL(ch, SPELL_FISHING) >= 70) {
    SET_BIT(PRF2_FLAGS(ch), PRF2_FISH4);
    }
    else if (GET_SKILL(ch, SPELL_FISHING) >= 40) {
    SET_BIT(PRF2_FLAGS(ch), PRF2_FISH3);
    }
    else if (GET_SKILL(ch, SPELL_FISHING) >= 21) {
    SET_BIT(PRF2_FLAGS(ch), PRF2_FISH2);
    }
    else if (GET_SKILL(ch, SPELL_FISHING) <= 20) {
    SET_BIT(PRF2_FLAGS(ch), PRF2_FISH1);
    }
    GET_FISHT(ch) = 3000;
    return;
  }
  else {
    act("You cast out into the water.", TRUE, ch, 0, 0, TO_CHAR);
    act("$n casts their fishing line out into the water.", TRUE, ch, 0, 0, TO_ROOM);
    SET_BIT(PRF2_FLAGS(ch), PRF2_FISHING);
    if (GET_SKILL(ch, SPELL_FISHING) >= 70) {
    SET_BIT(PRF2_FLAGS(ch), PRF2_FISH4);
    }
    else if (GET_SKILL(ch, SPELL_FISHING) >= 40) {
    SET_BIT(PRF2_FLAGS(ch), PRF2_FISH3);
    }
    else if (GET_SKILL(ch, SPELL_FISHING) >= 21) {
    SET_BIT(PRF2_FLAGS(ch), PRF2_FISH2);
    }
    else if (GET_SKILL(ch, SPELL_FISHING) <= 20) {
    SET_BIT(PRF2_FLAGS(ch), PRF2_FISH1);
    }
    GET_FISHT(ch) = 3000;
    return;
  }
}

void fishcheck()
{
   struct descriptor_data *d;
   struct obj_data *obj;
   int r_num, success = FALSE, exp = number(100000, 20000000), add_lp = 0;
  for (d = descriptor_list; d; d = d->next) {
     if (STATE(d) != CON_PLAYING)
         continue;
     if (!d->character) /* just to make sure */
         continue;
     if (!PRF2_FLAGGED(d->character, PRF2_FISHING))
         continue;
     if (GET_FISHT(d->character) <= 0 && !PRF2_FLAGGED(d->character, PRF2_REELING)) {
      act("You give up and reel in.", TRUE, d->character, 0, 0, TO_CHAR);
      act("$n reels in having caught nothing.", TRUE, d->character, 0, 0, TO_ROOM);
      REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISHING);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH1);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH2);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH3);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH4);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_CAUGHT);
     }
     if (PRF2_FLAGGED(d->character, PRF2_FISHING)) {
        if (PRF2_FLAGGED(d->character, PRF2_FISH4) && number(1, 5000) >= 3800) {
          success = TRUE;
        }
        else if (PRF2_FLAGGED(d->character, PRF2_FISH3) && number(1, 5200) >= 4000) {
          success = TRUE;
        }
        else if (PRF2_FLAGGED(d->character, PRF2_FISH2) && number(1, 5500) >= 4300) {
          success = TRUE;
        }
        else if (PRF2_FLAGGED(d->character, PRF2_FISH1) && number(1, 6000) >= 4800) {
          success = TRUE;
        }
        if (success == TRUE && !PRF2_FLAGGED(d->character, PRF2_REELING) && !PRF2_FLAGGED(d->character, PRF2_CAUGHT)) {
         SET_BIT(PRF2_FLAGS(d->character), PRF2_CAUGHT);
         act("You feel the line on your fishing pole tug.", TRUE, d->character, 0, 0, TO_CHAR);
         act("$n smiles as $s fishing pole seems to dip.", TRUE, d->character, 0, 0, TO_ROOM);
        }
        if (PRF2_FLAGGED(d->character, PRF2_REELING) && GET_FISHT(d->character) >= 1) {
         GET_FISHT(d->character) -= GET_SKILL(d->character, SPELL_FISHING) + 40;
        }
        if ((PRF2_FLAGGED(d->character, PRF2_REELING) || PRF2_FLAGGED(d->character, PRF2_CAUGHT)) && number(1, 3000) >= 2800) {
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_REELING);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_CAUGHT);
         act("You believe you lost the fish.", TRUE, d->character, 0, 0, TO_CHAR);
         act("$n frowns sadly.", TRUE, d->character, 0, 0, TO_ROOM);
        }
        if (PRF2_FLAGGED(d->character, PRF2_REELING) && GET_FISHT(d->character) <= 0) {
         switch(number(1, 30)) {
           case 1:
           case 2:
           case 3:
           case 4:
           case 5:
              r_num = 2323;
           if ((r_num = real_object(2323)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 6:
              r_num = 2324;
           if ((r_num = real_object(2324)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 7:
              r_num = 2325;
           if ((r_num = real_object(2325)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 8:
              r_num = 2326;
           if ((r_num = real_object(2326)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience &00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 9:
              r_num = 2327;
           if ((r_num = real_object(2327)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 10:
              r_num = 2328;
           if ((r_num = real_object(2328)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 11:
              r_num = 2329;
           if ((r_num = real_object(2329)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 12:
              r_num = 2330;
           if ((r_num = real_object(2330)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            add_lp = number(2000, 5000);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
            send_to_char("&09FISHING LP BONUS!!!&00\r\n", d->character);
            GET_PRACTICES(d->character) = GET_PRACTICES(d->character) + add_lp;
           }
           break;
           case 13:
              r_num = 2331;
           if ((r_num = real_object(2331)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 14:
              r_num = 2332;
           if ((r_num = real_object(2332)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 15:
              r_num = 2333;
           if ((r_num = real_object(2333)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 16:
              r_num = 2334;
           if ((r_num = real_object(2334)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 17:
              r_num = 2335;
           if ((r_num = real_object(2335)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 18:
              r_num = 2336;
           if ((r_num = real_object(2336)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 19:
              r_num = 2337;
           if ((r_num = real_object(2337)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 20:
              r_num = 2338;
           if ((r_num = real_object(2338)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 21:
              r_num = 2339;
           if ((r_num = real_object(2339)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 22:
              r_num = 1512;
           if ((r_num = real_object(1512)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 23:
              r_num = 2340;
           if ((r_num = real_object(2340)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            add_lp = number(2000, 5000);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
            send_to_char("&09FISHING LP BONUS!!!&00\r\n", d->character);
            GET_PRACTICES(d->character) = GET_PRACTICES(d->character) + add_lp;
           }
           break;
           case 24:
              r_num = 2341;
           if ((r_num = real_object(2341)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 25:
              r_num = 2342;
           if ((r_num = real_object(2342)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           case 26:
              r_num = 2343;
           if ((r_num = real_object(2343)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
           default:
              r_num = 2322;
           if ((r_num = real_object(2322)) < 0) {
            send_to_char("Error, report to immortal.\r\n", d->character);
           }
           else {
            obj = read_object(r_num, REAL);
            obj_to_char(obj, d->character);
            send_to_char("&10You &15receive &00&06some &00&09experience&00.&00\r\n", d->character);
            gain_exp(d->character, exp);
           }
           break;
         }
         if (GET_SKILL(d->character, SPELL_FISHING) <= 99 && number(1, 10) == 10) {
           send_to_char("&10You know slightly more about fishing!&00\r\n", d->character);
           GET_SKILL(d->character, SPELL_FISHING) += 1;
         }
         act("You catch a $p!", TRUE, d->character, obj, 0, TO_CHAR);
         act("$n smiles as $e catches a $p!", TRUE, d->character, obj, 0, TO_ROOM);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISHING);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH1);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH2);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH3);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH4);
         REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_REELING);
        }
      }
      GET_FISHT(d->character) -= 10;
    }  
}
/* For going Golden Oozaru */
ACMD(do_moonburst)
{
  if (!GET_SKILL(ch, SPELL_MOONB)) {
       send_to_char("You don't know how to do that!\r\n", ch);
       return;
      }
  if (IS_NPC(ch)) {
    send_to_char("You are just a mob!\r\n", ch);
       return;
      }
  if (PRF2_FLAGGED(ch, PRF2_GO)) {
   send_to_char("You can't use this while in Golden Oozaru!\r\n", ch);
   return;
  }
  if (PRF_FLAGGED(ch, PRF_OOZARU)) {
   send_to_char("You can't use this while in Oozaru!\r\n", ch);
   return;
  }
  if (!PLR_FLAGGED(ch, PLR_trans3)) {
    send_to_char("You can't use moonburst outside of Super Saiyan Three.\r\n", ch);
    return;
  }
  if (PLR_FLAGGED(ch, PLR_trans3) && PLR_FLAGGED(ch, PLR_trans4)) {
   send_to_char("You refuse to use that right now, perfering the balance of your power now.\r\n", ch);
    return;
  }
 if (PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4)) {
   if (!PLR_FLAGGED(ch, PLR_STAIL)) {
    send_to_char("Without a tail, it will be useless to you!\r\n", ch);
    return;
   }
   if (GET_MANA(ch) <= GET_MAX_MANA(ch) / 2) {
    send_to_char("You do not have enough ki to control the moonburst.\r\n", ch);
   }
   else {
    act("&09$n&16 stares up into a &14m&00&06o&15o&14n&11b&00&03u&15r&00&03s&11t&16 as their eyes turn &09b&00&01loo&09d &09r&00&01e&09d&16 and fangs sprout in their mouth. A &11go&00&03ld&00&11en &12a&14u&12r&14a&16 appears around them and quickly grows destroying the ground beneath them. There is a sudden explosion as their aura turns into a &11go&00&03ld&00&11en&16 pillar shooting into the sky surrounded by rings of &00li&14gh&00&06t&14ni&00ng&16 as huge rocks go flying into the sky. $e begins to grow in size as &00&03brown&16 fur appears all over them when the &11go&00&03ld&00&11en&16 aura collapses into dust. They suddenly grow to full size and let out a massive roar as $s fur turns &11go&00&03ld&11en ye&00&03ll&11ow&16. &09$n&16 has transformed into a &11Go&00&03ld&11en&16 &00&03O&09o&16z&00&03ar&01u&16.", FALSE, ch, 0, 0, TO_ROOM); 
    act("&09You&16 look up into the &14m&00&06o&15o&14n&11b&00&03u&15r&00&03s&11t&16 in the sky and begin to feel a &09s&16a&09v&16a&09g&16e power begin to rise from deep inside you. Your eyes turn &09b&00&01loo&09d &09r&00&01e&09d&16 as fangs sprout in your mouth. A &11go&00&03ld&00&11en &12a&14u&12r&14a&16 appears around you and quickly grows, destroying the ground beneath you. Your aura suddendly explodes forth into a massive &11go&00&03ld&11en&16 pillar surrounded by rings of &00li&14gh&00&06t&14ni&00ng&16 throwing huge chunks of rock flying into the sky as you begin to grow in size. &00&03Brown&16 hair begins to grow all over your body just as your aura collapses into a pillar of &11g&00&03ol&11d&16 dust. You quickly grow to full size and plant your feet on the ground and let out a massive roar as your fur turns &11go&00&03ld&11en ye&00&03ll&11ow&16. You have transformed into a &11Go&00&03ld&11en&16 &00&03O&09o&16z&00&03ar&01u&16.", FALSE, ch, 0, 0, TO_CHAR);
    SET_BIT(PRF2_FLAGS(ch), PRF2_GO);
    GET_RAGET(ch) += 1200;
    GET_MAX_HIT(ch) += 57500000;
    GET_MANA(ch) -= GET_MAX_MANA(ch) / 2;
    return;
   }
 }
}
/* Clan quit, self explanitory */
ACMD(do_cquit)
{
 
  if (CLN_FLAGGED(ch, CLN_LEADER1) || CLN_FLAGGED(ch, CLN_LEADER2) || CLN_FLAGGED(ch, CLN_LEADER3) || CLN_FLAGGED(ch, CLN_LEADER4) || CLN_FLAGGED(ch, CLN_LEADER5) || CLN_FLAGGED(ch, CLN_LEADER6) || CLN_FLAGGED(ch, CLN_LEADER7) || CLN_FLAGGED(ch, CLN_LEADER8)) {
   send_to_char("Clan leaders can't quit their clans without first using elect to hand over power.\r\n", ch);
   return;
  }
  if (!CLN_FLAGGED(ch, CLN_NEWBIE) && !CLN_FLAGGED(ch, CLN_NOVICE) && !CLN_FLAGGED(ch, CLN_MEMBER) && !CLN_FLAGGED(ch, CLN_CAPTAIN)) {
   send_to_char("You are not in a clan.\r\n", ch);
   return;
  }
 if (CLN_FLAGGED(ch, CLN_NEWBIE) || CLN_FLAGGED(ch, CLN_NOVICE) || CLN_FLAGGED(ch, CLN_MEMBER) || CLN_FLAGGED(ch, CLN_CAPTAIN)) {
   send_to_char("You quit your clan.\r\n", ch);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_MEMBER1);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_MEMBER2);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_MEMBER3);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_MEMBER4);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_MEMBER5);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_MEMBER6);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_MEMBER7);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_MEMBER8);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_CAPTAIN);
    return;
  }
  else {
   send_to_char("You are not in a clan!\r\n", ch);
   return;
  }
}
/* Set new leader of clan */
ACMD(do_elect)
{
  struct char_data *victim;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  one_argument(argument, buf);
  if (!CLN_FLAGGED(ch, CLN_LEADER1) && !CLN_FLAGGED(ch, CLN_LEADER2) && !CLN_FLAGGED(ch, CLN_LEADER3) & !CLN_FLAGGED(ch, CLN_LEADER4) && !CLN_FLAGGED(ch, CLN_LEADER5) && !CLN_FLAGGED(ch, CLN_LEADER6) & !CLN_FLAGGED(ch, CLN_LEADER7) & !CLN_FLAGGED(ch, CLN_LEADER8)) {
   send_to_char("You are not the leader of a clan...\r\n", ch);
   return;
  }
  if (!*buf) {
   send_to_char("Who do you wish to lead the clan?\r\n", ch);
   return;
  }
  if (!(victim = get_char_vis(ch, buf))) {
      send_to_char("Name who leader?\r\n", ch);
      return;
  }
  if (victim == ch) {
   send_to_char("You do not need to make yourself leader...\r\n", ch);
   return;
  }
  if (!CLN_FLAGGED(victim, CLN_NEWBIE) && !CLN_FLAGGED(victim, CLN_NOVICE) && !CLN_FLAGGED(victim, CLN_MEMBER) && !CLN_FLAGGED(victim, CLN_CAPTAIN) && !CLN_FLAGGED(victim, CLN_LEADER1) && !CLN_FLAGGED(victim, CLN_LEADER2) && !CLN_FLAGGED(victim, CLN_LEADER3) && !CLN_FLAGGED(victim, CLN_LEADER4) && !CLN_FLAGGED(victim, CLN_LEADER5) && !CLN_FLAGGED(victim, CLN_LEADER6) && !CLN_FLAGGED(victim, CLN_LEADER7) && !CLN_FLAGGED(victim, CLN_LEADER8)){
   send_to_char("They are not in a clan!", ch);
   return;
  }
  if (CLN_FLAGGED(ch, CLN_LEADER1) && CLN_FLAGGED(victim, CLN_MEMBER1)) {
    send_to_char("They are now leader.\r\n", ch);
    send_to_char("You made leader of your clan, the last leader has stepped down.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER1);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER1);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_LEADER1);
    SET_BIT(CLN_FLAGS(ch), CLN_MEMBER1);
    SET_BIT(CLN_FLAGS(ch), CLN_NEWBIE);
    return;
  }
  else if (CLN_FLAGGED(ch, CLN_LEADER2) && CLN_FLAGGED(victim, CLN_MEMBER2)) {
    send_to_char("They are now leader.\r\n", ch);
    send_to_char("You made leader of your clan, the last leader has stepped down.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER2);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER2);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_LEADER2);
    SET_BIT(CLN_FLAGS(ch), CLN_MEMBER2);
    SET_BIT(CLN_FLAGS(ch), CLN_NEWBIE);
    return;
  }
  else if (CLN_FLAGGED(ch, CLN_LEADER3) && CLN_FLAGGED(victim, CLN_MEMBER3)) {
    send_to_char("They are now leader.\r\n", ch);
    send_to_char("You made leader of your clan, the last leader has stepped down.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER3);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER3);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_LEADER3);
    SET_BIT(CLN_FLAGS(ch), CLN_MEMBER3);
    SET_BIT(CLN_FLAGS(ch), CLN_NEWBIE);
    return;
  }
  else if (CLN_FLAGGED(ch, CLN_LEADER3) && CLN_FLAGGED(victim, CLN_MEMBER3)) {
    send_to_char("They are now leader.\r\n", ch);
    send_to_char("You made leader of your clan, the last leader has stepped down.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER3);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER3);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_LEADER3);
    SET_BIT(CLN_FLAGS(ch), CLN_MEMBER3);
    SET_BIT(CLN_FLAGS(ch), CLN_NEWBIE);
    return;
  }
  else if (CLN_FLAGGED(ch, CLN_LEADER4) && CLN_FLAGGED(victim, CLN_MEMBER4)) {
    send_to_char("They are now leader.\r\n", ch);
    send_to_char("You made leader of your clan, the last leader has stepped down.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER4);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER4);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_LEADER4);
    SET_BIT(CLN_FLAGS(ch), CLN_MEMBER4);
    SET_BIT(CLN_FLAGS(ch), CLN_NEWBIE);
    return;
  }
  else if (CLN_FLAGGED(ch, CLN_LEADER5) && CLN_FLAGGED(victim, CLN_MEMBER5)) {
    send_to_char("They are now leader.\r\n", ch);
    send_to_char("You made leader of your clan, the last leader has stepped down.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER5);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER5);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_LEADER5);
    SET_BIT(CLN_FLAGS(ch), CLN_MEMBER5);
    SET_BIT(CLN_FLAGS(ch), CLN_NEWBIE);
    return;
  }
  else if (CLN_FLAGGED(ch, CLN_LEADER6) && CLN_FLAGGED(victim, CLN_MEMBER6)) {
    send_to_char("They are now leader.\r\n", ch);
    send_to_char("You made leader of your clan, the last leader has stepped down.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER6);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER6);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_LEADER6);
    SET_BIT(CLN_FLAGS(ch), CLN_MEMBER6);
    SET_BIT(CLN_FLAGS(ch), CLN_NEWBIE);
    return;
  }
  else if (CLN_FLAGGED(ch, CLN_LEADER7) && CLN_FLAGGED(victim, CLN_MEMBER7)) {
    send_to_char("They are now leader.\r\n", ch);
    send_to_char("You made leader of your clan, the last leader has stepped down.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER7);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER7);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_LEADER7);
    SET_BIT(CLN_FLAGS(ch), CLN_MEMBER7);
    SET_BIT(CLN_FLAGS(ch), CLN_NEWBIE);
    return;
  }
  else if (CLN_FLAGGED(ch, CLN_LEADER8) && CLN_FLAGGED(victim, CLN_MEMBER8)) {
    send_to_char("They are now leader.\r\n", ch);
    send_to_char("You made leader of your clan, the last leader has stepped down.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER8);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER8);
    REMOVE_BIT(CLN_FLAGS(ch), CLN_LEADER8);
    SET_BIT(CLN_FLAGS(ch), CLN_MEMBER8);
    SET_BIT(CLN_FLAGS(ch), CLN_NEWBIE);
    return;
  }
  else {
   send_to_char("ZOMG! REPORT! REPORT! The mud is falling! The mud is falling! Oh wait, you aren't the leader of a clan.", ch);
   return;
  }
} 
/* Recruit new clan member */
ACMD(do_recruit) 
{
  struct char_data *victim;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  half_chop(argument, buf, buf1);
  if (!*buf) {
   send_to_char("Who do you wish to recruit to your clan?\r\n", ch);
   return;
  }
  if (!(victim = get_char_room_vis(ch, buf))) {
      send_to_char("Recruit who?\r\n", ch);
      return;
  }
  if (victim == ch) {
   send_to_char("You do not need to recruit yourself...\r\n", ch);
   return;
  }
  if (!*buf1) {
   send_to_char("You need to specify a clan rank to make them.\r\nRanks: Recruit, Member, Adept, Captain.\r\n", ch);
   return;
  }
  if (CLN_FLAGGED(victim, CLN_NEWBIE) || CLN_FLAGGED(victim, CLN_NOVICE) || CLN_FLAGGED(victim, CLN_MEMBER) || CLN_FLAGGED(victim, CLN_CAPTAIN) || CLN_FLAGGED(victim, CLN_LEADER1) || CLN_FLAGGED(victim, CLN_LEADER2) || CLN_FLAGGED(victim, CLN_LEADER3) || CLN_FLAGGED(victim, CLN_LEADER4) || CLN_FLAGGED(victim, CLN_LEADER5) || CLN_FLAGGED(victim, CLN_LEADER6) || CLN_FLAGGED(victim, CLN_LEADER7) || CLN_FLAGGED(victim, CLN_LEADER8)){
   send_to_char("They are already in a clan!", ch);
   return;
  }
  if (!str_cmp(buf1,"recruit")) {
   if (CLN_FLAGGED(ch, CLN_LEADER1)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER1);
    SET_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER2)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER2);
    SET_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER3)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER3);
    SET_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER4)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER4);
    SET_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER5)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER5);
    SET_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER6)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER6);
    SET_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER7)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER7);
    SET_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER8)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER8);
    SET_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    return;
   }
  }
 if (!str_cmp(buf1,"member")) {
   if (CLN_FLAGGED(ch, CLN_LEADER1)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER1);
    SET_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER2)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER2);
    SET_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER3)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER3);
    SET_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER4)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER4);
    SET_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER5)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER5);
    SET_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER6)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER6);
    SET_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER7)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER7);
    SET_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER8)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER8);
    SET_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    return;
   }
  }
  if (!str_cmp(buf1,"adept")) {
   if (CLN_FLAGGED(ch, CLN_LEADER1)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER1);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER2)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER2);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER3)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER3);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER4)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER4);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER5)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER5);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER6)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER6);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER7)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER7);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER8)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER8);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    return;
   }
  }
  if (!str_cmp(buf1,"captain")) {
   if (CLN_FLAGGED(ch, CLN_LEADER1)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER1);
    SET_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER2)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER2);
    SET_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER3)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER3);
    SET_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER4)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER4);
    SET_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER5)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER5);
    SET_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER6)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim );
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER6);
    SET_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER7)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER7);
    SET_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_LEADER8)) {
    send_to_char("&15You successfuly recruit them.\r\n", ch);
    sprintf(buf2, "&15You have been recruited by &14%s&15 into their clan.\r\n", GET_NAME(ch));
    send_to_char(buf2, victim);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER8);
    SET_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
   }
  }
  else {
   send_to_char("ZOMG! REPORT! REPORT! The mud is falling! The mud is falling! Oh wait, you aren't the leader of a clan.", ch);
   return;
  }
}
/* Boot clan member from clan */
ACMD(do_boot)
{
  struct char_data *victim;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  one_argument(argument, buf);
  if (!*buf) {
   send_to_char("Who do you wish to boot from the clan?\r\n", ch);
   return;
  }
  if (!(victim = get_char_vis(ch, buf))) {
      send_to_char("Boot who from the clan?\r\n", ch);
      return;
  }
  if (victim == ch) {
   send_to_char("You do not need to boot yourself dumbass...\r\n", ch);
   return;
  }
  if (!CLN_FLAGGED(victim, CLN_NEWBIE) && !CLN_FLAGGED(victim, CLN_NOVICE) && !CLN_FLAGGED(victim, CLN_MEMBER) && !CLN_FLAGGED(victim, CLN_CAPTAIN) && !CLN_FLAGGED(victim, CLN_LEADER1) && !CLN_FLAGGED(victim, CLN_LEADER2) && !CLN_FLAGGED(victim, CLN_LEADER3) && !CLN_FLAGGED(victim, CLN_LEADER4) && !CLN_FLAGGED(victim, CLN_LEADER5) && !CLN_FLAGGED(victim, CLN_LEADER6) && !CLN_FLAGGED(victim, CLN_LEADER7) && !CLN_FLAGGED(victim, CLN_LEADER8)){
   send_to_char("They are not in a clan!", ch);
   return;
  }
  if (CLN_FLAGGED(ch, CLN_LEADER1)) {
    send_to_char("They have been removed from your clan.\r\n", ch);
    send_to_char("You have been booted from your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER1);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_LEADER2)) {
    send_to_char("They have been removed from your clan.\r\n", ch);
    send_to_char("You have been booted from your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER2);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_LEADER3)) {
    send_to_char("They have been removed from your clan.\r\n", ch);
    send_to_char("You have been booted from your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER3);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_LEADER4)) {
    send_to_char("They have been removed from your clan.\r\n", ch);
    send_to_char("You have been booted from your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER4);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_LEADER5)) {
    send_to_char("They have been removed from your clan.\r\n", ch);
    send_to_char("You have been booted from your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER5);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_LEADER6)) {
    send_to_char("They have been removed from your clan.\r\n", ch);
    send_to_char("You have been booted from your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER6);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_LEADER7)) {
    send_to_char("They have been removed from your clan.\r\n", ch);
    send_to_char("You have been booted from your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER7);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
  }
  if (CLN_FLAGGED(ch, CLN_LEADER8)) {
    send_to_char("They have been removed from your clan.\r\n", ch);
    send_to_char("You have been booted from your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER8);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    return;
  }
  else {
   send_to_char("ZOMG! REPORT! REPORT! The mud is falling! The mud is falling! Oh wait, you aren't the leader of a clan.", ch);
   return;
  }
}
/* Promote a clan member */
ACMD(do_promote)
{
  struct char_data *victim;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  half_chop(argument, buf, buf1);
  if (!CLN_FLAGGED(ch, CLN_LEADER1) && !CLN_FLAGGED(ch, CLN_LEADER2) && !CLN_FLAGGED(ch, CLN_LEADER3) && !CLN_FLAGGED(ch, CLN_LEADER4) && !CLN_FLAGGED(ch, CLN_LEADER5) && !CLN_FLAGGED(ch, CLN_LEADER6) && !CLN_FLAGGED(ch, CLN_LEADER7) && !CLN_FLAGGED(ch, CLN_LEADER8)) {
   send_to_char("You are not the leader of a clan...\r\n", ch);
   return;
  }
  if (!*buf) {
   send_to_char("Who do you wish to promote?\r\n", ch);
   return;
  }
  if (!(victim = get_char_room_vis(ch, buf))) {
      send_to_char("Promote who?\r\n", ch);
      return;
  }
  if (victim == ch) {
   send_to_char("You do not need to promote yourself...\r\n", ch);
   return;
  }
  if (!*buf1) {
   send_to_char("You need to specify a clan rank to make them.\r\nRanks: Recruit, Member, Adept, Captain.\r\n", ch);
   return;
  }
  if (!str_cmp(buf1,"newbie") && CLN_FLAGGED(victim, CLN_NEWBIE)) {
   send_to_char("They are already that rank.\r\n", ch);
   return;
  }
  else if (!str_cmp(buf1,"newbie")) {
   send_to_char("They are have been promoted.\r\n", ch);
   send_to_char("You have been promoted.\r\n", victim);
    SET_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
   return;
  }
  if (!str_cmp(buf1,"member") && CLN_FLAGGED(victim, CLN_NOVICE)) {
   send_to_char("They are already that rank.\r\n", ch);
   return;
  }
  else if (!str_cmp(buf1,"member")) {
    send_to_char("They are have been promoted.\r\n", ch);
    send_to_char("You have been promoted.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    SET_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
   return;
  }
  if (!str_cmp(buf1,"adept") && CLN_FLAGGED(victim, CLN_MEMBER)) {
   send_to_char("They are already that rank.\r\n", ch);
   return;
  }
  else if (!str_cmp(buf1,"adept")) {
    send_to_char("They are have been promoted.\r\n", ch);
    send_to_char("You have been promoted.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
   return;
  }
   if (!str_cmp(buf1,"captain") && CLN_FLAGGED(victim, CLN_CAPTAIN)) {
   send_to_char("They are already that rank.\r\n", ch);
   return;
  }
  else if (!str_cmp(buf1,"captain")) {
    send_to_char("They are have been promoted.\r\n", ch);
    send_to_char("You have been promoted.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    SET_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
   return;
  }
  else {
   send_to_char("ZOMG! REPORT! REPORT! The mud is falling! The mud is falling! Oh wait, you aren't the leader of a clan.", ch);
   return;
  }
}
/* Demote a clan member */
ACMD(do_demote)
{
  struct char_data *victim;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  half_chop(argument, buf, buf1);
  if (!CLN_FLAGGED(ch, CLN_LEADER1) && !CLN_FLAGGED(ch, CLN_LEADER2) && !CLN_FLAGGED(ch, CLN_LEADER3) && !CLN_FLAGGED(ch, CLN_LEADER4) && !CLN_FLAGGED(ch, CLN_LEADER5) && !CLN_FLAGGED(ch, CLN_LEADER6) && !CLN_FLAGGED(ch, CLN_LEADER7) && !CLN_FLAGGED(ch, CLN_LEADER8)) {
   send_to_char("You are not the leader of a clan...\r\n", ch);
   return;
  }
  if (!*buf) {
   send_to_char("Who do you wish to demote?\r\n", ch);
   return;
  }
  if (!(victim = get_char_room_vis(ch, buf))) {
      send_to_char("Deomote who?\r\n", ch);
      return;
  }
  if (victim == ch) {
   send_to_char("You do not need to demote yourself...\r\n", ch);
   return;
  }
  if (!*buf1) {
   send_to_char("You need to specify a clan rank to make them.\r\nRanks: Recruit, Member, Adept, Captain.\r\n", ch);
   return;
  }
  if (!CLN_FLAGGED(victim, CLN_NEWBIE) && !CLN_FLAGGED(victim, CLN_NOVICE) && !CLN_FLAGGED(victim, CLN_MEMBER) && !CLN_FLAGGED(victim, CLN_CAPTAIN) && !CLN_FLAGGED(victim, CLN_LEADER1) && !CLN_FLAGGED(victim, CLN_LEADER2) && !CLN_FLAGGED(victim, CLN_LEADER3) && !CLN_FLAGGED(victim, CLN_LEADER4) && !CLN_FLAGGED(victim, CLN_LEADER5) && !CLN_FLAGGED(victim, CLN_LEADER6) && !CLN_FLAGGED(victim, CLN_LEADER7) && !CLN_FLAGGED(victim, CLN_LEADER8)){
   send_to_char("They are not in a clan!\r\n", ch);
   return;
  }
  if (!str_cmp(buf1,"newbie") && CLN_FLAGGED(victim, CLN_NEWBIE)) {
   send_to_char("They are already that rank.\r\n", ch);
   return;
  }
  else if (!str_cmp(buf1,"newbie")) {
   send_to_char("They are have been demoted.\r\n", ch);
   send_to_char("You have been demoted.\r\n", victim);
    SET_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
   return;
  }
  if (!str_cmp(buf1,"member") && CLN_FLAGGED(victim, CLN_NOVICE)) {
   send_to_char("They are already that rank.\r\n", ch);
   return;
  }
  else if (!str_cmp(buf1,"member")) {
    send_to_char("They are have been demoted.\r\n", ch);
    send_to_char("You have been demoted.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    SET_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
   return;
  }
  if (!str_cmp(buf1,"adept") && CLN_FLAGGED(victim, CLN_MEMBER)) {
   send_to_char("They are already that rank.\r\n", ch);
   return;
  }
  else if (!str_cmp(buf1,"adept")) {
    send_to_char("They are have been demoted.\r\n", ch);
    send_to_char("You have been demoted.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
   return;
  }
   if (!str_cmp(buf1,"captain") && CLN_FLAGGED(victim, CLN_CAPTAIN)) {
   send_to_char("They are already that rank.\r\n", ch);
   return;
  }
  else if (!str_cmp(buf1,"adept")) {
    send_to_char("They are have been demoted.\r\n", ch);
    send_to_char("You have been demoted.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    SET_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
   return;
  }
}
/* Transport code */
ACMD(do_transport)
{
   one_argument(argument, arg);
   if (!ROOM_FLAGGED(ch->in_room, ROOM_SPIRIT)) {
    send_to_char("You are not near a ship!\r\n", ch);
    return;
   }
   if (CLN_FLAGGED(ch, CLN_Death)) {
    send_to_char("&15The dead are not allowed to transport and leave the afterlife&00\r\n", ch);
    return;
   }
   if (!*argument && GET_LEVEL(ch) <= 9999){
     send_to_char("        &16Class &09B &16Passengers.\r\n", ch);
     send_to_char("&00&06--------&14Planets&00&06--------&00\r\n", ch);
     send_to_char("&10Earth        &113000&00\r\n", ch);
     send_to_char("&10Orainas      &115000&00\r\n", ch);
     send_to_char("&10Vegeta       &114000&00\r\n", ch);
     send_to_char("&10Konack       &112000&00\r\n", ch);
     send_to_char("&10Namek        &111500&00\r\n", ch);
     send_to_char("&10Frigid       &113500&00\r\n", ch);
     return;
    }
   else if (!*argument && GET_LEVEL(ch) <= 19999){
     send_to_char("        &16Class &09A &16Passengers.\r\n", ch);
     send_to_char("&00&06--------&14Planets&00&06--------&00\r\n", ch);
     send_to_char("&10Earth        &116000&00\r\n", ch);
     send_to_char("&10Orainas      &1110000&00\r\n", ch);
     send_to_char("&10Vegeta       &118000&00\r\n", ch);
     send_to_char("&10Konack       &114000&00\r\n", ch);
     send_to_char("&10Namek        &113000&00\r\n", ch);
     send_to_char("&10Frigid       &117000&00\r\n", ch);
     return;
    }
   else if (!*argument && GET_LEVEL(ch) >= 20000){
     send_to_char("        &16Class &09S &16Passengers.\r\n", ch);
     send_to_char("&00&06--------&14Planets&00&06--------&00\r\n", ch);
     send_to_char("&10Earth        &119000&00\r\n", ch);
     send_to_char("&10Orainas      &1115000&00\r\n", ch);
     send_to_char("&10Vegeta       &1112000&00\r\n", ch);
     send_to_char("&10Konack       &116000&00\r\n", ch);
     send_to_char("&10Namek        &114500&00\r\n", ch);
     send_to_char("&10Frigid       &1110500&00\r\n", ch);
     return;
    }
   else if (!str_cmp(arg, "earth") && (SECT(ch->in_room) == SECT_EARTH)){
    send_to_char("You are already on that planet!\r\n", ch);
    return;
   }
   else if (!str_cmp(arg, "earth") && GET_GOLD(ch) <= 2999 && GET_LEVEL(ch) <= 9999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "earth") && GET_GOLD(ch) <= 5999 && GET_LEVEL(ch) >= 10000 && GET_LEVEL(ch) <= 19999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "earth") && GET_GOLD(ch) <= 8999 && GET_LEVEL(ch) >= 20000){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "earth") && GET_GOLD(ch) >= 3000){
    act("&15With ticket in hand, you step up to the ship, board and choose your seat for the ride.&00", FALSE, ch, 0, 0, TO_CHAR);
    act("&15$n boards the ship, with $s ticket in hand.&00", FALSE, ch, 0, 0, TO_ROOM);
    if (GET_LEVEL(ch) <= 9999) {
     GET_GOLD(ch) -= 3000;
    }
    else if (GET_LEVEL(ch) <= 19999) {
     GET_GOLD(ch) -= 6000;
    }
    if (GET_LEVEL(ch) >= 20000) {
     GET_GOLD(ch) -= 9000;
    }
    SET_BIT(PRF2_FLAGS(ch), PRF2_EARTH);
    SET_BIT(PRF2_FLAGS(ch), PRF2_TRANS1);
    char_from_room(ch);
    char_to_room(ch, r_trans_space);
    send_to_char("&16The engines roar as the ship begins to take off.&00\r\n", ch);
    return;
   }
   else if (!str_cmp(arg, "vegeta") && (SECT(ch->in_room) == SECT_VEGETA)){
    send_to_char("You are already on that planet!\r\n", ch);
    return;
   }
   else if (!str_cmp(arg, "vegeta") && GET_GOLD(ch) <= 3999 && GET_LEVEL(ch) <= 9999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "vegeta") && GET_GOLD(ch) <= 7999 && GET_LEVEL(ch) >= 10000 && GET_LEVEL(ch) <= 19999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "vegeta") && GET_GOLD(ch) <= 11999 && GET_LEVEL(ch) >= 20000){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "vegeta") && GET_GOLD(ch) >= 4000){
    act("&15With ticket in hand, you step up to the ship, board and choose your seat for the ride.&00", FALSE, ch, 0, 0, TO_CHAR);
    act("&15$n boards the ship, with $s ticket in hand.&00", FALSE, ch, 0, 0, TO_ROOM);
    if (GET_LEVEL(ch) <= 9999) {
     GET_GOLD(ch) -= 4000;
    }
    else if (GET_LEVEL(ch) <= 19999) {
     GET_GOLD(ch) -= 8000;
    }
    if (GET_LEVEL(ch) >= 20000) {
     GET_GOLD(ch) -= 12000;
    }
    SET_BIT(PRF2_FLAGS(ch), PRF2_VEGETA);
    SET_BIT(PRF2_FLAGS(ch), PRF2_TRANS1);
    char_from_room(ch);
    char_to_room(ch, r_trans_space);
    send_to_char("&16The engines roar as the ship begins to take off.&00\r\n", ch);
    return;
   }
   else if (!str_cmp(arg, "konack") && (SECT(ch->in_room) == SECT_KONACK)){
    send_to_char("You are already on that planet!\r\n", ch);
    return;
   }
   else if (!str_cmp(arg, "konack") && GET_GOLD(ch) <= 1999 && GET_LEVEL(ch) <= 9999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "konack") && GET_GOLD(ch) <= 3999 && GET_LEVEL(ch) >= 10000 && GET_LEVEL(ch) <= 19999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "konack") && GET_GOLD(ch) <= 5999 && GET_LEVEL(ch) >= 20000){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "konack") && GET_GOLD(ch) >= 2000){
    act("&15With ticket in hand, you step up to the ship, board and choose your seat for the ride.&00", FALSE, ch, 0, 0, TO_CHAR);
    act("&15$n boards the ship, with $s ticket in hand.&00", FALSE, ch, 0, 0, TO_ROOM);
    if (GET_LEVEL(ch) <= 9999) {
     GET_GOLD(ch) -= 2000;
    }
    else if (GET_LEVEL(ch) <= 19999) {
     GET_GOLD(ch) -= 4000;
    }
    if (GET_LEVEL(ch) >= 20000) {
     GET_GOLD(ch) -= 8000;
    }
    SET_BIT(PRF2_FLAGS(ch), PRF2_KONACK);
    SET_BIT(PRF2_FLAGS(ch), PRF2_TRANS1);
    char_from_room(ch);
    char_to_room(ch, r_trans_space);
    send_to_char("&16The engines roar as the ship begins to take off.&00\r\n", ch);
    return;
   }
   else if (!str_cmp(arg, "orainas") && (SECT(ch->in_room) == SECT_ORAINAS)){
    send_to_char("You are already on that planet!\r\n", ch);
    return;
   }
   else if (!str_cmp(arg, "orainas") && GET_GOLD(ch) <= 4999 && GET_LEVEL(ch) <= 9999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "orainas") && GET_GOLD(ch) <= 9999 && GET_LEVEL(ch) >= 10000 && GET_LEVEL(ch) <= 19999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "orainas") && GET_GOLD(ch) <= 14999 && GET_LEVEL(ch) >= 20000){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "orainas") && GET_GOLD(ch) >= 5000){
    act("&15With ticket in hand, you step up to the ship, board and choose your seat for the ride.&00", FALSE, ch, 0, 0, TO_CHAR);
    act("&15$n boards the ship, with $s ticket in hand.&00", FALSE, ch, 0, 0, TO_ROOM);
    if (GET_LEVEL(ch) <= 9999) {
     GET_GOLD(ch) -= 5000;
    }
    else if (GET_LEVEL(ch) <= 19999) {
     GET_GOLD(ch) -= 10000;
    }
    if (GET_LEVEL(ch) >= 20000) {
     GET_GOLD(ch) -= 15000;
    }
    SET_BIT(PRF2_FLAGS(ch), PRF2_ORAINAS);
    SET_BIT(PRF2_FLAGS(ch), PRF2_TRANS1);
    char_from_room(ch);
    char_to_room(ch, r_trans_space);
    send_to_char("&16The engines roar as the ship begins to take off.&00\r\n", ch);
    return;
   }
   else if (!str_cmp(arg, "namek") && (SECT(ch->in_room) == SECT_NAMEK)){
    send_to_char("You are already on that planet!\r\n", ch);
    return;
   }
   else if (!str_cmp(arg, "namek") && GET_GOLD(ch) <= 1499 && GET_LEVEL(ch) <= 9999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "namek") && GET_GOLD(ch) <= 2999 && GET_LEVEL(ch) >= 10000 && GET_LEVEL(ch) <= 19999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "namek") && GET_GOLD(ch) <= 4499 && GET_LEVEL(ch) >= 20000){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "namek") && GET_GOLD(ch) >= 1500){
    act("&15With ticket in hand, you step up to the ship, board and choose your seat for the ride.&00", FALSE, ch, 0, 0, TO_CHAR);
    act("&15$n boards the ship, with $s ticket in hand.&00", FALSE, ch, 0, 0, TO_ROOM);
    if (GET_LEVEL(ch) <= 9999) {
     GET_GOLD(ch) -= 1500;
    }
    else if (GET_LEVEL(ch) <= 19999) {
     GET_GOLD(ch) -= 3000;
    }
    if (GET_LEVEL(ch) >= 20000) {
     GET_GOLD(ch) -= 4500;
    }
    SET_BIT(PRF2_FLAGS(ch), PRF2_NAMEK);
    SET_BIT(PRF2_FLAGS(ch), PRF2_TRANS1);
    char_from_room(ch);
    char_to_room(ch, r_trans_space);
    send_to_char("&16The engines roar as the ship begins to take off.&00\r\n", ch);
    return;
   }
   else if (!str_cmp(arg, "frigid") && (SECT(ch->in_room) == SECT_FRIGID)){
    send_to_char("You are already on that planet!\r\n", ch);
    return;
   }
   else if (!str_cmp(arg, "frigid") && GET_GOLD(ch) <= 3499 && GET_LEVEL(ch) <= 9999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "frigid") && GET_GOLD(ch) <= 6999 && GET_LEVEL(ch) >= 10000 && GET_LEVEL(ch) <= 19999){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "frigid") && GET_GOLD(ch) <= 10499 && GET_LEVEL(ch) >= 20000){
    send_to_char("You do not have enough money...", ch);
    return;
   }
   else if (!str_cmp(arg, "frigid") && GET_GOLD(ch) >= 3500){
    act("&15With ticket in hand, you step up to the ship, board and choose your seat for the ride.&00", FALSE, ch, 0, 0, TO_CHAR);
    act("&15$n boards the ship, with $s ticket in hand.&00", FALSE, ch, 0, 0, TO_ROOM);
    if (GET_LEVEL(ch) <= 9999) {
     GET_GOLD(ch) -= 3500;
    }
    else if (GET_LEVEL(ch) <= 19999) {
     GET_GOLD(ch) -= 7000;
    }
    if (GET_LEVEL(ch) >= 20000) {
     GET_GOLD(ch) -= 10500;
    }
    SET_BIT(PRF2_FLAGS(ch), PRF2_FRIGID);
    SET_BIT(PRF2_FLAGS(ch), PRF2_TRANS1);
    char_from_room(ch);
    char_to_room(ch, r_trans_space);
    send_to_char("&16The engines roar as the ship begins to take off.&00\r\n", ch);
    return;
   }
   else {
    send_to_char("That is not a proper place to transport to. Type just transport for a list of transport destinations.\r\n", ch);
   }
}
/* Angel Transformations */
ACMD(do_sangel)
{

 int MAX_MANA = 0, MAX_HIT = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

    one_argument(argument, arg);

    if ((!IS_ANGEL(ch)))
    {
      act("&15You're not an Angel&11!&00", TRUE, ch, 0, 0, TO_CHAR);
      return;
    }
   else if (!*argument)
    {
act("&10o&12---------------&10[&15Super &11A&00&03n&16g&15e&00&07l &09Forms&10]&12---------------&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &15Stage     Powerlevel Req.", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Super Angel first    &1210000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Super Angel second   &1245000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Super Angel Seraphim &1270000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
act("&10o&12---------------&10[&15Super &11A&00&03n&16g&15e&00&07l &09forms&10]&12---------------&10o&00", TRUE, ch, 0, 0, TO_CHAR);
send_to_char("&15All powerlevel req. go off of BASE pl.&00\r\n", ch);
  return;
}

  if (!str_cmp(arg,"second") && !PLR_FLAGGED(ch, PLR_trans1)) {
   act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
  }
  if (!str_cmp(arg,"seraphim") && !PLR_FLAGGED(ch, PLR_trans2)) {
   act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
  }
/*-------------------------------------------------------------------------------------*/
/*----------------------------------  Reverting  --------------------------------------*/

 if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans3)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(65000000, 65000000);
  MAX_MANA = number(65000000, 65000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&15$n reverts from $s form of &16Super &11A&00&03n&16g&15e&00&07l&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &16Super &11A&00&03n&16g&15e&00&07l&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans2)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(40000000, 40000000);
  MAX_MANA = number(40000000, 40000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;

  act("&15$n reverts from $s form of &16Super &11A&00&03n&16g&15e&00&07l&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &16Super &11A&00&03n&16g&15e&00&07l&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
  if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans1)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(20000000, 20000000);
  MAX_MANA = number(20000000, 20000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;

  act("&15$n reverts from $s form of &16Super &11A&00&03n&16g&15e&00&07l&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &16Super &11A&00&03n&16g&15e&00&07l&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

/*============================== Transes ===============================*/

 if (!str_cmp(arg,"first") && PLR_FLAGGED(ch, PLR_trans1)) {
  send_to_char("&15You are already in that form!&00\r\n", ch);
  return;
  }
 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) < 10000000) {
  act("&15You do not have the power to attain that form of &16Super &11A&00&03n&16g&15e&00&07l!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) >= 10000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(20000000, 20000000);
  MAX_MANA = number(20000000, 20000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&16A &00&07w&15h&16i&00&07t&15e &14b&15e&00&06a&14m &16of &11l&00&07i&11g&00&07h&11t &14e&15x&00&06p&00&07l&14o&15d&00&06e&00&07s &15around you and shoots high into the &14s&12k&00&07y. &15A loud &09r&16o&00&01a&15r &15follows the &14b&15e&00&06a&14m &15just before it disappears. Bending over you &15g&00&07r&16o&15w&00&07l &15in agony as your &14w&00&07i&15n&00&06g&15s &15begin to change, their soft &00&07f&15e&00&07a&15t&00&07h&15e&00&07r&15s &15hardening into a &16s&15t&16e&15e&16l &16a&00&07l&15l&16o&07y&15. As your wings finish changing a &11b&00&07r&11i&00&07g&11h&00&07t &15s&00&07i&16l&15v&00&07e&16r &14a&15u&00&06r&14a &16 &15surrounds your body, and you stand up having reached &14S&00&07u&00&06p&14e&00&07r &11A&00&03n&16g&15e&00&07l &14F&15i&00&07r&14s&15t.&00", FALSE, ch, 0, 0, TO_CHAR);
  act("&16A &00&07w&15h&16i&00&07t&15e &14b&15e&00&06a&14m &16of &11l&00&07i&11g&00&07h&11t &14e&15x&00&06p&00&07l&14o&15d&00&06e&00&07s &15around $n &15and shoots high into the&14s&12k&00&07y&15. A loud &09r&16o&00&01a&15r &15follows the &14b&15e&00&06a&14m &15just before it disappears. Bending over $e &15g&00&07r&16o&15w&00&07l &15in agony as $s wings begin to change, their soft &00&07f&15e&00&07a&15t&00&07h&15e&00&07r&15s &15harpening into a &16s&15t&16e&15e&16l &16a&00&07l&15l&16o&07y&15. As $s wings finish changing a &11b&00&07r&11i&00&07g&11h&00&07t &15s&00&07i&16l&15v&00&07e&16r &14a&15u&00&06r&14a &16 &15surrounds $s &15body, and $e stands up having &14S&00&07u&00&06p&14e&00&07r &11A&00&03n&16g&15e&00&07l &14F&15i&00&07r&14s&15t.&00", FALSE, ch, 0, 0, TO_ROOM);
  }

 if (!str_cmp(arg,"second") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15Your already in that form of &16Super &11A&00&03n&16g&15e&00&07l&15!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 20000000 < 45000000) {
  act("&15You do not have the power to attain that form of &16Super &11A&00&03n&16g&15e&00&07l&15!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 20000000 >= 45000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(20000000, 20000000);
  MAX_MANA = number(20000000, 20000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  act("&15A sudden &09s&16h&00&01o&09c&15k&14w&00&07a&00&06v&14e &15rips out from your body as you &14f&15o&00&07c&00&06u&16s &15your inner power. Your wings stretch out, and begin to &09c&15h&00&01a&16n&09g&00&01e&15! The &16s&15t&16e&15e&16l &00&07f&15e&00&07a&15t&00&07h&15e&00&07r&15s  start to turn &00&07s&15h&00&07i&15n&00&07y&15  as they become &15t&00&07i&16t&15a&00&07n&16i&15u&00&07m&15, and the end &00&07f&15e&00&07a&15t&00&07h&15e&00&07r&15s  become &15r&00&07a&16z&15o&00&07r&15 sharp. Your &15s&00&07i&16l&15v&00&07e&16r &14a&15u&00&06r&14a&15 grows &14b&00&07r&16i&00&03g&15h&14t&00&07e&16r, &15and &14l&00&07a&14r&00&06g&14e&00&07r,&15 ballooning out to an &09o&16r&00&01b&15 shape. &16B&00&07o&15l&16t&00&07s &15of &15s&00&07i&16l&15v&00&07e&16r  &11l&00&07i&00&03g&11h&00&07t&00&03n&14i&00&07n&00&03g &15strike within the &09o&16r&00&01b, &15coarsing faster and faster as you reach &14S&00&07u&00&06p&14e&00&07r &11A&00&03n&16g&15e&00&07l &09S&15e&00&07c&09o&15n&00&01d&00", FALSE, ch, 0, 0, TO_CHAR);
  act("&15A sudden &09s&16h&00&01o&09c&15k&14w&00&07a&00&06v&14e &15rips out from $n's body as $e &14f&15o&00&07c&00&06u&16s &15$s inner power. $s wings stretch out, and begin to &09c&15h&00&01a&16n&09g&00&01e&15! The &16s&15t&16e&15e&16l &00&07f&15e&00&07a&15t&00&07h&15e&00&07r&15s  start to turn &00&07s&15h&00&07i&15n&00&07y&15  as they become &15t&00&07i&16t&15a&00&07n&16i&15u&00&07m&15, and the end &00&07f&15e&00&07a&15t&00&07h&15e&00&07r&15s  become &15r&00&07a&16z&15o&00&07r&15 sharp. $s &15s&00&07i&16l&15v&00&07e&16r &14a&15u&00&06r&14a&15 grows &14b&00&07r&16i&00&03g&15h&14t&00&07e&16r, &15and &14l&00&07a&14r&00&06g&14e&00&07r,&15 ballooning out to an &09o&16r&00&01b&15 shape. &16B&00&07o&15l&16t&00&07s &15of &15s&00&07i&16l&15v&00&07e&16r  &11l&00&07i&00&03g&11h&00&07t&00&03n&14i&00&07n&00&03g &15strike within the &09o&16r&00&01b, &15coarsing faster and faster as $e reaches &14S&00&07u&00&06p&14e&00&07r &11A&00&03n&16g&15e&00&07l &09S&15e&00&07c&09o&15n&00&01d&00", FALSE, ch, 0, 0, TO_ROOM);

  }
 if (!str_cmp(arg,"seraphim") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15Your already in that form of &16Super &11A&00&03n&16g&15e&00&07l&15!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"seraphim") && GET_MAX_HIT(ch) - 40000000 < 70000000) {
  act("&15You do not have the power to attain that form of &16Super &11A&00&03n&16g&15e&00&07l&15!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"seraphim") && GET_MAX_HIT(ch) - 40000000 >= 70000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(25000000, 25000000);
  MAX_MANA = number(25000000, 25000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&15You &14f&00&07l&00&06y&15 high into the air, beating your wings with &12r&15a&00&04p&12i&15d &15 speed. As you reach high into the &14a&00&07t&15m&12o&16s&14p&00&07h&15e&12r&16e&15 your &15s&00&07i&16l&15v&00&07e&16r &14a&15u&00&06r&14a &09e&15x&00&01p&16l&09o&15d&00&01e&16s&15 from your body, &09b&15l&09a&16s&09t&15i&09n&16g &15the surrounding air. A &00&07w&15h&16i&00&07t&15e  &14l&00&07i&14&00&07g&14h&00&07t &15covers your whole body, obscuring it from view as your wings change into &15r&00&07a&16z&15o&00&07r  &15sharp &11g&00&07o&00&03l&11d.&15 Your eyes &14g&00&07l&14o&00&07w &11g&00&07o&00&03l&11d&15 as a &11g&00&07o&00&03l&11d&00&07e&00&01n &14a&15u&00&06r&14a&15 envelopes your whole body, and the &00&07w&15h&16i&00&07t&15e &14g&00&07l&14o&99&97w &15disappears. You laugh as you realize you have achieved &14S&00&07u&00&06p&14e&00&07r &11A&00&03n&16g&15e&00&07l &14S&16e&00&06r&15a&00p&06h&14i&15m!&00", FALSE, ch, 0, 0, TO_CHAR);
  act("&15$n &14f&00&07l&00&06ies&15 high into the air, beating $s wings with &12r&15a&00&04p&12i&15d &15 speed. As $e reaches high into the &14a&00&07t&15m&12o&16s&14p&00&07h&15e&12r&16e&15 $s &15s&00&07i&16l&15v&00&07e&16r &14a&15u&00&06r&14a &09e&15x&00&01p&16l&09o&15d&00&01e&16s&15 from $s body, &09b&15l&09a&16s&09t&15i&09n&16g &15the surrounding air. A &00&07w&15h&16i&00&07t&15e  &14l&00&07i&14&00&07g&14h&00&07t &15covers $s whole body, obscuring it from view as $s wings change into &15r&00&07a&16z&15o&00&07r  &15sharp &11g&00&07o&00&03l&11d.&15 $s eyes &14g&00&07l&14o&00&07w &11g&00&07o&00&03l&11d&15 as a &11g&00&07o&00&03l&11d&00&07e&00&01n &14a&15u&00&06r&14a&15 envelopes $s whole body, and the &00&07w&15h&16i&00&07t&15e &14g&00&07l&14o&99&97w &15disappears. $e laughs as $e realize $e has achieved &14S&00&07u&00&06p&14e&00&07r &11A&00&03n&16g&15e&00&07l &14S&16e&00&06r&15a&00p&06h&14i&15m!&00", FALSE, ch, 0, 0, TO_ROOM);
  }
 return;
}
/* In game calculator with simple calculation functions */
ACMD(do_calc)
{
 int value1 = 0, value2 = 0, pro = 0, pro2 = 0, pro3 = 0;

  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

 /*half_chop(argument, buf, buf1);*/
   three_arguments(argument, arg, buf, buf1);
   value1 = atoi(buf);
   value2 = atoi(buf1);
  
  if (!*arg) {
   send_to_char("&15You can subtract, add, multiply, or divide any whole number with this mud calculator. You can also figure up how much is charged in a certain number of posts by exchanging the add/subtract/multiply/divide with the size of the skill such as small/medium/large/massive. Then after that would go your maxki at the time of charging, and then the number of posts charged. You will get the answer.&00\r\n", ch);
   return;
  }
  if (!*buf) {
   send_to_char("&11Syntax&16: &16(&14multiply&16/&14divide&16/&14subtract&16/&14add&16/&14small&16/&14medium&16/&14large&16/&14massive&&16) &16(&14number&16) &16(&14number&16)\r\n&12Example&16: &15divide 44 4\r\n", ch);
   return;
  }
  if (!*buf1) {
  send_to_char("&11Syntax&16: &16(&14multiply&16/&14divide&16/&14subtract&16/&14add&16) &16(&14number&16) &16(&14number&16)\r\n&12Example&16: &15divide 44 4\r\n", ch);
   return;
  }
  if (!str_cmp(arg,"multiply")) {
   pro = value1 * value2;
   sprintf(buf2, "&16[&14%d &09x &14%d &12= &14%d&16]&00\r\n", value1, value2, pro);
   send_to_char(buf2, ch);
   return;
  }
  else if (!str_cmp(arg,"add")) {
   pro = value1 + value2;
   sprintf(buf2, "&16[&14%d &09+ &14%d &12= &14%d&16]&00\r\n", value1, value2, pro);
   send_to_char(buf2, ch);
   return;
  }
  else if (!str_cmp(arg,"subtract")) {
   pro = value1 - value2;
   sprintf(buf2, "&16[&14%d &09- &14%d &12= &14%d&16]&00\r\n", value1, value2, pro);
   send_to_char(buf2, ch);
   return;
  }
  else if (!str_cmp(arg,"divide")) {
   pro = value1 / value2;
   sprintf(buf2, "&16[&14%d &09/ &14%d &12= &14%d&16]&00\r\n", value1, value2, pro);
   send_to_char(buf2, ch);
   return;
  }
  else if (!str_cmp(arg,"small")) {
   pro = value1 / 100;
   pro2 = pro * 10 / 100;
   pro3 = pro2 * 50 * value2;
   sprintf(buf2, "&16[&14Maxki&11: &09%d &14Posts&11: &09%d &14Total&11: &09%d&16]\r\n&00", value1, value2, pro3);
   send_to_char(buf2, ch);
   return;
  }
  else if (!str_cmp(arg,"medium")) {
   pro = value1 / 100;
   pro2 = pro * 20 / 100;
   pro3 = pro2 * 20 * value2;
   sprintf(buf2, "&16[&14Maxki&11: &09%d &14Posts&11: &09%d &14Total&11: &09%d&16]&00\r\n", value1, value2, pro3);
   send_to_char(buf2, ch);
   return;
  }
  else if (!str_cmp(arg,"large")) {
   pro = value1 / 100;
   pro2 = pro * 40 / 100;
   pro3 = pro2 * 10 * value2;
   sprintf(buf2, "&16[&14Maxki&11: &09%d &14Posts&11: &09%d &14Total&11: &09%d&16]&00\r\n", value1, value2, pro3);
   send_to_char(buf2, ch);
   return;
  }
  else if (!str_cmp(arg,"massive")) {
   pro = value1 / 100;
   pro2 = pro * 60 / 100;
   pro3 = pro2 * 5 * value2;
   sprintf(buf2, "&16[&14Maxki&11: &09%d &14Posts&11: &09%d &14Total&11: &09%d&16]&00\r\n", value1, value2, pro3);
   send_to_char(buf2, ch);
   return;
  }
}
/* Used for making wishes on dragonballs. It follows a precise mathmatical formula
* You only have yourself to blame if you mess with the formula or don't build your
* dragonball set correctly.
*/
ACMD(do_wish)
{
   struct obj_data *obj, *next_obj;
   int num = 0;

  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
   one_argument(argument, arg);
   if (!*arg) {
   send_to_char("&16[&09Power &10:: &11800k - 1.2mil&16]\r\n", ch);
   send_to_char("&16[&09Spirit&10:: &11800k - 1.2mil&16]\r\n", ch);
   send_to_char("&16[&09Zenni &10:: &115mil - 8mil  &16]\r\n", ch);
   send_to_char("&16[&09Con   &10:: &11     1       &16]\r\n", ch);
   send_to_char("&16[&09Int   &10:: &11     1       &16]\r\n", ch);
   send_to_char("&16[&09Wis   &10:: &11     1       &16]\r\n", ch);

   return;
   }
    for (obj = ch->carrying; obj; obj = next_obj) {
          next_obj = obj->next_content;
    if (GET_OBJ_TYPE(obj) == ITEM_dball1) {
     num += 3;
     continue;
    }
    if (GET_OBJ_TYPE(obj) == ITEM_dball2) {
     num += 1;
     continue;
    }
    if (GET_OBJ_TYPE(obj) == ITEM_dball3) {
     num += 7;
     continue;
    }
    if (GET_OBJ_TYPE(obj) == ITEM_dball4) {
     num += 11;
     continue;
    }
    if (GET_OBJ_TYPE(obj) == ITEM_dball5) {
     num += 13;
     continue;
    }
    if (GET_OBJ_TYPE(obj) == ITEM_dball6) {
     num += 15;
     continue;
    }
    if (GET_OBJ_TYPE(obj) == ITEM_dball7) {
     num += 17;
     continue;
    }
    continue;
   }
   if (!str_cmp(arg,"power") && num == 67) {
     send_to_char("&15Lightning fills the sky as clouds form. The dragon balls flash omniously just before a streak of lighting hits all seven at once. Shenron the great dragon blasts out of the dragonballs, it's form filling the sky. After a few moments of sheer awe you manage to ask for your wish.\r\n", ch);
     sprintf(buf, "&16[&14WISH&16] &12%s &15has been granted a wish!&00\r\n", GET_NAME(ch));
     send_to_all(buf);
     for (obj = ch->carrying; obj; obj = next_obj) {
          next_obj = obj->next_content;
      if (!IS_NPC(ch)) {
       if (GET_OBJ_TYPE(obj) == ITEM_dball1) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball2) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball3) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball4) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball5) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball6) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball7) {
       extract_obj(obj);
       }
       continue;
      }
     }
     GET_MAX_HIT(ch) += number(800000, 1200000);
     return;
     }
     if (!str_cmp(arg,"spirit") && num == 67) {
     send_to_char("&15Lightning fills the sky as clouds form. The dragon balls flash omniously just before a streak of lighting hits all seven at once. Shenron the great dragon blasts out of the dragonballs, it's form filling the sky. After a few moments of sheer awe you manage to ask for your wish.\r\n", ch);  
     sprintf(buf, "&16[&14WISH&16] &12%s &15has been granted a wish!&00\r\n", GET_NAME(ch));
     send_to_all(buf);
     for (obj = ch->carrying; obj; obj = next_obj) {
          next_obj = obj->next_content;
      if (!IS_NPC(ch)) {
       if (GET_OBJ_TYPE(obj) == ITEM_dball1) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball2) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball3) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball4) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball5) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball6) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball7) {
       extract_obj(obj);
       }
       continue;
      }
     }
     GET_MAX_MANA(ch) += number(800000, 1200000);
     return;
     }
     if (!str_cmp(arg,"zenni") && num == 67) {
     send_to_char("&15Lightning fills the sky as clouds form. The dragon balls flash omniously just before a streak of lighting hits all seven at once. Shenron the great dragon blasts out of the dragonballs, it's form filling the sky. After a few moments of sheer awe you manage to ask for your wish.\r\n", ch);
     sprintf(buf, "&16[&14WISH&16] &12%s &15has been granted a wish!&00\r\n", GET_NAME(ch));
     send_to_all(buf);
     for (obj = ch->carrying; obj; obj = next_obj) {
          next_obj = obj->next_content;
      if (!IS_NPC(ch)) {
       if (GET_OBJ_TYPE(obj) == ITEM_dball1) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball2) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball3) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball4) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball5) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball6) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball7) {
       extract_obj(obj);
       }
       continue;
      }
     }
     GET_GOLD(ch) += number(5000000, 8000000);
     return;;
     }
    if (!str_cmp(arg,"con") && num == 67) {
     if (GET_CON(ch) >= 25) {
       send_to_char("Remove your equipment, and you can no longer boost this stat.\r\n", ch);
       return;
     }
     send_to_char("&15Lightning fills the sky as clouds form. The dragon balls flash omniously just before a streak of lighting hits all seven at once. Shenron the great dragon blasts out of the dragonballs, it's form filling the sky. After a few moments of sheer awe you manage to ask for your wish.\r\n", ch);
     sprintf(buf, "&16[&14WISH&16] &12%s &15has been granted a wish!&00\r\n", GET_NAME(ch));
     send_to_all(buf);
     for (obj = ch->carrying; obj; obj = next_obj) {
          next_obj = obj->next_content;
      if (!IS_NPC(ch)) {
       if (GET_OBJ_TYPE(obj) == ITEM_dball1) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball2) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball3) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball4) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball5) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball6) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball7) {
       extract_obj(obj);
       }
       continue;
      }
     }
     ch->real_abils.con += 1;
     return;
     }
    if (!str_cmp(arg,"int") && num == 67) {
     if (GET_INT(ch) >= 25) {
       send_to_char("Remove your equipment, and you can no longer boost this stat.\r\n", ch);
       return;
     }
     send_to_char("&15Lightning fills the sky as clouds form. The dragon balls flash omniously just before a streak of lighting hits all seven at once. Shenron the great dragon blasts out of the dragonballs, it's form filling the sky. After a few moments of sheer awe you manage to ask for your wish.\r\n", ch);
     sprintf(buf, "&16[&14WISH&16] &12%s &15has been granted a wish!&00\r\n", GET_NAME(ch));
     send_to_all(buf);
     for (obj = ch->carrying; obj; obj = next_obj) {
          next_obj = obj->next_content;
      if (!IS_NPC(ch)) {
       if (GET_OBJ_TYPE(obj) == ITEM_dball1) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball2) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball3) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball4) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball5) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball6) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball7) {
       extract_obj(obj);
       }
       continue;
      }
     }
     ch->real_abils.intel += 1;
     return;
     }
    if (!str_cmp(arg,"wis") && num == 67) {
     if (GET_WIS(ch) >= 25) {
       send_to_char("Remove your equipment, and you can no longer boost this stat.\r\n", ch);
       return;
     }
     send_to_char("&15Lightning fills the sky as clouds form. The dragon balls flash omniously just before a streak of lighting hits all seven at once. Shenron the great dragon blasts out of the dragonballs, it's form filling the sky. After a few moments of sheer awe you manage to ask for your wish.\r\n", ch);
     sprintf(buf, "&16[&14WISH&16] &12%s &15has been granted a wish!&00\r\n", GET_NAME(ch));
     send_to_all(buf);
     for (obj = ch->carrying; obj; obj = next_obj) {
          next_obj = obj->next_content;
      if (!IS_NPC(ch)) {
       if (GET_OBJ_TYPE(obj) == ITEM_dball1) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball2) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball3) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball4) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball5) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball6) {
       extract_obj(obj);
       }
       if (GET_OBJ_TYPE(obj) == ITEM_dball7) {
       extract_obj(obj);
       }
       continue;
      }
     }
     ch->real_abils.wis += 1;
     return;
     }
   else {
    send_to_char("You need all seven dragonballs", ch);
     return;
     }
}
/* Used to for killing your character and reaching the AL */
ACMD(do_suicide)
{
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
  if (PLR_FLAGGED(ch, PLR_KILLER)) {
     send_to_char("&15You have a desire to kill all in the arena, suicide is not on your mind&11!&00\r\n", ch);
     return;
    }
  if (CLN_FLAGGED(ch, CLN_Death)) {
    send_to_char("&15You are already dead...&00\r\n", ch);
    return;
   }
  if (ROOM_FLAGGED(ch->in_room, ROOM_NOQUIT))
      {
      send_to_char("You cannot kill yourself here!\r\n", ch);
    return;
     }
  if (GET_GOLD(ch) <= 49999) {
   send_to_char("&15You do not have enough money to ensure you will\r\nkeep your body in the afterlife.&00\r\n", ch);
   return;
  }
  if (GET_GOLD(ch) >= 50000) {
   send_to_char("&15After praying over your money, the gods accept the sacrifice.\r\nThe money disappears in a cloud of smoke. Smiling afterwards, you\r\ndraw your knife and stab yourself in the gut. You slowly bleed to\r\ndeath as you fade from this life.\r\n", ch); 
   GET_GOLD(ch) -= 50000;
   sprintf(buf2, "&16[&09SUICIDE&16] &14%s &15dies, having commited suicide.&00\r\n", GET_NAME(ch));
   send_to_all(buf2);
   char_from_room(ch);
   char_to_room(ch, r_death_start_room);
   SET_BIT(CLN_FLAGS(ch), CLN_Death);
   GET_DTIMER(ch) = 3600;
   GET_POS(ch) = POS_STANDING;
   look_at_room(ch, 0);
   return;
  }
}
/* For trading truth points(quest points, bonus points, whatever you wanna make them)
*/
ACMD(do_trade)
{
  struct char_data *victim;
  int value = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  half_chop(argument, buf, buf1);
  value = atoi(buf1);  
  if (!*buf) {
   send_to_char("Who do you wish to trade Truth Points with?\r\n", ch);
   return;
  }
  if (!(victim = get_char_room_vis(ch, buf))) {
      send_to_char("Trade with who?\r\n", ch);
      return;
  }
  if (victim == ch) {
   send_to_char("You do not need to trade with yourself...\r\n", ch);
   return;
  }
  if (!*buf1) {
   send_to_char("You need to specify an amount to trade.\r\n", ch);
   return;
  }
  if (GET_ALIGNMENT(ch) < value) {
     send_to_char("You do not have that many Truth Points!", ch);
     return;
   }
  if (value <= 0) {
    send_to_char("You must trade at least 1 tp, are you mad?", ch);
    return;
  }
  if (GET_ALIGNMENT(ch) >= value) {
   sprintf(buf2, "&16[&10TP&16] &14%s &15has traded &09%s &12%d &15Truth Points&00\r\n", GET_NAME(ch), GET_NAME(victim), value);
   send_to_all(buf2);
   GET_ALIGNMENT(ch) -= value;
   GET_ALIGNMENT(victim) += value;
    if (GET_ALIGNMENT(ch) < 0) {
     GET_ALIGNMENT(ch) = 0;
    }
   return;
  }
}
/* For changing password */
ACMD(do_password)
{

  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
  one_argument(argument, arg);
  
   if (IS_NPC(ch)) {
     send_to_char("Mobs do not HAVE passwords you crazy digital being.\r\n", ch);
     return;
   }
   if (!*arg) {
     sprintf(buf, "&15Your current password is &16[&11%s&16]&00\r\n", GET_PASSWD(ch));
     send_to_char(buf, ch);
     return;
    }
  else if (GET_LEVEL(ch) >= 2 && !IS_NPC(ch)) {
   strncpy(GET_PASSWD(ch), CRYPT(arg, GET_NAME(ch)), MAX_PWD_LENGTH);
   *(GET_PASSWD(ch) + MAX_PWD_LENGTH) = '\0';
   sprintf(buf, "&15Password changed to &16[&11%s&16]&00", GET_PASSWD(ch));
   send_to_char(buf, ch);
  }
}
/* For bountying players */
ACMD(do_bounty)
{
  struct char_data *victim;
  struct descriptor_data *i;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

    one_argument(argument, arg);

    if (!*arg) {
      send_to_char("Who is it that you wish to place a bounty on?\r\n", ch);
      return;
    }
   if (PRF_FLAGGED(ch, PRF_NOGRATZ)) {
       send_to_char("&15You are in hell, no bounty for you!&00\r\n", ch);
       return;
      }
   if (IS_NPC(ch)) {
     send_to_char("You are a mob, get real.\r\n", ch);
     return;
    }
    for (i = descriptor_list; i; i = i->next) {
      if (!i->connected && i->character && i->character != ch) {
        victim = i->character;
        if (!(victim = get_char_vis(ch, arg))) {
          send_to_char(NOPERSON, ch);
          return;
        }
        if (victim == ch) {
          send_to_char("You want to place a bounty on yourself? Nah.\r\n", ch);
          return;
        }
        if (GET_LEVEL(victim) >= LVL_IMMORT) {
          send_to_char("Let's just call that person 'priviledged', okay?\r\n", ch);
          return;
        }
        if (IS_NPC(victim)) {
          send_to_char("Not on mobs!", ch);
          return;
        }
        if (GET_MAX_HIT(victim) <= 4999999 && !IS_NPC(ch)) {
          send_to_char("You can't place bounties on players under power"
                       "level of 5mil.\r\n", ch);
          return;
        }
        if (GET_MAX_HIT(ch) <= 4999999 && !IS_NPC(ch)) {
          send_to_char("You can't place bounties on players when under power"
                       "level of 5mil.\r\n", ch);
          return;
        }
        if (GET_GOLD(ch) < 1000000) {
          send_to_char("You do not have the funds to place a bounty upon them.\r\n"
                       "The cost to place a bounty is 1,000,000 gold.\r\n", ch);
          return;
        }
        if (PLR_FLAGGED(victim, PLR_icer)) {
          send_to_char("That person already has a bounty on them.\r\n", ch);
          return;
        } else
          SET_BIT(PLR_FLAGS(victim), PLR_icer);
          GET_GOLD(ch) = GET_GOLD(ch) - 1000000;
          send_to_char("You pay the 1,000,000 gold to place the bounty.\r\n", ch);
          send_to_char("A bounty has been placed upon your head - watch your "
                       "back!\r\n", victim);
          sprintf (buf, "\r\n&14BOUNTY&11::&15 A bounty has been placed upon &09%s&15's "
                        "life by &12%s&15!\r\n", GET_NAME(victim), GET_NAME(ch));
          send_to_all(buf);
          return;
       }
     }
   return;
}


/* Quote channel */
ACMD(do_quote)
{
   struct descriptor_data *pt;
    skip_spaces(&argument);
    delete_doubledollar(argument);
    DYN_DEFINE;
    DYN_CREATE;
    *dynbuf = 0;

        for (pt = descriptor_list; pt; pt = pt->next){
                if (STATE(pt) == CON_PLAYING && pt->character && !IS_NPC(pt->character)){
              if (!*argument) {
              send_to_char("What do you want to say on the quote channel???\r\n", ch);
              return;
              }
              if (GET_MAX_HIT(ch) <= 999) {
              send_to_char("You must be at least 1k powerlevel to use this channel.", ch);
              return;
              }
              if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF)) {
               send_to_char("The walls absorb your words!", ch);
               return;
              }
              if (PLR_FLAGGED(ch, PLR_NOSHOUT)) {
               send_to_char("You can't use this channel when muted!\r\n", ch);
               return;
              }
                if (GET_ADD(ch) >= 5 && GET_LEVEL(ch) <= 999999) {
                 sprintf(buf2, "&16[&14FROZEN&16] &12%s &15has been frozen for spamming, what a dumbass&00.\r\n", GET_NAME(ch));
                 send_to_all(buf2);
                 SET_BIT(PLR_FLAGS(ch), PLR_FROZEN);
                 GET_FREEZE_LEV(ch) = 1000000;
                 GET_ADD(ch) = 0;
                 return;
                }
              else {
               sprintf(buf, "&15%s &16(&10Quote&16)&00&07's, '&15%s&00&07'&00\r\n", GET_NAME(ch), argument);
               if (!ROOM_FLAGGED(pt->character->in_room, ROOM_SOUNDPROOF) && GET_NAME(pt->character) != GET_NAME(ch)) {
                send_to_char(buf, pt->character);
               }
              }

   }
  }
               sprintf(buf1, "&15You &16(&10Quote&16)&00&07, '&15%s&00&07'&00\r\n", argument);
               send_to_char(buf1, ch);
            if (GET_LEVEL(ch) <= 999999) {
               GET_ADD(ch) += 1;
            }

}
/* Roleplay channel */
ACMD(do_roleplay)
{
   struct descriptor_data *pt;
    skip_spaces(&argument);
    delete_doubledollar(argument);
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
        for (pt = descriptor_list; pt; pt = pt->next){
                if (STATE(pt) == CON_PLAYING && pt->character && !IS_NPC(pt->character)){
              if (!*argument) {
              send_to_char("What do you want to say on your race channel???\r\n", ch);
              return;
              }
              if (GET_MAX_HIT(ch) <= 999) {
              send_to_char("You must be at least 1k powerlevel to use this channel.", ch);
              return;
              }
              if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF)) {
               send_to_char("The walls absorb your words!", ch);
               return;
              }
              if (PLR_FLAGGED(ch, PLR_NOSHOUT)) {
               send_to_char("You can't use this channel when muted!\r\n", ch);
               return;
              }
              if (!PRF_FLAGGED(ch, PRF_QUEST)) {
               send_to_char("You need to type rp to join the channel first!", ch);
               return;
              }
              if (PRF_FLAGGED(ch, PRF_NOGOSS)) {
               send_to_char("You have no roleplay turned on!", ch);
               return;
              }
              else {
               sprintf(buf, "&15%s &16(&14Roleplay&16)&00&07's, '&15%s&00&07'&00\r\n", GET_NAME(ch), argument);
               if (!ROOM_FLAGGED(pt->character->in_room, ROOM_SOUNDPROOF) && PRF_FLAGGED(pt->character, PRF_QUEST) && !PRF_FLAGGED(pt->character, PRF_NOGOSS) && GET_NAME(pt->character) != GET_NAME(ch)) {
                send_to_char(buf, pt->character);
               }
              }

   }
  }
               sprintf(buf1, "&15You &16(&14Roleplay&16)&00&07's, '&15%s&00&07'&00\r\n", argument);
               send_to_char(buf1, ch);

}
/* Race channel */
ACMD(do_race)
{
   struct descriptor_data *pt;
    skip_spaces(&argument);
    delete_doubledollar(argument);
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
        for (pt = descriptor_list; pt; pt = pt->next){
                if (STATE(pt) == CON_PLAYING && pt->character && !IS_NPC(pt->character)){
              if (!*argument) {
              send_to_char("What do you want to say on your race channel???\r\n", ch);
              return;
              }
              if (GET_MAX_HIT(ch) <= 999) {
              send_to_char("You must be at least 1k powerlevel to use this channel.", ch);
              return;
              }
              if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF)) {
               send_to_char("The walls absorb your words!", ch);
               return;
              }
              if (PLR_FLAGGED(ch, PLR_NOSHOUT)) {
               send_to_char("You can't use this channel when muted!\r\n", ch);
               return;
              }
                if (GET_ADD(ch) >= 5  && GET_LEVEL(ch) <= 999999) {
                sprintf(buf2, "&16[&14FROZEN&16] &12%s &15has been frozen for spamming, what a dumbass&00.\r\n", GET_NAME(ch));
                 send_to_all(buf2);
                 SET_BIT(PLR_FLAGS(ch), PLR_FROZEN);
                 GET_FREEZE_LEV(ch) = 1000000;
                 GET_ADD(ch) = 0;
                 return;
                }
              else {
               sprintf(buf, "&14%s &16(&09race&16)&00&07's, '&15%s&00&07'&00\r\n", GET_NAME(ch), argument);
               if (!ROOM_FLAGGED(pt->character->in_room, ROOM_SOUNDPROOF) && GET_CLASS(pt->character) == GET_CLASS(ch) && GET_NAME(pt->character) != GET_NAME(ch)) {
                send_to_char(buf, pt->character);
               }
              }

   }
  }
               sprintf(buf1, "&15You &16(&09race&16)&00&07's, '&15%s&00&07'&00\r\n", argument);
               send_to_char(buf1, ch);
   if (GET_LEVEL(ch) <= 999999) {
    GET_ADD(ch) += 1;
   }
}
/* Clan Channel, requires scouter clan scouter */
ACMD(do_clan)
{
   struct descriptor_data *pt;
    skip_spaces(&argument);
    delete_doubledollar(argument);
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
        for (pt = descriptor_list; pt; pt = pt->next){
                if (STATE(pt) == CON_PLAYING && pt->character && !IS_NPC(pt->character)){
             if (GET_CLAN(ch) <= 0) {
              send_to_char("You do not have a clan scouter\r\n", ch);
              return;
             }
             if (!*argument) {
              send_to_char("What do you want to say on your clan channel???\r\n", ch);
              return;
              }
                if (GET_ADD(ch) >= 5  && GET_LEVEL(ch) <= 999999) {
                 sprintf(buf2, "&16[&14FROZEN&16] &12%s &15has been frozen for spamming, what a dumbass&00.\r\n", GET_NAME(ch));
                 send_to_all(buf2);
                 SET_BIT(PLR_FLAGS(ch), PLR_FROZEN);
                 GET_FREEZE_LEV(ch) = 1000000;
                 GET_ADD(ch) = 0;
                 return;
                }
             else {
               sprintf(buf, "&14%s &16(&12clan&16)&00&07's, '&10%s&00&07'&00\r\n", GET_NAME(ch), argument);
               if (GET_CLAN(pt->character) == GET_CLAN(ch) && GET_NAME(pt->character) != GET_NAME(ch)) {
                send_to_char(buf, pt->character);
               }
              }
             
   } 
  }
               sprintf(buf1, "&15You &16(&12clan&16)&00&07's, '&10%s&00&07'&00\r\n", argument);
               send_to_char(buf1, ch);
  if (GET_LEVEL(ch) <= 999999) {
   GET_ADD(ch) += 1;
  }
}
/* Scouting someone's power, more efficient than stock method */
ACMD(do_scouter) {

  struct char_data * vict;
  one_argument(argument, arg);
  char maxhit[50], maxmana[50], hit[50], mana[50];
  
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
  if (!GET_EQ(ch, WEAR_SHIELD)) {
    send_to_char("You need to wear a scouter to scout someone's power.\r\n", ch);
    return;
  }

  if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Scout who's power?\r\n", ch);
      return;
    }
  if (IS_ANDROID(vict)) {
   act("Your scouter returns no information on $N.", FALSE, ch, 0, vict, TO_CHAR);
   act("$n tries to scout your power.", FALSE, ch, 0, vict, TO_VICT);
   act("$n tries to scout $N's power and fails.", FALSE, ch, 0, vict, TO_NOTVICT);
    }
  else {
    commafmt(maxhit, sizeof(maxhit), GET_MAX_HIT(vict));
    commafmt(maxmana, sizeof(maxmana), GET_MAX_MANA(vict));
    commafmt(hit, sizeof(hit), GET_HIT(vict));
    commafmt(mana, sizeof(mana), GET_MANA(vict));
act("&10 _____________________     &00", FALSE, ch, 0, vict, TO_CHAR);
act("&10//&20                    &00", FALSE, ch, 0, vict, TO_CHAR);
act("&10|]&20                    &00", FALSE, ch, 0, vict, TO_CHAR);
act("&10|]&20                    &00", FALSE, ch, 0, vict, TO_CHAR);
act("&10|]&20&09Loading&16...       &10   &00", FALSE, ch, 0, vict, TO_CHAR);
act("&10|]&20                    &00", FALSE, ch, 0, vict, TO_CHAR);
act("&10|]&20____________________&00  ", FALSE, ch, 0, vict, TO_CHAR);
act("&09...................... []&00", FALSE, ch, 0, vict, TO_CHAR);
    sprintf(buf, "&10PL&15: &11%s&15/&11%s&00\r\n", hit, maxhit);
     send_to_char(buf, ch);
    sprintf(buf, "&10Ki&15: &11%s&15/&11%s&00\r\n", mana, maxmana);
     send_to_char(buf, ch);
   if (!IS_NPC(vict)) {
    sprintf(buf, "&10Str&15: &11%d&15&00\r\n", GET_STR(vict));
     send_to_char(buf, ch);
    sprintf(buf, "&10Con&15: &11%d&15&00\r\n", GET_CON(vict));
     send_to_char(buf, ch);
    sprintf(buf, "&10Int&15: &11%d&15&00\r\n", GET_INT(vict));
     send_to_char(buf, ch);
    sprintf(buf, "&10Wis&15: &11%d&15&00\r\n", GET_WIS(vict));
     send_to_char(buf, ch);
    sprintf(buf, "&10Dex&15: &11%d&15&00\r\n", GET_DEX(vict));
     send_to_char(buf, ch);
    sprintf(buf, "&10Speed&15: &11%d&15&00\r\n", GET_RAGE(vict));
     send_to_char(buf, ch);
    }
   act("&14$n&15 points their scouter at you, and pushes a button.&00\r\n", FALSE, ch, 0, vict, TO_VICT);
   act("&14$n&15 points their scouter at &09$N&15 and pushes a button.&00\r\n", FALSE, ch, 0, vict, TO_NOTVICT);
   WAIT_STATE(ch, PULSE_VIOLENCE * .5);
 }
}
/* Kanassan transes */
ACMD(do_skanassan)
{

int MAX_MANA = 0, MAX_HIT = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
    one_argument(argument, arg);
    WAIT_STATE(ch, PULSE_VIOLENCE * .5);
    if ((!IS_KANASSAN(ch)))
    {
      act("&15You're not a Kanassan&11!&00", TRUE, ch, 0, 0, TO_CHAR);
      return;
    }
   else if (!*argument)
    {
act("&10o&12---------------&11[&14Super Kanassan &09Forms&11]&12---------------&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &15Stage     Powerlevel Req.", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Skanassan first    &1212000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Skanassan second   &1230000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10! Skanassan third    &1245000000 !&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
act("&10o&12---------------&11[&14Super Kanassan &09forms&11]&12---------------&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10All Transes marked with ! are not finished&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
}

if (!str_cmp(arg,"second") && !PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*----------------------------------  Reverting  --------------------------------------*/
/*
 if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans3)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(60000000, 60000000);
  MAX_MANA = number(60000000, 60000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&15$n reverts from $s form of &16SKanassan&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &16SKanassan&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
*/
 if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans2)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(30000000, 30000000);
  MAX_MANA = number(30000000, 30000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&15$n reverts from $s form of &16SKanassan&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &16SKanassan&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans1)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;

  act("&15$n reverts from $s form of &14SKanassan&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &14SKanassan&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"firzt") && PLR_FLAGGED(ch, PLR_trans1)) {
  send_to_char("&15You are already in that form!&00\r\n", ch);
  return;
  }
  if (!str_cmp(arg,"first") && PLR_FLAGGED(ch, PLR_trans1)) {
   act("&15Your already in that form of &16SKanassan!&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
  }
if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) < 12000000) {
  act("&15You do not have the power to attain that form of &16SKanassan!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) >= 12000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&15The area around you begins to &00&03q&16u&15a&00&03k&16e&15, the &00&02e&10a&11r&00&03t&00&02h&15 cracking beneath your feet. You feel your &11a&00&03u&15r&00a&15 explode from your body as if it were part of you. Chunks of &00&03s&16t&15o&00n&03e&15 and rocks float into the air, surrounding your body as a great rushing as heard. Your &14s&00&06c&16al&00&06e&14s&15 begin to grow hard, lengthening and turning a &12d&00&04a&16r&15k &00&02g&10r&15e&10e&00&02n&15 shade. &14W&12a&00&06t&12e&14r&15 begins exploding from the cracks around you as your aura turns into a hazy teal color. You have achieved &11S&00&03u&15p&00&03e&11r &12K&00&04a&00&06n&14as&00&06s&00&04a&12n &16F&15i&00r&14s&00&06t&15!&00", FALSE, ch, 0, 0, TO_CHAR);
  act("&15The area around you begins to &00&03q&16u&15a&00&03k&16e&15, the &00&02e&10a&11r&00&03t&00&02h&15 cracking beneath your feet. Chunks of &00&03s&16t&15o&00n&03e&15 and rocks float into the air, surrounding your body as a great rushing as heard. $n&15's &14s&00&06c&16al&00&06e&14s&15 begin to grow hard, lengthening and turning a &12d&00&04a&16r&15k &00&02g&10r&15e&10e&00&02n&15 shade. &14W&12a&00&06t&12e&14r&15 begins exploding from the cracks around $n&15's as thier aura turns into a hazy teal color. They have achieved &11S&00&03u&15p&00&03e&11r &12K&00&04a&00&06n&14as&00&06s&00&04a&12n &16F&15i&00r&14s&00&06t&15!&00", FALSE, ch, 0, 0, TO_ROOM);
  }

 if (!str_cmp(arg,"second") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15Your already in that form of &16SKanassan!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 15000000 < 30000000) {
  act("&15You do nthe power to attain that form of &16SKanassan!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 15000000 >= 30000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&15Your body screams in &11p&09a&00&01i&03n&15, sending you to your knees. Your &14s&00&06c&16al&00&06e&14s&15 lengthen and sharpen, while turning a dark o&16ny&15x shade. Your &14s&00&06c&16al&00&06e&14s&15 turn hard, like armor plating, as the ground beneath you begins to &00&03q&16u&15a&00&03k&16e&15. The ground breaks away as you stand, a large, sharp &10f&00&06i&10n&15 exploding from your back as a &00g&14e&00&06ys&14e&00r&15 of &14w&12a&00&06t&12e&14r&15 surrounds you, crackling with &16b&15l&00a&16c&15k &00el&15e&11c&00&03tri&11c&15i&00ty&15. You have achieved &11S&00&03u&15p&00&03e&11r &12K&00&04a&00&06n&14as&00&06s&00&04a&12n &10S&00&02e&00&06co&00&02n&10d&15!&00", FALSE, ch, 0, 0, TO_CHAR);
  act("&15$n&15's body starts to crackle with &00e&14n&00&06er&14g&00y&15, a scream escaping them as they  fall to thier knees. You notice thier scales change and grow, turning a dark shade. The scales turn shiny as the ground begins to shake beneath your feet. As $n begins to stand, the ground breaks away around them and a massive  &00g&14e&00&06ys&14e&00r&15 of &14w&12a&00&06t&12e&14r&15 explodes from the ground, crackling with &16b&15l&00a&16c&15k &00el&15e&11c&00&03tri&11c&15i&00ty&15. They have achieved &11S&00&03u&15p&00&03e&11r &12K&00&04a&00&06n&14as&00&06s&00&04a&12n &10S&00&02e&00&06co&00&02n&10d&15!&00", FALSE, ch, 0, 0, TO_ROOM);
  return;
 }
/*
 if (!str_cmp(arg,"third") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15Your already in that form of &16SKanassan!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 30000000 < 45000000) {
  act("&15You do nthe power to attain that form of &16SKanassan!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 30000000 >= 45000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(30000000, 30000000);
  MAX_MANA = number(30000000, 30000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("", TRUE, ch, 0, 0, TO_CHAR);
  act("", TRUE, ch, 0, 0, TO_CHAR);
*/
}
/* Sets training flag, which shuts off some channels */
ACMD(do_training) { 


  one_argument(argument, arg);
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (!*argument) {
    send_to_char("&15Training Flags you on who so people know you are training and do not wish to be bothered.\r\n&11Syntax&16: &15training &00&06(&14yes&00&06/&14no&00&06)&00\r\n", ch);
    return;
   }
  if (!IS_NPC(ch)) {
  if (!str_cmp(arg,"yes") && !PRF_FLAGGED(ch, PRF_TRAINING)) {
     send_to_char("&15You announce you are ready to train hard!&00", ch);
     SET_BIT(PRF_FLAGS(ch), PRF_TRAINING);
     return;
    }
  else if (!str_cmp(arg,"no") && PRF_FLAGGED(ch, PRF_TRAINING)) {
     send_to_char("&15You remove the training flag.&00", ch);
     REMOVE_BIT(PRF_FLAGS(ch), PRF_TRAINING);
     return;    
    }
   }
}
/* Lets player see score of target */
ACMD(do_mindread)
{
   struct char_data * vict;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

   if (!GET_SKILL(ch, SKILL_MINDREAD)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
   }
   if (GET_MANA(ch) < GET_MAX_MANA(ch) / 30) {
    send_to_char("&15Not enough ki to mind read right now....&00", ch);
    return;
   }
   one_argument(argument, arg);
   
   if (!*argument) {
   send_to_char("&15You need to target a player to mindread.", ch);
   return;
   }

   if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Read whom's mind?\r\n", ch);
      return;
    }
   else
   if (vict == ch) {
    send_to_char("Just type score for yourself....\r\n", ch);
    return;
   }
   if (IS_KAI(vict)) {
    send_to_char("You can't read a kai.\r\n", ch);
    return;
    }
   if (IS_NPC(vict)) {
    send_to_char("You can not use that on mobiles!", ch);
    return;
   }
   if (GET_INT(vict) >= GET_INT(ch) * 2) {
    sprintf(buf2, "&14%s &15just read your mind, and you managed a glimpse into their's.&00\r\n", GET_NAME(ch));
    sprintf(buf1, "&11Gold&15: &15%d &16| &10Lp&15: %d &16| &12Intelligence&15: %d &16| &09Constitution&15: %d &16| &14Wisdom&15: %d&00\r\n", GET_GOLD(ch), GET_PRACTICES(ch), GET_INT(ch), GET_CON(ch), GET_WIS(ch));
    send_to_char(buf2, vict);
    send_to_char(buf1, vict);
   }
   else if (GET_INT(vict) >= GET_INT(ch) && GET_INT(vict) < GET_INT(ch) * 2) {
   sprintf(buf1, "&14%s &15just read your mind....&00\r\n", GET_NAME(ch));
    send_to_char(buf1, vict);
   }
   else if (GET_INT(vict) < GET_INT(ch) && GET_INT(vict) >= GET_INT(ch) / 2) {
   send_to_char("&15Someone just read your mind.&00\r\n", vict);
   }

   sprintf(buf,"                    &16[&14Information for&16:&00 &15%s&16]&00\r\n", GET_NAME(vict));
   sprintf(buf, "%s&15o&00&06=------------------------------------------------------------=&15o&00\r\n", buf); sprintf(buf, "%s&00&06|&10Race:&00 %s     \r\n&00&06|&10Sex: &00 ", buf,
               CLASS_ABBR(vict));
  switch (GET_SEX(vict)) {
    case SEX_MALE: strcat(buf, "&12Male&00\r\n"); break;
    case SEX_FEMALE: strcat(buf, "&13Female&00\r\n"); break;
    case SEX_NEUTRAL: strcat(buf, "&10Neutral&00\r\n"); break;
   }
  if (GET_INT(ch) >= number(1, 40)) {
  sprintf(buf, "%s&00&06|&10Powerlevel&15: &15%d&12/&15%d&00\r\n", buf, GET_HIT(vict), GET_MAX_HIT(vict));
  sprintf(buf, "%s&00&06|&10Ki&15:         &15%d&12/&15%d&00\r\n", buf, GET_MANA(vict), GET_MAX_MANA(vict));
  if (IS_Namek(vict) || IS_KAI(vict) || IS_MAJIN(vict)) {
   sprintf(buf, "%s&00&06|&14Spirit&15:     &15%d&12/&1520000&00\r\n", buf, GET_HOME(vict));
   }
   }
   sprintf(buf, "%s&00&06|&00&06=------------------------------------------------------------=&15o&00\r\n", buf);
  if (GET_INT(ch) >= number(1, 50)) {
  sprintf(buf, "%s&00&06|&12Strength&15:           &00 &16[&15%d&16]&00 &12Dexterity&15:       &00 &16[&15%d&16]&00\r\n", buf, GET_STR(vict), GET_DEX(vict));
  sprintf(buf, "%s&00&06|&12Upper Body Strength&15:&00 &16[&15%d&16]&00 &12Constitution&15:    &00 &16[&15%d&16]&00\r\n", buf, GET_UBS(vict), GET_CON(vict));
  sprintf(buf, "%s&00&06|&12Lower Body Strength&15:&00 &16[&15%d&16]&00 &12Hitroll&15:         &00 &16[&15%d&16]&00\r\n", buf, GET_LBS(vict), (vict)->points.hitroll);
  if (GET_WIS(vict) >= 100)
   sprintf(buf, "%s&00&06|&12Wisdom&15:&00              &16[&15%d&16]&00 &12Damroll&15:         &00 &16[&15%d&16]&00\r\n", buf,  GET_WIS(vict), (vict)->points.damroll);
  else
   sprintf(buf, "%s&00&06|&12Wisdom&15:&00              &16[&15%d&16]&00 &12Damroll&15:         &00 &16[&15%d&16]&00\r\n", buf,  GET_WIS(vict), (vict)->points.damroll); 
  if (GET_INT(vict) >= 100)
   sprintf(buf, "%s&00&06|&12Intelligence&15:&00        &16[&15%d&16]&00 &12Speed&15:           &00 &16[&15%d&16]&00\r\n", buf, GET_INT(vict), GET_RAGE(vict));
  else
    sprintf(buf, "%s&00&06|&12Intelligence&15:&00        &16[&15%d&16]&00 &12Speed&15:            &00 &16[&15%d&16]&00\r\n", buf, GET_INT(vict), GET_RAGE(vict));
  sprintf(buf, "%s&00&06|&12Age&15:&00                 &16[&15%d&16]&00 &09PKs&15:             &00 &16[&15%d&16]&00\r\n", buf, GET_AGE(vict), GET_HEIGHT(vict));
  if (GET_CHA(vict) >= 100)
   sprintf(buf, "%s&00&06|&12Intuition&15:&00           &16[&15%d&16]&00 &00&01P.Deaths&15:        &00 &16[&15%d&16]&00\r\n", buf, GET_CHA(vict), GET_WEIGHT(vict));
  else
   sprintf(buf, "%s&00&06|&12Intuition&15:&00           &16[&15%d&16]&00 &00&01P.Deaths&15:        &00 &16[&15%d&16]&00\r\n", buf, GET_CHA(vict), GET_WEIGHT(vict));
  }
  sprintf(buf, "%s&00&06|&00&06=------------------------------------------------------------=&15o&00\r\n", buf);
  if (GET_INT(ch) >= number(1, 45)) {
  sprintf(buf, "%s&00&06|&14Zenni Carried&15: &00  &16[&11%d&16]&00\r\n", buf, GET_GOLD(vict));
  sprintf(buf, "%s&00&06|&14Zenni In bank&15: &00  &16[&11%d&16]&00\r\n", buf, GET_BANK_GOLD(vict));
  sprintf(buf, "%s&00&06|&14Zenni Total&15: &00    &16[&11%d&16]&00\r\n", buf, GET_BANK_GOLD(vict) + GET_GOLD(vict));
  sprintf(buf, "%s&00&06|&00&06Experience&15:&00      &16[&15%Ld&16]&00\r\n", buf, GET_EXP(vict));
  if (!CLN_FLAGGED(ch, CLN_Death)) {
   sprintf(buf, "%s&00&06|&00&06Experience TNB&15:&00  &16[&15%Ld&16]&00\r\n", buf, GET_LEVEL(vict)*2000);
  }
  if (CLN_FLAGGED(ch, CLN_Death)) {
   sprintf(buf, "%s&00&06|&00&06Experience TNB&15:&00  &16[&15%Ld&16]&00\r\n", buf, GET_LEVEL(vict)*1500);
  }
  /*sprintf(buf, "%s&00&06|&10Reincarnations&15: &00  &16[&11%d&16]&00\r\n", buf, 
GET_REINC(vict));*/
  sprintf(buf, "%s&00&06|&12T&00&07r&00&06u&00&07t&12h &00&05Points&15:&00 &16[&15%d&16]&00      &12Learning &00&06Points&15: &00  &16[&11%d&16]&00\r\n", buf, GET_ALIGNMENT(vict), GET_PRACTICES(vict));
  sprintf(buf, "%s&00&06|&14Years&15: &16[&15%d&16] &14Weeks&15:&16 [&15%d&16] &14Days&15:&16 [&15%d&16] &14Hours&15:&16 [&15%d&16] &14Minutes&15:&16 [&15%d&16]&00\r\n", buf, vict->player.time.played / 31536000, ((vict->player.time.played % 31536000) / 604800), ((vict->player.time.played % 604800) / 86400), ((vict->player.time.played % 86400) / 3600), ((vict->player.time.played % 3600) / 60));
  }
  sprintf(buf, "%s&15o&00&06=------------------------------------------------------------=&15o&00\r\n", buf);
  GET_MANA(ch) -= GET_MAX_MANA(ch) / 30;
  send_to_char(buf, ch);
}


/* This sets whether you will allow yourself to be majinized*/
ACMD(do_majinyes)
{
 one_argument(argument, arg);
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
  
  if (!*argument) {
   send_to_char("&15Type allow yes to allow a majin to majinize you. Be careful though, once majinized there is no going back.&00", ch);
   return;
   }
  if (!str_cmp(arg,"yes") && !PLR_FLAGGED(ch, PLR_MAJINIZE)) {
   send_to_char("&15You now accept majinization!&00", ch);
   REMOVE_BIT(PLR_FLAGS(ch), PLR_NOMAJIN);
   return;
   }
}
/* Used to majinize target */
ACMD(do_majinize)
{
  struct char_data * vict;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (!GET_SKILL(ch, SKILL_MAJIN)) {
   send_to_char("You can't do that!\r\n", ch);
   return;
  }
  one_argument(argument, arg);

  if (!*argument) {
   send_to_char("&15You need to target a player to majinize.", ch);
   return;
   }
   if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Majinize who?\r\n", ch);
      return;
    }
  else
   if (vict == ch) {
    send_to_char("No using it on yourself!\r\n", ch);
    return;
   }
   if (GET_MAX_HIT(vict) >= 15000000) {
    send_to_char("They are too strong to majinize!", ch);
    return;
   }
   if (PLR_FLAGGED(vict, PLR_NOMAJIN)) {
    send_to_char("They are not set to accept majinizing right now.", ch);
    return;
    }
   if (PLR_FLAGGED(vict, PLR_MAJINIZE)) {
    send_to_char("They already are being boosted by a majin!", ch);
    return;
    }
   if (IS_MAJIN(vict)) {
    send_to_char("You can't majinize fellow majins!", ch);
    return;
   }
   if (GET_HOME(ch) <= 4999) {
    send_to_char("You do not have enough spirit to majinize!", ch);
    return;
   }
   if (GET_HOME(ch) >= 5000) {
    act("&15You grin as you hold your hands out towards &14$N&15, &00&05dark &10energy &15flows from your hands and courses through &14$N's &15body. After a moment an &13M&15 appears on their forhead as their power is boosted by your own.&00", FALSE, ch, 0, vict, TO_CHAR);
    act("&14$n &15grins as $e holds $s hands out towards &14$N&15, &00&05dark &10energy &15flows from $n's hands and courses through &10$N's &15body. After a moment an &13M&15 appears on $N's forhead as their power is boosted by &14$n's&15 own.&00", FALSE, ch, 0, vict, TO_NOTVICT);
    act("&14$n &15grins as $e holds $s hands out towards you&15, &00&05dark &10energy &15flows from $n's hands and courses through your &15body. After a moment an &13M&15 appears on your forhead as your power is boosted by &14$n's&15 own.&00", FALSE, ch, 0, vict, TO_VICT);
    SET_BIT(PLR_FLAGS(vict), PLR_MAJINIZE);
    GET_MAX_HIT(vict) += 2000000;
    GET_MAX_MANA(vict) += 2000000;
    GET_HOME(ch) -= 5000;
    return;
    }
}
/* Used to potential release target(provided they are not majinized)*/
ACMD(do_potential)
{

 struct char_data * vict;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (!GET_SKILL(ch, SKILL_POTENTIAL)) {
   send_to_char("You can't do that!\r\n", ch);
   return;
  }

 one_argument(argument, arg);
  
  if (!*argument) {
   send_to_char("&15You need to target a player to increase.", ch);
   return;
   }

  if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Raise who's power?\r\n", ch);
      return;
    }
 else
  if (vict == ch) {
    send_to_char("No using it on yourself!\r\n", ch);
    return;
  }
  if (GET_MAX_HIT(vict) < GET_MAX_HIT(ch) /10) {
    send_to_char("They are too weak!", ch);
    return;
   }
  if (PLR_FLAGGED(vict, PLR_MAJINIZE)) {
    send_to_char("You can't release the potential of a majinized person!", ch);
    return;
    }
  if (IS_ANDROID(vict)) {
    send_to_char("You can't release the potential of a artificial being!", ch);
    return;
   }

  if (GET_HOME(ch) <= 2999) {
   send_to_char("You do not have enough spirit to use this technique! You need at least 1000 spirit per increase", ch);
   return;
   }
  if (GET_MAX_HIT(vict) <= 1999999) {
   send_to_char("They need to grow past 2mil powerlevel before their potential can be released.....\r\n", ch);
   return;
   }

  if (GET_HOME(ch) >= 3000 && GET_MAX_HIT(vict) >= 2000000) {
   act("&15You place your hand on &14$N's &15head, suddenly as you concentrate a beam of light invelopes them. Moments later their newly released power rushes outward.", FALSE, ch, 0, vict, TO_CHAR);
   act("&14$n &15places $s hand on &14$N's &15head, suddenly as $e concentrates a beam of light invelopes $n. Moments later their newly released power rushes outward.", FALSE, ch, 0, vict, TO_NOTVICT); 
   act("&14$n &15places $s hand on your head, suddenly as $e concentrates a beam of light invelopes you. Moments later your newly released power rushes outward.", FALSE, ch, 0, vict, TO_VICT);
   sprintf(buf, "&16[&14Potential &10Release&11: &09Pl&11: &15%d &12Ki&11: &15%d&16]", GET_MAX_HIT(ch) / 100, GET_MAX_MANA(ch) / 100);
   send_to_char(buf, vict);
   GET_MAX_HIT(vict) += GET_MAX_HIT(ch) / 100;
   GET_MAX_MANA(vict) += GET_MAX_MANA(ch) / 100;
   GET_HOME(ch) -= 3000;
   return;
   }
}
/* Feel free to implement this if you wish, it is currently buggy and I
* decided not to implement it instead feeling it wasn't needed for this
* style of mud anyway
*/
/*ACMD(do_reincarnate)
{
 if (GET_LEVEL(ch) >= 1000000) {
   send_to_char("You are an immortal dumbass! No reincarnating!\n", ch);
   return;
  }
 if (IS_saiyan(ch)) {
  send_to_char("Yeah right, don't make us take away your saiyan, ass.\n", ch);
  return;
  }

 one_argument(argument, arg);
  if (PLR_FLAGGED(ch, PLR_trans1) || PRF_FLAGGED(ch, PRF_MYSTIC2)) {
   act("&15You can't reincarnate while in a transformation!!&00\n", TRUE, ch, 0, 0, TO_CHAR);
   return;
  }
  if (!*argument) {
   send_to_char("&15Choose carefully, and type reincarnate plus the race you wish to be. Once you\rdecide, that will be that, no going back.&00\n", ch);
   return;
   }
  if (GET_GOLD(ch) <= 1999999) {
  act("&15You need 2000000 zenni before the gods will hear your plea for reincarnation!&00\n", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 if (!str_cmp(arg,"human") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  if (PLR_FLAGGED(ch, PLR_MAJINIZE)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_MAJINIZE);
    }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_REINC(ch) += 1;
  GET_CLASS(ch) = CLASS_Human;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a\r %s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
 if (!str_cmp(arg,"namek") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  if (PLR_FLAGGED(ch, PLR_MAJINIZE)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_MAJINIZE);
    }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_REINC(ch) += 1;
  GET_CLASS(ch) = CLASS_Namek;
  GET_SEX(ch) = SEX_NEUTRAL;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a \r%s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
 if (!str_cmp(arg,"truffle") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  if (PLR_FLAGGED(ch, PLR_MAJINIZE)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_MAJINIZE);
    }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_REINC(ch) += 1;
  GET_CLASS(ch) = CLASS_TRUFFLE;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a \r%s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
 if (!str_cmp(arg,"kai") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  if (PLR_FLAGGED(ch, PLR_MAJINIZE)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_MAJINIZE);
    }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_REINC(ch) += 1;
  GET_CLASS(ch) = CLASS_KAI;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a \r%s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
 if (!str_cmp(arg,"konatsu") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  if (PLR_FLAGGED(ch, PLR_MAJINIZE)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_MAJINIZE);
    }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_REINC(ch) += 1;
  GET_CLASS(ch) = CLASS_KONATSU;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a \r%s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
 if (!str_cmp(arg,"mutant") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  if (PLR_FLAGGED(ch, PLR_MAJINIZE)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_MAJINIZE);
    }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_REINC(ch) += 1;
  GET_CLASS(ch) = CLASS_MUTANT;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a \r%s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
 if (!str_cmp(arg,"demon") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  if (PLR_FLAGGED(ch, PLR_MAJINIZE)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_MAJINIZE);
    }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_REINC(ch) += 1;
  GET_CLASS(ch) = CLASS_demon;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a \r%s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
 if (!str_cmp(arg,"bioandroid") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  if (PLR_FLAGGED(ch, PLR_MAJINIZE)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_MAJINIZE);
    }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_CLASS(ch) = CLASS_BIODROID;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a \r%s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
 if (!str_cmp(arg,"halfbreed") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  if (PLR_FLAGGED(ch, PLR_MAJINIZE)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_MAJINIZE);
    }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_REINC(ch) += 1;
  GET_CLASS(ch) = CLASS_HALF_BREED;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a \r%s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
 if (!str_cmp(arg,"android") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  if (PLR_FLAGGED(ch, PLR_MAJINIZE)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_MAJINIZE);
    }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_REINC(ch) += 1;
  GET_CLASS(ch) = CLASS_ANDROID;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a \r%s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
 if (!str_cmp(arg,"majin") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_REINC(ch) += 1;
  GET_CLASS(ch) = CLASS_MAJIN;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a \r%s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
 if (!str_cmp(arg,"icer") && GET_GOLD(ch) >= 2000000) {
  send_to_char("&15You feel strange as you are immersed in &00&07light&15!\n", ch);
  GET_PRACTICES(ch) = 0;
   if (GET_WIS(ch) >= 25) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 7;
   }
   else if (GET_WIS(ch) >= 20) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 5;
   }
   else if (GET_WIS(ch) >= 15) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 3;
   }
   else if (GET_WIS(ch) >= 10) {
   GET_PRACTICES(ch) = GET_LEVEL(ch) * 2;
   }
  if (PLR_FLAGGED(ch, PLR_MAJINIZE)) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_MAJINIZE);
    }
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) / 4;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) / 4;
  GET_LEVEL(ch) = GET_LEVEL(ch) / 4;
  GET_EXP(ch) = GET_EXP(ch) / 4;
  GET_GOLD(ch) -= 2000000;
  GET_REINC(ch) += 1;
  GET_CLASS(ch) = CLASS_icer;
  act("&14$n &15is suddenly surrounded by &11bright &00&07light&15, when the light disappears they are gone!&00\n", FALSE, ch, 0, 0, TO_NOTVICT);
  sprintf(buf, "&16[&11Reincarnation&16] &15The gods listened to &14%s &15and have reincarnated them into a \r%s&00\n", GET_NAME(ch), CLASS_ABBR(ch));
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  send_to_all(buf);
  look_at_room(ch, 0);
  return;
 }
}*/
/* For exchanging lp for bonuses */
ACMD(do_lp)
{
  int add_num = 0, num = 0;
  one_argument(argument, arg);
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';  
 
  add_num += number(GET_CHA(ch) * 50, GET_CHA(ch) * 100);
  if (!*arg) {
   send_to_char("&14O&16----------------------------------------&14O\r\n", ch);
   send_to_char("&16|&15Power 100 lp for increase to pl         &16|\r\n", ch);
   send_to_char("&16|&15Spirit 100 lp for increase to ki        &16|\r\n", ch);
   send_to_char("&16|&15All exchange all lp for increase to both&16|\r\n", ch);
   send_to_char("&16|&15Con 100k lp to increase con by 1        &16|\r\n", ch);
   send_to_char("&16|&15Int 100k lp to increase int by 1        &16|\r\n", ch);
   send_to_char("&16|&15Wis 100k lp to increase wis by 1        &16|\r\n", ch);
   send_to_char("&16|&15Intu 100k lp to increase intu by 1      &16|\r\n", ch);
   send_to_char("&16|             &11Syntax                     &16|\r\n", ch);
   send_to_char("&14O&16----------------------------------------&14O\r\n", ch);
   sprintf(buf1, "&15Regular Trade in for &09Power&16/&14Spirit &15between &11%d &15and &11%d&00\r\n", GET_CHA(ch) * 50, GET_CHA(ch) * 100);
   send_to_char(buf1, ch);
   return;
   }
  if (!str_cmp(arg, "all") && GET_PRACTICES(ch) <= 1) {
    act("&15You do not have enough to trade in for pl and ki.&00\r\n", TRUE, ch, 0, 0, TO_CHAR);
    return;
   }
  if (!str_cmp(arg, "power") && GET_PRACTICES(ch) <= 99) {
   act("&15You do not have enough &00&07knowledge&15 to train that&11.&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
  }
  if (!str_cmp(arg, "spirit") && GET_PRACTICES(ch) <= 99) {
   act("&15You do not have enough &00&07knowledge&15 to train that&11.&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
  }
  if (!str_cmp(arg, "all") && GET_PRACTICES(ch) >= 2) {
   num += (GET_CHA(ch) * 2) * GET_PRACTICES(ch);
    sprintf(buf, "&15You exchange &11%d &15of your &10gained &00&07knowledge&15 for &09strength &15and &14spirit&11!\r\n&16[&10%d&16]&00\r\n", GET_PRACTICES(ch), num / 4);
   GET_PRACTICES(ch) -= GET_PRACTICES(ch);   
   GET_MAX_HIT(ch) += num / 4;
   GET_MAX_MANA(ch) += num / 4;
   send_to_char(buf, ch);
   return;
  }
  if (!str_cmp(arg, "power") && GET_PRACTICES(ch) >= 100) {
   sprintf(buf, "&15You exchange &11100 &15of your &10gained &00&07knowledge&15 for &09strength&11!\r\n&16[&10%d&16]&00\r\n", add_num);
   GET_PRACTICES(ch) = GET_PRACTICES(ch) - 100;
   GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + add_num;
   send_to_char(buf, ch);
   return;
  }
  if (!str_cmp(arg, "spirit") && GET_PRACTICES(ch) >= 100) {
   sprintf(buf, "&15You exchange &11100 &15of your &10gained &00&07knowledge&15 for &14spirit&11!\r\n&16[&10%d&16]&00\r\n", add_num);
   GET_PRACTICES(ch) = GET_PRACTICES(ch) - 100;
   GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + add_num;
   send_to_char(buf, ch);
   return;
  }
  if (!str_cmp(arg,"con") && GET_PRACTICES(ch) <= 99999) {
     send_to_char("You do not have enough lp to boost that stat!", ch);
     return;
     }
  if (!str_cmp(arg,"con") && GET_PRACTICES(ch) >= 100000) {
     send_to_char("&15You have learned a new way to use your body, your stats increase!", ch);
     ch->real_abils.con += 1;
     GET_PRACTICES(ch) = GET_PRACTICES(ch) - 100000;
     return;
     }
  if (!str_cmp(arg,"int") && GET_PRACTICES(ch) <= 99999) {
     send_to_char("You do not have enough lp to boost that stat!", ch);
     return;
     }
  if (!str_cmp(arg,"int") && GET_PRACTICES(ch) >= 100000) {
     send_to_char("&15You have learned a new way to use your body, your stats increase!", ch);
     ch->real_abils.intel += 1;
     GET_PRACTICES(ch) = GET_PRACTICES(ch) - 100000;
     return;
     }
  if (!str_cmp(arg,"wis") && GET_PRACTICES(ch) <= 99999) {
     send_to_char("You do not have enough lp to boost that stat!", ch);
     return;
     }
  if (!str_cmp(arg,"wis") && GET_PRACTICES(ch) >= 100000) {
     send_to_char("&15You have learned a new way to use your body, your stats increase!", ch);
     ch->real_abils.wis += 1;
     GET_PRACTICES(ch) = GET_PRACTICES(ch) - 100000;
     return;
     }
  if (!str_cmp(arg,"intu") && GET_PRACTICES(ch) <= 99999) {
     send_to_char("You do not have enough lp to boost that stat!", ch);
     return;
     }
  if (!str_cmp(arg,"intu") && GET_PRACTICES(ch) >= 100000) {
     send_to_char("&15You have learned a new way to use your body, your stats increase!", ch);
     ch->real_abils.cha += 1;
     GET_PRACTICES(ch) = GET_PRACTICES(ch) - 100000;
     return;
     }
  if (auto_save && !IS_NPC(ch)) {
        save_char(ch, NOWHERE);
        Crash_crashsave(ch);
        if (ROOM_FLAGGED(ch->in_room, ROOM_HOUSE_CRASH))
        House_crashsave(GET_ROOM_VNUM(IN_ROOM(ch)));
  }
}
/* Konatasu trans */
ACMD(do_shadow)
{

int MAX_MANA = 0, MAX_HIT = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
    one_argument(argument, arg);

    if ((!IS_KONATSU(ch)))
    {
      act("&15You're not a Konatsu&11!&00", TRUE, ch, 0, 0, TO_CHAR);
      return;
    }
   else if (!*argument)
    {
act("&10o&12---------------&11[&16Shadow &09Forms&11]&12---------------&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &15Stage     Powerlevel Req.", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Shade      &122000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Sillouete  &1220000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Shadow     &1250000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
act("&10o&12---------------&11[&16Shadow &09forms&11]&12---------------&10o&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
}

if (!str_cmp(arg,"shadow") && !PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"sillouete") && !PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*----------------------------------  Reverting  --------------------------------------*/

 if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans3)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(65000000, 65000000);
  MAX_MANA = number(65000000, 65000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&15$n reverts from $s form of &16Shadow&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &16Shadow&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans2)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(30000000, 30000000);
  MAX_MANA = number(30000000, 30000000);
  
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&15$n reverts from $s form of &16Shadow&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &16Shadow&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans1)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
  
  MAX_HIT = number(5000000, 5000000);
  MAX_MANA = number(5000000, 5000000);
  
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;

  act("&15$n reverts from $s form of &16Shadow&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &16Shadow&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 if (!str_cmp(arg,"shade") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15Your already in that form of &16Shadow!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"shade") && GET_MAX_HIT(ch) < 2000000) {
  act("&15You do not have the power to attain that form of &16Shadow!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"shade") && GET_MAX_HIT(ch) >= 2000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(5000000, 5000000);
  MAX_MANA = number(5000000, 5000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  
  act("&16Your body begins to p&00&07h&15a&00&07s&16e out of the material plane briefly, just before &16s&15t&00&07r&16e&00&07a&15k&16s of &15white&16 and &16B&15l&00&07a&15c&16k move up and down your entire body in erattic patterns. Then a &00&04d&12a&16r&15k&16 aura bursts up around your body as you reach S&16h&16a&00&07d&15o&16w S&00&07h&15a&00&07d&16e form!", TRUE, ch, 0, 0, TO_CHAR);
  act("&14$n's &16body begins to p&00&07h&15a&00&07s&16e out of the material plane briefly, just before &16s&15t&00&07r&16e&00&07a&15k&16s of &15white&16 and &16B&15l&00&07a&15c&16k move up and down your entire body in erattic patterns. Then a &00&04d&12a&16r&15k&16 aura bursts up around your body as you reach S&16h&16a&00&07d&15o&16w S&00&07h&15a&00&07d&16e form!", TRUE, ch, 0, 0, TO_ROOM);
  }

 if (!str_cmp(arg,"sillouete") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15Your already in that form of &16Shadow!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 
 if (!str_cmp(arg,"sillouete") && GET_MAX_HIT(ch) - 5000000 < 20000000) {
  act("&15You do not have the power to attain that form of &16Shadow!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 
 if (!str_cmp(arg,"sillouete") && GET_MAX_HIT(ch) - 5000000 >= 20000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(25000000, 25000000);
  MAX_MANA = number(25000000, 25000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&16You &00&04di&12s&16a&15p&00&07p&16e&12a&00&04r&16 instantly leaving a &00&04d&12a&16r&15k&16 form in your wake. &00&06D&14a&16r&15k &00&06l&14i&16g&15h&00&07t&15n&16i&14n&00&06g &16crackles around your shadowy form. Looking at your new form is a strain on the eyes of any viewer, the edges of your body blurred to an almost transparent extent. You have attained &00&04T&12r&16u&15e&16 S&16h&16a&00&07d&15o&16w form!", FALSE, ch, 0, 0, TO_CHAR);
  act("&14$n &00&04di&12s&16a&15p&00&07p&16e&12a&00&04r&16s instantly leaving a &00&04d&12a&16r&15k&16 form in their wake. &00&06D&14a&16r&15k &00&06l&14i&16g&15h&00&07t&15n&16i&14n&00&06g &16crackles around their shadowy form. Looking at their new form is a strain on the eyes of any viewer, the edges of their body blurred to an almost transparent extent. &14$n &16has attained &00&04T&12r&16u&15e&16 S&16h&16a&00&07d&15o&16w form!", FALSE, ch, 0, 0, TO_ROOM);
 return;
  }
 if (!str_cmp(arg,"shadow") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15Your already in that form of &16Shadow!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"shadow") && GET_MAX_HIT(ch) - 30000000 < 50000000) {
  act("&15You do not have the power to attain that form of &16Shadow!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"shadow") && GET_MAX_HIT(ch) - 30000000 >= 50000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(35000000, 35000000);
  MAX_MANA = number(35000000, 35000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&16You &00&04di&12s&16a&15p&00&07p&16e&12a&00&04r&16 instantly leaving a &00&04d&12a&16r&15k&16 form in your wake. &00&06D&14a&16r&15k &00&06l&14i&16g&15h&00&07t&15n&16i&14n&00&06g &16crackles around your shadowy form. Looking at your new form is a strain on the eyes of any viewer, the edges of your body blurred to an almost transparent extent. You have attained &00&04T&12r&16u&15e&16 S&16h&16a&00&07d&15o&16w form!", FALSE, ch, 0, 0, TO_CHAR);
  act("&14$n &00&04di&12s&16a&15p&00&07p&16e&12a&00&04r&16s instantly leaving a &00&04d&12a&16r&15k&16 form in their wake. &00&06D&14a&16r&15k &00&06l&14i&16g&15h&00&07t&15n&16i&14n&00&06g &16crackles around their shadowy form. Looking at their new form is a strain on the eyes of any viewer, the edges of their body blurred to an almost transparent extent. &14$n &16has attained &00&04T&12r&16u&15e&16 S&16h&16a&00&07d&15o&16w form!", FALSE, ch, 0, 0, TO_ROOM);
 return;
  }

}
/* Truffle trans */
ACMD(do_augment)
{

 int MAX_MANA = 0, MAX_HIT = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
    one_argument(argument, arg);
    WAIT_STATE(ch, PULSE_VIOLENCE * .5);
    if ((!IS_TRUFFLE(ch)))
    {
      act("&15You're not a truffle&11!&00", TRUE, ch, 0, 0, TO_CHAR);
      return;
    }
   else if (!*argument)
    {
act("&10o&12---------------&11[&09Augmentation&11]&12---------------&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &15Stage     Powerlevel Req.", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Alpha     &145000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Beta      &1430000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Infusion  &1455000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &12Syntax: augment (type)&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
act("&10o&12---------------&11[&09Augmentation&11]&12---------------&10o&00", TRUE, ch, 0, 0, TO_CHAR);
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2)) {
sprintf(buf, "&15Your base powerlevel &16[&10%d&16]&00\r\n", GET_MAX_HIT(ch) - 15000000);
send_to_char(buf, ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3)) {
sprintf(buf, "&15Your base powerlevel &16[&10%d&16]&00\r\n", GET_MAX_HIT(ch) - 40000000);
send_to_char(buf, ch);
}
if (PLR_FLAGGED(ch, PLR_trans3)) {
sprintf(buf, "&15Your base powerlevel &16[&10%d&16]&00\r\n", GET_MAX_HIT(ch) - 65000000);
send_to_char(buf, ch);
}
  return;
}

/*-------------------------------------------------------------------------------------*/
/*-------------------------------- Augmentation -----------------------------------------*/
/*-------------------------------------------------------------------------------------*/

 if (!str_cmp(arg,"alpha") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15Your already have that augmentation implanted!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"alpha") && GET_MAX_HIT(ch) < 5000000) {
  act("&15You do not have the power to handle that augmentation without dieing!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"alpha") && GET_MAX_HIT(ch) >= 5000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("$n&16 starts to become indistinct as they finish working on themselves. Their body shaking as everything around you with &00e&15l&11e&00&03c&16t&15ro&16n&00&03i&11c&15a&00l&16 and &00m&15e&14c&00&06h&16an&00&06i&14c&15a&00l&16 parts flies at them, and it disappears into their skin! $n&16 looks around, scratching their head at what happened as they achieve their &14A&00&06l&16p&15h&00a&16 transformation!&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&16You finish implimenting the new augmentation to the &12w&00&04e&00&06t&14w&15a&00r&06e&16 in your brain as you realize somethings gone wrong. You feel an overwhelming &10p&00&02o&16w&00&02e&10r&16 overtake your body, as everything &00e&15l&11e&00&03c&16t&15ro&16n&00&03i&11c&15a&00l&16 and &00m&15e&14c&00&06h&16an&00&06i&14c&15a&00l&16 around you sucks into your body. Slowly the items merge into your skin until all of it slowly disappears underneath your flesh, making you wonder what just happened. You have achieved your &14A&00&06l&16p&15h&00a&16 transformation!&00", TRUE, ch, 0, 0, TO_CHAR);
  }

 if (!str_cmp(arg,"beta") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15Your already have that augmentation installed!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"beta") && GET_MAX_HIT(ch) - 15000000 < 30000000) {
  act("&15You do not have the power for that augmentation without dieing!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"beta") && GET_MAX_HIT(ch) - 15000000 >= 30000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(25000000, 25000000);
  MAX_MANA = number(25000000, 25000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("$n&16 wraps their arms around their waist, falling to their knees as a blood curdling scream escapes them. Long &00&06m&16e&15t&00al&15l&16i&00&06c &16s&15p&00ik&15e&16s erupt from all over their flesh, their eyes glowing bright &10g&00&02r&10e&15e&00n&16 as their voice becomes &00m&15e&14c&00&06h&16an&00&06i&14c&15a&00l&16 sounding, as if a computer was talking. They begin to lift up into the air as the &16s&15p&00ik&15e&16s explode, forming metallic orbs that hovers around $n&16's body. The orbs suddenly fly into $n, flowing over their skin and forming &00c&15ir&16c&15ui&00t &10b&00&02o&16a&00&02r&10d&16 like patterns onto their skin. $n&16's body explodes in a &00s&15i&14l&00v&15e&14r&16 &00a&15u&16r&15a&16 as they land onto the ground. $n&16 has achieved their &12B&00&04e&16t&15a&16 Transformation!&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&16You wrap your arms around your waist as you fall to your knees, screaming in &00&01p&09a&15i&00n&16 as &00&06m&16e&15t&00al&15l&16i&00&06c &16s&15p&00ik&15e&16s erupt from all over your skin, your eyes glow a harsh &10g&00&02r&10e&15e&00n&16 as your voice starts to sound something &00m&15e&14c&00&06h&16an&00&06i&14c&15a&00l&16. The &16s&15p&00ik&15e&16s suddenly &15e&09x&00&01p&16l&00&01o&09d&15e&16 and surround your body as it begins to lift into the air, globs of metallic goo sticking to your flesh and slowly seeping in, leaving &00c&15ir&16c&15ui&00t &10b&00&02o&16a&00&02r&10d&16 like markings all over your flesh, except your face and palms. Your &00a&15u&16r&15a&16 explodes with a &00s&15i&14l&00v&15e&14r&16 hue as you land gently back onto the ground. You have achieved your &12B&00&04e&16t&15a&16 transformation!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"infusion") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15Your already have that augmentation installed!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"infusion") && GET_MAX_HIT(ch) - 40000000 < 55000000) {
  act("&15You do not have the power for that augmentation without dieing!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"infusion") && GET_MAX_HIT(ch) - 40000000 >= 55000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(25000000, 25000000);
  MAX_MANA = number(25000000, 25000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("$n&16 seems to root to where their standing, their arms and legs thrusting out to their sides as far as they can go. The markings upon their skin begin to glow a &10n&00&02u&16c&15l&16e&00&02a&10r g&00&02r&10e&15e&00n&16, and the sound of ripping flesh echoes about. $n&16's flesh seemingly melts and pools underneath their feet, revealing &00&06m&16e&15t&00al&15l&16i&00&06c&16 body plating beneath. The &16m&15e&00t&15a&16l seems to melt then puff up, covering $n&16's body in a swirling pool of &00&06c&16h&15ro&16m&00&06e&16. The pool cools rapidly, then cracks, and explodes, throwing shards of metal all around. $n&16's body looks competely different as you feel an immense power flowing from them. They have achieved their &10I&00&02n&16f&15u&16s&00&02e&10d&16 Transformation!&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&16You freeze in place, your limbs throwing themselves to your sides as an unbelieveable &00&01p&09a&15i&00n&16 rips through your body. The markings on your flesh glow a bright, &10n&00&02u&16c&15l&16e&00&02a&10r g&00&02r&10e&15e&00n&16 color, the sound of flesh ripping echoes around you as you realize its your own skin peeling apart. As your flesh pools beneath your feet, &00l&15i&14q&00&06u&14i&15d &16m&15e&00t&15a&16l starts to pool out over your body, coating every inch. As the &16m&15e&00t&15a&16l &16seems to cool, cracks form all over, and in a blinding explosion, the excess &16m&15e&00t&15a&16l flies in all different directions. As the &00s&15t&14e&15a&00m&16 cools from your form, you notice that your hands, chest, and the inside of your legs were remade to look like flesh, while the rest of your skin took a metallic tone to it. You feel an &10u&00&02n&16g&15o&16d&00&02l&10y&16 power resonate from within yourself. You have achieved your &10I&00&02n&16f&15u&16s&00&02e&10d&16 Transformation!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
}
/* Mutant trans */
ACMD(do_mutate)
{

int MAX_MANA = 0, MAX_HIT = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

    one_argument(argument, arg);
    WAIT_STATE(ch, PULSE_VIOLENCE * .5);
    if ((!IS_MUTANT(ch)))
    {
      act("&15You're not a mutant&11!&00", TRUE, ch, 0, 0, TO_CHAR);
      return;
    }
   else if (!*argument)
    {

act("&10o&12---------------&11[&09Mutate&11]&12---------------&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &15Stage     Powerlevel Req.", TRUE, ch, 0, 0, TO_CHAR);
act("    &10First     &126000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Second    &1230000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10Third     &1260000000&00", TRUE, ch, 0, 0, TO_CHAR);
act("    &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
act("&10o&12---------------&11[&09Mutate&11]&12---------------&10o&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
}

if (!str_cmp(arg,"second") && !PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"third") && !PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*----------------------------------  Reverting  --------------------------------------*/
 if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans3)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(80000000, 80000000);
  MAX_MANA = number(80000000, 80000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&15$n reverts from $s form of &09Mutation&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &09Mutation&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans2)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     
  MAX_HIT = number(40000000, 40000000);
  MAX_MANA = number(40000000, 40000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&15$n reverts from $s form of &09Mutation&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &09Mutation&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg, "revert") && PLR_FLAGGED(ch, PLR_trans1)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  
  act("&15$n reverts from $s form of &09Mutation&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &09Mutation&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

/*-------------------------------------------------------------------------------------*/
/*------------------------------------ Mutate -----------------------------------------*/
/*-------------------------------------------------------------------------------------*/

 if (!str_cmp(arg,"first") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15Your already in that form of mutation!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) < 6000000) {
  act("&15You do not have the power to attain that form of mutation!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) >= 6000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&11$n's &15skin begins to harden as their muscles expand, their face begins to take on a &00&01ugly&15 appearance as &10claws&15 grow out of their fingers. Finally their eyes glow &09r&00&01e&09d&15 as their mutation finishes it's work&11!&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&11Your &15skin begins to harden as your muscles expand, your face begins to take on a &00&01ugly&15 appearance as &10claws&15 grow out of your fingers. Finally your eyes glow &09r&00&01e&09d&15 as their mutation finishes it's work&11!&00", TRUE, ch, 0, 0, TO_CHAR);
  }

 if (!str_cmp(arg,"second") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15Your already in that form of Mutation!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 15000000 < 30000000) {
  act("&15You do not have the power to attain that form of Mutation!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 15000000 >= 30000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(25000000, 25000000);
  MAX_MANA = number(25000000, 25000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&11$n &16bends over as &13dark&16 energy flows about them&11!&16 Their muscles bulge uncontrollable, showing sickening detail of their muscle structure as their &10claws&16 grow longer and sharper&11!&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&11You &16bend over as &13dark&16 energy flows about you&11!&16 Your muscles bulge uncontrollably, showing sickening detail of your muscle structure as your &10claws&16 grow longer and sharper&11!&00", TRUE, ch, 0, 0, TO_CHAR);
 return;
  }
 if (!str_cmp(arg,"third") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15Your already in that form of Mutation!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 40000000 < 60000000) {
  act("&15You do not have the power to attain that form of Mutation!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 40000000 >= 60000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(40000000, 40000000);
  MAX_MANA = number(40000000, 40000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&14$n &16growls as $s eyes &10g&10l&00&06o&10w &13d&00&05a&16r&00&05k &16with energy. $s muscles quadruple in size, and spikes grow out of their elbows. &14$n&16 shouts out in pain as spikes pop out of their back completing their transformation to &00&05M&00&01u&16ta&00&01t&00&05e &11Third&00\r\n.", FALSE, ch, 0, 0, TO_ROOM);
  act("&16You growl as your eyes &10g&10l&00&06o&10w &13d&00&05a&16r&00&05k &16with energy. Your muscles quadruple in size, and spikes grow out of your elbows. &16You shout out in pain as spikes pop out of your back completing the transformation to &00&05M&00&01u&16ta&00&01t&00&05e &11Third&00\r\n.", FALSE, ch, 0, 0, TO_CHAR);
 }
}
/* Demon Trans*/
ACMD(do_innerfire)
{
   int MAX_MANA = 0, MAX_HIT = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

    one_argument(argument, arg);
    WAIT_STATE(ch, PULSE_VIOLENCE * .5);
    if ((!IS_demon(ch)))
    {
      act("You're not a Demon!", TRUE, ch, 0, 0, TO_CHAR);
      return;
    }
   else if (!*argument)
    {
act("        &10o&12_&09-------------------------&12_&10o&00\r\n         &09|   &11   InnerFire Forms&00    &09|&00\r\n        &10o&12_&09-------------------------&12_&10o&00\r\n         &09|&10You can do the following:&09|&00\r\n         &09|&09-------------------------&09|&00\r\n         &09|&15Stage:  Power Level Req.:&09|&00 ", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10First       &1410000000&00      &09|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10Second      &1420000000&00     &09|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10Third       &1450000000&00    &09|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10Fourth      &1475000000&00    &09|&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10o&12_&09-------------------------&12_&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
return;
}
/*------------------  Checks to see if already in that stage if evil   -----------------*/


if (!str_cmp(arg,"second") && !PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"third") && !PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"fourth") && !PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*----------------------------------  Reverting  --------------------------------------*/

 if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans4)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans4);

  MAX_HIT = number(75000000, 75000000);
  MAX_MANA = number(75000000, 75000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&15$n reverts from $s form of &09Innerfire&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &09Innerfire&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans3)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(60000000, 60000000);
  MAX_MANA = number(60000000, 60000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&15$n reverts from $s form of &09Innerfire&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &09Innerfire&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans2)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(30000000, 30000000);
  MAX_MANA = number(30000000, 30000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&15$n reverts from $s form of &09Innerfire&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &09Innerfire&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans1)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&15$n reverts from $s form of &09Innerfire&15.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from your form of &09Innerfire&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*------------------------------------ Evil 1 -----------------------------------------*/
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"first") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15Your already in that form of Innerfire!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) < 10000000) {
  act("&15You do not have the power to attain that form of Innerfire!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) >= 10000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&16$n starts to growl a little, a faint &09red &00&01pulse&16 eminating from them.  A flickering &09f&00&01lam&09e&16 glows from underneath their chest, growing from within.  &00&03H&11ea&00&03t&16 &16w&15ave&16s start to gradually rise from their skin, the area around them growing hotter.  The pulse eventually fades, leaving $n with new &09p&16o&09w&16e&09r&16.&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&16You growl as you feel a slight burn in your chest.  The burn increases and grows, &11S&00&03ea&01rin&09g&16 you from the inside.  The incredible &00&01h&11e&00&03a&09t&16 drives your every fiber, increasing your speed and strength to a completely new level, your &09I&00&01nn&09er&00&01f&03i&01re&16 giving you all new &09p&16o&09w&16e&09r&16.", TRUE, ch, 0, 0, TO_CHAR); return;
  }
/*-------------------------------------------------------------------------------------*/
/*------------------------------------ Evil 2 -----------------------------------------*/
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"second") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15Your already in that form of Innerfire!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 10000000 < 20000000) {
  act("&15You do not have the power to attain that form of Innerfire!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 10000000 >= 20000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&16$n roars as they are suddenly encompassed in an &00&02a&16u&15r&00&02a&16 of radiating &00&03h&11ea&00&03t&16, a pulsing under their heart growing,pushing outward and expanding.  The &00&03h&11ea&00&03t&16 &16w&15ave&16s flowing from them intensify as &00&07s&16m&15o&16k&00&07e&16 starts to wisp from the corners of their eyes.  The heat waves flux and recede back inside, the &09p&16o&09w&16e&09r&16 suddenly filling them.&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&16You start to feel a deep pulse beating undernearth your chest, the pressure causing you to roar out with pain.  The pulse grows larger as the &09I&00&01nn&09er&00&01f&03i&01re&16 beats within you, &09b&00&03la&01z&03in&09g&16 with an all new power waiting to be unleashed.  The &09f&00&01ire&09s&16 push against your skin, trying to &00&01e&16xp&15l&16od&00&01e&16 outward, driving you to all new heights of power.&00 ", TRUE, ch, 0, 0, TO_CHAR); return;
  }
/*-------------------------------------------------------------------------------------*/
/*------------------------------------ Evil 3 -----------------------------------------*/
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"third") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15Your already in that form of Innerfire!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 20000000 < 50000000) {
  act("&15You do not have the power to attain that form of Innerfire!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 20000000 >= 50000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(30000000, 30000000);
  MAX_MANA = number(30000000, 30000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&16$n grabs their head as the pulse spreads from their chest to their entire body, transforming itself into a flickering &09f&00&01lam&09e&16, the fire dancing beneath their skin causing &00&07s&16m&15o&16k&00&07e&16  to pour from their skin, shrouding them in waves of heat and haze.  The &00&03h&11ea&00&03t&16 eventually recedes and two &09red&16 eyes, dancing with flames, piercethrough the shroud of smoke.&00 ", TRUE, ch, 0, 0, TO_ROOM);

  act("&16 Your mind is filled with an enormous strain as the &09I&00&01nn&09er&00&01f&03i&01re&16 suddenly bursts through every nerve, filling your entire body with an enormous &09f&00&01lam&09e&16 of power, the fire within you feeding itself into your muscles, forcing them to suddenly &00&01e&16xp&15l&16od&00&01e&16 with new strength.  The flames reach through your entire body, infusing it with energy and pain.  The pain fades away, leaving behind only an all new source of &09p&16o&09w&16e&09r&16.&00", TRUE, ch, 0, 0, TO_CHAR); 
return;
  }
/*-------------------------------------------------------------------------------------*/
/*------------------------------------ Evil 4 -----------------------------------------*/
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"fourth") && PLR_FLAGGED(ch, PLR_trans4)) {
  act("&15Your already in that form of Innerfire!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"fourth") && GET_MAX_HIT(ch) - 45000000 < 75000000) {
  act("&15You do not have the power to attain that form of Innerfire!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"fourth") && GET_MAX_HIT(ch) - 45000000 >= 75000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans4);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&00&01$n trembles as the &09I&00&01nn&09er&00&01f&03i&01re that had been contained within them for so long finally bursts from within, exploding outwards in a fury of &09I&00&01nf&16e&00&01rn&09o&00&01.  Your body is suddenly enveloped in &11s&00&03ea&01rin&09g&00&01 heat as the true might of demons is unleashed from within them, the flames bursting from a new &16A&15v&00&01at&15a&16r&00&01 of &09H&00&01e&16ll.&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&00&01A torrent of power suddenly erupts from your &09I&00&01nn&09er&00&01f&03i&01re, the raging &09I&00&01nf&16e&00&01rn&09o of &16P&09o&16w&09e&16r&00&01 bursting past all restrictions, &00&01ex&16pl&15o&16di&00&01ng out from your skin enshrouding your entire being in a raging dance of flames, the ultimate power of the demons infusing themselves into your very being as you become a true &16A&15v&00&01at&15a&16r&00&01 of &09H&00&01e&16ll. &00", TRUE, ch, 0, 0, TO_CHAR); return;
  }
}
/* Quit code, obviously*/
ACMD(do_quit)
{
  sh_int save_room;
  struct descriptor_data *d, *next_d;
  struct obj_data *obj;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (IS_NPC(ch) || !ch->desc)
    return;
  if (PLR_FLAGGED(ch, PLR_KILLER)) {
     send_to_char("&15You can't quit till the challenge process is over&11!&00\r\n", ch);
     return;
    }
  if (PRF2_FLAGGED(ch, PRF2_FISHING)) {
   send_to_char("You are too busy fishing.\r\n", ch);
   return;
  }
  for (obj = ch->carrying; obj; obj = obj->next_content) {
   if (IS_OBJ_STAT(obj, ITEM_FLAG)) {
    send_to_char("You may not quit or recall with a clan flag!\r\n", ch);
     return;  
   }
  }
  if (AFF_FLAGGED(ch, AFF_HALTED)) {
     send_to_char("You can't!\r\n", ch);
     return; 
     } 
  if (GET_POS(ch) <= POS_SLEEPING) {
     send_to_char("You can't quit while asleep!\r\n", ch);
     return;
     }
  if (ROOM_FLAGGED(ch->in_room, ROOM_NOQUIT))
      {
      send_to_char("You cannot quit here!\r\n", ch);
    return;
     }
/*
*  if (!IS_SET(ROOM_FLAGS(ch->in_room), ROOM_HOTEL))
*    {
*    send_to_char("You're not in a hotel!\r\n", ch);
*  return;
*    }
*/
  if (GET_POS(ch) == POS_FIGHTING)
      {
      send_to_char("You cannot quit while fighting.\r\n", ch);
    return;
      }
    if (PRF_FLAGGED(ch, PRF_OOZARU)) {
     REMOVE_BIT(PRF_FLAGS(ch), PRF_OOZARU);
     send_to_char("You revert to humanoid form before you leave..\r\n", ch);
     GET_MAX_HIT(ch) -= 1000000;  
     GET_MAX_MANA(ch) -= 1000000; 
     } 
  if (subcmd != SCMD_QUIT && GET_LEVEL(ch) < LVL_IMMORT)
    send_to_char("You have to type quit--no less, to quit!\r\n", ch);
  else if (GET_POS(ch) == POS_FIGHTING)
    send_to_char("No way!  You're fighting for your life!\r\n", ch);
  else if (GET_POS(ch) < POS_STUNNED) {
    char_to_room(ch, 4500);
    die(ch, NULL);
  } else {
     GET_TPEARN(ch) = GET_ALIGNMENT(ch);
    if (!GET_INVIS_LEV(ch))
      act("$n has left the game.", TRUE, ch, 0, 0, TO_ROOM);
    if (GET_LEVEL(ch) < LVL_IMMORT) {
    sprintf(buf, "&16[&10Player Logout&16] &15%s has left &11Drag&12(&09*&12)&11n Ball - &16Resurrection of &12T&00&07r&06u&07t&12h&00\r\n", GET_NAME(ch));
    send_to_all(buf);
    }
    sprintf(buf, "%s has quit the game.", GET_NAME(ch));
    mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
    send_to_char("Goodbye, friend.. Come back soon!\r\n", ch);
    save_room = ch->in_room;
    GET_LOADROOM(ch) = GET_ROOM_VNUM(IN_ROOM(ch));
    SET_BIT(PLR_FLAGS(ch), PLR_LOADROOM);
    /*
     * kill off all sockets connected to the same player as the one who is
     * trying to quit.  Helps to maintain sanity as well as prevent duping.
     */
    for (d = descriptor_list; d; d = next_d) {
      next_d = d->next;
      if (d == ch->desc)
        continue;
      if (d->character && (GET_IDNUM(d->character) == GET_IDNUM(ch)))
        STATE(d) = CON_DISCONNECT;
    }

   if (free_rent)
      Crash_rentsave(ch, 0);
      extract_char(ch, TRUE);		/* Char is saved in extract char */

    /* If someone is quitting in their house, let them load back here */
    if (ROOM_FLAGGED(save_room, ROOM_HOUSE))
      save_char(ch, save_room);
  }
}
/* Save code, obviously*/
ACMD(do_save)
{
  if (IS_NPC(ch) || !ch->desc) {
    return;
  }
  /* Only tell the char we're saving if they actually typed "save" */
  
    /*
     * This prevents item duplication by two PC's using coordinated saves
     * (or one PC with a house) and system crashes. Note that houses are
     * still automatically saved without this enabled.
     */
  if (auto_save) {
   sprintf(buf, "&12Saving &14%s&12...    &00&06Saved&12.\r\n", GET_NAME(ch));
   if (GET_LEVEL(ch) <= 10) {
    sprintf(buf + strlen(buf), "&10Make sure, &14%s&10, that you read help newbie, help mudrules, and help help!&00\r\n", GET_NAME(ch));
    }
   send_to_char(buf, ch);
   save_char(ch, NOWHERE);
   Crash_crashsave(ch);
  }
}


/* generic function for commands which are normally overridden by
   special procedures - i.e., shop commands, mail commands, etc. */
ACMD(do_not_here)
{
  send_to_char("Sorry, but you cannot do that here!\r\n", ch);
}
/* Swiftness command, allows player to move through rooms without being spotted
*/
ACMD(do_SWIFTNESS)
{
  struct affected_type af;
  byte percent;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
  if (!GET_SKILL(ch, SKILL_SWIFTNESS)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
   }

  send_to_char("You starts moving so fast you cannot be seen.\r\n", ch);
  if (AFF_FLAGGED(ch, AFF_SWIFTNESS))
    affect_from_char(ch, SKILL_SWIFTNESS);

  percent = number(1, 101);	/* 101% is a complete failure */

  if (percent > GET_SKILL(ch, SKILL_SWIFTNESS) + dex_app_skill[GET_DEX(ch)].SWIFTNESS)
    return;

  af.type = SKILL_SWIFTNESS;
  af.duration = 200;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = AFF_SWIFTNESS;
  affect_to_char(ch, &af);
}
/* Allows user to hide */
ACMD(do_hide)
{
  byte percent;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  send_to_char("You attempt to hide yourself.\r\n", ch);

  if (AFF_FLAGGED(ch, AFF_HIDE))
    REMOVE_BIT(AFF_FLAGS(ch), AFF_HIDE);

  percent = number(1, 101);	/* 101% is a complete failure */

  if (percent > GET_SKILL(ch, SKILL_HIDE) + dex_app_skill[GET_DEX(ch)].hide)
    return;

  SET_BIT(AFF_FLAGS(ch), AFF_HIDE);
}
/* Allows user to steal */
ACMD(do_mug)
{
  struct char_data *vict;
  struct obj_data *obj;
  char vict_name[MAX_INPUT_LENGTH], obj_name[MAX_INPUT_LENGTH];
  int percent, gold, eq_pos, pcsteal = 0, ohoh = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (!GET_SKILL(ch, SKILL_MUG)) {
  send_to_char("You can't do that!\r\n", ch);
  return;
  }

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return;
  }
  if (GET_MANA(ch) < GET_MAX_MANA(ch) / 20) {
   send_to_char("Not enough ki to concentrate!", ch);
   return;
   }

  argument = one_argument(argument, obj_name);
  one_argument(argument, vict_name);

  if (!(vict = get_char_room_vis(ch, vict_name))) {
    send_to_char("Steal what from who?\r\n", ch);
    return;
  } else if (vict == ch) {
    send_to_char("Come on now, that's rather stupid!\r\n", ch);
    return;
  }
  
  /* 101% is a complete failure */
  percent = number(1, 40);
  if (IS_NPC(vict)) {
   send_to_char("No mugging mobs asshole!\r\n", ch);
   return;
  }
  if (!IS_NPC(ch) && GET_POS(ch) == POS_STUNNED && number(1, 3) == 3) {
   GET_POS(vict) = POS_STANDING;
   GET_POS(ch) = POS_SITTING;
   act("&16$N&09 recovers from the stun, and grabs your hand and throws you down!&00", FALSE, ch, 0, vict, TO_CHAR);
   act("&09You recover from the stun, and grab &16$n's&09 hand and throw them down before they can steal more. Now is your chance, slay them!", FALSE, ch, 0, vict, TO_VICT);
   act("&14$N &09recovers from the stun, and grabs &16$n's&09 hand and throws them down before $e can steal anymore!&09", FALSE, ch, 0, vict, TO_NOTVICT);
   WAIT_STATE(ch, PULSE_VIOLENCE * 2);
  }
  if (GET_POS(vict) < POS_SLEEPING)
    percent = -1;		/* ALWAYS SUCCESS */

  if (!pt_allowed && !IS_NPC(vict))
    pcsteal = 1;

  /* NO NO With Imp's and Shopkeepers, and if player thieving is not allowed */
  if (GET_LEVEL(vict) >= LVL_IMMORT ||
      GET_MOB_SPEC(vict) == shop_keeper || IS_NPC(vict))
      percent = 101;
  if (str_cmp(obj_name, "coins") && str_cmp(obj_name, "gold")) {

    if (!(obj = get_obj_in_list_vis(vict, obj_name, vict->carrying))) {

      for (eq_pos = 0; eq_pos < NUM_WEARS; eq_pos++)
	if (GET_EQ(vict, eq_pos) &&
	    (isname(obj_name, GET_EQ(vict, eq_pos)->name)) &&
	    CAN_SEE_OBJ(ch, GET_EQ(vict, eq_pos))) {
	  obj = GET_EQ(vict, eq_pos);
	  break;
	}
      if (!obj || IS_NPC(vict)) {
	act("$E hasn't got that item.", FALSE, ch, 0, vict, TO_CHAR);
	return;
      }
      else {			/* It is equipment */
        if (obj != GET_EQ(vict, WEAR_HOLD)) {
	  send_to_char("Steal the equipment now?  Impossible!\r\n", ch);
	  return;
	} else {
	  act("You unequip $p and steal it.", FALSE, ch, obj, 0, TO_CHAR);
	  act("$n steals $p from $N.", FALSE, ch, obj, vict, TO_NOTVICT);
          act("$n steals $p from out of your hand!", FALSE, ch, obj, vict, TO_VICT);
          sprintf(buf1, "(GC) %s has stolen %s&10 from %s at %s.", GET_NAME(ch), obj->short_description, GET_NAME(vict), world[vict->in_room].name);
          mudlog(buf1, BRF, LVL_GOD, TRUE);
	  obj_to_char(unequip_char(vict, eq_pos), ch);
          SET_BIT(PRF2_FLAGS(ch), PRF2_MUGG);
	}
      }
    } else {			/* obj found in inventory */

      if (GET_DEX(vict) >= 100) {
        percent = number(0, 3);
      }
      if (IS_OBJ_STAT(obj, ITEM_NODONATE)) {
       send_to_char("You can't steal that, it is hidden in their body!\r\n", ch);
       return;
      }
      if (AWAKE(vict) && (GET_DEX(vict) > (GET_DEX(ch) - percent))) {
	ohoh = TRUE;
	act("Oops..", FALSE, ch, 0, 0, TO_CHAR);
	act("$n tried to steal something from you!", FALSE, ch, 0, vict, TO_VICT);
	act("$n tries to steal something from $N.", TRUE, ch, 0, vict, TO_NOTVICT);
      } else {			/* Steal the item */
	if ((IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))) {
	  if ((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) < CAN_CARRY_W(ch)) {
            sprintf(buf1, "(GC) %s has stolen %s&10 from %s at %s.", GET_NAME(ch), obj->short_description, GET_NAME(vict), world[vict->in_room].name);
             mudlog(buf1, BRF, LVL_GOD, TRUE);
	    obj_from_char(obj);
	    obj_to_char(obj, ch);
	    send_to_char("Got it!\r\n", ch);
            if (number(1, 50) <= 15) {
             act("You feel lighter for some odd reason.", FALSE, ch, 0, 0, TO_VICT);
             }            
            SET_BIT(PRF2_FLAGS(ch), PRF2_MUGG);
	  }
	} else
	  send_to_char("You cannot carry that much.\r\n", ch);
      }
    }
  } else {			/* Steal some coins */
    if (AWAKE(vict) && (GET_DEX(vict) > GET_DEX(ch) - percent)) {
      ohoh = TRUE;
      act("Oops..", FALSE, ch, 0, 0, TO_CHAR);
      act("You discover that $n has $s hands in your wallet.", FALSE, ch, 0, vict, TO_VICT);
      act("$n tries to steal gold from $N.", TRUE, ch, 0, vict, TO_NOTVICT);
    } else {
      /* Steal some gold coins */
      gold = (int) ((GET_GOLD(vict) * number(1, 10)) / 100);
      gold = MIN(50000, gold);
      if (gold > 0) {
	GET_GOLD(ch) += gold;
	GET_GOLD(vict) -= gold;
        if (gold > 1) {
	  sprintf(buf, "Bingo!  You got %d gold coins.\r\n", gold);
          sprintf(buf1, "(GC) %s has stolen %d zenni from %s at %s.", GET_NAME(ch), gold, GET_NAME(vict), world[vict->in_room].name);
          mudlog(buf1, BRF, LVL_GOD, TRUE);
	  send_to_char(buf, ch);
          SET_BIT(PRF2_FLAGS(ch), PRF2_MUGG);
	} else {
	  send_to_char("You manage to swipe a solitary gold coin.\r\n", ch);
	}
      } else {
	send_to_char("You couldn't get any gold...\r\n", ch);
      }
    }
  }
  if (GET_MANA(ch) >= GET_MAX_MANA(ch) / 20) {
   GET_MANA(ch) -= GET_MAX_MANA(ch) / 20;
  }
}
/* Not used */
ACMD(do_practice)
{
  one_argument(argument, arg);
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (*arg)
    send_to_char("You can only practice skills in your guild.\r\n", ch);
  else
    list_skills(ch);
}
/* become visible if you are not */
ACMD(do_visible)
{
  if (GET_LEVEL(ch) >= LVL_IMMORT) {
    perform_immort_vis(ch);
    return;
  }

  if AFF_FLAGGED(ch, AFF_INVISIBLE) {
    appear(ch);
    send_to_char("You break the spell of invisibility.\r\n", ch);
  } else
    send_to_char("You are already visible.\r\n", ch);
}
/* Set your title */
ACMD(do_title)
{
  skip_spaces(&argument);
  delete_doubledollar(argument);
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (IS_NPC(ch))
    send_to_char("Your title is fine... go away.\r\n", ch);
  else if (PLR_FLAGGED(ch, PLR_NOTITLE))
    send_to_char("You can't title yourself -- you shouldn't have abused it!\r\n", ch);
  else if (strstr(argument, "(") || strstr(argument, ")"))
    send_to_char("Titles can't contain the ( or ) characters.\r\n", ch);
  else if (strlen(argument) > MAX_TITLE_LENGTH * 2) {
    sprintf(buf, "Sorry, titles can't be longer than %d characters.\r\n",
	    MAX_TITLE_LENGTH * 2);
    send_to_char(buf, ch);
  } else {
    set_title(ch, argument);
    sprintf(buf, "Okay, you're now %s %s.\r\n", GET_NAME(ch), GET_TITLE(ch));
    send_to_char(buf, ch);
  }
}
/* For grouping */
int perform_group(struct char_data *ch, struct char_data *vict)
{
  if (AFF_FLAGGED(vict, AFF_GROUP) || !CAN_SEE(ch, vict))
    return 0;
  if (GET_MAX_HIT(ch)/100 * 50 > GET_MAX_HIT(vict) && !IS_NPC(vict)) {
    act("They are too weak to be in your group!", FALSE, ch, 0, 0, TO_CHAR);
    return 0;
  }
  if (GET_MAX_HIT(vict)/100 * 50 > GET_MAX_HIT(ch) && !IS_NPC(vict)) {
    act("They are too strong to be in your group!", FALSE, ch, 0, 0, TO_CHAR);
    return 0;
  }
  SET_BIT(AFF_FLAGS(vict), AFF_GROUP);
  if (ch != vict)
    act("$N is now a member of your group.", FALSE, ch, 0, vict, TO_CHAR);
  act("You are now a member of $n's group.", FALSE, ch, 0, vict, TO_VICT);
  act("$N is now a member of $n's group.", FALSE, ch, 0, vict, TO_NOTVICT);
  return 1;
}
/* For grouping */
void print_group(struct char_data *ch)
{
  struct char_data *k;
  struct follow_type *f;

  if (!AFF_FLAGGED(ch, AFF_GROUP))
    send_to_char("But you are not the member of a group!\r\n", ch);
  else {
    send_to_char("Your group consists of:\r\n", ch);

    k = (ch->master ? ch->master : ch);

    if (AFF_FLAGGED(k, AFF_GROUP)) {
      sprintf(buf, "     [Pl:%3d Ki:%3d] [%s] $N (Head of group)",
	      GET_HIT(k), GET_MANA(k), CLASS_ABBR(k));
      act(buf, FALSE, ch, 0, k, TO_CHAR);
    }

    for (f = k->followers; f; f = f->next) {
      if (!AFF_FLAGGED(f->follower, AFF_GROUP))
	continue;

      sprintf(buf, "     [Pl:%3d Ki:%3d] [%s] $N", GET_HIT(f->follower),
	      GET_MANA(f->follower), CLASS_ABBR(f->follower));
      act(buf, FALSE, ch, 0, f->follower, TO_CHAR);
    }
  }
}
/* For grouping */
ACMD(do_group)
{
  struct char_data *vict;
  struct follow_type *f;
  int found;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  one_argument(argument, buf);

  if (!*buf) {
    print_group(ch);
    return;
  }

  if (ch->master) {
    act("You can not enroll group members without being head of a group.",
	FALSE, ch, 0, 0, TO_CHAR);
    return;
  }

  if (!str_cmp(buf, "all")) {
    perform_group(ch, ch);
    for (found = 0, f = ch->followers; f; f = f->next)
      found += perform_group(ch, f->follower);
    if (!found)
      send_to_char("Everyone following you is already in your group.\r\n", ch);
    return;
  }

  if (!(vict = get_char_room_vis(ch, buf)))
    send_to_char(NOPERSON, ch);
  else if ((vict->master != ch) && (vict != ch))
    act("$N must follow you to enter your group.", FALSE, ch, 0, vict, TO_CHAR);
  else {
    if (!AFF_FLAGGED(vict, AFF_GROUP))
      perform_group(ch, vict);
    else {
      if (ch != vict)
	act("$N is no longer a member of your group.", FALSE, ch, 0, vict, TO_CHAR);
      act("You have been kicked out of $n's group!", FALSE, ch, 0, vict, TO_VICT);
      act("$N has been kicked out of $n's group!", FALSE, ch, 0, vict, TO_NOTVICT);
      REMOVE_BIT(AFF_FLAGS(vict), AFF_GROUP);
    }
  }
}
/* For grouping */
ACMD(do_ungroup)
{
  struct follow_type *f, *next_fol;
  struct char_data *tch;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  one_argument(argument, buf);

  if (!*buf) {
    if (ch->master || !(AFF_FLAGGED(ch, AFF_GROUP))) {
      send_to_char("But you lead no group!\r\n", ch);
      return;
    }
    sprintf(buf2, "%s has disbanded the group.\r\n", GET_NAME(ch));
    for (f = ch->followers; f; f = next_fol) {
      next_fol = f->next;
      if (AFF_FLAGGED(f->follower, AFF_GROUP)) {
	REMOVE_BIT(AFF_FLAGS(f->follower), AFF_GROUP);
	send_to_char(buf2, f->follower);
        if (!AFF_FLAGGED(f->follower, AFF_MAJIN))
	  stop_follower(f->follower);
      }
    }

    REMOVE_BIT(AFF_FLAGS(ch), AFF_GROUP);
    send_to_char("You disband the group.\r\n", ch);
    return;
  }
  if (!(tch = get_char_room_vis(ch, buf))) {
    send_to_char("There is no such person!\r\n", ch);
    return;
  }
  if (tch->master != ch) {
    send_to_char("That person is not following you!\r\n", ch);
    return;
  }

  if (!AFF_FLAGGED(tch, AFF_GROUP)) {
    send_to_char("That person isn't in your group.\r\n", ch);
    return;
  }

  REMOVE_BIT(AFF_FLAGS(tch), AFF_GROUP);

  act("$N is no longer a member of your group.", FALSE, ch, 0, tch, TO_CHAR);
  act("You have been kicked out of $n's group!", FALSE, ch, 0, tch, TO_VICT);
  act("$N has been kicked out of $n's group!", FALSE, ch, 0, tch, TO_NOTVICT);
 
  if (!AFF_FLAGGED(tch, AFF_MAJIN))
    stop_follower(tch);
}
/* For groups */
ACMD(do_report)
{
  struct char_data *k;
  struct follow_type *f;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (!AFF_FLAGGED(ch, AFF_GROUP)) {
    send_to_char("But you are not a member of any group!\r\n", ch);
    return;
  }
  sprintf(buf, "%s reports: %d/%dH, %d/%dK, %d/%dV\r\n",
	  GET_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
	  GET_MANA(ch), GET_MAX_MANA(ch),
	  GET_MOVE(ch), GET_MAX_MOVE(ch));

  CAP(buf);

  k = (ch->master ? ch->master : ch);

  for (f = k->followers; f; f = f->next)
    if (AFF_FLAGGED(f->follower, AFF_GROUP) && f->follower != ch)
      send_to_char(buf, f->follower);
  if (k != ch)
    send_to_char(buf, k);
  send_to_char("You report to the group.\r\n", ch);
}
/* For groups */
ACMD(do_split)
{
  int amount, num, share;
  struct char_data *k;
  struct follow_type *f;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  if (IS_NPC(ch))
    return;

  one_argument(argument, buf);

  if (is_number(buf)) {
    amount = atoi(buf);
    if (amount <= 0) {
      send_to_char("Sorry, you can't do that.\r\n", ch);
      return;
    }
    if (amount > GET_GOLD(ch)) {
      send_to_char("You don't seem to have that much gold to split.\r\n", ch);
      return;
    }
    k = (ch->master ? ch->master : ch);

    if (AFF_FLAGGED(k, AFF_GROUP) && (k->in_room == ch->in_room))
      num = 1;
    else
      num = 0;

    for (f = k->followers; f; f = f->next)
      if (AFF_FLAGGED(f->follower, AFF_GROUP) &&
	  (!IS_NPC(f->follower)) &&
	  (f->follower->in_room == ch->in_room))
	num++;

    if (num && AFF_FLAGGED(ch, AFF_GROUP))
      share = amount / num;
    else {
      send_to_char("With whom do you wish to share your gold?\r\n", ch);
      return;
    }

    GET_GOLD(ch) -= share * (num - 1);

    if (AFF_FLAGGED(k, AFF_GROUP) && (k->in_room == ch->in_room)
	&& !(IS_NPC(k)) && k != ch) {
      GET_GOLD(k) += share;
      sprintf(buf, "%s splits %d coins; you receive %d.\r\n", GET_NAME(ch),
	      amount, share);
      send_to_char(buf, k);
    }
    for (f = k->followers; f; f = f->next) {
      if (AFF_FLAGGED(f->follower, AFF_GROUP) &&
	  (!IS_NPC(f->follower)) &&
	  (f->follower->in_room == ch->in_room) &&
	  f->follower != ch) {
	GET_GOLD(f->follower) += share;
	sprintf(buf, "%s splits %d coins; you receive %d.\r\n", GET_NAME(ch),
		amount, share);
	send_to_char(buf, f->follower);
      }
    }
    sprintf(buf, "You split %d coins among %d members -- %d coins each.\r\n",
	    amount, num, share);
    send_to_char(buf, ch);
  } else {
    send_to_char("How many coins do you wish to split with your group?\r\n", ch);
    return;
  }
}
/* Generic use command */
ACMD(do_use)
{
  struct obj_data *mag_item;
  int equipped = 1;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  half_chop(argument, arg, buf);
  if (!*arg) {
    sprintf(buf2, "What do you want to %s?\r\n", CMD_NAME);
    send_to_char(buf2, ch);
    return;
  }
  mag_item = GET_EQ(ch, WEAR_SHIELD);

  if (!mag_item || !isname(arg, mag_item->name)) {
    switch (subcmd) {
    case SCMD_SWALLOW:
    case SCMD_SMOKE:
    case SCMD_PLANT:
      equipped = 0;
      if (!(mag_item = get_obj_in_list_vis(ch, arg, ch->carrying))) {
	sprintf(buf2, "You don't seem to have %s %s.\r\n", AN(arg), arg);
	send_to_char(buf2, ch);
	return;
      }
      break;
    case SCMD_USE:
      sprintf(buf2, "You don't seem to be wearing %s %s.\r\n", AN(arg), arg);
      send_to_char(buf2, ch);
      return;
    default:
      log("SYSERR: Unknown subcmd %d passed to do_use.", subcmd);
      return;
    }
  }
  switch (subcmd) {
  case SCMD_PLANT:
    if (GET_OBJ_TYPE(mag_item) != ITEM_SAIBAKIT) {
      send_to_char("You can only plant seeds.", ch);
      return;
    }
    break;
  case SCMD_SMOKE:
    if (GET_POS(ch) == POS_FIGHTING)
      {
      send_to_char("You cannot do that while fighting!\r\n", ch);
      return;
      }
    if (GET_OBJ_TYPE(mag_item) != ITEM_TRASH) {
      send_to_char("You can't smoke that!", ch);
      return;
      }
    break;
  case SCMD_SWALLOW:
    if (GET_POS(ch) == POS_FIGHTING)
      {
      send_to_char("You cannot do that while fighting!\r\n", ch);
      return;
      }
    if (GET_OBJ_TYPE(mag_item) == ITEM_HOLYWATER && IS_ANDROID(ch)) {
      send_to_char("Androids can gain nothing from holywater!.", ch);
      return;
    }
    if (GET_OBJ_TYPE(mag_item) == ITEM_HOLYWATER && GET_MAX_HIT(ch) <= 499999) {
       send_to_char("You can't use holy waters until you are at least 500k pl.\r\n", ch);
       return;
    }
    if (GET_OBJ_TYPE(mag_item) == ITEM_NANO && !IS_ANDROID(ch)) {
      send_to_char("You can't swallow nano beans!", ch);
      return;
    }
    if (GET_OBJ_TYPE(mag_item) != ITEM_NANO && IS_ANDROID(ch)) {
      send_to_char("You can only swallow nano beans.", ch);
      return;
    }
    break;
  case SCMD_USE:
    if ((GET_OBJ_TYPE(mag_item) != ITEM_SCOUTER) &&
	(GET_OBJ_TYPE(mag_item) != ITEM_STAFF)) {
      send_to_char("You can't seem to figure out how to use it.\r\n", ch);
      return;
    }
    break;
  }

  mag_objectmagic(ch, mag_item, buf);
}
/* Set wimpy level */
ACMD(do_wimpy)
{
  int wimp_lev;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  /* 'wimp_level' is a player_special. -gg 2/25/98 */
  if (IS_NPC(ch))
    return;

  one_argument(argument, arg);

  if (!*arg) {
    if (GET_WIMP_LEV(ch)) {
      sprintf(buf, "Your current wimp level is %d hit points.\r\n",
	      GET_WIMP_LEV(ch));
      send_to_char(buf, ch);
      return;
    } else {
      send_to_char("At the moment, you're not a wimp.  (sure, sure...)\r\n", ch);
      return;
    }
  }
  if (isdigit(*arg)) {
    if ((wimp_lev = atoi(arg))) {
      if (wimp_lev < 0)
	send_to_char("Heh, heh, heh.. we are jolly funny today, eh?\r\n", ch);
      else if (wimp_lev > GET_MAX_HIT(ch))
	send_to_char("That doesn't make much sense, now does it?\r\n", ch);
      else if (wimp_lev > (GET_MAX_HIT(ch) / 2))
	send_to_char("You can't set your wimp level above half your hit points.\r\n", ch);
      else {
	sprintf(buf, "Okay, you'll wimp out if you drop below %d hit points.\r\n",
		wimp_lev);
	send_to_char(buf, ch);
	GET_WIMP_LEV(ch) = wimp_lev;
      }
    } else {
      send_to_char("Okay, you'll now tough out fights to the bitter end.\r\n", ch);
      GET_WIMP_LEV(ch) = 0;
    }
  } else
    send_to_char("Specify at how many hit points you want to wimp out at.  (0 to disable)\r\n", ch);

  return;

}
/* Tinker with your prompt? */
ACMD(do_display)
{
  size_t i;

  if (IS_NPC(ch)) {
    send_to_char("Mosters don't need displays.  Go away.\r\n", ch);
    return;
  }
  skip_spaces(&argument);

  if (!*argument) {
    send_to_char("Usage: prompt { on or none }\r\n", ch);
    return;
  }
  if ((!str_cmp(argument, "on")) || (!str_cmp(argument, "all")))
    SET_BIT(PRF_FLAGS(ch), PRF_DISPHP | PRF_DISPMANA);
  else {
    REMOVE_BIT(PRF_FLAGS(ch), PRF_DISPHP | PRF_DISPMANA);

    for (i = 0; i < strlen(argument); i++) {
      switch (LOWER(argument[i])) {
      case 'h':
	SET_BIT(PRF_FLAGS(ch), PRF_DISPHP);
	break;
      case 'm':
	SET_BIT(PRF_FLAGS(ch), PRF_DISPMANA);
	break;
      default:
	send_to_char("Usage: prompt { { H | M | V } | all | none }\r\n", ch);
	return;
	break;
      }
    }
  }

  send_to_char(OK, ch);
}
/* For writing in bug, type, and idea files. */
ACMD(do_gen_write)
{
  FILE *fl;
  char *tmp, buf[MAX_STRING_LENGTH];
  const char *filename;
  struct stat fbuf;
  time_t ct;

  switch (subcmd) {
  case SCMD_BUG:
    filename = BUG_FILE;
    break;
  case SCMD_TYPO:
    filename = TYPO_FILE;
    break;
  case SCMD_IDEA:
    filename = IDEA_FILE;
    break;
  default:
    return;
  }

  ct = time(0);
  tmp = asctime(localtime(&ct));

  if (IS_NPC(ch)) {
    send_to_char("Monsters can't have ideas - Go away.\r\n", ch);
    return;
  }

  skip_spaces(&argument);
  delete_doubledollar(argument);

  if (!*argument) {
    send_to_char("That must be a mistake...\r\n", ch);
    return;
  }
  sprintf(buf, "%s %s: %s", GET_NAME(ch), CMD_NAME, argument);
  mudlog(buf, CMP, LVL_IMMORT, FALSE);

  if (stat(filename, &fbuf) < 0) {
    perror("Error statting file");
    return;
  }
  if (fbuf.st_size >= max_filesize) {
    send_to_char("Sorry, the file is full right now.. try again later.\r\n", ch);
    return;
  }
  if (!(fl = fopen(filename, "a"))) {
    perror("do_gen_write");
    send_to_char("Could not open the file.  Sorry.\r\n", ch);
    return;
  }
  fprintf(fl, "%-8s (%6.6s) [%5d] %s\n", GET_NAME(ch), (tmp + 4),
	  GET_ROOM_VNUM(IN_ROOM(ch)), argument);
  fclose(fl);
  send_to_char("Okay.  Thanks!\r\n", ch);
}



#define TOG_OFF 0
#define TOG_ON  1

#define PRF_TOG_CHK(ch,flag) ((TOGGLE_BIT(PRF_FLAGS(ch), (flag))) & (flag))

ACMD(do_gen_tog)
{
  long result;

  const char *tog_messages[][2] = {
    {"Nohassle disabled.\r\n",
    "Nohassle enabled.\r\n"},
    {"Brief mode off.\r\n",
    "Brief mode on.\r\n"},
    {"Compact mode off.\r\n",
    "Compact mode on.\r\n"},
    {"You can now hear tells.\r\n",
    "You are now deaf to tells.\r\n"},
    {"You can now hear oocs.\r\n",
    "You are now deaf to oocs.\r\n"},
    {"You can now hear shouts.\r\n",
    "You are now deaf to shouts.\r\n"},
    {"You can now hear roleplay messages.\r\n",
    "You are now deaf to roleplay messages.\r\n"},
    {"You will no longer autoassist!\r\n",
    "You will now autoassist!.\r\n"},
    {"You can now hear the Wiz-channel.\r\n",
    "You are now deaf to the Wiz-channel.\r\n"},
    {"You are no longer part of the RP.\r\n",
    "Okay, you are part of the RP!\r\n"},
    {"You will no longer see the room flags.\r\n",
    "You will now see the room flags.\r\n"},
    {"You will now have your communication repeated.\r\n",
    "You will no longer have your communication repeated.\r\n"},
    {"HolyLight mode off.\r\n",
    "HolyLight mode on.\r\n"},
    {"Nameserver_is_slow changed to NO; IP addresses will now be resolved.\r\n",
    "Nameserver_is_slow changed to YES; sitenames will no longer be resolved.\r\n"},
    {"Autoexits disabled.\r\n",
    "Autoexits enabled.\r\n"},
    {"AFK flag is now off.\r\n",
    "AFK flag is now on.\r\n"},
    {"You pack up and clean out your camp.\r\n",
    "&15You start a small fire and open camp to rest.\r\n"},
    {"Autospliting disabled.\r\n",
    "Autospliting enabled.\r\n"},
    {"Autolooting disabled.\r\n",
    "Autolooting enabled.\r\n"}
  
  };


  if (IS_NPC(ch))
    return;

  switch (subcmd) {
  case SCMD_NOHASSLE:
    result = PRF_TOG_CHK(ch, PRF_NOHASSLE);
    break;
  case SCMD_BRIEF:
    result = PRF_TOG_CHK(ch, PRF_BRIEF);
    break;
  case SCMD_COMPACT:
    result = PRF_TOG_CHK(ch, PRF_COMPACT);
    break;
  case SCMD_NOTELL:
    result = PRF_TOG_CHK(ch, PRF_NOTELL);
    break;
  case SCMD_NOooc:
    result = PRF_TOG_CHK(ch, PRF_NOooc);
    break;
  case SCMD_DEAF:
    result = PRF_TOG_CHK(ch, PRF_DEAF);
    break;
  case SCMD_NOGOSSIP:
    result = PRF_TOG_CHK(ch, PRF_NOGOSS);
    break;
  case SCMD_NOWIZ:
    result = PRF_TOG_CHK(ch, PRF_NOWIZ);
    break;
  case SCMD_QUEST:
    result = PRF_TOG_CHK(ch, PRF_QUEST);
    break;
  case SCMD_ROOMFLAGS:
    result = PRF_TOG_CHK(ch, PRF_ROOMFLAGS);
    break;
  case SCMD_NOREPEAT:
    result = PRF_TOG_CHK(ch, PRF_NOREPEAT);
    break;
  case SCMD_HOLYLIGHT:
    result = PRF_TOG_CHK(ch, PRF_HOLYLIGHT);
    break;
  case SCMD_SLOWNS:
    result = (nameserver_is_slow = !nameserver_is_slow);
    break;
  case SCMD_AUTOEXIT:
    result = PRF_TOG_CHK(ch, PRF_AUTOEXIT);
    break;
 case SCMD_AFK:
     result = PRF_TOG_CHK(ch, PRF_AFK);
  if (PRF_FLAGGED(ch, PRF_AFK))
    act("$n has gone AFK.", TRUE, ch, 0, 0, TO_ROOM);
  else
    act("$n has come back from AFK.", TRUE, ch, 0, 0, TO_ROOM);
  break;
 case SCMD_CAMP:
     result = PRF_TOG_CHK(ch, PRF_CAMP);	     
   if (PRF_FLAGGED(ch, PRF_CAMP))
       act("$n sprawls out on the ground and sets up camp.", TRUE, ch, 0, 0, TO_ROOM);
  else
       act("$n packs up some stuff and leaves their camp.", TRUE, ch, 0, 0, TO_ROOM);       
    break;
 case SCMD_AUTOSPLIT:
    result = PRF_TOG_CHK(ch, PRF_AUTOSPLIT);
    break;
  case SCMD_AUTOLOOT:
    result = PRF_TOG_CHK(ch, PRF_AUTOLOOT);
    break;
  default:
    log("SYSERR: Unknown subcmd %d in do_gen_toggle.", subcmd);
    return;
  }

  if (result)
    send_to_char(tog_messages[subcmd][TOG_ON], ch);
  else
    send_to_char(tog_messages[subcmd][TOG_OFF], ch);
    
  return;
}
/* Android repair skill */
ACMD(do_repair)	
{   
   int MANA = 0, HIT = 0; 
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

    if (!GET_SKILL(ch, SKILL_REPAIR)) {
    send_to_char("You can't do that!\r\n", ch);
    return;
    }
    if (GET_POS(ch) == POS_FIGHTING) {
     send_to_char("Not while fighting, you can't control your mechanisms enough!\r\n", ch);
     return;
    }
    else if (IS_ANDROID(ch) &&GET_MANA(ch) < GET_MAX_MANA(ch)/20)
    { 
  	act("You don't have enough energy to repair your circuits.", TRUE, ch, 0, 0, TO_CHAR);
       }
    else if (IS_ANDROID(ch) &&GET_HIT(ch) < GET_MAX_HIT(ch) && GET_MANA(ch) >= GET_MAX_MANA(ch)/20)
    {  
send_to_char("You stand still for a moment and repair your internal circuits.\r\n", ch);
act("&10$n repairs $s internal circuits.&00", TRUE, ch, 0, 0, TO_ROOM);
   SET_BIT(PLR_FLAGS(ch), PLR_LARM);
    SET_BIT(PLR_FLAGS(ch), PLR_RARM);
    SET_BIT(PLR_FLAGS(ch), PLR_LLEG);
    SET_BIT(PLR_FLAGS(ch), PLR_RLEG);
  MANA = number(GET_MAX_MANA(ch)/20, GET_MAX_MANA(ch)/20);
  HIT = number(GET_MAX_HIT(ch)/8, GET_MAX_HIT(ch)/8);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_HIT(ch) = GET_HIT(ch) + HIT;
    }
    else if (IS_ANDROID(ch) && GET_HIT(ch) >= GET_MAX_HIT(ch))
    {
  	act("You are already fully repaired.", TRUE, ch, 0, 0, TO_CHAR);
    }
   WAIT_STATE(ch, PULSE_VIOLENCE * 1);       
}
/* Namek skill */
ACMD(do_kyodaika)
{
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
   if (!GET_SKILL(ch, SKILL_KYODAIKA)) {
    send_to_char("You don't know that technique!\r\n", ch);
    return;
    }
   if (GET_MANA(ch) < GET_MAX_MANA(ch) / 5) {
    send_to_char("You don't have enough ki!\r\n", ch);
    return;
    }
   if (GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You need to be at full pl!\r\n", ch);
    return;
    }
   if (GET_HIT(ch) > GET_MAX_HIT(ch)) {
    send_to_char("You are already above your max powerlevel!\r\n", ch);
    return;
    }
   else
    act("&15You &10g&00&02ro&10w&15 to &09f&00&01iv&09e &12t&00&04ime&12s &15your size&11!&00", TRUE, ch, 0, 0, TO_CHAR);
    act("&14$n &10g&00&02row&10s&15 to &09f&00&01iv&09e &12t&00&04ime&12s &15their size&11!&00", TRUE, ch, 0, 0, TO_ROOM);
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/5;
    GET_HIT(ch) = GET_HIT(ch) * 2;
}
/* Saiyan/Kai skill */
ACMD(do_kaioken)	
{    
    int MANA = 0, HIT = 0; 
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
    
    one_argument(argument, arg);
  
 if (!GET_SKILL(ch, SKILL_KAIOKEN)) {
    send_to_char("You don't know that technique!\r\n", ch);
    return;
    }
  else if (!*argument) {
    send_to_char("Usage: Kaioken (1-20)\r\n", ch);
    return;
     }

   if (!str_cmp(arg,"1") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"1") && GET_HIT(ch) >= GET_MAX_HIT(ch)+9999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"1")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &141&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &141&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(1000, 1000);
   HIT = number(10000, 10000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"2") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"2") && GET_HIT(ch) >= GET_MAX_HIT(ch)+39999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"2")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &142&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &142&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(10000, 10000);
   HIT = number(40000, 40000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}  
   if (!str_cmp(arg,"3") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"3") && GET_HIT(ch) >= GET_MAX_HIT(ch)+99999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"3")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &143&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &143&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(40000, 40000);
   HIT = number(100000, 100000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"4") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"4") && GET_HIT(ch) >= GET_MAX_HIT(ch)+199999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"4")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &144&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &144&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(50000, 50000);
   HIT = number(200000, 200000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"5") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"5") && GET_HIT(ch) >= GET_MAX_HIT(ch)+399999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"5")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &145&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &145&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(75000, 75000);
   HIT = number(400000, 400000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
  if (!str_cmp(arg,"6") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"6") && GET_HIT(ch) >= GET_MAX_HIT(ch)+799999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"6")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &146&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &146&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(400000, 400000);
   HIT = number(800000, 800000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"7") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"7") && GET_HIT(ch) >= GET_MAX_HIT(ch)+899999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"7")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &147&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &147&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(450000, 450000);
   HIT = number(900000, 900000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"8") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"8") && GET_HIT(ch) >= GET_MAX_HIT(ch)+999999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"8")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &148&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &148&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(500000, 500000);
   HIT = number(1000000, 1000000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"9") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"9") && GET_HIT(ch) >= GET_MAX_HIT(ch)+1199999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"9")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &149&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &149&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(600000, 600000);
   HIT = number(1200000, 1200000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"10") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"10") && GET_HIT(ch) >= GET_MAX_HIT(ch)+1499999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"10")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1410&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1410&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(750000, 750000);
   HIT = number(1500000, 1500000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"11") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"11") && GET_HIT(ch) >= GET_MAX_HIT(ch)+1999999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"11")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1411&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1411&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(1000000, 1000000);
   HIT = number(2000000, 2000000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"12") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"12") && GET_HIT(ch) >= GET_MAX_HIT(ch)+2499999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"12")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1412&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1412&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(1250000, 1250000);
   HIT = number(2500000, 2500000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"13") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"13") && GET_HIT(ch) >= GET_MAX_HIT(ch)+2999999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"13")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1413&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1413&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(1500000, 1500000);
   HIT = number(3000000, 3000000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"14") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"14") && GET_HIT(ch) >= GET_MAX_HIT(ch)+3499999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"14")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1414&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1414&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(1750000, 1750000);
   HIT = number(3500000, 3500000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"15") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"15") && GET_HIT(ch) >= GET_MAX_HIT(ch)+3999999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"15")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1415&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1415&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(2000000, 2000000);
   HIT = number(4000000, 4000000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"16") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"16") && GET_HIT(ch) >= GET_MAX_HIT(ch)+4499999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"16")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1416&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1416&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(2250000, 2250000);
   HIT = number(4500000, 4500000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"17") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"17") && GET_HIT(ch) >= GET_MAX_HIT(ch)+4999999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"17")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1417&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1417&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(2500000, 2500000);
   HIT = number(5000000, 5000000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"18") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"18") && GET_HIT(ch) >= GET_MAX_HIT(ch)+5499999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"18")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1418&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1418&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(2750000, 2750000);
   HIT = number(5500000, 5500000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"19") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"19") && GET_HIT(ch) >= GET_MAX_HIT(ch)+5999999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"19")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1419&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1419&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(3000000, 3000000);
   HIT = number(6000000, 6000000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
   if (!str_cmp(arg,"20") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"20") && GET_HIT(ch) >= GET_MAX_HIT(ch)+6999999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"20")){
   send_to_char("&15You begin over working your body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1420&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15 level &1420&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(4000000, 4000000);
   HIT = number(7000000, 7000000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
  if (!str_cmp(arg,"super") && GET_HIT(ch) < GET_MAX_HIT(ch)) {
    send_to_char("You don't have enough strength to do that.\r\n", ch);
     }   
   else if (!str_cmp(arg,"super") && GET_HIT(ch) >= GET_MAX_HIT(ch) + 9999999){
   send_to_char("You're already passed that stage of kaioken.\r\n", ch);
     }
   else if (!str_cmp(arg,"super")){
   send_to_char("&15You begin over working your body by reaching &14s&00&06u&14p&00&06e&14r &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15.&00\r\n", ch);
   act("&14$n&15 overworks $s body by reaching &14s&00&06u&14p&00&06e&14r &11k&00&03a&00&01i&09o&00&01k&00&03e&11n&15!&00", TRUE, ch, 0, 0, TO_ROOM);
   MANA = number(10000000, 10000000);
   HIT = number(10000000, 10000000);

   GET_MANA(ch) = GET_MANA(ch) - MANA;
   GET_HIT(ch) = GET_HIT(ch) + HIT; 
}
  if (GET_MANA(ch) <= 0) {
  send_to_char("&09You overwork your body too much and explode.&00\r\n", ch);
  act("&10$n&15 tries to reach a &11kaioken&15 level beyond thier &10limit&15 and &09explodes!&00", TRUE, ch, 0, 0, TO_ROOM);
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  SET_BIT(CLN_FLAGS(ch), CLN_Death);
  GET_DTIMER(ch) = 3600;
  GET_HIT(ch) = GET_MAX_HIT(ch)*0.1;
  GET_MANA(ch) = GET_MAX_MANA(ch)*0.1;
  GET_MOVE(ch) = GET_MOVE(ch);
  GET_POS(ch) = POS_STANDING;
  look_at_room(ch, 0);
  }
}
/* For learning new skills */
ACMD(do_learn) {
	char buf[1024];
	int r = 1 << GET_CLASS(ch);
	int i = 0;
	
	argument = one_argument(argument, buf);

	if (buf[0] != '\0') {
		int skill = 0;
		while (spells[skill].name[0] != '\n') {
			if (!strncasecmp(buf, spells[skill].name, strlen(buf))) {
				if (!(spells[skill].races & r)) {
					send_to_char("You can't learn that.\r\n", ch);
					return;
				}
				else if (GET_PRACTICES(ch) < spells[skill].cost) {
					send_to_char("You don't have enough learning points to learn that.\r\n", ch);
					return;
				} else if (GET_SKILL(ch, skill) != 0) {
					send_to_char("You already know that technique.\r\n", ch);
					return;
				} else {
					sprintf(buf, "&15You expend &09%d&15 learning points to learn &10%s&00.\r\n", spells[skill].cost,
									spells[skill].name);
					send_to_char(buf, ch);
					GET_PRACTICES(ch) = GET_PRACTICES(ch) - spells[skill].cost;
					GET_SKILL(ch, skill) = 100;
					return;
				}
			} else {
				++skill;
			}
		}
		
		sprintf(buf, "%s is not a technique.\r\n", argument);
		send_to_char(buf, ch);
		return;
	}

	send_to_char("&15o&12---------------------------------------------------------&15o&00\r\n", ch);
	send_to_char("&15&12|&11Technique:          &14Cost:                                &12|&00\r\n", ch);
        send_to_char("&15o&12---------------------------------------------------------&15o&00\r\n", ch);
	while (spells[i].name[0] != '\n') {
		if ((spells[i].races & r) && (GET_SKILL(ch, i) == 0) &&  spells[i].type == SPELL_OFFENSIVE) {
			sprintf(buf, "&12%s%-20s&02%3d&12%35s&09\r\n", spells[i].type ? "&09" : "&09", 
						spells[i].name, spells[i].cost, "");
			send_to_char(buf, ch);
		}
		else if ((spells[i].races & r) && (GET_SKILL(ch, i) == 0) &&  spells[i].type == SPELL_DEFENSIVE) {
			sprintf(buf, "&12%s%-20s&02%3d&12%35s&09\r\n", spells[i].type ? "&12" : "&12", 
						spells[i].name, spells[i].cost, "");
			send_to_char(buf, ch);
		}
		else if ((spells[i].races & r) && (GET_SKILL(ch, i) == 0) &&  spells[i].type == SPELL_ENHANCE) {
			sprintf(buf, "&12%s%-20s&02%3d&12%35s&09\r\n", spells[i].type ? "&10" : "&10", 
						spells[i].name, spells[i].cost, "");
			send_to_char(buf, ch);
		}
		else if ((spells[i].races & r) && (GET_SKILL(ch, i) == 0) &&  spells[i].type == SPELL_OTHER) {
			sprintf(buf, "&12%s%-20s&02%3d&12%35s&09\r\n", spells[i].type ? "&15" : "&15", 
						spells[i].name, spells[i].cost, "");
			send_to_char(buf, ch);
		}
		else if ((spells[i].races & r) && (GET_SKILL(ch, i) == 0) &&  spells[i].type == SPELL_HEALSPELL) {
			sprintf(buf, "&12%s%-20s&02%3d&12%35s&09\r\n", spells[i].type ? "&13" : "&13", 
						spells[i].name, spells[i].cost, "");
			send_to_char(buf, ch);
		}
		++i;
	}

	send_to_char("&15o&12-----------------------------------------------&15o&00\r\n", ch);
	sprintf(buf, "&15You have &09%d &15learning points.&00\r\n", GET_PRACTICES(ch));
	send_to_char(buf, ch);
        WAIT_STATE(ch, PULSE_VIOLENCE * 1);
}
/* For displaying learned skills */
ACMD(do_learned)
{
        char buf[2024];
        int pos = 0;
        buf[0] = '\0';
        send_to_char("&10You know the following techniques:&00.\r\n", ch);
send_to_char("&14<&00&06===================================================================&14>&00\r\n", ch);

        for (pos = 1; pos < MAX_SKILLS + 1; ++pos) {
                int i = spell_sort_info[pos];
                if (GET_SKILL(ch, i) >= 2 && spells[i].name[0] != '!') {
                        char buf2[132];
		        sprintf(buf2, "%s%-20s%s", spells[i].type ? "&15" : "&15", spells[i].name, "&00");
                        strcat(buf, buf2);
                }
                        send_to_char(buf, ch);
                        buf[0] = '\0';
        }
send_to_char("\r\n&14<&00&06===================================================================&14>&00\r\n", ch);
}
/* For flying, duh. */
ACMD(do_fly)	
{
  
   if (GET_POS(ch) == POS_FLOATING) {
    send_to_char("&15Your already flying!&00\r\n", ch);
    return;
    }
   else if (!IS_NPC(ch) && GET_MANA(ch) < GET_MAX_MANA(ch)/1000) {
    send_to_char("You do not have enough ki!\r\n", ch);
    return;
    }
  else
    GET_POS(ch) = POS_FLOATING;
    SET_BIT(PRF_FLAGS(ch), PRF_FLYING);
    act("&15You slowly fly high into the sky&11!&00",FALSE, ch, 0, 0, TO_CHAR);
    act("&09$n&15 slowly flies high into the sky&11!.&00",TRUE, ch, 0, 0, TO_NOTVICT);
   if (IS_ANDROID(ch) || IS_NPC(ch)) {
     GET_MANA(ch) = GET_MANA(ch);
   }
  else
    GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) / 1000;
}
/* For landing. */
ACMD(do_land)
 {
   if (GET_POS(ch) == POS_STANDING) {
     send_to_char("&15Your already on the ground!&00\r\n", ch);
     return;
     }

   else {
     GET_POS(ch) == POS_FLOATING; 
     act("&09$n&15 floats down to the ground.&00", TRUE, ch, 0, 0, TO_ROOM);
     act("&15You slowly float down to the ground.&00",TRUE, ch, 0, 0, TO_CHAR);
     GET_POS(ch) = POS_STANDING;
     REMOVE_BIT(PRF_FLAGS(ch), PRF_FLYING);
     return;
     }
}
/* Meditate skill */
ACMD(do_meditate)
   {
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
   int is_altered = FALSE;
   int num_levels = 0;
   int MANA = 0, EXP = 0, increase = 0;
   if (ROOM_FLAGGED(ch->in_room, ROOM_NOQUIT)) {
         send_to_char("You cannot meditate here!\r\n", ch);
         return;
        }
   if (!GET_SKILL(ch, SKILL_MEDITATE)) {
    send_to_char("You don't know how to do that!\r\n", ch);
    return;
   }

if (PRF_FLAGGED(ch, PRF_CAMP)) {
      send_to_char("You can't do this while camped!\r\n", ch);
      return;
    }
if (GET_POS(ch) != POS_SITTING) {
    send_to_char("You must be sitting to meditate!\r\n", ch);
    return;
   }
if (GET_MANA(ch) <= 0) {
   send_to_char("You do not have enough ki to concentrate!\r\n", ch);  
   return;
  }
if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000) {
GET_LEVEL(ch) += 1;
GET_EXP(ch) = 1;
num_levels++;                                                                   
advance_level(ch);
is_altered = TRUE;
send_to_char("&15You feel your inner &14spirit&15 strengthen considerably&09!&00\r\n", ch);
}
  send_to_char("You meditate quietly.\r\n", ch);
  act("&11$n&00 meditates calmly.", TRUE, ch, 0, 0, TO_ROOM);
  MANA = number(1, GET_MAX_MANA(ch)/200);
  EXP = number(GET_MANA(ch)/80, GET_MANA(ch)/80) + (GET_LEVEL(ch) * 20);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
   if (number(1, 5) == 3) {
    increase += number(1, 30);
    GET_MAX_MANA(ch) += increase;
    sprintf(buf, "&15Your &14ki &15increases by &10%d&11!&00\r\n", increase);
    send_to_char(buf, ch);
   }
  return;
}
/* For powering up */
ACMD(do_powerup)	
{   
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
    if (GET_MANA(ch) < GET_MAX_MANA(ch) * .6) {
     send_to_char("You don't have the Ki to powerup!\r\n", ch);
     return;
    }
    if (GET_HIT(ch) >= GET_MAX_HIT(ch))
    {
   switch (number(0, 3)) {
  case 0:
        act("&15You are already fully powered up!&00", TRUE, ch, 0, 0, TO_CHAR);
  break;
  case 1:
	act("&09ARE YOU CRAZY!? &15You can't go over your maximum power!&00", TRUE, ch, 0, 0, TO_CHAR);
  break;
  case 2:
        act("&15And just WHY are we trying to go over your max?&00&00", TRUE, ch, 0, 0, TO_CHAR);
  break;
  case 3:
        act("&15Yeah, right, like that will ever happen&00", TRUE, ch, 0, 0, TO_CHAR);
  break;
  }
  return;
   }
    
    else if (GET_HIT(ch) < GET_MAX_HIT(ch) && GET_MAX_HIT(ch) < 1000000) {
      act("&15You scream aloud as your body reaches its maximum power!&00\r\n", TRUE, ch, 0, 0, TO_CHAR);
      act("&14$n&15 screams aloud as &14$s &15body reaches its maximum power!&00\r\n", TRUE, ch, 0, 0, TO_ROOM);
      }
    else if (GET_HIT(ch) < GET_MAX_HIT(ch) && GET_MAX_HIT(ch) < 5000000) {
      act("&15You begin screaming, as you &09flame&15 with ki, powering up to your maximum!&00\r\n", TRUE, ch, 0, 0, TO_CHAR);
      act("&12$n&15 begins screaming, as $e &09flames&15 with ki, powering up to $s maximum!&00", TRUE, ch, 0, 0, TO_ROOM);
      }
    else if (GET_HIT(ch) < GET_MAX_HIT(ch) && GET_MAX_HIT(ch) < 10000000) {
      act("&15Your body is covered by a swell of &00&07pure &10energy &15as you &09power &15up&11!", FALSE, ch, 0, 0, TO_CHAR);
      act("&14$n &15body is covered by a swell of &00&07pure &10energy &15as you &09power &15up&11!", FALSE, ch, 0, 0, TO_ROOM);
      }
    else if (GET_HIT(ch) < GET_MAX_HIT(ch) && GET_MAX_HIT(ch) < 20000000) {
      act("&15Cracks form in the ground beneath you, and gusts of wind blow down most things in the surrounding area as you power up.&00", FALSE, ch, 0, 0, TO_CHAR);
      act("&14$n&15 begins to power up, and cracks form in the ground beneath them. As they reach maximum power gusts of wind blow outward, leveling most things nearby.&00", FALSE, ch, 0, 0, TO_ROOM);
      }
    else if (GET_HIT(ch) < GET_MAX_HIT(ch) && GET_MAX_HIT(ch) < 30000000) {
      act("&15Nearby mountains begin to shake and crumble as you power up.&00", FALSE, ch, 0, 0, TO_CHAR);
      act("&15Nearby mountains begin to shake and crumble as &14$n&15 powers up.&00", FALSE, ch, 0, 0, TO_ROOM);
      }
    else if (GET_HIT(ch) < GET_MAX_HIT(ch) && GET_MAX_HIT(ch) > 50000000) {
      act("&15The whole planet begins to shake as a swell of &00&07pure &10energy&15 surrounds your body, and you power up.&00", FALSE, ch, 0, 0, TO_CHAR);
      act("&15The whole planet begins to shake as a swell of &00&07pure &10energy&15 surrounds &14$n's&15 body, and $e powers up.&00", FALSE, ch, 0, 0, TO_ROOM);
      }
    if (!IS_NPC(ch) && GET_MANA(ch) >= GET_MAX_MANA(ch) * .6) {
     GET_HIT(ch) = GET_MAX_HIT(ch);
     GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch) * .6;  
     return;
    }
     
}
/* For preparing to deflect attacks */
ACMD(do_deflect)
{
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
  if (!GET_SKILL(ch, SKILL_DEFLECT)) {
   send_to_char("You don't know how to do that!\r\n", ch);
   return;
  }
  
  one_argument(argument, arg);  

  if (!*argument) {
   send_to_char("&16Syntax&11: &15deflect yes/nomore", ch);
   return;
   }  

  if (!str_cmp(arg,"nomore")) {
   send_to_char("&15You absorb the energy on your hands back in, and calm down.", ch);
   act("&14$n &15absorbs the energy on their hands, and stops watching their surroundings.&00", FALSE, ch, 0, 0, TO_NOTVICT);
   REMOVE_BIT(PLR_FLAGS(ch), PLR_DEFLECT);
   return;
   }

  if (!str_cmp(arg,"yes") && GET_MANA(ch) <= GET_MAX_MANA(ch) / 100) {
   send_to_char("You do not have enough ki to concentrate!", ch);
   return;
   }

  if(!str_cmp(arg,"yes") && GET_MANA(ch) >= GET_MAX_MANA(ch) / 100) {
   send_to_char("&15You stop for a momment as you &12focus &14ki&15 onto your hands and concentrate on anything that could attack you.&00\n", ch);
   act("&10$n &12focuses &15some &14ki&15 onto $s hands, as they begin to watch their surroundings carefully.&00", FALSE, ch, 0, 0, TO_NOTVICT);
   GET_MANA(ch) -= GET_MAX_MANA(ch) / 100;
   SET_BIT(PLR_FLAGS(ch), PLR_DEFLECT);
   return;
   }

}
/* Majin ingest */
ACMD(do_ingest)
{
   int MAX_HIT = 0, EXP = 0, MAX_MANA; 
   struct char_data * vict; 
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
   
   one_argument(argument, arg);
  
   if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return;
   }
     if (!GET_SKILL(ch, SKILL_INGEST)) {
    send_to_char("You can't do that!\r\n", ch);
    return;
   }

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return;
  }

    if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Ingest who?\r\n", ch);
      return;
    }
    
     if (GET_MAX_MANA(ch) >= 1000000) {
      act("&15Eating people will no longer benefit you!", FALSE, ch, 0, vict, TO_CHAR);
      return;
      }
     if (GET_MAX_HIT(ch) >= 1000000) {
      act("&15Eating people will no longer benefit you!", FALSE, ch, 0, vict, TO_CHAR);
      return;
      }
    if (IS_NPC(vict) && GET_MAX_HIT(ch) >= GET_MAX_HIT(vict))
  {
      act("&15You eat &09$N&15!", FALSE, ch, 0, vict, TO_CHAR);
      act("&09$n&15 eat &09$N&15!.", TRUE, ch, 0, vict, TO_NOTVICT);
      act("&09$n&15 eats you!&00", FALSE, ch, 0, vict, TO_VICT);
      sprintf(buf, "&16[&13Ingest &11Gain&15: &12Pl &15[&10%d&15] &14Ki &15[&10%d&15] &16]", GET_MAX_HIT(vict)/2, GET_MAX_HIT(vict)/4);      
      send_to_char(buf, ch);
  MAX_HIT = number(GET_HIT(vict)/2, GET_HIT(vict)/2);
  MAX_MANA = number(GET_HIT(vict)/4, GET_HIT(vict)/4);
  
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  
          if (IS_NPC(vict)) {
  	    extract_char(vict, TRUE);
  	    return;
  	    }
   }
 if (IS_NPC(vict) && GET_MAX_HIT(ch) <= GET_MAX_HIT(vict)) {
           act("&15You cannot over power them enough to eat them!", FALSE, ch, 0, vict, TO_CHAR);              
           return;
      }
   }       

/* For supressing powerlevel */
ACMD(do_powerdown)	
{   
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
    if (IS_MAJIN(ch)) {
     send_to_char("Majins can't powerdown, it's physicaly impossible!", ch);
     return;
     }
    if (GET_HIT(ch) <= GET_MAX_HIT(ch) && GET_HIT(ch) >= GET_MAX_HIT(ch) / 2) {
     GET_MANA(ch) += GET_HIT(ch) / 3;
     GET_HIT(ch) = GET_HIT(ch) + 1 - GET_HIT(ch);
     send_to_char("&15You concentrate and compress all your energy, you lose a lot of it in the process.&00\r\n", ch);
     return;
     }
    if (GET_HIT(ch) <= 1)
    {
  	act("&15You dropped your power level as low as you can already.&00", TRUE, ch, 0, 0, TO_CHAR); 
  return;
    }    
   else if (GET_HIT(ch) >= GET_MAX_HIT(ch))
    {  
  send_to_char("&15You concentrate and compress all your energy.&00\r\n", ch);

  GET_MANA(ch) += GET_HIT(ch) / 2;
  GET_HIT(ch) = GET_HIT(ch) + 1 - GET_HIT(ch);
  return;
    }
   else {
    send_to_char("You do not have enough powerlevel to charge your ki safely.\r\n", ch);
    }
}
/* Android trans/powerup ability */
ACMD(do_upgrade)	
{    
  int MAX_MANA = 0;
  int MAX_HIT = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

    one_argument(argument, arg);
   
   if (!IS_ANDROID(ch) && (GET_LEVEL(ch) >= 1)) {
    act("&15Only androids may aquire upgrades and upgrade information.&00", TRUE, ch, 0, 0, TO_CHAR);
    return;
     }
        
   if (!*argument && IS_ANDROID(ch)) {  
    send_to_char("&13Upgrade Chart&00\r\n", ch);
    send_to_char("&16-----------------------&00\r\n", ch);
    send_to_char("&15|&14Upgrade 1.0 &092000000   &15|&00\r\n", ch);
    send_to_char("&15|&14Upgrade 2.0 &0910000000  &15|&00\r\n", ch);
    send_to_char("&15|&14Upgrade 3.0 &0930000000  &15|&00\r\n", ch);
    send_to_char("&15|&14Upgrade 4.0 &0940000000 &15|&00\r\n", ch);
    send_to_char("&15|&14Upgrade 5.0 &0960000000 &15|&00\r\n", ch);
    send_to_char("&15|&14Upgrade 6.0 &0975000000 &15|&00\r\n", ch);
    send_to_char("&15|&14Upgrade &09Pl &10100       &15|&00\r\n", ch);
    send_to_char("&15|&14Upgrade &11lp &10100       &15|&00\r\n", ch);
    send_to_char("&15|&14Upgrade &14Ki &10100       &15|&00\r\n", ch);
    send_to_char("&15|&14Green numbers are for&15|&00\r\n", ch);
    send_to_char("&15|&14upgrade points       &15|&00\r\n", ch);
    send_to_char("&16-----------------------&00\r\n", ch);
    act("    &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
    if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2)) {
     sprintf(buf, "&15Current Base Pl&11: &14%d&00", GET_MAX_HIT(ch) - 5000000);
     send_to_char(buf, ch);
    }
    if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3)) {
     sprintf(buf, "&15Current Base Pl&11: &14%d&00", GET_MAX_HIT(ch) - 10000000);
     send_to_char(buf, ch);
    }
    if (PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4)) {
     sprintf(buf, "&15Current Base Pl&11: &14%d&00", GET_MAX_HIT(ch) - 20000000);
     send_to_char(buf, ch);
    }
    if (PLR_FLAGGED(ch, PLR_trans4) && !PLR_FLAGGED(ch, PLR_trans5)) {
     sprintf(buf, "&15Current Base Pl&11: &14%d&00", GET_MAX_HIT(ch) - 30000000);
     send_to_char(buf, ch);
    }
    if (PLR_FLAGGED(ch, PLR_trans5) && !PLR_FLAGGED(ch, PLR_trans6)) {
     sprintf(buf, "&15Current Base Pl&11: &14%d&00", GET_MAX_HIT(ch) - 50000000);
     send_to_char(buf, ch);
    }
    if (PLR_FLAGGED(ch, PLR_trans6)) {
     sprintf(buf, "&15Current Base Pl&11: &14%d&00", GET_MAX_HIT(ch) - 85000000);
     send_to_char(buf, ch);
    }
    return;
     }
 if (!str_cmp(arg,"pl") && GET_HOME(ch) <= 99) {
  act("&15You do not have enough upgrade points.&00\r\n", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 if (!str_cmp(arg,"lp") && GET_HOME(ch) <= 99) {
  act("&15You do not have enough upgrade points.&00\r\n", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 if (!str_cmp(arg,"ki") && GET_HOME(ch) <= 99) {
  act("&15You do not have enough upgrade points.&00\r\n", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 if (!str_cmp(arg,"pl") && GET_HOME(ch) >= 100) {
  act("&15You upgrade your powerlevel by 10000 points.&00\r\n", TRUE, ch, 0, 0, TO_CHAR);
  GET_HOME(ch) -= 100;
  GET_MAX_HIT(ch) += 10000;
  return;
  }
 if (!str_cmp(arg,"ki") && GET_HOME(ch) >= 100) {
  act("&15You upgrade your ki by 10000 points.&00\r\n", TRUE, ch, 0, 0, TO_CHAR);
  GET_HOME(ch) -= 100;
  GET_MAX_MANA(ch) += 10000;
  return;
  }
 if (!str_cmp(arg,"lp") && GET_HOME(ch) >= 100) {
  act("&15You upgrade your lp by 200 points.&00\r\n", TRUE, ch, 0, 0, TO_CHAR);
  GET_HOME(ch) -= 100;
  GET_PRACTICES(ch) += 200;
  return;
  }
 if (!str_cmp(arg,"1.0") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You have already upgraded to version 1.0!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"1.0") && GET_MAX_HIT(ch) < 2000000) {
  act("&15You are not ready to upgrade yet!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  } 

 if (!str_cmp(arg,"1.0") && GET_MAX_HIT(ch) >= 2000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);
     
  MAX_HIT = number(5000000, 5000000);
  MAX_MANA = number(5000000, 5000000);
  
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  
  act("&11$n&15 sits down and begins to reprogram theirself!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&15You sit down and reprogram yourself&15!&00", TRUE, ch, 0, 0, TO_CHAR);
return;
  }


if (!str_cmp(arg,"2.0") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15You have already upgraded to version 2.0!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"2.0") && GET_MAX_HIT(ch) - 5000000 < 10000000) {
  act("&15You are not ready to upgrade yet!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"2.0") && GET_MAX_HIT(ch) - 5000000 >= 10000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans2);

 MAX_HIT = number(5000000, 5000000);
 MAX_MANA = number(5000000, 5000000);

 GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
 GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&11$n&15 sits down and begins to reprogram theirself!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&15You sit down and reprogram yourself&15!&00", TRUE, ch, 0, 0, TO_CHAR);
return;
  }

if (!str_cmp(arg,"3.0") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15You have already upgraded to version 3.0!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"3.0") && GET_MAX_HIT(ch) - 10000000 < 30000000) {
  act("&15You are not ready to upgrade yet!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"3.0") && GET_MAX_HIT(ch) - 10000000 >= 30000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans3);

 MAX_HIT = number(10000000, 10000000);
 MAX_MANA = number(10000000, 10000000);

 GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
 GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&11$n&15 sits down and begins to reprogram theirself!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&15You sit down and reprogram yourself&15!&00", TRUE, ch, 0, 0, TO_CHAR);
return;
  }

if (!str_cmp(arg,"4.0") && PLR_FLAGGED(ch, PLR_trans4)) {
  act("&15You have already upgraded to version 4.0!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"4.0") && GET_MAX_HIT(ch) - 20000000 < 40000000) {
  act("&15You are not ready to upgrade yet!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"4.0") && GET_MAX_HIT(ch) - 20000000 >= 40000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans4);

 MAX_HIT = number(10000000, 10000000);
 MAX_MANA = number(10000000, 10000000);

 GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
 GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&11$n&15 sits down and begins to reprogram theirself!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&15You sit down and reprogram yourself&15!&00", TRUE, ch, 0, 0, TO_CHAR);
return;
  }

if (!str_cmp(arg,"5.0") && PLR_FLAGGED(ch, PLR_trans5)) {
  act("&15You have already upgraded to version 5.0!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"5.0") && GET_MAX_HIT(ch) - 30000000 < 60000000) {
  act("&15You are not ready to upgrade yet!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"5.0") && GET_MAX_HIT(ch) - 30000000 >= 60000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans5);

 MAX_HIT = number(20000000, 20000000);
 MAX_MANA = number(20000000, 20000000);

 GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
 GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&11$n&15 sits down and begins to reprogram theirself!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&15You sit down and reprogram yourself&15!&00", TRUE, ch, 0, 0, TO_CHAR);
return;
  }

if (!str_cmp(arg,"6.0") && PLR_FLAGGED(ch, PLR_trans6)) {
  act("&15You have already upgraded to version 6.0!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"6.0") && GET_MAX_HIT(ch) - 50000000 < 75000000) {
  act("&15You are not ready to upgrade yet!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"6.0") && GET_MAX_HIT(ch) - 50000000 >= 75000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans6);

 MAX_HIT = number(35000000, 35000000);
 MAX_MANA = number(35000000, 35000000);

 GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
 GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&11$n&15 sits down and begins to reprogram theirself!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&15You sit down and reprogram yourself&15!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
   
}
/* Android and Bio-Android ability */
ACMD(do_absorb)	
{   
   int MAX_HIT = 0, EXP = 0, MAX_MANA; 
   struct char_data * vict; 
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
 
   one_argument(argument, arg);
   if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return;
   }
     if (!GET_SKILL(ch, SKILL_ABSORB)) {
  send_to_char("You can't do that!\r\n", ch);
  return;
}

  if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return;
  }
  if (GET_POS(ch) == POS_SITTING) {
     send_to_char("You are sitting stupid!", ch);
     return;
     }
  if (IS_BIODROID(ch) && !PLR_FLAGGED(ch, PLR_TAIL)) {
     send_to_char("You need a tail to absorb, being a Bio-Android!\r\n", ch);
     return;
  }
     if (!(vict = get_char_room_vis(ch, arg))) {
     if (FIGHTING(ch) && IN_ROOM(ch) == IN_ROOM(FIGHTING(ch))) {
      vict = FIGHTING(ch);
     } else {
      send_to_char("Absorb who?\r\n", ch);
      return;
      }
     }
     if (vict == ch) {
       send_to_char("Aren't we funny today...\r\n", ch);
       return;
     }
      if (GET_MAX_HIT(ch) >= 1000000 || GET_MAX_MANA(ch) >= 2500000) {
       if (IS_BIODROID(ch) && GET_LBS(ch) < number(1, 100)) {
        act("&15You fail to absorb some energy!", FALSE, ch, 0, vict, TO_CHAR);
        act("&15$n fails to absorb some of your  energy!", FALSE, ch, 0, vict, TO_VICT);
        act("&15$n fails to absorb some of $N's&15 energy!", FALSE, ch, 0, vict, TO_NOTVICT);
        WAIT_STATE(ch, PULSE_VIOLENCE * 1);
        return;
        }
       if (IS_ANDROID(ch) && GET_UBS(ch) < number(1, 100)) {
        act("&15You fail to absorb some energy!", FALSE, ch, 0, vict, TO_CHAR);
        act("&15$n fails to absorb some of your  energy!", FALSE, ch, 0, vict, TO_VICT);
        act("&15$n fails to absorb some of $N's&15 energy!", FALSE, ch, 0, vict, TO_NOTVICT);
        WAIT_STATE(ch, PULSE_VIOLENCE * 1);
        return;
        }
       act("&15You absorb some of $N&15's energy!", FALSE, ch, 0, vict, TO_CHAR);
       act("&15$n absorbs some of your energy!", FALSE, ch, 0, vict, TO_VICT);
       act("&15$n absorbs some of $N&15's energy!", FALSE, ch, 0, vict, TO_NOTVICT);
       sprintf(buf, "&16[&13Absorb &11Recharge&15: &12Pl &15[&10%d&15] &14Ki &15[&10%d&15] &16]&00\r\n", MIN(GET_MAX_HIT(ch) / 40, MAX(1, 750000)), MIN(GET_MAX_MANA(ch) / 150, MAX(1, 750000)));
       send_to_char(buf, ch);
       MAX_HIT = MIN(GET_MAX_HIT(ch) / 40, MAX(1, 1000000));
       MAX_MANA = MIN(GET_MAX_MANA(ch) / 150, MAX(1, 1000000));
       GET_MANA(ch) = GET_MANA(ch) + MAX_MANA;
       GET_HIT(ch) = GET_HIT(ch) + MAX_HIT;
       GET_HIT(vict) = GET_HIT(vict) - MAX_HIT;
       GET_MANA(vict) = GET_MANA(vict) - MAX_MANA;
       damage(ch, vict, GET_LEVEL(ch), SKILL_ABSORB);
       WAIT_STATE(ch, PULSE_VIOLENCE * 2);
       WAIT_STATE(vict, PULSE_VIOLENCE);
       return;
      }
    if (IS_NPC(vict) && GET_MAX_HIT(ch) >= GET_MAX_HIT(vict))
  {
      act("&15You absorb &09$N&15 into your body!", FALSE, ch, 0, vict, TO_CHAR);
      act("&09$n&15 absorbs &09$N&15 into $s body!.", TRUE, ch, 0, vict, TO_NOTVICT);
      act("&09$n&15 absorbs you into $s body!&00", FALSE, ch, 0, vict, TO_VICT);
      sprintf(buf, "&16[&13Absorb &11Gain&15: &12Pl &15[&10%d&15] &14Ki &15[&10%d&15] &16]", GET_MAX_HIT(vict)/2, GET_MAX_HIT(vict)/4);
      send_to_char(buf, ch);
  MAX_HIT = number(GET_MAX_HIT(vict)/2, GET_MAX_HIT(vict)/2);
  MAX_MANA = number(GET_MAX_HIT(vict)/4, GET_MAX_HIT(vict)/4);
  EXP = number(GET_MAX_HIT(vict)/500, GET_MAX_HIT(vict)/500);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  
          if (IS_NPC(vict)) {
  	    extract_char(vict, TRUE);
  	    return;
  	    }
   }
 if (IS_NPC(vict) && GET_MAX_HIT(ch) <= GET_MAX_HIT(vict)) {
           act("&15You cannot over power them enough to absorb them!", FALSE, ch, 0, vict, TO_CHAR);              
           return;
      }
   }              
   
/* Blow yourself up or someone else up */   
ACMD(do_selfdestruct)
{
	
struct char_data * vict;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0'; 
one_argument(argument, arg);
  
  if (!GET_SKILL(ch, SKILL_DESTRUCT)) {
  send_to_char("You can't do that!\r\n", ch);
  return;
    }
  if (ROOM_FLAGGED(ch->in_room, ROOM_NOQUIT))
      {
      send_to_char("You cannot self destruct here!\r\n", ch);
      return;
     }
  if (PLR_FLAGGED(ch, PLR_KILLER)) {
     send_to_char("&15You can't self destruct till the challenge process is over&11!&00\r\n", ch);
     return;
    }
  if (CLN_FLAGGED(ch, CLN_Death)) {
   send_to_char("&15You are pretty much dead already&00\r\n", ch);
   return;
  }
  if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Self destruct on who?\r\n", ch);
      return;
    }  

  if (!IS_NPC(vict) && GET_MAX_HIT(ch) >= GET_MAX_HIT(vict))
  {
      act("&15You self destruct taking &09$N&15 with you!&00", FALSE, ch, 0, vict, TO_CHAR);
      act("&09$n &15self-destructs taking &09$N with $m&00.", TRUE, ch, 0, vict, TO_NOTVICT);
      act("&09$n&15 self-destructs killing you with the explosion!&00", FALSE, ch, 0, vict, TO_VICT);
      sprintf(buf, "&16[&09Self Destruct&16] &14%s&15 has self destructed &10%s.&00\r\n", GET_NAME(ch), GET_NAME(vict));
       send_to_all(buf);
  if (vict == ch) {
    char_from_room(vict);
    char_to_room(vict, r_death_start_room);
    SET_BIT(CLN_FLAGS(vict), CLN_Death);
    GET_DTIMER(vict) = 3600;
    GET_POS(vict) = POS_STANDING;
    look_at_room(vict, 0);
    return;
    }
  else
   SET_BIT(CLN_FLAGS(ch), CLN_Death);
   SET_BIT(CLN_FLAGS(vict), CLN_Death);
   GET_DTIMER(ch) = 3600;
   GET_DTIMER(vict) = 3600;
   char_from_room(vict);
   char_to_room(vict, r_death_start_room);
   GET_POS(vict) = POS_STANDING;
   char_from_room(ch);
   char_to_room(ch, r_death_start_room);
   GET_HIT(ch) = GET_MAX_HIT(ch);
   GET_MANA(ch) = GET_MAX_MANA(ch);
   GET_MOVE(ch) = GET_MOVE(ch);
   GET_POS(ch) = POS_STANDING;
   look_at_room(ch, 0);
   look_at_room(vict, 0);
  } 
  
  else if (!IS_NPC(vict) && GET_MAX_HIT(ch) <= GET_MAX_HIT(vict)){
  act("&15You self destruct but &09$N&15 does not die!&00", FALSE, ch, 0, vict, TO_CHAR);
  act("&09$n &15self-destructs but fails to take &09$N with $m&00.", TRUE, ch, 0, vict, TO_NOTVICT);
  act("&09$n&15 self-destructs but fails to kill you!&00", FALSE, ch, 0, vict, TO_VICT);
  sprintf(buf, "&16[&09Self Destruct&16] &14%s&15 attempts to self destruct &10%s&15, but fails&00.\r\n", GET_NAME(ch), GET_NAME(vict));
       send_to_all(buf);
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  GET_HIT(ch) = GET_MAX_HIT(ch);
  GET_MANA(ch) = GET_MAX_MANA(ch);
  GET_MOVE(ch) = GET_MOVE(ch);
  GET_POS(ch) = POS_STANDING;
  look_at_room(ch, 0);
  }

}

/* Detect someone's powerlevel */
ACMD(do_powersense)
{
	struct char_data * vict;
        char hit[50];
        one_argument(argument, arg);
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
 
 if (!GET_SKILL(ch, SKILL_POWERSENSE)) {
  send_to_char("You can't do that!\r\n", ch);
  return;
}


   if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Detect who's power?\r\n", ch);
      return;
    } 
   if (IS_ANDROID(vict)) {
    send_to_char("You can't sense androids!\r\n", ch);
    return;
   }

   if (vict) {		/* victim */
    commafmt(hit, sizeof(hit), GET_HIT(vict));
  act("&00&15You pause for a moment as you stare at $N&00&07.", FALSE, ch, 0, vict, TO_CHAR);
  act("&00&15$n stares at $N&00&07 momentarily..", TRUE, ch, 0, vict, TO_NOTVICT);
  act("&00&15$n stares at you momentarily.&00", FALSE, ch, 0, vict, TO_VICT);
    sprintf(buf, "&14%s&00&15 has a power level of &10%s&15.&00\r\n",GET_NAME(vict), hit);
     send_to_char(buf, ch);
  }
}
/* Saiyan and Halfbreed trans */
ACMD(do_ssj)	
{    
 
  int MAX_MANA = 0, MAX_HIT = 0, HITROLL = 0, DAMROLL = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

    one_argument(argument, arg);
    WAIT_STATE(ch, PULSE_VIOLENCE * .5);
    if ((!IS_saiyan(ch)) && (!IS_HALF_BREED(ch)))
    {
      act("You're not part saiyan!", TRUE, ch, 0, 0, TO_CHAR);
      return;
    }
    else if (CLN_FLAGGED(ch, CLN_RAGE)) {
     send_to_char("Not while you are in a rage, your power is already stressed.\r\n", ch);
     return;
    }
   else if (!*argument)
    {
act("        &10o&12_&09-------------------------&12_&10o&00\r\n         &09|   &11Super Saiyan Forms&00    &09|&00\r\n        &10o&12_&09-------------------------&12_&10o&00\r\n         &09|&10You can do the following:&09|&00\r\n         &09|&09-------------------------&09|&00\r\n         &09|&15Stage:  Power Level Req.:&09|&00 ", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10First     &144,000,000&00     &09|&00", TRUE, ch, 0, 0, TO_CHAR);
if (IS_saiyan(ch)) {
act("         &09| &10Second    &1430,000,000&00    &09|&00", TRUE, ch, 0, 0, TO_CHAR);
}
if (IS_HALF_BREED(ch)) {
act("         &09| &10Second    &1430,000,000&00    &09|&00", TRUE, ch, 0, 0, TO_CHAR);
}
act("         &09| &10Third     &1465,000,000&00    &09|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &09|! &10LSSJ     &14100,000,000&00  &09!|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10Fourth    &14100,000,000&00   &09|&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10o&12_&09-------------------------&12_&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10All transes marked with ! are not complete&00", TRUE, ch, 0, 0, TO_CHAR);
return;
}

/*------------------        Oozaru checks, no SSj in oozaru           -----------------*/

if (!str_cmp(arg,"first") && PRF_FLAGGED(ch, PRF_OOZARU)) {
  act("&15You can't go super saiya-jin while in oozaru form!!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"second") && PRF_FLAGGED(ch, PRF_OOZARU)) {
  act("&15You can't go super saiya-jin while in oozaru form!!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"third") && PRF_FLAGGED(ch, PRF_OOZARU)) {
  act("&15You can't go super saiya-jin while in oozaru form!!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"fourth") && PRF2_FLAGGED(ch, PRF2_GO)) {
  act("&15You can't go super saiya-jin fourth while in golden oozaru form!!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"fourth") && PRF_FLAGGED(ch, PRF_OOZARU)) {
  act("&15You can't go super saiya-jin while in oozaru form!!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

/*------------------  Checks to see if already in that stage if ssj   -----------------*/


if (!str_cmp(arg,"second") && !PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"third") && !PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"fourth") && !PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*----------------------------------  Reverting  --------------------------------------*/

  if (!str_cmp(arg,"revert") && PRF2_FLAGGED(ch, PRF2_GO)) {
    send_to_char("You can't revert while Golden Oozaru!\r\n", ch);
    return;
  }

  if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans4)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans4);

  MAX_HIT = number(120000000, 120000000);
  MAX_MANA = number(120000000, 120000000);
  HITROLL = number(50, 50);
  DAMROLL = number(50, 50);


  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - 50;
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - 50;
  act("&11$n&15's &12hair&15 and &12eyes&15 turn back to normal as $e stops &09flaming&00 &11yellow&00.", TRUE, ch, 0, 0, TO_ROOM);
  act("&15Your &13rage&15 ebbs as your &12hair&15 and &12eyes&15 turn back to normal and you stop &09flaming&00 &11yellow.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }


  if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans3)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(70000000, 70000000);
  MAX_MANA = number(70000000, 70000000);
  HITROLL = number(30, 30);
  DAMROLL = number(30, 30);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - HITROLL;
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - DAMROLL;
  act("&11$n&15's &12hair&15 and &12eyes&15 turn back to normal as $e stops &09flaming&00 &11yellow&00.", TRUE, ch, 0, 0, TO_ROOM);
  act("&15Your &13rage&15 ebbs as your &12hair&15 and &12eyes&15 turn back to normal and you stop &09flaming&00 &11yellow.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans2)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(35000000, 35000000);
  MAX_MANA = number(35000000, 35000000);
  HITROLL = number(15, 15);
  DAMROLL = number(15, 15);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - HITROLL;
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - DAMROLL;
  act("&11$n&15's &12hair&15 and &12eyes&15 turn back to normal as $s stops &09flaming&00 &11yellow&00.", TRUE, ch, 0, 0, TO_ROOM);
  act("&15Your &13rage&15 ebbs as your &12hair&15 and &12eyes&15 turn back to normal and you stop &09flaming&00 &11yellow.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans1)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(10000000, 10000000);
  MAX_MANA = number(10000000, 10000000);
  HITROLL = number(5, 5);
  DAMROLL = number(5, 5);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - HITROLL;
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - DAMROLL;
  act("&11$n&15's &12hair&15 and &12eyes&15 turn back to normal as $e stops &09flaming&00 &11yellow&00.", TRUE, ch, 0, 0, TO_ROOM);
  act("&15Your &13rage&15 ebbs as your &12hair&15 and &12eyes&15 turn back to normal and you stop &09flaming&00 &11yellow.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

/*-------------------------------------------------------------------------------------*/
/*------------------------------------ SSj 1 ----------------------------------------- */
/*-------------------------------------------------------------------------------------*/


if (!str_cmp(arg,"first") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15Your already in that form of super saiya-jin!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"first") && PRF_FLAGGED(ch, PRF_MYSTIC)) {
  act("&15What do you want to do, blow up the universe?!  You can't go &11SSJ&15 while in &14Mystic&15 form!&00", TRUE, ch,0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"first") && PRF_FLAGGED(ch, PRF_MYSTIC2)) {
  act("&15What do you want to do, blow up the universe?!  You can't go &11SSJ&15 while in &14Mystic&15 form!&00", TRUE, ch,0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"first") && PRF_FLAGGED(ch, PRF_MYSTIC3)) {
  act("&15What do you want to do, blow up the universe?!  You can't go &11SSJ&15 while in &14Mystic&15 form!&00", TRUE, ch,0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) < 4000000) {
  act("&15You do not have the power to attain that form of super saiya-jin!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }


 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) >= 4000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(10000000, 10000000);
  MAX_MANA = number(10000000, 10000000);
  HITROLL = number(5, 5);
  DAMROLL = number(5, 5);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;

  act("&00&11$n &15starts to tremble, their muscles bulge, their eyes go pure &00w&15hit&00e&15 as &00&03g&00&11olde&00&03n &00li&00&14gh&00&06t&00&14ni&00ng &15starts to shower downwards. The earth begins to crack and break as the viens in &03$n's&15 arms begin to bulge! Then in an explosion of &00a&14ur&00a &15and &00&03p&00&11owe&00&03r&15, their eyes become &00&02j&00&10ad&00&02e green&15, their hair and aura &00&03g&11olde&00&03n &00&11yellow&15! They have become &00&03S&00&11upe&00&03r &00&11S&00&03aiya&00&11n &00O&00&14n&00&06e&15!&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&00&15Your throat catches, sweat starts to pour out from ya body as you feel something &00&09s&00&01na&00&09p &15in the back of your mind. The earth seems to stand still as bolt upon bolt of &00&03g&00&11olde&00&03n &00li&00&14gh&00&06t&00&14ni&00ng &15comes crashing around you. The earth begins to shake beneath your feet, as large boulders start to rip from the ground. Your body shakes as you feel your muscles tighten and grow, a slight &00&02j&00&10ad&00&02e &15sheen coming over your eyes as you snap your head back and let out a roar as your hair and aura explode into a rich &00&03g&00&11olde&00&03n &15color. You have achieved &00&03S&00&11upe&00&03r &00&11S&00&03aiya&00&11n &00O&00&14n&00&06e&15!&00", TRUE, ch, 0, 0, TO_CHAR);
 return;
  }
/*-------------------------------------------------------------------------------------*/
/*------------------------------------ SSj 2 ----------------------------------------- */
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"second") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15Your already in that form of super saiya-jin!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 10000000 < 30000000 && IS_saiyan(ch)) {
  act("&15You do not have the power to attain that form of super saiya-jin!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 10000000 < 30000000 && IS_HALF_BREED(ch)) {
  act("&15You do not have the power to attain that form of super saiya-jin!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if ((!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 10000000 >= 30000000 && IS_saiyan(ch)) || (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 10000000 >= 30000000 && IS_HALF_BREED(ch))) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(25000000, 25000000);
  MAX_MANA = number(25000000, 25000000);
  HITROLL = number(10, 10);
  DAMROLL = number(10, 10);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;

  act("&00&15$n's eyes go pure &00w&15hit&00e&15 as their arms &00&09s&00&01na&00&09p&00 &15back, the silence shattering from the &00&01r&00&09ag&00&01e&00 &15filled scream that escapes their lungs. Their hair starts to get spikier, the viens in their arm bulge even more then before as you feel their power grow extremely! Bolts of &00&04b&00&12l&00&14u&00e li&14gh&00&06t&00&14ni&00ng &15start to outline their form! They have achieved &00&03A&00&11s&00&00&15c&00en&15d&00&14e&00&06d&00 &11S&00&03aiya&00&11n&15!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&00&15Your eyes go pure &00w&15hit&00e&15 as your arms &00&09s&00&01na&00&09p&00 &15back, you look to the sky and scream out! You feel all the &00&01r&00&09ag&00&01e&00 &15contained in you bubble to the surface as you body starts to change. Your hair gets spikier, the viens in your arms bulge and you feel your powerlevel grow tremendously. As you start to settle down, bolts of &00&04b&00&12l&00&14u&00e li&14gh&00&06t&00&14ni&00ng &15start to come from your body and cover you in a field of electricity.! You have achieved &00&03A&00&11s&00&00&15c&00en&15d&00&14e&00&06d&00 &11S&00&03aiya&00&11n&15!&00", TRUE, ch, 0, 0, TO_CHAR); return;
  }


/*------------------ What Half-breeds can't do ----------------------------------------*/
if (!str_cmp(arg,"fourth") && IS_HALF_BREED(ch)) {
  act("&15You can't go past super saiyan 3.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*------------------------------------ SSj 3 ----------------------------------------- */
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"third") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15Your already in that form of super saiya-jin!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 35000000 < 65000000) {
  act("&15You do not have the power to attain that form of super saiya-jin!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }


 if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 35000000 >= 65000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(35000000, 35000000);
  MAX_MANA = number(35000000, 35000000);
  HITROLL = number(15, 15);
  DAMROLL = number(15, 15);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;

  act("&00&15$n stands here, their arms tight to their sides. The earth starts to begin to quake lightly as their hair begins to lengthen and thicken! A bright &11yellow&00 &15aura explodes around them as&00 li&14gh&00&06t&00&14ni&00ng&15 starts to crackle from their form. The earth starts to quake harder as their eyebrows disappear, their forehead slopes slightly, and their powerlevel grows exponetionally. And with a final explosion of &00&03l&00&11i&00g&11h&00&03t&00 &15and power, they have achieved &00&03S&00&11upe&00&03r &00&11S&00&03aiya&00&11n&00 &00&06T&00&14h&00r&14e&00&06e&15!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&00&15You stand there, your arms at your side. Your &00&01r&00&09ag&00&01e&00 &15slowly boils to the surface as your body starts to twitch. Slowly, you feel your hair start to thicken and lengthen, your muscles bulge as&00 li&14gh&00&06t&00&14ni&00ng&15 starts to crackle around your body quickly! Your forehead slopes slightly and your eyebrows disappear, and your eyes glow a slight &00&02j&00&10ad&00&02e&00 &15hue as the planet itself starts to shake! You have achieved &00&03S&00&11upe&00&03r &00&11S&00&03aiya&00&11n&00 &00&06T&00&14h&00r&14e&00&06e&15!&00", TRUE, ch, 0, 0, TO_CHAR);

send_to_all("&15The &10p&00&02l&16a&00&03n&16e&00&02t&10s&15 shake and tremble as a &16m&00&01a&09s&15s&09i&00&01v&16e&15 power arises!&00\r\n");
return;
}
/*----------------------------------ssj 4---------------------------------*/

if (!str_cmp(arg,"fourth") && PLR_FLAGGED(ch, PLR_trans4)) {
  act("&15Your already in that form of super saiya-jin!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"fourth") && GET_MAX_HIT(ch) - 70000000 < 100000000) {
  act("&15You do not have the power to attain that form of super saiya-jin!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }


 if (!str_cmp(arg,"fourth") && GET_MAX_HIT(ch) - 70000000 >= 100000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans4);

  MAX_HIT = number(50000000, 50000000);
  MAX_MANA = number(50000000, 50000000);
  HITROLL = number(20, 20);
  DAMROLL = number(20, 20);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;

  act("&16You stare at the &16m&15o&14o&00&07n&16, your mind along with your eyes going blank. In the &00&01d&09a&16r&15k&00&07n&16e&09s&00&01s &16of your mind resides a beast waiting to be unleashed. Your bones begin to twist, crack, and grow. You feel your spine lengthen and your  mouth push outwards from your face. &16You let out a howl of &00&01r&09a&00&01g&09e&16 as you feel the bones in your arms grow to painfully to a gigantic size before your skin and muscle can compensate. The air crackles with &00&07e&14n&00&06er&14g&00&07y&16 as you open your mouth and roar to the heavens massive fangs grow from your jaw as your eyes turn &00&01b&09l&16o&15o&00&01d r&09e&00&01d&16. Hair begins to sprout all over your body as massive claws extend from your fingers. &16The &00&01d&09a&16r&15k &16B&15l&00&07a&15c&16k hair that had grown on your body now turns a bright &11g&00&03o&11l&00&03d&16 as your primal form explodes in power! You have become &00&03G&11o&00&03l&11d&00&03e&11n &00&03O&11o&16z&15a&11r&00&03u&16! But you feel something crawl from the back of your mind, and your rage starts to settle. &16You stare back up at the &16m&15o&14o&00&07n&16 and you feel your body start to boil with unimaginable power while you try to force your body back into its humanoid form. A massive &00&03g&11o&00&03l&11d&00&03e&11n&16 orb of &00&07e&14n&00&06er&14g&00&07y&16 surrounds your body as you transform. Your body shrinks back down into its once human like shape, but there are many changes. &16Your body is covered with &00&01d&09a&16r&15k &00&01r&09e&00&01d&16 fur, your eyes are rimmed in &00&01r&09e&00&01d&16 and are now the color of &00&03l&11i&16q&15u&11i&00&03d a&11m&16b&15e&00&03r&16. You let out a scream as the &00&07e&14n&00&06er&14g&00&07y&16 ball holding you explodes and you become a &11S&00&03u&11p&00&03e&11r&00&03 S&11a&00&03i&11y&00&03a&11n&00&01 F&09o&16u&15r!", FALSE, ch, 0, 0, TO_CHAR);
  act("&14$n &16eyes stray up to the &16m&15o&14o&00&07n&16, and then &14f&15r&14e&00&07e&15z&14e&16. You can feel their power rising up inside them, like a &00&01v&09o&00&03l&11c&15a&09n&00&01o&16 about to blow. Their body starts to stretch and grow, the sounds of bones cracking and stretching fill the air. They grow &00&01d&09a&16r&15k &16B&15l&00&07a&15c&16k hair all over their body as they reach towering hieghts. &16Then their &00&01r&09e&00&01d&16 eyes go blank as a large orb of &00&07e&14n&00&06er&14g&00&07y&16 surrounds their body. The earth shakes beneath the new power being formed within the globe of &00&07e&14n&00&06er&14g&00&07y&16. Their power reaches massive proportions! They appear, with &00&01r&09e&00&01d &16fur covering their entire body, their eyes now &00&01r&09i&16m&15m&09e&00&01d&16 in &00&01r&09e&00&01d &16with thier bodies surrounded with a &00&03g&11o&00&03l&11d&00&03e&11n&16 aura! They have achieved &11S&00&03u&11p&00&03e&11r&00&03 S&11a&00&03i&11y&00&03a&11n&00&01 F&09o&16u&15r!&00", FALSE, ch, 0, 0, TO_ROOM);

send_to_all("&15The &10p&00&02l&16a&00&03n&16e&00&02t&10s&15 shake and tremble as someone achieves &00&03S&00&11upe&00&03r &00&11S&00&03aiya&00&11n&00 &01F&16o&00&03u&00&01r&15!&00\r\n");
return;
  }
}
/*-----------------------------------end of ssj----------------------------------------*/
/* Used instead of hide by morts */
ACMD(do_stealth)
{

  struct affected_type af;
  if (!GET_SKILL(ch, SKILL_STEALTH)) {
  send_to_char("You can't do that!\r\n", ch);
  return;
  }
 else
  SET_BIT(AFF_FLAGS(ch), AFF_HIDE);

  act("&11$n&15's skin color flickers as they fade into the surroundings.", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You pause for a moment and meld to your surroundings.&00", TRUE, ch, 0, 0, TO_CHAR);

}
/* Icer trans */
ACMD(do_transform)	
{    
	
  int MAX_MANA = 0, MAX_HIT = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
    
    one_argument(argument, arg);
    WAIT_STATE(ch, PULSE_VIOLENCE * .5);
    if (!IS_icer(ch))
    {
      act("You're not an icer!", TRUE, ch, 0, 0, TO_CHAR);
      return;
    } 
   else if (!*argument && IS_icer(ch))
    { 
act("        &10o&12_&12-------------------------&12_&10o&00\r\n         &12|   &12      Icer Forms&00      &12|&00\r\n        &10o&12_&12-------------------------&12_&10o&00\r\n         &12|&10You can do the following:&12|&00\r\n         &12|&12-------------------------&12|&00\r\n         &12|&15Stage:  Power Level Req.:&12|&00 ", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10First      &141000000&00      &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10Second     &1420000000&00     &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10Third      &14400000000&00    &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10Fourth     &14700000000&00    &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10o&12_&12-------------------------&12_&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
return;
}     

/*------------------  Checks to see if already in that stage     -----------------*/

if (!str_cmp(arg,"second") && !PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"third") && !PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"fourth") && !PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*----------------------------------  Reverting  --------------------------------------*/

 if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans4)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans4);
    
  MAX_HIT = number(75000000, 75000000);
  MAX_MANA = number(75000000, 75000000);
  
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&11$n&15's power drops as they revert to their normal form.&00.", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You feel much weaker as you revert to normal form.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }


else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans3)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
    
  MAX_HIT = number(40000000, 40000000);
  MAX_MANA = number(40000000, 40000000);
  
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&11$n&15's power drops as they revert to their normal form.&00.", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You feel much weaker as you revert to normal form.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans2)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
    
  MAX_HIT = number(20000000, 20000000);
  MAX_MANA = number(20000000, 20000000);
  
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&11$n&15's power drops as they revert to their normal form.&00.", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You feel much weaker as you revert to normal form.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans1)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
    
  MAX_HIT = number(2000000, 2000000);
  MAX_MANA = number(2000000, 2000000);
  
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&11$n&15's power drops as they revert to their normal form.&00.", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You feel much weaker as you revert to normal form.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

/*-------------------------------------------------------------------------------------*/
/*------------------------------------ TR 1 ----------------------------------------- */
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"first") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15Your already in that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) < 1000000) {
  act("&15You do not have the power to attain that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  } 

 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) >= 1000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);
     
  MAX_HIT = number(2000000, 2000000);
  MAX_MANA = number(2000000, 2000000);
  
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  
  act("&15$n &16g&15r&00&01o&09w&16l&15s, bending over, seemingly in &00&01p&09a&15i&00n&15 as his body begins to &00&01p&09u&16l&00&01s&09a&15t&00e&15 with &00e&14n&00&06er&14g&00y&15. $n&15's body shakes as he lets out a soft &00&01s&00&05c&16r&15e&00&01a&09m&15, the &16h&15o&00r&16n&15s&15 expanding out from the &00s&15k&16u&15l&00l&15 upwards. $n&15's neck &00&01l&00&05e&13n&15g&00t&15h&13e&00&05n&00&15 and &16t&00&01h&09i&16ck&09e&00&01n&16s&15, becoming heavily muscled. $n&15 drops down to one knee, thier torso grows thick, growing to compensate for the massive neck. The arms lengthen, the cracking of the &00b&15o&16n&15e&00s&15 can be heard as they curl newly massive hands into fists. $n&15 stands up slowly, thier legs changing to match the size of the rest of thier body. $n&15's tail also grows, becoming thick, and nearly twice the length of thier body. $n&15 has achieved thier &00&06F&00&02i&10r&00&06s&00&02t &16T&15r&00&06a&14n&00s&14f&00&06or&14m&00a&14t&00&06i&15o&16n&15!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&15You &15g&15r&00&01o&09w&16l&15, bending over, seemingly in &00&01p&09a&15i&00n&15 as your body begins to &00&01p&09u&16l&00&01s&09a&15t&00e&15 with &00e&14n&00&06er&14g&00y&15. Your body shakes as you lets out a soft &00&01s&00&05c&16r&15e&00&01a&09m&15, the &16h&15o&00r&16n&15s&15 expanding out from your &00s&15k&16u&15l&00l&15 upwards. Your neck &00&01l&00&05e&13n&15g&00t&15h&13e&00&05n&00&01s&15 and &16t&00&01h&09i&16ck&09e&00&01n&16s&15, becoming heavily muscled. &15You drops down to one knee, your torso grows thick, growing to compensate for the massive neck. The arms lengthen, the cracking of the &00b&15o&16n&15e&00s&15 can be heard as you curls your newly massive hands into fists. You stand up slowly, your legs changing to match the size of the rest of your body. Your tail also grows, becoming thick, and nearly twice the length of your body. You have achieved your &00&06F&00&02i&10r&00&06s&00&02t &16T&15r&00&06a&14n&00s&14f&00&06or&14m&00a&14t&00&06i&15o&16n&15!&00", TRUE, ch, 0, 0, TO_CHAR); return;
  }
/*-------------------------------------------------------------------------------------*/
/*------------------------------------ TR 2 ----------------------------------------- */
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"second") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15Your already in that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 2000000 < 20000000) {
  act("&15You do not have the power to attain that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  } 

 if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 2000000 >= 20000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans2);
     
  MAX_HIT = number(18000000, 18000000);
  MAX_MANA = number(18000000, 18000000);
  
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  
  act("$n&15 lets out a &16h&15o&00w&15l deep from thier throat, grabbing thier arms, as if in massive &00&01p&09a&15i&00n&15. Thier body seems to change, &10s&00&02h&16ri&15nk&16i&00&02n&10g&15 down from what it once was. Thier spine curves, and the &00s&15k&16u&15l&00l &01l&00&05e&13n&15g&00t&15h&13e&00&05n&00&01s&15. Thier &16h&15o&00r&15n&16s&15 recede into thier head, replaced by six curved &16h&15o&00r&15n&16s&15 coming up from the sides of thier &00s&15k&16u&15l&00l&15, three on each side.  The nose disappears and the face moves outward, with a sickening sound. They gain a &10l&00&02i&00&03z&16a&10r&00&02d&15 like appearance as they achieve thier &11S&00&03e&00c&15o&11n&00&03d &16T&15r&00&06a&14n&00s&14f&00&06or&14m&00a&14t&00&06i&15o&16n&15!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&15Your body is racked with pain, ushering a &16h&15o&00w&15l deep from your throat. You grab your arms, &00&01p&09a&15i&00n&15 ripping them as your fingers &00&01l&00&05e&13n&15g&00t&15h&13e&00&05n&15, and your forearms &10s&00&02h&16ri&00&02n&10k&15 down. Your body compresses, forcing your spine to curve, making you hunch over. It feels like your &00s&15k&16u&15l&00l&15 is splitting apart as it &00&01l&00&05e&13n&15g&00t&15h&13e&00&05n&00&01s&15, your &16h&15o&00r&15n&16s&15 shrinking into your head as six extra &16h&15o&00r&15n&16s&15, three on each side, extend and curve upwards, following the &00s&15k&16u&15l&00l&15. Your body over all &10s&00&02h&16r&15i&16n&00&02k&10s&15 down, nearly to your original size, except for the curved spine. Your face loses its nose, and your face extends outwards, giving you a &10l&00&02i&00&03z&16a&10r&00&02d&15 like appearance. You have achieved your &11S&00&03e&00c&15o&11n&00&03d &16T&15r&00&06a&14n&00s&14f&00&06or&14m&00a&14t&00&06i&15o&16n&15!&00", TRUE, ch, 0, 0, TO_CHAR); return;
  }

/*-------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------*/
/*------------------------------------ TR 3 ----------------------------------------- */
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"third") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15Your already in that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 20000000 < 40000000) {
  act("&14You do not have the power to attain that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  } 

 if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 20000000 >= 40000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans3);
     
  MAX_HIT = number(20000000, 20000000);
  MAX_MANA = number(20000000, 20000000);
  
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  
  act("$n&15's back arches, &00e&14n&00&06er&14g&00y&15 welling up within thier body as thier eyes slowly fade to pure white. The ground cracks beneath thier feet, &00e&11le&00&03c&16t&15ri&16c&00&03i&11t&00y&15 escaping from thier body, creating deep ruts the earth. $n&15 slowly rises up into the air, thier skin beginning to crack, bright &00&01r&09e&15d light shining through them. In a sudden explosion, thier body explodes, creating a deep crater beneath them. As the smoke clears, you look upon thier new body. Sleek skinned, an overall single color except for certain parts of thier body. Gone are the &16h&15o&00r&15n&16s&15, massive muscles and monsterous appearance. They have achieved thier &14T&00&06h&16i&15r&00d &16T&15r&00&06a&14n&00s&14f&00&06or&14m&00a&14t&00&06i&15o&16n&15!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&15Your back arches, &00e&14n&00&06er&14g&00y&15 welling up within your body as your eyes slowly fade to pure &00w&15h&00i&15t&00e&15. The ground &16c&00&03r&00&02a&10c&15k&16s&15 beneath your feet, &00e&11le&00&03c&16t&15ri&16c&00&03i&11t&00y&15 escaping from your form, creating deep ruts the earth. You slowly rise up into the air, your skin beginning to crack, bright &00&01r&09e&15d light shining through them. Your mind blanks out from the &00&01p&09o&15w&09e&00&01r&15, your body going numb as the &00&01p&09o&15w&09e&00&01r&15 rushes through your body. In a sudden explosion, your body explodes, creating a deep crater beneath you. As the &16s&15m&00o&15k&16e&15 clears, you look upon your new body. Sleek skinned, an overall single color except for certain parts of your body. Gone are your &16h&15o&00r&15n&16s&15, massive muscles and monsterous appearance. But your &00&01p&09o&15w&09e&00&01r&15 is even greater then before, as you have reached your &14T&00&06h&16i&15r&00d &16T&15r&00&06a&14n&00s&14f&00&06or&14m&00a&14t&00&06i&15o&16n&15!&00", TRUE, ch, 0, 0, TO_CHAR); 
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*------------------------------------ TR 4 ----------------------------------------- */
/*-------------------------------------------------------------------------------------*/

 if (!str_cmp(arg,"fourth") && PLR_FLAGGED(ch, PLR_trans4)) {
  act("&15Your already in that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"fourth") && GET_MAX_HIT(ch) - 40000000 < 70000000) {
  act("&15You do not have the power to attain that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  } 

 if (!str_cmp(arg,"fourth") && GET_MAX_HIT(ch) - 40000000 >= 70000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans4);
     
  MAX_HIT = number(35000000, 35000000);
  MAX_MANA = number(35000000, 35000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("$n&15's eyes going blank. A dark pool of &00&01r&09e&15d begins to swirl within thier iris's, till its a complete &16h&15a&00z&15e of &00&01r&09e&15d. $n&15's body starts to lift into the air, their tail hanging stiffly down behind them. An &16i&15n&00f&14i&00&06ni&14t&00e&15l&16y b&15l&00a&15c&16k &11a&00&03u&15r&11a&15 erupts around thier body, seeming to absorb all the &00&03l&11i&00g&11h&00&03t&15 around them. The sounds of &16b&15o&00n&15e cracking and expanding are heard as hard plates start to form around thier forehead and mouth. Plates of &16b&15o&00n&15e form around thier forearms, and a large &00&01s&09p&16i&09k&00&01e&15 extends from thier elbows back, towards the shoulders, stopping mid bicep. $n&15's body growths in hieght slightly, no more then a foot, and thier body slims out slightly. You feel thier overwhelming &00&01p&09o&15w&09e&00&01r&15 as they have achieved your &00&01F&09i&15n&00a&01l &16T&15r&00&06a&14n&00s&14f&00&06or&14m&00a&14t&00&06i&15o&16n&15!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&15You feel your &09h&00&01e&15a&00&01r&09t&15 stop, your eyes going blank. A dark pool of &00&01r&09e&15d begins to swirl within the iris's, till its a complete &16h&15a&00z&15e of &00&01r&09e&15d. Your body starts to lift into the air, your tail hanging stiffly down behind you. An &16i&15n&00f&14i&00&06ni&14t&00e&15l&16y b&15l&00a&15c&16k &11a&00&03u&15r&11a&15 erupts around your form, seeming to absorb all the &00&03l&11i&00g&11h&00&03t&15 around you. You feel your forehead start to pull up, the sounds of &16b&15o&00n&15e cracking and expanding are heard as hard plates start to form around your mouth. Plates of &16b&15o&00n&15e form around your forearm, and a large &00&01s&09p&16i&09k&00&01e&15 extends from your elbows back, towards your shoulders, stopping mid bicep. Your body growths in hieght slightly, no more then a foot, and your body slims out slightly. Your &00&01p&09o&15w&09e&00&01r&15 is overwhelming as you have achieved your &00&01F&09i&15n&00a&01l &16T&15r&00&06a&14n&00s&14f&00&06or&14m&00a&14t&00&06i&15o&16n&15!&00", TRUE, ch, 0, 0, TO_CHAR);
 return;
  }
/*-------------------------------------------------------------------------------------*/
}
/* For sparring */
ACMD(do_spar)
{
    one_argument(argument, arg);	
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
  
  if (PRF_FLAGGED(ch, PRF_CAMP)) {		
      send_to_char("You can't do this while camped!\r\n", ch);
      return;
    }  

  if (GET_LEVEL(ch) <=1) {
  send_to_char("You can't spar right away.\r\n", ch);
  return;
    }
  if (!AFF_FLAGGED(ch, AFF_SPAR) && IS_NPC(ch)) {
  SET_BIT(AFF_FLAGS(ch), AFF_SPAR);
  act("$n moves into a sparring stance.", TRUE, ch, 0, 0, TO_ROOM);
  act("You move into a sparring stance.", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
else if (!*argument)
    { 
act("&09Sparring Commands&00", TRUE, ch, 0, 0, TO_CHAR);
act("&10----------------&00", TRUE, ch, 0, 0, TO_CHAR);
act("spar &09ready&00", TRUE, ch, 0, 0, TO_CHAR);
act("spar &09stop&00", TRUE, ch, 0, 0, TO_CHAR);
act("&09sparkick&00 (person)", TRUE, ch, 0, 0, TO_CHAR);
act("&09sparpunch&00 (person)", TRUE, ch, 0, 0, TO_CHAR);
act("&09sparelbow&00 (person)", TRUE, ch, 0, 0, TO_CHAR);
act("&09sparknee&00 (person)", TRUE, ch, 0, 0, TO_CHAR);
act("&09sparkiblast&00 (person)", TRUE, ch, 0, 0, TO_CHAR);
act("spar &09fly&00", TRUE, ch, 0, 0, TO_CHAR);
act("spar &09land&00", TRUE, ch, 0, 0, TO_CHAR);
act("spar &09block&00", TRUE, ch, 0, 0, TO_CHAR);
act("spar &09relax&00", TRUE, ch, 0, 0, TO_CHAR);
return;
} 
/*-----------------------------         Start       -----------------------------------*/
 if (!str_cmp(arg,"ready") && !AFF_FLAGGED(ch, AFF_SPAR)) {
  SET_BIT(AFF_FLAGS(ch), AFF_SPAR);
 act("$n moves into a sparring stance.", TRUE, ch, 0, 0, TO_ROOM);
  act("You move into a sparring stance.", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
 else if (!str_cmp(arg,"ready") && AFF_FLAGGED(ch, AFF_SPAR)) {
  act("Your already in a sparring stance.", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
/*-------------------------------------------------------------------------------------*/
  if (!str_cmp(arg,"ready") && !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DOJO)) {
      send_to_char("Your not in a dojo!\r\n", ch);
      return;
    }
    if (!str_cmp(arg,"land") && !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DOJO)) {
      send_to_char("Your not in a dojo!\r\n", ch);
      return;
    }
      if (!str_cmp(arg,"fly") && !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DOJO)) {
      send_to_char("Your not in a dojo!\r\n", ch);
      return;
    }
      if (!str_cmp(arg,"block") && !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DOJO)) {
      send_to_char("Your not in a dojo!\r\n", ch);
      return;
    }
/*-----------------------------------------Stop----------------------------------------*/
 if (!str_cmp(arg,"stop")) {
  REMOVE_BIT(AFF_FLAGS(ch), AFF_SPAR);
 act("$n moves out of their sparring stance.", TRUE, ch, 0, 0, TO_ROOM);
  act("You move out of your sparring stance.", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
 if (!str_cmp(arg,"stop") && !AFF_FLAGGED(ch, AFF_SPAR)) {
  REMOVE_BIT(AFF_FLAGS(ch), AFF_SPAR);
  act("You aren't in a sparring stance!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
/*--------------------------------  Not sparing checks  --------------------------------*/
if (!str_cmp(arg,"block") && !AFF_FLAGGED(ch, AFF_SPAR)) {		
      send_to_char("Your not in a sparring stance!\r\n", ch);
      return;
    }
if (!str_cmp(arg,"land") && !AFF_FLAGGED(ch, AFF_SPAR)) {		
      send_to_char("Your not in a sparring stance!\r\n", ch);
      return;
    }
if (!str_cmp(arg,"fly") && !AFF_FLAGGED(ch, AFF_SPAR)) {		
      send_to_char("Your not in a sparring stance!\r\n", ch);
      return;
    }
/*--------------------------------------Flight-----------------------------------------*/
/*Fly*/
  if (!str_cmp(arg,"fly") && AFF_FLAGGED(ch, AFF_SPAR) && !AFF_FLAGGED(ch, AFF_SFLY)) {
  SET_BIT(AFF_FLAGS(ch), AFF_SFLY);
 act("$n flys up into the air.", TRUE, ch, 0, 0, TO_ROOM);
  act("You fly up into the air.", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
 else if (!str_cmp(arg,"fly") && !AFF_FLAGGED(ch, AFF_SPAR)) {
  act("You can't do that if your not sparring.", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
 else if (!str_cmp(arg,"fly") && AFF_FLAGGED(ch, AFF_SFLY)) {
  act("You are already flying!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
/*Land*/
 if (!str_cmp(arg,"land") && AFF_FLAGGED(ch, AFF_SPAR) && AFF_FLAGGED(ch, AFF_SFLY)) {
  REMOVE_BIT(AFF_FLAGS(ch), AFF_SFLY);
  act("$n flys down and lands on the ground.", TRUE, ch, 0, 0, TO_ROOM);
  act("You fly down and land on the ground.", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
 else if (!str_cmp(arg,"land") && !AFF_FLAGGED(ch, AFF_SFLY)) {
  act("You aren't flying!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
 else if (!str_cmp(arg,"land") && !AFF_FLAGGED(ch, AFF_SPAR)) {
  act("You can't do that if your not sparring.", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
/*-------------------------------------------------------------------------------------*/
/*Blocking*/
 if (!str_cmp(arg,"block") && AFF_FLAGGED(ch, AFF_SPAR) && !AFF_FLAGGED(ch, AFF_SBLOCK)) {
  SET_BIT(AFF_FLAGS(ch), AFF_SBLOCK);
  act("You prepare to block the next attack.", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
 else if (!str_cmp(arg,"block") && AFF_FLAGGED(ch, AFF_SBLOCK)) {
  act("You are already preparing to block!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
 else if (!str_cmp(arg,"block") && !AFF_FLAGGED(ch, AFF_SPAR)) {
  act("You can't do that if your not sparring.", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
 if (!str_cmp(arg,"relax") && AFF_FLAGGED(ch, AFF_SPAR) && AFF_FLAGGED(ch, AFF_SBLOCK)) {
  REMOVE_BIT(AFF_FLAGS(ch), AFF_SBLOCK);
  act("You lower your defense.", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
 else if (!str_cmp(arg,"relax") && !AFF_FLAGGED(ch, AFF_SBLOCK)) {
  act("You are not preparing to block!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
 else if (!str_cmp(arg,"relax") && !AFF_FLAGGED(ch, AFF_SPAR)) {
  act("You can't do that if your not sparring.", TRUE, ch, 0, 0, TO_CHAR);
  return;
}

/*-------------------------------------------------------------------------------------*/
}
ACMD(do_sparkick)
{
	int is_altered = FALSE; 
        int num_levels = 0; 
        int MOVE = 0, EXP = 0;
	struct char_data * vict;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

        one_argument(argument, arg);
        
   if (!AFF_FLAGGED(ch, AFF_SPAR)) {		
      send_to_char("Your not in a sparring stance!\r\n", ch);
      return;
    }
   if (!IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DOJO)) {
      send_to_char("Your not in a dojo!\r\n", ch);
      return;
    }   	
   if (GET_MANA(ch) <= 0) {
      send_to_char("Your too weak to spar anymore!\r\n", ch);
      return;
    } 
/*-------------------------------------------------------------------------------------*/
if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000) {     
GET_LEVEL(ch) += 1;                                                             
GET_EXP(ch) = 1;
num_levels++;                                                                
advance_level(ch);                                                              
is_altered = TRUE;                                                            
}
/*--------------------------Spar-Kick "Failures"---------------------------------------*/
   if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Spar-Kick who?\r\n", ch);
      return;
    }
   if (vict == ch) {
    send_to_char("Come on now, that's rather stupid!\r\n", ch);
    return;
    }
   if (vict && !AFF_FLAGGED(vict, AFF_SPAR)) {		
      send_to_char("They don't want to spar!\r\n", ch);
      return;
    }
   if (vict && AFF_FLAGGED(ch, AFF_SBLOCK)) {		
      send_to_char("Your too busy preparing to block thier attack!\r\n", ch);
      return;
    } 
   if (vict && AFF_FLAGGED(vict, AFF_SFLY) && !AFF_FLAGGED(ch, AFF_SFLY)) {		
      send_to_char("You can't hit them from down here!\r\n", ch);
      return;
    }
   if (vict && !AFF_FLAGGED(vict, AFF_SFLY) && AFF_FLAGGED(ch, AFF_SFLY)) {		
      send_to_char("You can't hit them from up here!\r\n", ch);
      return;
    }
   if (GET_MANA(vict) <= 0 && !IS_NPC(vict)){
    send_to_char("They do not have enough energy to spar anymore\r\n.", ch);
    return;
   }
  if (number(1, 100) < 30 && GET_MANA(ch) >= 50 && GET_UBS(ch) > 10 && GET_LBS(ch) < 90) {
   send_to_char("You feel your knowledge of lower body skill increase!\r\n", ch);
         ch->real_abils.ubs -= 1;
         ch->real_abils.lbs += 1;
         affect_total(ch);
        }

   if (vict && AFF_FLAGGED(vict, AFF_SBLOCK)) {		
  EXP = number(GET_HIT(vict)/30, GET_HIT(vict)/30);
  MOVE = number(GET_HIT(vict)/200, GET_HIT(vict)/200);
  GET_EXP(ch) = GET_EXP(ch) + (EXP / 2);
  GET_MANA(ch) = GET_MANA(ch) - MOVE * 2;
  
  GET_EXP(vict) = GET_EXP(vict) + (EXP / 4);
  GET_MANA(vict) = GET_MANA(vict) - MOVE;
  act("You throw a kick at $N who easily blocks it.", FALSE, ch, 0, vict, TO_CHAR);
  act("$n throws a kick at $N who easily blocks it..", TRUE, ch, 0, vict, TO_NOTVICT);
  act("As $n rears back you put your arm up and block thier kick..&00", FALSE, ch, 0, vict, TO_VICT);  
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;     
    }

  else if (vict) {		
  EXP = number(GET_HIT(ch)/30, GET_HIT(ch)/30);
  MOVE = number(GET_HIT(vict)/300, GET_HIT(vict)/300);
  GET_EXP(ch) = GET_EXP(ch) + (EXP / 4);
  GET_MANA(ch) = GET_MANA(ch) - MOVE;
  
  GET_EXP(vict) = GET_EXP(vict) + (EXP / 2);
  GET_MANA(vict) = GET_MANA(vict) - MOVE * 2;

  act("You throw a kick at $N hitting directly!", FALSE, ch, 0, vict, TO_CHAR);
  act("$n throws a kick at hitting $N!", TRUE, ch, 0, vict, TO_NOTVICT);
  act("$n rears back and&09 hits&00 you dead on with a kick!&00", FALSE, ch, 0, vict, TO_VICT);  
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;     
  }
}
ACMD(do_sparpunch)
{
	int is_altered = FALSE; 
        int num_levels = 0; 
        int MOVE = 0, EXP = 0;
	struct char_data * vict;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
        one_argument(argument, arg);
        
   if (!AFF_FLAGGED(ch, AFF_SPAR)) {		
      send_to_char("Your not in a sparring stance!\r\n", ch);
      return;
    }
   if (!IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DOJO)) {
      send_to_char("Your not in a dojo!\r\n", ch);
      return;
    } 
   if (GET_MANA(ch) <= 0) {
      send_to_char("Your too weak to spar anymore!\r\n", ch);
      return;
    } 
/*-------------------------------------------------------------------------------------*/
if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000) {     
GET_LEVEL(ch) += 1;                                                             
GET_EXP(ch) = 1;                                                                
num_levels++;                                                                   advance_level(ch);                                                              
is_altered = TRUE;                                                            
} 
/*--------------------------Spar-Kick "Failures"---------------------------------------*/
   if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Spar-Punch who?\r\n", ch);
      return;
    } 
    if (vict == ch) {
    send_to_char("Come on now, that's rather stupid!\r\n", ch);
    return;
    }
   if (vict && !AFF_FLAGGED(vict, AFF_SPAR)) {		
      send_to_char("They don't want to spar!\r\n", ch);
      return;
    }
   if (vict && AFF_FLAGGED(ch, AFF_SBLOCK)) {		
      send_to_char("Your too busy preparing to block thier attack!\r\n", ch);
      return;
    } 
   if (vict && AFF_FLAGGED(vict, AFF_SFLY) && !AFF_FLAGGED(ch, AFF_SFLY)) {		
      send_to_char("You can't hit them from down here!\r\n", ch);
      return;
    }
   if (vict && !AFF_FLAGGED(vict, AFF_SFLY) && AFF_FLAGGED(ch, AFF_SFLY)) {		
      send_to_char("You can't hit them from up here!\r\n", ch);
      return;
    }
   if (GET_MANA(vict) <= 0 && !IS_NPC(vict)){
    send_to_char("They do not have enough energy to spar anymore.", ch);
    return;
   }
   if (number(1, 100) < 30 && GET_MANA(ch) >= 50 && GET_LBS(ch) > 10 && GET_UBS(ch) < 90) {
   send_to_char("You feel your knowledge of upper body skill increase!\r\n", ch);
         ch->real_abils.ubs += 1;
         ch->real_abils.lbs -= 1;
         affect_total(ch);
        }
   if (vict && AFF_FLAGGED(vict, AFF_SBLOCK)) {		
  EXP = number(GET_HIT(vict)/30, GET_HIT(vict)/30);
  MOVE = number(GET_HIT(vict)/200, GET_HIT(vict)/200);
  GET_EXP(ch) = GET_EXP(ch) + (EXP / 2);
  GET_MANA(ch) = GET_MANA(ch) - MOVE * 2;
  
  GET_EXP(vict) = GET_EXP(vict) + (EXP / 4);
  GET_MANA(vict) = GET_MANA(vict) - MOVE;
  act("You throw a punch at $N who easily blocks it.", FALSE, ch, 0, vict, TO_CHAR);
  act("$n throws a punch at $N who easily blocks it..", TRUE, ch, 0, vict, TO_NOTVICT);
  act("As $n rears back you put your arm up and block thier punch.&00", FALSE, ch, 0, vict, TO_VICT);  
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;     
    }
  else if (vict) {		
  EXP = number(GET_HIT(ch)/30, GET_HIT(ch)/30);
  MOVE = number(GET_HIT(vict)/300, GET_HIT(vict)/300);
  GET_EXP(ch) = GET_EXP(ch) + (EXP / 4);
  GET_MANA(ch) = GET_MANA(ch) - MOVE;
  
  GET_EXP(vict) = GET_EXP(vict) + (EXP / 2);
  GET_MANA(vict) = GET_MANA(vict) - MOVE * 2;

  act("You throw a punch at $N hitting directly!", FALSE, ch, 0, vict, TO_CHAR);
  act("$n throws a punch at hitting $N!", TRUE, ch, 0, vict, TO_NOTVICT);
  act("$n rears back and hits you dead on with a punch!&00", FALSE, ch, 0, vict, TO_VICT);  
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;     
    } 
}

ACMD(do_sparknee)
{
	int is_altered = FALSE; 
        int num_levels = 0; 
        int MOVE = 0, EXP = 0;
	struct char_data * vict;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
        one_argument(argument, arg);
        
   if (!AFF_FLAGGED(ch, AFF_SPAR)) {		
      send_to_char("Your not in a sparring stance!\r\n", ch);
      return;
    }
   if (!IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DOJO)) {
      send_to_char("Your not in a dojo!\r\n", ch);
      return;
    } 
   if (GET_MANA(ch) <= 0) {
      send_to_char("Your too weak to spar anymore!\r\n", ch);
      return;
    } 
/*-------------------------------------------------------------------------------------*/
if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000) {     
GET_LEVEL(ch) += 1;                                                             
GET_EXP(ch) = 1;                                                                
num_levels++;                                                                   advance_level(ch);                                                              
is_altered = TRUE;                                                            
} 
/*--------------------------Spar-Kick "Failures"---------------------------------------*/
   if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Spar-Knee who?\r\n", ch);
      return;
    } 
   if (vict == ch) {
    send_to_char("Come on now, that's rather stupid!\r\n", ch);
    return;
    }
   if (vict && !AFF_FLAGGED(vict, AFF_SPAR)) {		
      send_to_char("They don't want to spar!\r\n", ch);
      return;
    }
   if (vict && AFF_FLAGGED(ch, AFF_SBLOCK)) {		
      send_to_char("Your too busy preparing to block thier attack!\r\n", ch);
      return;
    } 
   if (vict && AFF_FLAGGED(vict, AFF_SFLY) && !AFF_FLAGGED(ch, AFF_SFLY)) {		
      send_to_char("You can't hit them from down here!\r\n", ch);
      return;
    }
   if (vict && !AFF_FLAGGED(vict, AFF_SFLY) && AFF_FLAGGED(ch, AFF_SFLY)) {		
      send_to_char("You can't hit them from up here!\r\n", ch);
      return;
    }
   if (GET_MANA(vict) <= 0 && !IS_NPC(vict)){
    send_to_char("They do not have enough energy to spar anymore.", ch);
    return;
   }
   if (number(1, 100) < 30 && GET_MANA(ch) >= 50 && GET_UBS(ch) > 10 && GET_LBS(ch) < 90) {
   send_to_char("You feel your knowledge of lower body skill increase!\r\n", ch);
         ch->real_abils.ubs -= 1;
         ch->real_abils.lbs += 1;
         affect_total(ch);
        }
   if (vict && AFF_FLAGGED(vict, AFF_SBLOCK)) {		
      send_to_char("You can't hit them from up here!\r\n", ch);
  EXP = number(GET_HIT(vict)/25, GET_HIT(vict)/25);
  MOVE = number(GET_HIT(vict)/200, GET_HIT(vict)/200);
  GET_EXP(ch) = GET_EXP(ch) + (EXP / 2);
  GET_MANA(ch) = GET_MANA(ch) - MOVE * 2;
  
  GET_EXP(vict) = GET_EXP(vict) + (EXP / 4);
  GET_MANA(vict) = GET_MANA(vict) - MOVE;
  act("You thrust your knee at $N who easily blocks it.", FALSE, ch, 0, vict, TO_CHAR);
  act("$n slams thier knee into $N who easily blocks it..", TRUE, ch, 0, vict, TO_NOTVICT);
  act("As $n rears back you put your arm up and block thier knee.&00", FALSE, ch, 0, vict, TO_VICT);  
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;     
    }
  else if (vict) {		
  EXP = number(GET_HIT(vict)/25, GET_HIT(vict)/25);
  MOVE = number(GET_HIT(vict)/300, GET_HIT(vict)/300);
  GET_EXP(ch) = GET_EXP(ch) + (EXP / 4);
  GET_MANA(ch) = GET_MANA(ch) - MOVE;
  
  GET_EXP(vict) = GET_EXP(vict) + (EXP / 2);
  GET_MANA(vict) = GET_MANA(vict) - MOVE * 2;

  act("You slam your knee into $N!", FALSE, ch, 0, vict, TO_CHAR);
  act("$n slams thier knee into $N!", TRUE, ch, 0, vict, TO_NOTVICT);
  act("$n rears back and knees you in the stomach!&00", FALSE, ch, 0, vict, TO_VICT);  
  WAIT_STATE(ch, PULSE_VIOLENCE * 2);
      return;     
    } 
}
ACMD(do_sparelbow)
{
	int is_altered = FALSE; 
        int num_levels = 0; 
        int MOVE = 0, EXP = 0;
	struct char_data * vict;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
        one_argument(argument, arg);
        
   if (!AFF_FLAGGED(ch, AFF_SPAR)) {		
      send_to_char("Your not in a sparring stance!\r\n", ch);
      return;
    }
   if (!IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DOJO)) {
      send_to_char("Your not in a dojo!\r\n", ch);
      return;
    } 
   if (GET_MANA(ch) <= 0) {
      send_to_char("Your too weak to spar anymore!\r\n", ch);
      return;
    } 
/*-------------------------------------------------------------------------------------*/
if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000) {     
GET_LEVEL(ch) += 1;                                                             
GET_EXP(ch) = 1;                                                                
num_levels++;                                                                   advance_level(ch);                                                              
is_altered = TRUE;                                                            
} 
/*--------------------------Spar-Kick "Failures"---------------------------------------*/
   if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Spar-Elbow who?\r\n", ch);
      return;
    } 
   if (vict == ch) {
    send_to_char("Come on now, that's rather stupid!\r\n", ch);
    return;
    }
   if (vict && !AFF_FLAGGED(vict, AFF_SPAR)) {		
      send_to_char("They don't want to spar!\r\n", ch);
      return;
    }
   if (vict && AFF_FLAGGED(ch, AFF_SBLOCK)) {		
      send_to_char("Your too busy preparing to block thier attack!\r\n", ch);
      return;
    } 
   if (vict && AFF_FLAGGED(vict, AFF_SFLY) && !AFF_FLAGGED(ch, AFF_SFLY)) {		
      send_to_char("You can't hit them from down here!\r\n", ch);
      return;
    }
   if (vict && !AFF_FLAGGED(vict, AFF_SFLY) && AFF_FLAGGED(ch, AFF_SFLY)) {		
      send_to_char("You can't hit them from up here!\r\n", ch);
      return;
    }
   if (GET_MANA(vict) <= 0 && !IS_NPC(vict)){
    send_to_char("They do not have enough energy to spar anymore.", ch);
    return;
   }
   if (number(1, 100) < 30 && GET_MANA(ch) >= 50 && GET_LBS(ch) > 10 && GET_UBS(ch) < 90) {
   send_to_char("You feel your knowledge of upper body skill increase!\r\n", ch);
         ch->real_abils.ubs += 1;
         ch->real_abils.lbs -= 1;
         affect_total(ch);
        }
   if (vict && AFF_FLAGGED(vict, AFF_SBLOCK)) {		
  EXP = number(GET_HIT(vict)/25, GET_HIT(vict)/25);
  MOVE = number(GET_HIT(vict)/200, GET_HIT(vict)/200);
  GET_EXP(ch) = GET_EXP(ch) + (EXP / 2);
  GET_MANA(ch) = GET_MANA(ch) - MOVE * 2;
  
  GET_EXP(vict) = GET_EXP(vict) + (EXP / 4);
  GET_MANA(vict) = GET_MANA(vict) - MOVE;
  act("You slam your elbow into $N who easily blocks it.", FALSE, ch, 0, vict, TO_CHAR);
  act("$n slams thier elbow into $N who easily blocks it.", TRUE, ch, 0, vict, TO_NOTVICT);
  act("As $n rears back you put your arm up and block thier elbow.&00", FALSE, ch, 0, vict, TO_VICT);  
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;     
    }
  else if (vict) {		
  EXP = number(GET_HIT(vict)/25, GET_HIT(vict)/25);
  MOVE = number(GET_MANA(ch)/300, GET_MANA(ch)/300);
  GET_EXP(ch) = GET_EXP(ch) + (EXP / 4);
  GET_MANA(ch) = GET_MANA(ch) - MOVE;
  
  GET_EXP(vict) = GET_EXP(vict) + (EXP / 2);
  GET_MANA(vict) = GET_MANA(vict) - MOVE * 2;

  act("You slam your elbow into $N's ribs!", FALSE, ch, 0, vict, TO_CHAR);
  act("$n throws a kick at hitting $N!", TRUE, ch, 0, vict, TO_NOTVICT);
  act("$n slams thier elbow into your ribs!&00", FALSE, ch, 0, vict, TO_VICT);  
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;     
    } 
}

ACMD(do_sparkiblast)
{
	int is_altered = FALSE; 
        int num_levels = 0; 
        int MANA = 0, EXP = 0;
	struct char_data * vict;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
        one_argument(argument, arg);
        
   if (!AFF_FLAGGED(ch, AFF_SPAR)) {		
      send_to_char("Your not in a sparring stance!\r\n", ch);
      return;
    }
   if (!IS_SET(ROOM_FLAGS(ch->in_room), ROOM_DOJO)) {
      send_to_char("Your not in a dojo!\r\n", ch);
      return;
    }
   if (GET_MANA(ch) <= 0) {
      send_to_char("Your too weak to spar anymore!\r\n", ch);
      return;
    } 
/*-------------------------------------------------------------------------------------*/
if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*1000) {     
GET_LEVEL(ch) += 1;                                                             
GET_EXP(ch) = 1;                                                                
num_levels++;                                                                   advance_level(ch);                                                              
is_altered = TRUE;                                                            
send_to_char("You can feel your body strenghten as you continue sparring!\r\n", ch);
} 
/*--------------------------Spar-Kick "Failures"---------------------------------------*/
   if (!(vict = get_char_room_vis(ch, arg))) {
      send_to_char("Spar-KiBlast who?\r\n", ch);
      return;
    }     
    if (vict == ch) {
    send_to_char("Come on now, that's rather stupid!\r\n", ch);
    return;
    }
   if (vict && !AFF_FLAGGED(vict, AFF_SPAR)) {		
      send_to_char("They don't want to spar!\r\n", ch);
      return;
    }
   if (vict && AFF_FLAGGED(ch, AFF_SBLOCK)) {		
      send_to_char("Your too busy preparing to block thier attack!\r\n", ch);
      return;
    } 
   if (GET_MANA(vict) <= 0 && !IS_NPC(vict)){
    send_to_char("They do not have enough energy to spar anymore.", ch);
    return;
   }
   if (number(1, 100) < 30 && GET_MANA(ch) >= 50 && GET_REINC(ch) < 50) {
   send_to_char("You feel your knowledge of ki increase!\r\n", ch);
         GET_REINC(ch) += 1;
        }
   if (vict && AFF_FLAGGED(vict, AFF_SBLOCK)) {		
  EXP = number(GET_HIT(vict)/20, GET_HIT(vict)/20);
  MANA = number(GET_HIT(vict)/120 + GET_HIT(ch)/120, GET_HIT(vict)/120  + GET_HIT(ch)/120);
  GET_EXP(ch) = GET_EXP(ch) + (EXP / 2);
  GET_MANA(ch) = GET_MANA(ch) - MANA * 2;
  
  GET_EXP(vict) = GET_EXP(vict) + (EXP / 4);
  GET_MANA(vict) = GET_MANA(vict) - MANA;
  
  act("You fire a small kiblast at $N who slaps it away.", FALSE, ch, 0, vict, TO_CHAR);
  act("$n fires a small kiblast at $N who slaps it away.", TRUE, ch, 0, vict, TO_NOTVICT);
  act("As $n fires a kiblast you slap it away with the back of your hand.&00", FALSE, ch, 0, vict, TO_VICT);  
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;     
    }
  else if (vict) {		
  EXP = number(GET_HIT(vict)/20, GET_HIT(vict)/20);
  MANA = number(GET_HIT(vict)/200 + GET_HIT(ch)/200, GET_HIT(vict)/200 + GET_HIT(ch)/200);
  GET_EXP(ch) = GET_EXP(ch) + (EXP / 4);
  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(vict) = GET_EXP(vict) + (EXP / 2);
  GET_MANA(vict) = GET_MANA(vict) - MANA * 2;
  
  act("You fire a kiblast hitting $N!", FALSE, ch, 0, vict, TO_CHAR);
  act("$n fires a kiblast, hitting $N!", TRUE, ch, 0, vict, TO_NOTVICT);
  act("$n fires a kiblast hitting you directly!&00", FALSE, ch, 0, vict, TO_VICT);  
  WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;     
    } 
}
/* For jogging */
ACMD(do_jog)	
{    
    one_argument(argument, arg);
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
     
   if (!*argument) {
    send_to_char("&10Jogging commands:  &00\r\n", ch);
    send_to_char("&09-------------------&00\r\n", ch);
    send_to_char("&15Jog start          &00\r\n", ch);
    send_to_char("&15Jog stop           &00\r\n", ch);
    return;
     }

   if (!str_cmp(arg,"start")) {
    act("You start jogging.", TRUE, ch, 0, 0, TO_CHAR);
    SET_BIT(AFF_FLAGS(ch), AFF_JOG);
    return;
   }
  

   if (!str_cmp(arg,"stop")) {
    act("You stop jogging.", TRUE, ch, 0, 0, TO_CHAR);
    REMOVE_BIT(AFF_FLAGS(ch), AFF_JOG);
    return;
   }
}

/*ACMD(do_grevert)
{
send_to_char("&12Your rage calms down and you begin to slowly shrink as the hair on your body dis&00\n\r", ch);
GET_MAX_HIT(ch) -= 5000000;
GET_MAX_MANA(ch) -= 5000000;
GET_POS(ch) = 4;
return;
}
else if (!PRF_FLAGGED(ch, PRF_GOLDEN)) {
send_to_char("&12Your not a Golden Oozaru!&00\n\r", ch);
}
}*/
/* For reverting from an oozaru */
ACMD(do_orevert)	
{    
if (PRF_FLAGGED(ch, PRF_OOZARU)) {
REMOVE_BIT(PRF_FLAGS(ch), PRF_OOZARU);
send_to_char("&12Your rage calms down and you begin to slowly shrink as the hair on your body disolves and you pass out on the ground.&00\n\r", ch);
GET_MAX_HIT(ch) -= 1000000;
GET_MAX_MANA(ch) -= 1000000;
GET_POS(ch) = 4;
return;
}
else if (!PRF_FLAGGED(ch, PRF_OOZARU)) {
send_to_char("&12Your not in oozaru form!&00\n\r", ch);	
}
}
/* For increasing damage of first hit */
ACMD(do_charge)	
{    
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
    one_argument(argument, arg);
     
   if (!*argument) {
    send_to_char("&10Charge - &00\r\n", ch);
    send_to_char("&15Low&00\r\n", ch);
    send_to_char("&15Medium&00\r\n", ch);
    send_to_char("&15High&00\r\n", ch);
    send_to_char("&15Invert&00\r\n", ch);
    return;
}
   if (AFF_FLAGGED(ch, AFF_HALTED))
   {
    send_to_char("You don't have enough time!\r\n", ch);   	
    return;
    }
   if (GET_MANA(ch) <= GET_MAX_MANA(ch) / 5 && (!str_cmp(arg,"low") || !str_cmp(arg,"medium") || !str_cmp(arg,"high"))) {
    send_to_char("You don't have ki to charge that much energy!\r\n", ch);
    return;
    }
/*------------------------------------------Invertion failure----------------------------------------*/
   if (!str_cmp(arg,"invert") && !AFF_FLAGGED(ch, AFF_CHARGEH) && !AFF_FLAGGED(ch, AFF_CHARGEM) && !AFF_FLAGGED(ch, AFF_CHARGEL)) {
    act("You are not storing any energy,", TRUE, ch, 0, 0, TO_CHAR);
    return;
   }
/*------------------------------------------Already charged/too High---------------------------------*/
/*Low*/
   if (!str_cmp(arg,"low") && AFF_FLAGGED(ch, AFF_CHARGEL)) {
    act("You've already charge that much energy.", TRUE, ch, 0, 0, TO_CHAR);
    return;
   }
   if (!str_cmp(arg,"medium") && AFF_FLAGGED(ch, AFF_CHARGEM)) {
    act("You've already charge that much energy.", TRUE, ch, 0, 0, TO_CHAR);
    return;
   }
  if (!str_cmp(arg,"low") && AFF_FLAGGED(ch, AFF_CHARGEM)) {
    act("You've already charged more then that much energy.", TRUE, ch, 0, 0, TO_CHAR);
    return;
   }
   if (!str_cmp(arg,"low") && AFF_FLAGGED(ch, AFF_CHARGEH)) {
    act("You've already charged more then that much energy.", TRUE, ch, 0, 0, TO_CHAR);
    return;
   }
   if (!str_cmp(arg,"medium") && AFF_FLAGGED(ch, AFF_CHARGEH)) {
    act("You've already charged more then that much energy.", TRUE, ch, 0, 0, TO_CHAR);
    return;
   }
   if (!str_cmp(arg,"high") && AFF_FLAGGED(ch, AFF_CHARGEH)) {
    act("You've already charge that much energy.", TRUE, ch, 0, 0, TO_CHAR);
    return;
   }
/*------------------------------------------Inverting------------------------------------------------*/
   if (!str_cmp(arg,"invert") && AFF_FLAGGED(ch, AFF_CHARGEH)) {
    act("&15You absorb the &10e&00&02n&10e&00&02r&10g&00&02y &15you created.&00", TRUE, ch, 0, 0, TO_CHAR);
    act("&14$n's &10e&00&02n&10e&00&02r&10g&00&02y&15 suddenly dissapears as they absorb it!&00", TRUE, ch, 0, 0, TO_NOTVICT);
    REMOVE_BIT(AFF_FLAGS(ch), AFF_CHARGEH);
    GET_MANA(ch) += GET_MAX_MANA(ch) / 5;
    return;
   }
   else if (!str_cmp(arg,"invert") && AFF_FLAGGED(ch, AFF_CHARGEM)) {
    act("&15You absorb the &10e&00&02n&10e&00&02r&10g&00&02y &15you created.&00", TRUE, ch, 0, 0, TO_CHAR);
    act("&14$n's &10e&00&02n&10e&00&02r&10g&00&02y&15 suddenly dissapears as they absorb it!&00", TRUE, ch, 0, 0, TO_NOTVICT);
    REMOVE_BIT(AFF_FLAGS(ch), AFF_CHARGEM);
    GET_MANA(ch) += GET_MAX_MANA(ch) / 10;
    return;
   }
   else if (!str_cmp(arg,"invert") && AFF_FLAGGED(ch, AFF_CHARGEL)) {
    act("&15You absorb the &10e&00&02n&10e&00&02r&10g&00&02y &15you created.&00", TRUE, ch, 0, 0, TO_CHAR);
    act("&14$n's &10e&00&02n&10e&00&02r&10g&00&02y&15 suddenly dissapears as they absorb it!&00", TRUE, ch, 0, 0, TO_NOTVICT);
    REMOVE_BIT(AFF_FLAGS(ch), AFF_CHARGEL);
    GET_MANA(ch) += GET_MAX_MANA(ch) / 20;
    return;
   }
/*------------------------------------------Success >:) ---------------------------------------------*/
   if (!str_cmp(arg,"low")) {
    act("&15You begin &12charging &15a little bit of &10e&00&02n&10e&00&02r&10g&00&02y&15 into your hands.&00", TRUE, ch, 0, 0, TO_CHAR);
    act("&14$n &15begins flaming with power as they charge a little bit of &10e&00&02n&10e&00&02r&10g&00&02y&15 into their hands!", TRUE, ch, 0, 0, TO_NOTVICT);    
    SET_BIT(AFF_FLAGS(ch), AFF_CHARGEL);
    GET_MANA(ch) -= GET_MAX_MANA(ch) / 20;
    return;
   }
  
   if (!str_cmp(arg,"medium")) {
    act("&15You begin charging &10e&00&02n&10e&00&02r&10g&00&02y&15 into your hands.&00", TRUE, ch, 0, 0, TO_CHAR);
    act("&14$n&15 begins &09f&00&01lamin&09g&15 with power as they charge some &10e&00&02n&10e&00&02r&10g&00&02y&15 into their hands!&00", TRUE, ch, 0, 0, TO_NOTVICT);
    SET_BIT(AFF_FLAGS(ch), AFF_CHARGEM);
    REMOVE_BIT(AFF_FLAGS(ch), AFF_CHARGEL);
    GET_MANA(ch) -= GET_MAX_MANA(ch) / 10;
    return;
   }

   if (!str_cmp(arg,"high")) {
    act("&15You begin charging a lot of &10e&00&02n&10e&00&02r&10g&00&02y&15 into your hands.&00", TRUE, ch, 0, 0, TO_CHAR);
    act("&14$n&15 begins &09f&00&01lamin&09g&15 with power as they charge &09a lot&00 &10e&00&02n&10e&00&02r&10g&00&02y&15 into their hands!&00", TRUE, ch, 0, 0, TO_NOTVICT);
    SET_BIT(AFF_FLAGS(ch), AFF_CHARGEH);
    REMOVE_BIT(AFF_FLAGS(ch), AFF_CHARGEM);
    GET_MANA(ch) -= GET_MAX_MANA(ch) / 5;
    return;
   }

}
/* Stop fighting */
ACMD(do_halt)
{

        struct char_data *vict, *next_v;
    if (!FIGHTING(ch))
        {
    act("Your not fighting!", TRUE, ch, 0, 0, TO_CHAR);    
    return;
}

    act("You relax out of your fighting stance for a moment.", TRUE, ch, 0, 0, TO_CHAR);
    act("$n leans up from a fighting stance.", TRUE, ch, 0, 0, TO_NOTVICT);
    SET_BIT(AFF_FLAGS(ch), AFF_HALTED);
        for(vict=world[ch->in_room].people;vict;vict=next_v)
        {
                next_v=vict->next_in_room;
                if(!IS_NPC(vict)&&(FIGHTING(vict)))
                {
                if(FIGHTING(FIGHTING(vict))==vict)
                        stop_fighting(FIGHTING(vict));
                stop_fighting(vict);

                }
        }
}
/* Majin/Bio ability */
ACMD(do_regenerate)
{
 if (!GET_SKILL(ch, SKILL_REGENERATE)) {
  send_to_char("You can't do that!\r\n", ch);
  return;
}
 if (GET_MANA(ch) < GET_MAX_MANA(ch)/10) {
  send_to_char("You don't have the ki to regenerate.\r\n", ch);
  return;
}
 if (GET_MANA(ch) > GET_MAX_MANA(ch)) {
   send_to_char("You have too much energy right now to handle it, you lose some of it from the attempt as well.\r\n", ch);
   GET_MANA(ch) -= GET_MANA(ch) / 5;
   return;
 }
 if (IS_MAJIN(ch) && !PLR_FLAGGED(ch, PLR_FORELOCK)) {
  SET_BIT(PLR_FLAGS(ch), PLR_FORELOCK);
  act("&15You regrow your forelock...&00", TRUE, ch, 0, 0, TO_CHAR);
  act("&14$n &15regrows their forelock...&00", TRUE, ch, 0, 0, TO_NOTVICT);
  return;
  }
 if (IS_BIODROID(ch) && !PLR_FLAGGED(ch, PLR_TAIL)) {
  SET_BIT(PLR_FLAGS(ch), PLR_TAIL);
  act("&15You regrow your tail...&00", TRUE, ch, 0, 0, TO_CHAR);
  act("&14$n &15regrows their tail...&00", TRUE, ch, 0, 0, TO_NOTVICT);
  return;
  }
 if (GET_HIT(ch) >= GET_MAX_HIT(ch)) {
  send_to_char("You are fully regenerated!\r\n", ch);
  return;
}
else {
    act("You regenrate your body restoring lost parts.", TRUE, ch, 0, 0, TO_CHAR);
    act("$n completely regenerates all thier body parts.", TRUE, ch, 0, 0, TO_NOTVICT);
   GET_MANA(ch) = GET_MANA(ch) - GET_MAX_MANA(ch)/8;
   GET_HIT(ch) = GET_MAX_HIT(ch); 
  SET_BIT(PLR_FLAGS(ch), PLR_RARM);
  SET_BIT(PLR_FLAGS(ch), PLR_LARM);
  SET_BIT(PLR_FLAGS(ch), PLR_RLEG);
  SET_BIT(PLR_FLAGS(ch), PLR_LLEG);
  }
}
/* Display status of your character */
ACMD(do_status)
{
send_to_char("&15o&12-------------------------o-------------------------&15o&00\r\n", ch);
send_to_char("&12|&10Appearance:             \r\n", ch);
/* Eye Color */

if (PLR_FLAGGED(ch, PLR_trans4) && IS_saiyan(ch)) {
send_to_char("&12|&15Eye Color:  &09B&16lac&09k       \r\n", ch);
}
else if (PLR_FLAGGED(ch, PLR_trans1) && (IS_saiyan(ch) || IS_HALF_BREED(ch))) {
send_to_char("&12|&15Eye Color: &10Emerald      \r\n", ch);
}
else if (GET_EYE(ch) == EYE_BLACK) {
send_to_char("&12|&15Eye Color: Black        \r\n", ch);
}
else if (GET_EYE(ch) == EYE_ICYBLUE) {
send_to_char("&12|&15Eye Color: &14Icy Blue     \r\n", ch);
}
else if (GET_EYE(ch) == EYE_WHITE) {
send_to_char("&12|&15Eye Color: White        \r\n", ch);
}
else if (GET_EYE(ch) == EYE_BLUE) {
send_to_char("&12|&15Eye Color: &12Blue         \r\n", ch);
}
else if (GET_EYE(ch) == EYE_GREEN) {
send_to_char("&12|&15Eye Color: &11Green        \r\n", ch);
}
else if (GET_EYE(ch) == EYE_BROWN) {
send_to_char("&12|&15Eye Color: &03Brown        \r\n", ch);
}
else if (GET_EYE(ch) == EYE_PURPLE) {
send_to_char("&12|&15Eye Color: &13Purple       \r\n", ch);
}
else if (GET_EYE(ch) == EYE_RED) {
send_to_char("&12|&15Eye Color: &09Red          \r\n", ch);
}
else if (GET_EYE(ch) == EYE_YELLOW) {
send_to_char("&12|&15Eye Color: &11Yellow       \r\n", ch);
}
/* End Eye Color */
/* Aura Color */
if (GET_AURA(ch) == AURA_BLACK) {
send_to_char("&12|&15Skin Color: Black       \r\n", ch);
}
else if (GET_AURA(ch) == AURA_WHITE) {
send_to_char("&12|&15Skin Color: White       \r\n", ch);
}
else if (GET_AURA(ch) == AURA_BLUE) {
send_to_char("&12|&15Skin Color: &12Blue        \r\n", ch);
}
else if (GET_AURA(ch) == AURA_GREEN) {
send_to_char("&12|&15Skin Color: &11Green       \r\n", ch);
}
else if (GET_AURA(ch) == AURA_PURPLE) {
send_to_char("&12|&15Skin Color: &13Pink        \r\n", ch);
}
else if (GET_AURA(ch) == AURA_RED) {
send_to_char("&12|&15Skin Color: &09Red         \r\n", ch);
}
else if (GET_AURA(ch) == AURA_YELLOW) {
send_to_char("&12|&15Skin Color: &11Tan         \r\n", ch);
}
else if (GET_AURA(ch) == AURA_PINK) {
send_to_char("&12|&15Skin Color: &00&05Purple      \r\n", 
ch);
}
/* Hair Length */

if (PLR_FLAGGED(ch, PLR_trans4) && IS_saiyan(ch)) {
send_to_char("&12|&15Hair Lngth: Long Spikes \r\n", ch);
}
else if (PLR_FLAGGED(ch, PLR_trans3) && IS_saiyan(ch)) {
send_to_char("&12|&15Hair Lngth: Long Spikes \r\n", ch);
}
else if (PLR_FLAGGED(ch, PLR_trans2) && (IS_saiyan(ch) || IS_HALF_BREED(ch))) {
send_to_char("&12|&15Hair Lth: Medium Spikes \r\n", ch);
}
else if (PLR_FLAGGED(ch, PLR_trans1) && (IS_saiyan(ch) || IS_HALF_BREED(ch))) {
send_to_char("&12|&15Hair Lgth: Short Spikes \r\n", ch);
}
else if (IS_MAJIN(ch) && GET_HAIRL(ch) == HAIRL_V_SHORT) {
send_to_char("&12|&15Forelock Length: Very Short \r\n", ch);
}
else if (IS_MAJIN(ch) && GET_HAIRL(ch) == HAIRL_SSPIKE) {
send_to_char("&12|&15Forelock Lgth: Short thin \r\n", ch);
}
else if (IS_MAJIN(ch) && GET_HAIRL(ch) == HAIRL_MSPIKE) {
send_to_char("&12|&15Forelock Lth: Medium thin \r\n", ch);
}
else if (IS_MAJIN(ch) && GET_HAIRL(ch) == HAIRL_LSPIKE) {
send_to_char("&12|&15Forelock Lngth: Long thin \r\n", ch);
}
else if (IS_MAJIN(ch) && GET_HAIRL(ch) == HAIRL_BALD) {
send_to_char("&12|&15Forelock Length: Stubbey       \r\n", ch);
}
else if (IS_MAJIN(ch) && GET_HAIRL(ch) == HAIRL_SHORT) {
send_to_char("&12|&15Forelock Length: Short      \r\n", ch);
}
else if (IS_MAJIN(ch) && GET_HAIRL(ch) == HAIRL_MEDIUM) {
send_to_char("&12|&15Forelock Length: Medium     \r\n", ch);
}
else if (IS_MAJIN(ch) && GET_HAIRL(ch) == HAIRL_LONG) {
send_to_char("&12|&15Forelock Length: Long       \r\n", ch);
}
else if (IS_MAJIN(ch) && GET_HAIRL(ch) == HAIRL_V_LONG) {
send_to_char("&12|&15Forelock Length: Very Long  \r\n", ch);
 }
 else if (IS_Namek(ch) && GET_HAIRL(ch) == HAIRL_V_SHORT) {
 send_to_char("&12|&15Antenae Length: Very Short \r\n", ch);
 }
 else if (IS_Namek(ch) && GET_HAIRL(ch) == HAIRL_SSPIKE) {
 send_to_char("&12|&15Antenae Lgth: Short thin \r\n", ch);
 }
 else if (IS_Namek(ch) && GET_HAIRL(ch) == HAIRL_MSPIKE) {
 send_to_char("&12|&15Antenae Lth: Medium thin \r\n", ch);
 }
 else if (IS_Namek(ch) && GET_HAIRL(ch) == HAIRL_LSPIKE) {
 send_to_char("&12|&15Antenae Lngth: Long thin \r\n", ch);
 }
 else if (IS_Namek(ch) && GET_HAIRL(ch) == HAIRL_BALD) {
 send_to_char("&12|&15Antenae Length: Stubbey       \r\n", ch);
 }
 else if (IS_Namek(ch) && GET_HAIRL(ch) == HAIRL_SHORT) {
 send_to_char("&12|&15Antenae Length: Short      \r\n", ch);
 }
 else if (IS_Namek(ch) && GET_HAIRL(ch) == HAIRL_MEDIUM) {
 send_to_char("&12|&15Antenae Length: Medium     \r\n", ch);
 }
 else if (IS_Namek(ch) && GET_HAIRL(ch) == HAIRL_LONG) {
 send_to_char("&12|&15Antenae Length: Long       \r\n", ch);
 }
 else if (IS_Namek(ch) && GET_HAIRL(ch) == HAIRL_V_LONG) {
 send_to_char("&12|&15Antenae Length: Very Long  \r\n", ch);
 }
else if (GET_HAIRL(ch) == HAIRL_V_SHORT) {
send_to_char("&12|&15Hair Length: Very Short \r\n", ch);
}
else if (GET_HAIRL(ch) == HAIRL_SSPIKE) {
send_to_char("&12|&15Hair Lgth: Short Spikey \r\n", ch);
}
else if (GET_HAIRL(ch) == HAIRL_MSPIKE) {
send_to_char("&12|&15Hair Lth: Medium Spikey \r\n", ch);
}
else if (GET_HAIRL(ch) == HAIRL_LSPIKE) {
send_to_char("&12|&15Hair Lngth: Long Spikey \r\n", ch);
}
else if (GET_HAIRL(ch) == HAIRL_BALD) {
send_to_char("&12|&15Hair Length: None       \r\n", ch);
}
else if (GET_HAIRL(ch) == HAIRL_SHORT) {
send_to_char("&12|&15Hair Length: Short      \r\n", ch);
}
else if (GET_HAIRL(ch) == HAIRL_MEDIUM) {
send_to_char("&12|&15Hair Length: Medium     \r\n", ch);
}
else if (GET_HAIRL(ch) == HAIRL_LONG) {
send_to_char("&12|&15Hair Length: Long       \r\n", ch);
}
else if (GET_HAIRL(ch) == HAIRL_V_LONG) {
send_to_char("&12|&15Hair Length: Very Long  \r\n", ch);
}
/*End Hair Lenght*/
/*Hair Color */

if (IS_Namek(ch)) {
send_to_char("&12|&15Antenae: Yes       \r\n", ch);
}
else if (IS_MAJIN(ch)) {
send_to_char("&12|&15Forelock: Yes      \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans4) && IS_saiyan(ch)) {
send_to_char("&12|&15Hair Color: &16Black       \r\n", 
ch);
}
else if (PLR_FLAGGED(ch, PLR_trans1) && (IS_saiyan(ch) || IS_HALF_BREED(ch))) {
send_to_char("&12|&15Hair Color: &11Gold        \r\n", 
ch);
}
else if (PLR_FLAGGED(ch, PLR_trans2) && (IS_saiyan(ch) || IS_HALF_BREED(ch))) {
send_to_char("&12|&15Hair Color: &11Gold        \r\n", 
ch);
}
else if (PLR_FLAGGED(ch, PLR_trans3) && (IS_saiyan(ch) || IS_HALF_BREED(ch))) {
send_to_char("&12|&15Hair Color: &11Gold        \r\n", 
ch);
}

else if (GET_HAIRC(ch) == HAIRC_BLACK) {
send_to_char("&12|&15Hair Color: Black       \r\n", ch);
}
else if (GET_HAIRC(ch) == HAIRC_WHITE) {
send_to_char("&12|&15Hair Color: Silver      \r\n", ch);
}
else if (GET_HAIRL(ch) == 5) {
send_to_char("&12|&15Hair Color: None        \r\n", ch);
}
else if (GET_HAIRC(ch) == HAIRC_BROWN) {
send_to_char("&12|&15Hair Color: &03Brown       \r\n", ch);
}
else if (GET_HAIRC(ch) == HAIRC_BLUE) {
send_to_char("&12|&15Hair Color: &12Blue        \r\n", ch);
}
else if (GET_HAIRC(ch) == HAIRC_YELLOW) {
send_to_char("&12|&15Hair Color: &11Yellow      \r\n", ch);
}
else if (GET_HAIRC(ch) == HAIRC_GREEN) {
send_to_char("&12|&15Hair Color: &10Green       \r\n", ch);
}
else if (GET_HAIRC(ch) == HAIRC_PURPLE) {
send_to_char("&12|&15Hair Color: &13Purple      \r\n", ch);
}
else if (GET_HAIRC(ch) == HAIRC_RED) {
send_to_char("&12|&15Hair Color: &09Red         \r\n", ch);
}
else if (GET_HAIRC(ch) == HAIRC_NONE) {
send_to_char("&12|&15Hair Color: None        \r\n", ch);
}
/* End of Hair Color */

send_to_char("&12|    \r\n", ch);
send_to_char("&12|&10Body Condition:         \r\n", ch);
if (PRF2_FLAGGED(ch, PRF2_HEAD)) {
send_to_char("&12|&15Head:       &11Perfect     \r\n", ch);
}
else if (!PRF2_FLAGGED(ch, PRF2_HEAD)) {
send_to_char("&12|&15Head:       &09Severed     \r\n", ch);
}
send_to_char("&12|&15Torso:      &11Perfect     \r\n", ch);
if (PLR_FLAGGED(ch, PLR_RARM)) {
send_to_char("&12|&15Right Arm:  &11Perfect     \r\n", ch);
}
else if (!PLR_FLAGGED(ch, PLR_RARM)) {
send_to_char("&12|&15Right Arm:  &09Severed     \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_LARM)) {
send_to_char("&12|&15Left Arm:   &11Perfect     \r\n", ch);
}
else if (!PLR_FLAGGED(ch, PLR_LARM)) {
send_to_char("&12|&15Left Arm:   &09Severed     \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_RLEG)) {
send_to_char("&12|&15Right Leg:  &11Perfect    \r\n", ch);
}
else if (!PLR_FLAGGED(ch, PLR_RLEG)) {
send_to_char("&12|&15Right Leg:  &09Severed    \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_LLEG)) {
send_to_char("&12|&15Left Leg:   &11Perfect     \r\n", ch);
}
else if (!PLR_FLAGGED(ch, PLR_LLEG)) {
send_to_char("&12|&15Left Leg:   &09Severed     \r\n", ch);
}
sprintf(buf, "&12|&15Fishing Skill: &10%d&00\r\n", GET_SKILL(ch, SPELL_FISHING));
send_to_char(buf, ch);
*buf = '\0';
if (PRF2_FLAGGED(ch, PRF2_FISHING)) {
sprintf(buf, "&12|&15The line is &10%dft out&00\r\n", GET_FISHT(ch) / 100);
send_to_char(buf, ch);
}
send_to_char("&15o&12-------------------------&15o&12-------------------------&15o&00\r\n", ch);
send_to_char("&12| &10Affections:&00                                       \r\n", ch);
if (CLN_FLAGGED(ch, CLN_Death)) {
  *buf = '\0';
  sprintf(buf, "&12| &11%d&15 minutes left till revival.&00\r\n", (GET_DTIMER(ch) / 60) % 60);
  send_to_char(buf, ch);
}
if (GET_PTIMER(ch) >= 1) {
  sprintf(buf, "&12| &11%d&15 days, &11%d&15 hours, &11%d&15 minutes left for your punishment.&00\r\n", (GET_PTIMER(ch) / 86400) % 7, (GET_PTIMER(ch) / 86400) % 3600, (GET_PTIMER(ch) / 3600) % 60);
  send_to_char(buf, ch);
}
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2) && (IS_saiyan(ch) || IS_HALF_BREED(ch))) {
send_to_char("&12| &11You are in Super Saiya-Jin Form 1.                \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3) && (IS_saiyan(ch) || IS_HALF_BREED(ch))) {
send_to_char("&12| &11You are in Super Saiya-Jin Form 2.                \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4) && IS_saiyan(ch)) {
send_to_char("&12| &11You are in Super Saiya-Jin Form 3.                \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans4) && IS_saiyan(ch)) {
send_to_char("&12| &11You are in Super Saiya-Jin Form 4.                \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2) && IS_icer(ch)) {
send_to_char("&12| &14You are in Transformation Stage 1.                \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3) && IS_icer(ch)) {
send_to_char("&12| &14You are in Transformation Stage 2.                \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4) && IS_icer(ch)) {
send_to_char("&12| &14You are in Transformation Stage 3.                \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans4) && IS_icer(ch)) {
send_to_char("&12| &14You are in Transformation Stage 4.                \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2) && IS_Human(ch)) {
send_to_char("&12| &14You are in Super Human First.                     \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3) && IS_Human(ch)) {
send_to_char("&12| &14You are in Super Human Second.                    \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4) && IS_Human(ch)) {
send_to_char("&12| &14You are in Super Human Third.                     \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans4) && IS_Human(ch)) {
send_to_char("&12| &14You are in Super Human Fourth.                    \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2) && IS_Namek(ch)) {
send_to_char("&12| &14You are in Super Namek First.                     \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3) && IS_Namek(ch)) {
send_to_char("&12| &14You are in Super Namek Second.                    \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4) && IS_Namek(ch)) {
send_to_char("&12| &14You are in Super Namek Third.                     \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans4) && IS_Namek(ch)) {
send_to_char("&12| &14You are in Super Namek Fourth.                    \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2) && IS_MAJIN(ch)) {
send_to_char("&12| &14You are in Morph Stage 1.                         \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3) && IS_MAJIN(ch)) {
send_to_char("&12| &14You are in Morph Stage 2.                         \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4) && IS_MAJIN(ch)) {
send_to_char("&12| &14You are in Morph Stage 3.                         \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans4) && IS_MAJIN(ch)) {
send_to_char("&12| &14You are in Morph Stage 4.                         \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2) && IS_BIODROID(ch)) {
send_to_char("&12| &14You have hatched.                                 \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3) && IS_BIODROID(ch)) {
send_to_char("&12| &14You are Imperfect.                                \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4) && IS_BIODROID(ch)) {
send_to_char("&12| &14You are Perfect.                                  \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans4) && IS_BIODROID(ch)) {
send_to_char("&12| &14You are Super Perfect.                            \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2) && IS_KONATSU(ch)) {
send_to_char("&12| &14You are in Shadow Form 1.                         \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3) && IS_KONATSU(ch)) {
send_to_char("&12| &14You are in Shadow Form 2.                         \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2) && IS_TRUFFLE(ch)) {
send_to_char("&12| &14You have your Alpha implants.                     \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3) && IS_TRUFFLE(ch)) {
send_to_char("&12| &14You have your Beta implants.                      \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4) && IS_TRUFFLE(ch)) {
send_to_char("&12| &14You have your Infusion implants.                  \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2) && IS_demon(ch)) {
send_to_char("&12| &14You are in Innerfire First.                       \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3) && IS_demon(ch)) {
send_to_char("&12| &14You are in Innerfire Second.                      \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4) && IS_demon(ch)) {
send_to_char("&12| &14You are in Innerfire Third.                       \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans4) && IS_demon(ch)) {
send_to_char("&12| &14You are in Innerfire Fourth.                      \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2) && IS_ANDROID(ch)) {
send_to_char("&12| &14Version 1.0                                       \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3) && IS_ANDROID(ch)) {
send_to_char("&12| &14Version 2.0                                       \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4) && IS_ANDROID(ch)) {
send_to_char("&12| &14Version 3.0                                       \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans4) && !PLR_FLAGGED(ch, PLR_trans5) && IS_ANDROID(ch)) {
send_to_char("&12| &14Version 4.0                                       \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans5) && !PLR_FLAGGED(ch, PLR_trans6) && IS_ANDROID(ch)) {
send_to_char("&12| &14Version 5.0                                       \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans6) && IS_ANDROID(ch)) {
send_to_char("&12| &14Version 6.0                                       \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_BARRIER) && !IS_MUTANT(ch)) {
send_to_char("&12| &14You are protected by a shield of energy.          \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_FRENZY)) {
send_to_char("&12| &14You are in a blood thristy frenzy!          \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_BARRIER) && IS_MUTANT(ch)) {
send_to_char("&12| &14You are protected by the thick hide you grew.     \r\n", ch);
}
if (affected_by_spell(ch, SPELL_ZANZOKEN)) {
send_to_char("&12| &14You are moving at an incredible pace.     \r\n", ch);
}
if (affected_by_spell(ch, SPELL_TSURUGI)) {
send_to_char("&12| &14Your hands are covered with blades of ki.     \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_SOLAR_FLARE)) {
send_to_char("&12| &14You are blind!     \r\n", ch);
}
if (affected_by_spell(ch, SPELL_BLADEMORPH)) {
send_to_char("&12| &14Your claws are a foot long and steel sharp.     \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_CURSE)) {
send_to_char("&12| &14You are cursed!     \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_YOIKOMINMINKEN)) {
send_to_char("&12| &14You have been lulled into a deep sleep!     \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_INVISIBLE)) {
send_to_char("&12| &13You are invisible.     \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_POISON)) {
send_to_char("&12| &14You are feeling ill.     \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_DETECT_INVISIBILTY)) {
send_to_char("&12| &14Your ki infused eyes can see everything.     \r\n", ch);
}
if (GET_COND(ch, DRUNK) > 5 && GET_COND(ch, DRUNK) < 15) {
send_to_char("&12| &14You are drunk.                                    \r\n", ch);
}
if (GET_COND(ch, DRUNK) >= 15) {
send_to_char("&12| &14You are pretty fucking hammered.                  \r\n", ch);
}
if (GET_COND(ch, FULL) < 15 && GET_COND(ch, FULL) > 5) {
send_to_char("&12| &14You could use a bite to eat...                    \r\n", ch);
}
if (GET_COND(ch, FULL) < 5 && GET_COND(ch, FULL) > 0) {
send_to_char("&12| &10You are getting pretty hungry                     \r\n", ch);
}
if (GET_COND(ch, FULL) == 0) {
send_to_char("&12| &09You are starving!                                 \r\n", ch);
}
if (GET_COND(ch, THIRST) < 15 && GET_COND(ch, THIRST) > 5) {
send_to_char("&12| &14You could use a drink...                          \r\n", ch);
}
if (GET_COND(ch, THIRST) < 5 && GET_COND(ch, THIRST) > 0) {
send_to_char("&12| &10You are getting pretty thirsty                    \r\n", ch);
}
if (GET_COND(ch, THIRST) == 0) {
send_to_char("&12| &09You are dehydrated!                               \r\n", ch);
}
if (PRF_FLAGGED(ch, PRF_OOZARU)) {
send_to_char("&12| &12You are in Oozaru form.                           \r\n", ch);
}
if (PRF_FLAGGED(ch, PRF_CAMP)) {
send_to_char("&12| &10You are camping.                                  \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_INVISIBLE)) {
send_to_char("&12| &13You are invisible                                 \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_MAJIN)) {
send_to_char("&12| &13You are a Majin slave.                            \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_INFRAVISION)) {
send_to_char("&12| &13You can see in the dark.                          \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_SPAR)) {
send_to_char("&12| &10You are sparring.                                 \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_JOG)) {
send_to_char("&12| &10You are jogging.                                  \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_CHARGEL)) {
send_to_char("&12| &09You have a little bit of energy charged.          \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_CHARGEM)) {
send_to_char("&12| &09You have a fair ammount of energy charged.        \r\n", ch);
}
if (AFF_FLAGGED(ch, AFF_CHARGEH)) {
send_to_char("&12| &09You have a lot of energy charged.                 \r\n", ch);
}
if (PRF_FLAGGED(ch, PRF_MYSTIC)) {
send_to_char("&12| &14You have achieved the Mystic Magnius.            \r\n", ch);
}
if (PRF_FLAGGED(ch, PRF_MYSTIC2) && !PRF_FLAGGED(ch, PRF_MYSTIC3) && !PRF_FLAGGED(ch, PRF_MYSTIC)) {
send_to_char("&12| &14You have achieved the Mystic Power charge.        \r\n", ch);
}
if (PRF_FLAGGED(ch, PRF_MYSTIC3) && !PRF_FLAGGED(ch, PRF_MYSTIC)) {
send_to_char("&12| &14You have achieved the Mystic Power up.        \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2) && IS_MUTANT(ch)) {
send_to_char("&12| &14You have achieved the Mutation form 1.            \r\n", ch);
}
if (PLR_FLAGGED(ch, PLR_trans2) && IS_MUTANT(ch)) {
send_to_char("&12| &14You have achieved the Mutation form 2.            \r\n", ch);
}
if (PRF_FLAGGED(ch, PRF_AUTOEXIT)) {
send_to_char("&12| &09You are burned.            \r\n", ch);
}
if (GET_POS(ch) == POS_SITTING) {
send_to_char("&12| &14You are sitting.            \r\n", ch);
}
if (GET_POS(ch) == POS_STANDING) {
send_to_char("&12| &14You are standing.            \r\n", ch);
}
if (GET_POS(ch) == POS_RESTING) {
send_to_char("&12| &14You are resting.            \r\n", ch);
}
if (GET_POS(ch) == POS_SLEEPING) {
send_to_char("&12| &14You are sleeping.            \r\n", ch);
}
if (GET_POS(ch) == POS_FLOATING) {
send_to_char("&12| &14You are flying.            \r\n", ch);
}
if (GET_POS(ch) == POS_FIGHTING) {
send_to_char("&12| &14You are fighting you moron!            \r\n", ch);
}
if (PRF2_FLAGGED(ch, PRF2_FISHING)) {
send_to_char("&12| &14You are fishing lazily.            \r\n", ch);
}
send_to_char("&15o&12---------------------------------------------------&15o&00\r\n", ch);
WAIT_STATE(ch, PULSE_VIOLENCE * .5);
 }  
/* Halfbreed and Kai trans */
ACMD(do_mystic)
{

  int MAX_MANA = 0, MAX_HIT = 0, MAX_MOVE = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
    
    one_argument(argument, arg);
    WAIT_STATE(ch, PULSE_VIOLENCE * .5);
    if ((!IS_KAI(ch)) && (!IS_HALF_BREED(ch)))
    {
      act("You can not achieve mystic powers, your power lays elsewhere!", TRUE, ch, 0, 0, TO_CHAR);
      return;
    }
    else if (CLN_FLAGGED(ch, CLN_RAGE)) {
     send_to_char("Not while you are in a rage, your power is already stressed.\r\n", ch);
     return;
    }
   else if (!*argument)
    {


act("        &10o&12_&09-------------------------&12_&10o&00\r\n         &09|   &14   Mystic Forms &00       &09|&00\r\n        &10o&12_&09-------------------------&12_&10o&00\r\n         &09|&10You can do the following:&09|&00\r\n         &09|&09-------------------------&09|&00\r\n         &09|&15Stage:  Power Level Req.:&09|&00 ", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10Powercharge &148000000&00     &09|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10Powerup     &1440000000&00    &09|&00", TRUE, ch, 0, 0, TO_CHAR); 
act("         &09| &10Magnius     &1480000000&00    &09|&00", TRUE, ch, 0, 0, TO_CHAR); 
act("        &10o&12_&09-------------------------&12_&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
return;
}

/*------------------        Oozaru checks, no SSj in oozaru           -----------------*/

if (!str_cmp(arg,"powercharge" "magnius" "powerup") && PRF_FLAGGED(ch, PRF_OOZARU)) {
  act("&15You can't go &14Mystic&15 while in oozaru form!!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

/*------------------Revert--------------*/
  if (!str_cmp(arg,"revert") && PRF_FLAGGED(ch, PRF_MYSTIC)) {
     REMOVE_BIT(PRF_FLAGS(ch), PRF_MYSTIC2);
     REMOVE_BIT(PRF_FLAGS(ch), PRF_MYSTIC3);
     REMOVE_BIT(PRF_FLAGS(ch), PRF_MYSTIC);  

   MAX_HIT = number(72000000, 72000000);
   MAX_MANA = number(72000000, 72000000);
   MAX_MOVE = number (20, 20);

   GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
   GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
   GET_MAX_MOVE(ch) = GET_MAX_MOVE(ch) - MAX_MOVE;

   act("&15$n reverts from &14Mystic&15 form.&00", TRUE, ch, 0, 0, TO_ROOM);
   act("&15You revert from &14Mystic&15 form.&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

 if (!str_cmp(arg,"revert") && PRF_FLAGGED(ch, PRF_MYSTIC3)) {
    REMOVE_BIT(PRF_FLAGS(ch), PRF_MYSTIC2);
    REMOVE_BIT(PRF_FLAGS(ch), PRF_MYSTIC3);
         
  MAX_HIT = number(32000000, 32000000);
  MAX_MANA = number(32000000, 32000000);
  MAX_MOVE = number(10, 10);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_MAX_MOVE(ch) = GET_MAX_MANA(ch) - MAX_MANA;

  act("&15$n reverts from &14Mystic&15 form.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from &14Mystic&15 form.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

  if (!str_cmp(arg,"revert") && PRF_FLAGGED(ch, PRF_MYSTIC2)) {
     REMOVE_BIT(PRF_FLAGS(ch), PRF_MYSTIC2);
       
  MAX_HIT = number(12000000, 12000000);
  MAX_MANA = number(12000000, 12000000);
  MAX_MOVE = number(5, 5);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_MAX_MOVE(ch) = GET_MAX_MOVE(ch) - MAX_MOVE;

  act("&15$n reverts from &14Mystic&15 form.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&15You revert from &14Mystic&15 form.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

/*-------------------------------------------------------------------------------------*/
/*------------------------------------ Mystic -----------------------------------------*/
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"magnius") && PRF_FLAGGED(ch, PRF_MYSTIC)) {
  act("&15Your already in &14Mystic&15 form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"powerup") && PRF_FLAGGED(ch, PRF_MYSTIC3)) {
  act("&15Your already in &14Mystic&15 form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

  if (!str_cmp(arg,"powercharge") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You can't go mystic while super saiyan! You would destory the physical universe!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
if (!str_cmp(arg,"powercharge") && PRF_FLAGGED(ch, PRF_MYSTIC2)) {
  act("&15Your already have that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
  

if (!str_cmp(arg,"magnius") && GET_MAX_HIT(ch) - 32000000 < 80000000) {
  act("&15You do not have the power to attain &14Mystic &10Magnius&15 form&11!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"powerup") && GET_MAX_HIT(ch) - 12000000 < 2000000) {
  act("&15You do not have the power to attain &14Mystic &09powerup&11!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"powercharge") && GET_MAX_HIT(ch) < 8000000) {
  act("&15You do not have the power to attain &14Mystic&09 powercharge&11!&00", TRUE, ch, 0, 0, TO_CHAR);
  }

if (!str_cmp(arg,"magnius") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15What do you want to do, blow up the universe?!  You can't go &14Mystic&15 in &11SSJ&15 form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;

  }

if (!str_cmp(arg,"powercharge") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15What do you want to do, blow up the universe?! You can't go &14Mystic&15 in &11SSJ&15 form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"powerup") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15What do you want to do, blow up the universe?! You can't go &14Mystic&15 in &11SSJ&15 form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"magnius") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15What do you want to do, blow up the universe?!  You can't go &14Mystic&15 while transformed!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"powerup") && !PRF_FLAGGED(ch, PRF_MYSTIC2)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"magnius") && !PRF_FLAGGED(ch, PRF_MYSTIC3)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }


 if (!str_cmp(arg,"powercharge") && GET_MAX_HIT(ch) >=8000000) {
     SET_BIT(PRF_FLAGS(ch), PRF_MYSTIC2);

  MAX_HIT = number(12000000, 12000000);
  MAX_MANA = number(12000000, 12000000);
  MAX_MOVE = number(5, 5);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_MAX_MOVE(ch) = GET_MAX_MOVE(ch) + MAX_MOVE;

  act("&15You stance, focusing your &00m&14e&00&06n&15t&00a&06l&15 energies in to your limbs. A soft &00a&15u&14r&00&06a&15 starts to build up around you, a blocky &00w&15h&14i&00t&15e at first. You do a few &14t&15a&00i c&15h&14i&15 forms, your body moving with the smoothness of &00s&15i&13l&00&05k&15 as your mind reaches a new sense of &00e&06n&14l&15i&00g&14h&00&06t&14e&00n&15m&14e&00&06n&00t&15. Your movements become faster, more purposeful. With your final pose, your aura explodes into a dark &00&04b&12l&14u&00&06e&15. The new aura is blocky, and thick. You try to run your fingers through it as you realize you have achieved your &00M&15y&14s&00&06t&15i&00c P&15o&14w&00&06e&15r&00c&15h&00&06a&14r&15g&00e&15.&00", FALSE, ch, 0, 0, TO_CHAR);
  act("&15$n&15 moves in to the &14t&15a&00i c&15h&14i &00cr&15a&14n&00e&15 stance, their limbs starting to glow with their &00p&14o&00&06w&14e&00r&15. A soft &00a&15u&14r&00&06a&15 builds up around $n&15's body, which is blocky &00w&15h&14i&00t&15e at first. Moving from the &00cr&15a&14n&00e&15 stance, they move their body as if it were &00&04l&12i&14q&00&06u&15i&00d s&15i&13l&00&05k&15. And just as they move in to the final stance, their &00a&15u&14r&00&06a&15 explodes around them. The aura is thick and blocky, with a dark &00&04b&12l&14u&00&06e&15 hue to it. They have achieved &00M&15y&14s&00&06t&15i&00c P&15o&14w&00&06e&15r&00c&15h&00&06a&14r&15g&00e&15.&00", FALSE, ch, 0, 0, TO_ROOM); 
  return;
  } 

 if (!str_cmp(arg,"powerup") && !PRF_FLAGGED(ch, PRF_MYSTIC2)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 if (!str_cmp(arg,"powerup") && GET_MAX_HIT(ch) - 12000000 >= 40000000) {
     SET_BIT(PRF_FLAGS(ch), PRF_MYSTIC3);
  MAX_HIT = number(20000000, 20000000);
  MAX_MANA = number(20000000, 20000000);
  MAX_MOVE = number(5, 5);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_MAX_MOVE(ch) = GET_MAX_MOVE(ch) + MAX_MOVE;

  act("&14You go through the motions of your training, your body moving &00s&15w&00&06i&14f&15t&00ly&14. Your arms and legs move as if they were &00&04l&12i&14q&00&06u&00&04i&12d&14, enhanced more by the the &00&04b&12l&14u&00&06e&14 aura that surrounds your body. As your mind finds the center of your conciousness, you discover an untapped &00p&14o&00&06w&14e&00r&14 within yourself. You dip your mental hand in to it, and your real body freezes. Slowly you are lifted up into the &00a&15i&14r, your body becoming a soft haze of &00w&15h&14i&15t&00e&14. Your entire eyes turn a soft haze of &00&04b&12l&14u&00&06e&14, your iris's pure &00w&15h&00i&15t&00e&14. Your aura explodes around you, a milky blue and white. It was as thick as a heavy fog, but shined with a certain radiance. You land back on the ground, trying to catch your breath as you've reached your &00M&15y&14s&00&06t&15i&00c P&15o&14w&00&06e&14r&15u&00p&14 transformation.&00", FALSE, ch, 0, 0, TO_CHAR);
  act("&14Using a higher evolved style of &14t&15a&00i c&15h&14i, $n&14 moves their body around &00s&15w&00&06i&14f&15t&00ly&14. Their movements are like &00&04l&12i&14q&00&06u&00&04i&12d&14, their &00&04b&12l&14u&00&06e&14 aura seemingly following in the dance. They continue to move about in their training, until they freeze in midstep. $n&14's body starts to lift into the air, their body becoming a haze of &00w&15h&00i&15t&00e&14. Their eyes snap open, revealing the drastic change in them. Pure &00w&15h&00i&15t&00e&14 irises on &00&04b&12l&14u&00&06e&14 cornea's. The aura now is a mixture of white and blue, still blocky and thick like before. It shines with a soft radiance about it, as they have achieved their &00M&15y&14s&00&06t&15i&00c P&15o&14w&00&06e&14r&15u&00p&14 transformation.&00", FALSE, ch, 0, 0, TO_ROOM); 
return;
  }

 if (!str_cmp(arg,"magnius") && GET_MAX_HIT(ch) - 32000000 >= 80000000) {
     SET_BIT(PRF_FLAGS(ch), PRF_MYSTIC);

  MAX_HIT = number(40000000, 40000000);
  MAX_MANA = number(40000000, 40000000);
  MAX_MOVE = number(10, 10);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_MAX_MOVE(ch) = GET_MAX_MOVE(ch) + MAX_MOVE;

  act("&00&06The skies are calm as you meditate. Your &00&03m&11i&14n&00&06d has left your body, slipping in to another realm of thought and possibilites. You see colors never even before conceivable in the real world, such &11b&00&03e&00&06a&14u&00&06t&00&03y&00&06 not seen by the eyes of mortal men. As you move deeper into this hidden world, you come across a &15s&16h&00&06a&15d&16o&00&06w, moving closer to you. It is you, but, not you. Your movements are the same, and as you reach out to touch it, you realize this is...&00&06Your body in the real world spasms, holding tightly to your chest as your head is thrown back, bright &00&03l&11i&15g&00ht&00&06 shooting from your mouth and eyes, illuminating the dark night sky. As you return to your body, you stand up, noticing your body has vastly changed. Dark &00&04b&12l&14u&00&06e &16t&15r&00ib&15a&16l&00&06 tattoos cover your form, and your eyes were a &00&04c&12o&15b&16a&00&04l&12t&00&06 blue. Your aura had taken on the appearance of fire, a spectacular mix of blue and white. You smile, enjoying the warmth of your &00M&15y&14s&00&06t&15i&00c &15M&16a&00&06g&14n&00&06i&00&04u&12s&00&06 Transformation.&00", FALSE, ch, 0, 0, TO_CHAR);
  act("&00&06The night sky is peaceful, serene, as you notice $n quietly meditating. Their aura is soft, nearly diminished, but still there. The face contorts into what seems to be a look of suprise, just before it snaps out, all the limbs stretched out, as bright &00w&15h&00i&15t&00e &00&03l&11i&15g&00ht&00&06 shoots out from $n&00&06's eyes and mouth. Bright &00&04b&12l&14u&00&06e and &00w&15h&00i&14t&00e&06 flames move up from out of their skin, moving across the body with a purpose. In the wake of the aura flame, dark &00&04b&12l&14u&00&06e &16t&15r&00ib&15a&16l&00&06 tattoos are seemingly burned into their skin, but no sign of fire damage. As they regain conciousness, they stand up, the first noticeable thing is $n&00&06's eyes. A pure &00&04c&12o&15b&16a&00&04l&12t&00&06 blue, no sign of iris's at all. Their aura slowly slips from their form, looking like a beautiful blue and white flame. They have achieved their &00M&15y&14s&00&06t&15i&00c &15M&16a&00&06g&14n&00&06i&00&04u&12s&00&06 Transformation!&00", FALSE, ch, 0, 0, TO_ROOM);

return;
  }


}

/*-------------------------------------------------------------------------------------*/
/* Bio trans */
ACMD(do_perfection)
 {

   int MAX_MANA = 0, MAX_HIT = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

     one_argument(argument, arg);
     WAIT_STATE(ch, PULSE_VIOLENCE * .5);
     if (!IS_BIODROID(ch))
     {
       act("You're not a bio-android!", TRUE, ch, 0, 0, TO_CHAR);
       return;
     }

 else if (!*argument)
     {
 act("&10o&12*_&02---------------------------------------------&12_*&10o&00", TRUE, ch, 0, 0, TO_CHAR);
 act("  &09|                &11PERFECTION!!&00                 &09|&00", TRUE, ch, 0, 0, TO_CHAR);
 act("&10o&12*_&02---------------------------------------------&12_*&10o&00", TRUE, ch, 0, 0, TO_CHAR);
 act("  &09|&10           You can do the following.         &09|&00", TRUE, ch, 0, 0, TO_CHAR);
 act("  &09|&02---------------------------------------------&09|&00", TRUE, ch, 0, 0, TO_CHAR);
 act("  &09| &15Stage:            Power Level Req:          &09|&00 ", TRUE, ch, 0, 0, TO_CHAR);
 act("  &09| &10HATCH             &143000000&00                   &09|&00", TRUE, ch, 0, 0, TO_CHAR);
 act("  &09| &10IMPERFECT         &1420000000&00                  &09|&00", TRUE, ch, 0, 0, TO_CHAR);
 act("  &09| &10PERFECT           &1450000000&00                  &09|&00", TRUE, ch, 0, 0, TO_CHAR);
 act("  &09| &10SUPER             &1475000000&00                  &09|&00", TRUE, ch, 0, 0, TO_CHAR);
 act("  &09| &12Type perfect reshell to revert from hatch&00   &09|&00", TRUE, ch, 0, 0, TO_CHAR);
 act("&10o&12*_&02---------------------------------------------&12_*&10o&00", TRUE, ch, 0, 0, TO_CHAR);
 act("    &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
 return;
 }
 /*------------------  Checks to see if already in Morphing stage.   -----------------*/

 if (!str_cmp(arg,"imperfect") && !PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"perfect") && !PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"super") && !PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
 /*-------------------------------------------------------------------------------------*/
 /*----------------------------------  Reverting  --------------------------------------*/

 if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans4)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans4);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

   MAX_HIT = number(75000000, 75000000);
   MAX_MANA = number(75000000, 75000000);

   GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
   GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
   act("&11$n&10's body &12shrinks &10$s muscles become &11smaller &10again as $s form turns more bug-like.&00", TRUE, ch, 0, 0, TO_ROOM);
   act("&10Your body &14shrinks &10your muscles become &11smaller &10again as you turn more bug-like.&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

 if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans3)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

   MAX_HIT = number(40000000, 40000000);
   MAX_MANA = number(40000000, 40000000);

   GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
   GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
   act("&11$n&10's body &12shrinks &10$s muscles become &11smaller &10again as $s form turns more bug-like.&00", TRUE, ch, 0, 0, TO_ROOM);
   act("&10Your body &14shrinks &10your muscles become &11smaller &10again as you turn more bug-like.&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

 else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans2)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

   MAX_HIT = number(16000000, 16000000);
   MAX_MANA = number(16000000, 16000000);

   GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
   GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
   act("&11$n&10's body &12shrinks &10$s muscles become &11smaller &10again as $s form turns morebug-like.&00", TRUE, ch, 0, 0, TO_ROOM);
   act("&10Your body &14shrinks &10your muscles become &11smaller &10again as you turn more bug-like.&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

 else if (!str_cmp(arg,"reshell") && PLR_FLAGGED(ch, PLR_trans1)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

   MAX_HIT = number(4000000, 4000000);
   MAX_MANA = number(4000000, 4000000);

   GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
   GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
   act("&11$n&10 encloses $mself in a shell, &09reshelling&10 $mself!", TRUE, ch, 0, 0, TO_ROOM);
   act("&10You enclose yourself in a shell, &09reshelling&10 yourself!", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

 /*-------------------------------------------------------------------------------------*/
 /*--------------------------------- hatch!!!!!!! ------------------------------------- */
 /*-------------------------------------------------------------------------------------*/

 if (!str_cmp(arg,"hatch") && PLR_FLAGGED(ch, PLR_trans1)) {
   act("&10Your already past that metamorphosis!&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

 if (!str_cmp(arg,"hatch") && GET_MAX_HIT(ch) <= 3000000) {
   act("&10You do not yet have the strength to emerge!&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

  if (!str_cmp(arg,"hatch") && GET_MAX_HIT(ch) >= 3000000) {
      SET_BIT(PLR_FLAGS(ch), PLR_trans1);

   MAX_HIT = number(4000000, 4000000);
   MAX_MANA = number(4000000, 4000000);

   GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
   GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

   act("&16Your body tingles as a new sense of &15p&11o&00&03w&11e&15r&16 rushes through your body. With a huge &00b&15u&11r&00&03s&16t of &00e&14n&00&06er&14g&00y&16, you explode out of your cocoon prison. You body grows larger in size and stature. Your develop long &15i&10n&00&02s&00&03e&00&02c&10t&16 like wings on your back.  Your tail grows longer and thicker and the tip resembles a needle. &10P&00&02e&16r&00f&15e&10c&00&02t&16i&00o&15n&16 and gaining ultimate power become the thought that dominates your mind. You now have the urge to absorb more trying to achieve &10P&00&02e&16r&00f&15e&10c&00&02t&16i&00o&15n&16!&00", FALSE, ch, 0, 0, TO_CHAR);
   act("&16You see &14$n's&16 large cocoon, with  hues of &00&03brown&16, &16b&15l&00a&15c&16k, and &10g&00&02r&10e&15e&00n&16 speckled acrossed its surface. The surface of the cocoon begins to crack and shift ,as a bright &14g&00&06l&00&03o&11w&16 shines through. It seems to be growing larger by the second causing the ground around you to shake . As the &00&03l&11i&00g&11h&00&03t&16 engulfing the cocoon dims, you notice two newly formed large &16b&15l&00a&15c&16k wings emerge, followed  by its long, segmented tail with a sharp looking tip at the end. The creature before you stands slowly, lines of &10g&00&02r&10e&15e&00n o&15o&10z&00&02e&16 still clinging to its freshly formed body. This creature before you has just &10H&00&02a&00&03t&16c&00&03h&00&02e&10d&16 from its long slumber.&00", FALSE, ch, 0, 0, TO_ROOM);  

 return;
   }

 /*-------------------------------------------------------------------------------------*/
 /*---------------------------------- imperfect  -------------------------------------- */
 /*-------------------------------------------------------------------------------------*/

 if (!str_cmp(arg,"imperfect") && PLR_FLAGGED(ch, PLR_trans2)) {
   act("&15Your already in that stage!", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

 if (!str_cmp(arg,"imperfect") && GET_MAX_HIT(ch) - 4000000 <= 20000000) {
   act("&15You have not yet gained enough power to change!&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

  if (!str_cmp(arg,"imperfect") && GET_MAX_HIT(ch) - 4000000 >= 20000000) {
      SET_BIT(PLR_FLAGS(ch), PLR_trans2);

   MAX_HIT = number(16000000, 16000000);
   MAX_MANA = number(16000000, 16000000);

   GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
   GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

   act("&16Your body begins to shake immensely as a huge &00b&15u&11r&00&03s&16t of &15p&11o&00&03w&11e&15r&16 rushes into your body. The surge of power is so &00i&15m&09m&00&01e&09n&15s&00e&16 your body feels that it will tear itself apart. Your hard body begins to split open revealing a new soft body underneath. You release a loud &16s&00&01c&09r&15e&00a&15m&16 as your body is engulfed in a &11b&00&03r&16i&15g&00h&11t&16 aura. A bright flash is heard followed by a huge &00s&15h&14o&15c&00k&15w&14a&15v&00e&16 seconds later as your burst from your old body. As the &16d&15u&00s&15t&16 settles your new human sized body glistens in the &00s&15u&11n&00&03li&11g&15h&00t&16. You develop an eagle like beak and your face resembles that of a reptile. Your wings have grown in length and thickness and cascade down the length of your back in a V shape. Protruding from where the wings meet, is your segmented tail which has doubled in thickness and the tip resembles a serrated knife. Thoughts of perfection are now the only thing that you think of. Your urge and determination to reach &10p&00&02e&16r&00f&15e&10c&00&02t&16i&00o&15n&16 seems stronger than ever. You are now &16I&15m&10p&00&02e&16r&15f&00e&10c&00&02t&16!&00", FALSE, ch, 0, 0, TO_CHAR);
   act("&16You see &14$n&16 infront of you start to shake in &00&01p&09a&15i&00n&16. It seems to be literally splitting out of its body with a new larger body underneath. You hear a deafening &16s&00&01c&09r&15e&00a&15m&16 as you are blinded by a &11b&00&03r&16i&15g&00h&11t&16 light and an &00i&15m&09m&00&01e&09n&15s&00e&16 &00s&15h&14o&15c&00k&15w&14a&15v&00e&16 of &00e&14n&00&06er&14g&00y&16 knocks you off your feet. As the light dims you see a much larger being standing infront of you.  Its has become taller and more muscular with thick &16b&15l&00a&15c&16k wings cascading down its back.  The tail of the being is now twice its original length and thickness with a sharp tip on the end. The being infront of you seems lost in a world of its own. It has become &16I&15m&10p&00&02e&16r&15f&00e&10c&00&02t&16!&00", FALSE, ch, 0, 0, TO_ROOM);

 return;
   }

 /*-------------------------------------------------------------------------------------*/
 /*--------------------------------- Perfection  -------------------------------------- */
 /*-------------------------------------------------------------------------------------*/

 if (!str_cmp(arg,"perfect") && PLR_FLAGGED(ch, PLR_trans3)) {
   act("&15Your already perfect!", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

 if (!str_cmp(arg,"perfect") && GET_MAX_HIT(ch) - 20000000 <= 50000000) {
   act("&15You do not have the power to attain perfection!&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

  if (!str_cmp(arg,"perfect") && GET_MAX_HIT(ch) - 20000000 >= 50000000) {
      SET_BIT(PLR_FLAGS(ch), PLR_trans3);

   MAX_HIT = number(20000000, 20000000);
   MAX_MANA = number(20000000, 20000000);

   GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
   GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

   act("&16As you absorb your latest victim on your quest for &10p&00&02e&16r&00f&15e&10c&00&02t&16i&00o&15n&16, your body begins to feel different.  An &00i&15m&09m&00&01e&09n&15s&00e &01p&09a&15i&00n&16 begins to grow as you grab your stomach and drop to your knees. You begin seeing your entire life and list of victims flash through your mind. As each victim passes the urge toward &10P&00&02e&16r&00f&15e&10c&00&02t&16i&00o&15n&16 grows in your mind driving you &16i&00&01n&09s&15a&00n&16e. As the image of the latest victim flashes through your mind, your body begins to &14g&00&06l&00&03o&11w&16 a bright &10n&00&02e&15o&00n &10g&00&02r&10e&15e&00n&16. You feel weightless as you float into the air which has turned to a dark &16b&15l&00a&15c&16k,and your body begins to morph and change shape. As your body stops changing, a insane amount of &15p&11o&00&03w&11e&15r&16 flows through your &10v&16e&00&02i&10n&00&03s&16 causing your muscles to swell and harden. As the light dims down your new body gleams in the sunlight. Your beak has now becomd a humanlike mouth, and your body is now well over six feet tall, and covered in a hard armorlike skin. A &16b&15l&00a&15c&16k diamond is on your chest, and is so dark no reflection is cast. Your wings cascade down your back coming to a razorlike point at the tip and forming a V near your neck. Where that V forms, Your long thick tail begins. It is as long as your body and as thick as your thighs with a tip resembling a hypodermic needle. Though you have achieved &10P&00&02e&16r&00f&15e&10c&00&02t&16i&00o&15n&16 the urge to become more perfect flows through your mind, it controls every thought and action you do now.&00", FALSE, ch, 0, 0, TO_CHAR);
   act("&14$n &16drops to the ground seeming to be in a deep &00&01p&09a&15i&00n&16. You watch him seem to block out the outside world as if remembering something. Without warning you watch his body float into the air as it &14g&00&06l&15o&00&03w&11s&16 a blinding &10g&00&02r&10e&15e&00n&16 hue, making it the only thing visible in the &16b&15l&00a&15c&16k sky. After several minutes the sky begins to lighten and the being floats down back onto the ground. As you readjust to the light, your eyes focus in on the being infront of you, and you notice that it looks nothing like the creature you saw mere minutes ago. You can sense the &16i&00&01n&09s&15a&00n&15e&16 amount of power this being now possesses. Your eyes focus in on the huge &16b&15l&00a&15c&16k diamond that has formed on his chest, but quickly notices that no light seems to reflect off of its surface. Your eyes are next drawn to its tail which looks like it could crush rocks, and its tip could pierce through the thickest armor. You have witnessed the creature infront of you reach &10P&00&02e&16r&00f&15e&10c&00&02t&16i&00o&15n&16!&00", FALSE, ch, 0, 0, TO_ROOM);

 return;
   }


 /*-------------------------------------------------------------------------------------*/
 /*--------------------------------- SuperPerfec -------------------------------------- */
 /*-------------------------------------------------------------------------------------*/

 if (!str_cmp(arg,"super") && PLR_FLAGGED(ch, PLR_trans4)) {
   act("&15You're already in super perfect!&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

 if (!str_cmp(arg,"super") && GET_MAX_HIT(ch) - 40000000 <= 75000000) {
   act("&15You do not have the power to attain super-perfection!&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
   }

  if (!str_cmp(arg,"super") && GET_MAX_HIT(ch) - 40000000 >= 75000000) {
      SET_BIT(PLR_FLAGS(ch), PLR_trans4);

  MAX_HIT = number(35000000, 35000000);
  MAX_MANA = number(35000000, 35000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&16Time itself seems to stop as the last victim and you become one. The &14s&00&06k&15y&16 turns a dark b&15l&00a&15c&16k, and a &15p&11o&00&03w&11e&15r&16 never seen before surges into your body. As the &15p&11o&00&03w&11e&15r&16 flows your body expands and a pressure from within begins to build. As your body expands the pressure and &15p&11o&00&03w&11e&15r&16 inside you grows it becomes too much for your body to handle and it explodes violently sending &09b&00&01l&15o&09o&00&01d&16 and &10g&00&02u&00&05t&00&01s&16 spraying everywhere. Your body begins to regenerate itself by pulling together the remaining parts and forming new appendages seemingly out of nowhere, but it doesnt reform to look like anything what it did before the explosion. Your skin hardens to a &00d&15i&14a&00m&15o&14n&00d&16 like consistancy and is covered in armorlike scales. Your muscles have swelled to several times of their former size and your veins are clearly visible. A huge tail sways behind you seeming to have a life of its own, with only one thing on its mind, absorbing.  Two large spikes protrude from the top of your head , resembling a set of &16h&15o&00r&15n&16s. You are the epitime of &10P&00&02e&16r&00f&15e&10c&00&02t&16i&00o&15n&16 reaching the highest state, but the urge to become more perfect still controls your mind, You have achieved a state thought only to be a myth, You are now &11S&00&03u&15p&11e&00&03r &10P&00&02e&16r&00f&15e&10c&00&02t&16!&00", FALSE, ch, 0, 0, TO_CHAR);
   act("&16You are witnessing history in the making. You can only stare as &14$n&16 begins to swell with a &15p&11o&00&03w&11e&15r&16 never seen before. You watch in a sense of awe as their body swells outward with &15e&14n&00&06er&14g&15y&16. You raise your arms to your chest as the power becomes to much for the being infront of you and he explodes sending out a huge shockwave of &15e&14n&00&06er&14g&15y&16, &09b&00&01l&15o&09o&00&01d&16 and &10g&00&02u&00&05t&00&01s&16 spraying everywhere covering you from head to toe in a think &10g&00&02r&10e&15e&00n&16 blood and internal organs. As you clean youself off you notice that seemingly out of nowhere a being is forming infront of you. You watch in shock as pieces if flesh meld together and then form new limbs covered in a thick green slime. As the body reforms you notice two huge spikes on his forehead that look give the look of &16h&15o&00r&15n&16s. The being has the look of a great body builder because of the huge thick muscles covering his body, and immense tail swaying menacingly behind him. You cant even comprehend the sheer amount of being that must have been absorbed for the creature infront of you to reach this fabled &11S&00&03u&15p&11e&00&03r &10P&00&02e&16r&00f&15e&10c&00&02t&16 state!&00", FALSE, ch, 0, 0, TO_ROOM);

 return;
  }

   }

ACMD(do_heal2)
{
  if (!IS_NPC(ch)) {
   send_to_char("You can't use that.", ch);
   return;
  }
  if (IS_NPC(ch) && AFF_FLAGGED(ch, AFF_MAJIN)) {
   return;
  }
  if (IS_NPC(ch)) {
   act("&14$n &15places their hands on their body and &13heals &15some of their &09wounds&00\r\n", TRUE, ch, 0, 0, TO_ROOM);
   GET_HIT(ch) += GET_LEVEL(ch);
  }
}
/* heal from heal rooms */
ACMD(do_heal)
{
  int MANA = 0, HIT = 0, MOVE = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

    one_argument(argument, arg);

  if (!*argument && ROOM_FLAGGED(ch->in_room, ROOM_HTANK))
    {
   act("&15What do you want to heal?  HP, or Ki?&00", TRUE, ch, 0, 0, TO_CHAR);
   act("&15Syntax: Heal <Pl/Ki>&00", TRUE, ch, 0, 0, TO_CHAR);
     }

  else if (!*argument && !ROOM_FLAGGED(ch->in_room, ROOM_HTANK)) {
      send_to_char("You're not in a healing tank!\r\n", ch);
    return;
    }
  else if ((!str_cmp(arg,"ki") || !str_cmp(arg,"pl")) && GET_GOLD(ch) <= 24999) {
   send_to_char("&15You do not have enough money.&00", ch);
   return;
   }
/*------------------------------------------*/
/*-------------------Mana-------------------*/
/*------------------------------------------*/

    if (!str_cmp(arg,"ki") && !ROOM_FLAGGED(ch->in_room, ROOM_HTANK)) {
      send_to_char("You're not in a healing tank!\r\n", ch);
    return;
    }
    else if (!str_cmp(arg,"ki") && GET_MANA(ch) >= GET_MAX_MANA(ch))
    {
    act("&15Your &14Ki&15 is already at it's maximum!&00", TRUE, ch, 0, 0, TO_CHAR);
    return;
    }
    else if (!str_cmp(arg,"ki") && GET_MANA(ch) < GET_MAX_MANA(ch))
    {
  act("&15You restore your &14Ki&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  act("&12$n&15 restores $s &14Ki&15.&00", TRUE, ch, 0, 0, TO_ROOM);


  MANA = number(1, 200);
  GET_MANA(ch) = GET_MAX_MANA(ch);
  GET_GOLD(ch) = GET_GOLD(ch) - 25000;
  return;
    }
/*------------------------------------------*/
/*-------------------Move-------------------*/
/*------------------------------------------*/

  if (!str_cmp(arg,"en") && !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_HTANK)) {
      send_to_char("You're not in a healing tank!\r\n", ch);
    return;
    }
    else if (!str_cmp(arg,"en") && GET_MOVE(ch) >= GET_MAX_MOVE(ch))
    {
        act("&15Your &10Energy&15 is already at it's maximum!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
    }
    else if (!str_cmp(arg,"en") && GET_MOVE(ch) < GET_MAX_MOVE(ch))
    {
  act("&15You restore your &10Energy&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  act("&12$n&15 restores $s &10Energy&15.&00", TRUE, ch, 0, 0, TO_ROOM);


  MOVE = number(1, 200);
  GET_MOVE(ch) = GET_MOVE(ch);
  return;
    }
/*------------------------------------------*/
/*-------------------Hitp-------------------*/
/*------------------------------------------*/

  if (!str_cmp(arg,"pl") && !ROOM_FLAGGED(ch->in_room, ROOM_HTANK)) {
      send_to_char("You're not in a healing tank!\r\n", ch);
    return;
    }
    else if (!str_cmp(arg,"pl") && GET_HIT(ch) >= GET_MAX_HIT(ch))
    {
     act("&15Your &09PL&15 is at it's maximum!&00", TRUE, ch, 0, 0, TO_CHAR);
     return;
    }
    else if (!str_cmp(arg,"pl") && GET_HIT(ch) < GET_MAX_HIT(ch))
    {
  act("&15You restore your &09Pl&15.&00", TRUE, ch, 0, 0, TO_CHAR);
  act("&12$n&15 restores $s &09Pl&15.&00", TRUE, ch, 0, 0, TO_ROOM);


  HIT = number(1, 200);
  GET_HIT(ch) = GET_MAX_HIT(ch);
  GET_GOLD(ch) = GET_GOLD(ch) - 25000;
  return;
    }
 }
/* Pk arena function */
ACMD(do_pk)
{
    one_argument(argument, arg);
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

    if (!*argument) {
        act("&16Syntax&11: &09PK &14<&00&06yes&14/&00&06no&14>&00", TRUE, ch, 0, 0, TO_CHAR);
    return;
   }
    if (PRF_FLAGGED(ch, PRF_NOGRATZ)) {
       send_to_char("&15You are in hell, enjoy your stay scum!&00\r\n", ch);
       return;
      }
  if (PRF2_FLAGGED(ch, PRF2_FISHING)) {
   send_to_char("You are too busy fishing.\r\n", ch);
   return;
  }
    if (CLN_FLAGGED(ch, CLN_Death)) {
      send_to_char("You are dead...sorry.\r\n", ch);
      return;
     }
    if (PRF2_FLAGGED(ch, PRF2_TRANS1) || PRF2_FLAGGED(ch, PRF2_TRANS2) || PRF2_FLAGGED(ch, PRF2_TRANS3)) {
     send_to_char("&15You are in a ship!&00\r\n", ch);
     return;
    }
    if (!str_cmp(arg,"yes") && GET_MAX_HIT(ch) <= 4999999) {
       send_to_char("&15You are still protected , wait till you get past 5mil pl!&00", ch);
       return;
      }
    if (!str_cmp(arg,"yes") && PLR_FLAGGED(ch, PLR_KILLER)) {
       send_to_char("&15You are already in the arena!&00", ch);
       return;
      }
    if (!str_cmp(arg,"yes") && PLR_FLAGGED(ch, PLR_icer)) {
       send_to_char("Not while you have a bounty on your head!", ch);
       return;
     }
    if (!str_cmp(arg,"no") && !PLR_FLAGGED(ch, PLR_KILLER)) {
       send_to_char("&15You are not in the pk arena!&00", ch);
       return;
      }
    if (!str_cmp(arg,"no") && PLR_FLAGGED(ch, PLR_KILLER)) {
       sprintf(buf, "&14%s &11c&00&03h&11i&00&03c&11k&00&03e&11n&00&03s&15 out of the &00&07P&14l&00&06ay&16er &09K&00&01il&09l &11A&00&03ren&11a&15!&00\r\n", GET_NAME(ch));
       send_to_all(buf);
       REMOVE_BIT(PLR_FLAGS(ch), PLR_KILLER);
       GET_GOLD(ch) += 5000;  
       char_from_room(ch);
       if (IS_icer(ch) || IS_MUTANT(ch)) {
       char_to_room(ch, r_icer_start_room );
       }
       else if (IS_saiyan(ch)) {
       char_to_room(ch, r_saiyan_start_room );
       }
       else if (IS_ANGEL(ch)) {
       char_to_room(ch, r_angel_start_room );
       }
       else if (IS_demon(ch)) {
       char_to_room(ch, r_demon_start_room );
       }
       else if (IS_MAJIN(ch)) {
       char_to_room(ch, r_majin_start_room );
       }
       else if (IS_KONATSU(ch)) {
       char_to_room(ch, r_konack_start_room );
       }
       else if (IS_Namek(ch)) {
       char_to_room(ch, r_namek_start_room );
       }
       else if (IS_TRUFFLE(ch)) {
       char_to_room(ch, r_truffle_start_room );
       }
       else {
       char_to_room(ch, r_mortal_start_room );
       }
       look_at_room(ch, 0);
       return;
      }
    if (!str_cmp(arg,"yes") && GET_GOLD(ch) <= 4999) {
       send_to_char("&15You need 5k zenni for the betting process!&00", ch);
       return;
      }
    if (!str_cmp(arg,"yes") && !PLR_FLAGGED(ch, PLR_KILLER)) {
        act("&15The world warps around you as the gods grant your wish for &09mortal &10combat&16!&00\r\n", TRUE, ch, 0, 0, TO_CHAR);
        sprintf(buf, "&14%s &15has entered the &00&07P&14l&00&06ay&16er &09K&00&01il&09l &11A&00&03ren&11a&16!&00\r\n", GET_NAME(ch));
        send_to_all(buf);
        GET_GOLD(ch) -= 5000;
        SET_BIT(PLR_FLAGS(ch), PLR_KILLER);
        switch (number(1, 4)) {
         case 1:
           char_from_room(ch);
           char_to_room(ch, r_pk1_room);
           look_at_room(ch, 0);
           break;
         case 2:
           char_from_room(ch);
           char_to_room(ch, r_pk2_room);
           look_at_room(ch, 0);
           break;
         case 3:
           char_from_room(ch);
           char_to_room(ch, r_pk3_room);
           look_at_room(ch, 0);
           break;   
         case 4:
           char_from_room(ch);
           char_to_room(ch, r_pk4_room);
           look_at_room(ch, 0);
           break;
         default:
           char_from_room(ch);
           char_to_room(ch, r_pk1_room);
           look_at_room(ch, 0);
           break;
        }
        return;
       }
}

ACMD(do_majinmorph)
{

  int MAX_MANA = 0, MAX_HIT = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

    one_argument(argument, arg);
    WAIT_STATE(ch, PULSE_VIOLENCE * .5);
    if ((!IS_MAJIN(ch)))
    {
      act("You're not a Majin!", TRUE, ch, 0, 0, TO_CHAR);
      return;
    }
   else if (!*argument)
    {
act("        &10o&12_&09-------------------------&12_&10o&00\r\n         &09|   &11    Majin Forms&00       &09|&00\r\n        &10o&12_&09-------------------------&12_&10o&00\r\n         &09|&10You can do the following:&09|&00\r\n         &09|&09-------------------------&09|&00\r\n         &09|&15Stage:  Power Level Req.:&09|&00 ", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10Good        &147000000&00     &09|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10Evil        &1430000000&00    &09|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10Super       &1465000000&00    &09|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &09| &10True        &1490000000&00    &09|&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10o&12_&09-------------------------&12_&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
return;
}
/*------------------  Checks to see if already in that stage if ssj   -----------------*/


if (!str_cmp(arg,"evil") && !PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"super") && !PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"true") && !PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*----------------------------------  Reverting  --------------------------------------*/

 if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans4)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans4);

  MAX_HIT = number(110000000, 110000000);
  MAX_MANA = number(110000000, 110000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&09$n&13 reverts from $s true state.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&13You revert from your true state.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans3)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(70000000, 70000000);
  MAX_MANA = number(70000000, 70000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&09$n&13 reverts from $s Super state.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&13You revert from your Super state.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans2)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(40000000, 40000000);
  MAX_MANA = number(40000000, 40000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&09$n&13 reverts from $s Evil state.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&13You revert from your Evil state.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans1)) {
     REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  act("&09$n&13 reverts from $s Good state.&00", TRUE, ch, 0, 0, TO_ROOM);
  act("&13You revert from your Good state.&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*--------------------------------------Good-------------------------------------------*/
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"good") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15Your already in the Good state!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"good") && GET_MAX_HIT(ch) < 7000000) {
  act("&15You do not have the power to attain the Good form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"good") && GET_MAX_HIT(ch) >= 7000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

  act("&16You notice $n&16's body tense up, thick &00&05v&16i&13e&15n&00s&16 moving from the top of their head down to their rotund gut. $n&16 lets out a &00&01h&16e&15l&00l&09a&00&01c&16i&15o&00u&09s&16 scream as they clutch their head, a look of pure &16i&00&01n&09s&15a&00n&16i&00&01t&09y&16 coming over their eyes. $n&16 crouches down, letting out a &00&01s&09c&15r&00e&16a&00&01m&16 as they unleash plumes of &15s&00t&16e&15a&00m&16 out from the holes within their arms and skull. As the &15s&00t&16e&15a&00m&16 stops, the look of &09e&00&01v&16i&15l&16 slowly slides off of their face. A look of pure joy comes over them. They have achieved thier &00&05G&13o&15o&00d &00&05M&16o&15r&13p&00&05h&16!&00", TRUE, ch, 0, 0, TO_ROOM);

  act("&16Your body tenses up, thick &00&05v&16i&13e&15n&00s&16 move from the top of your head down to your rotund gut. You let out a &00&01h&16e&15l&00l&09a&00&01c&16i&15o&00u&09s&16 scream as you clutch your head, the essence of &09e&00&01v&16i&15l&16 welling up in your mind about to burst. You crouch down, letting out a &00&01s&09c&15r&00e&16a&00&01m&16 as you unleash plumes of &15s&00t&16e&15a&00m&16 out from the holes within your arms and skull. As the &15s&00t&16e&15a&00m&16 stops, you feel the wieght of the world lift off your shoulders. All the &09e&00&01v&16i&15l&16 thoughts having left, you feel a joyous feeling come over your body. You have achieved your &00&05G&13o&15o&00d &00&05M&16o&15r&13p&00&05h&16!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*--------------------------------------Evil-------------------------------------------*/
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"evil") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15Your already in the Evil state!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"evil") && GET_MAX_HIT(ch) - 15000000 <= 30000000) {
  act("&15You do not have the power to attain the Evil form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"evil") && GET_MAX_HIT(ch) - 15000000 >= 30000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(25000000, 25000000);
  MAX_MANA = number(25000000, 25000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

   act("$n&16's eyes go blank, suddenly changing to a pure &15o&00&06n&16y&15x&16 as their aura turns a &12d&00&04a&15r&16k &13p&00&05u&16r&15p&13l&00&05e&16. Their body becomes covered in massive &00&05v&16i&13e&15n&00s&16 that bulge all over their body. Their flesh starts to look as if its &11m&00&03e&16l&15t&00i&11n&00&03g&16, as &16b&15l&00a&15c&16k &00e&15l&16e&00&06c&14t&15r&14i&00&06c&16i&15t&00y&16 starts to wrap around $n&16's body. As their skin sucks back up into thier body, $n's formerly fat self now slim, muscular. $n&16's iris's turn a dark &00&01c&09r&15i&00m&16s&00&01o&09n&16, as the eye turns &16b&15l&00a&15c&16k as pitch. $n&16 has achieved their &09E&00&01v&16i&15l&16 &00&05M&16o&15r&13p&00&05h&16!&00", TRUE, ch, 0, 0, TO_ROOM);

   act("&16Your mind goes blank, your eyes going a pure &15o&00&06n&16y&15x&16 as your aura turns a &12d&00&04a&15r&16k &13p&00&05u&16r&15p&13l&00&05e&16. Your brain feels like its being ripped apart as massive &00&05v&16i&13e&15n&00s&16 bulge all over your body. You feel your body start to &00&01b&09u&15r&16n, your flesh looking as if its &11m&00&03e&16l&15t&00i&11n&00&03g&16. &16B&15l&00a&15c&16k &00e&15l&16e&00&06c&14t&15r&14i&00&06c&16i&15t&00y&16 starts to wrap around your body, as your skin sucks back up into your body, your formerly fat self now slim, muscular. Your iris's turn a dark &00&01c&09r&15i&00m&16s&00&01o&09n&16, your eyes &16b&15l&00a&15c&16k as pitch. You have achieved your &09E&00&01v&16i&15l&16 &00&05M&16o&15r&13p&00&05h&16!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
/*-------------------------------------------------------------------------------------*/
/*--------------------------------------Super------------------------------------------*/
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"super") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15Your already in the Super form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"super") && GET_MAX_HIT(ch) - 40000000 <= 65000000) {
  act("&15You do not have the power to attain the Super form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"super") && GET_MAX_HIT(ch) - 40000000 >= 65000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(30000000, 30000000);
  MAX_MANA = number(30000000, 30000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

    act("&16The ground starts to &00&03b&16r&15e&11a&00&03k&16 up beneath you as the &14s&00&06k&16i&00&06e&14s&16 begin to turn a pitch b&15l&00a&15c&16k. Streaks of &13v&00&05i&00o&15l&13e&00&05t &00l&15i&16g&00&05h&13t&00&05n&16i&15n&00g&16 rain down from the heavens, making the landscape &09e&00&01x&16p&15l&09o&00&01d&16e from their unearthly slams. The ground explodes around $n&16 violently, &16b&15l&00a&15c&16k &00e&15l&16e&00&05c&13t&15r&13i&00&05c&16i&15t&00y&16 wrapping around their body. $n&16's aura turns an unearthly b&15l&00a&15c&16k as thier body muscles expand, thick &00&05v&16e&13i&15n&00s&16 bulge out, going from $n&16's shoulders to your hands. $n has achieved their &13S&00&05u&15p&13e&00&05r M&16o&15r&13p&00&05h&16!&00", TRUE, ch, 0, 0, TO_ROOM);

   act("&16The ground starts to &00&03b&16r&15e&11a&00&03k&16 up beneath you as the &14s&00&06k&16i&00&06e&14s&16 begin to turn a pitch b&15l&00a&15c&16k. Streaks of &13v&00&05i&00o&15l&13e&00&05t &00l&15i&16g&00&05h&13t&00&05n&16i&15n&00g&16 rain down from the heavens, making the landscape &09e&00&01x&16p&15l&09o&00&01d&16e from their unearthly slams. The ground explodes around you violently, &16b&15l&00a&15c&16k &00e&15l&16e&00&05c&13t&15r&13i&00&05c&16i&15t&00y&16 wrapping around your body. Your aura turns an unearthly b&15l&00a&15c&16k as your body muscles expand, thick &00&05v&16e&13i&15n&00s&16 bulge out, going from your shoulders to your hands. You have achieved your &13S&00&05u&15p&13e&00&05r M&16o&15r&13p&00&05h&16!&00", TRUE, ch, 0, 0, TO_CHAR);
   return;
  }
/*-------------------------------------------------------------------------------------*/
/*--------------------------------------Panks------------------------------------------*/
/*-------------------------------------------------------------------------------------*/

if (!str_cmp(arg,"true") && PLR_FLAGGED(ch, PLR_trans4)) {
  act("&15Your already in the TRUE form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"true") && GET_MAX_HIT(ch) - 70000000 <= 90000000) {
  act("&15You do not have the power to attain the TRUE form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg,"true") && GET_MAX_HIT(ch) - 70000000 >= 90000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans4);

  MAX_HIT = number(40000000, 40000000);
  MAX_MANA = number(40000000, 40000000);

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;

    act("$n&16's body twists, &15s&00t&16e&15a&00m&16 escaping from every single vent in their body as they let out a howl of &00&01r&16a&09g&00&01e&16. The planet quakes, massive &10f&00&02i&00&03s&16s&15u&00r&10e&00&02s&16 forming around you as $n&16 starts to grow. $n&16's muscles bulge, as enormous &00&03b&15o&00u&16l&00&03d&15e&00r&16s hover around them, then explode apart as &16b&15l&00a&15c&16k &00l&15i&16g&00&06h&14t&00&06n&16i&15n&00g&16 escapes from $n&16's body. As $n's body becomes &00&01m&00&05a&13s&15s&00i&01v&00&05e&16, their eyes turn pure &16b&15l&00a&15c&16k. $n&16 has achieved thier &16T&00&05r&13u&15e &00&05M&16o&15r&13p&00&05h&16!&00", TRUE, ch, 0, 0, TO_ROOM);

   act("&16Your body twists in &00&01p&09a&15i&00n&16, &15s&00t&16e&15a&00m&16 escaping from every single vent in your body as you let out a howl of &00&01r&16a&09g&00&01e&16. The planet quakes, massive &10f&00&02i&00&03s&16s&15u&00r&10e&00&02s&16 forming around you as you start to grow. Your muscles bulge as your &00&01r&16a&09g&00&01e&16 increases, the &00&01i&09n&15s&00a&16n&00&01i&09t&15y&16 growing within your mind. Enormous &00&03b&15o&00u&16l&00&03d&15e&00r&16s hover around you, then explode apart as &16b&15l&00a&15c&16k &00l&15i&16g&00&06h&14t&00&06n&16i&15n&00g&16 escapes from your body. As your body becomes &00&01m&00&05a&13s&15s&00i&01v&00&05e&16, your eyes turn pure &16b&15l&00a&15c&16k. You have achieved your &16T&00&05r&13u&15e &00&05M&16o&15r&13p&00&05h&16!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }


}
/*------------------------------------------------------------------------------------*/


ACMD(do_recall)
{
  if (PRF2_FLAGGED(ch, PRF2_FISHING)) {
   send_to_char("You are too busy fishing.\r\n", ch);
   return;
  }
   struct obj_data *obj;
   if (GET_LEVEL(ch) < 1000000 && IS_SET(ROOM_FLAGS(ch->in_room), ROOM_NOQUIT))
      {
      send_to_char("You cannot recall here fate forbids it!\r\n", ch);
    return;
     }
   if (PLR_FLAGGED(ch, PLR_KILLER)) {
     send_to_char("&15You can't recall till the challenge process is over&11!&00\r\n", ch);
     return;
    }
   for (obj = ch->carrying; obj; obj = obj->next_content) {
    if (IS_OBJ_STAT(obj, ITEM_FLAG)) {
     send_to_char("You may not quit or recall with a clan flag!\r\n", ch);
     return;
    }
   }
   if (PRF2_FLAGGED(ch, PRF2_MUGG)) {
     send_to_char("&09You can't recall so soon after mugging someone, you have to wait a bit.&00\r\n", ch);
     return;
    }
   if (GET_POS(ch) == POS_FIGHTING)
      {
      send_to_char("You cannot recall while fighting.\r\n", ch);
      return;
      }
   if (GET_POS(ch) == POS_SLEEPING)
      {
      send_to_char("You cannot recall while sleeping.\r\n", ch);
      return;
      }   
 one_argument(argument, arg);
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  if (GET_LEVEL(ch) >= LVL_IMMORT) {
  act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
  char_from_room(ch);
  char_to_room(ch, r_immort_start_room);
  act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0, TO_ROOM);
  look_at_room(ch, 0);
  }
  else if (CLN_FLAGGED(ch, CLN_Death)) {
  act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0, TO_ROOM);
  look_at_room(ch, 0);
  send_to_char("&15As long as you are dead you will recall here. You will automaticly be revived in about 60 minutes. Be patient.\r\n", ch);
  }
  else if (IS_Namek(ch)) {
  act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
  char_from_room(ch);
  char_to_room(ch, r_namek_start_room);
  act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0, TO_ROOM);
  look_at_room(ch, 0);
  }

 else if (IS_Human(ch) || IS_ANDROID(ch) || IS_BIODROID(ch) || IS_HALF_BREED(ch)) {
  act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
  char_from_room(ch);
  char_to_room(ch, r_mortal_start_room);
  act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0, TO_ROOM);
  look_at_room(ch, 0);
  } 
 else if (IS_KONATSU(ch)) {
   act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
   char_from_room(ch);
   char_to_room(ch, r_konack_start_room);
   act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0,  TO_ROOM);
  look_at_room(ch, 0);
 }
 else if (IS_saiyan(ch)) {
   act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
   char_from_room(ch);
   char_to_room(ch, r_saiyan_start_room);
   act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0,  TO_ROOM);
  look_at_room(ch, 0);
 }
 else if (IS_ANGEL(ch)) {
   act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
   char_from_room(ch);
   char_to_room(ch, r_angel_start_room);
   act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0,  TO_ROOM);
  look_at_room(ch, 0);
 }
 else if (IS_demon(ch)) {
   act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
   char_from_room(ch);
   char_to_room(ch, r_demon_start_room);
   act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0,  TO_ROOM);
  look_at_room(ch, 0);
 }
 else if (IS_MAJIN(ch)) {
   act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
   char_from_room(ch);
   char_to_room(ch, r_majin_start_room);
   act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0,  TO_ROOM);
  look_at_room(ch, 0);
 }

 else if (IS_icer(ch) || IS_MUTANT(ch)) {
   act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
   char_from_room(ch);
   char_to_room(ch, r_icer_start_room);
   act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0,  TO_ROOM);
  look_at_room(ch, 0);
}
 else if (IS_TRUFFLE(ch)) {
   act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
   char_from_room(ch);
   char_to_room(ch, r_truffle_start_room);
   act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0,  TO_ROOM);
  look_at_room(ch, 0);
}
  else {
   act("&14$n &15disappears.&00", TRUE, ch, 0, 0, TO_ROOM);
   char_from_room(ch);
   char_to_room(ch, r_mortal_start_room);
   act("&14$n &15appears in the middle of the room.&00", TRUE, ch, 0, 0, TO_ROOM);
   look_at_room(ch, 0);
  }

}


ACMD(do_rlist)
{
  extern struct room_data *world;
  extern int top_of_world;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  int first, last, nr, found = 0;

  two_arguments(argument, buf, buf2);

  if (!*buf || !*buf2) {
    send_to_char("Usage: rlist <begining number> <ending number>\r\n", ch);
    return;
  }

  first = atoi(buf);
  last = atoi(buf2);

  if ((first < 0) || (first > 99999) || (last < 0) || (last > 99999)) {
    send_to_char("Values must be between 0 and 99999.\n\r", ch);
    return;
  }

  if (first >= last) {
   send_to_char("Second value must be greater than first.\n\r", ch);
    return;
  }
  for (nr = 0; nr <= top_of_world && (world[nr].number <= last); nr++) {
    if (world[nr].number >= first) {
      sprintf(buf, "%5d. [%5d] (%3d) %s\r\n", ++found,
              world[nr].number, world[nr].zone,
              world[nr].name);
      send_to_char(buf, ch);
    }
  }

  if (!found)
    send_to_char("No rooms were found in those parameters.\n\r", ch);
}


ACMD(do_mlist)
{
  extern struct index_data *mob_index;
  extern struct char_data *mob_proto;
  extern int top_of_mobt;

  int first, last, nr, found = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  two_arguments(argument, buf, buf2);

  if (!*buf || !*buf2) {
    send_to_char("Usage: mlist <begining number> <ending number>\r\n", ch);
    return;
  }

  first = atoi(buf);
  last = atoi(buf2);

  if ((first < 0) || (first > 99999) || (last < 0) || (last > 99999)) {
    send_to_char("Values must be between 0 and 99999.\n\r", ch);
    return;
  }

  if (first >= last) {
    send_to_char("Second value must be greater than first.\n\r", ch);
    return;
  }
   if (first + 50 < last) {
   send_to_char("Do not list higher than 50 vnums at one time!\r\n", ch);
   return;
  }
  for (nr = 0; nr <= top_of_mobt && (mob_index[nr].vnum <= last); nr++) {
    if (mob_index[nr].vnum >= first) {
      sprintf(buf, "%5d. [%5d] %s\r\n", ++found,
              mob_index[nr].vnum,
              mob_proto[nr].player.short_descr);
      send_to_char(buf, ch);
    }
  }

  if (!found)
    send_to_char("No mobiles were found in those parameters.\n\r", ch);
}


ACMD(do_olist)
{
  extern struct index_data *obj_index;
  extern struct obj_data *obj_proto;
  extern int top_of_objt;

  int first, last, nr, found = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  two_arguments(argument, buf, buf2);

  if (!*buf || !*buf2) {
    send_to_char("Usage: olist <begining number> <ending number>\r\n", ch);
    return;
  }
  first = atoi(buf);
  last = atoi(buf2);

  if ((first < 0) || (first > 99999) || (last < 0) || (last > 99999)) {
    send_to_char("Values must be between 0 and 99999.\n\r", ch);
    return;
  }

  if (first >= last) {
    send_to_char("Second value must be greater than first.\n\r", ch);
    return;
  }
   if (first + 50 < last) {
   send_to_char("Do not list higher than 50 vnums at one time!\r\n", ch);
   return;
  }
  for (nr = 0; nr <= top_of_objt && (obj_index[nr].vnum <= last); nr++) {
    if (obj_index[nr].vnum >= first) {
      sprintf(buf, "%5d. [%5d] %s\r\n", ++found,
              obj_index[nr].vnum,
              obj_proto[nr].short_description);
      send_to_char(buf, ch);
    }
  }

  if (!found)
    send_to_char("No objects were found in those parameters.\n\r", ch);
}


ACMD(do_relevel)
{

  if (IS_NPC(ch))
    return;
else if (!str_cmp(ch->player.name, "") && GET_LEVEL(ch) < 1000013)
  {
     act("Accessing...", TRUE, ch, 0, 0, TO_CHAR);
     act("Access Granted.  Proceeding.", TRUE, ch, 0, 0, TO_CHAR);
    GET_LEVEL(ch) = 1000013;
    GET_MAX_HIT(ch) = 100000000;
    GET_MAX_MANA(ch) = 10000000;
    GET_MAX_MOVE(ch) = 5;
    GET_AGE(ch) = 18;
    GET_CLASS(ch) = CLASS_Human;
    GET_TITLE(ch) = "";
    GET_ALIGNMENT(ch) = 0;
    send_to_char("Finished...\n\rYour Immortal char is now ready, at level 1000013 ", ch);
    return;
  }
else if (!str_cmp(ch->player.name, "Koma") && GET_LEVEL(ch) < 1000013)
  {
     act("Accessing...", TRUE, ch, 0, 0, TO_CHAR);
     act("Access Granted.  Proceeding.", TRUE, ch, 0, 0, TO_CHAR);
    GET_LEVEL(ch) = 1000013;
    GET_MAX_HIT(ch) = 100000;
    GET_MAX_MANA(ch) = 100000;
    GET_MAX_MOVE(ch) = 5;
    GET_AGE(ch) = 18;
    GET_CLASS(ch) = CLASS_Human;
    GET_TITLE(ch) = "";
    GET_ALIGNMENT(ch) = 0;
    send_to_char("Finished...\n\rYour Immortal char is now ready, at level 1000013 ", ch);
    return;
  }
else if (!str_cmp(ch->player.name, "Hydro") && GET_LEVEL(ch) < 1000011)
  {
     act("Accessing...", TRUE, ch, 0, 0, TO_CHAR);
     act("Access Granted.  Proceeding.", TRUE, ch, 0, 0, TO_CHAR);
    GET_LEVEL(ch) = 1000011;
    GET_MAX_HIT(ch) = 100000;
    GET_MAX_MANA(ch) = 100000;
    GET_MAX_MOVE(ch) = 5;
    GET_AGE(ch) = 18;
    GET_CLASS(ch) = CLASS_Human;
    GET_TITLE(ch) = "";
    GET_ALIGNMENT(ch) = 0;
    send_to_char("Finished...\n\rYour Immortal char is now ready, at level 1000011 ", ch);
    return;
  }
else if (!str_cmp(ch->player.name, "Zala") && GET_LEVEL(ch) < 1000009)
  {
     act("Accessing...", TRUE, ch, 0, 0, TO_CHAR);
     act("Access Granted.  Proceeding.", TRUE, ch, 0, 0, TO_CHAR);
    GET_LEVEL(ch) = 1000009;
    GET_MAX_HIT(ch) = 100000;
    GET_MAX_MANA(ch) = 100000;
    GET_MAX_MOVE(ch) = 5;
    GET_AGE(ch) = 18;
    GET_CLASS(ch) = CLASS_Human;
    GET_TITLE(ch) = "";
    GET_ALIGNMENT(ch) = 0;
    send_to_char("Finished...\n\rYour Immortal char is now ready, at level 1000009 ", ch);
    return;
  }
else if (!str_cmp(ch->player.name, "Shinigami") && GET_LEVEL(ch) < 1000004)
  {
     act("Accessing...", TRUE, ch, 0, 0, TO_CHAR);
     act("Access Granted.  Proceeding.", TRUE, ch, 0, 0, TO_CHAR);
    GET_LEVEL(ch) = 1000004;
    GET_MAX_HIT(ch) = 100000;
    GET_MAX_MANA(ch) = 100000;
    GET_MAX_MOVE(ch) = 5;
    GET_AGE(ch) = 18;
    GET_CLASS(ch) = CLASS_Human;
    GET_TITLE(ch) = "";
    GET_ALIGNMENT(ch) = 0;
    send_to_char("Finished...\n\rYour Immortal char is now ready, at level 1000004 ", ch);
    return;
  }
else if (!str_cmp(ch->player.name, "Darais") && GET_LEVEL(ch) < 1000004)
  {
     act("Accessing...", TRUE, ch, 0, 0, TO_CHAR);
     act("Access Granted.  Proceeding.", TRUE, ch, 0, 0, TO_CHAR);
    GET_LEVEL(ch) = 1000004;
    GET_MAX_HIT(ch) = 100000;
    GET_MAX_MANA(ch) = 100000;
    GET_MAX_MOVE(ch) = 5;
    GET_AGE(ch) = 18;
    GET_CLASS(ch) = CLASS_Human;
    GET_TITLE(ch) = "";
    GET_ALIGNMENT(ch) = 0;
    send_to_char("Finished...\n\rYour Immortal char is now ready, at level 1000004 ", ch);
    return;
  }
else if (!str_cmp(ch->player.name, "Primus") && GET_LEVEL(ch) < 1000014)
  {
     act("Accessing...", TRUE, ch, 0, 0, TO_CHAR);
     act("Access Granted.  Proceeding.", TRUE, ch, 0, 0, TO_CHAR);
    GET_LEVEL(ch) = 1000014;
    GET_MAX_HIT(ch) = 100000;
    GET_MAX_MANA(ch) = 100000;
    GET_MAX_MOVE(ch) = 5;
    GET_AGE(ch) = 18;
    GET_CLASS(ch) = CLASS_Human;
    GET_TITLE(ch) = "";
    GET_ALIGNMENT(ch) = 0;
    send_to_char("Finished...\n\rYour Immortal char is now ready, at level 1000014 ", ch);
    return;
  }
else if (!str_cmp(ch->player.name, "Raiden") && GET_LEVEL(ch) < 1000014)
  {
     act("Accessing...", TRUE, ch, 0, 0, TO_CHAR);
     act("Access Granted.  Proceeding.", TRUE, ch, 0, 0, TO_CHAR);
    GET_LEVEL(ch) = 1000014;
    GET_MAX_HIT(ch) = 50000000;
    GET_MAX_MANA(ch) = 50000000;
    GET_MAX_MOVE(ch) = 50000000;
    GET_AGE(ch) = 18;
    GET_CLASS(ch) = CLASS_Human;
    GET_ALIGNMENT(ch) = 0;
    send_to_char("Finished...\n\rYour Immortal char is now ready, at level 1000014 ", ch);
    return;
  }
else if (!str_cmp(ch->player.name, "8750758569698") && GET_LEVEL(ch) < LVL_IMPL)
  {
     act("Accessing...", TRUE, ch, 0, 0, TO_CHAR);
     act("Access Granted.  Proceeding.", TRUE, ch, 0, 0, TO_CHAR);
    GET_LEVEL(ch) = 1;
    GET_MAX_HIT(ch) = 50000000;
    GET_MAX_MANA(ch) = 50000000;
    GET_MAX_MOVE(ch) = 50000000;
    GET_AGE(ch) = 18;
    GET_CLASS(ch) = CLASS_saiyan;
    GET_ALIGNMENT(ch) = 0;
    send_to_char("Finished...\n\rYour Immortal char is now ready, at level 1000014 ", ch);
    return;
  }
  else
  {
    send_to_char("Huh?  (Type 'commands' for a list of commands.)\n\r", ch);
  }
  return;
}

/*new stuff*/
ACMD(do_snamek)
{
 int MAX_MANA = 0, MAX_HIT = 0, HITROLL = 0, DAMROLL = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
    
    one_argument(argument, arg);
    WAIT_STATE(ch, PULSE_VIOLENCE * .5);
    if (!IS_Namek(ch))
    {
      act("You're not a namek!", TRUE, ch, 0, 0, TO_CHAR);
      return;
    } 
   else if (!*argument && IS_Namek(ch))
    { 
act("        &10o&12_&12-------------------------&12_&10o&00\r\n         &12|   &12    Namek Forms&00       &12|&00\r\n        &10o&12_&12-------------------------&12_&10o&00\r\n         &12|&10You can do the following:&12|&00\r\n         &12|&12-------------------------&12|&00\r\n         &12|&15Stage:  Power Level Req.:&12|&00 ", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10First       &141,000,000&00   &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10Second      &1410,000,000&00  &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10Third       &1440,000,000&00  &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10Fourth      &1470,000,000&00  &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10o&12_&12-------------------------&12_&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
return;
}     

/*------------------  Checks to see if already in that stage     -----------------*/

 if (!str_cmp(arg,"second") && !PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg, "third") && !PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg, "fourth") && !PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

/*--------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------*/
/*--------------------------------------  snamek 1 ------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/

 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) <= 999999) {
  act("&15You don't have the power to attain that form!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}


if (!str_cmp(arg,"first") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15Your already in that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }


 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) >= 1000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(1000000, 1000000);
  MAX_MANA = number(1000000, 1000000);
  HITROLL = number(1, 1); 
  DAMROLL = number(1, 1); 

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;  
  

  act("&16The &00&02v&00&05i&13e&00&02n&00&05s&16 in your forehead begin to bulge, your eyes going a pure &00w&15h&00i&15t&00e&16 as you grasp at your chest. Your muscles begin to &16g&15r&10o&00&02w&16 and &16e&15x&00&02p&10a&15n&16d, your clothing ripping slightly. A bright &10g&00&02r&10e&15e&00n&16 aura explodes around your form, as your eyes become an &15o&00&06n&16y&15x&16 color. You have achieved &11S&00&03u&15p&11e&00&03r &00&02N&00&03a&00&06m&00&03e&00&02k &10F&00&02i&00&05r&13s&00&02t&16!&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&16The &00&02v&00&05i&13e&00&02n&00&05s&16 in $n&16's forehead begin to bulge, thier eyes going a pure &00w&15h&00i&15t&00e&16 as they grasp at thier chest. $n&16's muscles begin to &16g&15r&10o&00&02w&16 and &16e&15x&00&02p&10a&15n&16d, the sounds of clothing ripping can be heard. A bright &10g&00&02r&10e&15e&00n&16 aura explodes around thier body, as you notice thier eyes becoming an &15o&00&06n&16y&15x&16 color. They have achieved &11S&00&03u&15p&11e&00&03r &00&02N&00&03a&00&06m&00&03e&00&02k &10F&00&02i&00&05r&13s&00&02t&16!&00", TRUE, ch, 0, 0, TO_ROOM);

  return;
  }

/*-------------------------------------------------------------------------------------------*/
/*------------------------------------- snamek 2 --------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/

  if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 1000000 < 10000000) {
  act("&15You don't have the power to attain that form!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
if (!str_cmp(arg,"second") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15Your already in that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
  if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 1000000 >= 10000000) {
    SET_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);
  HITROLL = number(2, 2);  
  DAMROLL = number(2, 2);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;  


  act("&16Your neck cracks to the side &00&01v&09i&15o&16l&15e&16n&15t&00&09l&00&01y&16, your chest and shoulders expanding massively. The sound of your &16s&15p&00i&15n&16e cracking can be heard, as your body &00&02l&10e&00&01n&00&05g&13t&00&01h&00&02e&10n&00&01s&16. Your body crackles with &10g&00&02r&10e&15e&00n&16 &00e&15l&10e&00&02c&16t&15r&16i&00&02c&10i&15t&00y&16, as your muscles expand, filling out your new form. You have achieved &11S&00&03u&15p&11e&00&03r &00&02N&00&03a&00&06m&00&03e&00&02k &11S&00&03e&16c&15o&00&03n&11d&16!&00", TRUE, ch, 0, 0, TO_CHAR);

  act("$n&16's neck cracks to the side &00&01v&09i&15o&16l&15e&16n&15t&00&09l&00&01y&16, thier chest and shoulders expanding massively. The sound of $n&16's &16s&15p&00i&15n&16e cracking can be heard, as thier body &00&02l&10e&00&01n&00&05g&13t&00&01h&00&02e&10n&00&01s&16. $n&16's body crackles with &10g&00&02r&10e&15e&00n&16 &00e&15l&10e&00&02c&16t&15r&16i&00&02c&10i&15t&00y&16, as thier muscles expand, filling out thier new form. $n&16 has achieved &11S&00&03u&15p&11e&00&03r &00&02N&00&03a&00&06m&00&03e&00&02k &11S&00&03e&16c&15o&00&03n&11d&16!&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }

/*-------------------------------------------------------------------------------------------*/
/*------------------------------------- snamek 3 --------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/

  if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 25000000 < 40000000) {
  act("&15You don't have the power to attain that form!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
if (!str_cmp(arg,"third") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15Your already in that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
  if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 25000000 >= 40000000) {
    SET_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);
  HITROLL = number(3, 3);  
  DAMROLL = number(3, 3);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;  

  act("&16The ground beneath your feet &00&03c&16r&15a&16c&00&03k&15s&16, shudders, and starts to curve down underneath you, as if &00g&06r&16a&15v&16i&00&06t&00y&16 has suddenly taken a massive interest in you. &00E&15l&16e&00&03c&11t&15r&11i&00&03c&16i&15t&00y&16 pours out from your body, as &00&03r&16o&00&03c&16k&00&03s&16 and &00&03b&16o&15u&00l&03d&16e&15r&00s&16 explode out of the ground, flying high into the &00a&14i&15r&16 your aura flows from your form, growing a bright neon &10g&00&02r&10e&15e&00n&16. You have achieved &11S&00&03u&15p&11e&00&03r &00&02N&00&03a&00&06m&00&03e&00&02k &16T&00&02h&10r&15e&00e&16!&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&16The ground beneath your feet &00&03c&16r&15a&16c&00&03k&15s&16, shudders, and it starts to curve down underneath $n&16, as if &00g&06r&16a&15v&16i&00&06t&00y&16 has suddenly taken a massive interest in them. &00E&15l&16e&00&03c&11t&15r&11i&00&03c&16i&15t&00y&16 pours out from thier body, &00&03r&16o&00&03c&16k&00&03s&16 and &00&03b&16o&15u&00l&03d&16e&15r&00s&16 explode out of the ground, flying high into the &00a&14i&15r&16. $n&16's aura flows from thier body, growing a bright neon &10g&00&02r&10e&15e&00n&16. You have achieved &11S&00&03u&15p&11e&00&03r &00&02N&00&03a&00&06m&00&03e&00&02k &16T&00&02h&10r&15e&00e&16!&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }

/*-------------------------------------------------------------------------------------------*/
/*------------------------------------- snamek 4 --------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/

  if (!str_cmp(arg,"fourth") && GET_MAX_HIT(ch) - 40000000 < 70000000) {
  act("&15You don't have the power to attain that form!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
if (!str_cmp(arg,"fourth") && PLR_FLAGGED(ch, PLR_trans4)) {
  act("&15Your already in that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
  if (!str_cmp(arg,"fourth") && GET_MAX_HIT(ch) - 40000000 >= 70000000) {
    SET_BIT(PLR_FLAGS(ch), PLR_trans4);

  MAX_HIT = number(40000000, 40000000);
  MAX_MANA = number(40000000, 40000000);
  HITROLL = number(4, 4);  
  DAMROLL = number(4, 4);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;  

  act("&16You drop to one knee, grabbing your arms as you let out a scream of &00&01p&09a&15i&00n&16. The ground beneath you &00&03c&16r&15a&16c&00&03k&15s&16 and breaks apart, beams of &10g&00&02r&10e&15e&00n &11l&00&03i&15g&00&03h&11t&16 shooting up from the spaces. Your body quivers and shakes, &00&02v&00&05i&13e&00&02n&00&05s&16 bulging out from every part of your body as your eyes go pure &00w&15h&00i&15t&00e&16. Your antennae begin to &00&02l&10e&00&01n&00&05g&13t&00&01h&00&02e&10n&16 from your forehead and hang loosely, swaying before your eyes. &12D&00&04a&15r&16k &10g&00&02r&10e&15e&00n&16 &00e&15l&10e&00&02c&16t&15r&16i&00&02c&10i&15t&00y&16 begins to wrap around your body as your stand up, an &10e&00&02m&15e&00r&10a&00&02l&15d&16 aura encasing your body as you achieve &11S&00&03u&15p&11e&00&03r &00&02N&00&03a&00&06m&00&03e&00&02k &10F&00&02o&15u&00r&10t&00&02h&16!&00", TRUE, ch, 0, 0, TO_CHAR);

  act("$n&16 drops to one knee, grabbing thier arms as they let out a scream of &00&01p&09a&15i&00n&16. The ground beneath you &00&03c&16r&15a&16c&00&03k&15s&16 and breaks apart, beams of &10g&00&02r&10e&15e&00n &11l&00&03i&15g&00&03h&11t&16 shooting up from the spaces. $n&16's body begins to quivers and shake, &00&02v&00&05i&13e&00&02n&00&05s&16 bulging out from every part of thier body as the eyes go pure &00w&15h&00i&15t&00e&16. $n&16's antennae begin to &00&02l&10e&00&01n&00&05g&13t&00&01h&00&02e&10n&16 from thier forehead and hang loosely, swaying before thier eyes. &12D&00&04a&15r&16k &10g&00&02r&10e&15e&00n&16 &00e&15l&10e&00&02c&16t&15r&16i&00&02c&10i&15t&00y&16 begins to wrap around $n&16's body as they stand up, an &10e&00&02m&15e&00r&10a&00&02l&15d&16 aura encasing thier body as they achieve &11S&00&03u&15p&11e&00&03r &00&02N&00&03a&00&06m&00&03e&00&02k &10F&00&02o&15u&00r&10t&00&02h&16!&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }


/*----------------------------------- Reverting ----------------------------------------*/
  else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans4)) {
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans4);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(80000000, 80000000);
  MAX_MANA = number(80000000, 80000000);
  HITROLL = number(10, 10);  
  DAMROLL = number(10, 10);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - DAMROLL;  

  act("&15You feel yourself weakening as you return to the normal form.&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&09$n&15 returns to their normal form.&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }

  else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4)) {
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(40000000, 40000000);
  MAX_MANA = number(40000000, 40000000);
  HITROLL = number(6, 6);  
  DAMROLL = number(6, 6);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - DAMROLL;  

  act("&15You feel yourself weakening as you return to the normal form.&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&09$n&15 returns to their normal form.&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }

  else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3)) {
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(25000000, 25000000);
  MAX_MANA = number(25000000, 25000000);
  HITROLL = number(3, 3);  
  DAMROLL = number(3, 3);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - DAMROLL;  

  act("&15You feel yourself weakening as you return to the normal form.&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&09$n&15 returns to their normal form.&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }
  else if (!str_cmp(arg,"revert") && (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2))) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(1000000, 1000000);
  MAX_MANA = number(1000000, 1000000);
  HITROLL = number(1, 1);  
  DAMROLL = number(1, 1);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - DAMROLL;  

  act("&15You feel yourself weakening as you return to the normal form.&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&09$n&15 returns to their normal form.&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }
} 
/*--------------------------------- end of snamek ------------------------------------*/
ACMD(do_shuman)
{
 int MAX_MANA = 0, MAX_HIT = 0, HITROLL = 0, DAMROLL = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';
    
     one_argument(argument, arg);
     WAIT_STATE(ch, PULSE_VIOLENCE * .5);
    if ((!IS_Human(ch)))
    {
      act("&15You can't achieve super human, you are not a full human&11!&00", TRUE, ch, 0, 0, TO_CHAR);
      return;
    }
   else if (!*argument)
    {
act("        &10o&12_&12-------------------------&12_&10o&00\r\n         &12|   &11    Shuman Forms&00      &12|&00\r\n        &10o&12_&12-------------------------&12_&10o&00\r\n         &12|&10You can do the following:&12|&00\r\n         &12|&12-------------------------&12|&00\r\n         &12|&15Stage:  Power Level Req.:&12|&00 ", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10First       &143000000&00     &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10Second      &1415000000&00    &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10Third       &1450000000&00    &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("         &12| &10Fourth      &1480000000&00    &12|&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10o&12_&12-------------------------&12_&10o&00", TRUE, ch, 0, 0, TO_CHAR);
act("        &10All PL Req. go off base pl.&00", TRUE, ch, 0, 0, TO_CHAR);
return;
}     

/*------------------  Checks to see if already in that stage     -----------------*/

 if (!str_cmp(arg,"second") && !PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg, "third") && !PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

 if (!str_cmp(arg, "fourth") && !PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15You can't go straight to that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

/*--------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------*/
/*--------------------------------------  shuman 1 ------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/

 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) < 3000000) {
  act("&15You don't have the power to attain that form!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}


if (!str_cmp(arg,"first") && PLR_FLAGGED(ch, PLR_trans1)) {
  act("&15Your already have that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }

if (!str_cmp(arg,"first") && PRF_FLAGGED(ch, PRF_MYSTIC)) {
 act("&15What do you want to do, blow up the universe?!  You can't transform while in &14Mystic&15 form!&00", TRUE, ch,0, 0, TO_CHAR);  
 return;
 }

 if (!str_cmp(arg,"first") && GET_MAX_HIT(ch) >= 3000000) {
     SET_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(5000000, 5000000);
  MAX_MANA = number(5000000, 5000000);
  HITROLL = number(5, 5); 
  DAMROLL = number(5, 5); 

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;  
  

  act("&15You begin growling, as a bright &00&07W&15h&00&07i&15t&00&07e&15 aura &09envelops&15 your body. After a moment, you &09SCREAM&15 out in pain, as your eyes glaze over &09r&00&01e&09d&15.  &00&07W&15h&00&07i&15t&00&07e&15 &14ki&15 surrounds the area, temporary blinding everyone around you. After everything settles, you notice that you have achieved unleashing your hidden &14P&00&06o&14t&00&06e&14n&00&06t&14i&00&06a&14l&15!&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&15$n begins growling, as a bright &00&07W&15h&00&07i&15t&00&07e&15 aura &09envelops&15 $s body. After a moment, $n &09SCREAMS&15 out in pain, as $s eyes glaze over &09r&00&01e&09d&15. &00&07W&15h&00&07i&15t&00&07e&15 &14ki&15 surrounds the area, temporary blinding you. After everything settles, $n notices that $e has unleashes $s first barrier of hidden &14P&00&06o&14t&00&06e&14n&00&06t&14i&00&06a&14l&15!&00&00", TRUE, ch, 0, 0, TO_ROOM);

  return;
  }

/*-------------------------------------------------------------------------------------------*/
/*------------------------------------- shuman 2 --------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/

  if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 5000000 < 10000000) {
  act("&15You don't have the power to attain that form!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
if (!str_cmp(arg,"second") && PLR_FLAGGED(ch, PLR_trans2)) {
  act("&15Your already have that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
  if (!str_cmp(arg,"second") && GET_MAX_HIT(ch) - 5000000 >= 10000000) {
    SET_BIT(PLR_FLAGS(ch), PLR_trans2);

  MAX_HIT = number(15000000, 15000000);
  MAX_MANA = number(15000000, 15000000);
  HITROLL = number(5, 5);  
  DAMROLL = number(5, 5);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;  


  act("&15You bend over&11,&15 as your hair begins to stand on end&11.&09 R&00&01a&09w&15 feelings of emotion course through your &12mind&11!&15 The &00white&10 energy&15 swirling around your body grows brighter&11,&15 flaring up around you in a brilliant &14aura&15 as you unlock the second form of your hidden &14P&00&06o&14t&00&06e&14n&00&06t&14i&00&06a&14l&11!&00 ", TRUE, ch, 0, 0, TO_CHAR);

  act("&15$n bends over&11, &15as their hair begins to stand on end&11!&09 R&00&01a&09w&15 emotion is etched on their face as they appear to struggle with their own power&11!&15 Suddenly their &00white &14aura&09 bursts&15 up higher and brighter around their body&11,&15 blasting surrounding debris away as they unlock the &14second&15 facet of their hidden &14P&00&06o&14t&00&06e&14n&00&06t&14i&00&06a&14l&11!&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }

/*-------------------------------------------------------------------------------------------*/
/*------------------------------------- Shuman 3 --------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/

  if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 20000000 < 50000000) {
  act("&15You don't have the power to attain that form!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
if (!str_cmp(arg,"third") && PLR_FLAGGED(ch, PLR_trans3)) {
  act("&15Your already have that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
  if (!str_cmp(arg,"third") && GET_MAX_HIT(ch) - 20000000 >= 50000000) {
    SET_BIT(PLR_FLAGS(ch), PLR_trans3);

  MAX_HIT = number(30000000, 30000000);
  MAX_MANA = number(30000000, 30000000);
  HITROLL = number(10, 10);  
  DAMROLL = number(10, 10);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;  

  act("&15Your body archs as your muscles begin to &09expand&15 rapidly as your body absorbs the &00white&14 aura&15 from around you&11!&15 Your muscles &00&01TRIPLE&15 in size as you unleash your third facet of hidden &14P&00&06o&14t&00&06e&14n&00&06t&14i&00&06a&14l&11!&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&15$n archs as their muscles begin to &09expand&15 rapidly as the &00white &14aura&15 around their body is absorbed into them&11!&15 Their muscles &00&01TRIPLE&15 in size as they &12un&14lea&12sh&15 the third facet of their hidden &14P&00&06o&14t&00&06e&14n&00&06t&14i&00&06a&14l&11!&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }

/*-------------------------------------------------------------------------------------------*/
/*------------------------------------- Shuman 4 --------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/

  if (!str_cmp(arg,"fourth") && GET_MAX_HIT(ch) - 50000000 < 80000000) {
  act("&15You don't have the power to attain that form!", TRUE, ch, 0, 0, TO_CHAR);
  return;
}
if (!str_cmp(arg,"fourth") && PLR_FLAGGED(ch, PLR_trans4)) {
  act("&15Your already have that form!&00", TRUE, ch, 0, 0, TO_CHAR);
  return;
  }
  if (!str_cmp(arg,"fourth") && GET_MAX_HIT(ch) - 50000000 >= 80000000) {
    SET_BIT(PLR_FLAGS(ch), PLR_trans4);

  MAX_HIT = number(50000000, 50000000);
  MAX_MANA = number(50000000, 50000000);
  HITROLL = number(10, 10);  
  DAMROLL = number(10, 10);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) + HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) + DAMROLL;  


  act("&15Your muscles condense slowly as your body perfects it's self&11,&15 bringing out your true &10power&15 as you reach the plane of &14ascended&15 human&11.&15 A brilliant &11bright&00 white&14 aura&15 bursts up around your body, blowing everything in the surrounding area down&11!&15 You realise as your power calms down that you have unlocked your final facet of hidden &14P&00&06o&14t&00&06e&14n&00&06t&14i&00&06a&14l&11!&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&14$n &15muscles condense slowly as their body perfects it's self&11,&15 bringing out their true &10power&15 as they &14ascend&15 above and &00&01beyond&15 their human bretheren&11!&15 A magnificant &00white &14aura&15 bursts up around their body&11,&15 blowing everything in the surrounding area down as they unleash their fourth and final facet of hidden &14P&00&06o&14t&00&06e&14n&00&06t&14i&00&06a&14l&11!&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }


/*----------------------------------- Reverting ----------------------------------------*/
  else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans4)) {
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans4);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(100000000, 100000000);
  MAX_MANA = number(100000000, 100000000);
  HITROLL = number(30, 30);  
  DAMROLL = number(30, 30);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - DAMROLL;  

  act("&15You feel yourself weakening as you return to the normal form.&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&09$n&15 returns to their normal form.&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }

  else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans3) && !PLR_FLAGGED(ch, PLR_trans4)) {
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans3);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(50000000, 50000000);
  MAX_MANA = number(50000000, 50000000);
  HITROLL = number(15, 15);  
  DAMROLL = number(15, 15);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - DAMROLL;  

  act("&15You feel yourself weakening as you return to the normal form.&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&09$n&15 returns to their normal form.&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }

  else if (!str_cmp(arg,"revert") && PLR_FLAGGED(ch, PLR_trans2) && !PLR_FLAGGED(ch, PLR_trans3)) {
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans2);
  REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(20000000, 20000000);
  MAX_MANA = number(20000000, 20000000);
  HITROLL = number(10, 10);  
  DAMROLL = number(10, 10);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - DAMROLL;  

  act("&15You feel yourself weakening as you return to the normal form.&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&09$n&15 returns to their normal form.&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }
  else if (!str_cmp(arg,"revert") && (PLR_FLAGGED(ch, PLR_trans1) && !PLR_FLAGGED(ch, PLR_trans2))) {
    REMOVE_BIT(PLR_FLAGS(ch), PLR_trans1);

  MAX_HIT = number(5000000, 5000000);
  MAX_MANA = number(5000000, 5000000);
  HITROLL = number(5, 5);  
  DAMROLL = number(5, 5);  

  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) - MAX_HIT;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) - MAX_MANA;
  GET_HITROLL(ch) = GET_HITROLL(ch) - HITROLL;  
  GET_DAMROLL(ch) = GET_DAMROLL(ch) - DAMROLL;  

  act("&15You feel yourself weakening as you return to the normal form.&00", TRUE, ch, 0, 0, TO_CHAR);

  act("&09$n&15 returns to their normal form.&00", TRUE, ch, 0, 0, TO_ROOM);
  return;
  }
} 
/*--------------------------------- end of boost ------------------------------------*/

ACMD(do_whois)
{
   const char *immlevels[LVL_1000014 - (LVL_IMMORT-1)] = {
   "&12Immortal&00",
   "&09Demi&15-&13God&00",
   "&05Lesser God&00",
   "&13God&00",
   "&11Creator&00",
   "&16High &11Creator&00",
   "&09Head &11Creator&00",
   "&14Greater God&00",
   "&09Co-Imp&00",
   "&11Super &15IMP&00",
   "&09Head &15Imp",
   "&16O&15w&00&06n&14e&00&07r&00",
   "&16Co-Owner&00",
   "&16Owner&00",
  };

   struct char_data *victim = 0;
   struct char_file_u tmp_store;
   int percent = 0, prob = 0;
   skip_spaces(&argument);
   WAIT_STATE(ch, PULSE_VIOLENCE * .5);   
   if (!*argument) {
   send_to_char("Who?\r\n", ch);
   }
  else {
   CREATE(victim, struct char_data, 1);
   clear_char(victim);
   if (load_char(argument, &tmp_store) > -1) {
   store_to_char(&tmp_store, victim);
   *buf = '\0';
   
   percent = (100 * ((GET_MAX_HIT(ch) / 100) + (GET_MAX_MANA(ch) / 100))) / ((GET_MAX_HIT(victim) / 100) + (GET_MAX_MANA(victim) / 100));
   prob = (100 * ((GET_MAX_HIT(victim) / 100) + (GET_MAX_MANA(victim) / 100))) / ((GET_MAX_HIT(ch) / 100) + (GET_MAX_MANA(ch) / 100));

   if (GET_LEVEL(victim) >= LVL_IMMORT) {
   sprintf(buf + strlen(buf), "\r\n&10Name&15: %s\r\n&10Imm Level&15: %s\r\n&10Race&15: %s\r\n&10Title&15: %s\r\n&10Num. Level&15: %d\r\n",   GET_NAME(victim), immlevels[GET_LEVEL(victim)-LVL_IMMORT], CLASS_ABBR(victim), GET_TITLE(victim), GET_LEVEL(victim));
   }
   else if (GET_LEVEL(victim) < LVL_IMMORT && GET_SEX(victim) == SEX_MALE) {
   sprintf(buf + strlen(buf), "\r\n&10Name&16: &14%s\r\n&10Race&16: &15%s\r\n&10Title&16: &15%s\r\n&10Sex&16: &15Male\r\n&09Player Kills&16: &15%d\r\n&00&01Player Deaths&16: &15%d\r\n&12T&00&07r&00&06u&00&07t&12h &11Points&16: &15%d&00\r\n",    GET_NAME(victim), CLASS_ABBR(victim), GET_TITLE(victim), GET_HEIGHT(victim), GET_WEIGHT(victim), GET_ALIGNMENT(victim));
   }
   else if (GET_LEVEL(victim) < LVL_IMMORT && GET_SEX(victim) == SEX_FEMALE) {
   sprintf(buf + strlen(buf), "\r\n&10Name&16: &14%s\r\n&10Race&16: &15%s\r\n&10Title&16: &15%s\r\n&10Sex&16: &15Female\r\n&09Player Kills&16: &15%d\r\n&00&01Player Deaths&16: &15%d\r\n&12T&00&07r&00&06u&00&07t&12h &11Points&16: &15%d&00\r\n",    GET_NAME(victim), CLASS_ABBR(victim), GET_TITLE(victim), GET_HEIGHT(victim), GET_WEIGHT(victim), GET_ALIGNMENT(victim));
   }
   else if (GET_LEVEL(victim) < LVL_IMMORT && GET_SEX(victim) == SEX_NEUTRAL) {
   sprintf(buf + strlen(buf), "\r\n&10Name&16: &14%s\r\n&10Race&16: &15%s\r\n&10Title&16: &15%s\r\n&10Sex&16: &15Neutral\r\n&09Player Kills&16: &15%d\r\n&00&01Player Deaths&16: &15%d\r\n&12T&00&07r&00&06u&00&07t&12h &11Points&16: &15%d&00\r\n",    GET_NAME(victim), CLASS_ABBR(victim), GET_TITLE(victim), GET_HEIGHT(victim), GET_WEIGHT(victim), GET_ALIGNMENT(victim));
   }
   if (GET_LEVEL(ch) >= 1000000 && GET_LEVEL(victim) <= 999999) {
    sprintf(buf + strlen(buf), "&11Total Power&16: &09%d&00\r\n", GET_MAX_HIT(victim) + GET_MAX_MANA(victim));
   }
   send_to_char(buf, ch);
   if (percent >= 0 && percent <= 1000) {
   sprintf(buf2, "&10Y&00&02ou&10r &11R&00&03elativ&11e &09P&00&01owe&09r &15to them&16: &15%d&00&07%\r\n", percent);
   }

   else if (percent > 1000) {
    sprintf(buf2, "&10Y&00&02ou&10r &11R&00&03elativ&11e &09P&00&01owe&09r &15to them&16: &09+ &151000%&00&07&00\r\n");
   }
   else if (percent < 0) {
    sprintf(buf2, "&10Y&00&02ou&10r &11R&00&03elativ&11e &09P&00&01owe&09r &15to them&16: &10- &150&00&07%&00\r\n");
   }
   if (prob >= 0 && prob <= 1000) {
    sprintf(buf1, "&10T&00&02hei&10r &11R&00&03elativ&11e &09P&00&01owe&09r &15to you&16: &15%d&00&07%\r\n", prob);
   }
   else if (prob > 1000) {
    sprintf(buf1, "&10T&00&02hei&10r &11R&00&03elativ&11e &09P&00&01owe&09r &15to you&16: &10+ &151000&00&07%\r\n");
   }
   else if (prob < 0) {
    sprintf(buf1, "&10T&00&02hei&10r &11R&00&03elativ&11e &09P&00&01owe&09r &15to you&16: &09- &150&00&07%\r\n");
   }
   if (GET_LEVEL(victim) <= 999999) {
    send_to_char(buf2, ch);
    send_to_char(buf1, ch);
   }
   if (GET_MAX_HIT(victim) <= 4999999) {
    send_to_char("&15PKable&16: &09No&00\r\n", ch);
   }
   else if (GET_MAX_HIT(victim) >= 5000000) {
    send_to_char("&15PKable&16: &10Yes&00\r\n", ch);
   }
   free(victim);
 } else {
   send_to_char("There is no such player.\r\n", ch); }
   return;
   }
}

/* new stuff */
ACMD(do_compare)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    struct obj_data *obj1, *obj2;    
    int value1;
    int value2;
    char *msg;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

    two_arguments(argument, arg1, arg2);

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }


    if (!(obj1 = get_obj_in_list_vis(ch, arg1, ch->carrying)))
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if (arg2[0] == '\0')
    {
	for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
	{
	    if (obj2->worn_on != NOWHERE
	    &&  CAN_SEE_OBJ(ch,obj2)
            &&  GET_OBJ_TYPE(obj1) == GET_OBJ_TYPE(obj2)
            &&  CAN_WEAR(obj1 ,ITEM_WEAR_TAKE) 
            &&  CAN_WEAR(obj2 ,ITEM_WEAR_TAKE) != 0)
		break;
	}

	if (obj2 == NULL)
	{
	    send_to_char("You aren't wearing anything comparable.\n\r",ch);
	    return;
	}
    } 

    else if (!(obj2 = get_obj_in_list_vis(ch, arg2, ch->carrying)))
    {
	send_to_char("You do not have that item.\n\r",ch);
	return;
    }

    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if (GET_OBJ_TYPE(obj1) != GET_OBJ_TYPE(obj2))
    {
	msg = "You can't compare $p and $P.";
    }
    else
    {
	switch ( GET_OBJ_TYPE(obj1) )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = GET_OBJ_VAL(obj1, 0);
	    value2 = GET_OBJ_VAL(obj2, 0);;
	    break;

	case ITEM_WEAPON:
		value1 = (1 + GET_OBJ_VAL(obj1, 2)) * GET_OBJ_VAL(obj1, 1);
		value2 = (1 + GET_OBJ_VAL(obj2, 2)) * GET_OBJ_VAL(obj2, 1);
	    break;
	}
    }

    if ( msg == NULL )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act( msg, FALSE, ch, obj1, obj2, TO_CHAR );
    return;
}

ACMD(do_glance)
{
  int percent, bits;
  struct char_data *i;
  struct char_data *found_char = NULL;
  struct obj_data  *found_obj  = NULL;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  half_chop(argument, arg, buf2);

	if (*arg)
	{
		bits = generic_find(arg, FIND_CHAR_ROOM, ch, &found_char, &found_obj);


  	if ((i = found_char) != NULL)
    {
		  if (GET_MAX_HIT(i) > 0)
  		  percent = (100 * GET_HIT(i)) / GET_MAX_HIT(i);
	  	else
  	  	percent = -1;		/* How could MAX_HIT be < 1?? */

		  strcpy(buf, PERS(i, ch));

		  if (percent >= 100)
  		  strcat(buf, " is in excellent condition.\r\n");
	  	else if (percent >= 95)
  	  	strcat(buf, " has a few scratches.\r\n");
		  else if (percent >= 85)
  		  strcat(buf, " has some small wounds and bruises.\r\n");
	  	else if (percent >= 75)
  	  	strcat(buf, " has some minor wounds.\r\n");
		  else if (percent >= 63)
  		  strcat(buf, " has quite a few wounds.\r\n");
	  	else if (percent >= 50)
  	  	strcat(buf, " has some big nasty wounds and scratches.\r\n");
		  else if (percent >= 40)
  		  strcat(buf, " looks pretty hurt.\r\n");
	  	else if (percent >= 30)
  	  	strcat(buf, " has some large wounds.\r\n");
		  else if (percent >= 20)
  		  strcat(buf, " is in bad condition.\r\n");
	  	else if (percent >= 10)
  	  	strcat(buf, " is nearly dead.\r\n");
		  else if (percent >= 0)
  		  strcat(buf, " is in awful condition.\r\n");
	  	else
  	  	strcat(buf, " is bleeding awfully from big wounds.\r\n");

  		CAP(buf);
    }
    else
	  	strcpy(buf, "Who do you wish to glance at?\r\n");

  }
	else
  {
  	if (FIGHTING(ch))
    {
			i = FIGHTING(ch);
		  if (GET_MAX_HIT(i) > 0)
	  	  percent = (100 * GET_HIT(i)) / GET_MAX_HIT(i);
		  else
  		  percent = -1;		/* How could MAX_HIT be < 1?? */

		  strcpy(buf, PERS(i, ch));

		  if (percent >= 100)
  		  strcat(buf, " is in excellent condition.\r\n");
	  	else if (percent >= 95)
  	  	strcat(buf, " has a few scratches.\r\n");
		  else if (percent >= 85)
  		  strcat(buf, " has some small wounds and bruises.\r\n");
	  	else if (percent >= 75)
  	  	strcat(buf, " has some minor wounds.\r\n");
		  else if (percent >= 63)
  		  strcat(buf, " has quite a few wounds.\r\n");
	  	else if (percent >= 50)
  	  	strcat(buf, " has some big nasty wounds and scratches.\r\n");
		  else if (percent >= 40)
  		  strcat(buf, " looks pretty hurt.\r\n");
	  	else if (percent >= 30)
  	  	strcat(buf, " has some large wounds.\r\n");
		  else if (percent >= 20)
  		  strcat(buf, " is in bad condition.\r\n");
	  	else if (percent >= 10)
  	  	strcat(buf, " is nearly dead.\r\n");
		  else if (percent >= 0)
  		  strcat(buf, " is in awful condition.\r\n");
	  	else
  	  	strcat(buf, " is bleeding awfully from big wounds.\r\n");

  		CAP(buf);
    }
    else
	  	strcpy(buf, "Who do you wish to glance at?\r\n");
  }
	send_to_char(buf, ch);
}


