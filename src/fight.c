/*************************************************************************
*   File: fight.c                                       Part of CircleMUD *
*  Usage: Combat system                                                   *
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
#include "handler.h"
#include "interpreter.h"
#include "db.h"
#include "spells.h"
#include "screen.h"
#include "dg_scripts.h"

/* Structures */
struct char_data *combat_list = NULL;	/* head of l-list of fighting chars */
struct char_data *next_combat_list = NULL;

/* External structures */
extern struct index_data *mob_index;
extern struct str_app_type str_app[];
extern struct dex_app_type dex_app[];
extern struct room_data *world;
extern struct message_list fight_messages[MAX_MESSAGES];
extern struct obj_data *object_list;
extern int pk_allowed;		/* see config.c */
extern int auto_save;		/* see config.c -- not used in this file */
extern int max_exp_gain;	/* see config.c */
extern int max_exp_loss;	/* see config.c */
extern int top_of_world;
extern int max_npc_corpse_time, max_pc_corpse_time;
extern sh_int r_death_start_room;
extern sh_int r_icer_start_room;
extern sh_int r_demon_start_room;
extern sh_int r_majin_start_room;
extern sh_int r_saiyan_start_room;
extern sh_int r_angel_start_room;
extern sh_int r_konack_start_room;
extern sh_int r_namek_start_room;
extern sh_int r_truffle_start_room;
extern sh_int r_mortal_start_room ;
extern sh_int r_ctf_start_room;


/* External procedures */
char *fread_action(FILE * fl, int nr);
ACMD(do_flee);
ACMD(do_get);
ACMD(do_gen_door);
ACMD(do_split);
ACMD(do_drop);
int backstab_mult(int level);
int thaco(int ch_class, int level);
int ok_damage_shopkeeper(struct char_data * ch, struct char_data * victim);
int DAMG_DAM = 0;

/* local functions */
void perform_group_gain(struct char_data * ch, int base, struct char_data * victim);
void dam_message(int dam, struct char_data * ch, struct char_data * victim, int w_type);
void appear(struct char_data * ch);
void load_messages(void);
void check_killer(struct char_data * ch, struct char_data * vict);
void make_corpse(struct char_data * ch);
void change_alignment(struct char_data * ch, struct char_data * victim);
void death_cry(struct char_data * ch);
void raw_kill(struct char_data * ch, struct char_data * killer);
void die(struct char_data * ch, struct char_data * killer);
void group_gain(struct char_data * ch, struct char_data * victim);
void solo_gain(struct char_data * ch, struct char_data * victim);
char *replace_string(const char *str, const char *weapon_singular, const char *weapon_plural);
void perform_violence(void);

/* Weapon attack texts */
struct attack_hit_type attack_hit_text[] =
{
  {"hit", "hits"},		/* 0 */
  {"sting", "stings"},
  {"whip", "whips"},
  {"slash", "slashes"},
  {"bite", "bites"},
  {"bludgeon", "bludgeons"},	/* 5 */
  {"crush", "crushes"},
  {"pound", "pounds"},
  {"claw", "claws"},
  {"maul", "mauls"},
  {"thrash", "thrashes"},	/* 10 */
  {"pierce", "pierces"},
  {"blast", "blasts"},
  {"punch", "punches"},
  {"stab", "stabs"}

};

#define IS_WEAPON(type) (((type) >= TYPE_HIT) && ((type) < TYPE_SUFFERING))

/* The Fight related routines */
void appear(struct char_data * ch)
{ 
  if (affected_by_spell(ch, SPELL_INVISIBLE))
    affect_from_char(ch, SPELL_INVISIBLE);

  REMOVE_BIT(AFF_FLAGS(ch), AFF_INVISIBLE | AFF_HIDE);

  if (GET_LEVEL(ch) < LVL_IMMORT)
    act("$n slowly fades into existence.", FALSE, ch, 0, 0, TO_ROOM);
  else
    act("You feel a strange presence as $n appears, seemingly from nowhere.",
	FALSE, ch, 0, 0, TO_ROOM);
}



void load_messages(void)
{
  FILE *fl;
  int i, type;
  struct message_type *messages;
  char chk[128];

  if (!(fl = fopen(MESS_FILE, "r"))) {
    sprintf(buf2, "SYSERR: Error reading combat message file %s", MESS_FILE);
    perror(buf2);
    exit(1);
  }
  for (i = 0; i < MAX_MESSAGES; i++) {
    fight_messages[i].a_type = 0;
    fight_messages[i].number_of_attacks = 0;
    fight_messages[i].msg = 0;
  }


  fgets(chk, 128, fl);
  while (!feof(fl) && (*chk == '\n' || *chk == '*'))
    fgets(chk, 128, fl);

  while (*chk == 'M') {
    fgets(chk, 128, fl);
    sscanf(chk, " %d\n", &type);
    for (i = 0; (i < MAX_MESSAGES) && (fight_messages[i].a_type != type) &&
	 (fight_messages[i].a_type); i++);
    if (i >= MAX_MESSAGES) {
      log("SYSERR: Too many combat messages.  Increase MAX_MESSAGES and recompile.");
      exit(1);
    }
    CREATE(messages, struct message_type, 1);
    fight_messages[i].number_of_attacks++;
    fight_messages[i].a_type = type;
    messages->next = fight_messages[i].msg;
    fight_messages[i].msg = messages;

    messages->die_msg.attacker_msg = fread_action(fl, i);
    messages->die_msg.victim_msg = fread_action(fl, i);
    messages->die_msg.room_msg = fread_action(fl, i);
    messages->miss_msg.attacker_msg = fread_action(fl, i);
    messages->miss_msg.victim_msg = fread_action(fl, i);
    messages->miss_msg.room_msg = fread_action(fl, i);
    messages->hit_msg.attacker_msg = fread_action(fl, i);
    messages->hit_msg.victim_msg = fread_action(fl, i);
    messages->hit_msg.room_msg = fread_action(fl, i);
    messages->god_msg.attacker_msg = fread_action(fl, i);
    messages->god_msg.victim_msg = fread_action(fl, i);
    messages->god_msg.room_msg = fread_action(fl, i);
    fgets(chk, 128, fl);
    while (!feof(fl) && (*chk == '\n' || *chk == '*'))
      fgets(chk, 128, fl);
  }

  fclose(fl);
}

void update_pos(struct char_data * victim)
{
  if ((GET_HIT(victim) > 0) && (GET_POS(victim) > POS_STUNNED))
    return;
  else if (GET_HIT(victim) <= -11)             
    GET_POS(victim) = POS_DEAD;
  else if (GET_HIT(victim) <= -6)
    GET_POS(victim) = POS_MORTALLYW;
  else if (GET_HIT(victim) <= -3)
    GET_POS(victim) = POS_INCAP;
  else
    GET_POS(victim) = POS_STUNNED;     
}


void check_killer(struct char_data * ch, struct char_data * vict)
{
  if (!PLR_FLAGGED(vict, PLR_KILLER) && !PLR_FLAGGED(ch, PLR_KILLER) && !IS_NPC(ch) && !IS_NPC(vict) &&
      (ch != vict)) {
    char buf[256];

    SET_BIT(PLR_FLAGS(ch), PLR_KILLER);
    sprintf(buf, "PC Killer bit set on %s for initiating attack on %s at %s.",
	    GET_NAME(ch), GET_NAME(vict), world[vict->in_room].name);
    mudlog(buf, BRF, LVL_IMMORT, TRUE);
    send_to_char("If you want to be a PLAYER KILLER, so be it...\r\n", ch);
  }
}


/* start one char fighting another (yes, it is horrible, I know... )  */
void set_fighting(struct char_data * ch, struct char_data * vict)
{
  if (ch == vict)
    return;

  if (FIGHTING(ch)) {
    core_dump();
    return;
  }

  ch->next_fighting = combat_list;
  combat_list = ch;

  FIGHTING(ch) = vict;
  GET_POS(ch) = POS_FIGHTING;

  if (!pk_allowed)
    check_killer(ch, vict);
}


void unentangle(struct char_data *ch)
{
  static struct affected_type *af, *next;
  extern char *spell_wear_off_msg[];

  for (af = ch->affected; af; af = next) {
    next = af->next;
    if (af->bitvector == AFF_MFROZEN) {
      send_to_char(spell_wear_off_msg[af->type], ch);
      send_to_char("\r\n", ch);
    }    
    affect_remove(ch, af);
    act("$n is free of the mind freeze.",TRUE,ch,0,0,TO_ROOM);
  }
}

/* remove a char from the list of fighting chars */
void stop_fighting(struct char_data * ch)
{
  struct char_data *temp;
  struct char_data *vict;

  if (ch == next_combat_list)
    next_combat_list = ch->next_fighting;

  REMOVE_FROM_LIST(ch, combat_list, next_fighting);
  ch->next_fighting = NULL;
  FIGHTING(ch) = NULL;
   if (PRF_FLAGGED(ch, PRF_FLYING)) {
      GET_POS(ch) = POS_FLOATING;
      update_pos(ch);
      }
  else 
   GET_POS(ch) = POS_STANDING;
   update_pos(ch);
}



void make_corpse(struct char_data * ch)
{
  struct obj_data *corpse, *o;
  struct obj_data *money;
  int i;

  corpse = create_obj();

  corpse->item_number = NOTHING;
  corpse->in_room = NOWHERE;
  corpse->name = str_dup("corpse");

  sprintf(buf2, "&00&07The corpse of %s&00&07 is lying here.", GET_NAME(ch));
  corpse->description = str_dup(buf2);

  sprintf(buf2, "&00&07the corpse of %s&00&07", GET_NAME(ch));
  corpse->short_description = str_dup(buf2);

  GET_OBJ_TYPE(corpse) = ITEM_CONTAINER;
  GET_OBJ_WEAR(corpse) = ITEM_WEAR_TAKE;
  GET_OBJ_EXTRA(corpse) = ITEM_NODONATE;
  GET_OBJ_VAL(corpse, 0) = 0;	/* You can't store stuff in a corpse */
  GET_OBJ_VAL(corpse, 3) = 1;	/* corpse identifier */
  GET_OBJ_WEIGHT(corpse) = GET_WEIGHT(ch) + IS_CARRYING_W(ch);
  GET_OBJ_RENT(corpse) = 100000;
  if (IS_NPC(ch))
    GET_OBJ_TIMER(corpse) = max_npc_corpse_time;
  else
    GET_OBJ_TIMER(corpse) = max_pc_corpse_time;

  /* transfer character's inventory to the corpse */
   if ((!IS_NPC(ch)))
    {
      act("&15You have died&11!&00", TRUE, ch, 0, 0, TO_CHAR);
      return;
    }
 else  
  corpse->contains = ch->carrying;
  for (o = corpse->contains; o != NULL; o = o->next_content)
    o->in_obj = corpse;
  object_list_new_owner(corpse, NULL);

  /* transfer character's equipment to the corpse */
  for (i = 0; i < NUM_WEARS; i++)
    if (GET_EQ(ch, i))
      obj_to_obj(unequip_char(ch, i), corpse);

  /* transfer gold */
  if (GET_GOLD(ch) > 0) {
    /* following 'if' clause added to fix gold duplication loophole */
    if (IS_NPC(ch) || (!IS_NPC(ch) && ch->desc)) {
      money = create_money(GET_GOLD(ch));
      obj_to_obj(money, corpse);
    }
    GET_GOLD(ch) = 0;
  }
  ch->carrying = NULL;
  IS_CARRYING_N(ch) = 0;
  IS_CARRYING_W(ch) = 0;

  obj_to_room(corpse, ch->in_room);
}


/* When ch kills victim */
void change_alignment(struct char_data * ch, struct char_data * victim)
{
  /*
   * new alignment change algorithm: if you kill a monster with alignment A,
   * you move 1/16th of the way to having alignment -A.  Simple and fast.
   */
  GET_ALIGNMENT(ch) = GET_ALIGNMENT(ch);
}



void death_cry(struct char_data * ch)
{
  int door, was_in;

  act("&00&07You feel $n's&00&07 powerlevel fade away.&00", FALSE, ch, 0, 0, TO_ROOM);
  was_in = ch->in_room;

  for (door = 0; door < NUM_OF_DIRS; door++) {
    if (CAN_GO(ch, door)) {
      ch->in_room = world[was_in].dir_option[door]->to_room;
      act("You feel a nearby powerlevel fade away.", FALSE, ch, 0, 0, TO_ROOM);
      ch->in_room = was_in;
    }
  }
}


void raw_kill(struct char_data * ch, struct char_data * killer)
{

if (ROOM_FLAGGED(ch->in_room, ROOM_UNUSED)) {
  char_from_room(ch);
  char_to_room(ch, r_ctf_start_room);
  look_at_room(ch, 0);       
  return;
  }

  if (FIGHTING(ch))
    stop_fighting(ch);
  while (ch->affected)
    affect_remove(ch, ch->affected);
  if (killer) {
    if (death_mtrigger(ch, killer))
      death_cry(ch);
  } else  
      death_cry(ch);
  make_corpse(ch);
  char_from_room(ch);
  char_to_room(ch, r_death_start_room);
  GET_HIT(ch) = GET_MAX_HIT(ch)*0.1;
  GET_MANA(ch) = GET_MAX_MANA(ch)*0.1;
  if (IS_MAJIN(ch)) {
  SET_BIT(PLR_FLAGS(ch), PLR_FORELOCK);
  }
  if (IS_saiyan(ch)) {
  SET_BIT(PLR_FLAGS(ch), PLR_STAIL);  
  }
  if (IS_HALF_BREED(ch)) {
  SET_BIT(PLR_FLAGS(ch), PLR_STAIL);  
  }
  if (IS_icer(ch)) {
  SET_BIT(PLR_FLAGS(ch), PLR_TAIL);  
  } 
  if (IS_BIODROID(ch)) {
  SET_BIT(PLR_FLAGS(ch), PLR_TAIL);
  }
  GET_POS(ch) = POS_STANDING;
  look_at_room(ch, 0);
  
  if (IS_NPC(ch) || GET_HIT(ch) <= -11) 
   extract_char(ch, FALSE);
  
}

void die(struct char_data * ch, struct char_data * killer)
{
   struct char_data * victim;
   gain_exp(ch, -(GET_EXP(ch) / 2));
   raw_kill(ch, killer);
}



void perform_group_gain(struct char_data * ch, int base,
			     struct char_data * victim)
{
  char exp1[50];
  int share;

  share = MIN(max_exp_gain * 2 / 3, MAX(1, base));
 if (!IS_NPC(ch)) {
   commafmt(exp1, sizeof(exp1), share);
  if (share > 1) {
    sprintf(buf2, "&15You receive your share of experience &16-- &14%s &15points&16.&00\r\n", exp1);
    REMOVE_BIT(AFF_FLAGS(ch), AFF_HALTED);
    send_to_char(buf2, ch);
  } else
    send_to_char("&15You receive your share of experience &16--&15 one measly little point&16!&00\r\n", ch);
    REMOVE_BIT(AFF_FLAGS(ch), AFF_HALTED);
 }
 if (!IS_NPC(ch)) { 
   gain_exp(ch, share);
   change_alignment(ch, victim);
  }
}


void group_gain(struct char_data * ch, struct char_data * victim)
{
  int tot_members, base, tot_gain, add_hp = 0, add_mana = 0, add_lp = 0;
  struct char_data *k;
  struct follow_type *f;

  if (!(k = ch->master))
    k = ch;
  if (!IS_NPC(ch)) {
  if (AFF_FLAGGED(k, AFF_GROUP) && (k->in_room == ch->in_room))
    tot_members = 1;
  else
    tot_members = 0;

  for (f = k->followers; f; f = f->next)
    if (AFF_FLAGGED(f->follower, AFF_GROUP) && f->follower->in_room == ch->in_room)
      tot_members++;

  /* round up to the next highest tot_members */
  tot_gain = (GET_EXP(victim)) + tot_members - 1;

  /* prevent illegal xp creation when killing players */
  if (!IS_NPC(victim))
    tot_gain = MIN(max_exp_loss / 2, tot_gain);

  if (tot_members > 1)
    base = MAX(1, tot_gain / tot_members * 1.75);
  else
    base = MAX(1, tot_gain / tot_members);
  if (AFF_FLAGGED(k, AFF_GROUP) && k->in_room == ch->in_room && !IS_NPC(ch))
    perform_group_gain(k, base, victim);

  for (f = k->followers; f; f = f->next)
    if (AFF_FLAGGED(f->follower, AFF_GROUP) && f->follower->in_room == ch->in_room && !IS_NPC(ch))
      perform_group_gain(f->follower, base, victim);
 }
   if (IS_Human(ch) && GET_LEVEL(ch) <= 1000000) {
  switch (number(1, 30)) {
   case 1:
    add_hp += number(20, 30);
    sprintf(buf, "&16[&14Human &09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
    break;
   case 2:
   case 3:
   case 4:
    break;
   case 5:
    add_hp += number(20, 30);
    sprintf(buf, "&16[&14Human &09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
    break;
   case 6:
    break;
   case 7:
    add_hp += number(20, 30);
    sprintf(buf, "&16[&14Human &09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
    break;
   case 8:
   case 9:
   case 10:
    break;
   case 11:
    add_hp += number(31, 40);
    sprintf(buf, "&16[&14Human &09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
    break;
   case 12:
   case 13:
   case 14:
   case 15:
   case 16:
   case 17:
    break;
   case 18:
    add_mana += number(20, 30);
    sprintf(buf, "&16[&14Human &14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
    break;
   case 19:
    add_mana += number(20, 30);
    sprintf(buf, "&16[&14Human &14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
    break;
   case 20:
    add_mana += number(20, 30);
    sprintf(buf, "&16[&14Human &14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
    break;
   case 21:
   case 22:
    break;
   case 23:
    add_mana += number(31, 40);
    sprintf(buf, "&16[&14Human &14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
    break;
   case 24:
   case 25:
    break;
   case 26:
    add_mana += number(31, 40);
    sprintf(buf, "&16[&14Human &14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
    break;
   case 27:
   case 28:
   case 29:
    break;
   case 30:
    add_lp += number(1, 5);
    sprintf(buf, "&16[&14Human &10LP &00&06Bonus&11: &15%d &16]&00\r\n", add_lp);
        send_to_char(buf, ch);
    break;
   default:
    break;
  }
    GET_PRACTICES(ch) = GET_PRACTICES(ch) + add_lp;
    GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + add_hp;
    GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + add_mana;
  }
  if (IS_ANDROID(ch) && GET_HIT(ch) >= GET_MAX_HIT(ch) / 2) {
   switch (number(1, 80)) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
     break;
    case 12:
     send_to_char("&16[&09O&00&01V&09E&00&01R&16D&15R&00&07I&15V&16E &10BONUS&11!!!&16]&00\r\n", ch);
     GET_HIT(ch) = GET_MAX_HIT(ch) * 2;
     break;
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
     break;
    case 30:
     send_to_char("&16[&09O&00&01V&09E&00&01R&16D&15R&00&07I&15V&16E &10BONUS&11!!!&16]&00\r\n", ch);
     GET_HIT(ch) = GET_MAX_HIT(ch) * 2;
     break;
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
     break;
    case 48:
     send_to_char("&16[&09O&00&01V&09E&00&01R&16D&15R&00&07I&15V&16E &10BONUS&11!!!&16]&00\r\n", ch);
     GET_HIT(ch) = GET_MAX_HIT(ch) * 2;
     break;
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
     break;
    default:
     break;
    }
  }
   if (!IS_Human(ch) && GET_LEVEL(ch) <= 1000000 && !IS_NPC(ch)) {
  switch (number(1, 60)) {
  case 1:
   add_hp += number(20, 30);
   sprintf(buf, "&16[&09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
   break;
  case 2:
  case 3:
  case 4:
   break;
  case 5:
   add_hp += number(31, 40);
   sprintf(buf, "&16[&09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
   break;
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
   break;
  case 11:
   add_hp += number(41, 50);
   sprintf(buf, "&16[&09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
   break;
  case 12:
  case 13:
  case 14:
  case 15:
  case 16:
  case 17:
   break;
  case 18:
   add_mana += number(20, 30);
   sprintf(buf, "&16[&14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
   break;
  case 19:
  case 20:
  case 21:
  case 22:
  case 23:
  case 24:
  case 25:
  case 26:
  case 27:
   break;
  case 28:
   add_mana += number(31, 40);
   sprintf(buf, "&16[&14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
   break;
  case 29:
  case 30:
  case 31:
  case 32:
  case 33:
  case 34:
  case 35:
  case 36:
  case 37:
   break;
  case 38:
   add_mana += number(41, 50);
   sprintf(buf, "&16[&14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
   break;
  case 39:
  case 40:
  case 41:
  case 42:
  case 43:
  case 44:
  case 45:
  case 46:
  case 47:
  case 48:
  case 49:
   break;
  case 50:
   add_lp += number(1, 5);
   sprintf(buf, "&16[&10LP &00&06Bonus&11: &15%d &16]&00\r\n", add_lp);
        send_to_char(buf, ch);
   break;
  case 51:
  case 52:
  case 53:
   break;
  case 54:
   add_lp += number(1, 5);
   sprintf(buf, "&16[&10LP &00&06Bonus&11: &15%d &16]&00\r\n", add_lp);
        send_to_char(buf, ch);
   break;
  case 55:
  case 56:
  case 57:
  case 58:
  case 59:
  case 60:
   break;
  default:
   break;
  }
  GET_PRACTICES(ch) = GET_PRACTICES(ch) + add_lp;
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + add_hp;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + add_mana;
  }
  if (IS_Namek(ch)) {
   switch (number(1, 40)) {
    case 1:
     GET_HIT(ch) = GET_MAX_HIT(ch);
     GET_MANA(ch) = GET_MAX_MANA(ch);
     send_to_char("&16[&10N&00&02a&10m&00&02e&10k &16R&00&02e&10g&00&02e&10n &12Bonus&16]", ch);
     break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
     break;
    case 14:
     GET_HIT(ch) = GET_MAX_HIT(ch);
     GET_MANA(ch) = GET_MAX_MANA(ch);
     send_to_char("&16[&10N&00&02a&10m&00&02e&10k &16R&00&02e&10g&00&02e&10n &12Bonus&16]", ch);
     break;
    case 15:
    case 16:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 28:
    case 29:
     break;
    case 30:
     GET_HIT(ch) = GET_MAX_HIT(ch);
     GET_MANA(ch) = GET_MAX_MANA(ch);
     send_to_char("&16[&10N&00&02a&10m&00&02e&10k &16R&00&02e&10g&00&02e&10n &12Bonus&16]", ch);
     break;
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
     break;
    }
   }
  if (!IS_NPC(ch) && !IS_Namek(ch)) {
  switch (number(1, 120)) {

  case 1:
   if (!PRF_FLAGGED(ch, PRF_AUTOEXIT)) {
    GET_MANA(ch) = GET_MAX_MANA(ch);
    send_to_char("&12Restore &15Bonus&00!\r\n", ch);
   }
   break;
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
  case 15:
  case 16:
  case 17:
  case 18:
  case 19:
   break;
  case 20:
    GET_HIT(ch) = GET_MAX_HIT(ch);
    send_to_char("&10Restore &15Bonus&00!\r\n", ch);
   break;
  case 21:
  case 22:
  case 23:
  case 24:
  case 25:
  case 26:
  case 27:
   break;
  case 28:
   GET_HIT(ch) = GET_MAX_HIT(ch);
    send_to_char("&10Restore &15Bonus&00!\r\n", ch);
   if (!PRF_FLAGGED(ch, PRF_AUTOEXIT)) {
   GET_MANA(ch) = GET_MAX_MANA(ch);
    send_to_char("&12Restore &15Bonus&00!\r\n", ch);
   }
   break;
  case 29:
  case 30:
  case 31:
  case 32:
  case 33:
  case 34:
  case 35:
  case 36:
  case 37:
  case 38:
  case 39:
  case 40:
   break;
  case 41:
  case 42:
  case 43:
  case 44:
  case 45:
  case 46:
  case 47:
  case 48:
  case 49:
  case 50:
  case 51:
  case 52:
  case 53:
  case 54:
  case 55:
  case 56:
  case 57:
  case 58:
  case 59:
  case 60:
  case 61:
  case 62:
  case 63:
  case 64:
  case 65:
  case 66:
  case 67:
  case 68:
  case 69:
  case 70:
  case 71:
  case 72:
  case 73:
  case 74:
  case 75:
  case 76:
  case 77:
  case 78:
  case 79:
  case 80:
  case 81:
  case 82:
  case 83:
  case 84:
  case 85:
  case 86:
  case 87:
  case 88:
  case 89:
  case 90:
  case 91:
  case 92:
  case 93:
  case 94:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
  case 100:
  case 101:
  case 102:
  case 103:
  case 104:
  case 105:
  case 106:
  case 107:
  case 108:
  case 109:
  case 110:
  case 111:
  case 112:
  case 113:
  case 114:
  case 115:
  case 116:
  case 117:
  case 118:
  case 119:
  case 120:
   break;
  default:
   break;
  }
 }
}


void solo_gain(struct char_data * ch, struct char_data * victim)
{
  char exp1[50];
  int exp, add_hp, add_mana, add_lp;
  exp = MIN(max_exp_gain, GET_EXP(victim));
  
  exp = MAX(1, exp);
  add_hp = 0;
  add_mana = 0;
  add_lp = 0;
 if (!IS_NPC(ch)) { 
  if (GET_CHA(ch) >= 40 && GET_CHA(ch) <= 79) {
    exp = exp * 1.25;
    }
  if (GET_CHA(ch) >= 80) {
    exp = exp * 1.5;
    }
     commafmt(exp1, sizeof(exp1), exp);
  if (exp > 1) {
    sprintf(buf2, "&15You receive &14%s &15experience points&16.&00\r\n", exp1);
    REMOVE_BIT(AFF_FLAGS(ch), AFF_HALTED);
    send_to_char(buf2, ch);
  } else
    send_to_char("&15You receive one lousy experience point&16.&00\r\n", ch);
    REMOVE_BIT(AFF_FLAGS(ch), AFF_HALTED);
  gain_exp(ch, exp);
  change_alignment(ch, victim);
  }
 if (IS_Human(ch) && GET_LEVEL(ch) <= 1000000) {
  switch (number(1, 30)) {
   case 1:
    add_hp += number(20, 30);
    sprintf(buf, "&16[&14Human &09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
    break;
   case 2:
   case 3:
   case 4:
    break;
   case 5:
   case 6:
    break;
   case 7:
    add_hp += number(20, 30);
    sprintf(buf, "&16[&14Human &09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
    break;
   case 8:
   case 9:
   case 10:
    break;
   case 11:
    add_hp += number(31, 40);
    sprintf(buf, "&16[&14Human &09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
    break;
   case 12:
   case 13:
   case 14:
   case 15:
   case 16:
   case 17:
    break;
   case 18:
    add_mana += number(20, 30);
    sprintf(buf, "&16[&14Human &14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
    break;
   case 19:
   case 20:
   case 21:
   case 22:
    break;
   case 23:
    add_mana += number(31, 40);
    sprintf(buf, "&16[&14Human &14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
    break;
   case 24:
   case 25:
    break;
   case 26:
    add_mana += number(31, 40);
    sprintf(buf, "&16[&14Human &14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
    break;
   case 27:
   case 28:
   case 29:
    break;
   case 30:
    add_lp += number(1, 5);
    sprintf(buf, "&16[&14Human &10LP &00&06Bonus&11: &15%d &16]&00\r\n", add_lp);
        send_to_char(buf, ch);
    break;
   default:
    break;
  }
    GET_PRACTICES(ch) = GET_PRACTICES(ch) + add_lp;
    GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + add_hp;
    GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + add_mana;
  }
  if (IS_ANDROID(ch) && GET_HIT(ch) >= GET_MAX_HIT(ch) / 2) {
   switch (number(1, 60)) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
     break;
    case 12:
     send_to_char("&16[&09O&00&01V&09E&00&01R&16D&15R&00&07I&15V&16E &10BONUS&11!!!&16]&00\r\n", ch);
     GET_HIT(ch) = GET_MAX_HIT(ch) * 2;
     break;
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
     break;
    case 30:
     send_to_char("&16[&09O&00&01V&09E&00&01R&16D&15R&00&07I&15V&16E &10BONUS&11!!!&16]&00\r\n", ch);
     GET_HIT(ch) = GET_MAX_HIT(ch) * 2;
     break;
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
     break;
    case 48:
     send_to_char("&16[&09O&00&01V&09E&00&01R&16D&15R&00&07I&15V&16E &10BONUS&11!!!&16]&00\r\n", ch);
     GET_HIT(ch) = GET_MAX_HIT(ch) * 2;
     break;
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
     break;
    default:
     break;
    }
  }
 if (!IS_Human(ch) && GET_LEVEL(ch) <= 1000000 && !IS_NPC(ch)) {
  switch (number(1, 60)) {
  case 1:
   add_hp += number(20, 30);
   sprintf(buf, "&16[&09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
   break;
  case 2:
  case 3:
  case 4:
   break;
  case 5:
   add_hp += number(31, 40);
   sprintf(buf, "&16[&09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
   break;
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
   break;
  case 11:
   add_hp += number(41, 50);
   sprintf(buf, "&16[&09PL &00&06Bonus&11: &15%d &16]&00\r\n", add_hp);
        send_to_char(buf, ch);
   break;
  case 12:
  case 13:
  case 14:
  case 15:
  case 16:
  case 17:
   break;
  case 18:
   add_mana += number(20, 30);
   sprintf(buf, "&16[&14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
   break;
  case 19:
  case 20:
  case 21:
  case 22:
  case 23:
  case 24:
  case 25:
  case 26:
  case 27:
   break;
  case 28:
   add_mana += number(31, 40);
   sprintf(buf, "&16[&14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
   break;
  case 29:
  case 30:
  case 31:
  case 32:
  case 33:
  case 34:
  case 35:
  case 36:
  case 37:
   break;
  case 38:
   add_mana += number(41, 50);
   sprintf(buf, "&16[&14Ki &00&06Bonus&11: &15%d &16]&00\r\n", add_mana);
        send_to_char(buf, ch);
   break;
  case 39:
  case 40:  
  case 41:
  case 42:
  case 43:
  case 44:
  case 45:
  case 46:
  case 47:
  case 48:
  case 49:
   break;
  case 50:
   add_lp += number(1, 5);
   sprintf(buf, "&16[&10LP &00&06Bonus&11: &15%d &16]&00\r\n", add_lp);
        send_to_char(buf, ch);
   break;
  case 51:
  case 52:
  case 53:
   break;
  case 54:
   add_lp += number(1, 5);
   sprintf(buf, "&16[&10LP &00&06Bonus&11: &15%d &16]&00\r\n", add_lp);
        send_to_char(buf, ch);
   break;
  case 55:
  case 56:
  case 57:
  case 58:
  case 59:
  case 60:
   break;
  default:
   break;
  }
  GET_PRACTICES(ch) = GET_PRACTICES(ch) + add_lp;
  GET_MAX_HIT(ch) = GET_MAX_HIT(ch) + add_hp;
  GET_MAX_MANA(ch) = GET_MAX_MANA(ch) + add_mana;
 }
 if (!IS_NPC(ch)) {
  switch (number(1, 50)) {
  
  case 1:
    if (!PRF_FLAGGED(ch, PRF_AUTOEXIT)) {
    GET_MANA(ch) = GET_MAX_MANA(ch);
    send_to_char("&12Restore &15Bonus&00!\r\n", ch);
   }
   break;
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
  case 15:
  case 16:
  case 17:
  case 18:
  case 19:
   break;
  case 20:
    GET_HIT(ch) = GET_MAX_HIT(ch);
    send_to_char("&10Restore &15Bonus&00!\r\n", ch);
   break;
  case 21:
  case 22:
  case 23:
  case 24:
  case 25:
  case 26:
  case 27:
   break;
  case 28:
   GET_HIT(ch) = GET_MAX_HIT(ch);
    send_to_char("&10Restore &15Bonus&00!\r\n", ch);   
   if (!PRF_FLAGGED(ch, PRF_AUTOEXIT)) {
   GET_MANA(ch) = GET_MAX_MANA(ch);
    send_to_char("&12Restore &15Bonus&00!\r\n", ch);
   }
   break;
  case 29:
  case 30:
  case 31:
  case 32:
  case 33:
  case 34:
  case 35:
  case 36:
  case 37:
  case 38:
  case 39:
  case 40:
   break;
  case 41:
  case 42:
  case 43:
  case 44:
  case 45:
  case 46:
  case 47:
  case 48:
  case 49:
  case 50:
   break;
  default:
   break;
  }
 }
}


char *replace_string(const char *str, const char *weapon_singular, const char *weapon_plural)
{
  static char buf[256];
  char *cp = buf;

  for (; *str; str++) {
    if (*str == '#') {
      switch (*(++str)) {
      case 'W':
	for (; *weapon_plural; *(cp++) = *(weapon_plural++));
	break;
      case 'w':
	for (; *weapon_singular; *(cp++) = *(weapon_singular++));
	break;
      default:
	*(cp++) = '#';
	break;
      }
    } else
      *(cp++) = *str;

    *cp = 0;
  }				/* For */

  return (buf);
}


/* message for doing damage with a weapon */
void dam_message(int dam, struct char_data * ch, struct char_data * victim,
		      int w_type)
{
  char *buf;
  int msgnum, door, was_in;

  static struct dam_weapon_type {
    const char *to_room;
    const char *to_char;
    const char *to_victim;
  } dam_weapons[] = {

    /* use #w for singular (i.e. "slash") and #W for plural (i.e. "slashes") */

    {
      "&09$n&15 tries to #w &09$N&15, but misses.&00",	/* 0: 0     */
      "&15You try to #w &09$N&15, but miss.&00",
      "&09$n&15 tries to #w you, but misses.&00"
    },

    {
      "&09$n&15 scratches &09$N&15 in the &14right arm&15 as $e #W $M.&00",	/* 1:  */
      "&15You scratch &09$N&15 in the &14right arm&15 as you #w $M.&00",
      "&09$n&15 scratches you in the &14right arm&15 as $e #W you.&00"
    },
    
    {
      "&09$n&15 scratches &09$N&15 in the &14left arm&15 as $e #W $M.&00",      /* 2:  */
      "&15You scratch &09$N&15 in the &14left arm&15 as you #w $M.&00",
      "&09$n&15 scratches you in the &14left arm&15 as $e #W you.&00"
    },


    {
      "&09$n&15 scratches &09$N&15 in the &14right leg&15 as $e #W $M.&00",    /* 3:  */
      "&15You scratch &09$N&15 in the &14right leg&15 as you #w $M.&00",
      "&09$n&15 scratches you in the &14right leg&15 as $e #W you.&00"
    },
    
    {
      "&09$n&15 scratches &09$N&15 in the &14left leg&15 as $e #W $M.&00",     /* 4:  */
      "&15You scratch &09$N&15 in the &14left leg&15 as you #w $M.&00",
      "&09$n&15 scratches you in the &14left leg&15 as $e #W you.&00"
    },
    
    {
      "&09$n&15 scratches &09$N&15 in the &14chest&15 as $e #W $M.&00",	      /* 5:   */
      "&15You scratch &09$N&15 in the &14chest&15 as you #w $M.&00",
      "&09$n&15 scratches you in the &14chest&15 as $e #W you.&00"
    },
    
    {
      "&09$n&15 scratches &09$N&15 in the &14face&15 as $e #W $M.&00",        /* 6:   */
      "&15You scratch &09$N&15 in the &14face&15 as you #w $M.&00",
      "&09$n&15 scratches you in the &14face&15 as $e #W you.&00"
    },
    
    {
      "&09$n&15 barely #W &09$N&15 in the &14right arm&15.&00",		      /* 7:   */
      "&15You barely #w &09$N&15 in the &14right arm&15.&00",
      "&09$n&15 barely #W you in the &14right arm&15.&00"
    },
    
    {
      "&09$n&15 barely #W &09$N&15 in the &14left arm&15.&00",		      /* 8:   */
      "&15You barely #w &09$N&15 in the &14left arm&15.&00",
      "&09$n&15 barely #W you in the &14left arm&15.&00"
    },

    {
      "&09$n&15 barely #W &09$N&15 in the &14right leg&15.&00",		      /* 9:   */
      "&15You barely #w &09$N&15 in the &14right leg&15.&00",
      "&09$n&15 barely #W you in the &14right leg&15.&00"
    },

    {
      "&09$n&15 barely #W &09$N&15 in the &14left leg&15.&00",		      /* 10:   */
      "&15You barely #w &09$N&15 in the &14left leg&15.&00",
      "&09$n&15 barely #W you in the &14left leg&15.&00"
    },

    {
      "&09$n&15 barely #W &09$N&15 in the &14chest&15.&00",		      /* 11:   */
      "&15You barely #w &09$N&15 in the &14chest&15.&00",
      "&09$n&15 barely #W you in the &14chest&15.&00"
    },

    {
      "&09$n&15 barely #W &09$N&15 in the &14face&15.&00",		      /* 12:   */
      "&15You barely #w &09$N&15 in the &14face&15.&00",
      "&09$n&15 barely #W you in the &14face&15.&00"
    },

    {
      "&12$n&15 #W &09$N&15 in the &14right arm&15.&00",		      /* 13    */
      "&15You #w &09$N&15 in the &14right arm&15.&00",
      "&09$n&15 #W you in &14right arm&15.&00"
    },

    {
      "&12$n&15 #W &09$N&15 in the &14left arm&15.&00",		              /* 14    */
      "&15You #w &09$N&15 in the &14left arm&15.&00",
      "&09$n&15 #W you in &15left arm&15.&00"
    },
    
    {
      "&12$n&15 #W &09$N&15 in the &14right leg&15.&00",		      /* 15    */
      "&15You #w &09$N&15 in the &14right leg&15.&00",
      "&09$n&15 #W you in &14right leg&15.&00"
    },
    
    {
      "&12$n&15 #W &09$N&15 in the &14left leg&15.&00",		             /* 16    */
      "&15You #w &09$N&15 in the &14left leg&15.&00",
      "&09$n&15 #W you in &14left leg&15.&00"
    },

    {
      "&12$n&15 #W &09$N&15 in the &14chest&15.&00",		             /* 17    */
      "&15You #w &09$N&15 in the &14chest&15.&00",
      "&09$n&15 #W you in &14chest&15.&00"
    },

    {
      "&12$n&15 #W &09$N&15 in the &14face&15.&00",		             /* 18    */
      "&15You #w &09$N&15 in the &14face&15.&00",
      "&09$n&15 #W you in &14face&15.&00"
    },

    {
      "&09$n&15 #W &09$N&15 hard in the &14right arm&15.&00",	             /* 19    */
      "&15You #w &09$N&15 hard in the &14right arm&15.&00",
      "&09$n&15 #W you hard in the &14right arm&15.&00"
    },
    
    {
      "&09$n&15 #W &09$N&15 hard in the &14left arm&15.&00",	             /* 20    */
      "&15You #w &09$N&15 hard in the &14left arm&15.&00",
      "&09$n&15 #W you hard in the &14left arm&15.&00"
    },
    
    {
      "&09$n&15 #W &09$N&15 hard in the &14right leg&15.&00",	             /* 21    */
      "&15You #w &09$N&15 hard in the &14right leg&15.&00",
      "&09$n&15 #W you hard in the &14right leg&15.&00"
    },
    
    {
      "&09$n&15 #W &09$N&15 hard in the &14left leg&15.&00",	             /* 22    */
      "&15You #w &09$N&15 hard in the &14left leg&15.&00",
      "&09$n&15 #W you hard in the &14left leg&15.&00"
    },
    
    {
      "&09$n&15 #W &09$N&15 hard in the &14chest&15.&00",	             /* 23    */
      "&15You #w &09$N&15 hard in the &14chest&15.&00",
      "&09$n&15 #W you hard in the &14chest&15.&00"
    },
    
    {
      "&09$n&15 #W &09$N&15 hard in the &14face&15.&00",	             /* 24    */
      "&15You #w &09$N&15 hard in the &14face&15.&00",
      "&09$n&15 #W you hard in the &14face&15.&00"
    },
    
    {
      "&09$n&15 #W &09$N&15 very hard in the &14right arm&15.&00",          /* 25    */
      "&15You #w &09$N&15 very hard in the &14right arm&15.&00",
      "&09$n&15 #W you very hard in the &14right arm&15.&00"
    },

    {
      "&09$n&15 #W &09$N&15 very hard in the &14left arm&15.&00",          /* 26     */
      "&15You #w &09$N&15 very hard in the &14left arm&15.&00",
      "&09$n&15 #W you very hard in the &14left arm&15.&00"
    },

    {
      "&09$n&15 #W &09$N&15 very hard in the &14right leg&15.&00",         /* 27     */
      "&15You #w &09$N&15 very hard in the &14right leg&15.&00",
      "&09$n&15 #W you very hard in the &14right leg&15.&00"
    },

    {
      "&09$n&15 #W &09$N&15 very hard in the &14left leg&15.&00",          /* 28     */
      "&15You #w &09$N&15 very hard in the &14left leg &15.&00",
      "&09$n&15 #W you very hard in the &14left leg&15.&00"
    },
    
    {
      "&09$n&15 #W &09$N&15 very hard in the &14chest&15.&00",            /* 29     */
      "&15You #w &09$N&15 very hard in the &14chest&15.&00",
      "&09$n&15 #W you very hard in the &14chest&15.&00"
    },
    
    {
      "&09$n&15 #W &09$N&15 very hard in the &14face&15.&00",            /* 30     */
      "&15You #w &09$N&15 very hard in the &14face&15.&00",
      "&09$n&15 #W you very hard in the &14face&15.&00"
    },

    {
      "&09$n&15 #W &09$N&15 extremely hard in the &14right arm&15.&00",	/* 31      */
      "&15You #w &09$N&15 extremely hard in the &14right arm&15.&00",
      "&09$n&15 #W you extremely hard in the &14right arm&15.&00"
    },

    {
      "&09$n&15 #W &09$N&15 extremely hard in the &14left arm&15.&00",	/* 32      */
      "&15You #w &09$N&15 extremely hard in the &14left arm&15.&00",
      "&09$n&15 #W you extremely hard in the &14left arm&15.&00"
    },
   
    {
      "&09$n&15 #W &09$N&15 extremely hard in the &14right leg&15.&00",	/* 33      */
      "&15You #w &09$N&15 extremely hard in the &14right leg&15.&00",
      "&09$n&15 #W you extremely hard in the &14right leg&15.&00"
    },
    
    {
      "&09$n&15 #W &09$N&15 extremely hard in the &14left leg&15.&00",	/* 34      */
      "&15You #w &09$N&15 extremely hard in the &14left leg&15.&00",
      "&09$n&15 #W you extremely hard in the &14left leg&15.&00"
    },
    
    {
      "&09$n&15 #W &09$N&15 extremely hard in the &14chest&15.&00",	/* 35      */
      "&15You #w &09$N&15 extremely hard in the &14chest&15.&00",
      "&09$n&15 #W you extremely hard in the &14chest&15.&00"
    },
    
    {
      "&09$n&15 #W &09$N&15 extremely hard in the &14face&15.&00",	/* 36      */
      "&15You #w &09$N&15 extremely hard in the &14face&15.&00",
      "&09$n&15 #W you extremely hard in the &14face&15.&00"
    }
  };


  w_type -= TYPE_HIT;		/* Change to base of table with text */

  if (dam == 0)		msgnum = 0;
  else if (dam <= GET_MAX_HIT(victim) / 90) {
    switch (number(1, 6)) {
     case 1:
      msgnum = 1;
      break;
     case 2:
      msgnum = 2;
      break;
     case 3:
      msgnum = 3;
      break;
     case 4:
      msgnum = 4;
      break;
     case 5:
      msgnum = 5;
      break;
     case 6:
      msgnum = 6;
      break;
     }
  }
  else if (dam <= GET_MAX_HIT(victim) / 80) {
    switch (number(1, 6)) {
     case 1:
      msgnum = 7;
      break;
     case 2:
      msgnum = 8;
      break;
     case 3:
      msgnum = 9;
      break;
     case 4:
      msgnum = 10;
      break;
     case 5:
      msgnum = 11;
      break;
     case 6:
      msgnum = 12;
      break;
     }
  }
  else if (dam <= GET_MAX_HIT(victim) / 65) {
    switch (number(1, 6)) {
     case 1:
      msgnum = 13;
      break;
     case 2:
      msgnum = 14;
      break;
     case 3:
      msgnum = 15;
      break;
     case 4:
      msgnum = 16;
      break;
     case 5:
      msgnum = 17;
      break;
     case 6:
      msgnum = 18;
      break;
     }
  }
  else if (dam <= GET_MAX_HIT(victim) / 50) {
    switch (number(1, 6)) {
     case 1:
      msgnum = 19;
      break;
     case 2:
      msgnum = 20;
      break;
     case 3:
      msgnum = 21;
      break;
     case 4:
      msgnum = 22;
      break;
     case 5:
      msgnum = 23;
      break;
     case 6:
      msgnum = 24;
      break;
     }
  }
  else if (dam < GET_MAX_HIT(victim) / 30) {
    switch (number(1, 6)) {
     case 1:
      msgnum = 25;
      break;
     case 2:
      msgnum = 26;
      break;
     case 3:
      msgnum = 27;
      break;
     case 4:
      msgnum = 28;
      break;
     case 5:
      msgnum = 29;
      break;
     case 6:
      msgnum = 30;
      break;
     }
  }
  else if (dam >= GET_MAX_HIT(victim) / 30) {
    switch (number(1, 6)) {
     case 1:
      msgnum = 31;
      break;
     case 2:
      msgnum = 32;
      break;
     case 3:
      msgnum = 33;
      break;
     case 4:
      msgnum = 34;
      break;
     case 5:
      msgnum = 35;
      break;
     case 6:
      msgnum = 36;
      break;
     }
  }

  else			msgnum = 36;

  /* damage message to onlookers */
      if (CLN_FLAGGED(ch, CLN_ADEF) && IS_NPC(victim)) {
       REMOVE_BIT(CLN_FLAGS(ch), CLN_ADEF);
      }
      else if (CLN_FLAGGED(victim, CLN_ADEF) && !IS_NPC(ch)) {
       switch (number(1, 5)) {
        case 1:
         act("&15With lighting fast speed you block the attack.", FALSE, ch, 0, victim, TO_VICT);
         act("&14$N &15blocks &09$n's&15 attack with lightning fast speed!&00", FALSE, ch, 0, victim, TO_NOTVICT);
         act("&14$N&15 blocks your attack with lightning fast speed!", FALSE, ch, 0, victim, TO_CHAR);
         break;
        case 2:
         act("&15With lighting fast speed you dodge the attack, leaving an after-image behind.", FALSE, ch, 0, victim, TO_VICT);
         act("&14$N &15dodges &09$n's&15 attack with lightning fast speed, leaving behind an after-image!&00", FALSE, ch, 0, victim, TO_NOTVICT);
         act("&14$N &15dodge your  attack with lightning fast speed, leaving behind an after-image!", FALSE, ch, 0, victim, TO_CHAR);
         break;
        case 3:
         act("&15With a grin you direct &09$n's&15 attack to the ground and move out of the way!", FALSE, ch, 0, victim, TO_VICT);
         act("&15With a grin &14$N&15 directs &09$n's&15 attack to the ground and moves out of the way!", FALSE, ch, 0, victim, TO_NOTVICT);
         act("&15With a grin &14$N&15 directs your attack to the ground and moves out of the way!", FALSE, ch, 0, victim, TO_CHAR);
         break;
        case 4:
         act("&15You catch &09$n's&15 attack, ending its force harmlessly.", FALSE, ch, 0, victim, TO_VICT);
         act("&14$N&15 catches &09$n's&15 attack, ending its force harmlessly.", FALSE, ch, 0, victim, TO_NOTVICT);
         act("&14$N &15catches your attack, ending its force harmlessly.", FALSE, ch, 0, victim, TO_CHAR);
         break;
        case 5:
         act("&15You weave through &09$n's &15attacks as if they were in slow motion.", FALSE, ch, 0, victim, TO_VICT);
         act("&14$N &15weaves through &09$n's &15attacks as if they were in slow motion.", FALSE, ch, 0, victim, TO_NOTVICT);
         act("&14$N &15weaves through your attacks as if they were in slow motion.", FALSE, ch, 0, victim, TO_CHAR);
         break;
       }
      }
        else if (dam <= 1 && CLN_FLAGGED(ch, CLN_DODGE)) {
        }
        else if (dam <= 1 && CLN_FLAGGED(ch, CLN_BLOCK)) {
        }
  else if (dam >= 2) {
  buf = replace_string(dam_weapons[msgnum].to_room,
	  attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
  act(buf, FALSE, ch, NULL, victim, TO_NOTVICT);

   /* damage message to damager */
  send_to_char(CCYEL(ch, C_CMP), ch);
  buf = replace_string(dam_weapons[msgnum].to_char,
          attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
  sprintf(buf2, "%s &16[&10%d&16]&00", buf, dam);
  act(buf2, FALSE, ch, NULL, victim, TO_CHAR);
  send_to_char(CCNRM(ch, C_CMP), ch);
 /* damage message to damagee */
  send_to_char(CCRED(victim, C_CMP), victim);
  buf = replace_string(dam_weapons[msgnum].to_victim,
          attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
  sprintf(buf2, "%s &16[&09%d&16]&00", buf, dam);
  act(buf2, FALSE, ch, NULL, victim, TO_VICT | TO_SLEEP);
  send_to_char(CCNRM(victim, C_CMP), victim);
   was_in = ch->in_room;
  for (door = 0; door < NUM_OF_DIRS; door++) {
    if (CAN_GO(ch, door) && number(1, 12) == 6) {
      ch->in_room = world[was_in].dir_option[door]->to_room;
      act("The sounds of battle can be heard nearby!", FALSE, ch, 0, 0, TO_ROOM);
      ch->in_room = was_in;
    }
  }
 }
}


/*
 * message for doing damage with a spell or skill
 *  C3.0: Also used for weapon damage on miss and death blows
 */
int skill_message(int dam, struct char_data * ch, struct char_data * vict,
		      int attacktype)
{
  int i, j, nr;
  struct message_type *msg;

  struct obj_data *weap = GET_EQ(ch, WEAR_WIELD);

  for (i = 0; i < MAX_MESSAGES; i++) {
    if (fight_messages[i].a_type == attacktype) {
      nr = dice(1, fight_messages[i].number_of_attacks);
      for (j = 1, msg = fight_messages[i].msg; (j < nr) && msg; j++)
	msg = msg->next;

      if (!IS_NPC(vict) && (GET_LEVEL(vict) >= LVL_IMMORT)) {
	act(msg->god_msg.attacker_msg, FALSE, ch, weap, vict, TO_CHAR);
	act(msg->god_msg.victim_msg, FALSE, ch, weap, vict, TO_VICT);
	act(msg->god_msg.room_msg, FALSE, ch, weap, vict, TO_NOTVICT);
      } 
        else if (dam != 0) {
        if ((IS_NPC(ch) && IS_NPC(vict)) && GET_POS(vict) == POS_DEAD) {
          act("&14$n &15severely wounds &12$N&00", FALSE, ch, 0, vict, TO_NOTVICT);
          GET_HIT(vict) = -11;
        }
	else if (GET_POS(vict) == POS_DEAD) {
         switch (number(1, 7)) {
          case 1:
	  send_to_char(CCYEL(ch, C_CMP), ch);
	  act("&15You &09murder&15 $N&15 with a powerful blow to their head.&00", FALSE, ch, weap, vict, TO_CHAR);
	  send_to_char(CCNRM(ch, C_CMP), ch);

	  send_to_char(CCRED(vict, C_CMP), vict);
	  act("&14$n &09murders&15 you with a powerful blow to your head.&00", FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
	  send_to_char(CCNRM(vict, C_CMP), vict);

	  act("&14$n &09murders &10$N&15 with a powerful blow to their head.&00", FALSE, ch, weap, vict, TO_NOTVICT);
          break;
          case 2:
          send_to_char(CCYEL(ch, C_CMP), ch);
          act("&15You &09r&00&01i&09p &14$N's &15spine from their back, leaving their &09bloody&15 corpse on the ground.&00", FALSE, ch, weap, vict, TO_CHAR);
          send_to_char(CCNRM(ch, C_CMP), ch);

          send_to_char(CCRED(vict, C_CMP), vict);
          act("&14$n &09r&00&01i&09ps out your &15spine from your back, leaving you a &09bloody&15 corpse on the ground.&00", FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
          send_to_char(CCNRM(vict, C_CMP), vict);

          act("&14$n &09r&00&01i&09ps &14$N's &15spine from their back, leaving their &09bloody&15 corpse on the ground.&00", FALSE, ch, weap, vict, TO_NOTVICT);
          break;
          case 3:
          send_to_char(CCYEL(ch, C_CMP), ch);
          act("&15You &16b&00&01r&09e&00&01a&16k&14 $N's &15neck with a single blow, and leave their shattered corpse on the ground.&00", FALSE, ch, weap, vict, TO_CHAR);
          send_to_char(CCNRM(ch, C_CMP), ch);

          send_to_char(CCRED(vict, C_CMP), vict);
          act("&14$n &16b&00&01r&09e&00&01a&16ks&15 your &15neck with a single blow, and leaves your body as a shattered corpse on the ground..&00", FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
          send_to_char(CCNRM(vict, C_CMP), vict);

          act("&14$n &16b&00&01r&09e&00&01a&16ks&14 $N's &15neck with a single blow, and leaves their shattered corpse on the ground..&00",  FALSE, ch, weap, vict, TO_NOTVICT);
          break;
          case 4:
          send_to_char(CCYEL(ch, C_CMP), ch);
          act("&15You &16t&00&01e&09a&16r &14$N's &15arms off and beat them to death with them.&00", FALSE, ch, weap, vict, TO_CHAR);
          send_to_char(CCNRM(ch, C_CMP), ch);

          send_to_char(CCRED(vict, C_CMP), vict);
          act("&14$n &16t&00&01e&09a&16rs &15your &15arms off and beats you to death with them.&00", FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
          send_to_char(CCNRM(vict, C_CMP), vict);

          act("&14$n &16t&00&01e&09a&16rs &14$N's &15arms off and beats them to death with them.&00",  FALSE, ch, weap, vict, TO_NOTVICT);
          break;
          case 5:
          send_to_char(CCYEL(ch, C_CMP), ch);
          act("&15You &09r&00&01a&09m &15your fist through &14$N's&15 chest, and your bloody arm emerges through their back!&00", FALSE, ch, weap, vict, TO_CHAR);
          send_to_char(CCNRM(ch, C_CMP), ch);

          send_to_char(CCRED(vict, C_CMP), vict);
          act("&14$n &09r&00&01a&09m&16s &15$s fist through your&15 chest, and $s bloody arm emerges through your back!&00&00", FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
          send_to_char(CCNRM(vict, C_CMP), vict);

          act("&14$n &09r&00&01a&09m&16s &15$s fist through &14$N's&15 chest, and $n's bloody arm emerges through $N's&15 back!&00&00",  FALSE, ch, weap, vict, TO_NOTVICT);
          break;
          case 6:
          send_to_char(CCYEL(ch, C_CMP), ch);
          act("&14$N &15dies in agony as you rip their intestines out from their gut, viciously.&00", FALSE, ch, weap, vict, TO_CHAR);
          send_to_char(CCNRM(ch, C_CMP), ch);

          send_to_char(CCRED(vict, C_CMP), vict);
          act("&14You &15die in agony as $n rips your intestines out from your gut, viciously.&00", FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
          send_to_char(CCNRM(vict, C_CMP), vict);

          act("&14$N &15dies in agony as $n rips their intestines out from $N's&15 gut, viciously.&00",  FALSE, ch, weap, vict, TO_NOTVICT);
          break;
          case 7:
          send_to_char(CCYEL(ch, C_CMP), ch);
          act("&15You blow &14$N's&15 head off with a massive kishot, &00&07s&15p&16l&00&01a&09tte&00&01r&16i&15n&00&07g&15 brain matter everywhere.&00", FALSE, ch, weap, vict, TO_CHAR);
          send_to_char(CCNRM(ch, C_CMP), ch);

          send_to_char(CCRED(vict, C_CMP), vict);
          act("&14$n&15 blows your head off with a massive kishot, &00&07s&15p&16l&00&01a&09tte&00&01r&16i&15n&00&07g&15 brain matter everywhere.&00&00", FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
          send_to_char(CCNRM(vict, C_CMP), vict);

          act("&14$n&15 blows &14$N's&15 head off with a massive kishot, &00&07s&15p&16l&00&01a&09tte&00&01r&16i&15n&00&07g&15 brain matter everywhere.&00&00",  FALSE, ch, weap, vict, TO_NOTVICT);
          break;
          default:
          send_to_char(CCYEL(ch, C_CMP), ch);
          act("&15You &09murder&15 $N&15 with a powerful blow to their head.&00", FALSE, ch, weap, vict, TO_CHAR);
          send_to_char(CCNRM(ch, C_CMP), ch);

          send_to_char(CCRED(vict, C_CMP), vict);
          act("&14$n &09murders&15 you with a powerful blow to your head.&00", FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
          send_to_char(CCNRM(vict, C_CMP), vict);

          act("&14$n &09murders &10$N&15 with a powerful blow to their head.&00", FALSE, ch, weap, vict, TO_NOTVICT);
          break;
          }
	} else {
	  send_to_char(CCYEL(ch, C_CMP), ch);
	  act("", FALSE, ch, weap, vict, TO_CHAR);
	  send_to_char(CCNRM(ch, C_CMP), ch);

	  send_to_char(CCRED(vict, C_CMP), vict);
	  act("", FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
	  send_to_char(CCNRM(vict, C_CMP), vict);

	  act("", FALSE, ch, weap, vict, TO_NOTVICT);
	}
      } else if (ch != vict && dam <= 0 && AFF_FLAGGED(vict, AFF_PLATED)) {      /* Dam == 0 */
        send_to_char(CCYEL(ch, C_CMP), ch);
        act("&15You gasp in surprise as you break your hand on &14$N's&15 hard chest!&00", FALSE, ch, weap, vict, TO_CHAR);
        send_to_char(CCNRM(ch, C_CMP), ch);

        send_to_char(CCRED(vict, C_CMP), vict);
        act("&14$n&15 gasps in surprise as $e breaks $s hand on your chest!&00", FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
        send_to_char(CCNRM(vict, C_CMP), vict);

        act("&15&14$n&15 gasps in surprise as $e breaks $s hand on &09$N&15'&09s &15chest!&00", FALSE, ch, weap, vict, TO_NOTVICT);
      }
        else if (ch != vict && dam <= 0) {	/* Dam == 0 */
	send_to_char(CCYEL(ch, C_CMP), ch);
	act("&15You feel stupid as you &12miss &14$N&15 with your hit!&00", FALSE, ch, weap, vict, TO_CHAR);
	send_to_char(CCNRM(ch, C_CMP), ch);

	send_to_char(CCRED(vict, C_CMP), vict);
	act("&14$n feels stupid as they &12miss &15you with their hit!&00", FALSE, ch, weap, vict, TO_VICT | TO_SLEEP);
	send_to_char(CCNRM(vict, C_CMP), vict);

	act("&14$n feels stupid as they &12miss &14$N&15 with their hit!&00", FALSE, ch, weap, vict, TO_NOTVICT);
      }
      return 1;
     }
    }
  return 0;
}

/*
 * Alert: As of bpl14, this function returns the following codes:
 *	< 0	Victim died.
 *	= 0	No damage.
 *	> 0	How much damage done.
 */
int damage(struct char_data * ch, struct char_data * victim, int dam, int attacktype)
{
   ACMD(do_get);
   ACMD(do_split);
   struct obj_data *obj;
   struct obj_data *cont;
   int exp;
   long local_gold = 0;
   char local_buf[256];
  if (GET_POS(victim) <= POS_DEAD) {
    log("SYSERR: Attempt to damage corpse '%s' in room #%d by '%s'.",
		GET_NAME(victim), GET_ROOM_VNUM(IN_ROOM(victim)), GET_NAME(ch));
    die(victim, ch);
    return 0;			/* -je, 7/7/92 */
  }
  /* peaceful rooms */
  if (ch != victim && ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL)) {
    send_to_char("This room just has such a peaceful, easy feeling...\r\n", ch);
    return 0;
  }
  /* Frozen */
  if (IS_AFFECTED(ch, AFF_MFROZEN)) {
    send_to_char("You struggle against your mind freeze...\r\n", ch);
    return 0;
  }
  if (PLR_FLAGGED(victim, PLR_WRITING)) {
    return 0;
  }
  /* shopkeeper protection */
  if (!ok_damage_shopkeeper(ch, victim))
    return 0;

  /* You can't damage an immortal! */

  if (victim != ch) {
    /* Start the attacker fighting the victim */
    if (GET_POS(ch) > POS_STUNNED && (FIGHTING(ch) == NULL))
      set_fighting(ch, victim);

    /* Start the victim fighting the attacker */
    if (GET_POS(victim) > POS_STUNNED && (FIGHTING(victim) == NULL)) {
      set_fighting(victim, ch);
      if (MOB_FLAGGED(victim, MOB_MEMORY) && !IS_NPC(ch))
	remember(victim, ch);
    }
  }

  /* If you attack a pet, it hates your guts */
  if (victim->master == ch)
    stop_follower(victim);

  /* If the attacker is invisible, he becomes visible */
  if (AFF_FLAGGED(ch, AFF_INVISIBLE | AFF_HIDE))
    appear(ch);

  /* Cut damage in half if victim has sanct, to a minimum 1 */
  if (AFF_FLAGGED(victim, AFF_BARRIER) && dam >= 2)
    dam = dam / 2;
  if (GET_POS(victim) == POS_STUNNED && dam >= 2)
    dam = dam / 6;
  /* Check for PK if this is not a PK MUD */
  if (!pk_allowed) {
    check_killer(ch, victim);
    if (PLR_FLAGGED(ch, PLR_KILLER) && (ch != victim))
      dam = 0;
  }

  if (!IS_NPC(ch) && dam >= 10000) {
     switch (number(1, 2)) {
      case 1:
       dam += number(1, 4000);
       break;
      case 2:
       dam -= number(1, 4000);
       break;
      default:
       dam -= number(1, 500);
       break;
     }
    }
  if (CLN_FLAGGED(victim, CLN_ADEF) && !IS_NPC(ch)) {
       dam = 1;
       GET_HIT(victim) -= dam;
       update_pos(victim);
     }
  if (AFF_FLAGGED(victim, AFF_PLATED) && !PRF_FLAGGED(ch, PRF_NOHASSLE)) {
           dam = 0;
        }
  else if (!IS_NPC(victim) && CLN_FLAGGED(victim, CLN_DODGE) && GET_MANA(victim) >= GET_MAX_MANA(victim) / 20) {
      switch (number(1, 3)) {
      case 1:
         act("&15You manage to dodge &09$n's&15 attack.", FALSE, ch, 0, victim, TO_VICT);
         act("&14$N&15 manages to dodge &09$N's&15 attack.", FALSE, ch, 0, victim, TO_NOTVICT);
         act("&14$N&15 manages to dodge your attack.", FALSE, ch, 0, victim, TO_CHAR);
       dam = 1;
       GET_HIT(victim) -= dam;
       GET_MANA(victim) -= GET_MAX_MANA(victim) / 30;
       update_pos(victim);
       break;
      case 2:
       dam = dam / 4;
       dam = MAX(MIN(dam, 1500000), 0);
       GET_HIT(victim) -= dam;
       update_pos(victim);
       break;
      case 3:
       dam = dam / 8;
       dam = MAX(MIN(dam, 1500000), 0);
       GET_HIT(victim) -= dam;
       update_pos(victim);
       break;
      }
     }
  else if (!IS_NPC(victim) && CLN_FLAGGED(victim, CLN_BLOCK) && GET_MANA(victim) >= GET_MAX_MANA(victim) / 20) {
     switch (number(1, 6)) {
      case 1:
         act("&15You manage to block &09$n's&15 attack.", FALSE, ch, 0, victim, TO_VICT);
         act("&14$N&15 manages to block &09$n's&15 attack.", FALSE, ch, 0, victim, TO_NOTVICT);
         act("&14$N&15 manages to block your attack.", FALSE, ch, 0, victim, TO_CHAR);
       dam = 0;
       GET_HIT(victim) -= dam;
       GET_MANA(victim) -= GET_MAX_MANA(victim) / 50;
       update_pos(victim);
       break;
      case 2:
       dam = MAX(MIN(dam, 1500000), 0);
       GET_HIT(victim) -= dam;
       update_pos(victim);
       break;
      case 3:
       dam = MAX(MIN(dam, 1500000), 0);
       GET_HIT(victim) -= dam;
       update_pos(victim);
       break;
      case 4:
       dam = MAX(MIN(dam, 1500000), 0);
       GET_HIT(victim) -= dam;
       update_pos(victim);
       break;
      case 5:
       dam = MAX(MIN(dam, 1500000), 0);
       GET_HIT(victim) -= dam;
       update_pos(victim);
       break;
      case 6:
       dam = MAX(MIN(dam, 1500000), 0);
       GET_HIT(victim) -= dam;
       update_pos(victim);
       break;
      }
     }
  else if (PRF_FLAGGED(ch, PRF_NOHASSLE)) {
      dam = MAX(MIN(dam, 5000000), 0);
      GET_HIT(victim) -= dam;
      update_pos(victim);
     }
  else if (AFF_FLAGGED(ch, AFF_FRENZY) && !PRF_FLAGGED(ch, PRF_NOHASSLE)) {
           dam *= 2;
           dam = MAX(MIN(dam, 3000000), 0);
           GET_HIT(victim) -= dam;
           update_pos(victim);
        }
  else if (AFF_FLAGGED(victim, AFF_YOIKOMINMINKEN)) {
      affect_from_char(victim, SPELL_YOIKOMINMINKEN);
      dam = dam * 4;
      dam = MAX(MIN(dam, 5000000), 0);
      GET_HIT(victim) -= dam;
      update_pos(victim);
     }
  else if (AFF_FLAGGED(ch, AFF_SOLAR_FLARE)) {
      dam = MAX(MIN(dam, 1500000), 0);
      GET_HIT(victim) -= dam;
      update_pos(victim);
    }
  else if (GET_DEX(ch) >= 60 && number(1, 20) == 10 && dam >= 2 && (IS_demon(ch) || IS_BIODROID(ch))) {
      dam = dam * 2;
      dam = MAX(MIN(dam, 2500000), 0);
      GET_HIT(victim) -= dam;
      act("&16[&14$n &09critical &00&01hits &10YOU&11!!!&16]", TRUE, ch, 0, victim, TO_VICT);
      act("&16[&10|-------<&09Critical &00&01Hit&11!&10>-------|&16]", TRUE, ch, 0, victim, TO_CHAR);
      if (ch != victim)

      update_pos(victim);
     }
  else if (GET_DEX(ch) >= 60 && number(1, 20) == 10 && dam >= 2) {
      dam = dam * 2;
      dam = MAX(MIN(dam, 2000000), 0);
      GET_HIT(victim) -= dam;
      act("&16[&14$n &09critical &00&01hits &10YOU&11!!!&16]", TRUE, ch, 0, victim, TO_VICT);
      act("&16[&10|-------<&09Critical &00&01Hit&11!&10>-------|&16]", TRUE, ch, 0, victim, TO_CHAR);
      if (ch != victim)

      update_pos(victim);
     }
  else if (GET_DEX(ch) >= 30 && GET_DEX(ch) < 60 && number(1, 30) == 10  && dam >= 2 && (IS_demon(ch) || IS_BIODROID(ch))) {
      dam = dam * 2;
      dam = MAX(MIN(dam, 2500000), 0);
      GET_HIT(victim) -= dam;
      act("&16[&14$n &09critical &00&01hits &10YOU&11!!!&16]", TRUE, ch, 0, victim, TO_VICT);
      act("&16[&10|-------<&09Critical &00&01Hit&11!&10>-------|&16]", TRUE, ch, 0, victim, TO_CHAR);
      if (ch != victim)

      update_pos(victim);
     }
  else if (GET_DEX(ch) >= 30 && GET_DEX(ch) < 60 && number(1, 30) == 10  && dam >= 2) {
      dam = dam * 2;
      dam = MAX(MIN(dam, 2000000), 0);
      GET_HIT(victim) -= dam;
      act("&16[&14$n &09critical &00&01hits &10YOU&11!!!&16]", TRUE, ch, 0, victim, TO_VICT);
      act("&16[&10|-------<&09Critical &00&01Hit&11!&10>-------|&16]", TRUE, ch, 0, victim, TO_CHAR);
      if (ch != victim)

      update_pos(victim);
     }
  else if (GET_DEX(ch) <= 30 && number(1, 40) == 10 && dam >= 2) {
      dam = dam * 2;
      dam = MAX(MIN(dam, 2000000), 0);
      GET_HIT(victim) -= dam;
      act("&16[&14$n &09critical &00&01hits &10YOU&11!!!&16]", TRUE, ch, 0, victim, TO_VICT);
      act("&16[&10|-------<&09Critical &00&01Hit&11!&10>-------|&16]", TRUE, ch, 0, victim, TO_CHAR);
      if (ch != victim)

      update_pos(victim);
     }
     else
      if (AFF_FLAGGED(ch, AFF_CHARGEL)) {
        dam = dam + 100000;
        dam = MAX(MIN(dam, 3000000), 0);
        act("&14$n &15crackles with charged energy&11!&00\r\n", TRUE, ch, 0, victim, TO_VICT);
        act("&14$n &15crackles with charged energy&11!&00\r\n", TRUE, ch, 0, victim, TO_NOTVICT);
        act("&15You &15crackle with charged energy&11!&00\r\n", TRUE, ch, 0, victim, TO_CHAR);
        GET_HIT(victim) -= dam;
        if (ch != victim)

        update_pos(victim);
        REMOVE_BIT(AFF_FLAGS(ch), AFF_CHARGEL);
       }
      else if (AFF_FLAGGED(ch, AFF_CHARGEM)) {
        dam = dam + 400000;
        dam = MAX(MIN(dam, 3000000), 0);
        act("&14$n &15crackles with charged energy&11!&00\r\n", TRUE, ch, 0, victim, TO_VICT);
        act("&14$n &15crackles with charged energy&11!&00\r\n", TRUE, ch, 0, victim, TO_NOTVICT);
        act("&15You &15crackle with charged energy&11!&00\r\n", TRUE, ch, 0, victim, TO_CHAR);
        GET_HIT(victim) -= dam;
        if (ch != victim)

        update_pos(victim);
        REMOVE_BIT(AFF_FLAGS(ch), AFF_CHARGEM);
       }
      else if (AFF_FLAGGED(ch, AFF_CHARGEH)) {
        dam = dam + 800000;
        dam = MAX(MIN(dam, 3000000), 0);
        act("&14$n &15crackles with charged energy&11!&00\r\n", TRUE, ch, 0, victim, TO_VICT);
        act("&14$n &15crackles with charged energy&11!&00\r\n", TRUE, ch, 0, victim, TO_NOTVICT);
        act("&15You &15crackle with charged energy&11!&00\r\n", TRUE, ch, 0, victim, TO_CHAR);
        GET_HIT(victim) -= dam;
        if (ch != victim)

        update_pos(victim);
        REMOVE_BIT(AFF_FLAGS(ch), AFF_CHARGEH);
       }
     else if (IS_demon(ch) || IS_BIODROID(ch)) {
      dam = MAX(MIN(dam, 2000000), 0);
      GET_HIT(victim) -= dam;
      update_pos(victim);
     }
      else {
       dam = MAX(MIN(dam, 1500000), 0);
       GET_HIT(victim) -= dam;
       if (ch != victim)

       update_pos(victim);
      }
  /* Set the maximum damage per round and subtract the hit points */
  /* Gain exp for the hit */

  /*
   * skill_message sends a message from the messages file in lib/misc.
   * dam_message just sends a generic "You hit $n extremely hard.".
   * skill_message is preferable to dam_message because it is more
   * descriptive.
   * 
   * If we are _not_ attacking with a weapon (i.e. a spell), always use
   * skill_message. If we are attacking with a weapon: If this is a miss or a
   * death blow, send a skill_message if one exists; if not, default to a
   * dam_message. Otherwise, always send a dam_message.
   */
  if (!IS_WEAPON(attacktype))
    skill_message(dam, ch, victim, attacktype);
  else {
    if (GET_POS(victim) == POS_DEAD || dam == 0) {
      if (!skill_message(dam, ch, victim, attacktype))
	dam_message(dam, ch, victim, attacktype);
    } else {
      dam_message(dam, ch, victim, attacktype);
    }
  }

 if (GET_HIT(victim) <= -11 && IS_NPC(ch))                                                                 
{
if (IS_NPC(victim) && AFF_FLAGGED(ch, AFF_GROUP)) {
    GET_POS(victim) = POS_DEAD;
    GET_HIT(victim) = 0;
   }
  else if (IS_NPC(victim) && !AFF_FLAGGED(ch, AFF_GROUP)) {
   GET_POS(victim) = POS_DEAD;
   GET_HIT(victim) = 0;
   stop_fighting(ch); 
   stop_fighting(victim);
   make_corpse(victim);
   extract_char(victim, FALSE);
  }
else
send_to_char("&14You pass out, unable to continue fighting!&00\n\r", victim);
GET_POS(victim) = POS_STANDING;                                                 
GET_HIT(victim) = 10;                                                           
stop_fighting(ch);stop_fighting(victim);
 if (!IS_NPC(victim)) {
  char_from_room(victim);
   if (CLN_FLAGGED(victim, CLN_Death)) {
    char_to_room(victim, r_death_start_room );
   }
   else if (IS_icer(victim) || IS_MUTANT(victim)) {
    char_to_room(victim, r_icer_start_room );
   }
   else if (IS_saiyan(victim)) {
    char_to_room(victim, r_saiyan_start_room );
   }
   else if (IS_ANGEL(victim)) {
    char_to_room(victim, r_angel_start_room );
   }
   else if (IS_demon(victim)) {
    char_to_room(victim, r_demon_start_room );
   }
   else if (IS_MAJIN(victim)) {
    char_to_room(victim, r_majin_start_room );
   }
   else if (IS_KONATSU(victim)) {
    char_to_room(victim, r_konack_start_room );
   }
   else if (IS_Namek(victim)) {
    char_to_room(victim, r_namek_start_room );
   }
   else if (IS_TRUFFLE(victim)) {
    char_to_room(victim, r_truffle_start_room );
   }
   else {
    char_to_room(victim, r_mortal_start_room );
   }
  look_at_room(victim, 0);

  return dam;            
 }
}   

  /* Use send_to_char -- act() doesn't send message if you are DEAD. */
  switch (GET_POS(victim)) {
  case POS_MORTALLYW:
    act("$n is mortally wounded, and will die soon, if not aided.", TRUE, victim, 0, 0, TO_ROOM);
    send_to_char("You are mortally wounded, and will die soon, if not aided.\r\n", victim);
    break;
  case POS_INCAP:
    act("$n is incapacitated and will slowly die, if not aided.", TRUE, victim, 0, 0, TO_ROOM);
    send_to_char("You are incapacitated an will slowly die, if not aided.\r\n", victim);
    break;
  case POS_STUNNED:
    act("$n is stunned, but will probably regain consciousness again.", TRUE, victim, 0, 0, TO_ROOM);
    send_to_char("You're stunned, but will probably regain consciousness again.\r\n", victim);
    break;
  case POS_DEAD:
    send_to_char("You are dead!  You are sent to the Next Dimension.\r\n", victim);
    REMOVE_BIT(AFF_FLAGS(victim), AFF_HALTED);
    break;

  default:			/* >= POSITION SLEEPING */
    if (dam > (GET_MAX_HIT(victim) / 4))
      act("That really did HURT!", FALSE, victim, 0, 0, TO_CHAR);

    if (GET_HIT(victim) < (GET_MAX_HIT(victim) / 10)) {
      sprintf(buf2, "%sYou wish that your wounds would stop BLEEDING so much!%s\r\n",
	      CCRED(victim, C_SPR), CCNRM(victim, C_SPR));
      send_to_char(buf2, victim);
      if (ch != victim && MOB_FLAGGED(victim, MOB_WIMPY))
	do_flee(victim, NULL, 0, 0); 
    }
    if (!IS_NPC(victim) && GET_WIMP_LEV(victim) && (victim != ch) &&
	GET_HIT(victim) < GET_WIMP_LEV(victim) && GET_HIT(victim) > 0) {
      send_to_char("You wimp out, and attempt to flee!\r\n", victim);
      REMOVE_BIT(AFF_FLAGS(ch), AFF_HALTED);
      do_flee(victim, NULL, 0, 0);
    }
    break;
  }

  /* Help out poor linkless people who are attacked */
   for (obj = victim->carrying; obj; obj = obj->next_content) {
    if (IS_OBJ_STAT(obj, ITEM_FLAG) && !IS_NPC(victim) && !(victim->desc)) {
     return;
     }
    }
    if (!IS_NPC(victim) && !(victim->desc)) {
    do_flee(victim, NULL, 0, 0);
    /*if (!FIGHTING(victim)) {
      act("$n is rescued by divine forces.", FALSE, victim, 0, 0, TO_ROOM);
      GET_WAS_IN(victim) = victim->in_room;
      char_from_room(victim);
      char_to_room(victim, 0);
   }  */    
   }
  /* stop someone from fighting if they're stunned or worse */
  if ((GET_POS(victim) <= POS_STUNNED) && (FIGHTING(victim) != NULL))
    stop_fighting(victim);

  /* Uh oh.  Victim died. */
  if (GET_POS(victim) == POS_DEAD && !IS_NPC(ch)) {
    if ((ch != victim) && (IS_NPC(victim) || victim->desc)) {
      if (AFF_FLAGGED(ch, AFF_GROUP))
	group_gain(ch, victim);
      else
        solo_gain(ch, victim);
    }
    if (!IS_NPC(victim)) {
      for (obj = victim->carrying; obj; obj = obj->next_content) {
      if (GET_OBJ_TYPE(obj) == ITEM_dball1 || GET_OBJ_TYPE(obj) == ITEM_dball2 || GET_OBJ_TYPE(obj) == ITEM_dball3 || GET_OBJ_TYPE(obj) == ITEM_dball4 || GET_OBJ_TYPE(obj) == ITEM_dball5 || GET_OBJ_TYPE(obj) == ITEM_dball6 || GET_OBJ_TYPE(obj) == ITEM_dball7) {
        do_drop(victim,"dragonball",0,0);
        continue;
       }
      if (IS_OBJ_STAT(obj, ITEM_FLAG)) {
        do_drop(victim,"flag",0,0);
        continue;
       }
      else {
       continue;
       }
      }
      for (cont = victim->carrying; cont; cont = cont->next_content) {
       if (GET_OBJ_TYPE(cont) == ITEM_CONTAINER) {
          do_get(victim,"dragonball all",0,0);
          do_drop(victim,"dragonball",0,0);
          continue;
         }
      }
     /* do_drop(victim,"dragonball",0,0)*/
      sprintf(buf2, "%s killed by %s at %s", GET_NAME(victim), GET_NAME(ch),
	      world[victim->in_room].name);
      mudlog(buf2, BRF, LVL_IMMORT, TRUE);
      if (!PLR_FLAGGED(ch, PLR_KILLER) && !PLR_FLAGGED(victim, PLR_icer)) {
       sprintf(buf, "&16[&09Player Death&16] &14%s &15killed by &11%s&09!&00\r\n", GET_NAME(victim), GET_NAME(ch));
       SET_BIT(CLN_FLAGS(victim), CLN_Death);
       GET_DTIMER(victim) = 3600;
       send_to_all(buf);
       }
      else if (PLR_FLAGGED(victim, PLR_icer) && !PLR_FLAGGED(ch, PLR_KILLER)) {
         REMOVE_BIT(PLR_FLAGS(victim), PLR_icer);
         GET_GOLD(ch) = GET_GOLD(ch) + 200000;
         sprintf(buf, "\r\n&14BOUNTY&11:: &15The bounty on &09%s&15 has been collected "
                      "by &12%s&15!\r\n", GET_NAME(victim), GET_NAME(ch));
         SET_BIT(CLN_FLAGS(victim), CLN_Death);
         GET_DTIMER(victim) = 3600;
         send_to_all(buf);
       }
      else if (PLR_FLAGGED(ch, PLR_KILLER) && !PLR_FLAGGED(victim, PLR_icer)) {
       GET_GOLD(ch) += 10000;
       sprintf(buf1, "&16[&10Player &09K&00&01il&09l &11A&00&03ren&11a &00&07Win&11!&16]\r\n&14%s &15manages to &09k&00&01il&09l &00&06%s&15.  &14%s &15now has &09%d &00&07P&14l&00&06ay&16er &09K&00&01il&09l&16s!&00\r\n", GET_NAME(ch), GET_NAME(victim), GET_NAME(ch), GET_HEIGHT(ch) + 1);
       send_to_all(buf1);
       REMOVE_BIT(PLR_FLAGS(ch), PLR_KILLER);
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
       REMOVE_BIT(PLR_FLAGS(victim), PLR_KILLER);
      }
      GET_HEIGHT(ch) = GET_HEIGHT(ch) + 1;
      GET_WEIGHT(victim) = GET_WEIGHT(victim) + 1;
      if (MOB_FLAGGED(ch, MOB_MEMORY))
	forget(ch, victim);
    }
    /* Cant determine GET_GOLD on corpse, so do now and store */
     if (IS_NPC(victim)) {
       local_gold = GET_GOLD(victim);
       sprintf(local_buf,"%ld", (long)local_gold);
    }

     die(victim, ch);
     /* If Autoloot enabled, get all corpse */
     if (IS_NPC(victim) && !IS_NPC(ch) && PRF_FLAGGED(ch, PRF_AUTOLOOT) && !MOB_FLAGGED(victim, MOB_AWARE)) {
       do_get(ch,"all corpse",0,0);
     }
     /* If Autoloot AND AutoSplit AND we got money, split with group */
     if (IS_AFFECTED(ch, AFF_GROUP) && (local_gold > 0) &&
         PRF_FLAGGED(ch, PRF_AUTOSPLIT) && PRF_FLAGGED(ch,PRF_AUTOLOOT)) {
       do_split(ch,local_buf,0,0);
     }
    return -1;
  }
  return dam;
}



void hit(struct char_data * ch, struct char_data * victim, int type)
{
  struct obj_data *wielded2 = GET_EQ(ch, WEAR_DUAL);
  struct obj_data *wielded3 = GET_EQ(ch, WEAR_WRIST_R);
  struct obj_data *wielded = GET_EQ(ch, WEAR_WIELD);
  int w_type, victim_ac, calc_thaco, dam, diceroll;


  /* check if the character has a fight trigger */
  fight_mtrigger(ch);

if(!ch || !victim)
    return;
  /* This appears to fix my crash problem, Thanks to Chuck Reed */
  /*  if you notice anything */
  /* please email me about it so I can fix it or if you know of a fix */
  /* caminturn@earthlink.net */

  /* Do some sanity checking, in case someone flees, etc. */
  if (ch->in_room != victim->in_room) {
    if (FIGHTING(ch) && FIGHTING(ch) == victim)
      stop_fighting(ch);
    return;
  }

  /* Find the weapon type (for display purposes only) */
  if (wielded && GET_OBJ_TYPE(wielded) == ITEM_WEAPON && wielded2 && GET_OBJ_TYPE(wielded2) == ITEM_WEAPON)
    w_type = GET_OBJ_VAL(wielded, 3) + TYPE_HIT;
  else if (wielded && GET_OBJ_TYPE(wielded) == ITEM_WEAPON)
    w_type = GET_OBJ_VAL(wielded, 3) + TYPE_HIT;
  else if (wielded2 && GET_OBJ_TYPE(wielded2) == ITEM_WEAPON)
    w_type = GET_OBJ_VAL(wielded2, 3) + TYPE_HIT;
  else {
    if (IS_NPC(ch) && (ch->mob_specials.attack_type != 0))
      w_type = ch->mob_specials.attack_type + TYPE_HIT;
    else
      w_type = TYPE_HIT;
  }
  /* Calculate the THAC0 of the attacker */
  if (!IS_NPC(ch))
    calc_thaco = thaco((int) GET_CLASS(ch), (int) GET_LEVEL(ch));
  else		/* THAC0 for monsters is set in the HitRoll */
    calc_thaco = 20;
  calc_thaco -= str_app[STRENGTH_APPLY_INDEX(ch)].tohit;
  calc_thaco -= GET_HITROLL(ch);
  calc_thaco -= (int) ((GET_INT(ch) - 13) / 1.5);	
  calc_thaco -= (int) ((GET_WIS(ch) - 13) / 1.5);	/* So does wisdom */


  /* Calculate the raw armor including magic armor.  Lower AC is better. */
  victim_ac = GET_AC(victim) / 10;
  if (AWAKE(victim))
    victim_ac += dex_app[GET_DEX(victim)].defensive;
  victim_ac = MAX(-10, victim_ac);	/* -10 is lowest */

  /* roll the die and take your chances... */
  diceroll = number(1, 20);

  /* decide whether this is a hit or a miss */
  if ((((diceroll < 20) && AWAKE(victim)) &&
       ((diceroll == 1) || ((calc_thaco - diceroll) > victim_ac)))) {
    /* the attacker missed the victim */
    if (type == SKILL_BACKSTAB)
      damage(ch, victim, 0, SKILL_BACKSTAB);
    else
      damage(ch, victim, 0, w_type);
  } else {
    /* okay, we know the guy has been hit.  now calculate damage. */

    /* Start with the damage bonuses: the damroll and strength apply */
     dam = GET_DAMROLL(ch);
    if (IS_NPC(ch) && GET_DAMROLL(ch) >= 20000) {
     switch (number(1, 2)) {
      case 1:
       dam += number(1, 6000);
       break;
      case 2:
       dam -= number(1, 4000);
       break;
      default:
       dam += number(1, 2500);
       break;
     }
    }
    if (!IS_NPC(ch)) {
     dam += GET_DAMROLL(ch) * number(1, 100);
  
    }
    if (wielded && wielded2) {
       dam += dice(GET_OBJ_VAL(wielded, 1), GET_OBJ_VAL(wielded, 2));
       dam += dice(GET_OBJ_VAL(wielded2, 1), GET_OBJ_VAL(wielded2, 2));
    }
    else if (wielded2) {
       dam += dice(GET_OBJ_VAL(wielded2, 1), GET_OBJ_VAL(wielded2, 2));
    }
    else if (wielded) {
      /* Add weapon-based damage if a weapon is being wielded */
         dam += dice(GET_OBJ_VAL(wielded, 1), GET_OBJ_VAL(wielded, 2));
    } else {
      /* If no weapon, add bare hand damage instead */
      if (IS_NPC(ch)) {
	dam += dice(ch->mob_specials.damnodice, ch->mob_specials.damsizedice);
        if (IS_AFFECTED(ch, AFF_MFROZEN)) {
         dam = 1;
        }
        if (IS_KANASSAN(victim) && dam >= 2) {
           switch (number(1, 8)) {
            case 1:
            case 2:
            case 3:
             break;
            case 4:
             act("&14$N's &15forsee the attack and dodge most of it.&00", FALSE, ch, 0, victim, TO_CHAR);
             act("&14You &15forsees the attack and dodges most of it.&00", FALSE, ch, 0, victim, TO_VICT);
             act("&14$N &15forsees the attack and dodges most of it.&11!&00", FALSE, ch, 0, victim, TO_NOTVICT);
             dam = dam * .5;
             break;
            case 5:
            case 6:
            case 7:
            case 8:
             break;
           }
         }
        if (IS_ANGEL(victim) && dam >= 2) {
           switch (number(1, 40)) {
            case 1:
            case 2:
            case 3:
             break;
            case 4:
             act("&14$N &15knocks you down with a gust of wind before you can hit them.!&00", FALSE, ch, 0, victim, TO_CHAR);
             act("&14You &15knock $n&15 down with a gust of wind before they can hit you!&00", FALSE, ch, 0, victim, TO_VICT);
             act("&14$N &15knocks $n&15 down with a gust of wind before $e can hit them.!&00", FALSE, ch, 0, victim, TO_NOTVICT);
             dam = 0;
             GET_POS(ch) = POS_SITTING;
             WAIT_STATE(ch, PULSE_VIOLENCE * 1);
             break;
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
            case 31:
            case 32:
            case 33:
            case 34:
            case 35:
            case 36:
            case 37:
            case 38:
            case 39:
            case 40:
             break;
           }
         }

        if (IS_KAI(victim) && dam >= 2) {
           switch (number(1, 8)) {
            case 1:
            case 2:
            case 3:
             break;
            case 4:
             act("&14$N's &15mind throws up a &00&07psychic &12shield&11!&00", FALSE, ch, 0, victim, TO_CHAR);
             act("&14Your &15mind throws up a &00&07psychic &12shield&11!&00", FALSE, ch, 0, victim, TO_VICT);
             act("&14$N's &15mind throws up a &00&07psychic &12shield&11!&00", FALSE, ch, 0, victim, TO_NOTVICT);
             dam = 0;
             break;
            case 5:
            case 6:
            case 7:
            case 8:
             break;
           }
         }
       if (GET_EQ(victim, WEAR_FORE)) {
          dam = dam / 2;
         }
       if (IS_MAJIN(victim)) {
          dam = dam * .75;
         }
      } else if (!IS_NPC(ch)) {
        dam = number(GET_MAX_HIT(ch)/60 + GET_MANA(ch)/60, GET_MAX_HIT(ch)/50 + GET_MANA(ch)/50);       
        GET_MOVE(ch) += 1;
        if (IS_AFFECTED(ch, AFF_MFROZEN)) {
         dam = 1;
        }
        if (IS_KANASSAN(victim) && dam >= 2) {
           switch (number(1, 8)) {
            case 1:
            case 2:
            case 3:
             break;
            case 4:
             act("&14$N's &15forsee the attack and dodge most of it.&00", FALSE, ch, 0, victim, TO_CHAR);
             act("&14You &15forsees the attack and dodges most of it.&00", FALSE, ch, 0, victim, TO_VICT);
             act("&14$N &15forsees the attack and dodges most of it.&11!&00", FALSE, ch, 0, victim, TO_NOTVICT);
             dam = dam * .5;
             break;
            case 5:
            case 6:
            case 7:
            case 8:
             break;
           }
         }
         if (IS_ANGEL(victim) && dam >= 2) {
           switch (number(1, 40)) {
            case 1:
            case 2:
            case 3:
             break;
            case 4:
             act("&14$N &15knocks you down with a gust of wind before you can hit them.!&00", FALSE, ch, 0, victim, TO_CHAR);
             act("&14You &15knock $n&15 down with a gust of wind before they can hit you!&00", FALSE, ch, 0, victim, TO_VICT);
             act("&14$N &15knocks $n&15 down with a gust of wind before $e can hit them.!&00", FALSE, ch, 0, victim, TO_NOTVICT);
             dam = 0;
             GET_POS(ch) = POS_SITTING;
             WAIT_STATE(ch, PULSE_VIOLENCE * 1);
             break;
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
            case 31:
            case 32:
            case 33:
            case 34:
            case 35:
            case 36:
            case 37:
            case 38:
            case 39:
            case 40:
             break;
           }
         }
        if (IS_KAI(victim) && dam >= 2) {
           switch (number(1, 8)) {
            case 1:
            case 2:
            case 3:
             break;
            case 4:
             act("&14$N's &15mind throws up a &00&07psychic &12shield&11!&00", FALSE, ch, 0, victim, TO_CHAR);
             act("&14Your &15mind throws up a &00&07psychic &12shield&11!&00", FALSE, ch, 0, victim, TO_VICT);
             act("&14$N's &15mind throws up a &00&07psychic &12shield&11!&00", FALSE, ch, 0, victim, TO_NOTVICT);
             dam = 0;
             break;
            case 5:
            case 6:
            case 7:
            case 8:
             break;
           }
         } 
        if (IS_demon(ch)) {
           switch (number(1, 8)) {
            case 1:
            case 2:
            case 3:
             break;
            case 4:
             act("&14$n's &15fists &00&01burn &15with &09fire &15as they hit you&11!&00", FALSE, ch, 0, victim, TO_VICT);
             act("&14$n's &15fists &00&01burn &15with &09fire &15as they hit &10$N&11!&00", FALSE, ch, 0, victim, TO_NOTVICT);       
             act("&14Your &15fists &00&01burn &15with &09fire &15as you hit &10$N&11!&00", FALSE, ch, 0, victim, TO_CHAR);
             dam = dam * 2;
             break;
            case 5:
            case 6:
            case 7:
            case 8:
             break;
           }
         }
        if (IS_BIODROID(ch)) {
           dam = dam * 1.25;
           }
        if (GET_EQ(victim, WEAR_FORE)) {
           dam = dam / 2; 
           }
        if (IS_MAJIN(victim)) {
           dam = dam * .75;
           }
        if (AFF_FLAGGED(victim, AFF_PLATED)) {
           dam = 0;
        }
      }
     if(dam <=1)
    {
      dam = 0;
    }
    }

    /*
     * Include a damage multiplier if victim isn't ready to fight:
     *
     * Position sitting  1.33 x normal
     * Position resting  1.66 x normal
     * Position sleeping 2.00 x normal
     * Position stunned  2.33 x normal
     * Position incap    2.66 x normal
     * Position mortally 3.00 x normal
     *
     * Note, this is a hack because it depends on the particular
     * values of the POSITION_XXX constants.
     */

    /* at least 1 hp damage min per hit */
    dam = MAX(0, dam);

    if (type == SKILL_BACKSTAB) {
      dam *= backstab_mult(GET_LEVEL(ch));
      damage(ch, victim, dam, SKILL_BACKSTAB);
    } else
      damage(ch, victim, dam, w_type);
  }

  /* check if the victim has a hitprcnt trigger */
  hitprcnt_mtrigger(victim);
}


/* control the fights going on.  Called every 2 seconds from comm.c. */
void perform_violence(void)
{
  struct char_data *ch;
  struct char_data *victim;
  int attacks = 1, i;
  
  for (ch = combat_list; ch; ch = next_combat_list) {
    next_combat_list = ch->next_fighting;

    attacks = 1;   /* needs to be here or all char have same attacks */

    if (IS_NPC(ch) && (GET_MAX_HIT(ch) >= 1)) 
      attacks = 1;
    if (IS_NPC(ch) && (GET_MAX_HIT(ch) >= 10000000))
      attacks = 2;
    if (IS_NPC(ch) && (GET_MAX_HIT(ch) >= 25000000))
      attacks = 3;
    if (IS_NPC(ch) && (GET_MAX_HIT(ch) >= 40000000))
      attacks = 4;
    if (IS_NPC(ch) && (GET_MAX_HIT(ch) >= 70000000))
      attacks = 5;
    if (IS_NPC(ch) && (GET_MAX_HIT(ch) >= 95000000))
      attacks = 6;
    if (!IS_NPC(ch) && (GET_RAGE(ch) >= 0))
      GET_PC_ATTACKS(ch) = 1;  
    if (!IS_NPC(ch) && (GET_RAGE(ch) >= 20))
      GET_PC_ATTACKS(ch) = 2;
    if (!IS_NPC(ch) && (GET_RAGE(ch) >= 45))
      GET_PC_ATTACKS(ch) = 3;  
    if (!IS_NPC(ch) && (GET_RAGE(ch) < 0))
      GET_PC_ATTACKS(ch) = 4;
    if (!IS_NPC(ch) && (GET_RAGE(ch) >= 80))
      GET_PC_ATTACKS(ch) = 4;
    if (GET_EQ(ch, WEAR_FLOAT))
      GET_PC_ATTACKS(ch) += 1;
    if (IS_icer(ch) && PLR_FLAGGED(ch, PLR_TAIL))
      GET_PC_ATTACKS(ch) += 1;             
    if (!IS_NPC(ch) && !PLR_FLAGGED(ch, PLR_RARM))
      GET_PC_ATTACKS(ch) -= 1;
    if (!IS_NPC(ch) && !PLR_FLAGGED(ch, PLR_LARM))
      GET_PC_ATTACKS(ch) -= 1;
    if (!IS_NPC(ch) && !PLR_FLAGGED(ch, PLR_RLEG))
      GET_PC_ATTACKS(ch) -= 1;
    if (!IS_NPC(ch) && !PLR_FLAGGED(ch, PLR_LLEG))
      GET_PC_ATTACKS(ch) -= 1;
    if (!IS_NPC(ch) && (GET_PC_ATTACKS(ch) > 0))
      attacks = GET_PC_ATTACKS(ch);
    if (CLN_FLAGGED(ch, CLN_ADEF))
      attacks = 0;
   /* added to ensure that attacks are > 0 if not defaults to 1 */

    if (FIGHTING(ch) == NULL || ch->in_room != FIGHTING(ch)->in_room) {
      stop_fighting(ch);
      continue;
    }

    if (IS_NPC(ch)) {
      if (GET_MOB_WAIT(ch) > 0) {
	GET_MOB_WAIT(ch) -= PULSE_VIOLENCE;
	continue;
      }
      GET_MOB_WAIT(ch) = 0;
      if (GET_POS(ch) < POS_FIGHTING) {
       if (number(1, 3) <= 3) {
	GET_POS(ch) = POS_FIGHTING;
	act("$n scrambles to $s feet!", TRUE, ch, 0, 0, TO_ROOM);
       }
       else
        GET_POS(ch) = POS_SITTING;
      }
    }
    if (GET_POS(ch) < POS_FIGHTING) {
      send_to_char("You can't fight while sitting!!\r\n", ch);
      continue;
    }
	for (i = 0; i < attacks; i++) 
    hit(ch, FIGHTING(ch), TYPE_UNDEFINED);
    /* XXX: Need to see if they can handle "" instead of NULL. */
    if (MOB_FLAGGED(ch, MOB_SPEC) && mob_index[GET_MOB_RNUM(ch)].func != NULL)
      (mob_index[GET_MOB_RNUM(ch)].func) (ch, ch, 0, "");
  }
}
