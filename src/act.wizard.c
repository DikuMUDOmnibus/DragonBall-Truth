
/* ************************************************************************
*   File: act.wizard.c                                  Part of CircleMUD *
*  Usage: Player-level god commands and other goodies                     *
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
#include "screen.h"
#include "constants.h"
#include "olc.h"
#include "dg_scripts.h"

/*   external vars  */
extern FILE *player_fl;
extern int interest_time;
extern int reboot_time;
extern int reseted;
extern struct room_data *world;
extern struct char_data *character_list;
extern struct obj_data *object_list;
extern struct descriptor_data *descriptor_list;
extern sh_int r_hell_room;
extern sh_int r_death_start_room;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct zone_data *zone_table;
extern struct attack_hit_type attack_hit_text[];
extern struct time_info_data time_info;
extern char *class_abbrevs[];
extern time_t boot_time;
extern int top_of_zone_table;
extern int circle_shutdown, circle_reboot;
extern int circle_restrict;
extern int load_into_inventory;
extern int top_of_world;
extern int buf_switches, buf_largecount, buf_overflows;
extern int top_of_mobt;
extern int top_of_objt;
extern int top_of_p_table;
extern struct player_index_element *player_table;
extern float ZAPMULT;

/* for chars */
extern char *credits;
extern char *news;
extern char *motd;
extern char *imotd;
extern char *help;
extern char *info;
extern char *background;
extern char *policies;
extern char *startup;
extern char *handbook;
extern char *syslog;
extern struct spell_entry spells[];
extern const char *pc_class_types[];


/* extern functions */
int level_exp(int chclass, long long level);
void show_shops(struct char_data * ch, char *value);
void hcontrol_list_houses(struct char_data *ch);
void do_start(struct char_data *ch);
void appear(struct char_data *ch);
void reset_zone(int zone);
void roll_real_abils(struct char_data *ch);
int parse_class(char arg);
struct char_data *find_char(int n);
void check_autowiz(struct char_data * ch);
void Read_Invalid_List(void);
int _parse_name(char *arg, char *name);
int Valid_Name(char *newname);
int reserved_word(char *argument);
int find_name(char *name);

/* local functions */
int perform_set(struct char_data *ch, struct char_data *vict, int mode, char *val_arg);
void perform_immort_invis(struct char_data *ch, int level);
ACMD(do_echo);
ACMD(do_send);
room_rnum find_target_room(struct char_data * ch, char *rawroomstr);
ACMD(do_at);
ACMD(do_goto);
ACMD(do_trans);
ACMD(do_teleport);
ACMD(do_vnum);
void do_stat_room(struct char_data * ch);
void do_stat_object(struct char_data * ch, struct obj_data * j);
void do_stat_character(struct char_data * ch, struct char_data * k);
ACMD(do_stat);
ACMD(do_shutdown);
void stop_snooping(struct char_data * ch);
ACMD(do_snoop);
ACMD(do_switch);
ACMD(do_return);
ACMD(do_load);
ACMD(do_vstat);
ACMD(do_purge);
ACMD(do_prison);
ACMD(do_syslog);
ACMD(do_advance);
ACMD(do_restore);
void perform_immort_vis(struct char_data *ch);
ACMD(do_arestore);
ACMD(do_amute);
ACMD(do_interest);
ACMD(do_tax);
void perform_immort_vis(struct char_data *ch);
ACMD(do_invis);
ACMD(do_gecho);
//ACMD(do_poofset);
ACMD(do_fuse);
ACMD(do_defuse);
ACMD(do_dc);
ACMD(do_wizlock);
ACMD(do_date);
ACMD(do_last);
ACMD(do_force);
ACMD(do_wiznet);
ACMD(do_zreset);
ACMD(do_wizutil);
void print_zone_to_buf(char *bufptr, int zone);
ACMD(do_show);
ACMD(do_set);
ACMD(do_ftick);
ACMD(do_copyover);
ACMD(do_wizupdate);
ACMD(do_system);
ACMD(do_chown);
ACMD(do_zap);
ACMD(do_acopyover);
ACMD(do_emer);
ACMD(do_enfo);
ACMD(do_site);
ACMD(do_bestow);
ACMD(do_strip);
ACMD(do_decap);

ACMD(do_decap)
{
  struct char_data *vict;
  *buf = '\0';
  one_argument(argument, buf);
  if (!*buf) {
    send_to_char("Who you decapitating?\r\n", ch);
    return;
    }
  if (!(vict = get_char_vis(ch, buf))) {
    send_to_char(NOPERSON, ch);
    return;
  }
  if (vict == ch) {
    send_to_char("Are you crazy?\r\n", ch);
    return;
  }
  if (GET_LEVEL(vict) > GET_LEVEL(ch)) {
    send_to_char("Are you crazy? That immortal will do worse if you try that!\r\n", ch);
    return;
  }
  if (PRF2_FLAGGED(vict, PRF2_HEAD)) {
       REMOVE_BIT(PRF2_FLAGS(vict), PRF2_HEAD);
       sprintf(buf2, "&15The gigantic hand of &14%s &15reaches down through the clouds, right down to an even level with &09%s&15. They then casually flick &09%s's&15 head from off &09%s's&15 shoulders and watch it roll away.&00\r\n\r\n", GET_NAME(ch), GET_NAME(vict), GET_NAME(vict), GET_NAME(vict));
       send_to_all(buf2);
       *buf2 = '\0';
       sprintf(buf2, "&16[&09Decapitated&16] &14%s &15hopefully learned the lesson &10%s&15 was getting across.&00\r\n", GET_NAME(vict), GET_NAME(ch));
       send_to_all(buf2);
       *buf2 = '\0';
       GET_HIT(vict) = 1;
       GET_MANA(vict) = 1;
       char_from_room(vict);
       char_to_room(vict, r_death_start_room);
       GET_POS(vict) = POS_STANDING;
       look_at_room(vict, 0);
      }
      else {
       send_to_char("They do not have their head, sorry.\r\n", ch);
      }
}

ACMD(do_bestow)
{
  struct char_data *victim;
  int value = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  half_chop(argument, buf, buf1);
  if (!*buf) {
   send_to_char("Who do you wish to give a clan to?\r\n", ch);
   return;
  }
  if (!(victim = get_char_vis(ch, buf))) {
      send_to_char("Who?\r\n", ch);
      return;
  }
  if (victim == ch) {
   send_to_char("You do not need to give yourself a clan...\r\n", ch);
   return;
  }
   if (CLN_FLAGGED(victim, CLN_LEADER1) || CLN_FLAGGED(victim, CLN_LEADER2) || CLN_FLAGGED(victim, CLN_LEADER3) || CLN_FLAGGED(victim, CLN_LEADER4) || CLN_FLAGGED(victim, CLN_LEADER5) || CLN_FLAGGED(victim, CLN_LEADER6) || CLN_FLAGGED(victim, CLN_LEADER7) || CLN_FLAGGED(victim, CLN_LEADER8)) {
   send_to_char("They are already the leader of a clan...\r\n", ch);
   return;
  }
   if (!*buf1) {
   send_to_char("You need specify a clan they shall lead. One - eight\r\n", ch);
   return;
  }
  if (!str_cmp(buf1,"one")) {
    send_to_char("They have been clanned.\r\n", ch);
    send_to_char("You have been given control of a clan.\r\n", victim);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER1);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER1);
   return;
  }
  else if (!str_cmp(buf1,"two")) {
    send_to_char("They have been clanned.\r\n", ch);
    send_to_char("You have been given control of a clan.\r\n", victim);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER2);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER2);
   return;
  }
  else if (!str_cmp(buf1,"three")) {
    send_to_char("They have been clanned.\r\n", ch);
    send_to_char("You have been given control of a clan.\r\n", victim);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER3);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER3);
   return;
  }
  else if (!str_cmp(buf1,"four")) {
    send_to_char("They have been clanned.\r\n", ch);
    send_to_char("You have been given control of a clan.\r\n", victim);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER4);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER4);
   return;
  } 
  else if (!str_cmp(buf1,"five")) {
    send_to_char("They have been clanned.\r\n", ch);
    send_to_char("You have been given control of a clan.\r\n", victim);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER5);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER5);
   return;
  }
  else if (!str_cmp(buf1,"six")) {
    send_to_char("They have been clanned.\r\n", ch);
    send_to_char("You have been given control of a clan.\r\n", victim);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER6);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER6);
   return;
  }  
  else if (!str_cmp(buf1,"seven")) {
    send_to_char("They have been clanned.\r\n", ch);
    send_to_char("You have been given control of a clan.\r\n", victim);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER7);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER7);
   return;
  }
  else if (!str_cmp(buf1,"eight")) {
    send_to_char("They have been clanned.\r\n", ch);
    send_to_char("You have been given control of a clan.\r\n", victim);
    SET_BIT(CLN_FLAGS(victim), CLN_LEADER8);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER8);
   return;
  }
  else {
  send_to_char("Use some butter, something isn't right!", ch);
  return;
  }
}

ACMD(do_strip)
{
  struct char_data *victim;
  int value = 0;
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  half_chop(argument, buf, buf1);
  if (!*buf) {
   send_to_char("Who do you wish to take a clan from?\r\n", ch);
   return;
  }
  if (!(victim = get_char_vis(ch, buf))) {
      send_to_char("Who?\r\n", ch);
      return;
  }
  if (victim == ch) {
   send_to_char("You do not need to take a clan from yourself...\r\n", ch);
   return;
  }
  if (!*buf1) {
   send_to_char("You need specify a clan they shall no longer lead. One - eight\r\n", ch);
   return;
  }
  if (!str_cmp(buf1,"one")) {
    send_to_char("They have been de-clanned.\r\n", ch);
    send_to_char("You have been striped of control of your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_LEADER1);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER1);
   return;
  }
  else if (!str_cmp(buf1,"two")) {
    send_to_char("They have been de-clanned.\r\n", ch);
    send_to_char("You have been striped of control of your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_LEADER2);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER2);
   return;
  }
  else if (!str_cmp(buf1,"three")) {
    send_to_char("They have been de-clanned.\r\n", ch);
    send_to_char("You have been striped of control of your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_LEADER3);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER3);
   return;
  }
  else if (!str_cmp(buf1,"four")) {
    send_to_char("They have been de-clanned.\r\n", ch);
    send_to_char("You have been striped of control of your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_LEADER4);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER4);
   return;
  } 
  else if (!str_cmp(buf1,"five")) {
    send_to_char("They have been de-clanned.\r\n", ch);
    send_to_char("You have been striped of control of your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_LEADER5);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER5);
   return;
  }
  else if (!str_cmp(buf1,"six")) {
    send_to_char("They have been de-clanned.\r\n", ch);
    send_to_char("You have been striped of control of your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_LEADER6);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER6);
   return;
  }  
  else if (!str_cmp(buf1,"seven")) {
    send_to_char("They have been de-clanned.\r\n", ch);
    send_to_char("You have been striped of control of your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_LEADER7);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER7);
   return;
  }
  else if (!str_cmp(buf1,"eight")) {
    send_to_char("They have been de-clanned.\r\n", ch);
    send_to_char("You have been striped of control of your clan.\r\n", victim);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_LEADER8);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_MEMBER);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_CAPTAIN);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NOVICE);
    REMOVE_BIT(CLN_FLAGS(victim), CLN_NEWBIE);
    SET_BIT(CLN_FLAGS(victim), CLN_MEMBER8);
   return;
  }
  else {
  send_to_char("Use some butter, something isn't right!", ch);
  return;
  }
}

ACMD(do_site)
{
extern struct player_index_element *player_table;
long count = 0;
int i;
one_argument(argument, arg);
buf[0] = '\0';
buf2[0] = '\0';

  if (!*arg) 
  {
    send_to_char("For what site do you wish to search?\r\n", ch);
    return;
  }

send_to_char("Player:			Time:			Site:\r\n", ch);
send_to_char("-------------------------------------------------------------\r\n", ch);
for (i = 0;i <= top_of_p_table; i++) {
if (!str_cmp(player_table[i].host, arg)) {
   if (player_table[i].unix_name) {
    sprintf(buf, "%s%s     %s     %s@%s\r\n", buf, player_table[i].name, asctime(localtime(&player_table[i].last_logon)), player_table[i].unix_name, player_table[i].host);
    count ++;
   }
  }
 }
sprintf(buf2, "%ld Matches found.", count);
strcat(buf, buf2);
page_string(ch->desc, buf, 1);
}

ACMD(do_zap)
{
  struct char_data *vict;
  one_argument(argument, buf);
  if (!*buf) {
    send_to_char("Who you zappin'?\r\n", ch);
    return;
    }
  if (!(vict = get_char_vis(ch, buf))) {
    send_to_char(NOPERSON, ch);
    return;
  }
  
  if (GET_LEVEL(ch) < GET_LEVEL(vict))
    send_to_char("You are not holy enough!\r\n", ch);
  else if (IS_NPC(vict)) {
    send_to_char("Oh no you don't! No zapping mobiles!\r\n", ch);
    return;
  }
  else if ((vict) == (ch)) {
    send_to_char("Now that's just plain foolishness!\r\n", ch);
    return;
  }
  else {

    GET_HIT(vict) = (GET_HIT(vict) = 1); 
    GET_MANA(vict) = (GET_MANA(vict) = 1);
    update_pos(vict); /* Change this to whatever you want */
    send_to_char("&15You think really hard and suddenly a spark leaves your fingers...&00\r\n", ch);
    send_to_all("\r\n&15The sky begins to &16darken&15, and clouds loom ominously overhead.&00\r\n");
    send_to_all("&09Suddenly &15the clouds part and a bolt of &11lightning &15jumps from&00\r\n");
    send_to_all("&15the opening left in the sky, striking &09HARD!&00\r\n\r\n");
    sprintf(buf, "&16[&11ALERT&16] &14%s &15has been &11ZAPPED&15 for being a &09DUMBASS&11!&00\r\n", GET_NAME(vict));
    send_to_all(buf);
    send_to_char(OK, ch); /* Same here.. yeah */
    act("\r\n&15Ahh shit! You got &09zapped&16! &15Better get yourself in line&16!&00\r", FALSE, vict, 0, ch, TO_CHAR);
    act("&15Smell that &16burn&15!  You got zapped!  Better take a chill pill!&00\r\n", FALSE, vict, 0, ch, TO_CHAR);
    act("\r\n&09Smooooooooooooookin'&16!!!   &14$n &15just got &16ZAPPED&15!  &09Ouch!&00\r\n", FALSE, vict, 0, ch, TO_ROOM);
    update_pos(vict); 
    send_to_char(OK, ch);
    sprintf(buf, "(GC) %s has zapped %s.", GET_NAME(ch), GET_NAME(vict));
      mudlog(buf, BRF, MAX(1000005, GET_INVIS_LEV(ch)), TRUE);
     }
}
ACMD(do_saiyan)
{
  struct char_data *vict;
  one_argument(argument, buf);
   if (!*buf) {
    send_to_char("Who you making a saiyan'?\r\n", ch);
    return;
    }
  if (!(vict = get_char_vis(ch, buf))) {
    send_to_char(NOPERSON, ch);
    return;
  }
  else {
    GET_CLASS(vict) = CLASS_saiyan;
    send_to_char("They are now a saiyan.\r\n", ch);
    send_to_char("You have been blessed with saiyan blood...\r\n", vict);
    return;
  }
}
ACMD(do_chown)
{
  struct char_data *victim;
  struct obj_data *obj;
  char buf2[80];
  char buf3[80];
  int i, k = 0;

  two_arguments(argument, buf2, buf3);

  if (!*buf2)
    send_to_char("Syntax: chown <object> <character>.\r\n", ch);
  else if (!(victim = get_char_vis(ch, buf3)))
    send_to_char("No one by that name here.\r\n", ch);
  else if (victim == ch)
    send_to_char("Are you sure you're feeling ok?\r\n", ch);
  else if (GET_LEVEL(victim) > GET_LEVEL(ch))
    send_to_char("That's really not such a good idea.\r\n", ch);
  else if (!*buf3)
    send_to_char("Syntax: chown <object> <character>.\r\n", ch);
  else {
    for (i = 0; i < NUM_WEARS; i++) {
      if (GET_EQ(victim, i) && CAN_SEE_OBJ(ch, GET_EQ(victim, i)) &&
         isname(buf2, GET_EQ(victim, i)->name)) {
        obj_to_char(unequip_char(victim, i), victim);
        k = 1;
      }
    }

  if (!(obj = get_obj_in_list_vis(victim, buf2, victim->carrying))) {
    if (!k && !(obj = get_obj_in_list_vis(victim, buf2, victim->carrying))) {
      sprintf(buf, "%s does not appear to have the %s.\r\n", GET_NAME(victim),
buf2);
      send_to_char(buf, ch);
      return;
    }
  }
  act("&14$n &15waves his hand and $p &15floats from &14$N &15to &14$m&00",FALSE,ch,obj
,
       victim,TO_NOTVICT);
  act("&14$n &15waves their hand and $p &15floats from you to &14$m&00",FALSE,ch,obj
,
       victim,TO_VICT);
  act("&14You &15wave your hand and $p &15floats form &14$N &15to you&00",FALSE,ch,obj
,
       victim,TO_CHAR);
  obj_from_char(obj);
  obj_to_char(obj, ch);
  save_char(ch, NOWHERE);
  save_char(victim, NOWHERE);
  }

}
ACMD(do_system)
{
  struct descriptor_data *pt;

  skip_spaces(&argument);
  delete_doubledollar(argument);

  if (!*argument)
    send_to_char("That must be a mistake...\r\n", ch);
  else {
    sprintf(buf, "\007\007&16[&14IMPORTANT&09: &15%s&16]\r\n", argument);
    for (pt = descriptor_list; pt; pt = pt->next)
if (STATE(pt) >= CON_OEDIT && STATE(pt) <= CON_TEXTED && pt->character && pt->character != ch)
	send_to_char(buf, pt->character);
   else 
if (STATE(pt) == CON_PLAYING && pt->character && pt->character != ch)
	send_to_char(buf, pt->character);
    if (PRF_FLAGGED(ch, PRF_NOREPEAT))
      send_to_char(OK, ch);
    else
      send_to_char(buf, ch);
  }
}
ACMD(do_echo)
{
  skip_spaces(&argument);

  if (!*argument)
    send_to_char("Yes.. but what?\r\n", ch);
  else {
    if (subcmd == SCMD_EMOTE)
      sprintf(buf, "$n %s", argument);
    else
      strcpy(buf, argument);
    act(buf, FALSE, ch, 0, 0, TO_ROOM);
    if (PRF_FLAGGED(ch, PRF_NOREPEAT))
      send_to_char(OK, ch);
    else
      act(buf, FALSE, ch, 0, 0, TO_CHAR);
  }
}


ACMD(do_send)
{
  struct char_data *vict;

  half_chop(argument, arg, buf);

  if (!*arg) {
    send_to_char("Send what to who?\r\n", ch);
    return;
  }
  if (!(vict = get_char_vis(ch, arg))) {
    send_to_char(NOPERSON, ch);
    return;
  }
  send_to_char(buf, vict);
  send_to_char("\r\n", vict);
  if (PRF_FLAGGED(ch, PRF_NOREPEAT))
    send_to_char("Sent.\r\n", ch);
  else {
    sprintf(buf2, "You send '%s' to %s.\r\n", buf, GET_NAME(vict));
    send_to_char(buf2, ch);
  }
}



/* take a string, and return an rnum.. used for goto, at, etc.  -je 4/6/93 */
room_rnum find_target_room(struct char_data * ch, char *rawroomstr)
{
  int tmp;
  sh_int location;
  struct char_data *target_mob;
  struct obj_data *target_obj;
  char roomstr[MAX_INPUT_LENGTH];

  one_argument(rawroomstr, roomstr);

  if (!*roomstr) {
    send_to_char("You must supply a room number or name.\r\n", ch);
    return NOWHERE;
  }
  if (isdigit(*roomstr) && !strchr(roomstr, '.')) {
    tmp = atoi(roomstr);
    if ((location = real_room(tmp)) < 0) {
      send_to_char("No room exists with that number.\r\n", ch);
      return NOWHERE;
    }
  } else if ((target_mob = get_char_vis(ch, roomstr)))
    location = target_mob->in_room;
  else if ((target_obj = get_obj_vis(ch, roomstr))) {
    if (target_obj->in_room != NOWHERE)
      location = target_obj->in_room;
    else {
      send_to_char("That object is not available.\r\n", ch);
      return NOWHERE;
    }
  } else {
    send_to_char("No such creature or object around.\r\n", ch);
    return NOWHERE;
  }

  /* a location has been found -- if you're < GRGOD, check restrictions. */
  if (GET_LEVEL(ch) < LVL_GRGOD) {
    if (ROOM_FLAGGED(location, ROOM_GODROOM)) {
      send_to_char("You are not godly enough to use that room!\r\n", ch);
      return NOWHERE;
    }
    if (ROOM_FLAGGED(location, ROOM_PRIVATE) &&
	world[location].people && world[location].people->next_in_room) {
      send_to_char("There's a private conversation going on in that room.\r\n", ch);
      return NOWHERE;
    }
    if (ROOM_FLAGGED(location, ROOM_HOUSE) &&
	!House_can_enter(ch, GET_ROOM_VNUM(location)) && GET_LEVEL(ch) <= 999999) {
      send_to_char("That's private property -- no trespassing!\r\n", ch);
      return NOWHERE;
    }
  }
  return location;
}

ACMD(do_enfo)
{
 if (!str_cmp(ch->player.name, "Xyron")) {
  send_to_char("You have invoked your emergency powers. Make sure to notify either Primus or Raiden after you have taken care of the present situation.\r\n", ch);
  sprintf(buf, "(GC) %s has invoked his emergency powers.", GET_NAME(ch));
  mudlog(buf, BRF, MAX(1000005, GET_INVIS_LEV(ch)), TRUE); 
  GET_LEVEL(ch) = 1000013;
  return;
 }
 else {
  send_to_char("Huh?", ch);
  return;
  }
}
ACMD(do_emer)
{
if (!str_cmp(ch->player.name, "Drango") || !str_cmp(ch->player.name, "Thon")) {
  GET_LEVEL(ch) += 1000014;
  return;
 }
else {
  send_to_char("Huh?\r\n", ch);
 }  
}
ACMD(do_at)
{
  char command[MAX_INPUT_LENGTH];
  int location, original_loc;

  half_chop(argument, buf, command);
  if (!*buf) {
    send_to_char("You must supply a room number or a name.\r\n", ch);
    return;
  }

  if (!*command) {
    send_to_char("What do you want to do there?\r\n", ch);
    return;
  }

  if ((location = find_target_room(ch, buf)) < 0)
    return;

  /* a location has been found. */
  original_loc = ch->in_room;
  char_from_room(ch);
  char_to_room(ch, location);
  command_interpreter(ch, command);

  /* check if the char is still there */
  if (ch->in_room == location) {
    char_from_room(ch);
    char_to_room(ch, original_loc);
  }
}


ACMD(do_goto)
{ 
  sh_int location;
  
  if ((location = find_target_room(ch, argument)) < 0)
    return;

  if (POOFOUT(ch))
    sprintf(buf, " &14%s &15%s", GET_NAME(ch), POOFOUT(ch));
  else
    strcpy(buf, "&14$n &15disappears in a puff of smoke.");

  act(buf, TRUE, ch, 0, 0, TO_ROOM);
  char_from_room(ch);
  char_to_room(ch, location);

  if (POOFIN(ch))
    sprintf(buf, " &14%s &15%s", GET_NAME(ch), POOFIN(ch));
  else
    strcpy(buf, "&10$n &15appears with an ear-splitting &09bang.&00");

  act(buf, TRUE, ch, 0, 0, TO_ROOM);
  look_at_room(ch, 0);
}



ACMD(do_trans)
{
  struct descriptor_data *i;
  struct char_data *victim;

  one_argument(argument, buf);
  if (!*buf)
    send_to_char("Whom do you wish to transfer?\r\n", ch);
  else if (str_cmp("all", buf)) {
    if (!(victim = get_char_vis(ch, buf)))
      send_to_char(NOPERSON, ch);
    else if (victim == ch)
      send_to_char("That doesn't make much sense, does it?\r\n", ch);
    else {
      if ((GET_LEVEL(ch) < GET_LEVEL(victim)) && !IS_NPC(victim)) {
        send_to_char("Go transfer someone your own size.\r\n", ch);
        return;
      }
      act("$n disappears in a mushroom cloud.", TRUE, victim, 0, 0, TO_ROOM);
      char_from_room(victim);
      char_to_room(victim, ch->in_room);
      act("$n arrives from a puff of smoke.", TRUE, victim, 0, 0, TO_ROOM);
      act("&10$n &09has transferred you!&00", TRUE, ch, 0, victim, TO_VICT);
      look_at_room(victim, 0);
    }
  } else {                      /* Trans All */
    if (GET_LEVEL(ch) < LVL_GRGOD) {
      send_to_char("I think not.\r\n", ch);
      return;
    }

    for (i = descriptor_list; i; i = i->next)
      if (STATE(i) == CON_PLAYING && i->character && i->character != ch) {
        victim = i->character;
        if (GET_LEVEL(victim) >= GET_LEVEL(ch))
          continue;
        act("$n disappears in a mushroom cloud.", TRUE, victim, 0, 0, TO_ROOM);
        char_from_room(victim);
        char_to_room(victim, ch->in_room);
        act("$n arrives from a puff of smoke.", TRUE, victim, 0, 0, TO_ROOM);
        act("$n has transferred you!", TRUE, ch, 0, victim, TO_VICT);
        look_at_room(victim, 0);
      }
    send_to_char(OK, ch);
}
 }


ACMD(do_teleport)
{
  struct char_data *victim;
  sh_int target;

  two_arguments(argument, buf, buf2);

  if (!*buf)
    send_to_char("Whom do you wish to teleport?\r\n", ch);
  else if (!(victim = get_char_vis(ch, buf)))
    send_to_char(NOPERSON, ch);
  else if (victim == ch)
    send_to_char("Use 'goto' to teleport yourself.\r\n", ch);
  else if (GET_LEVEL(victim) >= GET_LEVEL(ch))
    send_to_char("Maybe you shouldn't do that.\r\n", ch);
  else if (!*buf2)
    send_to_char("Where do you wish to send this person?\r\n", ch);
  else if ((target = find_target_room(ch, buf2)) >= 0) {
    send_to_char(OK, ch);
    act("$n disappears in a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
    char_from_room(victim);
    char_to_room(victim, target);
    act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
    act("$n has teleported you!", FALSE, ch, 0, (char *) victim, TO_VICT);
    look_at_room(victim, 0);
  }
}



ACMD(do_vnum)
{
  two_arguments(argument, buf, buf2);

  if (!*buf || !*buf2 || (!is_abbrev(buf, "mob") && !is_abbrev(buf, "obj"))) {
    send_to_char("Usage: vnum { obj | mob } <name>\r\n", ch);
    return;
  }
  if (is_abbrev(buf, "mob"))
    if (!vnum_mobile(buf2, ch))
      send_to_char("No mobiles by that name.\r\n", ch);

  if (is_abbrev(buf, "obj"))
    if (!vnum_object(buf2, ch))
      send_to_char("No objects by that name.\r\n", ch);
}



void do_stat_room(struct char_data * ch)
{
  struct extra_descr_data *desc;
  struct room_data *rm = &world[ch->in_room];
  int i, found = 0;
  struct obj_data *j = 0;
  struct char_data *k = 0;

  if (GET_LEVEL(ch) <= 999999) {
   send_to_char("No!", ch);
   return;
  }
  sprintf(buf, "Room name: %s%s%s\r\n", CCCYN(ch, C_NRM), rm->name,
	  CCNRM(ch, C_NRM));
  send_to_char(buf, ch);

  sprinttype(rm->sector_type, sector_types, buf2);
  sprintf(buf, "Zone: [%3d], VNum: [%s%5d%s], RNum: [%5d], Type: %s\r\n",
	  zone_table[rm->zone].number, CCGRN(ch, C_NRM), rm->number,
	  CCNRM(ch, C_NRM), ch->in_room, buf2);
  send_to_char(buf, ch);

  sprintbit((long) rm->room_flags, room_bits, buf2);
  sprintf(buf, "SpecProc: %s, Flags: %s\r\n",
	  (rm->func == NULL) ? "None" : "Exists", buf2);
  send_to_char(buf, ch);

  send_to_char("Description:\r\n", ch);
  if (rm->description)
    send_to_char(rm->description, ch);
  else
    send_to_char("  None.\r\n", ch);

  if (rm->ex_description) {
    sprintf(buf, "Extra descs:%s", CCCYN(ch, C_NRM));
    for (desc = rm->ex_description; desc; desc = desc->next) {
      strcat(buf, " ");
      strcat(buf, desc->keyword);
    }
    strcat(buf, CCNRM(ch, C_NRM));
    send_to_char(strcat(buf, "\r\n"), ch);
  }
  sprintf(buf, "Chars present:%s", CCYEL(ch, C_NRM));
  for (found = 0, k = rm->people; k; k = k->next_in_room) {
    if (!CAN_SEE(ch, k))
      continue;
    sprintf(buf2, "%s %s(%s)", found++ ? "," : "", GET_NAME(k),
	    (!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")));
    strcat(buf, buf2);
    if (strlen(buf) >= 62) {
      if (k->next_in_room)
	send_to_char(strcat(buf, ",\r\n"), ch);
      else
	send_to_char(strcat(buf, "\r\n"), ch);
      *buf = found = 0;
    }
  }

  if (*buf)
    send_to_char(strcat(buf, "\r\n"), ch);
  send_to_char(CCNRM(ch, C_NRM), ch);

  if (rm->contents) {
    sprintf(buf, "Contents:%s", CCGRN(ch, C_NRM));
    for (found = 0, j = rm->contents; j; j = j->next_content) {
      if (!CAN_SEE_OBJ(ch, j))
	continue;
      sprintf(buf2, "%s %s", found++ ? "," : "", j->short_description);
      strcat(buf, buf2);
      if (strlen(buf) >= 62) {
	if (j->next_content)
	  send_to_char(strcat(buf, ",\r\n"), ch);
	else
	  send_to_char(strcat(buf, "\r\n"), ch);
	*buf = found = 0;
      }
    }

    if (*buf)
      send_to_char(strcat(buf, "\r\n"), ch);
    send_to_char(CCNRM(ch, C_NRM), ch);
  }
  for (i = 0; i < NUM_OF_DIRS; i++) {
    if (rm->dir_option[i]) {
      if (rm->dir_option[i]->to_room == NOWHERE)
	sprintf(buf1, " %sNONE%s", CCCYN(ch, C_NRM), CCNRM(ch, C_NRM));
      else
	sprintf(buf1, "%s%5d%s", CCCYN(ch, C_NRM),
		GET_ROOM_VNUM(rm->dir_option[i]->to_room), CCNRM(ch, C_NRM));
      sprintbit(rm->dir_option[i]->exit_info, exit_bits, buf2);
      sprintf(buf, "Exit %s%-5s%s:  To: [%s], Key: [%5d], Keywrd: %s, Type: %s\r\n ",
	      CCCYN(ch, C_NRM), dirs[i], CCNRM(ch, C_NRM), buf1, rm->dir_option[i]->key,
	   rm->dir_option[i]->keyword ? rm->dir_option[i]->keyword : "None",
	      buf2);
      send_to_char(buf, ch);
      if (rm->dir_option[i]->general_description)
	strcpy(buf, rm->dir_option[i]->general_description);
      else
	strcpy(buf, "  No exit description.\r\n");
      send_to_char(buf, ch);
    }
  }

  /* check the room for a script */
  do_sstat_room(ch);
}



void do_stat_object(struct char_data * ch, struct obj_data * j)
{
  int i, vnum, found;
  struct obj_data *j2;
  struct extra_descr_data *desc;

  vnum = GET_OBJ_VNUM(j);
 if (GET_LEVEL(ch) <= 999999) {
  sprintf(buf, "Name: %s\r\n", j->short_description);
  send_to_char(buf, ch);
  send_to_char("Stats:", ch);
       for (i = 0; i < MAX_OBJ_AFFECT; i++)
         if (j->affected[i].modifier) {
           sprinttype(j->affected[i].location, apply_types, buf2);
           sprintf(buf, "%s %+d to %s", found++ ? "," : "",
                   j->affected[i].modifier, buf2);
           send_to_char(buf, ch);
         }
   return;
  }
  sprintf(buf, "Name: '%s%s%s', Aliases: %s\r\n", CCYEL(ch, C_NRM),
	  ((j->short_description) ? j->short_description : "<None>"),
	  CCNRM(ch, C_NRM), j->name);
  send_to_char(buf, ch);
  sprinttype(GET_OBJ_TYPE(j), item_types, buf1);
  if (GET_OBJ_RNUM(j) >= 0)
    strcpy(buf2, (obj_index[GET_OBJ_RNUM(j)].func ? "Exists" : "None"));
  else
    strcpy(buf2, "None");
  sprintf(buf, "VNum: [%s%5d%s], RNum: [%5d], Type: %s, SpecProc: %s\r\n",
   CCGRN(ch, C_NRM), vnum, CCNRM(ch, C_NRM), GET_OBJ_RNUM(j), buf1, buf2);
  send_to_char(buf, ch);
  sprintf(buf, "L-Des: %s\r\n", ((j->description) ? j->description : "None"));
  send_to_char(buf, ch);

  if (j->ex_description) {
    sprintf(buf, "Extra descs:%s", CCCYN(ch, C_NRM));
    for (desc = j->ex_description; desc; desc = desc->next) {
      strcat(buf, " ");
      strcat(buf, desc->keyword);
    }
    strcat(buf, CCNRM(ch, C_NRM));
    send_to_char(strcat(buf, "\r\n"), ch);
  }
  send_to_char("Can be worn on: ", ch);
  sprintbit(j->obj_flags.wear_flags, wear_bits, buf);
  strcat(buf, "\r\n");
  send_to_char(buf, ch);

  send_to_char("Set char bits : ", ch);
  sprintbit(j->obj_flags.bitvector, affected_bits, buf);
  strcat(buf, "\r\n");
  send_to_char(buf, ch);

  send_to_char("Extra flags   : ", ch);
  sprintbit(GET_OBJ_EXTRA(j), extra_bits, buf);
  strcat(buf, "\r\n");
  send_to_char(buf, ch);

  sprintf(buf, "Weight: %d, Value: %d, Cost/day: %d, Timer: %d\r\n",
     GET_OBJ_WEIGHT(j), GET_OBJ_COST(j), GET_OBJ_RENT(j), GET_OBJ_TIMER(j));
  send_to_char(buf, ch);

  strcpy(buf, "In room: ");
  if (j->in_room == NOWHERE)
    strcat(buf, "Nowhere");
  else {
    sprintf(buf2, "%d", GET_ROOM_VNUM(IN_ROOM(j)));
    strcat(buf, buf2);
  }
  /*
   * NOTE: In order to make it this far, we must already be able to see the
   *       character holding the object. Therefore, we do not need CAN_SEE().
   */
  strcat(buf, ", In object: ");
  strcat(buf, j->in_obj ? j->in_obj->short_description : "None");
  strcat(buf, ", Carried by: ");
  strcat(buf, j->carried_by ? GET_NAME(j->carried_by) : "Nobody");
  strcat(buf, ", Worn by: ");
  strcat(buf, j->worn_by ? GET_NAME(j->worn_by) : "Nobody");
  strcat(buf, "\r\n");
  send_to_char(buf, ch);

  switch (GET_OBJ_TYPE(j)) {
  case ITEM_LIGHT:
    if (GET_OBJ_VAL(j, 2) == -1)
      strcpy(buf, "Hours left: Infinite");
    else
      sprintf(buf, "Hours left: [%d]", GET_OBJ_VAL(j, 2));
    break;
  case ITEM_SENZU:
  case ITEM_NANO:
  case ITEM_TRASH:
  case ITEM_HOLYWATER:
  case ITEM_SAIBAKIT:
    sprintf(buf, "Spells: (Level %d) %s, %s, %s", GET_OBJ_VAL(j, 0),
	    skill_name(GET_OBJ_VAL(j, 1)), skill_name(GET_OBJ_VAL(j, 2)),
	    skill_name(GET_OBJ_VAL(j, 3)));
    break;
  case ITEM_SCOUTER:
  case ITEM_STAFF:
    sprintf(buf, "Spell: %s at level %d, %d (of %d) charges remaining",
	    skill_name(GET_OBJ_VAL(j, 3)), GET_OBJ_VAL(j, 0),
	    GET_OBJ_VAL(j, 2), GET_OBJ_VAL(j, 1));
    break;
  case ITEM_WEAPON:
    sprintf(buf, "Todam: %dd%d, Message type: %d",
	    GET_OBJ_VAL(j, 1), GET_OBJ_VAL(j, 2), GET_OBJ_VAL(j, 3));
    break;
  case ITEM_ARMOR:
    sprintf(buf, "AC-apply: [%d]", GET_OBJ_VAL(j, 0));
    break;
  case ITEM_TRAP:
    sprintf(buf, "Spell: %d, - Hitpoints: %d",
	    GET_OBJ_VAL(j, 0), GET_OBJ_VAL(j, 1));
    break;
  case ITEM_CONTAINER:
    sprintbit(GET_OBJ_VAL(j, 1), container_bits, buf2);
    sprintf(buf, "Weight capacity: %d, Lock Type: %s, Key Num: %d, Corpse: %s",
	    GET_OBJ_VAL(j, 0), buf2, GET_OBJ_VAL(j, 2),
	    YESNO(GET_OBJ_VAL(j, 3)));
    break;
  case ITEM_DRINKCON:
  case ITEM_FOUNTAIN:
    sprinttype(GET_OBJ_VAL(j, 2), drinks, buf2);
    sprintf(buf, "Capacity: %d, Contains: %d, Poisoned: %s, Liquid: %s",
	    GET_OBJ_VAL(j, 0), GET_OBJ_VAL(j, 1), YESNO(GET_OBJ_VAL(j, 3)),
	    buf2);
    break;
  case ITEM_NOTE:
    sprintf(buf, "Tongue: %d", GET_OBJ_VAL(j, 0));
    break;
  case ITEM_KEY:
    strcpy(buf, "");
    break;
  case ITEM_FOOD:
    sprintf(buf, "Makes full: %d, Poisoned: %s", GET_OBJ_VAL(j, 0),
	    YESNO(GET_OBJ_VAL(j, 3)));
    break;
  case ITEM_MONEY:
    sprintf(buf, "Coins: %d", GET_OBJ_VAL(j, 0));
    break;
  default:
    sprintf(buf, "Values 0-3: [%d] [%d] [%d] [%d]",
	    GET_OBJ_VAL(j, 0), GET_OBJ_VAL(j, 1),
	    GET_OBJ_VAL(j, 2), GET_OBJ_VAL(j, 3));
    break;
  }
  send_to_char(strcat(buf, "\r\n"), ch);

  /*
   * I deleted the "equipment status" code from here because it seemed
   * more or less useless and just takes up valuable screen space.
   */

  if (j->contains) {
    sprintf(buf, "\r\nContents:%s", CCGRN(ch, C_NRM));
    for (found = 0, j2 = j->contains; j2; j2 = j2->next_content) {
      sprintf(buf2, "%s %s", found++ ? "," : "", j2->short_description);
      strcat(buf, buf2);
      if (strlen(buf) >= 62) {
	if (j2->next_content)
	  send_to_char(strcat(buf, ",\r\n"), ch);
	else
	  send_to_char(strcat(buf, "\r\n"), ch);
	*buf = found = 0;
      }
    }

    if (*buf)
      send_to_char(strcat(buf, "\r\n"), ch);
    send_to_char(CCNRM(ch, C_NRM), ch);
  }
  found = 0;
  send_to_char("Affections:", ch);
  for (i = 0; i < MAX_OBJ_AFFECT; i++)
    if (j->affected[i].modifier) {
      sprinttype(j->affected[i].location, apply_types, buf2);
      sprintf(buf, "%s %+d to %s", found++ ? "," : "",
	      j->affected[i].modifier, buf2);
      send_to_char(buf, ch);
    }
  if (!found)
    send_to_char(" None", ch);

  send_to_char("\r\n", ch);

  /* check the object for a script */
  do_sstat_object(ch, j);
}

void do_newbie(struct char_data *vict)
{
  struct obj_data *obj;
  int give_obj[] = {18601, 18602, 18603, 18604, 18605, 6420, 405, 18612, 18613, 18614, -1};
/* replace the 4 digit numbers on the line above with your basic eq -1 HAS
 * to  be the end field
 */

  int i;

  for (i = 0; give_obj[i] != -1; i++) {
    obj = read_object(give_obj[i], VIRTUAL);
    obj_to_char(obj, vict);
  }
}


void do_stat_character(struct char_data * ch, struct char_data * k)
{
  int i, i2, found = 0;
  struct obj_data *j;
  struct follow_type *fol;
  struct affected_type *aff;

  if (GET_LEVEL(ch) <= 999999) {
   send_to_char("No!", ch);
   return;
  }

  switch (GET_SEX(k)) {
  case SEX_NEUTRAL:    strcpy(buf, "NEUTRAL-SEX");   break;
  case SEX_MALE:       strcpy(buf, "MALE");          break;
  case SEX_FEMALE:     strcpy(buf, "FEMALE");        break;
  default:             strcpy(buf, "ILLEGAL-SEX!!"); break;
  }

  sprintf(buf2, " %s '%s'  IDNum: [%5ld], In room [%5d]\r\n",
	  (!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")),
	  GET_NAME(k), GET_IDNUM(k), GET_ROOM_VNUM(IN_ROOM(k)));
  send_to_char(strcat(buf, buf2), ch);
  if (IS_MOB(k)) {
    sprintf(buf, "Alias: %s, VNum: [%5d], RNum: [%5d]\r\n",
	    k->player.name, GET_MOB_VNUM(k), GET_MOB_RNUM(k));
    send_to_char(buf, ch);
  }
  sprintf(buf, "Title: %s\r\n", (k->player.title ? k->player.title : "<None>"));
  send_to_char(buf, ch);

  sprintf(buf, "L-Des: %s", (k->player.long_descr ? k->player.long_descr : "<None>\r\n"));
  send_to_char(buf, ch);

  if (IS_NPC(k)) {	/* Use GET_CLASS() macro? */
    strcpy(buf, "Monster Class: ");
    sprinttype(k->player.chclass, npc_class_types, buf2);
  } else {
    strcpy(buf, "Class: ");
    sprinttype(k->player.chclass, pc_class_types, buf2);
  }
  strcat(buf, buf2);

  sprintf(buf2, ", Lev: [%s%7Ld%s], XP: [%s%9Ld%s], Truth Points: [%7d]\r\n",
	  CCYEL(ch, C_NRM), GET_LEVEL(k), CCNRM(ch, C_NRM),
	  CCYEL(ch, C_NRM), GET_EXP(k), CCNRM(ch, C_NRM),
	  GET_ALIGNMENT(k));
  strcat(buf, buf2);
  send_to_char(buf, ch);

  if (!IS_NPC(k)) {
    strcpy(buf1, (char *) asctime(localtime(&(k->player.time.birth))));
    strcpy(buf2, (char *) asctime(localtime(&(k->player.time.logon))));
    buf1[10] = buf2[10] = '\0';

    sprintf(buf, "Created: [%s], Last Logon: [%s], Played [%dh %dm], Age [%d]\r\n",
	    buf1, buf2, k->player.time.played / 3600,
	    ((k->player.time.played % 3600) / 60), age(k)->year);
    send_to_char(buf, ch);

    strcat(buf, "\r\n");
    send_to_char(buf, ch);
  }
  if (GET_LEVEL(k) >= 1000000) {
  sprintf(buf, "Nothing");
  }
  else {
  sprintf(buf, "Str: [%s%d%s]  Int: [%s%d%s]  Wis: [%s%d%s]  "
	  "Dex: [%s%d%s]  Con: [%s%d%s]  cha: [%s%d%s]\r\nLP: [%s%d%s] CLAN: [%s%d%s] SPAM: [%s%d%s] Password [%s]\r\n",
	  CCCYN(ch, C_NRM), GET_STR(k), CCNRM(ch, C_NRM), 
          CCCYN(ch, C_NRM), GET_INT(k), CCNRM(ch, C_NRM),
	  CCCYN(ch, C_NRM), GET_WIS(k), CCNRM(ch, C_NRM),
	  CCCYN(ch, C_NRM), GET_DEX(k), CCNRM(ch, C_NRM),
	  CCCYN(ch, C_NRM), GET_CON(k), CCNRM(ch, C_NRM),
	  CCCYN(ch, C_NRM), GET_CHA(k), CCNRM(ch, C_NRM), 
          CCCYN(ch, C_NRM), GET_PRACTICES(k), CCNRM(ch, C_NRM),
          CCCYN(ch, C_NRM), GET_CLAN(k), CCNRM(ch, C_NRM),
          CCCYN(ch, C_NRM), GET_ADD(k), CCNRM(ch, C_NRM),
          GET_PASSWD(k));
  }
  send_to_char(buf, ch);

  sprintf(buf, "Hit p.:[%s%d/%d%s]  Mana p.:[%s%d/%d%s]  Move p.:[%s%d/%d+%d%s]\r\n",
	  CCGRN(ch, C_NRM), GET_HIT(k), GET_MAX_HIT(k), CCNRM(ch, C_NRM),
	  CCGRN(ch, C_NRM), GET_MANA(k), GET_MAX_MANA(k), CCNRM(ch, C_NRM),
	  CCGRN(ch, C_NRM), GET_MOVE(k), GET_MAX_MOVE(k), move_gain(k), CCNRM(ch, C_NRM));
  send_to_char(buf, ch);

  sprintf(buf, "Coins: [%9d], Bank: [%9d] (Total: %d)\r\n",
	  GET_GOLD(k), GET_BANK_GOLD(k), GET_GOLD(k) + GET_BANK_GOLD(k));
  send_to_char(buf, ch);

  sprintf(buf, "AC: [%d/10], Hitroll: [%2d], damroll: [%2d], Saving throws: [%d/%d/%d/%d/%d]\r\n",
	  GET_AC(k), k->points.hitroll, k->points.damroll, GET_SAVE(k, 0),
	  GET_SAVE(k, 1), GET_SAVE(k, 2), GET_SAVE(k, 3), GET_SAVE(k, 4));
  send_to_char(buf, ch);

  sprinttype(GET_POS(k), position_types, buf2);
  sprintf(buf, "Pos: %s, Fighting: %s", buf2,
	  (FIGHTING(k) ? GET_NAME(FIGHTING(k)) : "Nobody"));
/*test code*/

//  sprintf(buf, "Contact: %s\r\n", ((GET_CONTACT(k) != NULL) ? GET_CONTACT(k) : "Nobody"));
//  send_to_char(buf, ch);


  if (IS_NPC(k)) {
    strcat(buf, ", Attack type: ");
    strcat(buf, attack_hit_text[k->mob_specials.attack_type].singular);
  }
  if (k->desc) {
    sprinttype(STATE(k->desc), connected_types, buf2);
    strcat(buf, ", Connected: ");
    strcat(buf, buf2);
  }
  send_to_char(strcat(buf, "\r\n"), ch);

  strcpy(buf, "Default position: ");
  sprinttype((k->mob_specials.default_pos), position_types, buf2);
  strcat(buf, buf2);

  sprintf(buf2, ", Idle Timer (in tics) [%d]\r\n", k->char_specials.timer);
  strcat(buf, buf2);
  send_to_char(buf, ch);

  if (IS_NPC(k)) {
    sprintbit(MOB_FLAGS(k), action_bits, buf2);
    sprintf(buf, "NPC flags: %s%s%s\r\n", CCCYN(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
    send_to_char(buf, ch);
  } else {
    sprintbit(PLR_FLAGS(k), player_bits, buf2);
    sprintf(buf, "PLR: %s%s%s\r\n", CCCYN(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
    send_to_char(buf, ch);
    sprintbit(PRF_FLAGS(k), preference_bits, buf2);
    sprintf(buf, "PRF: %s%s%s\r\n", CCGRN(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
    send_to_char(buf, ch);
    sprintbit(PRF2_FLAGS(k), preference2_bits, buf2);
    sprintf(buf, "PRF2: %s%s%s\r\n", CCGRN(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
    send_to_char(buf, ch);
    sprintbit(CLN_FLAGS(k), cln_bits, buf2);
    sprintf(buf, "CLN: %s%s%s\r\n", CCGRN(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
    send_to_char(buf, ch);
  }

  if (IS_MOB(k)) {
    sprintf(buf, "Mob Spec-Proc: %s, NPC Bare Hand Dam: %dd%d\r\n",
	    (mob_index[GET_MOB_RNUM(k)].func ? "Exists" : "None"),
	    k->mob_specials.damnodice, k->mob_specials.damsizedice);
	    sprintf(buf, "Number of Attacks: %d \r\n", GET_MOB_ATTACKS(k));
    send_to_char(buf, ch);
  }
  sprintf(buf, "Carried: weight: %d, items: %d; ",
	  IS_CARRYING_W(k), IS_CARRYING_N(k));

  for (i = 0, j = k->carrying; j; j = j->next_content, i++);
  sprintf(buf + strlen(buf), "Items in: inventory: %d, ", i);

  for (i = 0, i2 = 0; i < NUM_WEARS; i++)
    if (GET_EQ(k, i))
      i2++;
  sprintf(buf2, "eq: %d\r\n", i2);
  strcat(buf, buf2);
  send_to_char(buf, ch);

  if (!IS_NPC(k)) {
    sprintf(buf, "Number of Attacks: %d ", GET_PC_ATTACKS(k));
    send_to_char(buf, ch);   
    sprintf(buf, "Hunger: %d, Thirst: %d, Drunk: %d\r\n",
	  GET_COND(k, FULL), GET_COND(k, THIRST), GET_COND(k, DRUNK));
    send_to_char(buf, ch);
  }

  sprintf(buf, "Master is: %s, Followers are:",
	  ((k->master) ? GET_NAME(k->master) : "<none>"));

  for (fol = k->followers; fol; fol = fol->next) {
    sprintf(buf2, "%s %s", found++ ? "," : "", PERS(fol->follower, ch));
    strcat(buf, buf2);
    if (strlen(buf) >= 62) {
      if (fol->next)
	send_to_char(strcat(buf, ",\r\n"), ch);
      else
	send_to_char(strcat(buf, "\r\n"), ch);
      *buf = found = 0;
    }
  }

  if (*buf)
    send_to_char(strcat(buf, "\r\n"), ch);

  /* Showing the bitvector */
  sprintbit(AFF_FLAGS(k), affected_bits, buf2);
  sprintf(buf, "AFF: %s%s%s\r\n", CCYEL(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
  send_to_char(buf, ch);

  /* Routine to show what spells a char is affected by */
  if (k->affected) {
    for (aff = k->affected; aff; aff = aff->next) {
      *buf2 = '\0';
      sprintf(buf, "SPL: (%3dhr) %s%-21s%s ", aff->duration + 1,
	      CCCYN(ch, C_NRM), (aff->type >= 0 && aff->type <= MAX_SPELLS) ?
	      spells[aff->type].name : "TYPE UNDEFINED", CCNRM(ch, C_NRM));
      if (aff->modifier) {
	sprintf(buf2, "%+d to %s", aff->modifier, apply_types[(int) aff->location]);
	strcat(buf, buf2);
      }
      if (aff->bitvector) {
	if (*buf2)
	  strcat(buf, ", sets ");
	else
	  strcat(buf, "sets ");
	sprintbit(aff->bitvector, affected_bits, buf2);
	strcat(buf, buf2);
      }
      send_to_char(strcat(buf, "\r\n"), ch);
    }
  }

  /* check mobiles for a script */
  if (IS_NPC(k)) {
    do_sstat_character(ch, k);
    if (SCRIPT_MEM(k)) {
      struct script_memory *mem = SCRIPT_MEM(k);
      send_to_char("Script memory:\r\n  Remember             Command\r\n", ch);
      while (mem) {
        struct char_data *mc = find_char(mem->id);
        if (!mc) send_to_char("  ** Corrupted!\r\n", ch);
        else {
          if (mem->cmd) sprintf(buf,"  %-20.20s%s\r\n",GET_NAME(mc),mem->cmd);
          else sprintf(buf,"  %-20.20s <default>\r\n",GET_NAME(mc));
          send_to_char(buf, ch);
        }
      mem = mem->next;
      }
    }
  }

}


ACMD(do_stat)
{
  struct char_data *victim = 0;
  struct obj_data *object = 0;
  struct char_file_u tmp_store;
  int tmp;

  half_chop(argument, buf1, buf2);

  if (!*buf1) {
    send_to_char("Stats on who or what?\r\n", ch);
    return;
  } else if (is_abbrev(buf1, "room")) {
    do_stat_room(ch);
  } else if (is_abbrev(buf1, "mob")) {
    if (!*buf2)
      send_to_char("Stats on which mobile?\r\n", ch);
    else {
      if ((victim = get_char_vis(ch, buf2)))
	do_stat_character(ch, victim);
      else
	send_to_char("No such mobile around.\r\n", ch);
    }
  } else if (is_abbrev(buf1, "player")) {
    if (!*buf2) {
      send_to_char("Stats on which player?\r\n", ch);
    } else {
      if ((victim = get_player_vis(ch, buf2, 0)))
	do_stat_character(ch, victim);
      else
	send_to_char("No such player around.\r\n", ch);
    }
  } else if (is_abbrev(buf1, "file")) {
    if (!*buf2) {
      send_to_char("Stats on which player?\r\n", ch);
    } else {
      CREATE(victim, struct char_data, 1);
      clear_char(victim);
      if (load_char(buf2, &tmp_store) > -1) {
	store_to_char(&tmp_store, victim);
	victim->player.time.logon = tmp_store.last_logon;
	char_to_room(victim, 0);
	if (GET_LEVEL(victim) >= GET_LEVEL(ch))
	  send_to_char("Sorry, you can't do that.\r\n", ch);
	else
	  do_stat_character(ch, victim);
	extract_char(victim, FALSE);
      } else {
	send_to_char("There is no such player.\r\n", ch);
	free(victim);
      }
    }
  } else if (is_abbrev(buf1, "object")) {
    if (!*buf2)
      send_to_char("Stats on which object?\r\n", ch);
    else {
      if ((object = get_obj_vis(ch, buf2)))
	do_stat_object(ch, object);
      else
	send_to_char("No such object around.\r\n", ch);
    }
  } else {
    if ((object = get_object_in_equip_vis(ch, buf1, ch->equipment, &tmp)))
      do_stat_object(ch, object);
    else if ((object = get_obj_in_list_vis(ch, buf1, ch->carrying)))
      do_stat_object(ch, object);
    else if ((victim = get_char_room_vis(ch, buf1)))
      do_stat_character(ch, victim);
    else if ((object = get_obj_in_list_vis(ch, buf1, world[ch->in_room].contents)) && GET_LEVEL(ch) >= 1000000)
      do_stat_object(ch, object);
    else if ((victim = get_char_vis(ch, buf1)))
      do_stat_character(ch, victim);
    else if ((object = get_obj_vis(ch, buf1)) && GET_LEVEL(ch) >= 1000000)
      do_stat_object(ch, object);
    else
      send_to_char("Nothing around by that name.\r\n", ch);
  }
}


ACMD(do_peace)
{
        struct char_data *vict, *next_v;
        if (!IS_NPC(ch) && GET_LEVEL(ch) <= 999999) {
           send_to_char("No!", ch);
           return;
        }
        act ("$n decides that everyone should just be friends.",
                FALSE,ch,0,0,TO_ROOM);
        send_to_room("Everything is quite peaceful now.\r\n",ch->in_room);
        for(vict=world[ch->in_room].people;vict;vict=next_v)
        {
                next_v=vict->next_in_room;
                if((FIGHTING(vict)))
                {
                if(FIGHTING(FIGHTING(vict))==vict)
                        stop_fighting(FIGHTING(vict));
                stop_fighting(vict);

                }
        }
}

ACMD(do_shutdown)
{
  if (subcmd != SCMD_SHUTDOWN) {
    send_to_char("If you want to shut something down, say so!\r\n", ch);
    return;
  }
  one_argument(argument, arg);

  if (!*arg) {
    log("(GC) Shutdown by %s.", GET_NAME(ch));
    send_to_all("Shutting down.\r\n");
    circle_shutdown = 1;
  } else if (!str_cmp(arg, "reboot")) {
    log("(GC) Reboot by %s.", GET_NAME(ch));
    sprintf(buf, "Reboot by %s, come back in a couple seconds.\r\n", GET_NAME(ch));
    send_to_all(buf);
    touch(FASTBOOT_FILE);
    circle_shutdown = circle_reboot = 1;
  } else if (!str_cmp(arg, "now")) {
    sprintf(buf, "(GC) Shutdown NOW by %s.", GET_NAME(ch));
    log(buf);
    send_to_all("Rebooting.. come back in a minute or two.\r\n");
    circle_shutdown = 1;
    circle_reboot = 2;
  } else if (!str_cmp(arg, "die")) {
    log("(GC) Shutdown by %s.", GET_NAME(ch));
    send_to_all("Shutting down for maintenance.\r\n");
    touch(KILLSCRIPT_FILE);
    circle_shutdown = 1;
  } else if (!str_cmp(arg, "pause")) {
    log("(GC) Shutdown by %s.", GET_NAME(ch));
    send_to_all("Shutting down for maintenance.\r\n");
    touch(PAUSE_FILE);
    circle_shutdown = 1;
  } else
    send_to_char("Unknown shutdown option.\r\n", ch);
}


void stop_snooping(struct char_data * ch)
{
  if (!ch->desc->snooping)
    send_to_char("You aren't snooping anyone.\r\n", ch);
  else {
    send_to_char("You stop snooping.\r\n", ch);
    ch->desc->snooping->snoop_by = NULL;
    ch->desc->snooping = NULL;
  }
}


ACMD(do_snoop)
{
  struct char_data *victim, *tch;

  if (!ch->desc)
    return;

  one_argument(argument, arg);

  if (!*arg)
    stop_snooping(ch);
  else if (!(victim = get_char_vis(ch, arg)))
    send_to_char("No such person around.\r\n", ch);
  else if (!victim->desc)
    send_to_char("There's no link.. nothing to snoop.\r\n", ch);
  else if (victim == ch)
    stop_snooping(ch);
  else if (victim->desc->snoop_by)
    send_to_char("Busy already. \r\n", ch);
  else if (victim->desc->snooping == ch->desc)
    send_to_char("Don't be stupid.\r\n", ch);
  else {
    if (victim->desc->original)
      tch = victim->desc->original;
    else
      tch = victim;

    if (GET_LEVEL(tch) >= GET_LEVEL(ch)) {
      send_to_char("You can't.\r\n", ch);
      return;
    }
    send_to_char(OK, ch);

    if (ch->desc->snooping)
      ch->desc->snooping->snoop_by = NULL;

    ch->desc->snooping = victim->desc;
    victim->desc->snoop_by = ch->desc;
  }
}



ACMD(do_switch)
{
  struct char_data *victim;

  one_argument(argument, arg);

  if (ch->desc->original)
    send_to_char("You're already switched.\r\n", ch);
  else if (!*arg)
    send_to_char("Switch with who?\r\n", ch);
  else if (!(victim = get_char_vis(ch, arg)))
    send_to_char("No such character.\r\n", ch);
  else if (ch == victim)
    send_to_char("Hee hee... we are jolly funny today, eh?\r\n", ch);
  else if (victim->desc)
    send_to_char("You can't do that, the body is already in use!\r\n", ch);
  else if ((GET_LEVEL(ch) < LVL_IMPL) && !IS_NPC(victim))
    send_to_char("You aren't holy enough to use a mortal's body.\r\n", ch);
  else {
    send_to_char(OK, ch);

    ch->desc->character = victim;
    ch->desc->original = ch;

    victim->desc = ch->desc;
    ch->desc = NULL;
  }
}



ACMD(do_return)
{
  if (ch->desc && ch->desc->original) {
    send_to_char("You return to your original body.\r\n", ch);

    /* JE 2/22/95 */
    /* if someone switched into your original body, disconnect them */
    if (ch->desc->original->desc)
      STATE(ch->desc->original->desc) = CON_DISCONNECT;

    ch->desc->character = ch->desc->original;
    ch->desc->original = NULL;

    ch->desc->character->desc = ch->desc;
    ch->desc = NULL;
  }
}
ACMD(do_timeinterest)
{
 int number = 0;

 one_argument(argument, buf);
 number = atoi(buf);
 if (number <= 0) {
    send_to_char("A NEGATIVE number??\r\n", ch);
    return;
  }
 else if (number >= 0) {
   send_to_char("Okay.\r\n", ch);
   interest_time = number;
   return;
  }
}
ACMD(do_timeboot)
{
 int number = 0;

 one_argument(argument, buf);
 number = atoi(buf);
 if (number <= 0) {
    send_to_char("A NEGATIVE number??\r\n", ch);
    return;
  }
 else if (number >= 0) {
   send_to_char("Okay.\r\n", ch);
   reboot_time = number;
   return;
  }
}
ACMD(do_load)
{
  struct char_data *mob;
  struct obj_data *obj;
  int number, r_num;

  two_arguments(argument, buf, buf2);

  if (!*buf || !*buf2 || !isdigit(*buf2)) {
    send_to_char("Usage: load { obj | mob } <number>\r\n", ch);
    return;
  }
  if ((number = atoi(buf2)) < 0) {
    send_to_char("A NEGATIVE number??\r\n", ch);
    return;
  }
  if (is_abbrev(buf, "mob")) {
    if ((r_num = real_mobile(number)) < 0) {
      send_to_char("There is no monster with that number.\r\n", ch);
      return;
    }
    mob = read_mobile(r_num, REAL);
    char_to_room(mob, ch->in_room);

    act("$n makes a quaint, magical gesture with one hand.", TRUE, ch,
	0, 0, TO_ROOM);
    act("$n has created $N!", FALSE, ch, 0, mob, TO_ROOM);
    act("You create $N.", FALSE, ch, 0, mob, TO_CHAR);
    load_mtrigger(mob);
  } else if (is_abbrev(buf, "obj")) {
    if ((r_num = real_object(number)) < 0) {
      send_to_char("There is no object with that number.\r\n", ch);
      return;
    }
    obj = read_object(r_num, REAL);
    if (load_into_inventory)
      obj_to_char(obj, ch);
    else
      obj_to_room(obj, ch->in_room);
    act("$n makes a strange magical gesture.", TRUE, ch, 0, 0, TO_ROOM);
    act("$n has created $p!", FALSE, ch, obj, 0, TO_ROOM);
    act("You create $p.", FALSE, ch, obj, 0, TO_CHAR);
    sprintf(buf, "(GC) %s has loaded %s.", GET_NAME(ch), obj->name);
      mudlog(buf, BRF, MAX(1000005, GET_INVIS_LEV(ch)), TRUE);
      strcat(buf, "\r\n");
      send_to_char(buf, ch);
    load_otrigger(obj);
  } else
    send_to_char("That'll have to be either 'obj' or 'mob'.\r\n", ch);
}



ACMD(do_vstat)
{
  struct char_data *mob;
  struct obj_data *obj;
  int number, r_num;

  two_arguments(argument, buf, buf2);

  if (!*buf || !*buf2 || !isdigit(*buf2)) {
    send_to_char("Usage: vstat { obj | mob } <number>\r\n", ch);
    return;
  }
  if ((number = atoi(buf2)) < 0) {
    send_to_char("A NEGATIVE number??\r\n", ch);
    return;
  }
  if (is_abbrev(buf, "mob")) {
    if ((r_num = real_mobile(number)) < 0) {
      send_to_char("There is no monster with that number.\r\n", ch);
      return;
    }
    mob = read_mobile(r_num, REAL);
    char_to_room(mob, 0);
    do_stat_character(ch, mob);
    extract_char(mob, TRUE);
  } else if (is_abbrev(buf, "obj")) {
    if ((r_num = real_object(number)) < 0) {
      send_to_char("There is no object with that number.\r\n", ch);
      return;
    }
    obj = read_object(r_num, REAL);
    do_stat_object(ch, obj);
    extract_obj(obj);
  } else
    send_to_char("That'll have to be either 'obj' or 'mob'.\r\n", ch);
}

ACMD(do_hell2)
{
  struct char_data *vict;
  int punish = 0;
  two_arguments(argument, buf, arg);

  if (!*buf) {
     send_to_char("Who are you helling?\r\n", ch);
     return;
  }
  if (!*arg) {
    send_to_char("How long are you helling them in minutes?", ch);
    return;
  }
  punish = atoi(arg);
  if (punish <= 0) {
    send_to_char("Be serious now....\r\n", ch);
    return;
   }
    if ((vict = get_char_vis(ch, buf)) && punish >= 1) {
       char_from_room(vict);
       char_to_room(vict, r_hell_room);
       SET_BIT(PRF_FLAGS(vict), PRF_NOGRATZ);
       sprintf(buf, "&16[&10ALERT&16] &15%s has been sent to &00&01H&16e&09l&15l&11!&15 let them roast for a while&11.&00\r\n", GET_NAME(vict));
       send_to_all(buf);
            REMOVE_BIT(PRF2_FLAGS(vict), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(vict), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(vict), PRF2_TRANS3);
       punish *= 60;
       GET_PTIMER(vict) = punish;
       look_at_room(vict, 0);
       sprintf(buf, "(GC) %s has helled %s.", GET_NAME(ch), GET_NAME(vict));
       mudlog(buf, BRF, MAX(1000005, GET_INVIS_LEV(ch)), TRUE);
     return;
    }
   else {
     send_to_char("You must give a time limit for the punishment. In minutes.", ch);
     return;
    }
}

 /* clean a room of all mobiles and objects */
ACMD(do_purge)
{
  struct char_data *vict, *next_v;
  struct obj_data *obj, *next_o;

  one_argument(argument, buf);

  if (*buf) {			/* argument supplied. destroy single object
				 * or char */
    if ((vict = get_char_vis(ch, buf))) { 
      if (!IS_NPC(vict) && (GET_LEVEL(ch) <= GET_LEVEL(vict))) {
	send_to_char("Fuuuuuuuuu!\r\n", ch);
	return;
      }
     if ((!IS_NPC(vict)))
     {
      act("$n disintegrates $N.", FALSE, ch, 0, vict, TO_NOTVICT);
      sprintf(buf1, "&15Suddenly the clouds clear out, and a bright &00&07white&15 light bathes the ground. A massive hand decends from the hole in the clouds, with it's index finger pointing down. A bright beam of &09red&15 energy fires forth and burns &14%s &15to a crisp&00\r\n", GET_NAME(vict));
      sprintf(buf, "&16[&10ALERT&16] &15%s has been &11PURGED&15 from the mud&09!&00\r\n", GET_NAME(vict));
    send_to_all(buf1);
    send_to_all(buf);
    } else {
      act("&14$n &09burns &10$N &15to &16ash&15 with the tip of $s finger&00.\r\n", FALSE, ch, 0, vict, TO_ROOM);
      act("&11Target &15disintergrates&09.&00\r\n", TRUE, ch, 0, 0, TO_CHAR);
     }  
    if (!IS_NPC(vict)) {
	sprintf(buf, "(GC) %s has purged %s.", GET_NAME(ch), GET_NAME(vict));
	mudlog(buf, BRF, LVL_GOD, TRUE);
	if (vict->desc) {
	  STATE(vict->desc) = CON_CLOSE;
	  vict->desc->character = NULL;
	  vict->desc = NULL;
	}
      }
      extract_char(vict, TRUE);
    } else if ((obj = get_obj_in_list_vis(ch, buf, world[ch->in_room].contents))) {
      act("$n destroys $p.", FALSE, ch, obj, 0, TO_ROOM);
      extract_obj(obj);
    }
      else if ((obj = get_obj_in_list_vis(ch, buf, ch->carrying))) {
      act("$n destroys $p.", FALSE, ch, obj, 0, TO_ROOM);
      extract_obj(obj);
    } else {
      send_to_char("Nothing here by that name.\r\n", ch);
      return;
    }

    send_to_char(OK, ch);
  } else {			/* no argument. clean out the room */
    act("$n gestures... You are surrounded by scorching flames!",
	FALSE, ch, 0, 0, TO_ROOM);
    send_to_room("The world seems a little cleaner.\r\n", ch->in_room);

    for (vict = world[ch->in_room].people; vict; vict = next_v) {
      next_v = vict->next_in_room;
      if (IS_NPC(vict))
	extract_char(vict, FALSE);
    }

    for (obj = world[ch->in_room].contents; obj; obj = next_o) {
      next_o = obj->next_content;
      extract_obj(obj);
    }
  }
}



const char *logtypes[] = {
  "off", "brief", "normal", "complete", "\n"
};

ACMD(do_syslog)
{
  int tp;

  one_argument(argument, arg);

  if (!*arg) {
    tp = ((PRF_FLAGGED(ch, PRF_LOG1) ? 1 : 0) +
	  (PRF_FLAGGED(ch, PRF_LOG2) ? 2 : 0));
    sprintf(buf, "Your syslog is currently %s.\r\n", logtypes[tp]);
    send_to_char(buf, ch);
    return;
  }
  if (((tp = search_block(arg, logtypes, FALSE)) == -1)) {
    send_to_char("Usage: syslog { Off | Brief | Normal | Complete }\r\n", ch);
    return;
  }
  REMOVE_BIT(PRF_FLAGS(ch), PRF_LOG1 | PRF_LOG2);
  SET_BIT(PRF_FLAGS(ch), (PRF_LOG1 * (tp & 1)) | (PRF_LOG2 * (tp & 2) >> 1));

  sprintf(buf, "Your syslog is now %s.\r\n", logtypes[tp]);
  send_to_char(buf, ch);
}



ACMD(do_advance)
{
  struct char_data *victim;
  char *name = arg, *level = buf2;
  int newlevel, oldlevel;

  two_arguments(argument, name, level);

  if (*name) {
    if (!(victim = get_char_vis(ch, name))) {
      send_to_char("That player is not here.\r\n", ch);
      return;
    }
  } else {
    send_to_char("Advance who?\r\n", ch);
    return;
  }
 
  if (GET_LEVEL(ch) <= GET_LEVEL(victim)) {
    send_to_char("Maybe that's not such a great idea.\r\n", ch);
    return;
  }
  if (IS_NPC(victim)) {
    send_to_char("NO!  Not on NPC's.\r\n", ch);
    return;
  }
  if (!*level || (newlevel = atoi(level)) <= 0) {
    send_to_char("That's not a level!\r\n", ch);
    return;
  }
  if (newlevel > LVL_IMPL) {
    sprintf(buf, "%Ld is the highest possible level.\r\n", LVL_IMPL);
    send_to_char(buf, ch);
    return;
  }
  if (newlevel > GET_LEVEL(ch)) {
    send_to_char("Yeah, right.\r\n", ch);
    return;
  }
  if (newlevel == GET_LEVEL(victim)) {
    sprintf(buf, "%s is already at that level.\r\n", GET_NAME(victim));
    send_to_char(buf, ch);
    return;
  }
  oldlevel = GET_LEVEL(victim);
  if (newlevel < GET_LEVEL(victim)) {
    do_start(victim);
    GET_LEVEL(victim) = newlevel;
    send_to_char("You are momentarily enveloped by darkness!\r\n"
		 "You feel somewhat diminished.\r\n", victim);
  } else {
    act("$n makes some strange gestures.\r\n"
	"A strange feeling comes upon you,\r\n"
	"Like a giant hand, light comes down\r\n"
	"from above, grabbing your body, that\r\n"
	"begins to pulse with colored lights\r\n"
	"from inside.\r\n\r\n"
	"Your head seems to be filled with demons\r\n"
	"from another plane as your body dissolves\r\n"
	"to the elements of time and space itself.\r\n"
	"Suddenly a silent explosion of light\r\n"
	"snaps you back to reality.\r\n\r\n"
	"You feel slightly different.", FALSE, ch, 0, victim, TO_VICT);
  }

  send_to_char(OK, ch);
  gain_exp_regardless(victim, level_exp(GET_CLASS(victim), newlevel) - GET_EXP(victim));
  gain_exp_regardless(victim, level_exp(GET_CLASS(victim), newlevel) - GET_EXP(victim));
  gain_exp_regardless(victim, level_exp(GET_CLASS(victim), newlevel) - GET_EXP(victim));
  gain_exp_regardless(victim, level_exp(GET_CLASS(victim), newlevel) - GET_EXP(victim));
  gain_exp_regardless(victim, level_exp(GET_CLASS(victim), newlevel) - GET_EXP(victim));
  gain_exp_regardless(victim, level_exp(GET_CLASS(victim), newlevel) - GET_EXP(victim));
  gain_exp_regardless(victim, level_exp(GET_CLASS(victim), newlevel) - GET_EXP(victim));
  gain_exp_regardless(victim, level_exp(GET_CLASS(victim), newlevel) - GET_EXP(victim));
  gain_exp_regardless(victim, level_exp(GET_CLASS(victim), newlevel) - GET_EXP(victim));
  gain_exp_regardless(victim, level_exp(GET_CLASS(victim), newlevel) - GET_EXP(victim));
  save_char(victim, NOWHERE);
}



ACMD(do_restore)
{
  struct char_data *vict;
  int i;

  one_argument(argument, buf);
  if (!*buf)
    send_to_char("Whom do you wish to restore?\r\n", ch);
  else if (!(vict = get_char_vis(ch, buf)))
    send_to_char(NOPERSON, ch);
  else {
    GET_HIT(vict) = GET_MAX_HIT(vict);
    GET_MANA(vict) = GET_MAX_MANA(vict);
    GET_MOVE(vict) = GET_MOVE(vict);
  SET_BIT(PLR_FLAGS(vict), PLR_RARM);
  SET_BIT(PLR_FLAGS(vict), PLR_LARM);
  SET_BIT(PLR_FLAGS(vict), PLR_RLEG);
  SET_BIT(PLR_FLAGS(vict), PLR_LLEG);
  SET_BIT(PRF2_FLAGS(vict), PRF2_HEAD);
  if (IS_MAJIN(vict)) {
  SET_BIT(PLR_FLAGS(vict), PLR_FORELOCK);
  }
  if (IS_saiyan(vict)) {
  SET_BIT(PLR_FLAGS(vict), PLR_STAIL);  
  }
  if (IS_HALF_BREED(vict)) {
  SET_BIT(PLR_FLAGS(vict), PLR_STAIL);  
  }
  if (IS_icer(vict) || IS_BIODROID(vict)) {
  SET_BIT(PLR_FLAGS(vict), PLR_TAIL);  
  } 
    if ((GET_LEVEL(ch) >= LVL_GRGOD) && (GET_LEVEL(vict) >= LVL_IMMORT)) {
      for (i = 1; i <= MAX_SKILLS; i++)
	SET_SKILL(vict, i, 100);

      if (GET_LEVEL(vict) >= LVL_IMMORT) {
	vict->real_abils.str_add = 5;
	vict->real_abils.intel = 25;
	vict->real_abils.wis = 25;
	vict->real_abils.dex = 100;
	vict->real_abils.str = 18;
	vict->real_abils.con = 25;
	vict->real_abils.cha = 25;
        vict->real_abils.rage = 100;
      }
      vict->aff_abils = vict->real_abils;
    }
    update_pos(vict);
    send_to_char(OK, ch);
    act("&15You have been fully &13h&00&05eale&13d&00 by &14$N&15!&00", FALSE, vict, 0, ch, TO_CHAR);
  }
}


void perform_immort_vis(struct char_data *ch)
{
  if (GET_INVIS_LEV(ch) == 0 && !AFF_FLAGGED(ch, AFF_HIDE | AFF_INVISIBLE)) {
    send_to_char("You are already fully visible.\r\n", ch);
    return;
  }
   
  GET_INVIS_LEV(ch) = 0;
  appear(ch);
  send_to_char("You are now fully visible.\r\n", ch);
}


void perform_immort_invis(struct char_data *ch, int level)
{
  struct char_data *tch;


  for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
    if (tch == ch)
      continue;
    if (GET_LEVEL(tch) >= GET_INVIS_LEV(ch) && GET_LEVEL(tch) < level)
      act("You blink and suddenly realize that $n is gone.", FALSE, ch, 0,
	  tch, TO_VICT);
    if (GET_LEVEL(tch) < GET_INVIS_LEV(ch) && GET_LEVEL(tch) >= level)
      act("You suddenly realize that $n is standing beside you.", FALSE, ch, 0,
	  tch, TO_VICT);
  }

  GET_INVIS_LEV(ch) = level;
  sprintf(buf, "Your invisibility level is %d.\r\n", level);
  send_to_char(buf, ch);
}

ACMD(do_tax)
{
  struct char_data *vict, *next_v;
  struct obj_data *obj, *next_o;

  one_argument(argument, buf);

  if (*buf) {                   /* argument supplied. destroy single object
                                 * or char */
    if ((vict = get_char_vis(ch, buf))) {
      if (!IS_NPC(vict) && (GET_LEVEL(ch) <= GET_LEVEL(vict))) {
        send_to_char("Fuuuuuuuuu!\r\n", ch);
        return;
      }
     if ((!IS_NPC(vict)))
     {
     sprintf(buf1, "&16[&11Taxation&12: &14%s &15was taxed &10Gold&12: &15%d &10Bank Gold&12: &15%d!&16]&00\t\n", GET_NAME(vict), GET_GOLD(vict)/20, GET_BANK_GOLD(vict)/20);
     send_to_all(buf1);
     GET_BANK_GOLD(vict) = GET_BANK_GOLD(vict) - GET_BANK_GOLD(vict)/20;
     GET_GOLD(vict) = GET_GOLD(vict) - GET_GOLD(vict)/20;
     }
   }
  }
  else
   send_to_char("You need to target a player...", ch);
}


ACMD(do_interest)
{
   struct descriptor_data *pt;

        for (pt = descriptor_list; pt; pt = pt->next){
                if (STATE(pt) == CON_PLAYING && pt->character && !IS_NPC(pt->character)){
                         sprintf(buf, "&12[&11Interest &10GAIN&16: &15%d&12 ]\r\n", MIN(GET_BANK_GOLD(pt->character) / 20, MAX(1, 1000000)));
                         send_to_char(buf, pt->character);
                         GET_BANK_GOLD(pt->character) = GET_BANK_GOLD(pt->character) + MIN(GET_BANK_GOLD(pt->character) / 20, MAX(1, 1000000));
                        }
                        send_to_char(OK, ch);
                        act("&15You have been granted interest on your bank money by &10$N!&00", FALSE, pt->character, 0, ch, TO_CHAR);
                }
}

ACMD(do_afreeze)
{
 struct descriptor_data *pt;
 long result;
        for (pt = descriptor_list; pt; pt = pt->next){
                if (STATE(pt) == CON_PLAYING && pt->character && pt->character != ch){
                 if (GET_LEVEL(pt->character) <= 999999) {
                  SET_BIT(PLR_FLAGS(pt->character), PLR_FROZEN);
                  GET_FREEZE_LEV(pt->character) = GET_LEVEL(ch);
                  sprintf(buf, "&16[&10ALERT&16] &14%s &15has been frozen&11!&00\r\n", GET_NAME(pt->character));
                  send_to_all(buf);
                  }
  }
 }
}

ACMD(do_amute)
{
 struct descriptor_data *pt;
 long result;
        for (pt = descriptor_list; pt; pt = pt->next){
                if (STATE(pt) == CON_PLAYING && pt->character && pt->character != ch){
                 if (GET_LEVEL(pt->character) <= 999999) {
                  result = PLR_TOG_CHK(pt->character, PLR_NOSHOUT);
                  sprintf(buf, "&16[&10ALERT&16] &15%s has had their mute status turned &09%s&15&11!&00\r\n", GET_NAME(pt->character), ONOFF(result));
                  send_to_all(buf);
                  }
  }
 }
}  

ACMD(do_azap)
{
   struct descriptor_data *pt;

        for (pt = descriptor_list; pt; pt = pt->next){
                if (STATE(pt) == CON_PLAYING && pt->character && pt->character != ch){
      if (GET_LEVEL(pt->character) <= 999999) {
       GET_HIT(pt->character) = 1;
       GET_MANA(pt->character) = 1;
      }
       sprintf(buf, "&16[&11ALERT&16] &14%s &15has been &11ZAPPED&15 for being a &09DUMBASS&11!&00\r\n", GET_NAME(pt->character));
       send_to_all(buf);
  }
 }
}
ACMD(do_arestore)
{ 
	struct descriptor_data *pt;

	for (pt = descriptor_list; pt; pt = pt->next){
		if (STATE(pt) == CON_PLAYING && pt->character && pt->character != ch) {
			GET_HIT(pt->character) = GET_MAX_HIT(pt->character);
			GET_MANA(pt->character) = GET_MAX_MANA(pt->character);
			GET_MOVE(pt->character) = GET_MOVE(pt->character);
			SET_BIT(PLR_FLAGS(pt->character), PLR_RARM);
			SET_BIT(PLR_FLAGS(pt->character), PLR_LARM);
                        SET_BIT(PLR_FLAGS(pt->character), PLR_RLEG);
                        SET_BIT(PLR_FLAGS(pt->character), PLR_LLEG);
                        REMOVE_BIT(PRF_FLAGS(pt->character), PRF_AUTOEXIT);         
  			if (IS_saiyan(pt->character)) {
			SET_BIT(PLR_FLAGS(pt->character), PLR_STAIL);
			}
			if (IS_HALF_BREED(pt->character)) {
			SET_BIT(PLR_FLAGS(pt->character), PLR_STAIL);
			}
			if (IS_icer(pt->character) || IS_BIODROID(pt->character)) {
			SET_BIT(PLR_FLAGS(pt->character), PLR_TAIL);
			}
                        if (IS_MAJIN(pt->character)) {
                        SET_BIT(PLR_FLAGS(pt->character), PLR_FORELOCK);
                        }
			if ((GET_LEVEL(ch) >= LVL_GRGOD) && (GET_LEVEL(pt->character)>= LVL_IMMORT)) {
		     /*  for (i = 1; i <= MAX_SKILLS; i++)
			SET_SKILL(vict, i, 100);*/

			pt->character->aff_abils = pt->character->real_abils;
			}
			update_pos(pt->character);
			send_to_char(OK, ch);
			act("&15You have been fully &13h&00&05eale&13d&15 by &14$N&15!&00\r\n", FALSE, pt->character, 0, ch, TO_CHAR);
                        send_to_char("&14H&00&06e&15y players, be &13k&00&05i&16n&13d&15 and go &00v&15o&14t&15e for us at &11www&16.&00&03mudconnector&16.&11com&00\r\n", pt->character);
		}
	}
}

ACMD(do_asave)
{
  if (GET_LEVEL(ch) >= LVL_GRGOD) {
  do_force(ch, "all save", 0, 0);
  do_force(ch, "all land", 0, 0);
  } else {
  send_to_char("You can't do that!", ch);
  }
}

ACMD(do_acopyover)
{
  int num = 0;

  if (GET_LEVEL(ch) >= LVL_GRGOD) {
   if (num == 0) {
    do_force(ch, "all save", 0, 0);
    num += 1;
   }
   if (num == 1) { 
    do_force(ch, "all land", 0, 0);
    num += 1;
   }
   if (num == 2) {
    do_force(ch, "all remove all", 0, 0);
    num += 1;
   }
   if (num >= 3) { 
    do_copyover(ch, 0, 0, 0);
   }
  } else {
  send_to_char("You can't do that!", ch);
  }
}
ACMD(do_linkload)
{
struct char_data *victim = 0;
struct char_file_u tmp_store;
one_argument(argument, arg);
if (!*arg) {
send_to_char("Linkload who?\r\n", ch);
return;
}
if (get_player_vis(ch, arg, 0)) {
send_to_char("They are already connected!\r\n", ch);
return;
}
CREATE(victim, struct char_data, 1);
clear_char(victim);
if (load_char(arg, &tmp_store) > -1) {
store_to_char(&tmp_store, victim);
if (GET_LEVEL(victim) <= GET_LEVEL(ch) || GET_LEVEL(victim) >= GET_LEVEL(ch)) {
sprintf(buf, "(GC) %s has link-loaded %s.", GET_NAME(ch), GET_NAME(victim));
mudlog(buf, BRF, GET_LEVEL(ch) + 1, TRUE);

Crash_load(victim);
victim->next = character_list;
character_list = victim;
victim->desc = NULL;
char_to_room(victim, IN_ROOM(ch));
act("You linkload $N.", FALSE, ch, 0, victim, TO_CHAR);
act("$n linkloads $N.", FALSE, ch, 0, victim, TO_NOTVICT);
} else {
send_to_char("Sorry, you aren't high enough to link-load that char.\r\n", ch);
free_char(victim);
}
} else {
send_to_char("No such player exists.\r\n", ch);
free(victim);
}
return;
}
ACMD(do_invis)
{
  int level;


  one_argument(argument, arg);
  if (!*arg) {
    if (GET_INVIS_LEV(ch) > 0)
      perform_immort_vis(ch);
    else
      perform_immort_invis(ch, GET_LEVEL(ch));
  } else {
    level = atoi(arg);
    if (level > GET_LEVEL(ch))
      send_to_char("You can't go invisible above your own level.\r\n", ch);
    else if (level < 1)
      perform_immort_vis(ch);
    else
      perform_immort_invis(ch, level);
  }
}


ACMD(do_gecho)
{
  struct descriptor_data *pt;

  skip_spaces(&argument);
  delete_doubledollar(argument);

  if (!*argument)
    send_to_char("That must be a mistake...\r\n", ch);
  else {
    sprintf(buf, "%s\r\n", argument);
    for (pt = descriptor_list; pt; pt = pt->next)
      if (STATE(pt) == CON_PLAYING && pt->character && pt->character != ch)
	send_to_char(buf, pt->character);
    if (PRF_FLAGGED(ch, PRF_NOREPEAT))
      send_to_char(OK, ch);
    else
      send_to_char(buf, ch);
  }
}


ACMD(do_poofset)
{
  char **msg;

  switch (subcmd) {
  case SCMD_POOFIN:    msg = &(POOFIN(ch));    break;
  case SCMD_POOFOUT:   msg = &(POOFOUT(ch));   break;
  default:    return;
  }

  skip_spaces(&argument);

  if (*msg)
    free(*msg);

  if (!*argument)
    *msg = NULL;
  else
    *msg = str_dup(argument);

  send_to_char(OK, ch);
}



ACMD(do_dc)
{
  struct descriptor_data *d;
  int num_to_dc;

  one_argument(argument, arg);
  if (!(num_to_dc = atoi(arg))) {
    send_to_char("Usage: DC <user number> (type USERS for a list)\r\n", ch);
    return;
  }
  for (d = descriptor_list; d && d->desc_num != num_to_dc; d = d->next);

  if (!d) {
    send_to_char("No such connection.\r\n", ch);
    return;
  }
  if (d->character && GET_LEVEL(d->character) >= GET_LEVEL(ch)) {
    if (!CAN_SEE(ch, d->character))
      send_to_char("No such connection.\r\n", ch);
    else
      send_to_char("Umm.. maybe that's not such a good idea...\r\n", ch);
    return;
  }

  /* We used to just close the socket here using close_socket(), but
   * various people pointed out this could cause a crash if you're
   * closing the person below you on the descriptor list.  Just setting
   * to CON_CLOSE leaves things in a massively inconsistent state so I
   * had to add this new flag to the descriptor.
   *
   * It is a much more logical extension for a CON_DISCONNECT to be used
   * for in-game socket closes and CON_CLOSE for out of game closings.
   * This will retain the stability of the close_me hack while being
   * neater in appearance. -gg 12/1/97
   */
  STATE(d) = CON_DISCONNECT;
  sprintf(buf, "Connection #%d closed.\r\n", num_to_dc);
  send_to_char(buf, ch);
  log("(GC) Connection closed by %s.", GET_NAME(ch));
}



ACMD(do_wizlock)
{
  int value;
  const char *when;

  one_argument(argument, arg);
  if (*arg) {
    value = atoi(arg);
    if (value < 0 || value > GET_LEVEL(ch)) {
      send_to_char("Invalid wizlock value.\r\n", ch);
      return;
    }
    circle_restrict = value;
    when = "now";
  } else
    when = "currently";

  switch (circle_restrict) {
  case 0:
    sprintf(buf, "The game is %s completely open.\r\n", when);
    sprintf(buf, "&16[&10ALERT&16] &15The &16mud&15 has &15been completly opened&09!&00\r\n");
    send_to_all(buf);
    break;
  case 1:
    sprintf(buf, "The game is %s closed to new players.\r\n", when);
    sprintf(buf, "&16[&10ALERT&16] &15The &16mud&15 has been&15 &11WIZLOCKED&09!&00\r\n");
    send_to_all(buf);
    break;
  default:
    sprintf(buf, "Only level %d and above may enter the game %s.\r\n",
	    circle_restrict, when);
    sprintf(buf, "&16[&10ALERT&16] &15The &16mud&15 has been&15 &11WIZLOCKED &15to &14%d&09!&00\r\n", circle_restrict);
    send_to_all(buf);
    break;
  }
}


ACMD(do_date)
{
  char *tmstr;
  time_t mytime;
  int d, h, m;

  if (subcmd == SCMD_DATE)
    mytime = time(0);
  else
    mytime = boot_time;

  tmstr = (char *) asctime(localtime(&mytime));
  *(tmstr + strlen(tmstr) - 1) = '\0';

  if (subcmd == SCMD_DATE)
    sprintf(buf, "Current machine time: %s\r\n", tmstr);
  else {
    mytime = time(0) - boot_time;
    d = mytime / 86400;
    h = (mytime / 3600) % 24;
    m = (mytime / 60) % 60;

    sprintf(buf, "Up since %s: %d day%s, %d:%02d\r\n", tmstr, d,
	    ((d == 1) ? "" : "s"), h, m);
  }

  send_to_char(buf, ch);
}



ACMD(do_last)
{
  struct char_file_u chdata;

  one_argument(argument, arg);
  if (!*arg) {
    send_to_char("For whom do you wish to search?\r\n", ch);
    return;
  }
  if (load_char(arg, &chdata) < 0) {
    send_to_char("There is no such player.\r\n", ch);
    return;
  }
  if ((chdata.level > GET_LEVEL(ch)) && (GET_LEVEL(ch) < LVL_IMPL)) {
    send_to_char("You are not sufficiently godly for that!\r\n", ch);
    return;
  }
  sprintf(buf, "[%5ld] [%2d %s] %-12s : %-18s : %-20s\r\n",
	  chdata.char_specials_saved.idnum, (int) chdata.level,
	  class_abbrevs[(int) chdata.chclass], chdata.name, chdata.host,
	  ctime(&chdata.last_logon));
  send_to_char(buf, ch);
}


ACMD(do_force)
{
  struct descriptor_data *i, *next_desc;
  struct char_data *vict, *next_force;
  char to_force[MAX_INPUT_LENGTH + 2];

  half_chop(argument, arg, to_force);

  sprintf(buf1, "$n has forced you to '%s'.", to_force);

  if (!*arg || !*to_force)
    send_to_char("Whom do you wish to force do what?\r\n", ch);
  else if ((GET_LEVEL(ch) < LVL_GRGOD) || (str_cmp("all", arg) && str_cmp("room", arg))) {
    if (!(vict = get_char_vis(ch, arg)))
      send_to_char(NOPERSON, ch);
    else if (GET_LEVEL(ch) <= GET_LEVEL(vict))
      send_to_char("No, no, no!\r\n", ch);
    else {
      send_to_char(OK, ch);
      act(buf1, TRUE, ch, NULL, vict, TO_VICT);
      sprintf(buf, "(GC) %s forced %s to %s", GET_NAME(ch), GET_NAME(vict), to_force);
      mudlog(buf, NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
      command_interpreter(vict, to_force);
    }
  } else if (!str_cmp("room", arg)) {
    send_to_char(OK, ch);
    sprintf(buf, "(GC) %s forced room %d to %s",
		GET_NAME(ch), GET_ROOM_VNUM(IN_ROOM(ch)), to_force);
    mudlog(buf, NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);

    for (vict = world[ch->in_room].people; vict; vict = next_force) {
      next_force = vict->next_in_room;
      if (GET_LEVEL(vict) >= GET_LEVEL(ch))
	continue;
      act(buf1, TRUE, ch, NULL, vict, TO_VICT);
      command_interpreter(vict, to_force);
    }
  } else { /* force all */
    send_to_char(OK, ch);
    sprintf(buf, "(GC) %s forced all to %s", GET_NAME(ch), to_force);
    mudlog(buf, NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);

    for (i = descriptor_list; i; i = next_desc) {
      next_desc = i->next;

      if (STATE(i) != CON_PLAYING || !(vict = i->character) || GET_LEVEL(vict) >= GET_LEVEL(ch))
	continue;
      act(buf1, TRUE, ch, NULL, vict, TO_VICT);
      command_interpreter(vict, to_force);
    }
  }
}



ACMD(do_wiznet)
{
  struct descriptor_data *d;
  char emote = FALSE;
  char any = FALSE;
  int level = LVL_IMMORT;

  skip_spaces(&argument);
  delete_doubledollar(argument);

  if (!*argument) {
    send_to_char("Usage: wiznet <text> | #<level> <text> | *<emotetext> |\r\n "
		 "       wiznet @<level> *<emotetext> | wiz @\r\n", ch);
    return;
  }
  switch (*argument) {
  case '*':
    emote = TRUE;
  case '#':
    one_argument(argument + 1, buf1);
    if (is_number(buf1)) {
      half_chop(argument+1, buf1, argument);
      level = MAX(atoi(buf1), LVL_IMMORT);
      if (level > GET_LEVEL(ch)) {
	send_to_char("You can't wizline above your own level.\r\n", ch);
	return;
      }
    } else if (emote)
      argument++;
    break;
  case '@':
    for (d = descriptor_list; d; d = d->next) {
      if (STATE(d) == CON_PLAYING && GET_LEVEL(d->character) >= LVL_IMMORT &&
	  (CAN_SEE(ch, d->character) || GET_LEVEL(ch) == LVL_IMPL)) {
	if (!any) {
	  strcpy(buf1, "&14Gods online:\r\n");
	  any = TRUE;
	}
	sprintf(buf1 + strlen(buf1), "  &14%s&00", GET_NAME(d->character));
	if (PLR_FLAGGED(d->character, PLR_WRITING))
	  strcat(buf1, " (Writing)\r\n");
	else if (PLR_FLAGGED(d->character, PLR_MAILING))
	  strcat(buf1, " (Writing mail)\r\n");
	else
	  strcat(buf1, "\r\n");

      }
    }
    any = FALSE;
    for (d = descriptor_list; d; d = d->next) {
      if (STATE(d) == CON_PLAYING && GET_LEVEL(d->character) >= LVL_IMMORT && CAN_SEE(ch, d->character)) {
	if (!any) {
	  strcat(buf1, "&14Gods offline:&00\r\n");
	  any = TRUE;
	}
	sprintf(buf1 + strlen(buf1), "  &14%s&00\r\n", GET_NAME(d->character));
      }
    }
    send_to_char(buf1, ch);
    return;
  case '\\':
    ++argument;
    break;
  default:
    break;
  }
  skip_spaces(&argument);

  if (!*argument) {
    send_to_char("Don't bother the gods like that!\r\n", ch);
    return;
  }
/* change these colored names to whatever you will, and make sure to add the actual name
into the "" of str_cmp */
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&13Z&00&05a&13l&00&05a&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&12R&00&04a&00&03i&11d&00&04e&12n&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&09P&00&01r&16i&15m&00&01u&09s&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&10H&16y&00&02d&16r&10o&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&14S&00&06h&00&01i&09n&15i&09g&00&01a&00&06m&14i&00&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&09N&00&01e&00&06x&00&01u&09s&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&09K&00&01u&16v&00&01o&09c&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&14S&00&06o&16l&15b&00&07et&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&16K&15o&00&07m&16a&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&00&06O&06&00&14c&14&00&15e&15&00&16l&16&00&14o&14&00&06t&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&00&03K&11i&15sa&14t&00&06o&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&14S&00&06h&00&15ig&00&09e&00&01r&00&15u&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&10R&00&02a&00&01vo&00&02r&10n&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&00&06T&00&12a&00&14i&00&02s&00&06h&00&12y&00&10r&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&16M&00&01h&00&05u&00&01r&16z&00&05i&00&01n&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&10J&00&06e&02n&10t&00&06o&02w&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&00&06E&14r&15a&00g&14o&00&06n&00&07] &10%s\r\n", argument);
else if (!str_cmp(ch->player.name, ""))
sprintf(buf1, "&00&07[&00&02X&10y&00&02r&10o&00&02n&00&07] &10%s\r\n", argument);
  else if (level > LVL_IMMORT) {
    sprintf(buf1, "&12[&09%s&12] &11<%d> %s%s&00\r\n", GET_NAME(ch), level,
	    emote ? "&14<--- " : "", argument);
    sprintf(buf2, "&12[&15Someone&12] &13<%d> %s%s&00\r\n", level, emote ? "&14<--- " : "",
	    argument);
  } else {
    sprintf(buf1, "&12[&09%s&12]&00 &11%s%s\r\n", GET_NAME(ch), emote ? "&14<--- " : "",
	    argument);
    sprintf(buf2, "&12[&15Someone&12] &13%s%s&00\r\n", emote ? "&14<--- " : "", argument);
  }

  for (d = descriptor_list; d; d = d->next) {
    if ((STATE(d) == CON_PLAYING) && (GET_LEVEL(d->character) >= level) && (!PLR_FLAGGED(d->character, PLR_WRITING | PLR_MAILING))
	&& (d != ch->desc || !(PRF_FLAGGED(d->character, PRF_NOREPEAT)))) {
      send_to_char(CCCYN(d->character, C_NRM), d->character);
      if (CAN_SEE(d->character, ch))
	send_to_char(buf1, d->character);
      else
	send_to_char(buf1, d->character);
      send_to_char(CCNRM(d->character, C_NRM), d->character);
    }
  }

  if (PRF_FLAGGED(ch, PRF_NOREPEAT))
    send_to_char(OK, ch);
}



ACMD(do_zreset)
{
  int i, j;

  one_argument(argument, arg);
  if (!*arg) {
    send_to_char("You must specify a zone.\r\n", ch);
    return;
  }
  if (*arg == '*') {
    if (reseted == TRUE) {
     send_to_char("A world reset has already been done in the last 30 seconds.\r\n", ch);
     return;
    }
    for (i = 0; i <= top_of_zone_table; i++) {
      reset_zone(i);
      reset_zone(i);
      reset_zone(i);
      reset_zone(i);
      reset_zone(i);
      reset_zone(i);
      reset_zone(i);
      reset_zone(i);
      reset_zone(i);
      reset_zone(i);
      reseted = TRUE;
    }
      sprintf(buf1, "&16[&14%s &10Reset &15the entire &00&07Universe&16]&00\r\n", GET_NAME(ch));
      send_to_all(buf1);
      return;
  } else if (*arg == '.')
    i = world[ch->in_room].zone;
  else {
    j = atoi(arg);
    for (i = 0; i <= top_of_zone_table; i++)
      if (zone_table[i].number == j)
	break;
  }
  if (i >= 0 && i <= top_of_zone_table) {
    reset_zone(i);
    sprintf(buf, "Reset zone %d (#%d): %s.\r\n", i, zone_table[i].number,
	    zone_table[i].name);
    send_to_char(buf, ch);
    sprintf(buf, "(GC) %s reset zone %d (%s)", GET_NAME(ch), i, zone_table[i].name);
    mudlog(buf, NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE);
  } else
    send_to_char("Invalid zone number.\r\n", ch);
}


/*
 *  General fn for wizcommands of the sort: cmd <player>
 */

ACMD(do_wizutil)
{
  struct char_data *vict;
  int punish = 0;
  char arg2[MAX_STRING_LENGTH];
  long result;

  two_arguments(argument, arg, arg2);

  if (!*arg)
    send_to_char("Yes, but for whom?!?\r\n", ch);
  else if (!(vict = get_char_vis(ch, arg)))
    send_to_char("There is no such player.\r\n", ch);
  else if (IS_NPC(vict))
    send_to_char("You can't do that to a mob!\r\n", ch);
  else if (GET_LEVEL(vict) > GET_LEVEL(ch))
    send_to_char("Hmmm...you'd better not.\r\n", ch);
  else {
    switch (subcmd) {
    case SCMD_REROLL:
      send_to_char("Rerolled...\r\n", ch);
      roll_real_abils(vict);
      log("(GC) %s has rerolled %s.", GET_NAME(ch), GET_NAME(vict));
      sprintf(buf, "New stats: Str %d/%d, Int %d, Wis %d, Dex %d, Con %d, cha %d\r\n",
	      GET_STR(vict), GET_ADD(vict), GET_INT(vict), GET_WIS(vict),
	      GET_DEX(vict), GET_CON(vict), GET_CHA(vict));
      send_to_char(buf, ch);
      break;
    case SCMD_PARDON:
      if (vict == ch) {
       REMOVE_BIT(PRF_FLAGS(vict), PRF_NOGRATZ);
       send_to_char("You are no longer hell flagged", ch);
       return;
      }
      if (!PRF_FLAGGED(vict, PRF_NOGRATZ)) {
        send_to_char("Your victim is not in hell.\r\n", ch);
        return;
      }
      char_from_room(vict);
      char_to_room(vict, ch->in_room);
      GET_PTIMER(vict) = 0;
      REMOVE_BIT(PRF_FLAGS(vict), PRF_NOGRATZ);
      command_interpreter(vict, "recall");
      send_to_char("&15You have been pardoned by the Gods!&00\r\n", vict);
      sprintf(buf1, "&16[&14PARDON&16] &11%s &15has been released from hell&09!&00\r\n", GET_NAME(vict));
      send_to_all(buf1);
      sprintf(buf, "(GC) %s pardoned by %s", GET_NAME(vict), GET_NAME(ch));
      mudlog(buf, BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
      break;
    case SCMD_NOTITLE:
      result = PLR_TOG_CHK(vict, PLR_NOTITLE);
      sprintf(buf, "(GC) Notitle %s for %s by %s.", ONOFF(result),
	      GET_NAME(vict), GET_NAME(ch));
      mudlog(buf, NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
      strcat(buf, "\r\n");
      send_to_char(buf, ch);
      break;
    case SCMD_SQUELCH:
      result = PLR_TOG_CHK(vict, PLR_NOSHOUT);
      sprintf(buf, "(GC) Squelch %s for %s by %s.", ONOFF(result),
	      GET_NAME(vict), GET_NAME(ch));
      mudlog(buf, BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
      strcat(buf, "\r\n");
      send_to_char(buf, ch);
      sprintf(buf, "&16[&10ALERT&16] &15%s has had their mute status turned &09%s&15&11!&00\r\n", GET_NAME(vict), ONOFF(result));
      send_to_all(buf);
      break;
    case SCMD_FREEZE:
     if (!*arg2) {
        send_to_char("How long are you freezing them in minutes?", ch);
        return;
       }
       punish = atoi(arg2);
      if (ch == vict) {
	send_to_char("Oh, yeah, THAT'S real smart...\r\n", ch);
	return;
      }
      if (PLR_FLAGGED(vict, PLR_FROZEN)) {
	send_to_char("Your victim is already pretty cold.\r\n", ch);
	return;
      }
      else if (punish >= 1) {
      punish *= 60;
      GET_PTIMER(vict) = punish;
      SET_BIT(PLR_FLAGS(vict), PLR_FROZEN);
      GET_FREEZE_LEV(vict) = GET_LEVEL(ch);
      send_to_char("A bitter wind suddenly rises and drains every erg of heat from your body!\r\nYou feel frozen!\r\n", vict);
      send_to_char("Frozen.\r\n", ch);
      act("A sudden cold wind conjured from nowhere freezes $n!", FALSE, vict, 0, 0, TO_ROOM);
      sprintf(buf, "&16[&10ALERT&16] &15%s has been &14FROZEN&15 that'll teach the asshole...&09!&00\r\n", GET_NAME(vict));
      send_to_all(buf);
      sprintf(buf, "(GC) %s frozen by %s.", GET_NAME(vict), GET_NAME(ch));
      mudlog(buf, BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
      }
      break;
    case SCMD_THAW:
      if (!PLR_FLAGGED(vict, PLR_FROZEN)) {
	send_to_char("Sorry, your victim is not morbidly encased in ice at the moment.\r\n", ch);
	return;
      }
      if (GET_FREEZE_LEV(vict) > GET_LEVEL(ch)) {
	sprintf(buf, "Sorry, a level %Ld God froze %s... you can't unfreeze %s.\r\n",
	   GET_FREEZE_LEV(vict), GET_NAME(vict), HMHR(vict));
	send_to_char(buf, ch);
	return;
      }
      GET_PTIMER(vict) = 0;
      sprintf(buf, "(GC) %s un-frozen by %s.", GET_NAME(vict), GET_NAME(ch));
      mudlog(buf, BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE);
      REMOVE_BIT(PLR_FLAGS(vict), PLR_FROZEN);
      send_to_char("A fireball suddenly explodes in front of you, melting the ice!\r\nYou feel thawed.\r\n", vict);
      send_to_char("Thawed.\r\n", ch);
      act("A sudden fireball conjured from nowhere thaws $n!", FALSE, vict, 0, 0, TO_ROOM);
      sprintf(buf, "&16[&10ALERT&16] &15%s has been &09T&00&01h&09a&00&01w&09e&00&01d&15 unfortunatly...&09!&00\r\n", GET_NAME(vict));
      send_to_all(buf);
      break;
    case SCMD_UNAFFECT:
      if (vict->affected) {
	while (vict->affected)
	  affect_remove(vict, vict->affected);
	send_to_char("There is a brief flash of light!\r\n"
		     "You feel slightly different.\r\n", vict);
	send_to_char("All spells removed.\r\n", ch);
      } else {
	send_to_char("Your victim does not have any affections!\r\n", ch);
	return;
      }
      break;
    default:
      log("SYSERR: Unknown subcmd %d passed to do_wizutil (%s)", subcmd, __FILE__);
      break;
    }
    save_char(vict, NOWHERE);
  }
}


/* single zone printing fn used by "show zone" so it's not repeated in the
   code 3 times ... -je, 4/6/93 */

void print_zone_to_buf(char *bufptr, int zone)
{
  sprintf(bufptr, "%s%3d %-30.30s Age: %3d; Reset: %3d (%1d); Top: %5d\r\n",
	  bufptr, zone_table[zone].number, zone_table[zone].name,
	  zone_table[zone].age, zone_table[zone].lifespan,
	  zone_table[zone].reset_mode, zone_table[zone].top);
}


ACMD(do_show)
{
  struct char_file_u vbuf;
  int i, j, k, l, con;
  char self = 0;
  struct char_data *vict;
  struct obj_data *obj;
  char field[MAX_INPUT_LENGTH], value[MAX_INPUT_LENGTH], birth[80];

  struct show_struct {
    const char *cmd;
    const long long level;
  } fields[] = {
    { "nothing",	0  },				/* 0 */
    { "zones",		LVL_IMMORT },			/* 1 */
    { "player",		LVL_GOD },
    { "rent",		LVL_GOD },
    { "stats",		LVL_IMMORT },
    { "errors",		LVL_IMPL },			/* 5 */
    { "death",		LVL_GOD },
    { "godrooms",	LVL_GOD },
    { "shops",		1000014 },
    { "houses",		LVL_GOD },
    { "\n", 0 }
  };

  skip_spaces(&argument);

  if (!*argument) {
    strcpy(buf, "Show options:\r\n");
    for (j = 0, i = 1; fields[i].level; i++)
      if (fields[i].level <= GET_LEVEL(ch))
	sprintf(buf + strlen(buf), "%-15s%s", fields[i].cmd, (!(++j % 5) ? "\r\n" : ""));
    strcat(buf, "\r\n");
    send_to_char(buf, ch);
    return;
  }

  strcpy(arg, two_arguments(argument, field, value));

  for (l = 0; *(fields[l].cmd) != '\n'; l++)
    if (!strncmp(field, fields[l].cmd, strlen(field)))
      break;

  if (GET_LEVEL(ch) < fields[l].level) {
    send_to_char("You are not godly enough for that!\r\n", ch);
    return;
  }
  if (!strcmp(value, "."))
    self = 1;
  buf[0] = '\0';
  switch (l) {
  case 1:			/* zone */
    /* tightened up by JE 4/6/93 */
    if (self)
      print_zone_to_buf(buf, world[ch->in_room].zone);
    else if (*value && is_number(value)) {
      for (j = atoi(value), i = 0; zone_table[i].number != j && i <= top_of_zone_table; i++);
      if (i <= top_of_zone_table)
	print_zone_to_buf(buf, i);
      else {
	send_to_char("That is not a valid zone.\r\n", ch);
	return;
      }
    } else
      for (i = 0; i <= top_of_zone_table; i++)
	print_zone_to_buf(buf, i);
    page_string(ch->desc, buf, TRUE);
    break;
  case 2:			/* player */
    if (!*value) {
      send_to_char("A name would help.\r\n", ch);
      return;
    }

    if (load_char(value, &vbuf) < 0) {
      send_to_char("There is no such player.\r\n", ch);
      return;
    }
    sprintf(buf, "Player: %-12s (%s) [%7Ld %s] They have %s eyes, and %s %s hair.\r\n", vbuf.name,
      genders[(int) vbuf.sex], vbuf.level, class_abbrevs[(int) vbuf.chclass], eyes[(int) vbuf.eye], hl[(int) vbuf.hairl], hc[(int) vbuf.hairc] );
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
    break;
  case 3:
    Crash_listrent(ch, value);
    break;
  case 4:
    i = 0;
    j = 0;
    k = 0;
    con = 0;
    for (vict = character_list; vict; vict = vict->next) {
      if (IS_NPC(vict))
	j++;
      else if (CAN_SEE(ch, vict)) {
	i++;
	if (vict->desc)
	  con++;
      }
    }
    for (obj = object_list; obj; obj = obj->next)
      k++;
    strcpy(buf, "Current stats:\r\n");
    sprintf(buf + strlen(buf), "  %5d players in game  %5d connected\r\n",
		i, con);
    sprintf(buf + strlen(buf), "  %5d registered\r\n",
		top_of_p_table + 1);
    sprintf(buf + strlen(buf), "  %5d mobiles          %5d prototypes\r\n",
		j, top_of_mobt + 1);
    sprintf(buf + strlen(buf), "  %5d objects          %5d prototypes\r\n",
		k, top_of_objt + 1);
    sprintf(buf + strlen(buf), "  %5d rooms            %5d zones\r\n",
		top_of_world + 1, top_of_zone_table + 1);
    sprintf(buf + strlen(buf), "  %5d large bufs\r\n",
		buf_largecount);
    sprintf(buf + strlen(buf), "  %5d buf switches     %5d overflows\r\n",
		buf_switches, buf_overflows);
    send_to_char(buf, ch);
    break;
  case 5:
    strcpy(buf, "Errant Rooms\r\n------------\r\n");
    for (i = 0, k = 0; i <= top_of_world; i++)
      for (j = 0; j < NUM_OF_DIRS; j++)
	if (world[i].dir_option[j] && world[i].dir_option[j]->to_room == 0)
	  sprintf(buf + strlen(buf), "%2d: [%5d] %s\r\n", ++k, GET_ROOM_VNUM(i),
		  world[i].name);
    page_string(ch->desc, buf, TRUE);
    break;
  case 6:
    strcpy(buf, "Death Traps\r\n-----------\r\n");
    for (i = 0, j = 0; i <= top_of_world; i++)
      if (ROOM_FLAGGED(i, ROOM_DEATH))
	sprintf(buf + strlen(buf), "%2d: [%5d] %s\r\n", ++j,
		GET_ROOM_VNUM(i), world[i].name);
    page_string(ch->desc, buf, TRUE);
    break;
  case 7:
    strcpy(buf, "Godrooms\r\n--------------------------\r\n");
    for (i = 0, j = 0; i < top_of_world; i++)
    if (ROOM_FLAGGED(i, ROOM_GODROOM))
      sprintf(buf + strlen(buf), "%2d: [%5d] %s\r\n",
		++j, GET_ROOM_VNUM(i), world[i].name);
    page_string(ch->desc, buf, TRUE);
    break;
  case 8:
    send_to_char("Some people are too stupid to use this command without crashing the mud..\r\n", ch);
    /*show_shops(ch, value);*/
    break;
  case 9:
    hcontrol_list_houses(ch);
    break;
  default:
    send_to_char("Sorry, I don't understand that.\r\n", ch);
    break;
  }
}


/***************** The do_set function ***********************************/

#define PC   1
#define NPC  2
#define BOTH 3

#define MISC	0
#define BINARY	1
#define NUMBER	2

#define SET_OR_REMOVE(flagset, flags) { \
	if (on) SET_BIT(flagset, flags); \
	else if (off) REMOVE_BIT(flagset, flags); }

#define RANGE(low, high) (value = MAX((low), MIN((high), (value))))


/* The set options available */
  struct set_struct {
    const char *cmd;
    const long long level;
    const char pcnpc;
    const char type;
  } set_fields[] = {
   { "brief",		LVL_GOD, 	PC, 	BINARY },  /* 0 */
   { "invstart", 	LVL_GOD, 	PC, 	BINARY },  /* 1 */
   { "title",		LVL_GOD, 	PC, 	MISC },
   { "nosummon", 	LVL_GRGOD, 	PC, 	BINARY },
   { "maxhit",		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "maxmana", 	LVL_GRGOD, 	BOTH, 	NUMBER },  /* 5 */
   { "maxmove", 	LVL_GRGOD, 	BOTH, 	NUMBER },
   { "hit", 		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "mana",		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "move",		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "truth",		LVL_GOD, 	BOTH, 	NUMBER },  /* 10 */
   { "str",		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "stradd",		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "int", 		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "wis", 		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "dex", 		LVL_GRGOD, 	BOTH, 	NUMBER },  /* 15 */
   { "con", 		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "cha",		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "ac", 		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "gold",		LVL_GOD, 	BOTH, 	NUMBER },
   { "bank",		LVL_GOD, 	PC, 	NUMBER },  /* 20 */
   { "exp", 		LVL_GRGOD, 	BOTH, 	NUMBER },
   { "hitroll", 	LVL_GRGOD, 	BOTH, 	NUMBER },
   { "damroll", 	LVL_GRGOD, 	BOTH, 	NUMBER },
   { "invis",		LVL_IMPL, 	PC, 	NUMBER },
   { "nohassle", 	LVL_GRGOD, 	PC, 	BINARY },  /* 25 */
   { "frozen",		LVL_FREEZE, 	PC, 	BINARY },
   { "practices", 	LVL_GRGOD, 	PC, 	NUMBER },
   { "lessons", 	LVL_GRGOD, 	PC, 	NUMBER },
   { "drunk",		LVL_GRGOD, 	BOTH, 	MISC },
   { "hunger",		LVL_GRGOD, 	BOTH, 	MISC },    /* 30 */
   { "thirst",		LVL_GRGOD, 	BOTH, 	MISC },
   { "killer",		LVL_GOD, 	PC, 	BINARY },
   { "icer",		LVL_GOD, 	PC, 	BINARY },
   { "level",		LVL_IMPLL, 	BOTH, 	NUMBER },
   { "room",		LVL_IMPL, 	BOTH, 	NUMBER },  /* 35 */
   { "roomflag", 	LVL_GRGOD, 	PC, 	BINARY },
   { "siteok",		LVL_IMPLL, 	PC, 	BINARY },
   { "deleted", 	LVL_IMPLL, 	PC, 	BINARY },
   { "class",		LVL_GRGOD, 	BOTH, 	MISC },
   { "nowizlist", 	LVL_GOD, 	PC, 	BINARY },  /* 40 */
   { "quest",		LVL_GOD, 	PC, 	BINARY },
   { "loadroom", 	LVL_GRGOD, 	PC, 	MISC },
   { "color",		LVL_GOD, 	PC, 	BINARY },
   { "idnum",		LVL_IMPL, 	PC, 	NUMBER },
   { "passwd",		LVL_IMPL, 	PC, 	MISC },    /* 45 */
   { "nodelete", 	LVL_GOD, 	PC, 	BINARY },
   { "sex", 		LVL_GRGOD, 	BOTH, 	MISC },
   { "age",		LVL_GRGOD,	BOTH,	NUMBER },
   { "attacks",		LVL_GOD,	BOTH,	NUMBER },
   { "mystic",          LVL_GRGOD,      PC,     BINARY },  /* 50 */
   { "rage",            LVL_GRGOD,      BOTH,   NUMBER },
   { "clan",            LVL_IMMORT,     PC,     NUMBER },
   { "ubs",             LVL_GOD,        PC,     NUMBER },
   { "lbs",             LVL_GOD,        PC,     NUMBER },
   { "hairl",           LVL_GOD,        PC,     NUMBER }, /* 55 */
   { "pdeath",          LVL_GOD,        PC,     NUMBER }, /* 58 */
   { "pk",          LVL_GOD,        PC,     NUMBER }, /* 57 */
   { "skin",          LVL_GOD,        PC,     NUMBER }, /* 57 */
   { "hairc",          LVL_GOD,        PC,     NUMBER },
   { "eye",          LVL_GOD,        PC,     NUMBER },
   { "reinc",        LVL_GOD,        PC,     NUMBER },
   { "spirit",        LVL_GOD,        PC,     NUMBER },
   { "helper",        LVL_GOD,        PC,     BINARY },
   { "build",        LVL_GOD,        PC,     NUMBER },
   { "majin",        LVL_GOD,        PC,     BINARY },
   { "npc",        LVL_GOD,        PC,     BINARY },
   { "login",        1000006,        PC,     NUMBER },
   { "\n", 0, BOTH, MISC }
  };


int perform_set(struct char_data *ch, struct char_data *vict, int mode,
		char *val_arg)
{
  int i, on = 0, off = 0;
  long long value = 0;
  char output[MAX_STRING_LENGTH];

  /* Check to make sure all the levels are correct */
  if (GET_LEVEL(ch) != LVL_IMPL) {
    if (!IS_NPC(vict) && GET_LEVEL(ch) <= GET_LEVEL(vict) && vict != ch) {
      send_to_char("Maybe that's not such a great idea...\r\n", ch);
      return 0;
    }
  }
  if (GET_LEVEL(ch) < set_fields[mode].level) {
    send_to_char("You are not godly enough for that!\r\n", ch);
    return 0;
  }

  /* Make sure the PC/NPC is correct */
  if (IS_NPC(vict) && !(set_fields[mode].pcnpc & NPC) && GET_LEVEL(vict) <= 999999) {
    send_to_char("You can't do that to a beast!\r\n", ch);
    return 0;
  } else if (!IS_NPC(vict) && !(set_fields[mode].pcnpc & PC)) {
    send_to_char("That can only be done to a beast!\r\n", ch);
    return 0;
  }

  /* Find the value of the argument */
  if (set_fields[mode].type == BINARY) {
    if (!strcmp(val_arg, "on") || !strcmp(val_arg, "yes"))
      on = 1;
    else if (!strcmp(val_arg, "off") || !strcmp(val_arg, "no"))
      off = 1;
    if (!(on || off)) {
      send_to_char("Value must be 'on' or 'off'.\r\n", ch);
      return 0;
    }
    sprintf(output, "%s %s for %s.", set_fields[mode].cmd, ONOFF(on),
	    GET_NAME(vict));
  } else if (set_fields[mode].type == NUMBER) {
    value = atol(val_arg);
    sprintf(output, "%s's %s set to %Ld.", GET_NAME(vict),
	    set_fields[mode].cmd, value);
  } else {
    strcpy(output, "Okay.");  /* can't use OK macro here 'cause of \r\n */
  }

  switch (mode) {
  case 0:
    SET_OR_REMOVE(PRF_FLAGS(vict), PRF_BRIEF);
    break;
  case 1:
    SET_OR_REMOVE(PLR_FLAGS(vict), PLR_INVSTART);
    break;
  case 2:
    set_title(vict, val_arg);
    sprintf(output, "%s's title is now: %s", GET_NAME(vict), GET_TITLE(vict));
    break;
  case 3:
  case 4:
    vict->points.max_hit = RANGE(1, 2000000000);
    affect_total(vict);
    sprintf(buf, "(GC) %s has set %s's &10Max hit&00", GET_NAME(ch), GET_NAME(vict));
        mudlog(buf, BRF, 1000014, TRUE);
    break;
  case 5:
    vict->points.max_mana = RANGE(1, 2000000000);
    affect_total(vict);
    sprintf(buf, "(GC) %s has set %s's &10Max Mana", GET_NAME(ch), GET_NAME(vict));
        mudlog(buf, BRF, 1000014, TRUE);
    break;
  case 6:
    vict->points.max_move = RANGE(1, 50000);
    affect_total(vict);
    break;
  case 7:
    vict->points.hit = RANGE(-9, vict->points.max_hit);
    affect_total(vict);
    break;
  case 8:
    vict->points.mana = RANGE(0, vict->points.max_mana);
    affect_total(vict);
    break;
  case 9:
    vict->points.move = RANGE(0, vict->points.max_move);
    affect_total(vict);
    break;
  case 10:
    sprintf(buf, "&16(&11CONGRATULATIONS&16) &14%s &15has been awarded &10%d &12T&00&07r&00&06u&00&07t&12h &11Points&09!&00\r\n", GET_NAME(vict), value - GET_ALIGNMENT(vict));
    send_to_all(buf);
    GET_ALIGNMENT(vict) = RANGE(0, 1000);
    affect_total(vict);
    break;
  case 11:
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(3, 100);
    else
      RANGE(3, 100);
    vict->real_abils.str = value;
    vict->real_abils.str_add = 0;
    affect_total(vict);
    break;
  case 12:
    vict->real_abils.str_add = RANGE(0, 100);
    if (value > 0)
      vict->real_abils.str = 200;
    affect_total(vict);
    break;
  case 13:
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(3, 100);
    else
      RANGE(3, 100);
    vict->real_abils.intel = value;
    affect_total(vict);
    break;
  case 14:
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(3, 100);
    else
      RANGE(3, 100);
    vict->real_abils.wis = value;
    affect_total(vict);
    break;
  case 15:
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(3, 100);
    else
      RANGE(3, 100);
    vict->real_abils.dex = value;
    affect_total(vict);
    break;
  case 16:
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(3, 100);
    else
      RANGE(3, 100);
    vict->real_abils.con = value;
    affect_total(vict);
    break;
  case 17:
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(3, 100);
    else
      RANGE(3, 100);
    vict->real_abils.cha = value;
    affect_total(vict);
    break;
  case 18:
    vict->points.armor = RANGE(-100, 100);
    affect_total(vict);
    break;
  case 19:
    GET_GOLD(vict) = RANGE(0, 900000000);
    break;
  case 20:
    GET_BANK_GOLD(vict) = RANGE(0, 900000000);
    break;
  case 21:
    vict->points.exp = RANGE(0, 999999999);
    break;
  case 22:
    vict->points.hitroll = RANGE(-20, 1000);
    affect_total(vict);
    break;
  case 23:
    vict->points.damroll = RANGE(-20, 1000000);
    affect_total(vict);
    break;
  case 24:
    if (GET_LEVEL(ch) < LVL_IMPL && ch != vict) {
      send_to_char("You aren't godly enough for that!\r\n", ch);
      return 0;
    }
    GET_INVIS_LEV(vict) = RANGE(0, GET_LEVEL(vict));
    break;
  case 25:
    if (GET_LEVEL(ch) < LVL_IMPL && ch != vict) {
      send_to_char("You aren't godly enough for that!\r\n", ch);
      return 0;
    }
    SET_OR_REMOVE(PRF_FLAGS(vict), PRF_NOHASSLE);
    break;
  case 26:
    if (ch == vict) {
      send_to_char("Better not -- could be a long winter!\r\n", ch);
      return 0;
    }
    SET_OR_REMOVE(PLR_FLAGS(vict), PLR_FROZEN);
    break;
  case 27:
  case 28:
    GET_PRACTICES(vict) = RANGE(0, 3000000);
    break;
  case 29:
  case 30:
  case 31:
    if (!str_cmp(val_arg, "off")) {
      GET_COND(vict, (mode - 29)) = (char) -1; /* warning: magic number here */
      sprintf(output, "%s's %s now off.", GET_NAME(vict), set_fields[mode].cmd);
    } else if (is_number(val_arg)) {
      value = atoi(val_arg);
      RANGE(0, 24);
      GET_COND(vict, (mode - 29)) = (char) value; /* and here too */
      sprintf(output, "%s's %s set to %Ld.", GET_NAME(vict),
	      set_fields[mode].cmd, value);
    } else {
      send_to_char("Must be 'off' or a value from 0 to 24.\r\n", ch);
      return 0;
    }
    break;
  case 32:
    SET_OR_REMOVE(PLR_FLAGS(vict), PLR_KILLER);
    break;
  case 33:
    SET_OR_REMOVE(PLR_FLAGS(vict), PLR_icer);
    break;
  case 34:
    if (value > GET_LEVEL(ch) || value > GET_LEVEL(ch)) {
      send_to_char("You can't do that.\r\n", ch);
      return 0;
    }
    else
    RANGE(0, 1000014);
    vict->player.level = (long long) value;
    break;
  case 35:
    if ((i = real_room(value)) < 0) {
      send_to_char("No room exists with that number.\r\n", ch);
      return 0;
    }
    if (IN_ROOM(vict) != NOWHERE)	/* Another Eric Green special. */
      char_from_room(vict);
    char_to_room(vict, i);
    break;
  case 36:
    SET_OR_REMOVE(PRF_FLAGS(vict), PRF_ROOMFLAGS);
    break;
  case 37:
    SET_OR_REMOVE(PLR_FLAGS(vict), PLR_SITEOK);
    break;
  case 38:
    SET_OR_REMOVE(PLR_FLAGS(vict), PLR_DELETED);
    break;
  case 39:
    if ((i = parse_class(*val_arg)) == CLASS_UNDEFINED) {
      send_to_char("That is not a class.\r\n", ch);
      return 0;
    }
    GET_CLASS(vict) = i;
    break;
  case 40:
    SET_OR_REMOVE(PLR_FLAGS(vict), PLR_NOWIZLIST);
    break;
  case 41:
    SET_OR_REMOVE(PRF_FLAGS(vict), PRF_QUEST);
    break;
  case 42:
    if (!str_cmp(val_arg, "off")) {
      REMOVE_BIT(PLR_FLAGS(vict), PLR_LOADROOM);
    } else if (is_number(val_arg)) {
      value = atoi(val_arg);
      if (real_room(value) != NOWHERE) {
        SET_BIT(PLR_FLAGS(vict), PLR_LOADROOM);
	GET_LOADROOM(vict) = value;
	sprintf(output, "%s will enter at room #%d.", GET_NAME(vict),
		GET_LOADROOM(vict));
      } else {
	send_to_char("That room does not exist!\r\n", ch);
	return 0;
      }
    } else {
      send_to_char("Must be 'off' or a room's virtual number.\r\n", ch);
      return 0;
    }
    break;
  case 43:
    SET_OR_REMOVE(PRF_FLAGS(vict), (PRF_COLOR_1 | PRF_COLOR_2));
    break;
  case 44:
    if (GET_IDNUM(ch) != 1 || !IS_NPC(vict))
      return 0;
    GET_IDNUM(vict) = value;
    break;
  case 45:
    if (GET_LEVEL(vict) >= GET_LEVEL(ch) && GET_NAME(ch) != GET_NAME(vict)) {
      send_to_char("You cannot change that.\r\n", ch);
      return 0;
    }
    strncpy(GET_PASSWD(vict), CRYPT(val_arg, GET_NAME(vict)), MAX_PWD_LENGTH);
    *(GET_PASSWD(vict) + MAX_PWD_LENGTH) = '\0';
    sprintf(output, "Password changed to '%s'.", val_arg);
    break;
  case 46:
    SET_OR_REMOVE(PLR_FLAGS(vict), PLR_NODELETE);
    break;
  case 47:
    if (!str_cmp(val_arg, "male"))
      vict->player.sex = SEX_MALE;
    else if (!str_cmp(val_arg, "female"))
      vict->player.sex = SEX_FEMALE;
    else if (!str_cmp(val_arg, "neutral"))
      vict->player.sex = SEX_NEUTRAL;
    else {
      send_to_char("Must be 'male', 'female', or 'neutral'.\r\n", ch);
      return 0;
    }
    break;
  case 48:	/* set age */
    if (value < 2 || value > 200) {	/* Arbitrary limits. */
      send_to_char("Ages 2 to 200 accepted.\r\n", ch);
      return 0;
    }
     else if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(2, 200);
    else
      RANGE(2, 200);
    age(vict)->year = value;
    affect_total(vict);
    break;   
    case 49:
   if (value < 1 || value > 10) {
    send_to_char("Number of attacks can only be 1 to 10\r\n", ch);
    return (0);
    }
   if (!IS_NPC(vict))
    vict->player_specials->saved.pc_attacks = value;
   else
   if (IS_NPC(vict))
    vict->mob_specials.num_attacks = value;
   else
   send_to_char("Sorry you can't do that right now", ch);
   break;
  case 50:
    if (GET_LEVEL(ch) < LVL_GRGOD && ch != vict) {
      send_to_char("You aren't godly enough for that!\r\n", ch);
      return 0;
    }
    SET_OR_REMOVE(PRF_FLAGS(vict), PRF_MYSTIC);
    break;
  case 51:
    if (GET_LEVEL(ch) < LVL_GRGOD && ch != vict) {
      send_to_char("You aren't godly enough for that!\r\n", ch);
      return 0;
    }
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(3, 100);
    else
      RANGE(3, 100);
    vict->real_abils.rage = value;
    affect_total(vict);
    break;
  case 52:
    if (value > 8) {
    send_to_char("The range is 0-8.  Clans are in this order: ZW, SE, RRA, DW, PV, IE, DL, DV, None.\r\n", ch);
   }
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(0, 8);
    else
      RANGE(0, 8);
    vict->player.clan = value;
    affect_total(vict);
    break;
  case 53:
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(3, 100);
    else
      RANGE(3, 100);
    vict->real_abils.ubs = value;
    affect_total(vict);
    break;
  case 54:
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(3, 100);
    else
      RANGE(3, 100);
    vict->real_abils.lbs = value;
    affect_total(vict);
    break;
  case 55:
      RANGE(1, 9);
   vict->player.hairl = value;
    break;
  case 56:
    if (GET_LEVEL(ch) < 1000000 && ch != vict) {
      send_to_char("You aren't godly enough for that!\r\n", ch);
      return 0;
    }
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(0, 10000);
    else
      RANGE(0, 10000);
    vict->player.weight = value;
    affect_total(vict);
    break;
  case 57:
    if (GET_LEVEL(ch) < 1000000 && ch != vict) {
      send_to_char("You aren't godly enough for that!\r\n", ch);
      return 0;
    }
    if (IS_NPC(vict) || GET_LEVEL(vict) >= LVL_GRGOD)
      RANGE(0, 10000);
    else
      RANGE(0, 10000);
    vict->player.height = value;
    affect_total(vict);
    break;
  case 58:
      RANGE(0, 7);
   vict->player.aura = value;
    break;
  case 59:
      RANGE(0, 8);
   vict->player.hairc = value;
    break;
  case 60:
      RANGE(1, 8);
   vict->player.eye = value;
    break;
  case 61:
     RANGE(0, 50);
   vict->player.god = value;
    break;
  case 62:
     RANGE(0, 100000);
   vict->player.hometown = value;
    break;
  case 63:
     SET_OR_REMOVE(PRF_FLAGS(vict), PRF_AUTOLOOT);
    break;
  case 64:
      RANGE(0, 5);
   vict->player.build = value;
    break;
  case 65:
     SET_OR_REMOVE(PLR_FLAGS(vict), PLR_MAJINIZE);
    break;
  case 66:
    SET_OR_REMOVE(PLR_FLAGS(vict), PLR_DONTSET);
    break;
  case 67:
     RANGE(0, 50);
    vict->player_specials->saved.loginc = value;
    break;
  /* just noticed I contradicted myself here */
  /* can be set 1-5 for mobs and pc's */
  /* but olc can set mobs to 10 .. oh well.. fix as you see fit*/

    /*
     * NOTE: May not display the exact age specified due to the integer
     * division used elsewhere in the code.  Seems to only happen for
     * some values below the starting age (17) anyway. -gg 5/27/98
     */
    ch->player.time.birth = time(0) - ((value - 17) * SECS_PER_MUD_YEAR);
    break;
  default:
    send_to_char("Can't set that!\r\n", ch);
    return 0;
  }

  strcat(output, "\r\n");
  send_to_char(CAP(output), ch);
  return 1;
}


ACMD(do_set)
{
  struct char_data *vict = NULL, *cbuf = NULL;
  struct char_file_u tmp_store;
  char field[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH],
	val_arg[MAX_INPUT_LENGTH];
  int mode = -1, len = 0, player_i = 0, retval;
  char is_file = 0, is_mob = 0, is_player = 0;

  half_chop(argument, name, buf);

  if (!strcmp(name, "file")) {
    is_file = 1;
    half_chop(buf, name, buf);
  } else if (!str_cmp(name, "player")) {
    is_player = 1;
    half_chop(buf, name, buf);
  } else if (!str_cmp(name, "mob")) {
    is_mob = 1;
    half_chop(buf, name, buf);
  }
  half_chop(buf, field, buf);
  strcpy(val_arg, buf);

  if (!*name || !*field) {
    send_to_char("Usage: set <victim> <field> <value>\r\n", ch);
    return;
  }

  /* find the target */
  if (!is_file) {
    if (is_player) {
      if (!(vict = get_player_vis(ch, name, 0))) {
	send_to_char("There is no such player.\r\n", ch);
	return;
      }
    } else {
      if (!(vict = get_char_vis(ch, name))) {
	send_to_char("There is no such creature.\r\n", ch);
	return;
      }
    }
  } else if (is_file) {
    /* try to load the player off disk */
    CREATE(cbuf, struct char_data, 1);
    clear_char(cbuf);
    if ((player_i = load_char(name, &tmp_store)) > -1) {
      store_to_char(&tmp_store, cbuf);
      if (GET_LEVEL(cbuf) >= GET_LEVEL(ch)) {
	free_char(cbuf);
	send_to_char("Sorry, you can't do that.\r\n", ch);
	return;
      }
      vict = cbuf;
    } else {
      free(cbuf);
      send_to_char("There is no such player.\r\n", ch);
      return;
    }
  }

  /* find the command in the list */
  len = strlen(field);
  for (mode = 0; *(set_fields[mode].cmd) != '\n'; mode++)
    if (!strncmp(field, set_fields[mode].cmd, len))
      break;

  /* perform the set */
  retval = perform_set(ch, vict, mode, val_arg);

  /* save the character if a change was made */
  if (retval) {
    if (!is_file && !IS_NPC(vict))
      save_char(vict, NOWHERE);
    if (is_file) {
      char_to_store(vict, &tmp_store);
      fseek(player_fl, (player_i) * sizeof(struct char_file_u), SEEK_SET);
      fwrite(&tmp_store, sizeof(struct char_file_u), 1, player_fl);
      send_to_char("Saved in file.\r\n", ch);
    }
  }

  /* free the memory if we allocated it earlier */
  if (is_file)
    free_char(cbuf);
}

ACMD(do_ftick)
{
 if (GET_LEVEL(ch) >= 1000000) {
 /*time_info.day += 1;*/
 send_to_char("You Summon the Fuzziness inside you and make the time PASS!\n\r", ch);               
 return;                                                                     
 sprintf(buf, "(GC) %s has sped up time by one mud day with ftick.", GET_NAME(ch));
  mudlog(buf, BRF, LVL_IMMORT, TRUE);
 }            
 else
  send_to_char("You make a fool out of yourself!", ch);
  sprintf(buf, "(GC) %s has attempted to ftick!", GET_NAME(ch));
  mudlog(buf, BRF, LVL_IMMORT, TRUE);
}
/* eighteen */

extern int mother_desc, port;
extern FILE *player_fl;
void Crash_rentsave(struct char_data * ch, int cost);

#define EXE_FILE "bin/circle" /* maybe use argv[0] but it's not reliable 
*/


ACMD(do_copyover)
{
   FILE *fp;
   struct descriptor_data *d, *d_next;
   char buf [100], buf2[100];
 	
   fp = fopen (COPYOVER_FILE, "w");
     if (!fp) {
       send_to_char ("Copyover file not writeable, aborted.\n\r",ch);
       return;
     }
 
    /* 
     * Uncomment if you use OasisOLC2.0, this saves all OLC modules:
      save_all();
     *
     */
    sprintf (buf, "\n\rCopyover by %s.\n\r", GET_NAME(ch));
 	
    /* For each playing descriptor, save its state */
    for (d = descriptor_list; d ; d = d_next) {
      struct char_data * och = d->character;
    /* We delete from the list , so need to save this */
      d_next = d->next;
 
   /* drop those logging on */
    if (!d->character || d->connected > CON_PLAYING) {
      write_to_descriptor (d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r");
      close_socket (d); /* throw'em out */
    } else {
       fprintf (fp, "%d %s %s\n", d->descriptor, GET_NAME(och), d->host);
       /* save och */
       Crash_rentsave(och,0);
       save_char(och, och->in_room);
       write_to_descriptor (d->descriptor, buf);
     }
   }
 
   fprintf (fp, "-1\n");
   fclose (fp);
 
   /* Close reserve and other always-open files and release other resources */
    fclose(player_fl);
 
   /* exec - descriptors are inherited */
    sprintf (buf, "%d", port);
    sprintf (buf2, "-C%d", mother_desc);
 
   /* Ugh, seems it is expected we are 1 step above lib - this may be dangerous! */
    chdir ("..");
 
    execl (EXE_FILE, "dbi", buf2, buf, (char *) NULL);
 
    /* Failed - sucessful exec will not return */
    perror ("do_copyover: execl");
    send_to_char ("Copyover FAILED!\n\r",ch);
 
  exit (1); /* too much trouble to try to recover! */
 }

ACMD(do_tedit) {
  int l, i;
  char field[MAX_INPUT_LENGTH];
  
  struct editor_struct {
    char *cmd;
    long long level;
    char *buffer;
    int  size;
    char *filename;
  } fields[] = {
    /* edit the lvls to your own needs */
	{ "credits",	LVL_IMPL,	credits,	2400,	CREDITS_FILE},
	{ "news",	LVL_GRGOD,	news,		8192,	NEWS_FILE},
	{ "motd",	LVL_GRGOD,	motd,		2400,	MOTD_FILE},
	{ "imotd",	LVL_IMPL,	imotd,		2400,	IMOTD_FILE},
	{ "help",       LVL_GRGOD,	help,		2400,	HELP_PAGE_FILE},
	{ "info",	LVL_GRGOD,	info,		8192,	INFO_FILE},
	{ "background",	LVL_IMPL,	background,	8192,	BACKGROUND_FILE},
	{ "handbook",   LVL_IMPL,	handbook,	8192,   HANDBOOK_FILE},
	{ "policies",	LVL_IMPL,	policies,	8192,	POLICIES_FILE},
	{ "startup",	LVL_IMPL,	startup,	8192,	STARTUP_FILE},
        { "logs",       LVL_IMPL,       syslog,         2400,   SYSLOG_FILE},
	{ "\n",		0,		NULL,		0,	NULL }
  };

  if (ch->desc == NULL) {
    send_to_char("Get outta here you linkdead head!\r\n", ch);
    return;
  }
  
  if (GET_LEVEL(ch) < LVL_GRGOD) {
    send_to_char("You do not have text editor permissions.\r\n", ch);
    return;
  }
  
  half_chop(argument, field, buf);

  if (!*field) {
    strcpy(buf, "Files available to be edited:\r\n");
    i = 1;
    for (l = 0; *fields[l].cmd != '\n'; l++) {
      if (GET_LEVEL(ch) >= fields[l].level) {
        sprintf(buf, "%s%-11.11s", buf, fields[l].cmd);
        if (!(i % 7)) strcat(buf, "\r\n");
        i++;
      }
    }
    if (--i % 7) strcat(buf, "\r\n");
    if (i == 0) strcat(buf, "None.\r\n");
    send_to_char(buf, ch);
    return;
  }

  for (l = 0; *(fields[l].cmd) != '\n'; l++)
    if (!strncmp(field, fields[l].cmd, strlen(field)))
      break;

  if (*fields[l].cmd == '\n') {
    send_to_char("Invalid text editor option.\r\n", ch);
    return;
  }
 
  if (GET_LEVEL(ch) < fields[l].level) {
    send_to_char("You are not godly enough for that!\r\n", ch);
    return;
  }

  switch (l) {
    case 0: ch->desc->str = &credits; break;
    case 1: ch->desc->str = &news; break;
    case 2: ch->desc->str = &motd; break;
    case 3: ch->desc->str = &imotd; break;
    case 4: ch->desc->str = &help; break;
    case 5: ch->desc->str = &info; break;
    case 6: ch->desc->str = &background; break;
    case 7: ch->desc->str = &handbook; break;
    case 8: ch->desc->str = &policies; break;
    case 9: ch->desc->str = &startup; break;
    case 10: ch->desc->str = &syslog; break;
    default:
      send_to_char("Invalid text editor option.\r\n", ch);
      return;
  }
  
  /* set up editor stats */
  send_to_char("\x1B[H\x1B[J", ch);
  send_to_char("Edit file below: (/s saves /h for help)\r\n", ch);
  ch->desc->backstr = NULL;
  if (fields[l].buffer) {
    send_to_char(fields[l].buffer, ch);
    ch->desc->backstr = str_dup(fields[l].buffer);
  }
  ch->desc->max_str = fields[l].size;
  ch->desc->mail_to = 0;
  CREATE(ch->desc->olc, struct olc_data, 1);
  OLC_STORAGE(ch->desc) = str_dup(fields[l].filename);
  act("$n begins editing a text file.", TRUE, ch, 0, 0, TO_ROOM);
  SET_BIT(PLR_FLAGS(ch), PLR_WRITING);
  STATE(ch->desc) = CON_TEXTED;
}


/* DECK-ME-OUT [11:08 PST March,5, 2000]
 * ---------------------------------------
 * This is intented to make IMMS feel they're more special in the mud.
 * This is mainly just for fun!
 * FEEL free to modify it anyway you like.
 *
 * Wilson Tam 
 * wilsontam@yahoo.com
 *
 * [Final Fantasy Universe MUD]
 * telnet://kin.dynip.com 4000
 */
#define MAX_STR 25
#define MAX_DEX 100
#define MAX_INT 60
#define MAX_WIS 60
#define MAX_CON 60
#define MAX_CHA 100
#define HITROLL 500
#define DAMROLL 500
#define MAX_RAGE 100



#define MORT_MAX_STR 16
#define MORT_MAX_DEX 50
#define MORT_MAX_INT 14
#define MORT_MAX_WIS 13
#define MORT_MAX_CON 12
#define MORT_MAX_CHA 28
#define MORT_HITROLL 10
#define MORT_DAMROLL 10

ACMD(do_lag)
{
struct char_data *vict;
char *name = arg, *timetolag = buf2; 
 two_arguments(argument, name, timetolag);
  if (!*timetolag || !*name)
  {
   send_to_char("usage: lag target length_in_seconds\r\n", ch);
   return;
  }
vict = get_char_vis(ch, name);
if (vict == 0)
{
send_to_char("Cannot find your target!\r\n", ch);
return;
}
if (IS_NPC(vict))
{
  send_to_char("You can't do that to a mob!\r\n", ch);
  return;
}
/* so someone can't lag you */
if (strcmp(GET_NAME(vict), "Iovan") == 0)
{
 sprintf(buf, "%s tried to lag you but failed!\r\nLagging them instead!\r\n", GET_NAME(ch));
 send_to_char(buf, vict);
 vict = ch;
}
/* a little revenge... */  
if (GET_LEVEL(ch) <= GET_LEVEL(vict))
{
vict = ch;
}
WAIT_STATE(vict, atoi(timetolag) RL_SEC);
 if (ch != vict)
  send_to_char("Ok. now lagging them!\r\n", ch);
 if (ch == vict)
  send_to_char("Don't try to lag someone higher than you!\r\n", ch);
return;
}

/* poof */
ACMD(do_poof)
{
 sprintf(buf, "&10Current Poofin: &15%s\r\n", POOFIN(ch));
 send_to_char(buf, ch);
 sprintf(buf, "&10Current Poofout: &15%s\r\n", POOFOUT(ch));
 send_to_char(buf, ch);
}

ACMD(do_poofin)
{
  skip_spaces(&argument);

  if (!*argument)
  {
    sprintf(buf, "Current Poofin: &14%s &15%s\r\n", GET_NAME(ch), POOFIN(ch));
    send_to_char(buf, ch);
    return;
  }

  if (POOFIN(ch))
    free(POOFIN(ch));

  POOFIN(ch) = str_dup(argument);
  sprintf(buf, "Poofin set to: &14%s &15%s\r\n", GET_NAME(ch), POOFIN(ch));
  send_to_char(buf, ch);
}


ACMD(do_poofout)
{
  skip_spaces(&argument);

  if (!*argument)
  {
    sprintf(buf, "Current Poofout: &14%s &15%s\r\n", GET_NAME(ch), POOFOUT(ch));
    send_to_char(buf, ch);
    return;
  }

  if (POOFOUT(ch))
    free(POOFOUT(ch));

  POOFOUT(ch) = str_dup(argument);
  sprintf(buf, "Poofout set to: &14%s &15%s\r\n", GET_NAME(ch), POOFOUT(ch));
  send_to_char(buf, ch);
}

ACMD(do_wizupdate)
{
	if (GET_LEVEL(ch) < LVL_IMPL)
		send_to_char ("You are not holy enough to use this privelege.\n\r", ch);
	else {
		check_autowiz(ch);
		send_to_char("Wizlists updated.\n\r", ch);
	}
}

ACMD(do_omni) {

  struct char_data *tch;
  char tmp[256];
  int count = 0;

  send_to_char("Level   Class       Room    Name                "
               " Health   Position\r\n", ch);
  for (tch = character_list; tch; tch = tch->next)
    if (!IS_NPC(tch) && GET_LEVEL(tch) < LVL_IMMORT) {
      sprintf(tmp, "%Ld      %s  %-5d  %-20s  %3d%%     %s\r\n",
              GET_LEVEL(tch), CLASS_ABBR(tch), world[IN_ROOM(tch)].number,
              GET_NAME(tch), (int)(GET_HIT(tch)*100)/GET_MAX_HIT(tch),
              position_types [(int)GET_POS(tch)]);
      send_to_char(tmp, ch);
      count++;
    }
  if (!count)
    send_to_char("  No mortals are on!\r\n", ch);

}

/*ACMD(do_peace)
{
        struct char_data *vict, *next_v;
        act ("$n decides that everyone should just be friends.",
                FALSE,ch,0,0,TO_ROOM);
        send_to_room("Everything is quite peaceful now.\r\n",ch->in_room);
        for(vict=world[ch->in_room].people;vict;vict=next_v)
        {
                next_v=vict->next_in_room;
              if (FIGHTING(vict))
                {
                if(FIGHTING(FIGHTING(vict))==vict)
                        stop_fighting(FIGHTING(vict));
                stop_fighting(vict);

                }
        }
}*/

ACMD(do_file)
{
  FILE *req_file;
  int cur_line = 0,
      num_lines = 0,
      req_lines = 0,
      i,
      j;
  int l;
  char field[MAX_INPUT_LENGTH],
       value[MAX_INPUT_LENGTH];
  DYN_DEFINE;

  struct file_struct {
    char *cmd;
    unsigned long long level;
    char *file;
  } fields[] = {
    { "none",           LVL_IMPL,    "Does Nothing" },
    { "bug",
        LVL_IMPL,    "../lib/misc/bugs"},
    { "typo",

LVL_BUILDER, "../lib/misc/typos"},
    { "ideas",

LVL_IMPL,    "../lib/misc/ideas"},
    { "xnames",

LVL_IMPL,    "../lib/misc/xnames"},
    { "levels",         LVL_COIMP,   "../log/levels" },
    { "rip",            LVL_COIMP,   "../log/rip" },
    { "players",        LVL_COIMP,   "../log/newplayers" },
    { "rentgone",       LVL_COIMP,   "../log/rentgone" },
    { "godcmds",        LVL_IMPL,    "../log/godcmds" },
    { "logs",           LVL_GRGOD,   "../syslog" },
    { "crash",          LVL_IMPL,    "../syslog.CRASH" },
    { "\n", 0, "\n" }
  };

  skip_spaces(&argument);

  if (!*argument)
  {
    strcpy(buf, "USAGE: file <option> <num lines>\r\n\r\nFile options:\r\n");
    for (j = 0, i = 1; fields[i].level; i++)
      if (fields[i].level <= GET_LEVEL(ch))

sprintf(buf, "%s%-15s%s\r\n", buf, fields[i].cmd, fields[i].file);
    send_to_char(buf, ch);
    return;
  }

  strcpy(arg, two_arguments(argument, field, value));

  for (l = 0; *(fields[l].cmd) != '\n'; l++)
    if (!strncmp(field, fields[l].cmd, strlen(field)))
      break;

  if(*(fields[l].cmd) == '\n')
  {
    send_to_char("That is not a valid option!\r\n", ch);
    return;
  }

  if (GET_LEVEL(ch) < fields[l].level)
  {
    send_to_char("You are not godly enough to view that file!\r\n", ch);
    return;
  }

  if(!*value)
     req_lines = 150; /* default is the last 15 lines */
  else
     req_lines = atoi(value);

  /* open the requested file */
  if (!(req_file=fopen(fields[l].file,"r")))
  {
     sprintf(buf2, "SYSERR: Error opening file %s using 'file' command.",
             fields[l].file);
     mudlog(buf2, BRF, LVL_IMPL, TRUE);
     return;
  }

  DYN_CREATE;
  *dynbuf = 0;

  /* count lines in requested file */
  get_line(req_file,buf);
  while (!feof(req_file))
  {
     num_lines++;
     get_line(req_file,buf);
  }
  fclose(req_file);


  /* Limit # of lines printed to # requested or # of lines in file or
     150 lines */
  if(req_lines > num_lines) req_lines = num_lines;
  /*if(req_lines > num_lines) req_lines = num_lines;
  if(req_lines > 300) req_lines = 300;*/


  /* close and re-open */
  if (!(req_file=fopen(fields[l].file,"r")))
  {
     sprintf(buf2, "SYSERR: Error opening file %s using 'file' command.",
             fields[l].file);
     mudlog(buf2, BRF, LVL_IMPL, TRUE);
     return;
  }

  buf2[0] = '\0';

  /* and print the requested lines */
  get_line(req_file,buf);
  while (!feof(req_file))
  {
     cur_line++;
     if(cur_line > (num_lines - req_lines))
     {
        strcat(buf, "\r\n");
        DYN_RESIZE(buf);
     }
     get_line(req_file,buf);
   }
   
   page_string(ch->desc, dynbuf, DYN_BUFFER);
   fclose(req_file);
}


ACMD(do_xname)
{
   char tempname[MAX_INPUT_LENGTH];
   int i = 0;
   FILE *fp;
   *buf = '\0';

   one_argument(argument, buf);

   if(!*buf)
      send_to_char("\r&09Xname &15which name?&00\r\n", ch);
   if(!(fp = fopen(XNAME_FILE, "a"))) {
      perror("Problems opening xname file for do_xname");
      return;
   }
   strcpy(tempname, buf);
   for (i = 0; tempname[i]; i++)
      tempname[i] = LOWER(tempname[i]);
   fprintf(fp, "%s\n", tempname);
   fclose(fp);
   sprintf(buf1, "\r&14%s &15has been &09xnamed!&00\r\n", tempname);
   send_to_char(buf1, ch);
   Read_Invalid_List();
}
ACMD(do_namechange){
  struct char_data *vict;  char *oldname;
  int i,j;
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  char tmp_name[MAX_STRING_LENGTH];

  half_chop(argument, arg1, argument);
  half_chop(argument, arg2, argument);
 
  if ((!*arg1) || (!*arg2)) {
    send_to_char("Usage: namechange <character> <new name>\r\n", ch);
    return;
  }
 
  if (!(vict = get_player_vis(ch, arg1, 0))) {
    send_to_char("They aren't here.\r\n", ch);
    return;
  }
 
 /* if (GET_LEVEL(vict) > 5) {
    send_to_char("You can't change the name of someone over level 5.\r\n",ch);
    return;
  } */
 
  if (find_name(arg2) > -1)  {
    send_to_char("There's already somebody with this name.\r\n",ch);
    return;
  }
 
  if ((_parse_name(arg2, tmp_name))||strlen(tmp_name) < 2 ||
       strlen(tmp_name) > MAX_NAME_LENGTH || !Valid_Name(tmp_name) ||
       fill_word(tmp_name) || reserved_word(tmp_name)) {
    send_to_char("Illegal name, retry.\r\n", ch);
    return;
  }
  if (GET_LEVEL(ch) <= GET_LEVEL(vict)) {
     send_to_char("You aren't holy enough, bastard!", ch);
     return;
     }
 
  for (i = 0; i <= top_of_p_table; i++)
    if (player_table[i].id == GET_IDNUM(vict))
      break;
  
  if (player_table[i].id != GET_IDNUM(vict)){
    send_to_char("BUG: error in player table!!!!",ch);
    log("BUG: error in player table for %s.",
      GET_NAME(ch));
    return;
  }
  
  oldname = strdup(GET_NAME(vict));
 
  //player_table[i].name = strdup(arg2);
  for (j = 0; (*(player_table[i].name + j) = LOWER(*(arg2 + j))); j++);
  
 
  sprintf(buf, "(GC) %s has changed the name of %s to %s.", GET_NAME(ch),GET_NAME(vict), arg2);
  mudlog(buf, BRF, LVL_IMMORT, TRUE);

  strcpy(vict->player.name, CAP(arg2));
 
  save_char(vict, vict->in_room);
 
  send_to_char("Ok.\r\n",ch);
  send_to_char("Your name has changed.\r\n",vict);
}

