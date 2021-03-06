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
extern int reboot_time;
extern int reboot_state;
extern int interest_state;
extern int interest_time;
extern time_t boot_time;
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
extern char *syslog;
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
ACMD(do_clanwho);
ACMD(do_battlelust);
ACMD(do_rage);
ACMD(do_detect);
ACMD(do_flood);
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
void list_all_char(struct char_data * i, struct char_data * ch, int num);
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
    if (IS_OBJ_STAT(object, ITEM_GLOW)) {
      strcat(buf, " (&00&02Glowing&00)");
      found = TRUE;
    }
    if (IS_OBJ_STAT(object, ITEM_DIVINE)) {
      strcat(buf, " (&00&04DIVINE&00)");
      found = TRUE;
    }
    if (IS_OBJ_STAT(object, ITEM_MEDIDIVINE)) {
      strcat(buf, " (&00&02MIDI-DIVINE&00)");
      found = TRUE;
    }
    if (IS_OBJ_STAT(object, ITEM_UBERDIVINE)) {
      strcat(buf, " (&00&01ULTRA-DIVINE)");
      found = TRUE;
    }
    if (IS_OBJ_STAT(object, ITEM_HUM)) {
      strcat(buf, " (&00&06Humming&00)");
      found = TRUE;
    }
  }
  strcat(buf, "\r\n");
  page_string(ch->desc, buf, TRUE);
}

   void list_scanned_chars(struct char_data * list, struct char_data * ch, int distance, 
int door)
{
  const char *how_far[] = {
    "close by",
    "a ways off",
    "far off to the"
  };

  struct char_data *i;
  int count = 0;
  *buf = '\0';

/* this loop is a quick, easy way to help make a grammatical sentence
   (i.e., "You see x, x, y, and z." with commas, "and", etc.) */

  for (i = list; i; i = i->next_in_room)

/* put any other conditions for scanning someone in this if statement -
   i.e., if (CAN_SEE(ch, i) && condition2 && condition3) or whatever */

    if (CAN_SEE(ch, i))
     count++;

  if (!count)
    return;

  for (i = list; i; i = i->next_in_room) {

/* make sure to add changes to the if statement above to this one also, using
   or's to join them.. i.e., 
   if (!CAN_SEE(ch, i) || !condition2 || !condition3) */

    if (!CAN_SEE(ch, i))
      continue; 
    if (!*buf)
      sprintf(buf, "&15You see &00%s", GET_NAME(i));
    else 
      sprintf(buf, "%s%s", buf, GET_NAME(i));
    if (--count > 1)
      strcat(buf, "&15,&00 ");
    else if (count == 1)
      strcat(buf, " &15and&00 ");
    else {
      sprintf(buf2, "&15 %s %s.\r\n", how_far[distance], dirs[door]);
      strcat(buf, buf2);      
    }
    
  }
  send_to_char(buf, ch);
}


void list_obj_to_char(struct obj_data * list, struct char_data * ch, int mode,
		           int show)
{

  struct obj_data *i, *j;
  char buf[10];
  bool found;
  int num;

  found = FALSE;
  for (i = list; i; i = i->next_content) {
      num = 0;
      for (j = list; j != i; j = j->next_content)

if (j->item_number==NOTHING) {

    if(strcmp(j->short_description,i->short_description)==0) break;

}

else if (j->item_number==i->item_number) break;
      if (j!=i) continue;
      for (j = i; j; j = j->next_content)

if (j->item_number==NOTHING) {

    if(strcmp(j->short_description,i->short_description)==0) num++;

  }

else if (j->item_number==i->item_number) num++;

      if (CAN_SEE_OBJ(ch, i)) {

  if (num!=1) {


sprintf(buf,"&16(&14%2i&16)&00 ",num);


send_to_char(buf,ch);

    }

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

  strcpy(buf, PERS(i, ch));
  CAP(buf);
 if (IS_NPC(i)) {
  if (GET_HIT(i) >= GET_MAX_HIT(i))
    strcat(buf, " &00is in excellent condition.\r\n");
   else if (GET_HIT(i) >= GET_MAX_HIT(i) * .9)
    strcat(buf, " &00has a few scratches.\r\n");
   else if (GET_HIT(i) >= GET_MAX_HIT(i) * .8)
    strcat(buf, " &00has some small wounds and bruises.\r\n");
   else if (GET_HIT(i) >= GET_MAX_HIT(i) * .5)
    strcat(buf, " &00has quite a few wounds.\r\n");
   else if (GET_HIT(i) >= GET_MAX_HIT(i) * .3)
    strcat(buf, " &00has some big nasty wounds and scratches.\r\n");
   else if (GET_HIT(i) >= GET_MAX_HIT(i) * .15)
    strcat(buf, " &00looks pretty hurt.\r\n");
   else if (GET_HIT(i) >= 0)
    strcat(buf, " &00is bleeding awfully from big wounds.\r\n");
  
  }
  else if (!IS_NPC(i)) {
   if (GET_HIT(i) >= GET_MAX_HIT(i))
    strcat(buf, " &00is in excellent condition.\r\n");
   else if (GET_HIT(i) >= GET_MAX_HIT(i) * .9)
    strcat(buf, " &00has a few scratches.\r\n");
   else if (GET_HIT(i) >= GET_MAX_HIT(i) * .8)
    strcat(buf, " &00has some small wounds and bruises.\r\n");
   else if (GET_HIT(i) >= GET_MAX_HIT(i) * .5)
    strcat(buf, " &00has quite a few wounds.\r\n");
   else if (GET_HIT(i) >= GET_MAX_HIT(i) * .3)
    strcat(buf, " &00has some big nasty wounds and scratches.\r\n");
   else if (GET_HIT(i) >= GET_MAX_HIT(i) * .15)
    strcat(buf, " &00looks pretty hurt.\r\n");
   else if (GET_HIT(i) >= 0)
    strcat(buf, " &00is bleeding awfully from big wounds.\r\n");
  }
  send_to_char(buf, ch);
}


void look_at_char(struct char_data * i, struct char_data * ch)
{
  int j, found;
  struct obj_data *tmp_obj;

  if (!ch->desc)
    return;
if (!IS_NPC(i)) {
 if (PLR_FLAGGED(i, PLR_MAJINIZE)) {
    send_to_char("&16[&14Forehead&15: &00&07'&13M&00&07'&16]&00\r\n", ch);
    }
  
 if (IS_saiyan(i) || IS_HALF_BREED(i)) {
  if (!PLR_FLAGGED(i, PLR_trans1) && i->player.eye == EYE_WHITE)
     send_to_char("&16[&14Eye Color&15: &00&07White&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && i->player.eye == EYE_BLACK)
     send_to_char("&16[&14Eye Color&15: &16Black&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && i->player.eye == EYE_BROWN)
     send_to_char("&16[&14Eye Color&15: &00&03Brown&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && i->player.eye == EYE_GREEN)
     send_to_char("&16[&14Eye Color&15: &10Green&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && i->player.eye == EYE_BLUE)
     send_to_char("&16[&14Eye Color&15: &12Blue&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && i->player.eye == EYE_PURPLE)
     send_to_char("&16[&14Eye Color&15: &00&04Purple&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && i->player.eye == EYE_RED)
     send_to_char("&16[&14Eye Color&15: &09Red&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && i->player.eye == EYE_YELLOW)
     send_to_char("&16[&14Eye Color&15: &11Yellow&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && i->player.eye == EYE_ICYBLUE)
     send_to_char("&16[&14Eye Color&15: &14Icy blue&16]&00\r\n", ch);
  }
 if (IS_Human(i) || IS_demon(i) || IS_icer(i) || IS_Namek(i) || IS_KONATSU(i) || IS_KAI(i) || IS_BIODROID(i) || IS_ANDROID(i) || IS_TRUFFLE(i) || IS_MUTANT(i) || IS_MAJIN(i) || IS_KANASSAN(i) || IS_ANGEL(i)) {
  if (i->player.eye == EYE_WHITE)
     send_to_char("&16[&14Eye Color&15: &00&07White&16]&00\r\n", ch);
  else if (i->player.eye == EYE_BLACK)
     send_to_char("&16[&14Eye Color&15: &16Black&16]&00\r\n", ch);
  else if (i->player.eye == EYE_BROWN)
     send_to_char("&16[&14Eye Color&15: &00&03Brown&16]&00\r\n", ch);
  else if (i->player.eye == EYE_GREEN)
     send_to_char("&16[&14Eye Color&15: &10Green&16]&00\r\n", ch);
  else if (i->player.eye == EYE_BLUE)
     send_to_char("&16[&14Eye Color&15: &12Blue&16]&00\r\n", ch);
  else if (i->player.eye == EYE_PURPLE)
     send_to_char("&16[&14Eye Color&15: &00&04Purple&16]&00\r\n", ch);
  else if (i->player.eye == EYE_RED)
     send_to_char("&16[&14Eye Color&15: &09Red&16]&00\r\n", ch);
  else if (i->player.eye == EYE_YELLOW)
     send_to_char("&16[&14Eye Color&15: &11Yellow&16]&00\r\n", ch);
  else if (i->player.eye == EYE_ICYBLUE)
     send_to_char("&16[&14Eye Color&15: &14Icy blue&16]&00\r\n", ch);
  }

  if (i->player.aura == AURA_WHITE)
     send_to_char("&16[&14Skin&15: &00&07White&16]&00\r\n", ch);
  else if (i->player.aura == AURA_BLACK)
     send_to_char("&16[&14Skin&15: &00&03Black&16]&00\r\n", ch);
  else if (i->player.aura == AURA_BLUE)
     send_to_char("&16[&14Skin&15: &12Blue&16]&00\r\n", ch);
  else if (i->player.aura == AURA_GREEN)
     send_to_char("&16[&14Skin&15: &10Green&16]&00\r\n", ch);
  else if (i->player.aura == AURA_PURPLE)
     send_to_char("&16[&14Skin&15: &13Pink&16]&00\r\n", ch);
  else if (i->player.aura == AURA_PINK)
     send_to_char("&16[&14Skin&15: &00&05Purple&16]&00\r\n", ch);
  else if (i->player.aura == AURA_RED)
     send_to_char("&16[&14Skin&15: &09Red&16]&00\r\n", ch);  
  else if (i->player.aura == AURA_YELLOW)
     send_to_char("&16[&14Skin&15: &11Tan&16]&00\r\n", ch);
  if (IS_MAJIN(i) && PLR_FLAGGED(i, PLR_FORELOCK))
     send_to_char("&16[&14Forelock&15: &10Yes&16]&00\r\n", ch);
  else if (IS_MAJIN(i))
     send_to_char("&16[&14Forelock&15: &09No&16]&00\r\n", ch);
  if (PLR_FLAGGED(i, PLR_trans1) && (!PLR_FLAGGED(i, PLR_trans4) && !PLR_FLAGGED(i, PLR_trans3)) && (IS_saiyan(i) || IS_HALF_BREED(i))) 
     send_to_char("&16[&14Hair Length&15: Spikey&16]\r\n&16[&14Hair Color&15: &11G&00&03o&11l&00&03d&16]&00\r\n&16[&14Eye&15: &10E&00&02m&10e&00&02r&10a&00&02l&10d&16]&00\r\n", ch);
  if (PLR_FLAGGED(i, PLR_trans4) && IS_saiyan(i))
     send_to_char("&16[&14Hair Color&15: &16Black &15with &09Red &00&03fur&00]&00\r\n", ch);
  if (PLR_FLAGGED(i, PLR_trans3) && !PLR_FLAGGED(i, PLR_trans4) && (IS_saiyan(i) || IS_HALF_BREED(i)))
     send_to_char("&16[&14Hair Color&15: &11G&00&03o&11l&00&03d&16]\r\n&16[&14Eye&15: &10E&00&02m&10e&00&02r&10a&00&02l&10d&16]&00\r\n&16[&14Hair Length&15: Very Long Spikey&00]&00\r\n", ch);
  if (PLR_FLAGGED(i, PLR_trans4) && IS_saiyan(i))
     send_to_char("&16[&14Hair Length&15: Long Spikey&00]&00\r\n&16[&14Eye&15: &09R&16e&09d &09R&16i&09m&16m&09e&16d ]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairc == HAIRC_WHITE)
     send_to_char("&16[&14Hair Color&15: &15Silver&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairc == HAIRC_BLACK)
     send_to_char("&16[&14Hair Color&15: &16Black&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairc == HAIRC_BROWN)
     send_to_char("&16[&14Hair Color&15: &00&03Brown&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairc == HAIRC_PURPLE)
     send_to_char("&16[&14Hair Color&15: &00&05Purple&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairc == HAIRC_RED)
     send_to_char("&16[&14Hair Color&15: &09Red&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairc == HAIRC_GREEN)
     send_to_char("&16[&14Hair Color&15: &10Green&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairc == HAIRC_BLUE)
     send_to_char("&16[&14Hair Color&15: &12Blue&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairc == HAIRC_YELLOW)
     send_to_char("&16[&14Hair Color&15: &11Blonde&16]&00\r\n", ch);
 if (IS_KANASSAN(i)) {
  if (i->player.hairc == HAIRC_BLUE)
     send_to_char("&16[&14Scale Color&15: &12Blue&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_WHITE)
     send_to_char("&16[&14Scale Color&15: &15Silver&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_BLACK)
     send_to_char("&16[&14Scale Color&15: &16Black&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_YELLOW)
     send_to_char("&16[&14Scale Color&15: &11Blonde&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_BROWN)
     send_to_char("&16[&14Scale Color&15: &00&03Brown&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_PURPLE)
     send_to_char("&16[&14Scale Color&15: &00&05Purple&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_RED)
     send_to_char("&16[&14Scale Color&15: &09Red&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_GREEN)
     send_to_char("&16[&14Scale Color&15: &10Green&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_NONE)
     send_to_char("&16[&14Scale Color&15: &15Albino&16]&00\r\n", ch);
  }
 if (IS_Human(i) || IS_demon(i) || IS_icer(i) || IS_KONATSU(i) || IS_KAI(i) || IS_BIODROID(i) || IS_ANDROID(i) || IS_TRUFFLE(i) || IS_MUTANT(i) || IS_ANGEL(i)) {
  if (i->player.hairc == HAIRC_BLUE)
     send_to_char("&16[&14Hair Color&15: &12Blue&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_WHITE)
     send_to_char("&16[&14Hair Color&15: &15Silver&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_BLACK)
     send_to_char("&16[&14Hair Color&15: &16Black&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_YELLOW)
     send_to_char("&16[&14Hair Color&15: &11Blonde&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_BROWN)
     send_to_char("&16[&14Hair Color&15: &00&03Brown&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_PURPLE)
     send_to_char("&16[&14Hair Color&15: &00&05Purple&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_RED)
     send_to_char("&16[&14Hair Color&15: &09Red&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_GREEN)
     send_to_char("&16[&14Hair Color&15: &10Green&16]&00\r\n", ch);
  else if (i->player.hairc == HAIRC_NONE)
     send_to_char("&16[&14Hair Color&15: &15None&16]&00\r\n", ch);
  }
  if (IS_KANASSAN(i)) {
   if (i->player.hairl == HAIRL_V_SHORT)
     send_to_char("&16[&14Scale Length&15: Very Short&16]&00\r\n", ch);
   else if (i->player.hairl == HAIRL_SHORT)
     send_to_char("&16[&14Scale Length&15: Short&16]&00\r\n", ch);
   else if (i->player.hairl == HAIRL_MEDIUM)
     send_to_char("&16[&14Scale Length&15: Medium&16]&00\r\n", ch);
   else if (i->player.hairl == HAIRL_LONG)
     send_to_char("&16[&14Scale Length&15: Long&16]&00\r\n", ch);
   else if (i->player.hairl == HAIRL_V_LONG)
     send_to_char("&16[&14Scale Length&15: Very Long&16]&00\r\n", ch);
   else if (i->player.hairl == HAIRL_BALD)
     send_to_char("&16[&14Scale Length&15: Scaleless&16]&00\r\n", ch);
   else if (i->player.hairl == HAIRL_SSPIKE)
     send_to_char("&16[&14Scale Length&15: Short Spikey&16]&00\r\n", ch);
   else if (i->player.hairl == HAIRL_LSPIKE)
     send_to_char("&16[&14Scale Length&15: Long Spikey&16]&00\r\n", ch);
   else if (i->player.hairl == HAIRL_MSPIKE)
     send_to_char("&16[&14Scale Length&15: Medium Spikey&16]&00\r\n", ch);
  }
  if (GET_CLASS(i) == CLASS_MAJIN && i->player.hairl == HAIRL_V_SHORT)
     send_to_char("&16[&14Forelock Length&15: Very Short&16]&00\r\n", ch);
  else if (GET_CLASS(i) == CLASS_MAJIN && i->player.hairl == HAIRL_SHORT)
     send_to_char("&16[&14Forelock Length&15: Short&16]&00\r\n", ch);
  else if (GET_CLASS(i) == CLASS_MAJIN && i->player.hairl == HAIRL_MEDIUM)
     send_to_char("&16[&14Forelock Length&15: Medium&16]&00\r\n", ch);
  else if (GET_CLASS(i) == CLASS_MAJIN && i->player.hairl == HAIRL_LONG)
     send_to_char("&16[&14Forelock Length&15: Long&16]&00\r\n", ch);
  else if (GET_CLASS(i) == CLASS_MAJIN && i->player.hairl == HAIRL_V_LONG)
     send_to_char("&16[&14Forelock Length&15: Very Long&16]&00\r\n", ch);
  else if (GET_CLASS(i) == CLASS_MAJIN && i->player.hairl == HAIRL_BALD)
     send_to_char("&16[&14Forelock Length&15: Stubbey&16]&00\r\n", ch);
  else if (GET_CLASS(i) == CLASS_MAJIN && i->player.hairl == HAIRL_SSPIKE)
     send_to_char("&16[&14Forelock Length&15: Short Thin&16]&00\r\n", ch);
  else if (GET_CLASS(i) == CLASS_MAJIN && i->player.hairl == HAIRL_LSPIKE)
     send_to_char("&16[&14Forelock Length&15: Long Thin&16]&00\r\n", ch);
  else if (GET_CLASS(i) == CLASS_MAJIN && i->player.hairl == HAIRL_MSPIKE)
     send_to_char("&16[&14Forelock Length&15: Medium Thin&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairl == HAIRL_V_SHORT)
     send_to_char("&16[&14Hair Length&15: Very Short&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairl == HAIRL_SHORT)
     send_to_char("&16[&14Hair Length&15: Short&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairl == HAIRL_MEDIUM)
     send_to_char("&16[&14Hair Length&15: Medium&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairl == HAIRL_LONG)
     send_to_char("&16[&14Hair Length&15: Long&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairl == HAIRL_V_LONG)
     send_to_char("&16[&14Hair Length&15: Very Long&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairl == HAIRL_BALD)
     send_to_char("&16[&14Hair Length&15: Bald&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairl == HAIRL_SSPIKE)
     send_to_char("&16[&14Hair Length&15: Short Spikey&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairl == HAIRL_MSPIKE)
     send_to_char("&16[&14Hair Length&15: Medium Spikey&16]&00\r\n", ch);
  else if (!PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)) && i->player.hairl == HAIRL_LSPIKE)
     send_to_char("&16[&14Hair Length&15: Long Spikey&16]&00\r\n", ch);
 if (IS_Namek(i)) {
  if (i->player.hairl == HAIRL_V_SHORT)
     send_to_char("&16[&14Antennae Length&15: Very Short&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_SHORT)
     send_to_char("&16[&14Antennae Length&15: Short&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_MEDIUM)
     send_to_char("&16[&14Antennae Length&15: Medium&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_LONG)
     send_to_char("&16[&14Antennae Length&15: Long&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_V_LONG)
     send_to_char("&16[&14Antennae Length&15: Very Long&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_BALD)
     send_to_char("&16[&14Antennae Length&15: Bald&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_SSPIKE)
     send_to_char("&16[&14Antennae Length&15: Short Spikey&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_LSPIKE)
     send_to_char("&16[&14Antennae Length&15: Long Spikey&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_MSPIKE)
     send_to_char("&16[&14Antennae Length&15: Medium Spikey&16]&00\r\n", ch);
 }
 if (IS_Human(i) || IS_demon(i) || IS_icer(i) || IS_KONATSU(i) || IS_KAI(i) || IS_BIODROID(i) || IS_ANDROID(i) || IS_TRUFFLE(i) || IS_MUTANT(i) || IS_ANGEL(i)) {
  if (i->player.hairl == HAIRL_V_SHORT)
     send_to_char("&16[&14Hair Length&15: Very Short&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_SHORT)
     send_to_char("&16[&14Hair Length&15: Short&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_MEDIUM)
     send_to_char("&16[&14Hair Length&15: Medium&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_LONG)
     send_to_char("&16[&14Hair Length&15: Long&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_V_LONG)
     send_to_char("&16[&14Hair Length&15: Very Long&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_BALD)
     send_to_char("&16[&14Hair Length&15: Bald&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_SSPIKE)
     send_to_char("&16[&14Hair Length&15: Short Spikey&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_LSPIKE)
     send_to_char("&16[&14Hair Length&15: Long Spikey&16]&00\r\n", ch);
  else if (i->player.hairl == HAIRL_MSPIKE)
     send_to_char("&16[&14Hair Length&15: Medium Spikey&16]&00\r\n", ch);
  }
  if (IS_saiyan(i) && PLR_FLAGGED(i, PLR_STAIL)) 
   send_to_char("&16[&14Tail&15: &10Yes&16]&00\r\n", ch);  
  else if (IS_saiyan(i))
   send_to_char("&16[&14Tail&15: &09No&16]&00\r\n", ch);
  if (IS_HALF_BREED(i) && PLR_FLAGGED(i, PLR_STAIL))
   send_to_char("&16[&14Tail&15: &10Yes&16]&00\r\n", ch);
  else if (IS_HALF_BREED(i))
   send_to_char("&16[&14Tail&15: &09No&16]&00\r\n", ch);
  if (IS_icer(i) && PLR_FLAGGED(i, PLR_TAIL))
   send_to_char("&16[&14Tail&15: &10Yes&16]&00\r\n", ch);
  else if (IS_icer(i))
   send_to_char("&16[&14Tail&15: &09No&16]&00\r\n", ch);
  if (IS_BIODROID(i) && PLR_FLAGGED(i, PLR_TAIL))
   send_to_char("&16[&14Tail&15: &10Yes&16]&00\r\n", ch);
  else if (IS_BIODROID(i))
   send_to_char("&16[&14Tail&15: &09No&16]&00\r\n", ch);
}
   if (!IS_NPC(i)) {
    if (!PLR_FLAGGED(i, PLR_RARM)) {
    send_to_char("&16[&14Right Arm&15:  &09Severed&16]\r\n", ch);
    }
    else if (PLR_FLAGGED(i, PLR_RARM)) {
    send_to_char("&16[&14Right Arm&15:  &11Perfect&16]\r\n", ch);
    }
    if (PLR_FLAGGED(i, PLR_LARM)) {
    send_to_char("&16[&14Left Arm&15:  &11Perfect&16]\r\n", ch);
    }
    else if (!PLR_FLAGGED(i, PLR_LARM)) {
    send_to_char("&16[&14Left Arm&15:  &09Severed&16]\r\n", ch);
    }
    if (PLR_FLAGGED(i, PLR_RLEG)) {
    send_to_char("&16[&14Right Leg&15:  &11Perfect&16]\r\n", ch);
    }
    else if (!PLR_FLAGGED(i, PLR_RLEG)) {
    send_to_char("&16[&14Right Leg&15:  &09Severed&16]\r\n", ch);
    }
    if (PLR_FLAGGED(i, PLR_LLEG)) {
    send_to_char("&16[&14Left Leg&15:  &11Perfect&16]\r\n", ch);
    }
    else if (!PLR_FLAGGED(i, PLR_LLEG)) {
    send_to_char("&16[&14Left Leg&15:  &09Severed&16]\r\n", ch);
    }
  }
   send_to_char("\n", ch);
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
  if (ch != i && (IS_KAI(ch) || GET_LEVEL(ch) >= LVL_IMMORT)) {
    found = FALSE;
    if (GET_MAX_HIT(i) > GET_MAX_HIT(ch) * 10 && GET_LEVEL(ch) <= 999999) {
     act("\r\nThey are too powerful for you to read!", FALSE, i, 0, ch, TO_VICT);
     return;
     }
    else
    act("\r\nYou read what $s inventory is:", FALSE, i, 0, ch, TO_VICT);
    /*for (tmp_obj = i->carrying; tmp_obj; tmp_obj = tmp_obj->next_content) {
      if (CAN_SEE_OBJ(ch, tmp_obj) && (number(0, 1) < GET_LEVEL(ch))) {
       list_obj_to_char(i->carrying, ch, 1, TRUE);
       show_obj_to_char(tmp_obj, ch, 1);     
       found = TRUE;
      }
    }*/
     list_obj_to_char(i->carrying, ch, 1, TRUE);
  }
}


void list_all_char(struct char_data * i, struct char_data * ch, int num)
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
    " stands here.",
    " is flying here."
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

            if (num > 1)
            {
                while ((buf[strlen(buf)-1]=='\r') ||
                       (buf[strlen(buf)-1]=='\n') ||
                       (buf[strlen(buf)-1]==' ')) {
                    buf[strlen(buf)-1] = '\0';
                }
                sprintf(buf2,"&16[&14%d&16]&00 ", num);
                strcat(buf, buf2);
            }

    strcat(buf, i->player.long_descr);
    send_to_char(buf, ch);

    if (AFF_FLAGGED(i, AFF_BARRIER))
      act("&15$e is engulfed in a &12globe &15of &10energy&15.&00", FALSE, i, 0, ch, TO_VICT);
    if (PRF2_FLAGGED(i, PRF2_FISHING))
      act("&15...$e is fishing with a lazy smile.&00", FALSE, i, 0, ch, TO_VICT);
    if (AFF_FLAGGED(i, AFF_SOLAR_FLARE) && !AFF_FLAGGED(i, AFF_INFRAVISION))
      act("&15$e is groping around blindly!&00", FALSE, i, 0, ch, TO_VICT);
    if (AFF_FLAGGED(i, AFF_INFRAVISION))
      act("&15$e has glowing &09r&00&01e&09d&15 eyes!&00", FALSE, i, 0, ch, TO_VICT);
    return;
  }
  if (IS_NPC(i) && GET_POS(i) != POS_FIGHTING && GET_POS(i) != GET_DEFAULT_POS(i)) {

    if (AFF_FLAGGED(i, AFF_INVISIBLE))
      strcpy(buf, "*");
    else
      *buf = '\0';
      if (num > 1)
            {
                while ((buf[strlen(buf)-1]=='\r') ||
                       (buf[strlen(buf)-1]=='\n') ||
                       (buf[strlen(buf)-1]==' ')) {
                    buf[strlen(buf)-1] = '\0';
                }
                sprintf(buf2,"&16[&14%d&16]&00 ", num);
                strcat(buf, buf2);
            }
       strcat(buf, i->player.short_descr);
       strcat(buf, positions[(int) GET_POS(i)]);
       strcat(buf, "\r\n");
       send_to_char(buf, ch);
       return;
    }
    else if (IS_NPC(i)) {
     if (AFF_FLAGGED(i, AFF_INVISIBLE))
      strcpy(buf, "*");
     else
      *buf = '\0';
      if (num > 1)
            {
                while ((buf[strlen(buf)-1]=='\r') ||
                       (buf[strlen(buf)-1]=='\n') ||
                       (buf[strlen(buf)-1]==' ')) {
                    buf[strlen(buf)-1] = '\0';
                }
                sprintf(buf2,"&16[&14%d&16]&00 ", num);
                strcat(buf, buf2);
            }
       strcat(buf, i->player.short_descr);
    }
/* Boost Look Flags */
  else if (PLR_FLAGGED(i, PLR_trans4) && (IS_MAJIN(i)))
    sprintf(buf, "&14%s&15, covered in &00&01m&00&05a&13s&15s&00i&01v&00&05e&15 muscles and &16b&15l&00a&15c&16k &00e&15l&16e&00&05c&13t&15r&13i&00&05c&16i&15t&00y&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans3) && (IS_MAJIN(i)))
    sprintf(buf, "&14%s&15, surrounded by &16b&15l&00a&15c&16k &00e&15l&16e&00&05c&13t&15r&13i&00&05c&16i&15t&00y&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_MAJIN(i)))
    sprintf(buf, "&14%s&15, with a dark &13p&00&05u&16r&15p&13l&00&05e&15 aura,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_MAJIN(i)))
    sprintf(buf, "&14%s&15, with a bright &13p&00&05u&16r&15p&13l&00&05e&15 aura,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_KANASSAN(i)))
    sprintf(buf, "&14%s&15, covered in &15o&16n&00&06y&15x &14s&00&06c&16al&00&06e&14s&15 and &16b&15l&00a&15c&16k &00e&15l&16e&00&06c&14tr&00&06i&16c&15i&15t&00y&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_KANASSAN(i)))
    sprintf(buf, "&14%s&15, with &12d&00&04a&16r&15k &10g&00&02r&10e&15e&00n &14s&00&06c&16al&00&06e&14s&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans4) && (IS_BIODROID(i)))
    sprintf(buf, "&14%s&15, surrounded by a &10d&00&02a&16r&15k &10g&00&02r&10e&15e&00n&15 aura and &10g&00&02r&10e&15e&00n &00e&15l&16e&00&02c&10t&15r&10i&00&02c&16i&15t&00y&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans3) && (IS_BIODROID(i)))
    sprintf(buf, "&14%s&15, covered with bright &10g&00&02r&10e&15e&00n &00e&15l&16e&00&02c&10t&15r&10i&00&02c&16i&15t&00y&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_BIODROID(i)))
    sprintf(buf, "&14%s&15, surrounded by a &10g&00&02r&10e&15e&00n&15 aura,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_BIODROID(i)))
    sprintf(buf, "&14%s&15, covered with &10g&00&02r&10e&15e&00n s&10l&00&02i&10m&00e&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_KONATSU(i)))
    sprintf(buf, "&14%s&15, &16almost completely &00&07p&15h&16as&15e&00&07d&16 out of this &10m&00&02a&16t&00&03er&16i&00&02a&10l &16plane,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_KONATSU(i)))
    sprintf(buf, "&14%s&15, &15with &14c&00&06u&16r&00&06r&14e&16n&00&06t&14s&15 of &00&07w&15h&16i&15t&00&07e&15 and &16b&15l&00&07a&16c&15k &14c&00&06r&00&03a&11c&16k&15li&00ng&15 about them&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans4) && (IS_demon(i)))
    sprintf(buf, "&14%s&15, &16with &11torrents &16of &00&01f&09l&00&01a&09m&00&01e&09s&11&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans3) && (IS_demon(i)))
    sprintf(buf, "&14%s&15, &16with &10bursts &16of &00&01f&09l&00&01a&09m&00&01e&09s &16around their body&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_demon(i)))
    sprintf(buf, "&14%s&15, &16with &00&01f&09i&00&01e&09r&00&01y &16bursts &00&07flowing &16up from around their body&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_demon(i)))
    sprintf(buf, "&14%s&15, &16with &00&01hot &00&07steam &16rising off of their body&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans3) && (IS_TRUFFLE(i)))
    sprintf(buf, "&14%s&15, &15with &00&07g&10l&00&02o&16w&00&02i&10n&00&07g &15implants all over their body&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_TRUFFLE(i)))
    sprintf(buf, "&14%s&15, &15with large &14B&00&06i&00&02o&16-&15T&00&02e&10c&16h&15 implants in their arms and legs&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_TRUFFLE(i)))
    sprintf(buf, "&14%s&15, &15with small &14B&00&06i&00&02o&16-&15T&00&02e&10c&16h &15implants in their arms and legs&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_MUTANT(i)))
    sprintf(buf, "&14%s&15, with &00&01m&00&05a&13s&15s&00i&00&01v&00&05e&15 muscles and &00r&15a&16z&00o&15r sharp claws,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_MUTANT(i)))
    sprintf(buf, "&14%s&15, with &15g&14l&00&06o&15w&00&03i&11n&15g &00&01r&09e&15d eyes,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans4) && (IS_icer(i)))
    sprintf(buf, "&14%s&15, &15with a &14l&00&06a&16r&00&06g&14e &13d&00&05a&16rk &00&07a&14u&00&06r&14a&15 covered body&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans3) && (IS_icer(i)))
    sprintf(buf, "&14%s&15, &15with a small &00&07s&15l&16e&00&06e&14k&15 body and &13d&00&05a&16rk &00&07a&14u&00&06r&14a&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_icer(i)))
    sprintf(buf, "&14%s&15, &15their head &14l&00&06on&14g&15 and &00&07l&15i&00&02z&10a&15r&00&07d&15 like&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_icer(i)))
    sprintf(buf, "&14%s&15, &15tall and with &09l&00&01a&16r&00&01g&09e &00&07h&15o&16r&15n&00&07s&15 coming out of their head&15,", i->player.name);
  else if (PRF_FLAGGED(i, PRF_OOZARU) && (IS_saiyan(i) || IS_HALF_BREED(i)))
    sprintf(buf, "&14%s&15, &00&03stomping around here as a &10large &09O&00&01o&16z&15a&00&01r&09u&15,", i->player.name);
  else if (PRF2_FLAGGED(i, PRF2_GO))
    sprintf(buf, "&14%s&15, towering as a majestic &11G&00&03o&11l&00&03d&11e&00&03n &09O&00&01o&16z&15a&00&01r&09u&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans4) && (IS_saiyan(i) || IS_HALF_BREED(i)))
    sprintf(buf, "&14%s&15, covered with &00&01c&09r&15i&00m&16s&00&01o&09n&15 fur and &00&01r&09e&15d rimmed eyes,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans3) && (IS_saiyan(i) || IS_HALF_BREED(i)))
    sprintf(buf, "&14%s&15, with long, spikey &11g&00&03o&11l&00&03d&11e&00&03n&15 hair and covered with &00&04b&12l&14u&00e &00e&15l&16e&00&04c&12t&14r&12i&00&04c&16i&15t&00y&15,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_saiyan(i) || IS_HALF_BREED(i)))
    sprintf(buf, "&14%s&15, covered with &00&04b&12l&14u&00e &00e&15l&16e&00&04c&12t&14r&12i&00&04c&16i&15t&00y&15 and a &11g&00&03o&11l&00&03d&11e&00&03n&15 aura,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_saiyan(i) || IS_HALF_BREED(i)))
    sprintf(buf, "&14%s&15, with spikey &11g&00&03o&11l&00&03d&11e&00&03n&15 hair and a &11g&00&03o&11l&00&03d&11e&00&03n&15 aura,", i->player.name);
/* SNAMEK Look Flags */
 else if (PLR_FLAGGED(i, PLR_trans4) && (IS_Namek(i)))
    sprintf(buf, "&14%s&15, covered in &10g&00&02r&10e&15e&00n&15 &00e&15l&16e&00&02c&10t&15r&10i&00&02c&16i&15t&00y&15 and an &00e&15m&10e&00&02r&00a&15l&10d&15 aura,", i->player.name);
 else if (PLR_FLAGGED(i, PLR_trans3) && (IS_Namek(i)))
    sprintf(buf, "&14%s&15, covered with a &10n&00&02e&15o&00n &10g&00&02r&10e&15e&00n&15 aura,", i->player.name);
 else if (PLR_FLAGGED(i, PLR_trans2) && (IS_Namek(i)))
    sprintf(buf, "&14%s&15, covered with &10g&00&02r&10e&15e&00n&15 &00e&15l&16e&00&02c&10t&15r&10i&00&02c&16i&15t&00y&15,", i->player.name);
 else if (PLR_FLAGGED(i, PLR_trans1) && (IS_Namek(i)))
    sprintf(buf, "&14%s&15, with a bright &10g&00&02r&10e&15e&00n&15 aura,", i->player.name);
/* SSJ Look Flags */
  else if (!str_cmp(i->player.name, "Oob") && PLR_FLAGGED(i, PLR_trans4) && (IS_Human(i)))
    sprintf(buf, "&14Uub&15, with sharp spikey hair and a &00b&15l&11i&00&03n&00d&15i&11n&00&03g &00w&15h&00i&15t&00e&15 aura,", i->player.name);
  else if (!str_cmp(i->player.name, "Oob") && PLR_FLAGGED(i, PLR_trans3) && (IS_Human(i)))
    sprintf(buf, "&14Uub&15, with &00&01m&00&05a&13s&15s&00i&01v&00&05e&15 muscles and a vibrant &00w&15h&00i&15t&00e&15 aura,", i->player.name);
  else if (!str_cmp(i->player.name, "Oob") && PLR_FLAGGED(i, PLR_trans2) && (IS_Human(i)))
    sprintf(buf, "&14Uub&15, with slightly spikey hair and a blazing &00w&15h&00i&15t&00e&15 aura,", i->player.name);
  else if (!str_cmp(i->player.name, "Oob") && PLR_FLAGGED(i, PLR_trans1) && (IS_Human(i)))
    sprintf(buf, "&14Uub&15, with a soft &00w&15h&00i&15t&00e&15 aura,&15", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans4) && (IS_Human(i)))
    sprintf(buf, "&14%s&15, with sharp spikey hair and a &00b&15l&11i&00&03n&00d&15i&11n&00&03g &00w&15h&00i&15t&00e&15 aura,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans3) && (IS_Human(i)))
    sprintf(buf, "&14%s&15, with &00&01m&00&05a&13s&15s&00i&01v&00&05e&15 muscles and a vibrant &00w&15h&00i&15t&00e&15 aura,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_Human(i)))
    sprintf(buf, "&14%s&15, with slightly spikey hair and a blazing &00w&15h&00i&15t&00e&15 aura,", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_Human(i)))
    sprintf(buf, "&14%s&15, with a soft &00w&15h&00i&15t&00e&15 aura,&15", i->player.name); 
  else if (PLR_FLAGGED(i, PLR_trans3) && (IS_ANGEL(i)))
    sprintf(buf, "&14%s&15, with &11g&00&03o&11l&00&03d &00&07w&15i&00&07n&15g&00&07s &15and a &11g&00&03o&11l&00&03d &14a&00&06u&16r&00&07a,&15", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans2) && (IS_ANGEL(i)))
    sprintf(buf, "&14%s&15, with &16t&15i&00&07t&16a&15n&00&07i&16u&15m &16f&15e&00&07ather&15e&16d &15wings,&15", i->player.name);
  else if (PLR_FLAGGED(i, PLR_trans1) && (IS_ANGEL(i)))
    sprintf(buf, "&14%s&15, with &16s&15t&00&07e&15e&16l &00&07w&15i&16n&15g&00&07s ,&15", i->player.name);
  else if (PRF_FLAGGED(i, PRF_MYSTIC2))
    sprintf(buf, "&14%s&15, surrounded by a &00&04b&12l&15u&00e&15 aura, confidently&15", i->player.name);
  else if (PRF_FLAGGED(i, PRF_MYSTIC3))
    sprintf(buf, "&14%s&15, with a &00&04n&12a&15v&00y &00&04b&12l&15u&00e&15 aura and &16m&00&04a&12s&15s&00i&16v&00&04e&15 muscles, confidently&15", i->player.name);
  else if (PRF_FLAGGED(i, PRF_MYSTIC))
    sprintf(buf, "&14%s&15, with a &15f&00&04l&12a&15m&00i&00&04n&12g &00&04b&12l&15u&00e&15 aura, confidently&15", i->player.name);
//  else if (GET_LEVEL(ch) == LVL_IMPL)
//    sprintf(buf, "&14%s&00 %s&15", i->player.name, i->player.title);
  else if (!str_cmp(i->player.name, "Oob") && !IS_NPC(i))
     sprintf(buf, "&14Uub", i->player.name);
  else
//    sprintf(buf, "&14%s &15the %s %s %s &15with %s &15eyes, %s %s &15hair and a %s &15aura", i->player.name, build[(int) i->player.build], genders[(int) i->player.sex], class_abbrevs[(int) i->player.chclass], eyes[(int) i->player.eye], hl[(int) i->player.hairl], hc[(int) i->player.hairc], aura[(int) i->player.aura] );
    sprintf(buf, "&14%s", i->player.name);

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
  if (PRF2_FLAGGED(i, PRF2_FISHING))
      act("&15$e is fishing with a lazy smile.&00", FALSE, i, 0, ch, TO_VICT);
  if (AFF_FLAGGED(i, AFF_BARRIER))
    act("&15...$e is engulfed in a &12globe&15 of &10energy&15.&00", FALSE, i, 0, ch, TO_VICT);
}



void list_char_to_char(struct char_data * list, struct char_data * ch)
{

    struct char_data *i, *plr_list[100];
    int num, counter, locate_list[100], found=FALSE;

    num = 0;

    for (i=list; i; i = i->next_in_room) {
      if(i != ch)  {
        if (CAN_SEE(ch, i))
        {
            if (num< 100)
            {
                found = FALSE;
                for (counter=0;(counter<num&& !found);counter++)
                {
                    if (i->nr == plr_list[counter]->nr &&

/* for odd reasons, you may want to seperate same nr mobs by factors
other than position, the aff flags, and the fighters. (perhaps you want
to list switched chars differently.. or polymorphed chars?) */

                        (GET_POS(i) == GET_POS(plr_list[counter])) &&
                        (AFF_FLAGS(i)==AFF_FLAGS(plr_list[counter])) &&
                        (FIGHTING(i) == FIGHTING(plr_list[counter])) &&
                        !strcmp(GET_NAME(i), GET_NAME(plr_list[counter])))
                    {
                        locate_list[counter] += 1;
                        found=TRUE;
                    }
                }
                if (!found) {
                    plr_list[num] = i;
                    locate_list[num] = 1;
                    num++;
                }
            } else {
                list_all_char(i,ch,0);
            }
        }
      }
    }
    if (num) {
        for (counter=0; counter<num; counter++) {
            if (locate_list[counter] > 1) {
                list_all_char(plr_list[counter],ch,locate_list[counter]);
            } else {
                list_all_char(plr_list[counter],ch,0);
            }
        }
    }
}

void do_auto_exits(struct char_data * ch)
{
  int door;

  *buf = '\0';

  for (door = 0; door < NUM_OF_DIRS; door++)
     if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE) {
       if (EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED) && GET_LEVEL(ch) >= LVL_IMMORT) {
         sprintf(buf2, "&15%-5s &09- &10[&11%5d&10 ] &12(&16Closed&12)&00&07\r\n", dirs[door],
                GET_ROOM_VNUM(EXIT(ch, door)->to_room));
        }      
       else if (GET_LEVEL(ch) >= LVL_IMMORT && !EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED)) {
	sprintf(buf2, "&15%-5s &09- &10[&11%5d&10 ] &15%s&00&07\r\n", dirs[door],
		GET_ROOM_VNUM(EXIT(ch, door)->to_room),
		world[EXIT(ch, door)->to_room].name);
       }
       else if (EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED) && GET_LEVEL(ch) <= 999999){
	sprintf(buf2, "&15%-5s&00 &09-&15 &12(&16Closed&12)&00&07\r\n", dirs[door]);
        }
       else if (GET_LEVEL(ch) <= 999999 && !EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED)){
        sprintf(buf2, "&15%-5s&00 &09-&15 %s&00&07\r\n", dirs[door], world[EXIT(ch, door)->to_room].name);
	if (IS_DARK(EXIT(ch, door)->to_room) && !CAN_SEE_IN_DARK(ch))
	  strcat(buf2, "");
      }
      strcat(buf, CAP(buf2));
    }
  send_to_char("&14Obvious exits&15:\r\n", ch);

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
	sprintf(buf2, "&15%-5s - [%5d] %s&00&07\r\n", dirs[door],
		GET_ROOM_VNUM(EXIT(ch, door)->to_room),
		world[EXIT(ch, door)->to_room].name);
      else {
	sprintf(buf2, "&15%-5s - %s&00&07\r\n", dirs[door], world[EXIT(ch, door)->to_room].name);
	if (IS_DARK(EXIT(ch, door)->to_room) && !CAN_SEE_IN_DARK(ch))
	  strcat(buf2, "Too dark to tell\r\n");
	else {
          sprintf(buf2, "&15%-5s - %s&00&07\r\n", dirs[door],
                world[EXIT(ch, door)->to_room].name);
	}
      }
      strcat(buf, CAP(buf2));
    }
  send_to_char("&00&07Obvious exits&15:&00&07\r\n", ch);

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
  } else if (AFF_FLAGGED(ch, AFF_SOLAR_FLARE) && !AFF_FLAGGED(ch, AFF_INFRAVISION)) {
    send_to_char("You see nothing but infinite darkness...\r\n", ch);
    return;
  }
  send_to_char(CCCYN(ch, C_NRM), ch);
  if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_ROOMFLAGS)) {
    sprintbit(ROOM_FLAGS(ch->in_room), room_bits, buf);
    sprintf(buf2, "&16[&10%5d&16] [&10 %s&16] &16[%s&16]\r\n", GET_ROOM_VNUM(IN_ROOM(ch)), buf, world[ch->in_room].name);
    send_to_char(buf2, ch);
   
  } else if (!PRF_FLAGGED(ch, PRF_ROOMFLAGS)) {
   if (AFF_FLAGGED(ch, AFF_STONED)) {
   switch (number(1, 10)) {
    case 1:
     send_to_char("&00&05Someplace.&00", ch);
     send_to_char("\r\n", ch);
     break;
    case 2:
     send_to_char("&00&05You stare at the room name and try to figure it out.&00", ch);
     send_to_char("\r\n", ch);
     break;
    case 3:
     send_to_char("&00&05You overlook the room name.&00", ch);
     send_to_char("\r\n", ch);
     break;
    case 4:
     send_to_char("&00&05Budville.&00", ch);
     send_to_char("\r\n", ch);
     break;
    case 5:
     send_to_char("&00&05Hahahahaha.&00", ch);
     send_to_char("\r\n", ch);
     break;
    case 6:
     send_to_char("&00&05Copperhead Road.&00", ch);
     send_to_char("\r\n", ch);
     break;
    case 7:
     send_to_char("&00&05The narrator refuses to make out the room name for you.&00", ch);
     send_to_char("\r\n", ch);
     break;
    case 8:
     send_to_char("&00&05Nexus of Lighters, all the missing lighters come here.&00", ch);
     send_to_char("\r\n", ch);
     break;
    case 9:
     send_to_char("&00&05Cheese.&00", ch);
     send_to_char("\r\n", ch);
     break;
    case 10:
     send_to_char("&00&05Hahahahaha hahahah ahah ahaha ahaha.&00", ch);
     send_to_char("\r\n", ch);
     break;
   }
   }
   else {
    send_to_char(world[ch->in_room].name, ch);
    send_to_char(CCNRM(ch, C_NRM), ch);
    send_to_char("\r\n", ch);
   }
  }
  
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10)) {
  send_to_char("&12Gravity:&16 10 times normal gravity.&00\r\n", ch);
  }

  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100)) {
  send_to_char("&12Gravity:&16 100 times normal gravity.&00\r\n", ch);
  
  }
  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200)) {
  send_to_char("&12Gravity:&16 200 times normal gravity.&00\r\n", ch);
  
  }
  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300)) {
  send_to_char("&12Gravity:&16 300 times normal gravity.&00\r\n", ch);
  
  }
  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400)) {
  send_to_char("&12Gravity:&16 400 times normal gravity.&00\r\n", ch);
  
  }
  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_SPIRIT)) {
  send_to_char("&12Gravity:&09 Unknown.&00\r\n", ch);
  
  }
  else
  send_to_char("&12Gravity:&16 Normal.&00\r\n", ch);
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_HTANK) && IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GOOD_REGEN)) {
   send_to_char("&14Healing &00&07T&15a&16n&00&07k&16, &10Regen &12Room&00\r\n", ch);
  }
  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GOOD_REGEN)) {
   send_to_char("&10Regen &12Room&00\r\n", ch);
  }
  else if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_HTANK)) {
   send_to_char("&14Healing &00&07T&15a&16n&00&07k&00\r\n", ch);
  }
  
  if ((!IS_NPC(ch) && !PRF_FLAGGED(ch, PRF_BRIEF)) || ignore_brief || ROOM_FLAGGED(ch->in_room, ROOM_DEATH)) {
    if (AFF_FLAGGED(ch, AFF_STONED)) {
     switch (number(1, 5)) {
     case 1:
     send_to_char("&15This place has some stuff over there. And umm, some shit here.\r\nA dorito shaped thing here. And, uh... uh... something here.\r\n", ch);
     break;
     case 2:
     send_to_char("&15You look around this place and recognise nothing, the narrator seems to have disappeared and won't explain jack shit to you. As you stare some of the description looks rather funny, but you don't care exactly why.&00\r\n", ch);
     break;
     case 3:
     send_to_char("&15The narrator sits and laughs at you as you try to make your way around. You make it to one side of the room to look at something and forget why you went in the first place. Maybe you smoked to much......nah..\r\n", ch);
     break;
     case 4:
     send_to_char("&15One million doobies on the wall, One million doobies on the wall.... take one down, pass it around, nine hundred and ninty nine thousand doobies on the wall.....&00\r\n", ch);
     break;
     case 5:
     send_to_char("&15You forget to care about the room as you search for your lighter. You know you dropped it somewhere, but you can't remember where. Some one hands you their lighter, and you put it in your pocket when finished with it.&00\r\n", ch);
     break;
     }
    }
    else if (!PRF_FLAGGED(ch, PRF_BRIEF)) {
     send_to_char(world[ch->in_room].description, ch);
    }
  }
  /* autoexits */
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
  else if (AFF_FLAGGED(ch, AFF_SOLAR_FLARE) && !AFF_FLAGGED(ch, AFF_INFRAVISION))
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
  *buf = '\0';
    char maxhit[50], hit[50], maxmana[50], mana[50], gold[50], bank[50], totalgold[50], exp[50], tnb[50];
    commafmt(maxhit, sizeof(maxhit), GET_MAX_HIT(ch));
    commafmt(maxmana, sizeof(maxmana), GET_MAX_MANA(ch));
    commafmt(mana, sizeof(mana), GET_MANA(ch));
    commafmt(hit, sizeof(hit), GET_HIT(ch));
    commafmt(gold, sizeof(gold), GET_GOLD(ch));
    commafmt(bank, sizeof(bank), GET_BANK_GOLD(ch));
    commafmt(totalgold, sizeof(totalgold), GET_BANK_GOLD(ch) + GET_GOLD(ch));
    commafmt(exp, sizeof(exp), GET_EXP(ch));
    if (!CLN_FLAGGED(ch, CLN_Death)) {
    commafmt(tnb, sizeof(tnb), GET_LEVEL(ch) * 2000);
    }
    if (CLN_FLAGGED(ch, CLN_Death)) {
    commafmt(tnb, sizeof(tnb), GET_LEVEL(ch) * 1500);
    }

  sprintf(buf,"                    &16[&14Information for&16:&00 &15%s&16]&00\r\n", GET_NAME(ch));
  sprintf(buf, "%s&15o&00&06=------------------------------------------------------------=&15o&00\r\n", buf);  sprintf(buf, "%s&00&06|&10Race:&00 %s     \r\n&00&06|&10Sex: &00 ", buf,
               CLASS_ABBR(ch));
  switch (GET_SEX(ch)) {
    case SEX_MALE: strcat(buf, "&12Male&00\r\n"); break;
    case SEX_FEMALE: strcat(buf, "&13Female&00\r\n"); break;
    case SEX_NEUTRAL: strcat(buf, "&10Neutral&00\r\n"); break;

/* Begin Test Code */
//  sprintf(buf, "%s&15o&12------------------------------------------------------------&15o&00\r\n", buf);
//  sprintf(buf, "%s&15o&12------------------------------------------------------------&15o&00\r\n", buf);
/* end Test Code */

  }
  sprintf(buf, "%s&00&06|&10Level&15: &15%d&00\r\n", buf, GET_LEVEL(ch));
  sprintf(buf, "%s&00&06|&10Powerlevel&15: &15%s&12/&15%s&00\r\n", buf, hit, maxhit);
  sprintf(buf, "%s&00&06|&10Ki&15:         &15%s&12/&15%s&00\r\n", buf, mana, maxmana);
  if (IS_ANDROID(ch)) {
   sprintf(buf, "%s&00&06|&14Upgrade points&15:     &15%d&12/&1520000&00\r\n", buf, GET_HOME(ch));
   }
  if (IS_Namek(ch) || IS_KAI(ch) || IS_MAJIN(ch) || IS_KANASSAN(ch)) {
   sprintf(buf, "%s&00&06|&14Spirit&15:     &15%d&12/&1520000&00\r\n", buf, GET_HOME(ch));
   }
  sprintf(buf, "%s&00&06|&00&06=------------------------------------------------------------=&15o&00\r\n", buf);
  sprintf(buf, "%s&00&06|&12Strength&15:           &00 &16[&15%d&16]&00     &12Dexterity&15:       &00 &16[&15%d&16]&00\r\n", buf, GET_STR(ch), GET_DEX(ch));
  sprintf(buf, "%s&00&06|&12Upper Body Skill&15:   &00 &16[&15%d&16]&00     &12Constitution&15:    &00 &16[&15%d&16]&00\r\n", buf, GET_UBS(ch), GET_CON(ch));
  sprintf(buf, "%s&00&06|&12Lower Body Skill&15:   &00 &16[&15%d&16]&00     &12Hitroll&15:         &00 &16[&15%d&16]&00\r\n", buf, GET_LBS(ch), (ch)->points.hitroll);
  if (GET_WIS(ch) >= 100)
   sprintf(buf, "%s&00&06|&12Wisdom&15:&00              &16[&15%d&16]&00    &12Damroll&15:         &00 &16[&15%d&16]&00\r\n", buf,  GET_WIS(ch), (ch)->points.damroll);
  else
   sprintf(buf, "%s&00&06|&12Wisdom&15:&00              &16[&15%d&16]&00     &12Damroll&15:         &00 &16[&15%d&16]&00\r\n", buf,  GET_WIS(ch), (ch)->points.damroll);
  if (GET_INT(ch) >= 100)
   sprintf(buf, "%s&00&06|&12Intelligence&15:&00        &16[&15%d&16]&00    &12Speed&15:           &00 &16[&15%d&16]&00\r\n", buf, GET_INT(ch), GET_RAGE(ch));
  else
   sprintf(buf, "%s&00&06|&12Intelligence&15:&00        &16[&15%d&16]&00     &12Speed&15:           &00 &16[&15%d&16]&00\r\n", buf, GET_INT(ch), GET_RAGE(ch));
  sprintf(buf, "%s&00&06|&12Age&15:&00                 &16[&15%d&16]&00     &09PKs&15:             &00 &16[&15%d&16]&00\r\n", buf, GET_AGE(ch), GET_HEIGHT(ch));
  if (GET_CHA(ch) >= 100)
   sprintf(buf, "%s&00&06|&12Intuition&15:&00           &16[&15%d&16]&00    &00&01P.Deaths&15:        &00 &16[&15%d&16]&00\r\n", buf, GET_CHA(ch), GET_WEIGHT(ch));   
  else
   sprintf(buf, "%s&00&06|&12Intuition&15:&00           &16[&15%d&16]&00     &00&01P.Deaths&15:        &00 &16[&15%d&16]&00\r\n", buf, GET_CHA(ch), GET_WEIGHT(ch));
  sprintf(buf, "%s&00&06|&12Ki Skill&15:&00            &16[&15%d&16]&00\r\n", buf, GET_REINC(ch));
  sprintf(buf, "%s&00&06|&00&06=------------------------------------------------------------=&15o&00\r\n", buf);
  sprintf(buf, "%s&00&06|&14Zenni Carried&15: &00  &16[&11%s&16]&00\r\n", buf, gold);
  sprintf(buf, "%s&00&06|&14Zenni In bank&15: &00  &16[&11%s&16]&00\r\n", buf, bank);
  sprintf(buf, "%s&00&06|&14Zenni Total&15: &00    &16[&11%s&16]&00\r\n", buf, totalgold);
  sprintf(buf, "%s&00&06|&00&06Experience&15:&00      &16[&15%s&16]&00\r\n", buf, exp);
  sprintf(buf, "%s&00&06|&00&06Experience TNB&15:&00  &16[&15%s&16]&00\r\n", buf, tnb);
  sprintf(buf, "%s&00&06|&12T&00&07r&00&06u&00&07t&12h &00&05Points&15:&00     &16[&15%d&16]&00      &12Learning &00&06Points&15: &00  &16[&11%d&16]&00\r\n", buf, GET_ALIGNMENT(ch), GET_PRACTICES(ch));
  sprintf(buf, "%s&00&06|&14Years&15: &16[&15%d&16] &14Weeks&15:&16 [&15%d&16] &14Days&15:&16 [&15%d&16] &14Hours&15:&16 [&15%d&16] &14Minutes&15:&16 [&15%d&16]&00\r\n", buf, ch->player.time.played / 31536000, ((ch->player.time.played % 31536000) / 604800), ((ch->player.time.played % 604800) / 86400), ((ch->player.time.played % 86400) / 3600), ((ch->player.time.played % 3600) / 60));
  sprintf(buf, "%s&15o&00&06=------------------------------------------------------------=&15o&00\r\n", buf);

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
ACMD(do_flood)
{
  struct obj_data *obj;
  int num = 0;
   
   if (!GET_SKILL(ch, SPELL_FLOOD)) {
       send_to_char("You don't know how to do Dimizu Toride!\r\n", ch);
       return;
      }
   if (ROOM_FLAGGED(IN_ROOM(ch), ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return;
   }
   if (GET_HOME(ch) <= 149) {
      send_to_char("You do not have enough spirit to focus the Dimizu Toride, you need at least 100 spirit!\r\n", ch);
       return;
      }
   num = 2060;
   if ((num = real_object(2060)) < 0) {
      send_to_char("Error! Error! Report to an immortal now!\r\n", ch);
      return;
    }
   if (GET_HOME(ch) >= 150) {
    obj = read_object(num, REAL);
    obj_to_room(obj, ch->in_room);
    GET_OBJ_TIMER(obj) = 10;
    GET_HOME(ch) -= 50;
    act("&00&06You begin to gather a large amount of &12k&00&04i &00&06into your hands, that forms a &14b&00&06l&00&04u&12e &14h&00&06u&14e &00&06around them. Once you've gain a surfactant amount of &00&04e&12n&00&04e&12r&00&04g&12y &00&06you &16slam &00&06both hands into the ground beginning your search of a &00&04w&12a&00&06t&14e&00r &00&06source. After a source is found you draw it quickly from its incased cell. The &00&06w&14a&12t&00&04e&00r &00&06blast out in the form of a &15g&00&06e&14y&00&06s&15e&00r&00&06. With the first step done you send a second &14w&00&06a&14v&00&06e of &00&04e&12n&00&04e&12r&00&04g&12y &00&06into the ground that spreads out about the area and quickly makes a &15c&00ell &00&06to contain the &14w&00&06a&00ter.", FALSE, ch, 0, 0, TO_CHAR);
    act("&15$n &00&06draws both hands together as they focus a large amount of &00&06b&14l&12u&00&04e &00&04k&12i &00&06into them. Grinning as enough power is gained they slam both hands into the ground &16s&15u&14m&00&06m&14o&00&06n&15i&00ng &00&06w&14a&00ter &00&06from a source deep within the &00&02p&10l&00&03a&11n&16e&15t&00's &00&03crust. &00&06Soon after a &15f&00&06o&14u&12n&00&04t&00ain &00&06of &00&06w&14a&12t&00&04e&15r &00&06springs to life just before them, a second blast of power is sent out from &15$n &00&06that forms an &16i&15n&00v&16i&15s&00i&16b&15l&00e &15c&00ell &00&06around them. Only noticeable as the &12w&00&04a&00&06t&14e&00r &00&06level continues to rise within it's &00w&15a&00l&15l&00s!", FALSE, ch, 0, 0, TO_ROOM);
    load_otrigger(obj);
    return;
   }

}
ACMD(do_detect)
{
   struct char_data *tch, *tch_next;
   int dir, found = 0;
   char *heard_nothing = "You don't detect anyone hidden.\r\n";
   char *room_spiel    = "$n seems to stop and concentrate intently for something.";

        /* no argument means that the character is listening for
       * hidden or invisible beings in the room he/she is in
       */
      if (!GET_SKILL(ch, SPELL_DETECT)) {
       send_to_char("You don't know how to do that!\r\n", ch);
       return;
      }
      for(tch = world[ch->in_room].people; tch; tch = tch_next) {
         tch_next = tch->next_in_room;
         if((tch != ch) && !CAN_SEE(ch, tch) && (GET_LEVEL(tch) < LVL_IMMORT))
            found++;
      }
      if(found) {
        if (GET_INT(ch) > number(50, 101)) {
            /* being a higher level is better */
            sprintf(buf, "You sense someone invisible, or hiding.\r\n");
            send_to_char(buf, ch);
           for(tch = world[ch->in_room].people; tch; tch = tch_next) {
            if((tch != ch) && !CAN_SEE(ch, tch) && (GET_LEVEL(tch) < LVL_IMMORT) && AFF_FLAGGED(tch, AFF_HIDE)) {
             send_to_char("You suddenly feel exsposed.\r\n", tch);
             REMOVE_BIT(AFF_FLAGS(tch), AFF_HIDE);
            }
           }
           send_to_char("They have been revealed!\r\n", ch);
         }
        else if (GET_INT(ch) >= number(1, 30)) {
         sprintf(buf, "You sense someone in the immediate area.\r\n");
         send_to_char(buf, ch);
        }
      }
      else { 
      send_to_char(heard_nothing, ch);
      act(room_spiel, TRUE, ch, 0, 0, TO_ROOM);
      return;
   }
}
ACMD(do_rage)
{
 if (!GET_SKILL(ch, SPELL_RAGE)) {
       send_to_char("You don't know how to do that!\r\n", ch);
       return;
      }
 if (GET_MANA(ch) < GET_MAX_MANA(ch)) {
   send_to_char("You do not have anough ki to release your rage! You need full ki.\r\n", ch);
   return;
   }
 if (CLN_FLAGGED(ch, CLN_RAGE)) {
   send_to_char("You have already released your rage and are beyond your normal power!\r\n", ch);
   return;
   }
 else {
   act("&15You grit your teeth as breath is let out through your nose in great big huffs. Your eyes strain as you begin to struggle with your pure rage within!", FALSE, ch, 0, 0, TO_CHAR);
   act("&14$n &15grits $s teeth as breath is let out through $s nose in great big huffs. $n's eyes strain as $e begins to struggle with $s pure rage within!", FALSE, ch, 0, 0, TO_ROOM);
   GET_MANA(ch) -= GET_MAX_MANA(ch);
   GET_MAX_HIT(ch) += 10000000;
   GET_HIT(ch) += 10000000;
   GET_RAGET(ch) = 300;
   SET_BIT(CLN_FLAGS(ch), CLN_RAGE);
   return;
  }
}
ACMD(do_battlelust)
{
  if (!GET_SKILL(ch, SPELL_BLUST)) {
       send_to_char("You don't know how to do that!\r\n", ch);
       return;
      }
 if (GET_MAX_MANA(ch) < 10000001) {
   send_to_char("You do not have anough ki to focus your saiyan blood! You need at least 10mil max ki.\r\n", ch);
   return;
   }
 if (CLN_FLAGGED(ch, CLN_BLUST)) {
   send_to_char("You have already focused your saiyan blood!\r\n", ch);
   return;
   }
 else {
   act("&15You crouch down, as a dark &09r&00&01e&09d&15 aura &00&07flashes&15 around your body! You scream out as your eyes turn white, and your saiyan blood takes over your body!&00", FALSE, ch, 0, 0, TO_CHAR);
   act("&14$n&15 crouches down, as a dark &09r&00&01e&09d&15 aura &00&07flashes&15 around $s body! $e screams out as $s eyes turn white, and $s saiyan blood takes over $s body!", FALSE, ch, 0, 0, TO_ROOM);
   GET_MAX_MANA(ch) -= 10000000;
   GET_LUSTT(ch) = 600;
   SET_BIT(CLN_FLAGS(ch), CLN_BLUST);
   return;
  }
}
/*
* This handles displaying clan membership when clanwho is typed. You can change the clan 
*names in here, or expand the total number of clans(you will also need to add more clan 
*flags in that case)
*/
ACMD(do_clanwho)
{
  struct descriptor_data *d;
     *buf = '\0';
     send_to_char("&16<----------------------------------------------->&00\r\n", ch);
   for (d = descriptor_list; d; d = d->next) {
     if (STATE(d) != CON_PLAYING)
         continue;
     if (!d->character) /* just to make sure */
         continue;
     
     if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && CLN_FLAGGED(d->character, CLN_LEADER1)){
       sprintf(buf2, "&00&04U&12n&16i&15v&00&07e&15r&16s&12a&00&04l &00&04D&12e&16f&15e&00&07n&15s&16e &00&07F&15o&16r&12c&00&04e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Leader &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER1) && CLN_FLAGGED(d->character, CLN_CAPTAIN))){
       sprintf(buf2, "&00&04U&12n&16i&15v&00&07e&15r&16s&12a&00&04l &00&04D&12e&16f&15e&00&07n&15s&16e &00&07F&15o&16r&12c&00&04e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Captain &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0'; 
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER1) && CLN_FLAGGED(d->character, CLN_MEMBER))){
      sprintf(buf2, "&00&04U&12n&16i&15v&00&07e&15r&16s&12a&00&04l &00&04D&12e&16f&15e&00&07n&15s&16e &00&07F&15o&16r&12c&00&04e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Adept &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER1) && CLN_FLAGGED(d->character, CLN_NOVICE))){
       sprintf(buf2, "&00&04U&12n&16i&15v&00&07e&15r&16s&12a&00&04l &00&04D&12e&16f&15e&00&07n&15s&16e &00&07F&15o&16r&12c&00&04e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Member &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER1) && CLN_FLAGGED(d->character, CLN_NEWBIE))){
       sprintf(buf2, "&00&04U&12n&16i&15v&00&07e&15r&16s&12a&00&04l &00&04D&12e&16f&15e&00&07n&15s&16e &00&07F&15o&16r&12c&00&04e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Newbie &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && CLN_FLAGGED(d->character, CLN_LEADER4)){
       sprintf(buf2, "&16Order &15of &00&07T&14e&12m&00&04p&00&06er&00&04a&12n&14c&00&07e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Leader &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER4) && CLN_FLAGGED(d->character, CLN_CAPTAIN))){
       sprintf(buf2, "&16Order &15of &00&07T&14e&12m&00&04p&00&06er&00&04a&12n&14c&00&07e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Captain &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0'; 
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER4) && CLN_FLAGGED(d->character, CLN_MEMBER))){
       sprintf(buf2, "&16Order &15of &00&07T&14e&12m&00&04p&00&06er&00&04a&12n&14c&00&07e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Adept &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER4) && CLN_FLAGGED(d->character, CLN_NOVICE))){
       sprintf(buf2, "&16Order &15of &00&07T&14e&12m&00&04p&00&06er&00&04a&12n&14c&00&07e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Member &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER4) && CLN_FLAGGED(d->character, CLN_NEWBIE))){
       sprintf(buf2, "&16Order &15of &00&07T&14e&12m&00&04p&00&06er&00&04a&12n&14c&00&07e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Newbie &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }

     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && CLN_FLAGGED(d->character, CLN_LEADER2)){
       sprintf(buf2, "&09A&00&01k&16a&00&07t&09s&00&01u&16k&00&07i &14L&00&06a&00&07b&15o&16r&14a&00&06t&14o&16r&15i&00&06e&14s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Leader &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER2) && CLN_FLAGGED(d->character, CLN_CAPTAIN))){
       sprintf(buf2, "&09A&00&01k&16a&00&07t&09s&00&01u&16k&00&07i &14L&00&06a&00&07b&15o&16r&14a&00&06t&14o&16r&15i&00&06e&14s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Captain &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0'; 
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER2) && CLN_FLAGGED(d->character, CLN_MEMBER))){
       sprintf(buf2, "&09A&00&01k&16a&00&07t&09s&00&01u&16k&00&07i &14L&00&06a&00&07b&15o&16r&14a&00&06t&14o&16r&15i&00&06e&14s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Adept &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER2) && CLN_FLAGGED(d->character, CLN_NOVICE))){
       sprintf(buf2, "&09A&00&01k&16a&00&07t&09s&00&01u&16k&00&07i &14L&00&06a&00&07b&15o&16r&14a&00&06t&14o&16r&15i&00&06e&14s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Member &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER2) && CLN_FLAGGED(d->character, CLN_NEWBIE))){
       sprintf(buf2, "&09A&00&01k&16a&00&07t&09s&00&01u&16k&00&07i &14L&00&06a&00&07b&15o&16r&14a&00&06t&14o&16r&15i&00&06e&14s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Newbie &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }

     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && CLN_FLAGGED(d->character, CLN_LEADER3)){
       sprintf(buf2, "&16S&15h&00&07ad&15o&16w &12H&00&04u&16n&15t&16e&00&04r&12s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Leader &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER3) && CLN_FLAGGED(d->character, CLN_CAPTAIN))){
       sprintf(buf2, "&16S&15h&00&07ad&15o&16w &12H&00&04u&16n&15t&16e&00&04r&12s&00"); 
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Captain &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0'; 
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER3) && CLN_FLAGGED(d->character, CLN_MEMBER))){
       sprintf(buf2, "&16S&15h&00&07ad&15o&16w &12H&00&04u&16n&15t&16e&00&04r&12s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Adept &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER3) && CLN_FLAGGED(d->character, CLN_NOVICE))){
       sprintf(buf2, "&16S&15h&00&07ad&15o&16w &12H&00&04u&16n&15t&16e&00&04r&12s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Member &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER3) && CLN_FLAGGED(d->character, CLN_NEWBIE))){
       sprintf(buf2, "&16S&15h&00&07ad&15o&16w &12H&00&04u&16n&15t&16e&00&04r&12s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Newbie &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }

     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && CLN_FLAGGED(d->character, CLN_LEADER5)){
       sprintf(buf2, "&16D&15ar&16k &16R&15e&16gi&15m&16e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Leader &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER5) && CLN_FLAGGED(d->character, CLN_CAPTAIN))){
       sprintf(buf2, "&16D&15ar&16k &16R&15e&16gi&15m&16e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Captain &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0'; 
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER5) && CLN_FLAGGED(d->character, CLN_MEMBER))){
       sprintf(buf2, "&16D&15ar&16k &16R&15e&16gi&15m&16e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Adept &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER5) && CLN_FLAGGED(d->character, CLN_NOVICE))){
       sprintf(buf2, "&16D&15ar&16k &16R&15e&16gi&15m&16e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Member &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER5) && CLN_FLAGGED(d->character, CLN_NEWBIE))){
       sprintf(buf2, "&16D&15ar&16k &16R&15e&16gi&15m&16e&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Newbie &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }

     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && CLN_FLAGGED(d->character, CLN_LEADER6)){
       sprintf(buf2, "&00&06H&14o&16m&15i&00c&15i&16d&14a&00&06l &14M&00&06a&16n&15i&16a&00&06c&14s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Leader &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER6) && CLN_FLAGGED(d->character, CLN_CAPTAIN))){
       sprintf(buf2, "&00&06H&14o&16m&15i&00c&15i&16d&14a&00&06l &14M&00&06a&16n&15i&16a&00&06c&14s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Captain &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0'; 
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER6) && CLN_FLAGGED(d->character, CLN_MEMBER))){
       sprintf(buf2, "&00&06H&14o&16m&15i&00c&15i&16d&14a&00&06l &14M&00&06a&16n&15i&16a&00&06c&14s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Adept &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER6) && CLN_FLAGGED(d->character, CLN_NOVICE))){
       sprintf(buf2, "&00&06H&14o&16m&15i&00c&15i&16d&14a&00&06l &14M&00&06a&16n&15i&16a&00&06c&14s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Member &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER6) && CLN_FLAGGED(d->character, CLN_NEWBIE))){
       sprintf(buf2, "&00&06H&14o&16m&15i&00c&15i&16d&14a&00&06l &14M&00&06a&16n&15i&16a&00&06c&14s&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Newbie &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }

     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && CLN_FLAGGED(d->character, CLN_LEADER7)){
       sprintf(buf2, "&00&02Z&10e&16t&15t&10o&00&02u S&10e&16n&15s&10h&00&02i&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Leader &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER7) && CLN_FLAGGED(d->character, CLN_CAPTAIN))){
       sprintf(buf2, "&00&02Z&10e&16t&15t&10o&00&02u S&10e&16n&15s&10h&00&02i&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Captain &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0'; 
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER7) && CLN_FLAGGED(d->character, CLN_MEMBER))){
       sprintf(buf2, "&00&02Z&10e&16t&15t&10o&00&02u S&10e&16n&15s&10h&00&02i&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Adept &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER7) && CLN_FLAGGED(d->character, CLN_NOVICE))){
       sprintf(buf2, "&00&02Z&10e&16t&15t&10o&00&02u S&10e&16n&15s&10h&00&02i&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Member &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER7) && CLN_FLAGGED(d->character, CLN_NEWBIE))){
       sprintf(buf2, "&00&02Z&10e&16t&15t&10o&00&02u S&10e&16n&15s&10h&00&02i&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Newbie &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }

     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && CLN_FLAGGED(d->character, CLN_LEADER8)){
       sprintf(buf2, "&15S&00&07w&16o&00&07r&15d&14s&00&06w&00&07o&00&06r&14n&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Leader &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER8) && CLN_FLAGGED(d->character, CLN_CAPTAIN))){
       sprintf(buf2, "&15S&00&07w&16o&00&07r&15d&14s&00&06w&00&07o&00&06r&14n&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Captain &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0'; 
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER8) && CLN_FLAGGED(d->character, CLN_MEMBER))){
       sprintf(buf2, "&15S&00&07w&16o&00&07r&15d&14s&00&06w&00&07o&00&06r&14n&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Adept &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER8) && CLN_FLAGGED(d->character, CLN_NOVICE))){
       sprintf(buf2, "&15S&00&07w&16o&00&07r&15d&14s&00&06w&00&07o&00&06r&14n&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Member &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
     else if (STATE(d) == CON_PLAYING && d->character && !IS_NPC(d->character) && (CLN_FLAGGED(d->character, CLN_MEMBER8) && CLN_FLAGGED(d->character, CLN_NEWBIE))){
       sprintf(buf2, "&15S&00&07w&16o&00&07r&15d&14s&00&06w&00&07o&00&06r&14n&00");
       sprintf(buf + strlen(buf), "&00&07[ %s &00&07] &00&07( &16Newbie &00&07) &14%s&00\r\n", buf2, GET_NAME(d->character));
       *buf2 = '\0';
     }
   }
           send_to_char(buf, ch);
    send_to_char("&16<----------------------------------------------->&00\r\n", ch);
}

#define WHO_FORMAT \
"format: who [minlev[-maxlev]] [-n name] [-c classlist] [-s] [-o] [-q] [-r] [-z]\r\n"

ACMD(do_who)
{
  struct descriptor_data *d;
  struct char_data *tch;
  struct obj_data *obj;
  char name_search[MAX_INPUT_LENGTH];
  char mode;
  size_t i;
  char *tmstr;
  time_t mytime;
  int de, h, m;
  int low = 0, high = LVL_1000014, localwho = 0, questwho = 0;
  int showclass = 0, short_list = 0, outlaws = 0, num_can_see = 0;
  int who_room = 0;
  int num = 0;

  skip_spaces(&argument);
  strcpy(buf, argument);
  name_search[0] = '\0';
  *buf = '\0';
  *buf1 = '\0';
  *buf2 = '\0';

  mytime = boot_time;
  tmstr = (char *) asctime(localtime(&mytime));
  *(tmstr + strlen(tmstr) - 1) = '\0';  
  while (*buf) {
    half_chop(buf, arg, buf1);
   /* if (isdigit(*arg)) {
      sscanf(arg, "%d-%d", &low, &high);
      strcpy(buf, buf1);
      send_to_char("Ah ah ah, no arguments with who jackass.\r\n", ch);
    }*/ if (*arg == '-') {
      mode = *(arg + 1);       /* just in case; we destroy arg in the switch */
      switch (mode) {
      case 'k':
         send_to_char("Ah ah ah, no arguments with who jackass.\r\n", ch);
         break;
      default:
	send_to_char("Ah ah ah, no arguments with who jackass.\r\n", ch);
	return;
      }				/* end of switch */

    } else {			/* endif */
        send_to_char("Ah ah ah, no arguments with who jackass.\r\n", ch);
      return;
    }
  }				/* end while (parser) */

  send_to_char("\r\n &100&00&06=============&16[&15Warriors on &11Drag&12(&09*&12)&11nball &15- &16Resurrection of &12T&00&07r&06u&07t&12h&16]&00&06=============&100&00\r\n\r\n", ch);

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
    if (outlaws && !PLR_FLAGGED(tch, PLR_KILLER))
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
      sprintf(buf, "%s [%2Ld %s] %-12.12s%s%s",
	      (GET_LEVEL(tch) >= LVL_IMMORT ? CCYEL(ch, C_SPR) : ""),
	      GET_LEVEL(tch), CLASS_ABBR(tch), GET_NAME(tch),
	      (GET_LEVEL(tch) >= LVL_IMMORT ? CCNRM(ch, C_SPR) : ""),
	      ((!(++num_can_see % 4)) ? "\r\n" : ""));
      send_to_char("Ah Ah ah, no arguments with who jackass.\r\n", ch);
    } else {
      num_can_see++;
      sprintf(buf, "&05-&03(&00%s&03)&05-&00 &03%s&00 %s",
              CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch));
      if (!str_cmp(tch->player.name, "Oob") && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) > (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
               sprintf(buf, " &00&07[%s] &15{&12M&15}&00  &14Uub&00 %s&00", CLASS_ABBR2(tch), GET_TITLE(tch));
      else if (!str_cmp(tch->player.name, "Oob") && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) == (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
               sprintf(buf, " &00&07[%s] &15{&12M&15}&00  &15Uub&00 %s&00", CLASS_ABBR2(tch), GET_TITLE(tch));
      else if (!str_cmp(tch->player.name, "Oob") && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) < (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
               sprintf(buf, " &00&07[%s] &15{&12M&15}&00  &00&06Uub&00 %s&00", CLASS_ABBR2(tch), GET_TITLE(tch));
      else if (GET_SEX(tch) == SEX_MALE && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) > (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
        sprintf(buf, " &00&07[%s] &15{&12M&15}&00  &14%s&00 %s&00", CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch), GET_LEVEL(tch));
      else if (GET_SEX(tch) == SEX_MALE && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) < (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
        sprintf(buf, " &00&07[%s] &15{&12M&15}&00  &00&06%s&00 %s&00", CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch), GET_LEVEL(tch));
      else if (GET_SEX(tch) == SEX_MALE && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) == (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
        sprintf(buf, " &00&07[%s] &15{&12M&15}&00  &15%s&00 %s&00", CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch), GET_LEVEL(tch));
      if (GET_SEX(tch) == SEX_NEUTRAL && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) > (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
        sprintf(buf, " &00&07[%s] &15{&10N&15}&00  &14%s&00 %s&00", CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch), GET_LEVEL(tch));
      else if (GET_SEX(tch) == SEX_NEUTRAL && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) < (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
        sprintf(buf, " &00&07[%s] &15{&10N&15}&00  &06%s&00 %s&00", CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch), GET_LEVEL(tch));
      else if (GET_SEX(tch) == SEX_NEUTRAL && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) == (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
        sprintf(buf, " &00&07[%s] &15{&10N&15}&00  &15%s&00 %s&00", CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch), GET_LEVEL(tch));
      if (GET_SEX(tch) == SEX_FEMALE && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) > (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
        sprintf(buf, " &00&07[%s] &15{&13F&15}&00  &14%s&00 %s&00", CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch), GET_LEVEL(tch));
      else if (GET_SEX(tch) == SEX_FEMALE && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) < (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
        sprintf(buf, " &00&07[%s] &15{&13F&15}&00  &06%s&00 %s&00", CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch), GET_LEVEL(tch));
      else if (GET_SEX(tch) == SEX_FEMALE && (GET_MAX_HIT(ch) + GET_MAX_MANA(ch)) == (GET_MAX_HIT(tch) + GET_MAX_MANA(tch)))
        sprintf(buf, " &00&07[%s] &15{&13F&15}&00  &15%s&00 %s&00", CLASS_ABBR2(tch), GET_NAME(tch), GET_TITLE(tch), GET_LEVEL(tch));
      
      /* Imms */
      if ((GET_LEVEL(tch) == LVL_IMMORT && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, " &00[&12Immortal&00     ] &15{&12M&15}  &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_IMMORT && GET_SEX(tch) == SEX_FEMALE))
      	sprintf(buf, " &00[&12Immortal&00     ] &15{&13F&15}  &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_LSSRGOD && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, " &00[&12Lesser God   &00] &15{&12M&15}  &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_LSSRGOD && GET_SEX(tch) == SEX_FEMALE))
      	sprintf(buf, " &00[&12Lesser Goddess&00] &15{&13F&15} &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_GOD && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, " &00[&15God&00          ] &15{&12M&15}  &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_GOD && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, " &00[&15Godess&00       ] &15{&13F&15}  &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_ARCANE && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, " &00[&10Arcane&00       ] &15{&12M&15}  &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_ARCANE && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, " &00[&10Arcane&00       ] &15{&13F&15}  &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_GRGOD && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, " &00[&14Greater God&00  ] &15{&12M&15}  &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_GRGOD && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, " &00[&14Gr. Goddess&00  ] &15{&13F&15}  &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_COIMP && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, " &00[&11Co-Imp&00       ] &15{&12M&15}  &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_COIMP && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, " &00[&11Co-Imp&00       ] &15{&13F&15}  &14%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_IMPLL && GET_SEX(tch) == SEX_MALE))
      	sprintf(buf, " &00[&09Implementer&00  ] &15{&12M&15}  &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_IMPLL && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, " &00[&09Implementress&00] &15{&13F&15}  &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch)); 

      else if ((GET_LEVEL(tch) == LVL_IMPL && GET_SEX(tch) == SEX_MALE))
        sprintf(buf, " &00[&16Super &09Imp    &00] &15{&12M&15}  &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_IMPL && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, " &00[&16Super &09Imp    &00] &15{&13F&15}  &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_1000012 && GET_SEX(tch) == SEX_MALE))
        sprintf(buf, " &00[&16Head &09Imp    &00 ] &15{&12M&15}  &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_1000012 && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, " &00[&16Head &09Imp    &00 ] &15{&13F&15}  &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch)); 

      else if ((GET_LEVEL(tch) == LVL_1000013 && GET_SEX(tch) == SEX_MALE))
        sprintf(buf, " &00[&16Co&15-&16Owner    &00 ] &15{&12M&15}  &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_1000013 && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, " &00[&16Co&15-&16Owner    &00 ] &15{&13F&15}  &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

      else if ((GET_LEVEL(tch) == LVL_1000014 && GET_SEX(tch) == SEX_MALE))
        sprintf(buf, " &00[&16Owner       &00 ] &15{&12M&15}  &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));
      else if ((GET_LEVEL(tch) == LVL_1000014 && GET_SEX(tch) == SEX_FEMALE))
        sprintf(buf, " &00[&16Owner       &00 ] &15{&13F&15}  &15%s&00&11 %s&00 ",
              GET_NAME(tch), GET_TITLE(tch));

/* Owner */
       if (!str_cmp(tch->player.name, ""))
        sprintf(buf, " &00[&12O&00&07w&00&06n&00&07e&12r&00        &00] &15{&12M&15}  &00&07( &14S&00&06o&16l&15b&00&07et&00&07 ) %s",  GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Zala"))
        sprintf(buf, " &00[&11Co-Imp       &00] &15{&13F&15}  &00&07( &13Z&00&05a&13l&00&05a&07 ) %s", GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Dromine"))
        sprintf(buf, " &00[&10E&00&02n&00&06f&10o&00&02r&00&06c&10e&00&02r     &00] &15{&12M&15}  &00&07( &00&07D&15r&00&03o&00&01m&00&03i&15n&00&07e&00&07 ) %s", GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Primus"))
        sprintf(buf, " &00[&12O&00&07w&00&06n&00&07e&12r&00        &00] &15{&12M&15}  &00&07( &09P&00&01r&00&16i&07m&00&01u&09s&00&07 ) %s", GET_TITLE(tch)
              );       
       if (!str_cmp(tch->player.name, "Leandra"))
        sprintf(buf, " &00[&16H&15o&00&07u&15s&16e &12I&00&04m&16m&00    &00] &15{&13F&15}  &00&07( &00&06L&16e&15a&00&07n&15d&16r&00&06a&00&00&07 ) %s", GET_TITLE(tch)
              );       
       if (!str_cmp(tch->player.name, "Kuvoc"))
        sprintf(buf, " &00[&12O&00&07w&00&06n&00&07e&12r&00        &00] &15{&12M&15}  &00&07( &09K&00&01u&16v&00&01o&09c&00&07 ) %s", GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Hydro"))
        sprintf(buf, " &00[&10Head&15-&12I&00&04m&00&06p&00     ] &15{&12M&15}  &00&07( &10H&16y&00&02d&16r&10o&00&07 ) %s", GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Raiden"))
        sprintf(buf, " &00[&12O&00&07w&00&06n&00&07e&12r&00        &00] &15{&12M&15}  &00&07( &12R&00&04a&03i&00&11d&00&12e&00&04n&00&07 ) %s",  GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Machina"))
        sprintf(buf, " &00[&09B&00&01u&09i&00&01l&09d&00&01e&09r&00      ] &15{&12M&15}  &00&07( &00&11M&00&03a&00&16c&00&07h&00&16i&00&03n&00&11a &00&07) %s", GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Shinigami"))
        sprintf(buf, " &00[&09B&00&01u&09i&00&01l&09d&00&01e&09r&00      ] &15{&12M&15}  &00&07( &14S&00&06h&00&01i&09n&15i&09g&00&01a&00&06m&14i&00 &00&07) %s", GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Nexus"))
        sprintf(buf, " &00[&09B&00&01u&09i&00&01l&09d&00&01e&09r&00      ] &15{&12M&15}  &00&07( &09N&00&01e&00&06x&00&01u&09s&00 &00&07) %s", GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Koma"))
        sprintf(buf, " &00[&00&05Im&16pl&15e&00&07m&15e&16nt&00&05or  &00] &15{&12M&15}  &00&07( &16K&15o&00&07m&16a&00&07 ) %s", GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Taishyr"))
        sprintf(buf, " &00[&10E&00&02n&00&06f&10o&00&02r&00&06c&10e&00&02r     &00] &15{&12M&15}  &00&07( &00&06T&00&12a&00&14i&00&02s&00&06h&00&12y&00&02r &00&07) %s", GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Ravorn"))
        sprintf(buf, " &00[&10E&00&02n&00&06f&10o&00&02r&00&06c&10e&00&02r     &00] &15{&12M&15}  &00&07( &10R&00&02a&00&01vo&00&02r&10n &00&07) %s", GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Mhurzin"))
        sprintf(buf, " &00[&00&05Im&16pl&15e&00&07m&15e&16nt&00&05or&00  &00] &15{&12M&15}  &00&07( &16M&00&01h&00&05u&00&01r&16z&00&05i&00&01n &00&07) %s", GET_TITLE(tch)
              );      
       if (!str_cmp(tch->player.name, "Kisato"))
        sprintf(buf, " &00[&09R&00&01p &14I&00&06m&16m       &00] &15{&13F&15}  &00&07( &00&03K&11i&15sa&14t&00&06o &00&07) %s", GET_TITLE(tch)
              );
       if (!str_cmp(tch->player.name, "Xyron"))
        sprintf(buf, " &00[&00&05Im&16pl&15e&00&07m&15e&16nt&00&05or&00  ] &15{&12M&15}  &00&07( &00&02X&10y&00&02r&10o&00&02n&00 &00&07) %s", GET_TITLE(tch)
              );


      if (GET_INVIS_LEV(tch))
	sprintf(buf + strlen(buf), " &09(%Ld)&00", GET_INVIS_LEV(tch));
      else if (AFF_FLAGGED(tch, AFF_INVISIBLE) && GET_LEVEL(tch) <= 999999)
	strcat(buf, " &09(&00&05Invis&09)&00");

      if (PLR_FLAGGED(tch, PLR_MAILING) && GET_LEVEL(tch) <= 999999)
	strcat(buf, " &15(Mailing)&00");
      else if (PLR_FLAGGED(tch, PLR_MAILING) && GET_LEVEL(tch) > 999999)
        strcat(buf, " &15(Mailing)&00");
      else if ((STATE(d) >= CON_OEDIT) && (STATE(d) <= CON_TEXTED))
         strcat(buf, " &12(BUILDING)&00");
      else if (PLR_FLAGGED(tch, PLR_WRITING) && GET_LEVEL(tch) <= 999999)
	strcat(buf, " &15(Writing)&00");
      else if (PLR_FLAGGED(tch, PLR_WRITING) && GET_LEVEL(tch) > 999999)
        strcat(buf, " &15(Writing)&00");
      else if (GET_CLAN(ch) == GET_CLAN(tch) && GET_CLAN(ch) >= 1)
        strcat(buf, " &16(&12Clan&16)&00");      

      if (PRF_FLAGGED(tch, PRF_NOTELL) && GET_LEVEL(tch) <= 999999)
	strcat(buf, " &16(&00&06No Tell&16)&00");
      else if (PRF_FLAGGED(tch, PRF_NOTELL) && GET_LEVEL(tch) > 999999)
        strcat(buf, " &16(&00&06No Tell&16)&00");
      if (GET_MAX_HIT(tch) <= 24999) 
         strcat(buf, " &16(&00&07N&14e&00&06wb&14i&00&07e&16)&00");
      if (PRF_FLAGGED(tch, PRF_QUEST) && GET_LEVEL(tch) <= 999999)
	strcat(buf, " &15(&14RP&15)&00");
      else if (PRF_FLAGGED(tch, PRF_QUEST) && GET_LEVEL(tch) > 999999)
        strcat(buf, " &15(&14RP&15)&00");
      if (PLR_FLAGGED(tch, PLR_KILLER))
	strcat(buf, " &16(&09PK &11A&00&03r&11e&00&03n&11a&16)&00");
      if (PLR_FLAGGED(tch, PLR_FROZEN))
        strcat(buf, " &16(&14Frozen&16)&00");
      if (PRF_FLAGGED(tch, PRF_NOGRATZ))
        strcat(buf, " &16(&09H&15e&00&01l&09l&16)&00");  
      if (PLR_FLAGGED(tch, PLR_NOSHOUT))
        strcat(buf, " &16(&00&05Mute&16)&00");
      if (PLR_FLAGGED(tch, PLR_icer))
        strcat(buf, " &12(&16B&15o&00&07un&15t&16y&12)&00");
      for (obj = tch->carrying; obj; obj = obj->next_content) {
      if (GET_OBJ_TYPE(obj) == ITEM_dball1 || GET_OBJ_TYPE(obj) == ITEM_dball2 || GET_OBJ_TYPE(obj) == ITEM_dball3 || GET_OBJ_TYPE(obj) == ITEM_dball4 || GET_OBJ_TYPE(obj) == ITEM_dball5 || GET_OBJ_TYPE(obj) == ITEM_dball6 || GET_OBJ_TYPE(obj) == ITEM_dball7) {
         num += 1;
        if (num <= 7) {
         strcat(buf, " &11*&00");
        }
        continue;
         }
        if (IS_OBJ_STAT(obj, ITEM_FLAG)) {
          strcat(buf, "&16(&14C.Flg&16)&00");
         continue;
        }
       }
      if (PRF_FLAGGED(tch, PRF_AFK) && GET_LEVEL(tch) <= 999999)
        strcat(buf, " &15(&09Away&15)&00");
      else if (PRF_FLAGGED(tch, PRF_AFK) && GET_LEVEL(tch) > 999999)
        strcat(buf, " &15(&09Away&15)&00");
      if (PRF2_FLAGGED(tch, PRF2_GO) && GET_LEVEL(tch) <= 1000014)
        strcat(buf, " &16(&11G&00&03.&11O&00&03.&16)&00");
      if (PRF_FLAGGED(tch, PRF_OOZARU) && GET_LEVEL(tch) <= 999999)
        strcat(buf, " &16(&09O&00&01o&16z&15a&00&01r&09u&16)&00");
      else if (PRF_FLAGGED(tch, PRF_OOZARU) && GET_LEVEL(tch) > 999999)
        strcat(buf, " &16(&09O&00&01o&16z&15a&00&01r&09u&16)&00");
      if (PRF_FLAGGED(tch, PRF_NOGOSS) && GET_LEVEL(tch) <= 999999)
        strcat(buf, " &16(&00&04No Rp&16)&00");
      else if (PRF_FLAGGED(tch, PRF_NOGOSS) && GET_LEVEL(tch) > 999999)
        strcat(buf, " &16(&00&04No Rp&16)&00");
      if (PRF_FLAGGED(tch, PRF_DEAF) && GET_LEVEL(tch) <= 999999)
        strcat(buf, " &16(&10No Hol.&16)&00");
      else if (PRF_FLAGGED(tch, PRF_DEAF) && GET_LEVEL(tch) > 999999)
        strcat(buf, " &16(&10No Hol.&16)&00");
      if (PRF_FLAGGED(tch, PRF_NOooc) && GET_LEVEL(tch) <= 999999)
        strcat(buf, " &16(&12No OOC&16)&00");
      else if (PRF_FLAGGED(tch, PRF_NOooc) && GET_LEVEL(tch) > 999999)
        strcat(buf, " &16(&12No OOC&16)&00");
      if (PRF_FLAGGED(tch, PRF_TRAINING) && GET_LEVEL(tch) <= 999999)
        strcat(buf, " &16(&16T&00&03r&11a&15i&16n&00&03i&11n&15g&16)&00");
      else if (PRF_FLAGGED(tch, PRF_TRAINING) && GET_LEVEL(tch) > 999999)
        strcat(buf, " &16(&16T&00&03r&11a&15i&16n&00&03i&11n&15g&16)&00");
      if (tch->char_specials.timer >= 5 && GET_LEVEL(tch) <= 999999)
        strcat(buf, " &16(&09Idle&16)&00");
      else if (tch->char_specials.timer >= 5 && GET_LEVEL(tch) > 999999)
        strcat(buf, " &16(&09Idle&16)&00");
      if (PLR_FLAGGED(tch, PLR_trans4) && IS_BIODROID(tch))
        strcat(buf, " &16(&11S&00&03upe&11r &10P&00&02er&16f&15e&00&07ct&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && IS_BIODROID(tch))
        strcat(buf, " &16(&10P&00&02er&16f&15e&00&07ct&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_BIODROID(tch))
        strcat(buf, " &16(&00&07I&15m&16p&00&02e&10r&00&02f&16e&15c&00&07t&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_BIODROID(tch))
        strcat(buf, " &16(&10H&00&02a&16t&00&02c&10h&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans5) && (IS_saiyan(tch) || IS_HALF_BREED(tch)))
        strcat(buf, " &16(&11L&00&03e&11g&00&3e&11n&00&03d&11a&00&03r&11y &11S&00&03S&11J&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans4) && (IS_saiyan(tch) || IS_HALF_BREED(tch)))
        strcat(buf, " &16(&11S&00&03S&11J &154&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && (IS_saiyan(tch) || IS_HALF_BREED(tch)))
        strcat(buf, " &16(&11S&00&03S&11J &153&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && (IS_saiyan(tch) || IS_HALF_BREED(tch)))
        strcat(buf, " &16(&11S&00&03S&11J &152&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && (IS_saiyan(tch) || IS_HALF_BREED(tch)))
        strcat(buf, " &16(&11S&00&03S&11J&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans4) && IS_icer(tch))
        strcat(buf, " &16(&14T&00&06r&16a&15n&00&07s&15f&16o&00&06r&14m &154&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && IS_icer(tch))
        strcat(buf, " &16(&14T&00&06r&16a&15n&00&07s&15f&16o&00&06r&14m &153&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_icer(tch))
        strcat(buf, " &16(&14T&00&06r&16a&15n&00&07s&15f&16o&00&06r&14m &152&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_icer(tch))
        strcat(buf, " &16(&14T&00&06r&16a&15n&00&07s&15f&16o&00&06r&14m &151&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_Human(tch))
        strcat(buf, " &16(&11S&15.&14H&00&06u&14m&00&06a&14n&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_Human(tch))
        strcat(buf, " &16(&11S&15.&14H&00&06u&14m&00&06a&14n &152&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && IS_Human(tch))      
        strcat(buf, " &16(&11S&15.&14H&00&06u&14m&00&06a&14n &153&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans4) && IS_Human(tch))
        strcat(buf, " &16(&11S&15.&14H&00&06u&14m&00&06a&14n &154&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_MAJIN(tch))
        strcat(buf, " &16(&13G&00&05o&00&04o&13d&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_MAJIN(tch))
        strcat(buf, " &16(&13E&00&05v&00&04i&13l&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && IS_MAJIN(tch))
        strcat(buf, " &16(&13S&00&05u&16p&00&05e&13r&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans4) && IS_MAJIN(tch))
        strcat(buf, " &16(&13T&00&05r&00&04u&13e&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_MUTANT(tch))
        strcat(buf, " &16(&09M&00&01u&00&05t&16a&00&01t&09e &151&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_MUTANT(tch))
        strcat(buf, " &16(&09M&00&01u&00&05t&16a&00&01t&09e &152&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && IS_MUTANT(tch))
        strcat(buf, " &16(&09M&00&01u&00&05t&16a&00&01t&09e &153&16)&00");    
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_ANGEL(tch))
        strcat(buf, " &16(&15S&16.&11A&00&03n&16g&15e&00&07l &141&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_ANGEL(tch))
        strcat(buf, " &16(&15S&16.&11A&00&03n&16g&15e&00&07l &142&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && IS_ANGEL(tch))
        strcat(buf, " &16(&14S&16e&00&06r&15a&00p&06h&14i&15m&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_ANDROID(tch))
        strcat(buf, " &16(&141&15.&140&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_ANDROID(tch))
        strcat(buf, " &16(&142&15.&140&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && IS_ANDROID(tch))
        strcat(buf, " &16(&143&15.&140&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans4) && !PLR_FLAGGED(tch, PLR_trans5) && IS_ANDROID(tch))
        strcat(buf, " &16(&144&15.&140&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans5) && !PLR_FLAGGED(tch, PLR_trans6) && IS_ANDROID(tch))
        strcat(buf, " &16(&145&15.&140&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans6) && IS_ANDROID(tch))
        strcat(buf, " &16(&146&15.&140&16)&00");
      else if (PRF_FLAGGED(tch, PRF_MYSTIC2) && !PRF_FLAGGED(tch, PRF_MYSTIC3) && (IS_HALF_BREED(tch) | IS_KAI(tch)))
        strcat(buf, " &16(&14Mystic&16)&00");
      else if (PRF_FLAGGED(tch, PRF_MYSTIC3) && !PRF_FLAGGED(tch, PRF_MYSTIC) && (IS_HALF_BREED(tch) | IS_KAI(tch)))
        strcat(buf, " &16(&14Mystic &152&16)&00");
      else if (PRF_FLAGGED(tch, PRF_MYSTIC) && (IS_HALF_BREED(tch) | IS_KAI(tch)))
        strcat(buf, " &16(&14Magnius&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_TRUFFLE(tch))
        strcat(buf, " &16(&14A&00&06l&16p&15h&00&07a&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_TRUFFLE(tch))
        strcat(buf, " &16(&12B&00&04e&16t&15a&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && IS_TRUFFLE(tch))
        strcat(buf, " &16(&10I&00&02n&16f&15u&00&07s&16i&00&02o&10n&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && IS_KANASSAN(tch))
        strcat(buf, " &16(&11S&15.&12K&00&04a&00&06n&14as&00&06s&00&04a&12n &153&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_KANASSAN(tch))
        strcat(buf, " &16(&11S&15.&12K&00&04a&00&06n&14as&00&06s&00&04a&12n &151&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_KANASSAN(tch))
        strcat(buf, " &16(&11S&15.&12K&00&04a&00&06n&14as&00&06s&00&04a&12n &152&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_demon(tch))
        strcat(buf, " &16(&00&07I&15n&16n&09e&00&01r&09f&16i&15r&00&07e &141&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_demon(tch))
        strcat(buf, " &16(&00&07I&15n&16n&09e&00&01r&09f&16i&15r&00&07e &142&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && IS_demon(tch))
        strcat(buf, " &16(&00&07I&15n&16n&09e&00&01r&09f&16i&15r&00&07e &143&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans4) && IS_demon(tch))
        strcat(buf, " &16(&00&07I&15n&16n&09e&00&01r&09f&16i&15r&00&07e &144&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_Namek(tch))
        strcat(buf, " &16(&11S&15.&10N&00&02ame&10k&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_Namek(tch))
        strcat(buf, " &16(&11S&15.&10N&00&02ame&10k &152&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && !PLR_FLAGGED(tch, PLR_trans4) && IS_Namek(tch))
        strcat(buf, " &16(&11S&15.&10N&00&02ame&10k &153&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans4) && IS_Namek(tch))
        strcat(buf, " &16(&11S&15.&10N&00&02ame&10k &154&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans3) && IS_KONATSU(tch))
        strcat(buf, " &16(&00&07S&15h&16ad&15o&00&07w &143&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans1) && !PLR_FLAGGED(tch, PLR_trans2) && IS_KONATSU(tch))
        strcat(buf, " &16(&00&07S&15h&16ad&15o&00&07w &141&16)&00");
      else if (PLR_FLAGGED(tch, PLR_trans2) && !PLR_FLAGGED(tch, PLR_trans3) && IS_KONATSU(tch))
        strcat(buf, " &16(&00&07S&15h&16ad&15o&00&07w &142&16)&00");


        strcat(buf, "\r\n");
        send_to_char(buf, ch);
    }				/* endif shortlist */
  }				/* end of for */
  if (short_list && (num_can_see % 4))
    send_to_char("\r\n", ch);
  if (num_can_see <= 0) {
    sprintf(buf, "\r\n&15No-one at all!&00\r\n");
  }
  else if (num_can_see == 1) {
    sprintf(buf, "\r\n &100&00&06==================&16[&141&00 &15warrior in the &16Resurrection of &12T&00&07r&06u&07t&12h&15.&16]&00&06=================&100&00\r\n");
  }
  else if (num_can_see <= 9 && num_can_see > 1) {
    sprintf(buf, "\r\n &100&00&06=================&16[&14%d&00 &15warriors in the &16Resurrection of &12T&00&07r&06u&07t&12h&15.&16]&00&06=================&100&00\r\n", num_can_see);
  }
  else if (num_can_see >= 10) {
    sprintf(buf, "\r\n &100&00&06=================&16[&14%d&00 &15warriors in the &16Resurrection of &12T&00&07r&06u&07t&12h&15.&16]&00&06================&100&00\r\n", num_can_see);
  }
  send_to_char(buf, ch);
  sprintf(buf2, "             &15Time to next reboot: &14%d &15day(s), &14%d &15hour(s), &14%d &15minute(s)&00\r\n", (reboot_time / 86400) % 7, (reboot_time / 3600) % 24, (reboot_time / 60) % 60);
  send_to_char(buf2, ch);
  *buf2 = '\0';
  sprintf(buf2, "             &15Next interest in: &14%d &15hour(s), &14%d &15minute(s), &14%d &15second(s)&00\r\n", (interest_time / 3600) % 24, (interest_time / 60) % 60, interest_time % 60);
  send_to_char(buf2, ch);
  send_to_char("                           &15Please check out help vote.&00\r\n", ch);
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
  int low = 0, high = 1000014, num_can_see = 0;
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
      if (outlaws && !PLR_FLAGGED(tch, PLR_KILLER))
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
  case SCMD_LOG:
    page_string(ch->desc, syslog, 0);
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
  int num2 = 0;

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
   }
  if ((!str_cmp(arg,"dragonball") || !str_cmp(arg,"dragonb") || !str_cmp(arg,"dragonba") || !str_cmp(arg,"dragonbal") || !str_cmp(arg,"dragon")) && GET_LEVEL(ch) <= 1000011) {
    send_to_char("Do not where dragonballs, or dragons!", ch);
    return;
   }
   else {
    for (i = character_list; i; i = i->next)
      if (CAN_SEE(ch, i) && i->in_room != NOWHERE && isname(arg, i->player.name)) {
        found = 1;
        sprintf(buf, "M%3d. %-25s - [%5d] %s\r\n", ++num, GET_NAME(i),
                GET_ROOM_VNUM(IN_ROOM(i)), world[IN_ROOM(i)].name);
        send_to_char(buf, ch);
      }
    for (num = 0, k = object_list; k; k = k->next)
      if (CAN_SEE_OBJ(ch, k) && !str_cmp(arg,"key")) {
       if (GET_OBJ_TYPE(k) == ITEM_KEY) {
        found = 1;
        print_object_location(++num, k, ch, TRUE);
       }
       else {
        continue;
       }
      }
      else if (CAN_SEE_OBJ(ch, k) && isname(arg, k->name) && GET_OBJ_TYPE(k) != ITEM_KEY) {
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
  if (GET_LEVEL(ch) >= LVL_IMMORT) {
    perform_immort_where(ch, arg);
  }
  else {
    perform_mortal_where(ch, arg);
  }
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
  diff = ((GET_MAX_HIT(victim) - GET_MAX_HIT(ch)));

  if (diff <= -2000000)
    send_to_char("&16[&11Difficulty&15: &14Now where did that chicken go?&16]\r\n", ch);
  else if (diff <= -500000)
    send_to_char("&16[&11Difficulty&15: &14You could do it with a needle!&16]\r\n", ch);
  else if (diff <= -200000)
    send_to_char("&16[&11Difficulty&15: &14Easy.&16]\r\n", ch);
  else if (diff <= -50000)
    send_to_char("&16[&11Difficulty&15: &14Fairly easy.&16]\r\n", ch);
  else if (diff == 0)
    send_to_char("&16[&11Difficulty&15: &14The perfect match!&16]\r\n", ch);
  else if (diff <= 100)
    send_to_char("&16[&11Difficulty&15: &14You would need some luck!&16]\r\n", ch);
  else if (diff <= 50000)
    send_to_char("&16[&11Difficulty&15: &14You would need a lot of luck!&16]\r\n", ch);
  else if (diff <= 500000)
    send_to_char("&16[&11Difficulty&15: &14You would need a lot of luck and great equipment!&16]\r\n", ch);
  else if (diff <= 750000)
    send_to_char("&16[&11Difficulty&15: &14Do you feel lucky, punk?&16]\r\n", ch);
  else if (diff <= 1500000)
    send_to_char("&16[&11Difficulty&15: &14Are you mad!?&16]\r\n", ch);
  else if (diff >= 1500000)
    send_to_char("&16[&11Difficulty&15: &14You ARE mad!&16]\r\n", ch);


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
    send_to_char("Usage: color { Off | Complete }\r\n", ch);
    send_to_char(buf, ch);
    return;
  }
  if (((tp = search_block(arg, ctypes, FALSE)) == -1)) {
    send_to_char("Usage: color { Off | Complete }\r\n", ch);
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
          "   Auto Looting: %-3s    "	  
	  " Auto Splitting: %-3s    " 

	  "    Color Level: %s\r\n",

	  ONOFF(PRF_FLAGGED(ch, PRF_DISPHP)),
	  ONOFF(PRF_FLAGGED(ch, PRF_BRIEF)),
	  ONOFF(!PRF_FLAGGED(ch, PRF_SUMMONABLE)),

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
          ONOFF(PRF_FLAGGED(ch, PRF_AUTOLOOT)),
          ONOFF(PRF_FLAGGED(ch, PRF_AUTOSPLIT)),

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

  if (subcmd == SCMD_SOCIALS) {
    socials = 1;
  }
  else if (subcmd == SCMD_WIZHELP) {
    wizhelp = 1;
  }

  sprintf(buf, "The following are available to %s:\r\n", GET_NAME(vict));
	  wizhelp ? "privileged " : "",
	  socials ? "socials" : "commands",
	  vict == ch ? "you" : GET_NAME(vict);

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
   int MANA = 0, EXP = 0;
        if (ROOM_FLAGGED(ch->in_room, ROOM_NOQUIT)) {
         send_to_char("You cannot exercise here!\r\n", ch);
         return;
        }
        if (PRF_FLAGGED(ch, PRF_CAMP)) {
        send_to_char("You cannot exercise while camped!\r\n", ch);
        return;
        }
        if (GET_MANA(ch) <= 0) {
        send_to_char("You don't have enough ki to continue!", ch);
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
send_to_char("&15You seem to have &10g&00&02r&16o&00&02w&10n &15surprisingly &09s&00&02t&16r&15o&00&07n&16g&00&01e&09r&15 with that pushup&11!&00\r\n", ch);
}
   if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10) && GET_MANA(ch) >= GET_MAX_MANA(ch)/250)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/250, GET_MAX_MANA(ch)/250);
  EXP = number(GET_HIT(ch)/200, GET_HIT(ch)/200) + GET_LEVEL(ch) * 5;
  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
   if (number(1, 30) == 15) {
    GET_MAX_HIT(ch) += number(9, 20);
    send_to_char("&15You gain some bodily strength from that pushup!&00\r\n", ch);
   }
  return;
}
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) && GET_MANA(ch) >= GET_MAX_MANA(ch)/200)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/200, GET_MAX_MANA(ch)/200);
  EXP = number(GET_HIT(ch)/180, GET_HIT(ch)/180) + (GET_LEVEL(ch) * 10);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  if (number(1, 30) == 15) {
    GET_MAX_HIT(ch) += number(9, 20);
    send_to_char("&15You gain some bodily strength from that pushup!&00\r\n", ch);
   }
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200) && GET_MANA(ch) >= GET_MAX_MANA(ch)/150)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/150, GET_MAX_MANA(ch)/150);
  EXP = number(GET_HIT(ch)/160, GET_HIT(ch)/160) + (GET_LEVEL(ch) * 15);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  if (number(1, 30) == 15) {
    GET_MAX_HIT(ch) += number(9, 20);
    send_to_char("&15You gain some bodily strength from that pushup!&00\r\n", ch);
   }
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300) && GET_MANA(ch) >= GET_MAX_MANA(ch)/100)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/100, GET_MAX_MANA(ch)/100);
  EXP = number(GET_HIT(ch)/150, GET_HIT(ch)/150) + (GET_LEVEL(ch) * 20);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  if (number(1, 30) == 15) {
    GET_MAX_HIT(ch) += number(9, 20);
    send_to_char("&15You gain some bodily strength from that pushup!&00\r\n", ch);
   }
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400) && GET_MANA(ch) >= GET_MAX_MANA(ch)/75)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/75, GET_MAX_MANA(ch)/75);
  EXP = number(GET_HIT(ch)/125, GET_HIT(ch)/125) + (GET_LEVEL(ch) * 25);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  if (number(1, 30) == 15) {
    GET_MAX_HIT(ch) += number(9, 20);
    send_to_char("&15You gain some bodily strength from that pushup!&00\r\n", ch);
   }
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) && GET_MANA(ch) >= GET_MAX_MANA(ch)/250)
      {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/250, GET_MAX_MANA(ch)/250);
  EXP = number(GET_HIT(ch)/100, GET_HIT(ch)/100);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  return;
    }
 else if (GET_MANA(ch) >= GET_MAX_MANA(ch)/300 &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_SPIRIT))
   {
send_to_char("You do some pushups and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some pushups.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/300, GET_MAX_MANA(ch)/300);
  EXP = number(GET_HIT(ch)/150, GET_HIT(ch)/150);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
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
   int MANA = 0, EXP = 0;
   if (ROOM_FLAGGED(ch->in_room, ROOM_NOQUIT)) {
         send_to_char("You cannot exercise here!\r\n", ch);
         return;
        }
   if (PRF_FLAGGED(ch, PRF_CAMP)) {
      send_to_char("You can't do this while camped!\r\n", ch);
      return;
    }
    if (GET_MANA(ch) <= 0) {
        send_to_char("You don't have enough ki to continue!", ch);
        return;
        }
if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000) {
GET_LEVEL(ch) += 1;
GET_EXP(ch) = 1;
num_levels++;                                                                   advance_level(ch);                                                           
is_altered = TRUE;
send_to_char("&15You seem to have &10g&00&02r&16o&00&02w&10n &15surprisingly &09s&00&02t&16r&15o&00&07n&16g&00&01e&09r&15 with that pushup&11!&00\r\n", ch);
}
   if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10) && GET_MANA(ch) >= GET_MAX_MANA(ch)/250)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/250, GET_MAX_MANA(ch)/250);
  EXP = number(GET_HIT(ch)/200, GET_HIT(ch)/200) + (GET_LEVEL(ch) * 5);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  if (number(1, 30) == 15) {
    GET_MAX_HIT(ch) += number(9, 20);
    send_to_char("&15You gain some bodily strength from that situp!&00\r\n", ch);
   }
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  return;
}
  if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) && GET_MANA(ch) >= GET_MAX_MANA(ch)/200)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/200, GET_MAX_MANA(ch)/200);
  EXP = number(GET_HIT(ch)/190, GET_HIT(ch)/190) + (GET_LEVEL(ch) * 10);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  if (number(1, 30) == 15) {
    GET_MAX_HIT(ch) += number(9, 20);
    send_to_char("&15You gain some bodily strength from that situp!&00\r\n", ch);
   }
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200) && GET_MANA(ch) >= GET_MAX_MANA(ch)/150)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/150, GET_MAX_MANA(ch)/150);
  EXP = number(GET_HIT(ch)/180, GET_HIT(ch)/180) + (GET_LEVEL(ch) * 15);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  if (number(1, 30) == 15) {
    GET_MAX_HIT(ch) += number(9, 20);
    send_to_char("&15You gain some bodily strength from that situp!&00\r\n", ch);
   }
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300) && GET_MANA(ch) >= GET_MAX_MANA(ch)/100)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/100, GET_MAX_MANA(ch)/100);
  EXP = number(GET_HIT(ch)/160, GET_HIT(ch)/160) + (GET_LEVEL(ch) * 20);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  if (number(1, 30) == 15) {
    GET_MAX_HIT(ch) += number(9, 20);
    send_to_char("&15You gain some bodily strength from that situp!&00\r\n", ch);
   }
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400) && GET_MANA(ch) >= GET_MAX_MANA(ch)/75)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/75, GET_MAX_MANA(ch)/75);
  EXP = number(GET_HIT(ch)/155, GET_HIT(ch)/155) + (GET_LEVEL(ch) * 25);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  if (number(1, 30) == 15) {
    GET_MAX_HIT(ch) += number(9, 20);
    send_to_char("&15You gain some bodily strength from that situp!&00\r\n", ch);
   }
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  return;
}
if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) && GET_MANA(ch) >= GET_MAX_MANA(ch)/250)
      {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some situps.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/250, GET_MAX_MANA(ch)/250);
  EXP = number(GET_HIT(ch)/100, GET_HIT(ch)/100);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
  return;
    }
   else if (GET_MANA(ch) >= GET_MAX_MANA(ch)/300 &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV100) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV200) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV300) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV400) &&
            !IS_SET(ROOM_FLAGS(ch->in_room), ROOM_SPIRIT))
   {
send_to_char("You do some situps and feel yourself getting stronger.\r\n", ch);
act("&11$n&00 does some &12situps&00.", TRUE, ch, 0, 0, TO_ROOM);

  MANA = number(GET_MAX_MANA(ch)/300, GET_MAX_MANA(ch)/300);
  EXP = number(GET_HIT(ch)/200, GET_HIT(ch)/200);

  GET_MANA(ch) = GET_MANA(ch) - MANA;
  GET_EXP(ch) = GET_EXP(ch) + EXP;
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
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
  /* >scan
     You quickly scan the area.
     You see John, a large horse and Frank close by north.
     You see a small rabbit a ways off south.
     You see a huge dragon and a griffon far off to the west.

  */
  int door;
  
  *buf = '\0';
  
  if (IS_AFFECTED(ch, AFF_SOLAR_FLARE) && !AFF_FLAGGED(ch, AFF_INFRAVISION)) {
    send_to_char("You can't see a damned thing, you're blind!\r\n", ch);
    return;
  }
  /* may want to add more restrictions here, too */
  send_to_char("You quickly scan the area.\r\n", ch);
  for (door = 0; door < NUM_OF_DIRS - 2; door++) /* don't scan up/down */
    if (EXIT(ch, door) && EXIT(ch, door)->to_room != NOWHERE &&
	!IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED)) {
      if (world[EXIT(ch, door)->to_room].people) {
	list_scanned_chars(world[EXIT(ch, door)->to_room].people, ch, 0, door);
      } else if (_2ND_EXIT(ch, door) && _2ND_EXIT(ch, door)->to_room != 
		 NOWHERE && !IS_SET(_2ND_EXIT(ch, door)->exit_info, EX_CLOSED)
                 ) {
   /* check the second room away */
	if (world[_2ND_EXIT(ch, door)->to_room].people) {
	  list_scanned_chars(world[_2ND_EXIT(ch, door)->to_room].people, ch, 1, door);
	} else if (_3RD_EXIT(ch, door) && _3RD_EXIT(ch, door)->to_room !=
		   NOWHERE && !IS_SET(_3RD_EXIT(ch, door)->exit_info, EX_CLOSED)
                   ) {
	  /* check the third room */
	  if (world[_3RD_EXIT(ch, door)->to_room].people) {
	    list_scanned_chars(world[_3RD_EXIT(ch, door)->to_room].people, ch, 2, door);
	  }

	}
      }
    }                
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

if (PRF_FLAGGED(ch, PRF_CAMP)) {
      send_to_char("You can't do this while camped!\r\n", ch);
      return;
    }
if (!IS_NPC(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= GET_LEVEL(ch)*2000) {
GET_LEVEL(ch) += 1;
GET_EXP(ch) = 1;
num_levels++;                                                                   advance_level(ch);                           
is_altered = TRUE;
send_to_char("&15You seem to have &10g&00&02r&16o&00&02w&10n &15surprisingly &09s&00&02t&16r&15o&00&07n&16g&00&01e&09r&15 with that pushup&11!&00\r\n", ch);
}
   if (IS_SET(ROOM_FLAGS(ch->in_room), ROOM_GRAV10))
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
ACMD(do_top)
{
  FILE *fl;
  struct char_file_u player;
  int done=FALSE, done2=FALSE, done3=FALSE, done4=FALSE, done5=FALSE, done6=FALSE, done7=FALSE, done8=FALSE, done9=FALSE, done10=FALSE, finished=FALSE, pass=FALSE;
  char buf[MAX_STRING_LENGTH];
  int num1 = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0, num6 = 0, num7 = 0, num8 = 0, num9 = 0, num10 = 0, pker = 0, pker2 = 0;
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh. Report to the immortals immeadiatly.\r\n", ch);
    return;
  }
  sprintf(buf, "&00&01S&09t&16r&15o&00n&15g&16e&09s&00&01t &14P&00&06l&16a&15y&16e&00&06r&14s &16on Resurrection of &00&04T&15r&00&06u&15t&00&04h&00\r\n\r\n");
  while (!done)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      done = TRUE;
    }
    if (!done)
      if (player.level <= 999999) {
       if ((num1 > 0 || num2 > 0 || num3 > 0 || num4 > 0 || num5 > 0 || num6 > 0 || num7 > 0 || num8 > 0 || num9 > 0 || num10 > 0) && pass == FALSE) {
        pass = TRUE;
       }
      if (pass == FALSE) {
       if (player.points.max_hit >= num1) {
        num1 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num2) {
        num2 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num3) {
        num3 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num4) {
        num4 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num5) {
        num5 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num6) {
        num6 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num7) {
        num7 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num8) {
        num8 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num9) {
        num9 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num10) {
        num10 = player.points.max_hit;
       }
       if (player.height >= pker) {
        pker = player.height;
        pker2 = player.weight;
       }
      }
       if (pass == TRUE) {
       if (player.points.max_hit >= num1) {
        num10 = num9;
        num9 = num8;
        num8 = num7;
        num7 = num6;
        num6 = num5;
        num5 = num4;
        num4 = num3;
        num3 = num2;
        num2 = num1;
        num1 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num2) {
        num10 = num9;
        num9 = num8;
        num8 = num7;
        num7 = num6;
        num6 = num5;
        num5 = num4;
        num4 = num3;
        num3 = num2;
        num2 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num3) {
        num10 = num9;
        num9 = num8;
        num8 = num7;
        num7 = num6;
        num6 = num5;
        num5 = num4;
        num4 = num3;
        num3 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num4) {
        num10 = num9;
        num9 = num8;
        num8 = num7;
        num7 = num6;
        num6 = num5;
        num5 = num4;
        num4 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num5) {
        num10 = num9;
        num9 = num8;
        num8 = num7;
        num7 = num6;
        num6 = num5;
        num5 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num6) {
        num10 = num9;
        num9 = num8;
        num8 = num7;
        num7 = num6;
        num6 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num7) {
        num10 = num9;
        num9 = num8;
        num8 = num7;
        num7 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num8) {
        num10 = num9;
        num9 = num8;
        num8 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num9) {
        num10 = num9;
        num9 = player.points.max_hit;
       }
       else if (player.points.max_hit >= num10) {
        num10 = player.points.max_hit;
       }
       if (player.height >= pker) {
        pker = player.height;
        pker2 = player.weight;
       }
       }
      }
  }
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh. Report to the immortals immeadiatly.\r\n", ch);
    return;
  }
  while (!done2)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      done2 = TRUE;
    }
    if (!done2)
      if (player.points.max_hit == num1) {
        sprintf(buf, "%s&10(&151&10)&00&06 %s&00\r\n", buf, player.name);
      }
  }
/*----------*/
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh. Report to the immortals immeadiatly.\r\n", ch);
    return;
  }
  while (!done3)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      done3 = TRUE;
    }
    if (!done3)
      if (player.points.max_hit == num2) {
        sprintf(buf, "%s&10(&152&10)&00&06 %s&00\r\n", buf, player.name);
      }
  }
/*----------*/
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh. Report to the immortals immeadiatly.\r\n", ch);
    return;
  }
  while (!done4)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      done4 = TRUE;
    }
    if (!done4)
      if (player.points.max_hit == num3) {
        sprintf(buf, "%s&10(&153&10)&00&06 %s&00\r\n", buf, player.name);
      }
  }
/*----------*/
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh. Report to the immortals immeadiatly.\r\n", ch);
    return;
  }
  while (!done5)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      done5 = TRUE;
    }
    if (!done5)
      if (player.points.max_hit == num4) {
        sprintf(buf, "%s&10(&154&10)&00&06 %s&00\r\n", buf, player.name);
      }
  }
/*----------*/
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh. Report to the immortals immeadiatly.\r\n", ch);
    return;
  }
  while (!done6)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      done6 = TRUE;
    }
    if (!done6)
      if (player.points.max_hit == num5) {
        sprintf(buf, "%s&10(&155&10)&00&06 %s&00\r\n", buf, player.name);
      }
  }
/*----------*/
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh. Report to the immortals immeadiatly.\r\n", ch);
    return;
  }
  while (!done7)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      done7 = TRUE;
    }
    if (!done7)
      if (player.points.max_hit == num6) {
        sprintf(buf, "%s&10(&156&10)&00&06 %s&00\r\n", buf, player.name);
      }
  }
/*----------*/
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh. Report to the immortals immeadiatly.\r\n", ch);
    return;
  }
  while (!done8)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      done8 = TRUE;
    }
    if (!done8)
      if (player.points.max_hit == num7) {
        sprintf(buf, "%s&10(&157&10)&00&06 %s&00\r\n", buf, player.name);
      }
  }
/*----------*/
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh. Report to the immortals immeadiatly.\r\n", ch);
    return;
  }
  while (!done9)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      done9 = TRUE;
    }
    if (!done9)
      if (player.points.max_hit == num8) {
        sprintf(buf, "%s&10(&158&10)&00&06 %s&00\r\n", buf, player.name);
      }
  }
/*----------*/
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh. Report to the immortals immeadiatly.\r\n", ch);
    return;
  }
  while (!done10)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      done10 = TRUE;
    }
    if (!done10)
      if (player.points.max_hit == num9) {
        sprintf(buf, "%s&10(&159&10)&00&06 %s&00\r\n", buf, player.name);
      }
  }
/*----------*/
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh. Report to the immortals immeadiatly.\r\n", ch);
    return;
  }
  while (!finished)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      finished = TRUE;
    }
    if (!finished)
      if (player.points.max_hit == num10) {
        sprintf(buf, "%s&10(&1510&10)&00&06 %s&00\r\n", buf, player.name);
      }
      if (player.height == pker && player.weight == pker2) {
        sprintf(buf2, "\r\n&00&06%s &09Pks: &15%d&00\r\n", player.name, player.height);
      }
  }
/*----------*/
  send_to_char(buf, ch);
        send_to_char("\r\n&00&01T&09o&16p &00&01P&09l&16ay&09e&00&01r &16K&09il&16le&09r&00\r\n", ch);
  send_to_char(buf2, ch);
  WAIT_STATE(ch, PULSE_VIOLENCE * .5);
}
ACMD(do_players)
{
  FILE *fl;
  struct char_file_u player;
  int done=FALSE;
  char buf[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
  int lowl, highl;

  two_arguments(argument, arg1, arg2);
  if (!*arg1 || !*arg2)
  { send_to_char("Must provide low and high total power.\r\n", ch);
    return;
  }
  else
  {
    lowl = atoi(arg1);
    highl = atoi(arg2);
  }
  if (!(fl = fopen(PLAYER_FILE, "r+")))
  { send_to_char("Can't open player file.  Uh oh.", ch);
    return;
  }
  sprintf(buf, "&11Player Name             &09Total power&00\r\n");
  while (!done)
  { fread(&player, sizeof(struct char_file_u), 1, fl);
    if (feof(fl))
    { fclose(fl);
      done = TRUE;
    }
    if (!done)
      if (player.points.max_hit + player.points.max_mana >= lowl && player.points.max_hit + player.points.max_mana <= highl)
        sprintf(buf, "%s%-20s %-2i\r\n", buf, player.name, player.points.max_hit + player.points.max_mana);
  }
  send_to_char(buf, ch);
}
