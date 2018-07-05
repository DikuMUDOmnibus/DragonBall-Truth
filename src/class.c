/* 
************************************************************************
*   File: class.c                                       Part of CircleMUD *
*  Usage: Source file for class-specific code                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/*
 * This file attempts to concentrate most of the code which must be changed
 * in order for new classes to be added.  If you're adding a new class,
 * you should go through this entire file from beginning to end and add
 * the appropriate new special cases for your new class.
 */



#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "db.h"
#include "utils.h"
#include "spells.h"
#include "interpreter.h"

extern struct wis_app_type wis_app[];
extern struct con_app_type con_app[];
extern struct int_app_type int_app[];
extern int siteok_everyone;

/* local functions */
int parse_class(char arg);
long find_class_bitvector(char arg);
int thaco(int class_num, int level);
void roll_real_abils(struct char_data * ch);
void do_start(struct char_data * ch);
int backstab_mult(int level);
int invalid_class(struct char_data *ch, struct obj_data *obj);
long long level_exp(int chclass, long long level);
const char *title_male(int chclass, int level);
const char *title_female(int chclass, int level);

/* Names first */

const char *class_abbrevs[] = {
  "&14H&00&06u&14m&00&06a&14n&00      ",
  "&10N&00&02a&10m&00&02e&10k&00      ",
  "&14I&00&06c&15e&00&07r&00       ",
  "&11S&00&03a&11i&00&03y&11a&00&03n&00     ",
  "&13M&00&05a&13j&00&05i&13n&00      ",
  "&16A&15n&00&07dro&15i&16d&00    ",
  "&12K&14a&00&06i&00        ",
  "&14H&00&06a&14l&00&06f&12B&00&04r&12e&00&04e&12d  &00",
  "&10B&00&02i&10o&00&06-&16A&15n&00&07dro&15i&16d&00",
  "&00&07K&15on&16a&15ts&00&07u&00    ",
  "&13Changeling&00 ",
  "&09D&00&01e&09m&00&01o&09n&00      ",
  "&12T&14r&00&06u&00&04f&00&06f&14l&12e&00    ",
  "&09M&00&01u&00&05ta&00&01n&09t&00      ",
  "&12K&00&04a&00&06n&14as&00&06s&00&04a&12n&00      ",
  "&11A&00&03n&16g&15e&00&07l&00      ",
  "\n"
};

const char *class_abbrevs2[] = {
  "&14H&00&06u&14m&00&06a&14n&00        ",
  "&10N&00&02a&10m&00&02e&10k&00        ",
  "&14I&00&06c&15e&00&07r&00         ",
  "&11S&00&03a&11i&00&03y&11a&00&03n&00       ",
  "&13M&00&05a&13j&00&05i&13n&00        ",
  "&16A&15n&00&07dro&15i&16d&00      ",
  "&12K&14a&00&06i&00          ",
  "&14H&00&06a&14l&00&06f&12B&00&04r&12e&00&04e&12d    &00",
  "&10B&00&02i&10o&00&06-&16A&15n&00&07dro&15i&16d&00  ",
  "&00&07K&15on&16a&15ts&00&07u&00      ",
  "&13Changeling&00   ",
  "&09D&00&01e&09m&00&01o&09n&00        ",
  "&12T&14r&00&06u&00&04f&00&06f&14l&12e&00      ",
  "&09M&00&01u&00&05ta&00&01n&09t&00       ",
  "&12K&00&04a&00&06n&14as&00&06s&00&04a&12n&00     ",
  "&11A&00&03n&16g&15e&00&07l&00        ",
  "\n"
};


const char *pc_class_types[] = {
  "Human",
  "Namek",
  "Icer",
  "Saiyan",
  "Majin",
  "Android",
  "Kai",
  "Half-Breed",
  "Bio-Android",
  "Konatsu",
  "Changeling",
  "Demon",
  "Truffle",
  "Mutant",
  "Kanassan",
  "Angel",
  "\n"
};


/* The menu for choosing a class in interpreter.c: */
const char *class_menu =
"\r\nSaiyans and Majins are restricted, read help request to find how to request one.\r\n"
"\r\nType the race name to get info on that race. Type list to see this menu again.\r\n"
"\x1B[1;37mo\x1b[1;34m-----------------------------------------------------\x1B[1;37mo\x1B[0;0m\r\n"
"\x1b[1;34m|  \x1b[1;34m(\x1b[0;32m1\x1b[1;34m)\x1B[0;0m Human       \x1b[1;34m(\x1b[0;32m6\x1b[1;34m)\x1B[0;0m Android       \x1b[1;34m(\x1b[0;32mA\x1b[1;34m)\x1B[0;0m Mutant        \x1B[1;31m\r\n"
"\x1b[1;34m|  \x1b[1;34m(\x1b[0;32m2\x1b[1;34m)\x1B[0;0m Namek       \x1b[1;34m(\x1b[0;32m7\x1b[1;34m)\x1B[0;0m Kai           \x1b[1;34m(\x1b[0;32mB\x1b[1;34m)\x1B[0;0m Demon         \x1B[1;31m\r\n"
"\x1b[1;34m|  \x1b[1;34m(\x1b[0;32m*\x1b[1;34m)\x1B[0;0m Saiyan      \x1b[1;34m(\x1b[0;32m8\x1b[1;34m)\x1B[0;0m Half-Breed    \x1b[1;34m(\x1b[0;32mC\x1b[1;34m)\x1B[0;0m Kanassan             \x1B[1;31m\r\n"
"\x1b[1;34m|  \x1b[1;34m(\x1b[0;32m4\x1b[1;34m)\x1B[0;0m Icer        \x1b[1;34m(\x1b[0;32m9\x1b[1;34m)\x1B[0;0m Bio-Android   \x1b[1;34m(\x1b[0;32mE\x1b[1;34m)\x1B[0;0m Truffle           \x1B[1;31m\r\n"  
"\x1b[1;34m|  \x1b[1;34m(\x1b[0;32m*\x1b[1;34m)\x1B[0;0m Majin       \x1b[1;34m(\x1b[0;32m0\x1b[1;34m)\x1B[0;0m Konatsu       \x1b[1;34m(\x1b[0;32mD\x1b[1;34m)\x1B[0;0m Angel\r\n "
"\x1B[1;37mo\x1b[1;34m-----------------------------------------------------\x1B[1;37mo\x1B[0;0m\r\n";


/*
 * The code to interpret a class letter -- used in interpreter.c when a
 * new character is selecting a class and by 'set class' in act.wizard.c.
 */

int parse_class(char arg)
{
  arg = LOWER(arg);

  switch (arg) {
  case '1': return CLASS_Human;
  case '2': return CLASS_Namek;
  case '?': return CLASS_Human;
  case '4': return CLASS_icer;
  case '`': return CLASS_MAJIN;
  case '6': return CLASS_ANDROID;
  case '7': return CLASS_KAI;
  case '8': return CLASS_HALF_BREED;
  case '9': return CLASS_BIODROID;
  case '0': return CLASS_KONATSU;
  case 'A': return CLASS_MUTANT;
  case 'a': return CLASS_MUTANT;
  case 'B': return CLASS_demon;
  case 'b': return CLASS_demon;
  case 'C': return CLASS_KANASSAN;
  case 'c': return CLASS_KANASSAN;
  case 'D': return CLASS_ANGEL;
  case 'd': return CLASS_ANGEL;
  case 'E': return CLASS_TRUFFLE;
  case 'e': return CLASS_TRUFFLE;
  default:  return CLASS_UNDEFINED;
  }
}

/*
 * bitvectors (i.e., powers of two) for each class, mainly for use in
 * do_who and do_users.  Add new classes at the end so that all classes
 * use sequential powers of two (1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4,
 * 1 << 5, etc.
 */

long find_class_bitvector(char arg)
{
  arg = LOWER(arg);

  switch (arg) {
    case '1': return (1 << CLASS_Human);
    case '2': return (1 << CLASS_Namek);
    case '3': return (1 << CLASS_icer);
    case '4': return (1 << CLASS_saiyan);
    case '`': return (1 << CLASS_MAJIN);
    case '6': return (1 << CLASS_ANDROID);
    case '7': return (1 << CLASS_KAI);
    case '8': return (1 << CLASS_HALF_BREED);
    case '9': return (1 << CLASS_BIODROID);
    case '0': return (1 << CLASS_KONATSU);
    case 'A': return (1 << CLASS_MUTANT);
    case 'a': return (1 << CLASS_MUTANT);
    case 'B': return (1 << CLASS_demon);
    case 'b': return (1 << CLASS_demon);
    case 'C': return (1 << CLASS_KANASSAN);
    case 'c': return (1 << CLASS_KANASSAN);
    case 'D': return (1 << CLASS_ANGEL);
    case 'd': return (1 << CLASS_ANGEL);
    case 'E': return (1 << CLASS_TRUFFLE);
    case 'e': return (1 << CLASS_TRUFFLE);
            
    default:  return 0;
  }
}


/*
 * These are definitions which control the guildmasters for each class.
 *
 * The first field (top line) controls the highest percentage skill level
 * a character of the class is allowed to attain in any skill.  (After
 * this level, attempts to practice will say "You are already learned in
 * this area."
 * 
 * The second line controls the maximum percent gain in learnedness a
 * character is allowed per practice -- in other words, if the random
 * die throw comes out higher than this number, the gain will only be
 * this number instead.
 *
 * The third line controls the minimu percent gain in learnedness a
 * character is allowed per practice -- in other words, if the random
 * die throw comes out below this number, the gain will be set up to
 * this number.
 * 
 * The fourth line simply sets whether the character knows 'spells'
 * or 'skills'.  This does not affect anything except the message given
 * to the character when trying to practice (i.e. "You know of the
 * following spells" vs. "You know of the following skills"
 */

#define SPELL	0
#define SKILL	1

/* #define LEARNED_LEVEL	0  % known which is considered "learned" */
/* #define MAX_PER_PRAC		1  max percent gain in skill per practice */
/* #define MIN_PER_PRAC		2  min percent gain in skill per practice */
/* #define PRAC_TYPE		3  should it say 'spell' or 'skill'?	*/

int prac_params[4][NUM_CLASSES] = {
/* MAG	CLE  THE  WAR */
  {100,	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},		/* learned level */
  {100,	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},		/* max per prac */
  {25,	25,  25,  25,  25,  25,  25,  25,  25,  25,  25,  25,  25,  25,  25,},		/* min per pac */
  {SPELL, SPELL, SPELL, SPELL, SPELL, SPELL, SPELL, SPELL, SPELL, SPELL, SPELL, SPELL, SPELL, SPELL, SPELL}	/* prac name */
};


/*
 * ...And the appropriate rooms for each guildmaster/guildguard; controls
 * which types of people the various guildguards let through.  i.e., the
 * first line shows that from room 3017, only HumanS are allowed
 * to go south.
 */
int guild_info[][3] = {

/* Midgaard */

/* this must go last -- add new guards above! */
{-1, -1, -1}};


int thaco(int class, int level) {
	if (level < 0 || level > 1000014) {
		log("SYSERR: Unknown class in thac0 chart.");
		return 100;
	} else {
		return 1;
	}
}
	    

/*
 * Roll the 6 stats for a character... each stat is made of the sum of
 * the best 3 out of 4 rolls of a 6-sided die.  Each class then decides
 * which priority will be given for the best to worst stats.
 */
void roll_real_abils(struct char_data * ch)
{
  int i, j, k, temp;
  ubyte table[6];
  ubyte rolls[4];

  for (i = 0; i < 6; i++)
    table[i] = 0;

  for (i = 0; i < 6; i++) {

    for (j = 0; j < 4; j++)
      rolls[j] = number(1, 6);

    temp = rolls[0] + rolls[1] + rolls[2] + rolls[3] -
      MIN(rolls[0], MIN(rolls[1], MIN(rolls[2], rolls[3])));

    for (k = 0; k < 6; k++)
      if (table[k] < temp) {
	temp ^= table[k];
	table[k] ^= temp;
	temp ^= table[k];
      }
  }
 
  ch->real_abils.str_add = 0;
  ch->player.god = 0;
  SET_BIT(PLR_FLAGS(ch), PLR_NOMAJIN);
  GET_ALIGNMENT(ch) = 0;
  switch (GET_CLASS(ch)) {
  case CLASS_Human:
    GET_MAX_HIT(ch) += number(200, 400);
    GET_MAX_MANA(ch) += number(150, 200);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 15);
    ch->real_abils.con = number(8, 16);
    ch->real_abils.intel = number(12, 16);
    ch->real_abils.wis = number(8, 16);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_Namek:
    GET_MAX_HIT(ch) += number(200, 400);
    GET_MAX_MANA(ch) += number(150, 200);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_HOME(ch) += 1;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 15);
    ch->real_abils.con = number(8, 16);
    ch->real_abils.intel = number(10, 15);
    ch->real_abils.wis = number(8, 14);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_icer:
    GET_MAX_HIT(ch) += number(250, 500);
    GET_MAX_MANA(ch) += number(100, 150);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 14);
    ch->real_abils.con = number(8, 16);
    ch->real_abils.intel = number(8, 15);
    ch->real_abils.wis = number(8, 14);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_MAJIN:
    GET_MAX_HIT(ch) += number(200, 500);
    GET_MAX_MANA(ch) += number(120, 180);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 14);
    ch->real_abils.con = number(11, 16);
    ch->real_abils.intel = number(8, 13);
    ch->real_abils.wis = number(8, 13);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_ANDROID:
    GET_MAX_HIT(ch) += number(200, 450);
    GET_MAX_MANA(ch) += number(150, 170);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_HOME(ch) += 1;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 14);
    ch->real_abils.con = number(8, 16);
    ch->real_abils.intel = number(8, 15);
    ch->real_abils.wis = number(8, 14);
    ch->real_abils.cha = number(4, 11);
    break;
  case CLASS_KANASSAN:
    GET_MAX_HIT(ch) += number(200, 350);
    GET_MAX_MANA(ch) += number(200, 250);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_HOME(ch) += 1;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 15);
    ch->real_abils.con = number(8, 12);
    ch->real_abils.intel = number(8, 16);
    ch->real_abils.wis = number(8, 16);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_ANGEL:
    GET_MAX_HIT(ch) += number(200, 350);
    GET_MAX_MANA(ch) += number(200, 250);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(14, 18);
    ch->real_abils.dex = number(8, 15);
    ch->real_abils.con = number(8, 15);
    ch->real_abils.intel = number(8, 14);
    ch->real_abils.wis = number(8, 16);
    ch->real_abils.cha = number(8, 16);
    break;
  case CLASS_KAI:
    GET_MAX_HIT(ch) += number(200, 350);
    GET_MAX_MANA(ch) += number(200, 250);
    GET_MAX_MOVE(ch) = number(4, 4);
     GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_HOME(ch) += 1;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 15);
    ch->real_abils.con = number(8, 14);
    ch->real_abils.intel = number(8, 16);
    ch->real_abils.wis = number(8, 16);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_HALF_BREED:
    GET_MAX_HIT(ch) += number(200, 500);
    GET_MAX_MANA(ch) += number(100, 200);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 14);
    ch->real_abils.con = number(8, 16);
    ch->real_abils.intel = number(8, 16);
    ch->real_abils.wis = number(8, 16);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_BIODROID:
    GET_MAX_HIT(ch) += number(200, 400);
    GET_MAX_MANA(ch) += number(150, 200);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 15);
    ch->real_abils.con = number(8, 16);
    ch->real_abils.intel = number(8, 14);
    ch->real_abils.wis = number(8, 14);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_KONATSU:
    GET_MAX_HIT(ch) += number(200, 300);
    GET_MAX_MANA(ch) += number(200, 250);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(13, 15);
    ch->real_abils.con = number(8, 13);
    ch->real_abils.intel = number(8, 14);
    ch->real_abils.wis = number(8, 15);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_CHANGELING:
    ch->real_abils.str = table[0];
    ch->real_abils.dex = table[1];
    ch->real_abils.con = table[2];
    ch->real_abils.intel = table[3];
    ch->real_abils.wis = table[4];
    ch->real_abils.cha = number(10, 15);
    break;
  case CLASS_demon:
    GET_MAX_HIT(ch) += number(300, 400);
    GET_MAX_MANA(ch) += number(150, 200);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 14);
    ch->real_abils.con = number(8, 16);
    ch->real_abils.intel = number(8, 14);
    ch->real_abils.wis = number(8, 15);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_TRUFFLE:
    GET_MAX_HIT(ch) += number(250, 400);
    GET_MAX_MANA(ch) += number(125, 200);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 14);
    ch->real_abils.con = number(8, 14);
    ch->real_abils.intel = number(11, 16);
    ch->real_abils.wis = number(11, 16);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_MUTANT:
    GET_MAX_HIT(ch) += number(200, 400);
    GET_MAX_MANA(ch) += number(150, 200);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 16);
    ch->real_abils.con = number(8, 16);
    ch->real_abils.intel = number(8, 14);
    ch->real_abils.wis = number(8, 13);
    ch->real_abils.cha = number(8, 15);
    break;
  case CLASS_saiyan:
    GET_MAX_HIT(ch) += number(200, 400);
    GET_MAX_MANA(ch) += number(150, 200);
    GET_MAX_MOVE(ch) = number(4, 4);
    GET_MOVE(ch) = 0;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    ch->real_abils.str = number(15, 18);
    ch->real_abils.dex = number(8, 16);
    ch->real_abils.con = number(12, 16);
    ch->real_abils.intel = number(8, 13);
    ch->real_abils.wis = number(8, 14);
    ch->real_abils.cha = number(8, 15);
    break;
  }
  ch->real_abils.rage = 1;
  ch->real_abils.ubs = ch->real_abils.lbs = 50;
  ch->aff_abils = ch->real_abils;
}


/* Some initializations for characters, including initial skills */
void do_start(struct char_data * ch)
{
  GET_LEVEL(ch) = 1;
  GET_EXP(ch) = 1;

  set_title(ch, NULL);
  roll_real_abils(ch);
  ch->points.max_hit = 10;

  switch (GET_CLASS(ch)) {
  	
  case CLASS_Human:
    GET_MAX_HIT(ch) = 70;
    GET_MAX_MANA(ch) = 150;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(11, 15);
    GET_CON(ch) = 0 + number(10, 13);
    GET_WIS(ch) = 0 + number(10, 14);     
    GET_RAGE(ch) = 0 + number(1, 5);
    GET_DEX(ch) = 0 + number(1, 10);
    GET_STR(ch) = 0 + number(16, 18);
    break;
  case CLASS_Namek:
    GET_MAX_HIT(ch) = 80;
    GET_MAX_MANA(ch) = 100;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(10, 14);
    GET_CON(ch) = 0 + number(10, 13);
    GET_WIS(ch) = 0 + number(10, 15);
    GET_RAGE(ch) = 0 + number(1, 5);
    GET_DEX(ch) = 0 + number(1, 10); 
    GET_STR(ch) = 0 + number(16, 18);
    break;
  case CLASS_KANASSAN:
    GET_MAX_HIT(ch) = 100;
    GET_MAX_MANA(ch) = 75;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    break;
   case CLASS_ANGEL:
    GET_MAX_HIT(ch) = 100;
    GET_MAX_MANA(ch) = 75;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    break;
  case CLASS_icer:
    GET_MAX_HIT(ch) = 100;
    GET_MAX_MANA(ch) = 75;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(10, 14);
    GET_CON(ch) = 0 + number(10, 15);
    GET_WIS(ch) = 0 + number(10, 13);
    GET_RAGE(ch) = 0 + number(2, 5);
    GET_DEX(ch) = 0 + number(1, 5);
    GET_STR(ch) = 0 + number(16, 18);
    break;
  case CLASS_saiyan:
    GET_MAX_HIT(ch) = 150;
    GET_MAX_MANA(ch) = 100;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(10, 13);
    GET_CON(ch) = 0 + number(11, 15);
    GET_WIS(ch) = 0 + number(10, 14);
    GET_RAGE(ch) = 0 + number(3, 6);
    GET_DEX(ch) = 0 + number(1, 10);
    GET_STR(ch) = 0 + number(18, 20);
    break;
  case CLASS_MAJIN:
    GET_MAX_HIT(ch) = 125;
    GET_MAX_MANA(ch) = 125;
    GET_AC(ch) += 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(10, 14);
    GET_CON(ch) = 0 + number(10, 14);
    GET_WIS(ch) = 0 + number(10, 14);
    GET_RAGE(ch) = 0 + number(2, 5);
    GET_DEX(ch) = 0 + number(1, 10);
    GET_STR(ch) = 0 + number(16, 18);
    break;
  case CLASS_ANDROID:
    GET_MAX_HIT(ch) = 100;
    GET_MAX_MANA(ch) = 100;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(10, 13);
    GET_CON(ch) = 0 + number(10, 14);
    GET_WIS(ch) = 0 + number(10, 14);
    GET_RAGE(ch) = 0 + number(1, 5);
    GET_DEX(ch) = 0 + number(1, 10);
    GET_STR(ch) = 0 + number(16, 18);
    break;
  case CLASS_KAI:
    GET_MAX_HIT(ch) = 75;
    GET_MAX_MANA(ch) = 150;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(10, 15);
    GET_CON(ch) = 0 + number(10, 13);
    GET_WIS(ch) = 0 + number(10, 15);
    GET_RAGE(ch) = 0 + number(1, 5);
    GET_DEX(ch) = 0 + number(1, 10);
    GET_STR(ch) = 0 + number(16, 18);
    break;
  case CLASS_HALF_BREED:
    GET_MAX_HIT(ch) = 120;
    GET_MAX_MANA(ch) = 120;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(10, 14);
    GET_CON(ch) = 0 + number(11, 14);
    GET_WIS(ch) = 0 + number(10, 14);
    GET_RAGE(ch) = 0 + number(3, 6);
    GET_DEX(ch) = 0 + number(1, 9);
    GET_STR(ch) = 0 + number(16, 18);
    break;
  case CLASS_BIODROID:
    GET_MAX_HIT(ch) = 175;
    GET_MAX_MANA(ch) = 60;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(10, 14);
    GET_CON(ch) = 0 + number(11, 14);
    GET_WIS(ch) = 0 + number(10, 14);
    GET_RAGE(ch) = 0 + number(2, 5);
    GET_DEX(ch) = 0 + number(1, 8);
    GET_STR(ch) = 0 + number(16, 18);
    break;
  case CLASS_KONATSU:
    GET_MAX_HIT(ch) = 100;
    GET_MAX_MANA(ch) = 100;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(10, 15);
    GET_CON(ch) = 0 + number(10, 12);
    GET_WIS(ch) = 0 + number(10, 15);
    GET_RAGE(ch) = 0 + number(1, 4);
    GET_DEX(ch) = 0 + number(5, 15);
    GET_STR(ch) = 0 + number(16, 18);
    break;
  case CLASS_CHANGELING:
    break;
  case CLASS_demon:
    GET_MAX_HIT(ch) = 200;
    GET_MAX_MANA(ch) = 50;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(11, 13);
    GET_CON(ch) = 0 + number(10, 15);
    GET_WIS(ch) = 0 + number(10, 13);
    GET_RAGE(ch) = 0 + number(2, 5);
    GET_DEX(ch) = 0 + number(1, 7);
    GET_STR(ch) = 0 + number(16, 18);
    break;
  case CLASS_TRUFFLE:
    GET_MAX_HIT(ch) = 130;
    GET_MAX_MANA(ch) = 80;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(10, 15);
    GET_CON(ch) = 0 + number(10, 13);
    GET_WIS(ch) = 0 + number(10, 14);
    GET_RAGE(ch) = 0 + number(2, 4);
    GET_DEX(ch) = 0 + number(1, 10);
    GET_STR(ch) = 0 + number(16, 18);
    break;
  case CLASS_MUTANT:
    GET_MAX_HIT(ch) = 115;
    GET_MAX_MANA(ch) = 115;
    GET_AC(ch) -= 99;
    GET_WEIGHT(ch) = 0;
    GET_HEIGHT(ch) = 0;
    GET_INT(ch) = 0 + number(10, 14);
    GET_CON(ch) = 0 + number(10, 14);
    GET_WIS(ch) = 0 + number(10, 15);
    GET_RAGE(ch) = 0 + number(2, 4);
    GET_DEX(ch) = 0 + number(1, 10);
    GET_STR(ch) = 0 + number(16, 18);
    break;
  }
  
  advance_level(ch);
  /*if (GET_LEVEL(ch) <= 1 && ch->player.build == BUILD_FAT) {
     GET_STR(ch) += 5;
     GET_CON(ch) += 2;
    }
    else if (GET_LEVEL(ch) <= 1 && ch->player.build == BUILD_MUSC) {
     GET_STR(ch) += 2;
     GET_CON(ch) += 3;
    }*/
    /*else if (GET_LEVEL(ch) <= 1 && ch->player.build == BUILD_LEAN) {
     GET_INT(ch) += 2;
     GET_WIS(ch) += 2;
    }
    else if (GET_LEVEL(ch) <= 1 && ch->player.build == BUILD_SKIN) {
     GET_DEX(ch) += 20;
    }
    else if (GET_LEVEL(ch) <= 1 && ch->player.build == BUILD_CHUB) {
     GET_INT(ch) += 1;
     GET_CON(ch) += 1;
     GET_WIS(ch) += 1;
    }*/
  GET_HIT(ch) = GET_MAX_HIT(ch);
  GET_MANA(ch) = GET_MAX_MANA(ch);
  GET_MOVE(ch) = 0;
  GET_COND(ch, FULL) = 24;
  GET_COND(ch, DRUNK) = 0;
  GET_COND(ch, THIRST) = 24;
  GET_RAGE(ch) = 1;
  GET_UBS(ch) = 50;
  GET_LBS(ch) = 50;
  
 if (IS_Namek(ch)) {
  GET_COND(ch, FULL) = -1;
  }
 if (IS_ANDROID(ch)) {
  GET_COND(ch, FULL) = -1;
  GET_COND(ch, THIRST) = -1;
  }

  ch->player.time.played = 0;
  ch->player.time.logon = time(0);
  ch->player_specials->saved.pc_attacks = 1;
 
  SET_BIT(PRF_FLAGS(ch), PRF_DISPHP);
  SET_BIT(PRF_FLAGS(ch), PRF_DISPMANA);
  SET_BIT(PRF_FLAGS(ch), PRF_COLOR_1);
  SET_BIT(PRF_FLAGS(ch), PRF_COLOR_2);
  SET_BIT(PLR_FLAGS(ch), PLR_RARM);
  SET_BIT(PLR_FLAGS(ch), PLR_LARM);
  SET_BIT(PLR_FLAGS(ch), PLR_RLEG);
  SET_BIT(PLR_FLAGS(ch), PLR_LLEG);
  
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
}



/*
 * This function controls the change to maxmove, maxmana, and maxhp for
 * each class every time they gain a level.
 */
void advance_level(struct char_data * ch)
{
  int add_hp = 0, add_mana = 0, add_lp = 0, add_speed, i;

  if (GET_AGE(ch) >= 50) {
    add_lp += 1;
   }
  if (GET_AGE(ch) >= 65) {
    add_lp += 1;
    add_mana += 100;
    add_hp += 100;
   }
  if ((IS_Namek(ch) || IS_KAI(ch) || IS_MAJIN(ch) || IS_ANDROID(ch) || IS_KANASSAN(ch)) && GET_HOME(ch) <= 19999) {
   GET_HOME(ch) += 1;
   }  

  if (IS_HALF_BREED(ch) && !CLN_FLAGGED(ch, CLN_BLUST)) {
   switch (number(1, 15)) {

  case 1:
   add_hp += number(30, 30);
   send_to_char("&15Your inner &11s&00&03a&11i&00&03y&11a&00&03n&15 strength grows&00!\r\n", ch);
   break;
  case 2:
   add_hp += number(55, 55);
   send_to_char("&15Your inner &11s&00&03a&11i&00&03y&11a&00&03n&15 strength grows&00!\r\n", ch);
   break;
  case 3:
   add_hp += number(80, 80);
   send_to_char("&15Your inner &11s&00&03a&11i&00&03y&11a&00&03n&15 strength grows&00!\r\n", ch);
   break;
  case 4:
   GET_MAX_HIT(ch) = GET_MAX_HIT(ch);
   break;
  case 5:
   GET_MAX_HIT(ch) = GET_MAX_HIT(ch);
   break;
  case 6:
   GET_MAX_HIT(ch) = GET_MAX_HIT(ch);
   break;
  case 7:
   add_mana += number(40, 40);
   send_to_char("&15Your inner &11s&00&03a&11i&00&03y&11a&00&03n&15 strength grows&00!\r\n", ch);
   break;
  case 8:
   add_mana += number(60, 60);
   send_to_char("&15Your inner &11s&00&03a&11i&00&03y&11a&00&03n&15 strength grows&00!\r\n", ch);
   break;
  case 9:
   GET_MAX_HIT(ch) = GET_MAX_HIT(ch);
   break;
  case 10:
   GET_MAX_HIT(ch) = GET_MAX_HIT(ch);
   break;
  case 11:
  case 12:
  case 13:
  case 14:
  case 15:
   break;
 }
 }

  if (IS_saiyan(ch) || CLN_FLAGGED(ch, CLN_BLUST)) {
   switch (number(1, 15)) {
  
  case 1:
   add_hp += number(50, 50);
   send_to_char("&15Your inner &11s&00&03a&11i&00&03y&11a&00&03n&15 strength grows&00!\r\n", ch);
   break;
  case 2:
   add_hp += number(85, 85);
   send_to_char("&15Your inner &11s&00&03a&11i&00&03y&11a&00&03n&15 strength grows&00!\r\n", ch);
   break;
  case 3:
   add_hp += number(125, 125);
   send_to_char("&15Your inner &11s&00&03a&11i&00&03y&11a&00&03n&15 strength grows&00!\r\n", ch);
   break;
  case 4:
   GET_MAX_HIT(ch) = GET_MAX_HIT(ch);
   break;
  case 5:
   GET_MAX_HIT(ch) = GET_MAX_HIT(ch);
   break;
  case 6:
   GET_MAX_HIT(ch) = GET_MAX_HIT(ch);
   break;
  case 7:
   add_mana += number(70, 70);
   send_to_char("&15Your inner &11s&00&03a&11i&00&03y&11a&00&03n&15 strength grows&00!\r\n", ch);
   break;
  case 8:
   add_mana += number(100, 100);
   send_to_char("&15Your inner &11s&00&03a&11i&00&03y&11a&00&03n&15 strength grows&00!\r\n", ch);
   break;
  case 9:
   GET_MAX_HIT(ch) = GET_MAX_HIT(ch);
   break;
  case 10:
   GET_MAX_HIT(ch) = GET_MAX_HIT(ch);
   break;
  case 11:
  case 12:
  case 13:
  case 14:
  case 15:
   break;
 }
 }
  switch (GET_WIS(ch)) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
   add_lp += number(1, 1);
   break;   
  case 10:
   add_lp += number(1, 1);
   break;
  case 11:
   add_lp += number(2, 2);
   break;
  case 12:
   add_lp += number(2, 2);
   break;
  case 13:
   add_lp += number(2, 2);
   break;
  case 14:
   add_lp += number(2, 2);
   break;
  case 15:
   add_lp += number(3, 3);
   break;
  case 16:
   add_lp += number(3, 3);
   break;
  case 17:
   add_lp += number(3, 3);
   break;
  case 18:
   add_lp += number(3, 3);
   break;
  case 19:
   add_lp += number(4, 4);
   break;
  case 20:
   add_lp += number(4, 4);
   break;
  case 21:
   add_lp += number(4, 4);
   break;
  case 22:
   add_lp += number(4, 4);
   break;
  case 23:
   add_lp += number(5, 5);
   break;
  case 24:
   add_lp += number(5, 5);
   break;
  case 25:
   add_lp += number(5, 5);
   break;
  case 26:
   add_lp += number(5, 5);
   break;
  case 27:
   add_lp += number(6, 6);
   break;
  case 28:
   add_lp += number(6, 6);
   break;
  case 29:
   add_lp += number(6, 6);
   break;
  case 30:
   add_lp += number(6, 6);
   break;
  case 31:
   add_lp += number(7, 7);
   break;
  case 32:
   add_lp += number(7, 7);
   break;
  case 33:
   add_lp += number(7, 7);
   break;
  case 34:
   add_lp += number(7, 7);
   break;
  case 35:
   add_lp += number(8, 8);
   break;
  case 36:
   add_lp += number(8, 8);
   break;
  case 37:
   add_lp += number(8, 8);
   break;
  case 38:
   add_lp += number(8, 8);
   break;
  case 39:
   add_lp += number(9, 9);
   break;
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
  case 50:
  case 51:
  case 52:
  case 53:
  case 54:
  case 55:
   add_lp += number(10, 10);
   break;
  case 56:
  case 57:
   add_lp += number(11, 11);
   break;
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
   add_lp += number(12, 12);
   break;
  case 71:
  case 72:
  case 73:
  case 74:
  case 75:
   add_lp += number(13, 13);
   break;
  case 76:
  case 77:
  case 78:
  case 79:
  case 80:
   add_lp += number(14, 14);
   break;
  case 81:
  case 82:
  case 83:
  case 84:
  case 85:
   add_lp += number(15, 15);
   break;
  case 86:
  case 87:
  case 88:
  case 89:
  case 90:
   add_lp += number(16, 16);
   break;
  case 91:
  case 92:
  case 93:
  case 94:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
   add_lp += number(17, 17);
   break;
  case 100:
   add_lp += number(18, 18);
   break;
  case 101:
  case 102:
  case 103:
  case 104:
  case 105:
  case 106:
  case 107:
  case 108:
  case 109:
   add_lp += number(18, 18);
   break;
  case 110:
   add_lp += number(20, 20);
   break;
  case 111:
  case 112:
  case 113:
  case 114:
  case 115:
  case 116:
  case 117:
  case 118:
  case 119:
   add_lp += number(20, 20);
   break;
  case 120:
   add_lp += number(21, 21);
   break;
  case 121:
  case 122:
  case 123:
  case 124:
   add_lp += number(21, 21);
   break;
  case 125:
   add_lp += number(22, 22);
   break;
  }

  switch (GET_INT(ch)) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
    add_mana += number(30, 30);
    break;
  case 10:
    add_mana += number(45, 45);
    break;
  
  case 11:
    add_mana += number(55, 55);
    break;
  
  case 12:
    add_mana += number(65, 65);
    break;
  
  case 13:
    add_mana += number(80, 80);
    break;
  
  case 14:
    add_mana += number(100, 100);
    break;
  
  case 15:
    add_mana += number(110, 110);
    break;
  
  case 16:
    add_mana += number(120, 120);
    break;
  
  case 17:
    add_mana += number(130, 130);
    break;
  
  case 18:
    add_mana += number(140, 140);
    break;
  
  case 19:
    add_mana += number(150, 150);
    break;
  
  case 20:
    add_mana += number(160, 160);
    break;
  
  case 21:
    add_mana += number(170, 170);
    break;
  
  case 22:
    add_mana += number(180, 180);
    break;
  
  case 23:
    add_mana += number(190, 190);
    break;
  
  case 24:
    add_mana += number(200, 200);
    break;
  
  case 25:
    add_mana += number(210, 210);
    break;
  
  case 26:
    add_mana += number(220, 220);
    break;
  
  case 27:
    add_mana += number(230, 230);
    break;
  
  case 28:
    add_mana += number(240, 240);
    break;
  
  case 29:
    add_mana += number(250, 250);
    break;
  
  case 30:
    add_mana += number(260, 260);
    break;
  case 31:
    add_mana += number(270, 270);
    break;
  case 32:
    add_mana += number(280, 280);
    break;
  case 33:
    add_mana += number(290, 290);
    break;
  case 34:
    add_mana += number(300, 300);
    break;
  case 35:
    add_mana += number(310, 310);
    break;
  case 36:
    add_mana += number(320, 320);
    break;
  case 37:
    add_mana += number(330, 330);
    break;
  case 38:
    add_mana += number(340, 340);
    break;
  case 39:
    add_mana += number(350, 350);
    break;
  case 40:
    add_mana += number(360, 360);
    break;
  case 41:
    add_mana += number(370, 370);
    break;
  case 42:
    add_mana += number(380, 380);
    break;
  case 43:
    add_mana += number(390, 390);
    break;
  case 44:
    add_mana += number(400, 400);
    break;
  case 45:
    add_mana += number(410, 410);
    break;
  case 46:
    add_mana += number(420, 420);
    break;
  case 47:
    add_mana += number(430, 430);
    break;
  case 48:
    add_mana += number(440, 440);
    break;
  case 49:
    add_mana += number(450, 450);
    break;
  case 50:
    add_mana += number(460, 460);
    break;
  case 51:
    add_mana += number(470, 470);
    break;
  case 52:
    add_mana += number(480, 480);
    break;
  case 53:
    add_mana += number(490, 490);
    break;
  case 54:
    add_mana += number(500, 500);
    break;
  case 55:
    add_mana += number(510, 510);
    break;
  case 56:
    add_mana += number(520, 520);
    break;
  case 57:
    add_mana += number(530, 530);
    break;
  case 58:
    add_mana += number(540, 540);
    break;
  case 59:
    add_mana += number(550, 550);
    break;
  case 60:
    add_mana += number(560, 560);
    break;
  case 61:
    add_mana += number(570, 570);
    break;
  case 62:
    add_mana += number(580, 580);
    break;
  case 63:
    add_mana += number(590, 590);
    break;
  case 64:
    add_mana += number(600, 600);
    break;
  case 65:
    add_mana += number(610, 610);
    break;
  case 66:
    add_mana += number(620, 620);
    break;
  case 67:
    add_mana += number(630, 630);
    break;
  case 68:
    add_mana += number(640, 640);
    break;
  case 69:
    add_mana += number(650, 650);
    break;
  case 70:
    add_mana += number(660, 660);
    break;
  case 71:
    add_mana += number(670, 670);
    break;
  case 72:
    add_mana += number(680, 680);
    break;
  case 73:
    add_mana += number(690, 690);
    break;
  case 74:
    add_mana += number(700, 700);
    break;
  case 75:
   add_mana += number(710, 710);
    break;
  case 76:
   add_mana += number(720, 720);
   break;
  case 77:
   add_mana += number(730, 730);
   break;
  case 78:
   add_mana += number(740, 740);
   break;
  case 79:
   add_mana += number(750, 750);
   break;
  case 80:
   add_mana += number(760, 760);
   break;
  case 81:
   add_mana += number(770, 770);
   break;
  case 82:
   add_mana += number(780, 780);
   break;
  case 83:
   add_mana += number(790, 790);
   break;
  case 84:
   add_mana += number(800, 800);
   break;
  case 85:
   add_mana += number(810, 810);
   break;
  case 86:
   add_mana += number(820, 820);
   break;
  case 87:
   add_mana += number(830, 830);
   break;
  case 88:
   add_mana += number(840, 840);
   break;
  case 89:
   add_mana += number(850, 850);
   break;
  case 90:
   add_mana += number(860, 860);
   break;
  case 91:
   add_mana += number(870, 870);
   break;
  case 92:
   add_mana += number(880, 880);
   break;
  case 93:
   add_mana += number(890, 890);
   break;
  case 94:
   add_mana += number(900, 900);
   break;
  case 95:
   add_mana += number(910, 910);
   break;
  case 96:
   add_mana += number(920, 920);
   break;
  case 97:
   add_mana += number(930, 930);
   break;
  case 98:
   add_mana += number(940, 940);
   break;
  case 99:
   add_mana += number(950, 950);
   break;
  case 100:
   add_mana += number(1000, 1000);
   break;
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
  case 121:
  case 122:
  case 123:
  case 124:
   add_mana += number(1000, 1000);
   break;
  case 125:
   add_mana += number(1200, 1200);
   break;
 }
 switch (GET_CON(ch)) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
    add_hp += number(30, 30);
    break;
  case 10:
    add_hp += number(55, 55);
    break;
  
  case 11:
    add_hp += number(65, 65);
    break;
  
  case 12:
    add_hp += number(80, 80);
    break;
  
  case 13:
    add_hp += number(100, 100);
    break;
  
  case 14:
    add_hp += number(110, 110);
    break;
  
  case 15:
    add_hp += number(120, 120);
    break;
  
  case 16:
    add_hp += number(130, 130);
    break;
  
  case 17:
    add_hp += number(140, 140);
    break;
  
  case 18:
    add_hp += number(150, 150);
    break;
  
  case 19:
    add_hp += number(160, 160);
    break;
  
  case 20:
    add_hp += number(170, 170);
    break;
  
  case 21:
    add_hp += number(180, 180);
    break;
  
  case 22:
    add_hp += number(190, 190);
    break;
  
  case 23:
    add_hp += number(200, 200);
    break;
  
  case 24:
    add_hp += number(210, 210);
    break;
  
  case 25:
    add_hp += number(220, 220);
    break;
  
  case 26:
    add_hp += number(230, 230);
    break;
  
  case 27:
    add_hp += number(240, 240);
    break;
  
  case 28:
    add_hp += number(250, 250);
    break;
  
  case 29:
    add_hp += number(260, 260);
    break;
  
  case 30:
    add_hp += number(270, 270);
    break;
  case 31:
    add_hp += number(280, 280);
    break;
  case 32:
    add_hp += number(290, 290);
    break;
  case 33:
    add_hp += number(300, 300);
    break;
  case 34:
    add_hp += number(310, 310);
    break;
  case 35:
    add_hp += number(320, 320);
    break;
  case 36:
    add_hp += number(330, 330);
    break;
  case 37:
    add_hp += number(340, 340);
    break;
  case 38:
    add_hp += number(350, 350);
    break;
  case 39:
    add_hp += number(360, 360);
    break;
  case 40:
    add_hp += number(370, 370);
    break;
  case 41:
    add_hp += number(380, 380);
    break;
  case 42:
    add_hp += number(390, 390);
    break;
  case 43:
    add_hp += number(400, 400);
    break;
  case 44:
    add_hp += number(410, 410);
    break;
  case 45:
    add_hp += number(420, 420);
    break;
  case 46:
    add_hp += number(430, 430);
    break;
  case 47:
    add_hp += number(440, 440);
    break;
  case 48:
    add_hp += number(450, 450);
    break;
  case 49:
    add_hp += number(460, 460);
    break;
  case 50:
    add_hp += number(470, 470);
    break;
  case 51:
    add_hp += number(480, 480);
    break;
  case 52:
    add_hp += number(490, 490);
    break;
  case 53:
    add_hp += number(500, 500);
    break;
  case 54:
    add_hp += number(510, 510);
    break;
  case 55:
    add_hp += number(520, 520);
    break;
  case 56:
    add_hp += number(530, 530);
    break;
  case 57:
    add_hp += number(540, 540);
    break;
  case 58:
    add_hp += number(550, 550);
    break;
  case 59:
    add_hp += number(560, 560);
    break;
  case 60:
    add_hp += number(570, 570);
    break;
  case 61:
    add_hp += number(580, 580);
    break;
  case 62:
    add_hp += number(590, 590);
    break;
  case 63:
    add_hp += number(600, 600);
    break;
  case 64:
    add_hp += number(610, 610);
    break;
  case 65:
    add_hp += number(620, 620);
    break;
  case 66:
    add_hp += number(630, 630);
    break;
  case 67:
    add_hp += number(640, 640);
    break;
  case 68:
    add_hp += number(650, 650);
    break;
  case 69:
    add_hp += number(660, 660);
    break;
  case 70:
    add_hp += number(670, 670);
    break;
  case 71:
   add_hp += number(680, 680);
    break;
  case 72:
   add_hp += number(690, 690);
    break;
  case 73:
   add_hp += number(700, 700);
    break;
  case 74:
   add_hp += number(710, 710);
    break;
  case 75:
   add_hp += number(720, 720);
    break;
  case 76:
   add_hp += number(730, 730);
    break;
  case 77:
   add_hp += number(740, 740);
    break;
  case 78:
   add_hp += number(750, 750);
    break;
  case 79:
   add_hp += number(760, 760);
    break;
  case 80:
   add_hp += number(770, 770);
    break;
  case 81:
   add_hp += number(780, 780);
    break;
  case 82:
   add_hp += number(790, 790);
    break;
  case 83:
   add_hp += number(800, 800);
    break;
  case 84:
   add_hp += number(810, 810);
    break;
  case 85:
   add_hp += number(820, 820);
    break;
  case 86:
   add_hp += number(830, 830);
    break;
  case 87:
   add_hp += number(840, 840);
    break;
  case 88:
   add_hp += number(850, 850);
    break;
  case 89:
   add_hp += number(860, 860);
    break;
  case 90:
   add_hp += number(870, 870);
    break;
  case 91:
   add_hp += number(880, 880);
    break;
  case 92:
   add_hp += number(890, 890);
    break;
  case 93:
   add_hp += number(900, 900);
    break;
  case 94:
   add_hp += number(910, 910);
    break;
  case 95:
   add_hp += number(920, 920);
    break;
  case 96:
   add_hp += number(930, 930);
    break;
  case 97:
   add_hp += number(940, 940);
    break;
  case 98:
   add_hp += number(950, 950);
    break;
  case 99:
   add_hp += number(960, 960);
    break;
  case 100:
   add_hp += number(1000, 1000);
    break;
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
  case 121:
  case 122:
  case 123:
  case 124:
   add_hp += number(1000, 1000);
   break;
  case 125:
   add_hp += number(1200, 1200);
   break;
  }
  sprintf(buf, "&16[&10GAIN&16] &09PL&11: &15%d &14Ki&11: &15%d &12LP&11: &15%d\r\n", add_hp, add_mana, add_lp);
        send_to_char(buf, ch);
  GET_PRACTICES(ch) = GET_PRACTICES(ch) + add_lp;
  ch->points.max_hit += MAX(1, add_hp);
  ch->points.max_mana += MAX(1, add_mana);
  if (GET_LEVEL(ch) >= LVL_IMMORT) {
    for (i = 0; i < 3; i++)
      GET_COND(ch, i) = (char) -1;
    SET_BIT(PRF_FLAGS(ch), PRF_HOLYLIGHT);
  }

  save_char(ch, NOWHERE);
}


/*
 * This simply calculates the backstab multiplier based on a character's
 * level.  This used to be an array, but was changed to be a function so
 * that it would be easier to add more levels to your MUD.  This doesn't
 * really create a big performance hit because it's not used very often.
 */
int backstab_mult(int level)
{
  if (level <= 0)
    return 1;	  /* level 0 */
  else if (level <= 7)
    return 2;	  /* level 1 - 7 */
  else if (level <= 13)
    return 3;	  /* level 8 - 13 */
  else if (level <= 20)
    return 4;	  /* level 14 - 20 */
  else if (level <= 28)
    return 5;	  /* level 21 - 28 */
  else if (level < LVL_IMMORT)
    return 6;	  /* all remaining mortal levels */
  else
    return 20;	  /* immortals */
}


/*
 * invalid_class is used by handler.c to determine if a piece of equipment is
 * usable by a particular class, based on the ITEM_ANTI_{class} bitvectors.
 */

int invalid_class(struct char_data *ch, struct obj_data *obj) {
  if ((IS_OBJ_STAT(obj, ITEM_ANTI_Human) && IS_Human(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_Namek) && IS_Namek(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_saiyan) && IS_saiyan(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_MAJIN) && IS_MAJIN(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_ANDROID) && IS_ANDROID(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_KAI) && IS_KAI(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_HALF_BREED) && IS_HALF_BREED(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_TRUFFLE) && IS_TRUFFLE(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_demon) && IS_demon(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_KONATSU) && IS_KONATSU(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_BIODROID) && IS_BIODROID(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_MUTANT) && IS_MUTANT(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_HALF_BREED) && IS_HALF_BREED(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_HALF_BREED) && IS_HALF_BREED(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_icer) && IS_icer(ch)))
	return 1;
  else
	return 0;
}




/*
 * SPELLS AND SKILLS.  This area defines which spells are assigned to
 * which classes, and the minimum level the character must be to use
 * the spell or skill.
 */
void init_spell_levels(void)
{
  /* Human */
  spell_level(SPELL_FISHING, CLASS_Human, 1);
  spell_level(SPELL_HASSHUKEN, CLASS_Human, 1);
  spell_level(SPELL_GROUP_BOOST, CLASS_Human, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_Human, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_Human, 1);
  spell_level(SPELL_SOLAR_FLARE, CLASS_Human, 1);
  spell_level(SPELL_PORTAL, CLASS_Human, 1);
  spell_level(SPELL_BARRIER, CLASS_Human, 1);
  spell_level(SPELL_YOIKOMINMINKEN, CLASS_Human, 1);
  spell_level(SPELL_MULTIFORM, CLASS_Human, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_Human, 1);    
  spell_level(SPELL_CREATE_FOOD, CLASS_Human, 1);
  spell_level(SKILL_KISHOT, CLASS_Human, 1);
  spell_level(SKILL_SWEEP, CLASS_Human, 1);
  spell_level(SKILL_HIDE, CLASS_Human, 1);
  spell_level(SKILL_ZANELB, CLASS_Human, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_Human, 1);
  spell_level(SKILL_HEEL, CLASS_Human, 1);
  spell_level(SKILL_RESCUE, CLASS_Human, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_Human, 1);
  spell_level(SKILL_MUG, CLASS_Human, 1);
  spell_level(SKILL_SENSE, CLASS_Human, 1);
  spell_level(SKILL_TAILWHIP, CLASS_Human, 1);
  spell_level(SKILL_HEAVEN, CLASS_Human, 1);
  spell_level(SKILL_REPAIR, CLASS_Human, 1);
  spell_level(SKILL_KAIOKEN, CLASS_Human, 1);
  spell_level(SKILL_KNEE, CLASS_Human, 1);
  spell_level(SKILL_ELBOW, CLASS_Human, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_Human, 1);
  spell_level(SKILL_HYDRO, CLASS_Human, 1);
  spell_level(SKILL_INGEST, CLASS_Human, 1);
  spell_level(SKILL_ECLIPSE, CLASS_Human, 1);
  spell_level(SKILL_PHEONIX, CLASS_Human, 1);
  spell_level(SKILL_ABSORB, CLASS_Human, 1);
  spell_level(SKILL_DESTRUCT, CLASS_Human, 1);
  spell_level(SKILL_POWERSENSE, CLASS_Human, 1);
  spell_level(SKILL_STEALTH, CLASS_Human, 1);
  spell_level(SKILL_REGENERATE, CLASS_Human, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_Human, 1);
  spell_level(SKILL_BIGBANG, CLASS_Human, 1);
  spell_level(SKILL_FINALFLASH, CLASS_Human, 1);
  spell_level(SKILL_CHOUKAME, CLASS_Human, 1);
  spell_level(SKILL_DISK, CLASS_Human, 1);
  spell_level(SKILL_MASENKO, CLASS_Human, 1);
  spell_level(SKILL_RENZO, CLASS_Human, 1);
  spell_level(SKILL_MAKANKO, CLASS_Human, 1);
  spell_level(SKILL_KISHOT, CLASS_Human, 1);
  spell_level(SKILL_KIKOHO, CLASS_Human, 1);
  spell_level(SKILL_DBALL, CLASS_Human, 1);
  spell_level(SKILL_HFLASH, CLASS_Human, 1);
  spell_level(SKILL_KOUSEN, CLASS_Human, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_Human, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_Human, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_Human, 1);
  spell_level(SKILL_TWIN, CLASS_Human, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_Human, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_Human, 1);
  spell_level(SKILL_GIGABEAM, CLASS_Human, 1);
  spell_level(SKILL_MEDITATE, CLASS_Human, 1);
  spell_level(SKILL_BEAM, CLASS_Human, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_Human, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_Human, 1);
  spell_level(SKILL_BBK, CLASS_Human, 1);
  spell_level(SKILL_HAVOC, CLASS_Human, 1);
  spell_level(SKILL_GALIKGUN, CLASS_Human, 1);
  spell_level(SKILL_HONOO, CLASS_Human, 1);
  spell_level(SKILL_BAKU, CLASS_Human, 1);
  spell_level(SKILL_CRUSHER, CLASS_Human, 1);
  spell_level(SKILL_ERASER, CLASS_Human, 1);
  spell_level(SKILL_KYODAIKA, CLASS_Human, 1);
  spell_level(SKILL_SCATTER, CLASS_Human, 1);
  spell_level(SKILL_GENKI, CLASS_Human, 1);
  spell_level(SKILL_FORELOCK, CLASS_Human, 1);
  spell_level(SKILL_CHIKYUU, CLASS_Human, 1);
  spell_level(SKILL_GENOCIDE, CLASS_Human, 1);
  spell_level(SKILL_DEFLECT, CLASS_Human, 1);
  spell_level(SKILL_POTENTIAL, CLASS_Human, 1);
  spell_level(SKILL_MAJIN, CLASS_Human, 1);
  spell_level(SKILL_MINDREAD, CLASS_Human, 1);
  spell_level(SKILL_DUALWIELD, CLASS_Human, 1);

  /* Namek */
  spell_level(SPELL_FISHING, CLASS_Namek, 1);
  spell_level(SPELL_FOCUS, CLASS_Namek, 1);
  spell_level(SPELL_CALM, CLASS_Namek, 1);
  spell_level(SPELL_FORMWRIST, CLASS_Namek, 1);
  spell_level(SPELL_FORMPANTS, CLASS_Namek, 1);
  spell_level(SPELL_PORTAL, CLASS_Namek, 1);
  spell_level(SPELL_GROUP_HEAL, CLASS_Namek, 1);
  spell_level(SPELL_PROT_FROM_EVIL, CLASS_Namek, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_Namek, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_Namek, 1);
  spell_level(SPELL_HEAL, CLASS_Namek, 1);
  spell_level(SPELL_BARRIER, CLASS_Namek, 1);
  spell_level(SPELL_MULTIFORM, CLASS_Namek, 1);
  spell_level(SPELL_REPRODUCE, CLASS_Namek, 1);
  spell_level(SPELL_KYODAIKA, CLASS_Namek, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_Namek, 1); 
  spell_level(SPELL_CREATE_FOOD, CLASS_Namek, 1);
  spell_level(SKILL_KISHOT, CLASS_Namek, 1);
  spell_level(SKILL_SWEEP, CLASS_Namek, 1);
  spell_level(SKILL_HIDE, CLASS_Namek, 1);
  spell_level(SKILL_ZANELB, CLASS_Namek, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_Namek, 1);
  spell_level(SKILL_HEEL, CLASS_Namek, 1);
  spell_level(SKILL_RESCUE, CLASS_Namek, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_Namek, 1);
  spell_level(SKILL_MUG, CLASS_Namek, 1);
  spell_level(SKILL_SENSE, CLASS_Namek, 1);
  spell_level(SKILL_TAILWHIP, CLASS_Namek, 1);
  spell_level(SKILL_HEAVEN, CLASS_Namek, 1);
  spell_level(SKILL_REPAIR, CLASS_Namek, 1);
  spell_level(SKILL_KAIOKEN, CLASS_Namek, 1);
  spell_level(SKILL_KNEE, CLASS_Namek, 1);
  spell_level(SKILL_ELBOW, CLASS_Namek, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_Namek, 1);
  spell_level(SKILL_HYDRO, CLASS_Namek, 1);
  spell_level(SKILL_INGEST, CLASS_Namek, 1);
  spell_level(SKILL_ECLIPSE, CLASS_Namek, 1);
  spell_level(SKILL_PHEONIX, CLASS_Namek, 1);
  spell_level(SKILL_ABSORB, CLASS_Namek, 1);
  spell_level(SKILL_DESTRUCT, CLASS_Namek, 1);
  spell_level(SKILL_POWERSENSE, CLASS_Namek, 1);
  spell_level(SKILL_STEALTH, CLASS_Namek, 1);
  spell_level(SKILL_REGENERATE, CLASS_Namek, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_Namek, 1);
  spell_level(SKILL_BIGBANG, CLASS_Namek, 1);
  spell_level(SKILL_FINALFLASH, CLASS_Namek, 1);
  spell_level(SKILL_CHOUKAME, CLASS_Namek, 1);
  spell_level(SKILL_DISK, CLASS_Namek, 1);
  spell_level(SKILL_MASENKO, CLASS_Namek, 1);
  spell_level(SKILL_RENZO, CLASS_Namek, 1);
  spell_level(SKILL_MAKANKO, CLASS_Namek, 1);
  spell_level(SKILL_KISHOT, CLASS_Namek, 1);
  spell_level(SKILL_KIKOHO, CLASS_Namek, 1);
  spell_level(SKILL_DBALL, CLASS_Namek, 1);
  spell_level(SKILL_HFLASH, CLASS_Namek, 1);
  spell_level(SKILL_KOUSEN, CLASS_Namek, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_Namek, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_Namek, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_Namek, 1);
  spell_level(SKILL_TWIN, CLASS_Namek, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_Namek, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_Namek, 1);
  spell_level(SKILL_GIGABEAM, CLASS_Namek, 1);
  spell_level(SKILL_MEDITATE, CLASS_Namek, 1);
  spell_level(SKILL_BEAM, CLASS_Namek, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_Namek, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_Namek, 1);
  spell_level(SKILL_BBK, CLASS_Namek, 1);
  spell_level(SKILL_HAVOC, CLASS_Namek, 1);
  spell_level(SKILL_GALIKGUN, CLASS_Namek, 1);
  spell_level(SKILL_HONOO, CLASS_Namek, 1);
  spell_level(SKILL_BAKU, CLASS_Namek, 1);
  spell_level(SKILL_CRUSHER, CLASS_Namek, 1);
  spell_level(SKILL_ERASER, CLASS_Namek, 1);
  spell_level(SKILL_KYODAIKA, CLASS_Namek, 1);
  spell_level(SKILL_SCATTER, CLASS_Namek, 1);
  spell_level(SKILL_GENKI, CLASS_Namek, 1);
  spell_level(SKILL_FORELOCK, CLASS_Namek, 1);
  spell_level(SKILL_CHIKYUU, CLASS_Namek, 1);
  spell_level(SKILL_GENOCIDE, CLASS_Namek, 1);
  spell_level(SKILL_DEFLECT, CLASS_Namek, 1);
  spell_level(SKILL_POTENTIAL, CLASS_Namek, 1);
  spell_level(SKILL_MAJIN, CLASS_Namek, 1);
  spell_level(SKILL_MINDREAD, CLASS_Namek, 1);
  spell_level(SKILL_DUALWIELD, CLASS_Namek, 1);

  /* Icer */
  spell_level(SPELL_FISHING, CLASS_icer, 1);
  spell_level(SPELL_FRENZY, CLASS_icer, 1);
  spell_level(SPELL_PLATED, CLASS_icer, 1);
  spell_level(SPELL_PORTAL, CLASS_icer, 1);
  spell_level(SPELL_BARRIER, CLASS_icer, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_icer, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_icer, 1);
  spell_level(SPELL_INFRAVISION, CLASS_icer, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_icer, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_icer, 1);
  spell_level(SKILL_KISHOT, CLASS_icer, 1);
  spell_level(SKILL_SWEEP, CLASS_icer, 1);
  spell_level(SKILL_HIDE, CLASS_icer, 1);
  spell_level(SKILL_ZANELB, CLASS_icer, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_icer, 1);
  spell_level(SKILL_HEEL, CLASS_icer, 1);
  spell_level(SKILL_RESCUE, CLASS_icer, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_icer, 1);
  spell_level(SKILL_MUG, CLASS_icer, 1);
  spell_level(SKILL_SENSE, CLASS_icer, 1);
  spell_level(SKILL_TAILWHIP, CLASS_icer, 1);
  spell_level(SKILL_HEAVEN, CLASS_icer, 1);
  spell_level(SKILL_REPAIR, CLASS_icer, 1);
  spell_level(SKILL_KAIOKEN, CLASS_icer, 1);
  spell_level(SKILL_KNEE, CLASS_icer, 1);
  spell_level(SKILL_ELBOW, CLASS_icer, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_icer, 1);
  spell_level(SKILL_HYDRO, CLASS_icer, 1);
  spell_level(SKILL_INGEST, CLASS_icer, 1);
  spell_level(SKILL_ECLIPSE, CLASS_icer, 1);
  spell_level(SKILL_PHEONIX, CLASS_icer, 1);
  spell_level(SKILL_ABSORB, CLASS_icer, 1);
  spell_level(SKILL_DESTRUCT, CLASS_icer, 1);
  spell_level(SKILL_POWERSENSE, CLASS_icer, 1);
  spell_level(SKILL_STEALTH, CLASS_icer, 1);
  spell_level(SKILL_REGENERATE, CLASS_icer, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_icer, 1);
  spell_level(SKILL_BIGBANG, CLASS_icer, 1);
  spell_level(SKILL_FINALFLASH, CLASS_icer, 1);
  spell_level(SKILL_CHOUKAME, CLASS_icer, 1);
  spell_level(SKILL_DISK, CLASS_icer, 1);
  spell_level(SKILL_MASENKO, CLASS_icer, 1);
  spell_level(SKILL_RENZO, CLASS_icer, 1);
  spell_level(SKILL_MAKANKO, CLASS_icer, 1);
  spell_level(SKILL_KISHOT, CLASS_icer, 1);
  spell_level(SKILL_KIKOHO, CLASS_icer, 1);
  spell_level(SKILL_DBALL, CLASS_icer, 1);
  spell_level(SKILL_HFLASH, CLASS_icer, 1);
  spell_level(SKILL_KOUSEN, CLASS_icer, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_icer, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_icer, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_icer, 1);
  spell_level(SKILL_TWIN, CLASS_icer, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_icer, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_icer, 1);
  spell_level(SKILL_GIGABEAM, CLASS_icer, 1);
  spell_level(SKILL_MEDITATE, CLASS_icer, 1);
  spell_level(SKILL_BEAM, CLASS_icer, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_icer, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_icer, 1);
  spell_level(SKILL_BBK, CLASS_icer, 1);
  spell_level(SKILL_HAVOC, CLASS_icer, 1);
  spell_level(SKILL_GALIKGUN, CLASS_icer, 1);
  spell_level(SKILL_HONOO, CLASS_icer, 1);
  spell_level(SKILL_BAKU, CLASS_icer, 1);
  spell_level(SKILL_CRUSHER, CLASS_icer, 1);
  spell_level(SKILL_ERASER, CLASS_icer, 1);
  spell_level(SKILL_KYODAIKA, CLASS_icer, 1);
  spell_level(SKILL_SCATTER, CLASS_icer, 1);
  spell_level(SKILL_GENKI, CLASS_icer, 1);
  spell_level(SKILL_FORELOCK, CLASS_icer, 1);
  spell_level(SKILL_CHIKYUU, CLASS_icer, 1);
  spell_level(SKILL_GENOCIDE, CLASS_icer, 1);
  spell_level(SKILL_DEFLECT, CLASS_icer, 1);
  spell_level(SKILL_POTENTIAL, CLASS_icer, 1);
  spell_level(SKILL_MAJIN, CLASS_icer, 1);
  spell_level(SKILL_MINDREAD, CLASS_icer, 1);
  spell_level(SKILL_DUALWIELD, CLASS_icer, 1);


  /* Saiyan */
  spell_level(SPELL_FISHING, CLASS_saiyan, 1);
  spell_level(SPELL_PORTAL, CLASS_saiyan, 1);
  spell_level(SPELL_BARRIER, CLASS_saiyan, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_saiyan, 1);
  spell_level(SPELL_SOLAR_FLARE, CLASS_saiyan, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_saiyan, 1);
  spell_level(SPELL_HASSHUKEN, CLASS_saiyan, 1);
  spell_level(SPELL_TSURUGI, CLASS_saiyan, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_saiyan, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_saiyan, 1);
  spell_level(SKILL_KISHOT, CLASS_saiyan, 1);
  spell_level(SKILL_SWEEP, CLASS_saiyan, 1);
  spell_level(SKILL_HIDE, CLASS_saiyan, 1);
  spell_level(SKILL_ZANELB, CLASS_saiyan, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_saiyan, 1);
  spell_level(SKILL_HEEL, CLASS_saiyan, 1);
  spell_level(SKILL_RESCUE, CLASS_saiyan, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_saiyan, 1);
  spell_level(SKILL_MUG, CLASS_saiyan, 1);
  spell_level(SKILL_SENSE, CLASS_saiyan, 1);
  spell_level(SKILL_TAILWHIP, CLASS_saiyan, 1);
  spell_level(SKILL_HEAVEN, CLASS_saiyan, 1);
  spell_level(SKILL_REPAIR, CLASS_saiyan, 1);
  spell_level(SKILL_KAIOKEN, CLASS_saiyan, 1);
  spell_level(SKILL_KNEE, CLASS_saiyan, 1);
  spell_level(SKILL_ELBOW, CLASS_saiyan, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_saiyan, 1);
  spell_level(SKILL_HYDRO, CLASS_saiyan, 1);
  spell_level(SKILL_INGEST, CLASS_saiyan, 1);
  spell_level(SKILL_ECLIPSE, CLASS_saiyan, 1);
  spell_level(SKILL_PHEONIX, CLASS_saiyan, 1);
  spell_level(SKILL_ABSORB, CLASS_saiyan, 1);
  spell_level(SKILL_DESTRUCT, CLASS_saiyan, 1);
  spell_level(SKILL_POWERSENSE, CLASS_saiyan, 1);
  spell_level(SKILL_STEALTH, CLASS_saiyan, 1);
  spell_level(SKILL_REGENERATE, CLASS_saiyan, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_saiyan, 1);
  spell_level(SKILL_BIGBANG, CLASS_saiyan, 1);
  spell_level(SKILL_FINALFLASH, CLASS_saiyan, 1);
  spell_level(SKILL_CHOUKAME, CLASS_saiyan, 1);
  spell_level(SKILL_DISK, CLASS_saiyan, 1);
  spell_level(SKILL_MASENKO, CLASS_saiyan, 1);
  spell_level(SKILL_RENZO, CLASS_saiyan, 1);
  spell_level(SKILL_MAKANKO, CLASS_saiyan, 1);
  spell_level(SKILL_KISHOT, CLASS_saiyan, 1);
  spell_level(SKILL_KIKOHO, CLASS_saiyan, 1);
  spell_level(SKILL_DBALL, CLASS_saiyan, 1);
  spell_level(SKILL_HFLASH, CLASS_saiyan, 1);
  spell_level(SKILL_KOUSEN, CLASS_saiyan, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_saiyan, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_saiyan, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_saiyan, 1);
  spell_level(SKILL_TWIN, CLASS_saiyan, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_saiyan, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_saiyan, 1);
  spell_level(SKILL_GIGABEAM, CLASS_saiyan, 1);
  spell_level(SKILL_MEDITATE, CLASS_saiyan, 1);
  spell_level(SKILL_BEAM, CLASS_saiyan, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_saiyan, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_saiyan, 1);
  spell_level(SKILL_BBK, CLASS_saiyan, 1);
  spell_level(SKILL_HAVOC, CLASS_saiyan, 1);
  spell_level(SKILL_GALIKGUN, CLASS_saiyan, 1);
  spell_level(SKILL_HONOO, CLASS_saiyan, 1);
  spell_level(SKILL_BAKU, CLASS_saiyan, 1);
  spell_level(SKILL_CRUSHER, CLASS_saiyan, 1);
  spell_level(SKILL_ERASER, CLASS_saiyan, 1);
  spell_level(SKILL_KYODAIKA, CLASS_saiyan, 1);
  spell_level(SKILL_SCATTER, CLASS_saiyan, 1);
  spell_level(SKILL_GENKI, CLASS_saiyan, 1);
  spell_level(SKILL_FORELOCK, CLASS_saiyan, 1);
  spell_level(SKILL_CHIKYUU, CLASS_saiyan, 1);
  spell_level(SKILL_GENOCIDE, CLASS_saiyan, 1);
  spell_level(SKILL_DEFLECT, CLASS_saiyan, 1);
  spell_level(SKILL_POTENTIAL, CLASS_saiyan, 1);
  spell_level(SKILL_MAJIN, CLASS_saiyan, 1);
  spell_level(SKILL_MINDREAD, CLASS_saiyan, 1);
  spell_level(SKILL_DUALWIELD, CLASS_saiyan, 1);

  /* Majin */
  spell_level(SPELL_FISHING, CLASS_MAJIN, 1);
  spell_level(SPELL_IREG, CLASS_MAJIN, 1);
  spell_level(SPELL_PORTAL, CLASS_MAJIN, 1);
  spell_level(SPELL_HEAL, CLASS_MAJIN, 1);
  spell_level(SPELL_CANDYBEAM, CLASS_MAJIN, 1);
  spell_level(SPELL_GROUP_HEAL, CLASS_MAJIN, 1);
  spell_level(SPELL_BARRIER, CLASS_MAJIN, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_MAJIN, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_MAJIN, 1);
  spell_level(SPELL_MAJIN, CLASS_MAJIN, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_MAJIN, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_MAJIN, 1);
  spell_level(SKILL_KISHOT, CLASS_MAJIN, 1);
  spell_level(SKILL_SWEEP, CLASS_MAJIN, 1);
  spell_level(SKILL_HIDE, CLASS_MAJIN, 1);
  spell_level(SKILL_ZANELB, CLASS_MAJIN, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_MAJIN, 1);
  spell_level(SKILL_HEEL, CLASS_MAJIN, 1);
  spell_level(SKILL_RESCUE, CLASS_MAJIN, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_MAJIN, 1);
  spell_level(SKILL_MUG, CLASS_MAJIN, 1);
  spell_level(SKILL_SENSE, CLASS_MAJIN, 1);
  spell_level(SKILL_TAILWHIP, CLASS_MAJIN, 1);
  spell_level(SKILL_HEAVEN, CLASS_MAJIN, 1);
  spell_level(SKILL_REPAIR, CLASS_MAJIN, 1);
  spell_level(SKILL_KAIOKEN, CLASS_MAJIN, 1);
  spell_level(SKILL_KNEE, CLASS_MAJIN, 1);
  spell_level(SKILL_ELBOW, CLASS_MAJIN, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_MAJIN, 1);
  spell_level(SKILL_HYDRO, CLASS_MAJIN, 1);
  spell_level(SKILL_INGEST, CLASS_MAJIN, 1);
  spell_level(SKILL_ECLIPSE, CLASS_MAJIN, 1);
  spell_level(SKILL_PHEONIX, CLASS_MAJIN, 1);
  spell_level(SKILL_ABSORB, CLASS_MAJIN, 1);
  spell_level(SKILL_DESTRUCT, CLASS_MAJIN, 1);
  spell_level(SKILL_POWERSENSE, CLASS_MAJIN, 1);
  spell_level(SKILL_STEALTH, CLASS_MAJIN, 1);
  spell_level(SKILL_REGENERATE, CLASS_MAJIN, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_MAJIN, 1);
  spell_level(SKILL_BIGBANG, CLASS_MAJIN, 1);
  spell_level(SKILL_FINALFLASH, CLASS_MAJIN, 1);
  spell_level(SKILL_CHOUKAME, CLASS_MAJIN, 1);
  spell_level(SKILL_DISK, CLASS_MAJIN, 1);
  spell_level(SKILL_MASENKO, CLASS_MAJIN, 1);
  spell_level(SKILL_RENZO, CLASS_MAJIN, 1);
  spell_level(SKILL_MAKANKO, CLASS_MAJIN, 1);
  spell_level(SKILL_KISHOT, CLASS_MAJIN, 1);
  spell_level(SKILL_KIKOHO, CLASS_MAJIN, 1);
  spell_level(SKILL_DBALL, CLASS_MAJIN, 1);
  spell_level(SKILL_HFLASH, CLASS_MAJIN, 1);
  spell_level(SKILL_KOUSEN, CLASS_MAJIN, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_MAJIN, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_MAJIN, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_MAJIN, 1);
  spell_level(SKILL_TWIN, CLASS_MAJIN, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_MAJIN, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_MAJIN, 1);
  spell_level(SKILL_GIGABEAM, CLASS_MAJIN, 1);
  spell_level(SKILL_MEDITATE, CLASS_MAJIN, 1);
  spell_level(SKILL_BEAM, CLASS_MAJIN, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_MAJIN, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_MAJIN, 1);
  spell_level(SKILL_BBK, CLASS_MAJIN, 1);
  spell_level(SKILL_HAVOC, CLASS_MAJIN, 1);
  spell_level(SKILL_GALIKGUN, CLASS_MAJIN, 1);
  spell_level(SKILL_HONOO, CLASS_MAJIN, 1);
  spell_level(SKILL_BAKU, CLASS_MAJIN, 1);
  spell_level(SKILL_CRUSHER, CLASS_MAJIN, 1);
  spell_level(SKILL_ERASER, CLASS_MAJIN, 1);
  spell_level(SKILL_KYODAIKA, CLASS_MAJIN, 1);
  spell_level(SKILL_SCATTER, CLASS_MAJIN, 1);
  spell_level(SKILL_GENKI, CLASS_MAJIN, 1);
  spell_level(SKILL_FORELOCK, CLASS_MAJIN, 1);
  spell_level(SKILL_CHIKYUU, CLASS_MAJIN, 1);
  spell_level(SKILL_GENOCIDE, CLASS_MAJIN, 1);
  spell_level(SKILL_DEFLECT, CLASS_MAJIN, 1);
  spell_level(SKILL_POTENTIAL, CLASS_MAJIN, 1);
  spell_level(SKILL_MAJIN, CLASS_MAJIN, 1);
  spell_level(SKILL_MINDREAD, CLASS_MAJIN, 1);
  spell_level(SKILL_DUALWIELD, CLASS_MAJIN, 1);

  /* Android */
  spell_level(SPELL_FISHING, CLASS_ANDROID, 1);
  spell_level(SPELL_PORTAL, CLASS_ANDROID, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_ANDROID, 1);
  spell_level(SPELL_INFRAVISION, CLASS_ANDROID, 1);
  spell_level(SPELL_BARRIER, CLASS_ANDROID, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_ANDROID, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_ANDROID, 1);
  spell_level(SPELL_BARRIER, CLASS_ANDROID, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_ANDROID, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_ANDROID, 1);
  spell_level(SPELL_INFRAVISION, CLASS_ANDROID, 1);
  spell_level(SKILL_KISHOT, CLASS_ANDROID, 1);
  spell_level(SKILL_SWEEP, CLASS_ANDROID, 1);
  spell_level(SKILL_HIDE, CLASS_ANDROID, 1);
  spell_level(SKILL_ZANELB, CLASS_ANDROID, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_ANDROID, 1);
  spell_level(SKILL_HEEL, CLASS_ANDROID, 1);
  spell_level(SKILL_RESCUE, CLASS_ANDROID, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_ANDROID, 1);
  spell_level(SKILL_MUG, CLASS_ANDROID, 1);
  spell_level(SKILL_SENSE, CLASS_ANDROID, 1);
  spell_level(SKILL_TAILWHIP, CLASS_ANDROID, 1);
  spell_level(SKILL_HEAVEN, CLASS_ANDROID, 1);
  spell_level(SKILL_REPAIR, CLASS_ANDROID, 1);
  spell_level(SKILL_KAIOKEN, CLASS_ANDROID, 1);
  spell_level(SKILL_KNEE, CLASS_ANDROID, 1);
  spell_level(SKILL_ELBOW, CLASS_ANDROID, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_ANDROID, 1);
  spell_level(SKILL_HYDRO, CLASS_ANDROID, 1);
  spell_level(SKILL_INGEST, CLASS_ANDROID, 1);
  spell_level(SKILL_ECLIPSE, CLASS_ANDROID, 1);
  spell_level(SKILL_PHEONIX, CLASS_ANDROID, 1);
  spell_level(SKILL_ABSORB, CLASS_ANDROID, 1);
  spell_level(SKILL_DESTRUCT, CLASS_ANDROID, 1);
  spell_level(SKILL_POWERSENSE, CLASS_ANDROID, 1);
  spell_level(SKILL_STEALTH, CLASS_ANDROID, 1);
  spell_level(SKILL_REGENERATE, CLASS_ANDROID, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_ANDROID, 1);
  spell_level(SKILL_BIGBANG, CLASS_ANDROID, 1);
  spell_level(SKILL_FINALFLASH, CLASS_ANDROID, 1);
  spell_level(SKILL_CHOUKAME, CLASS_ANDROID, 1);
  spell_level(SKILL_DISK, CLASS_ANDROID, 1);
  spell_level(SKILL_MASENKO, CLASS_ANDROID, 1);
  spell_level(SKILL_RENZO, CLASS_ANDROID, 1);
  spell_level(SKILL_MAKANKO, CLASS_ANDROID, 1);
  spell_level(SKILL_KISHOT, CLASS_ANDROID, 1);
  spell_level(SKILL_KIKOHO, CLASS_ANDROID, 1);
  spell_level(SKILL_DBALL, CLASS_ANDROID, 1);
  spell_level(SKILL_HFLASH, CLASS_ANDROID, 1);
  spell_level(SKILL_KOUSEN, CLASS_ANDROID, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_ANDROID, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_ANDROID, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_ANDROID, 1);
  spell_level(SKILL_TWIN, CLASS_ANDROID, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_ANDROID, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_ANDROID, 1);
  spell_level(SKILL_GIGABEAM, CLASS_ANDROID, 1);
  spell_level(SKILL_MEDITATE, CLASS_ANDROID, 1);
  spell_level(SKILL_BEAM, CLASS_ANDROID, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_ANDROID, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_ANDROID, 1);
  spell_level(SKILL_BBK, CLASS_ANDROID, 1);
  spell_level(SKILL_HAVOC, CLASS_ANDROID, 1);
  spell_level(SKILL_GALIKGUN, CLASS_ANDROID, 1);
  spell_level(SKILL_HONOO, CLASS_ANDROID, 1);
  spell_level(SKILL_BAKU, CLASS_ANDROID, 1);
  spell_level(SKILL_CRUSHER, CLASS_ANDROID, 1);
  spell_level(SKILL_ERASER, CLASS_ANDROID, 1);
  spell_level(SKILL_KYODAIKA, CLASS_ANDROID, 1);
  spell_level(SKILL_SCATTER, CLASS_ANDROID, 1);
  spell_level(SKILL_GENKI, CLASS_ANDROID, 1);
  spell_level(SKILL_FORELOCK, CLASS_ANDROID, 1);
  spell_level(SKILL_CHIKYUU, CLASS_ANDROID, 1);
  spell_level(SKILL_GENOCIDE, CLASS_ANDROID, 1);
  spell_level(SKILL_DEFLECT, CLASS_ANDROID, 1);
  spell_level(SKILL_POTENTIAL, CLASS_ANDROID, 1);
  spell_level(SKILL_MAJIN, CLASS_ANDROID, 1);
  spell_level(SKILL_MINDREAD, CLASS_ANDROID, 1);
  spell_level(SKILL_DUALWIELD, CLASS_ANDROID, 1);

  /* KAI */
  spell_level(SPELL_FISHING, CLASS_KAI, 1);
  spell_level(SPELL_LIMBREGO, CLASS_KAI, 1);
  spell_level(SPELL_PORTAL, CLASS_KAI, 1);
  spell_level(SPELL_PROT_FROM_EVIL, CLASS_KAI, 1);
  spell_level(SPELL_REMOVE_POISON, CLASS_KAI, 1);
  spell_level(SPELL_REMOVE_CURSE, CLASS_KAI, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_KAI, 1);
  spell_level(SPELL_MIND_FREEZE, CLASS_KAI, 1);
  spell_level(SPELL_HEAL, CLASS_KAI, 1);
  spell_level(SPELL_CURE_SOLAR_FLARE, CLASS_KAI, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_KAI, 1);
  spell_level(SPELL_BARRIER, CLASS_KAI, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_KAI, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_KAI, 1);
  spell_level(SPELL_KYODAIKA, CLASS_KAI, 1);
  spell_level(SKILL_KISHOT, CLASS_KAI, 1);
  spell_level(SKILL_SWEEP, CLASS_KAI, 1);
  spell_level(SKILL_HIDE, CLASS_KAI, 1);
  spell_level(SKILL_ZANELB, CLASS_KAI, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_KAI, 1);
  spell_level(SKILL_HEEL, CLASS_KAI, 1);
  spell_level(SKILL_RESCUE, CLASS_KAI, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_KAI, 1);
  spell_level(SKILL_MUG, CLASS_KAI, 1);
  spell_level(SKILL_SENSE, CLASS_KAI, 1);
  spell_level(SKILL_TAILWHIP, CLASS_KAI, 1);
  spell_level(SKILL_HEAVEN, CLASS_KAI, 1);
  spell_level(SKILL_REPAIR, CLASS_KAI, 1);
  spell_level(SKILL_KAIOKEN, CLASS_KAI, 1);
  spell_level(SKILL_KNEE, CLASS_KAI, 1);
  spell_level(SKILL_ELBOW, CLASS_KAI, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_KAI, 1);
  spell_level(SKILL_HYDRO, CLASS_KAI, 1);
  spell_level(SKILL_INGEST, CLASS_KAI, 1);
  spell_level(SKILL_ECLIPSE, CLASS_KAI, 1);
  spell_level(SKILL_PHEONIX, CLASS_KAI, 1);
  spell_level(SKILL_ABSORB, CLASS_KAI, 1);
  spell_level(SKILL_DESTRUCT, CLASS_KAI, 1);
  spell_level(SKILL_POWERSENSE, CLASS_KAI, 1);
  spell_level(SKILL_STEALTH, CLASS_KAI, 1);
  spell_level(SKILL_REGENERATE, CLASS_KAI, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_KAI, 1);
  spell_level(SKILL_BIGBANG, CLASS_KAI, 1);
  spell_level(SKILL_FINALFLASH, CLASS_KAI, 1);
  spell_level(SKILL_CHOUKAME, CLASS_KAI, 1);
  spell_level(SKILL_DISK, CLASS_KAI, 1);
  spell_level(SKILL_MASENKO, CLASS_KAI, 1);
  spell_level(SKILL_RENZO, CLASS_KAI, 1);
  spell_level(SKILL_MAKANKO, CLASS_KAI, 1);
  spell_level(SKILL_KISHOT, CLASS_KAI, 1);
  spell_level(SKILL_KIKOHO, CLASS_KAI, 1);
  spell_level(SKILL_DBALL, CLASS_KAI, 1);
  spell_level(SKILL_HFLASH, CLASS_KAI, 1);
  spell_level(SKILL_KOUSEN, CLASS_KAI, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_KAI, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_KAI, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_KAI, 1);
  spell_level(SKILL_TWIN, CLASS_KAI, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_KAI, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_KAI, 1);
  spell_level(SKILL_GIGABEAM, CLASS_KAI, 1);
  spell_level(SKILL_MEDITATE, CLASS_KAI, 1);
  spell_level(SKILL_BEAM, CLASS_KAI, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_KAI, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_KAI, 1);
  spell_level(SKILL_BBK, CLASS_KAI, 1);
  spell_level(SKILL_HAVOC, CLASS_KAI, 1);
  spell_level(SKILL_GALIKGUN, CLASS_KAI, 1);
  spell_level(SKILL_HONOO, CLASS_KAI, 1);
  spell_level(SKILL_BAKU, CLASS_KAI, 1);
  spell_level(SKILL_CRUSHER, CLASS_KAI, 1);
  spell_level(SKILL_ERASER, CLASS_KAI, 1);
  spell_level(SKILL_KYODAIKA, CLASS_KAI, 1);
  spell_level(SKILL_SCATTER, CLASS_KAI, 1);
  spell_level(SKILL_GENKI, CLASS_KAI, 1);
  spell_level(SKILL_FORELOCK, CLASS_KAI, 1);
  spell_level(SKILL_CHIKYUU, CLASS_KAI, 1);
  spell_level(SKILL_GENOCIDE, CLASS_KAI, 1);
  spell_level(SKILL_DEFLECT, CLASS_KAI, 1);
  spell_level(SKILL_POTENTIAL, CLASS_KAI, 1);
  spell_level(SKILL_MAJIN, CLASS_KAI, 1);
  spell_level(SKILL_MINDREAD, CLASS_KAI, 1);
  spell_level(SKILL_DUALWIELD, CLASS_KAI, 1);


  /* BIODROID */
  spell_level(SPELL_FISHING, CLASS_BIODROID, 1);
  spell_level(SPELL_IREG, CLASS_BIODROID, 1);
  spell_level(SPELL_PORTAL, CLASS_BIODROID, 1);
  spell_level(SPELL_INFRAVISION, CLASS_BIODROID, 1);
  spell_level(SPELL_BARRIER, CLASS_BIODROID, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_BIODROID, 1);
  spell_level(SPELL_MULTIFORM, CLASS_BIODROID, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_BIODROID, 1);
  spell_level(SPELL_SOLAR_FLARE, CLASS_BIODROID, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_BIODROID, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_BIODROID, 1);
  spell_level(SKILL_KISHOT, CLASS_BIODROID, 1);
  spell_level(SKILL_SWEEP, CLASS_BIODROID, 1);
  spell_level(SKILL_HIDE, CLASS_BIODROID, 1);
  spell_level(SKILL_ZANELB, CLASS_BIODROID, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_BIODROID, 1);
  spell_level(SKILL_HEEL, CLASS_BIODROID, 1);
  spell_level(SKILL_RESCUE, CLASS_BIODROID, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_BIODROID, 1);
  spell_level(SKILL_MUG, CLASS_BIODROID, 1);
  spell_level(SKILL_SENSE, CLASS_BIODROID, 1);
  spell_level(SKILL_TAILWHIP, CLASS_BIODROID, 1);
  spell_level(SKILL_HEAVEN, CLASS_BIODROID, 1);
  spell_level(SKILL_REPAIR, CLASS_BIODROID, 1);
  spell_level(SKILL_KAIOKEN, CLASS_BIODROID, 1);
  spell_level(SKILL_KNEE, CLASS_BIODROID, 1);
  spell_level(SKILL_ELBOW, CLASS_BIODROID, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_BIODROID, 1);
  spell_level(SKILL_HYDRO, CLASS_BIODROID, 1);
  spell_level(SKILL_INGEST, CLASS_BIODROID, 1);
  spell_level(SKILL_ECLIPSE, CLASS_BIODROID, 1);
  spell_level(SKILL_PHEONIX, CLASS_BIODROID, 1);
  spell_level(SKILL_ABSORB, CLASS_BIODROID, 1);
  spell_level(SKILL_DESTRUCT, CLASS_BIODROID, 1);
  spell_level(SKILL_POWERSENSE, CLASS_BIODROID, 1);
  spell_level(SKILL_STEALTH, CLASS_BIODROID, 1);
  spell_level(SKILL_REGENERATE, CLASS_BIODROID, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_BIODROID, 1);
  spell_level(SKILL_BIGBANG, CLASS_BIODROID, 1);
  spell_level(SKILL_FINALFLASH, CLASS_BIODROID, 1);
  spell_level(SKILL_CHOUKAME, CLASS_BIODROID, 1);
  spell_level(SKILL_DISK, CLASS_BIODROID, 1);
  spell_level(SKILL_MASENKO, CLASS_BIODROID, 1);
  spell_level(SKILL_RENZO, CLASS_BIODROID, 1);
  spell_level(SKILL_MAKANKO, CLASS_BIODROID, 1);
  spell_level(SKILL_KISHOT, CLASS_BIODROID, 1);
  spell_level(SKILL_KIKOHO, CLASS_BIODROID, 1);
  spell_level(SKILL_DBALL, CLASS_BIODROID, 1);
  spell_level(SKILL_HFLASH, CLASS_BIODROID, 1);
  spell_level(SKILL_KOUSEN, CLASS_BIODROID, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_BIODROID, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_BIODROID, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_BIODROID, 1);
  spell_level(SKILL_TWIN, CLASS_BIODROID, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_BIODROID, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_BIODROID, 1);
  spell_level(SKILL_GIGABEAM, CLASS_BIODROID, 1);
  spell_level(SKILL_MEDITATE, CLASS_BIODROID, 1);
  spell_level(SKILL_BEAM, CLASS_BIODROID, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_BIODROID, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_BIODROID, 1);
  spell_level(SKILL_BBK, CLASS_BIODROID, 1);
  spell_level(SKILL_HAVOC, CLASS_BIODROID, 1);
  spell_level(SKILL_GALIKGUN, CLASS_BIODROID, 1);
  spell_level(SKILL_HONOO, CLASS_BIODROID, 1);
  spell_level(SKILL_BAKU, CLASS_BIODROID, 1);
  spell_level(SKILL_CRUSHER, CLASS_BIODROID, 1);
  spell_level(SKILL_ERASER, CLASS_BIODROID, 1);
  spell_level(SKILL_KYODAIKA, CLASS_BIODROID, 1);
  spell_level(SKILL_SCATTER, CLASS_BIODROID, 1);
  spell_level(SKILL_GENKI, CLASS_BIODROID, 1);
  spell_level(SKILL_FORELOCK, CLASS_BIODROID, 1);
  spell_level(SKILL_CHIKYUU, CLASS_BIODROID, 1);
  spell_level(SKILL_GENOCIDE, CLASS_BIODROID, 1);
  spell_level(SKILL_DEFLECT, CLASS_BIODROID, 1);
  spell_level(SKILL_POTENTIAL, CLASS_BIODROID, 1);
  spell_level(SKILL_MAJIN, CLASS_BIODROID, 1);
  spell_level(SKILL_MINDREAD, CLASS_BIODROID, 1);
  spell_level(SKILL_DUALWIELD, CLASS_BIODROID, 1);

  /* KONATSU */
  spell_level(SPELL_FISHING, CLASS_KONATSU, 1);
  spell_level(SPELL_PORTAL, CLASS_KONATSU, 1);
  spell_level(SPELL_PROT_FROM_EVIL, CLASS_KONATSU, 1);
  spell_level(SKILL_PHEONIX, CLASS_KONATSU, 1);
  spell_level(SPELL_GROUP_INVISIBLE, CLASS_KONATSU, 1);
  spell_level(SPELL_INFRAVISION, CLASS_KONATSU, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_KONATSU, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_KONATSU, 1);
  spell_level(SPELL_SWORD, CLASS_KONATSU, 1);
  spell_level(SPELL_INFRAVISION, CLASS_KONATSU, 1);
  spell_level(SPELL_INVISIBLE, CLASS_KONATSU, 1);
  spell_level(SPELL_BURNING_SOULS, CLASS_KONATSU, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_KONATSU, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_KONATSU, 1);
  spell_level(SKILL_KISHOT, CLASS_KONATSU, 1);
  spell_level(SKILL_SWEEP, CLASS_KONATSU, 1);
  spell_level(SKILL_HIDE, CLASS_KONATSU, 1);
  spell_level(SKILL_ZANELB, CLASS_KONATSU, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_KONATSU, 1);
  spell_level(SKILL_HEEL, CLASS_KONATSU, 1);
  spell_level(SKILL_RESCUE, CLASS_KONATSU, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_KONATSU, 1);
  spell_level(SKILL_MUG, CLASS_KONATSU, 1);
  spell_level(SKILL_SENSE, CLASS_KONATSU, 1);
  spell_level(SKILL_TAILWHIP, CLASS_KONATSU, 1);
  spell_level(SKILL_HEAVEN, CLASS_KONATSU, 1);
  spell_level(SKILL_REPAIR, CLASS_KONATSU, 1);
  spell_level(SKILL_KAIOKEN, CLASS_KONATSU, 1);
  spell_level(SKILL_KNEE, CLASS_KONATSU, 1);
  spell_level(SKILL_ELBOW, CLASS_KONATSU, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_KONATSU, 1);
  spell_level(SKILL_HYDRO, CLASS_KONATSU, 1);
  spell_level(SKILL_INGEST, CLASS_KONATSU, 1);
  spell_level(SKILL_ECLIPSE, CLASS_KONATSU, 1);
  spell_level(SKILL_PHEONIX, CLASS_KONATSU, 1);
  spell_level(SKILL_ABSORB, CLASS_KONATSU, 1);
  spell_level(SKILL_DESTRUCT, CLASS_KONATSU, 1);
  spell_level(SKILL_POWERSENSE, CLASS_KONATSU, 1);
  spell_level(SKILL_STEALTH, CLASS_KONATSU, 1);
  spell_level(SKILL_REGENERATE, CLASS_KONATSU, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_KONATSU, 1);
  spell_level(SKILL_BIGBANG, CLASS_KONATSU, 1);
  spell_level(SKILL_FINALFLASH, CLASS_KONATSU, 1);
  spell_level(SKILL_CHOUKAME, CLASS_KONATSU, 1);
  spell_level(SKILL_DISK, CLASS_KONATSU, 1);
  spell_level(SKILL_MASENKO, CLASS_KONATSU, 1);
  spell_level(SKILL_RENZO, CLASS_KONATSU, 1);
  spell_level(SKILL_MAKANKO, CLASS_KONATSU, 1);
  spell_level(SKILL_KISHOT, CLASS_KONATSU, 1);
  spell_level(SKILL_KIKOHO, CLASS_KONATSU, 1);
  spell_level(SKILL_DBALL, CLASS_KONATSU, 1);
  spell_level(SKILL_HFLASH, CLASS_KONATSU, 1);
  spell_level(SKILL_KOUSEN, CLASS_KONATSU, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_KONATSU, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_KONATSU, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_KONATSU, 1);
  spell_level(SKILL_TWIN, CLASS_KONATSU, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_KONATSU, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_KONATSU, 1);
  spell_level(SKILL_GIGABEAM, CLASS_KONATSU, 1);
  spell_level(SKILL_MEDITATE, CLASS_KONATSU, 1);
  spell_level(SKILL_BEAM, CLASS_KONATSU, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_KONATSU, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_KONATSU, 1);
  spell_level(SKILL_BBK, CLASS_KONATSU, 1);
  spell_level(SKILL_HAVOC, CLASS_KONATSU, 1);
  spell_level(SKILL_GALIKGUN, CLASS_KONATSU, 1);
  spell_level(SKILL_HONOO, CLASS_KONATSU, 1);
  spell_level(SKILL_BAKU, CLASS_KONATSU, 1);
  spell_level(SKILL_CRUSHER, CLASS_KONATSU, 1);
  spell_level(SKILL_ERASER, CLASS_KONATSU, 1);
  spell_level(SKILL_KYODAIKA, CLASS_KONATSU, 1);
  spell_level(SKILL_SCATTER, CLASS_KONATSU, 1);
  spell_level(SKILL_GENKI, CLASS_KONATSU, 1);
  spell_level(SKILL_FORELOCK, CLASS_KONATSU, 1);
  spell_level(SKILL_CHIKYUU, CLASS_KONATSU, 1);
  spell_level(SKILL_GENOCIDE, CLASS_KONATSU, 1);
  spell_level(SKILL_DEFLECT, CLASS_KONATSU, 1);
  spell_level(SKILL_POTENTIAL, CLASS_KONATSU, 1);
  spell_level(SKILL_MAJIN, CLASS_KONATSU, 1);
  spell_level(SKILL_MINDREAD, CLASS_KONATSU, 1);
  spell_level(SKILL_DUALWIELD, CLASS_KONATSU, 1);
  
  /* Kanassan */
  spell_level(SPELL_FISHING, CLASS_KANASSAN, 1);
  spell_level(SPELL_PORTAL, CLASS_KANASSAN, 1);
  spell_level(SPELL_PROT_FROM_EVIL, CLASS_KANASSAN, 1);
  spell_level(SPELL_REMOVE_POISON, CLASS_KANASSAN, 1);
  spell_level(SPELL_REMOVE_CURSE, CLASS_KANASSAN, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_KANASSAN, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_KANASSAN, 1);
  spell_level(SPELL_BLADEMORPH, CLASS_KANASSAN, 1);
  spell_level(SPELL_INFRAVISION, CLASS_KANASSAN, 1);
  spell_level(SPELL_MUTATE, CLASS_KANASSAN, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_KANASSAN, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_KANASSAN, 1);
  spell_level(SKILL_KISHOT, CLASS_KANASSAN, 1);
  spell_level(SKILL_SWEEP, CLASS_KANASSAN, 1);
  spell_level(SKILL_HIDE, CLASS_KANASSAN, 1);
  spell_level(SKILL_ZANELB, CLASS_KANASSAN, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_KANASSAN, 1);
  spell_level(SKILL_HEEL, CLASS_KANASSAN, 1);
  spell_level(SKILL_RESCUE, CLASS_KANASSAN, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_KANASSAN, 1);
  spell_level(SKILL_MUG, CLASS_KANASSAN, 1);
  spell_level(SKILL_SENSE, CLASS_KANASSAN, 1);
  spell_level(SKILL_TAILWHIP, CLASS_KANASSAN, 1);
  spell_level(SKILL_HEAVEN, CLASS_KANASSAN, 1);
  spell_level(SKILL_REPAIR, CLASS_KANASSAN, 1);
  spell_level(SKILL_KAIOKEN, CLASS_KANASSAN, 1);
  spell_level(SKILL_KNEE, CLASS_KANASSAN, 1);
  spell_level(SKILL_ELBOW, CLASS_KANASSAN, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_KANASSAN, 1);
  spell_level(SKILL_HYDRO, CLASS_KANASSAN, 1);
  spell_level(SKILL_INGEST, CLASS_KANASSAN, 1);
  spell_level(SKILL_ECLIPSE, CLASS_KANASSAN, 1);
  spell_level(SKILL_PHEONIX, CLASS_KANASSAN, 1);
  spell_level(SKILL_ABSORB, CLASS_KANASSAN, 1);
  spell_level(SKILL_DESTRUCT, CLASS_KANASSAN, 1);
  spell_level(SKILL_POWERSENSE, CLASS_KANASSAN, 1);
  spell_level(SKILL_STEALTH, CLASS_KANASSAN, 1);
  spell_level(SKILL_REGENERATE, CLASS_KANASSAN, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_KANASSAN, 1);
  spell_level(SKILL_BIGBANG, CLASS_KANASSAN, 1);
  spell_level(SKILL_FINALFLASH, CLASS_KANASSAN, 1);
  spell_level(SKILL_CHOUKAME, CLASS_KANASSAN, 1);
  spell_level(SKILL_DISK, CLASS_KANASSAN, 1);
  spell_level(SKILL_MASENKO, CLASS_KANASSAN, 1);
  spell_level(SKILL_RENZO, CLASS_KANASSAN, 1);
  spell_level(SKILL_MAKANKO, CLASS_KANASSAN, 1);
  spell_level(SKILL_KISHOT, CLASS_KANASSAN, 1);
  spell_level(SKILL_KIKOHO, CLASS_KANASSAN, 1);
  spell_level(SKILL_DBALL, CLASS_KANASSAN, 1);
  spell_level(SKILL_HFLASH, CLASS_KANASSAN, 1);
  spell_level(SKILL_KOUSEN, CLASS_KANASSAN, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_KANASSAN, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_KANASSAN, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_KANASSAN, 1);
  spell_level(SKILL_TWIN, CLASS_KANASSAN, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_KANASSAN, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_KANASSAN, 1);
  spell_level(SKILL_GIGABEAM, CLASS_KANASSAN, 1);
  spell_level(SKILL_MEDITATE, CLASS_KANASSAN, 1);
  spell_level(SKILL_BEAM, CLASS_KANASSAN, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_KANASSAN, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_KANASSAN, 1);
  spell_level(SKILL_BBK, CLASS_KANASSAN, 1);
  spell_level(SKILL_HAVOC, CLASS_KANASSAN, 1);
  spell_level(SKILL_GALIKGUN, CLASS_KANASSAN, 1);
  spell_level(SKILL_HONOO, CLASS_KANASSAN, 1);
  spell_level(SKILL_BAKU, CLASS_KANASSAN, 1);
  spell_level(SKILL_CRUSHER, CLASS_KANASSAN, 1);
  spell_level(SKILL_ERASER, CLASS_KANASSAN, 1);
  spell_level(SKILL_KYODAIKA, CLASS_KANASSAN, 1);
  spell_level(SKILL_SCATTER, CLASS_KANASSAN, 1);
  spell_level(SKILL_GENKI, CLASS_KANASSAN, 1);
  spell_level(SKILL_FORELOCK, CLASS_KANASSAN, 1);
  spell_level(SKILL_CHIKYUU, CLASS_KANASSAN, 1);
  spell_level(SKILL_GENOCIDE, CLASS_KANASSAN, 1);
  spell_level(SKILL_DEFLECT, CLASS_KANASSAN, 1);
  spell_level(SKILL_POTENTIAL, CLASS_KANASSAN, 1);
  spell_level(SKILL_MAJIN, CLASS_KANASSAN, 1);
  spell_level(SKILL_MINDREAD, CLASS_KANASSAN, 1);
  spell_level(SKILL_DUALWIELD, CLASS_KANASSAN, 1);

  /* ANGEL */
  spell_level(SPELL_FISHING, CLASS_ANGEL, 1); 
  spell_level(SPELL_MAGIC, CLASS_ANGEL, 1);
  spell_level(SPELL_RESTORE, CLASS_ANGEL, 1);
  spell_level(SPELL_PORTAL, CLASS_ANGEL, 1);
  spell_level(SPELL_PROT_FROM_EVIL, CLASS_ANGEL, 1);
  spell_level(SPELL_REMOVE_POISON, CLASS_ANGEL, 1);
  spell_level(SPELL_REMOVE_CURSE, CLASS_ANGEL, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_ANGEL, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_ANGEL, 1);
  spell_level(SPELL_BLADEMORPH, CLASS_ANGEL, 1);
  spell_level(SPELL_INFRAVISION, CLASS_ANGEL, 1);
  spell_level(SPELL_MUTATE, CLASS_ANGEL, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_ANGEL, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_ANGEL, 1);
  spell_level(SKILL_KISHOT, CLASS_ANGEL, 1);
  spell_level(SKILL_SWEEP, CLASS_ANGEL, 1);
  spell_level(SKILL_HIDE, CLASS_ANGEL, 1);
  spell_level(SKILL_ZANELB, CLASS_ANGEL, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_ANGEL, 1);
  spell_level(SKILL_HEEL, CLASS_ANGEL, 1);
  spell_level(SKILL_RESCUE, CLASS_ANGEL, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_ANGEL, 1);
  spell_level(SKILL_MUG, CLASS_ANGEL, 1);
  spell_level(SKILL_SENSE, CLASS_ANGEL, 1);
  spell_level(SKILL_TAILWHIP, CLASS_ANGEL, 1);
  spell_level(SKILL_HEAVEN, CLASS_ANGEL, 1);
  spell_level(SKILL_REPAIR, CLASS_ANGEL, 1);
  spell_level(SKILL_KAIOKEN, CLASS_ANGEL, 1);
  spell_level(SKILL_KNEE, CLASS_ANGEL, 1);
  spell_level(SKILL_ELBOW, CLASS_ANGEL, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_ANGEL, 1);
  spell_level(SKILL_COSMIC, CLASS_ANGEL, 1);
  spell_level(SKILL_HYDRO, CLASS_ANGEL, 1);
  spell_level(SKILL_INGEST, CLASS_ANGEL, 1);
  spell_level(SKILL_ECLIPSE, CLASS_ANGEL, 1);
  spell_level(SKILL_PHEONIX, CLASS_ANGEL, 1);
  spell_level(SKILL_ABSORB, CLASS_ANGEL, 1);
  spell_level(SKILL_DESTRUCT, CLASS_ANGEL, 1);
  spell_level(SKILL_POWERSENSE, CLASS_ANGEL, 1);
  spell_level(SKILL_STEALTH, CLASS_ANGEL, 1);
  spell_level(SKILL_REGENERATE, CLASS_ANGEL, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_ANGEL, 1);
  spell_level(SKILL_BIGBANG, CLASS_ANGEL, 1);
  spell_level(SKILL_FINALFLASH, CLASS_ANGEL, 1);
  spell_level(SKILL_CHOUKAME, CLASS_ANGEL, 1);
  spell_level(SKILL_DISK, CLASS_ANGEL, 1);
  spell_level(SKILL_MASENKO, CLASS_ANGEL, 1);
  spell_level(SKILL_RENZO, CLASS_ANGEL, 1);
  spell_level(SKILL_MAKANKO, CLASS_ANGEL, 1);
  spell_level(SKILL_KISHOT, CLASS_ANGEL, 1);
  spell_level(SKILL_KIKOHO, CLASS_ANGEL, 1);
  spell_level(SKILL_DBALL, CLASS_ANGEL, 1);
  spell_level(SKILL_HFLASH, CLASS_ANGEL, 1);
  spell_level(SKILL_KOUSEN, CLASS_ANGEL, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_ANGEL, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_ANGEL, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_ANGEL, 1);
  spell_level(SKILL_TWIN, CLASS_ANGEL, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_ANGEL, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_ANGEL, 1);
  spell_level(SKILL_GIGABEAM, CLASS_ANGEL, 1);
  spell_level(SKILL_MEDITATE, CLASS_ANGEL, 1);
  spell_level(SKILL_BEAM, CLASS_ANGEL, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_ANGEL, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_ANGEL, 1);
  spell_level(SKILL_BBK, CLASS_ANGEL, 1);
  spell_level(SKILL_HAVOC, CLASS_ANGEL, 1);
  spell_level(SKILL_GALIKGUN, CLASS_ANGEL, 1);
  spell_level(SKILL_HONOO, CLASS_ANGEL, 1);
  spell_level(SKILL_BAKU, CLASS_ANGEL, 1);
  spell_level(SKILL_CRUSHER, CLASS_ANGEL, 1);
  spell_level(SKILL_ERASER, CLASS_ANGEL, 1);
  spell_level(SKILL_KYODAIKA, CLASS_ANGEL, 1);
  spell_level(SKILL_SCATTER, CLASS_ANGEL, 1);
  spell_level(SKILL_GENKI, CLASS_ANGEL, 1);
  spell_level(SKILL_FORELOCK, CLASS_ANGEL, 1);
  spell_level(SKILL_CHIKYUU, CLASS_ANGEL, 1);
  spell_level(SKILL_GENOCIDE, CLASS_ANGEL, 1);
  spell_level(SKILL_DEFLECT, CLASS_ANGEL, 1);
  spell_level(SKILL_POTENTIAL, CLASS_ANGEL, 1);
  spell_level(SKILL_MAJIN, CLASS_ANGEL, 1);
  spell_level(SKILL_MINDREAD, CLASS_ANGEL, 1);
  spell_level(SKILL_DUALWIELD, CLASS_ANGEL, 1);

  /* DEMON */
  spell_level(SPELL_FISHING, CLASS_demon, 1);
  spell_level(SPELL_PORTAL, CLASS_demon, 1);
  spell_level(SPELL_CURSE, CLASS_demon, 1);
  spell_level(SPELL_REMOVE_CURSE, CLASS_demon, 1);
  spell_level(SPELL_SPEAR, CLASS_demon, 1);
  spell_level(SPELL_ANIMATE_DEAD, CLASS_demon, 1);
  spell_level(SPELL_BARRIER, CLASS_demon, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_demon, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_demon, 1);
  spell_level(SPELL_HEAL, CLASS_demon, 1);
  spell_level(SPELL_SUMMON, CLASS_demon, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_demon, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_demon, 1);
  spell_level(SKILL_KISHOT, CLASS_demon, 1);
  spell_level(SKILL_SWEEP, CLASS_demon, 1);
  spell_level(SKILL_HIDE, CLASS_demon, 1);
  spell_level(SKILL_ZANELB, CLASS_demon, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_demon, 1);
  spell_level(SKILL_HEEL, CLASS_demon, 1);
  spell_level(SKILL_RESCUE, CLASS_demon, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_demon, 1);
  spell_level(SKILL_MUG, CLASS_demon, 1);
  spell_level(SKILL_SENSE, CLASS_demon, 1);
  spell_level(SKILL_TAILWHIP, CLASS_demon, 1);
  spell_level(SKILL_HEAVEN, CLASS_demon, 1);
  spell_level(SKILL_REPAIR, CLASS_demon, 1);
  spell_level(SKILL_KAIOKEN, CLASS_demon, 1);
  spell_level(SKILL_KNEE, CLASS_demon, 1);
  spell_level(SKILL_ELBOW, CLASS_demon, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_demon, 1);
  spell_level(SKILL_HYDRO, CLASS_demon, 1);
  spell_level(SKILL_INGEST, CLASS_demon, 1);
  spell_level(SKILL_ECLIPSE, CLASS_demon, 1);
  spell_level(SKILL_PHEONIX, CLASS_demon, 1);
  spell_level(SKILL_ABSORB, CLASS_demon, 1);
  spell_level(SKILL_DESTRUCT, CLASS_demon, 1);
  spell_level(SKILL_POWERSENSE, CLASS_demon, 1);
  spell_level(SKILL_STEALTH, CLASS_demon, 1);
  spell_level(SKILL_REGENERATE, CLASS_demon, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_demon, 1);
  spell_level(SKILL_BIGBANG, CLASS_demon, 1);
  spell_level(SKILL_FINALFLASH, CLASS_demon, 1);
  spell_level(SKILL_CHOUKAME, CLASS_demon, 1);
  spell_level(SKILL_DISK, CLASS_demon, 1);
  spell_level(SKILL_MASENKO, CLASS_demon, 1);
  spell_level(SKILL_RENZO, CLASS_demon, 1);
  spell_level(SKILL_MAKANKO, CLASS_demon, 1);
  spell_level(SKILL_KISHOT, CLASS_demon, 1);
  spell_level(SKILL_KIKOHO, CLASS_demon, 1);
  spell_level(SKILL_DBALL, CLASS_demon, 1);
  spell_level(SKILL_HFLASH, CLASS_demon, 1);
  spell_level(SKILL_KOUSEN, CLASS_demon, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_demon, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_demon, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_demon, 1);
  spell_level(SKILL_TWIN, CLASS_demon, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_demon, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_demon, 1);
  spell_level(SKILL_GIGABEAM, CLASS_demon, 1);
  spell_level(SKILL_MEDITATE, CLASS_demon, 1);
  spell_level(SKILL_BEAM, CLASS_demon, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_demon, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_demon, 1);
  spell_level(SKILL_BBK, CLASS_demon, 1);
  spell_level(SKILL_HAVOC, CLASS_demon, 1);
  spell_level(SKILL_GALIKGUN, CLASS_demon, 1);
  spell_level(SKILL_HONOO, CLASS_demon, 1);
  spell_level(SKILL_BAKU, CLASS_demon, 1);
  spell_level(SKILL_CRUSHER, CLASS_demon, 1);
  spell_level(SKILL_ERASER, CLASS_demon, 1);
  spell_level(SKILL_KYODAIKA, CLASS_demon, 1);
  spell_level(SKILL_SCATTER, CLASS_demon, 1);
  spell_level(SKILL_GENKI, CLASS_demon, 1);
  spell_level(SKILL_FORELOCK, CLASS_demon, 1);
  spell_level(SKILL_CHIKYUU, CLASS_demon, 1);
  spell_level(SKILL_GENOCIDE, CLASS_demon, 1);
  spell_level(SKILL_DEFLECT, CLASS_demon, 1);
  spell_level(SKILL_POTENTIAL, CLASS_demon, 1);
  spell_level(SKILL_MAJIN, CLASS_demon, 1);
  spell_level(SKILL_MINDREAD, CLASS_demon, 1);
  spell_level(SKILL_DUALWIELD, CLASS_demon, 1);

  /* TRUFFLE */
  spell_level(SPELL_FISHING, CLASS_TRUFFLE, 1);
  spell_level(SPELL_STASIS, CLASS_TRUFFLE, 1);
  spell_level(SPELL_PORTAL, CLASS_TRUFFLE, 1);
  spell_level(SPELL_BUILDNANO, CLASS_TRUFFLE, 1);
  spell_level(SPELL_BUILDSHIELD, CLASS_TRUFFLE, 1);
  spell_level(SPELL_GROUP_HEAL, CLASS_TRUFFLE, 1);
  spell_level(SPELL_GROUP_armor, CLASS_TRUFFLE, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_TRUFFLE, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_TRUFFLE, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_TRUFFLE, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_TRUFFLE, 1);
  spell_level(SKILL_KISHOT, CLASS_TRUFFLE, 1);
  spell_level(SKILL_SWEEP, CLASS_TRUFFLE, 1);
  spell_level(SKILL_HIDE, CLASS_TRUFFLE, 1);
  spell_level(SKILL_ZANELB, CLASS_TRUFFLE, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_TRUFFLE, 1);
  spell_level(SKILL_HEEL, CLASS_TRUFFLE, 1);
  spell_level(SKILL_RESCUE, CLASS_TRUFFLE, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_TRUFFLE, 1);
  spell_level(SKILL_MUG, CLASS_TRUFFLE, 1);
  spell_level(SKILL_SENSE, CLASS_TRUFFLE, 1);
  spell_level(SKILL_TAILWHIP, CLASS_TRUFFLE, 1);
  spell_level(SKILL_HEAVEN, CLASS_TRUFFLE, 1);
  spell_level(SKILL_REPAIR, CLASS_TRUFFLE, 1);
  spell_level(SKILL_KAIOKEN, CLASS_TRUFFLE, 1);
  spell_level(SKILL_KNEE, CLASS_TRUFFLE, 1);
  spell_level(SKILL_ELBOW, CLASS_TRUFFLE, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_TRUFFLE, 1);
  spell_level(SKILL_HYDRO, CLASS_TRUFFLE, 1);
  spell_level(SKILL_INGEST, CLASS_TRUFFLE, 1);
  spell_level(SKILL_ECLIPSE, CLASS_TRUFFLE, 1);
  spell_level(SKILL_PHEONIX, CLASS_TRUFFLE, 1);
  spell_level(SKILL_ABSORB, CLASS_TRUFFLE, 1);
  spell_level(SKILL_DESTRUCT, CLASS_TRUFFLE, 1);
  spell_level(SKILL_POWERSENSE, CLASS_TRUFFLE, 1);
  spell_level(SKILL_STEALTH, CLASS_TRUFFLE, 1);
  spell_level(SKILL_REGENERATE, CLASS_TRUFFLE, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_TRUFFLE, 1);
  spell_level(SKILL_BIGBANG, CLASS_TRUFFLE, 1);
  spell_level(SKILL_FINALFLASH, CLASS_TRUFFLE, 1);
  spell_level(SKILL_CHOUKAME, CLASS_TRUFFLE, 1);
  spell_level(SKILL_DISK, CLASS_TRUFFLE, 1);
  spell_level(SKILL_MASENKO, CLASS_TRUFFLE, 1);
  spell_level(SKILL_RENZO, CLASS_TRUFFLE, 1);
  spell_level(SKILL_MAKANKO, CLASS_TRUFFLE, 1);
  spell_level(SKILL_KISHOT, CLASS_TRUFFLE, 1);
  spell_level(SKILL_KIKOHO, CLASS_TRUFFLE, 1);
  spell_level(SKILL_DBALL, CLASS_TRUFFLE, 1);
  spell_level(SKILL_HFLASH, CLASS_TRUFFLE, 1);
  spell_level(SKILL_KOUSEN, CLASS_TRUFFLE, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_TRUFFLE, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_TRUFFLE, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_TRUFFLE, 1);
  spell_level(SKILL_TWIN, CLASS_TRUFFLE, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_TRUFFLE, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_TRUFFLE, 1);
  spell_level(SKILL_GIGABEAM, CLASS_TRUFFLE, 1);
  spell_level(SKILL_MEDITATE, CLASS_TRUFFLE, 1);
  spell_level(SKILL_BEAM, CLASS_TRUFFLE, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_TRUFFLE, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_TRUFFLE, 1);
  spell_level(SKILL_BBK, CLASS_TRUFFLE, 1);
  spell_level(SKILL_HAVOC, CLASS_TRUFFLE, 1);
  spell_level(SKILL_GALIKGUN, CLASS_TRUFFLE, 1);
  spell_level(SKILL_HONOO, CLASS_TRUFFLE, 1);
  spell_level(SKILL_BAKU, CLASS_TRUFFLE, 1);
  spell_level(SKILL_CRUSHER, CLASS_TRUFFLE, 1);
  spell_level(SKILL_ERASER, CLASS_TRUFFLE, 1);
  spell_level(SKILL_KYODAIKA, CLASS_TRUFFLE, 1);
  spell_level(SKILL_SCATTER, CLASS_TRUFFLE, 1);
  spell_level(SKILL_GENKI, CLASS_TRUFFLE, 1);
  spell_level(SKILL_FORELOCK, CLASS_TRUFFLE, 1);
  spell_level(SKILL_CHIKYUU, CLASS_TRUFFLE, 1);
  spell_level(SKILL_GENOCIDE, CLASS_TRUFFLE, 1);
  spell_level(SKILL_DEFLECT, CLASS_TRUFFLE, 1);
  spell_level(SKILL_POTENTIAL, CLASS_TRUFFLE, 1);
  spell_level(SKILL_MAJIN, CLASS_TRUFFLE, 1);
  spell_level(SKILL_MINDREAD, CLASS_TRUFFLE, 1);
  spell_level(SKILL_DUALWIELD, CLASS_TRUFFLE, 1);

  /* MUTANT */
  spell_level(SPELL_FISHING, CLASS_MUTANT, 1);
  spell_level(SPELL_PLATED, CLASS_MUTANT, 1);
  spell_level(SPELL_FRENZY, CLASS_MUTANT, 1);
  spell_level(SPELL_PORTAL, CLASS_MUTANT, 1);
  spell_level(SPELL_MUTATE, CLASS_MUTANT, 1);
  spell_level(SPELL_BLADEMORPH, CLASS_MUTANT, 1);
  spell_level(SPELL_GROUP_HEAL, CLASS_MUTANT, 1);
  spell_level(SPELL_POISON, CLASS_MUTANT, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_MUTANT, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_MUTANT, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_MUTANT, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_MUTANT, 1);
  spell_level(SKILL_KISHOT, CLASS_MUTANT, 1);
  spell_level(SKILL_SWEEP, CLASS_MUTANT, 1);
  spell_level(SKILL_HIDE, CLASS_MUTANT, 1);
  spell_level(SKILL_ZANELB, CLASS_MUTANT, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_MUTANT, 1);
  spell_level(SKILL_HEEL, CLASS_MUTANT, 1);
  spell_level(SKILL_RESCUE, CLASS_MUTANT, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_MUTANT, 1);
  spell_level(SKILL_MUG, CLASS_MUTANT, 1);
  spell_level(SKILL_SENSE, CLASS_MUTANT, 1);
  spell_level(SKILL_TAILWHIP, CLASS_MUTANT, 1);
  spell_level(SKILL_HEAVEN, CLASS_MUTANT, 1);
  spell_level(SKILL_REPAIR, CLASS_MUTANT, 1);
  spell_level(SKILL_KAIOKEN, CLASS_MUTANT, 1);
  spell_level(SKILL_KNEE, CLASS_MUTANT, 1);
  spell_level(SKILL_ELBOW, CLASS_MUTANT, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_MUTANT, 1);
  spell_level(SKILL_HYDRO, CLASS_MUTANT, 1);
  spell_level(SKILL_INGEST, CLASS_MUTANT, 1);
  spell_level(SKILL_ECLIPSE, CLASS_MUTANT, 1);
  spell_level(SKILL_PHEONIX, CLASS_MUTANT, 1);
  spell_level(SKILL_ABSORB, CLASS_MUTANT, 1);
  spell_level(SKILL_DESTRUCT, CLASS_MUTANT, 1);
  spell_level(SKILL_POWERSENSE, CLASS_MUTANT, 1);
  spell_level(SKILL_STEALTH, CLASS_MUTANT, 1);
  spell_level(SKILL_REGENERATE, CLASS_MUTANT, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_MUTANT, 1);
  spell_level(SKILL_BIGBANG, CLASS_MUTANT, 1);
  spell_level(SKILL_FINALFLASH, CLASS_MUTANT, 1);
  spell_level(SKILL_CHOUKAME, CLASS_MUTANT, 1);
  spell_level(SKILL_DISK, CLASS_MUTANT, 1);
  spell_level(SKILL_MASENKO, CLASS_MUTANT, 1);
  spell_level(SKILL_RENZO, CLASS_MUTANT, 1);
  spell_level(SKILL_MAKANKO, CLASS_MUTANT, 1);
  spell_level(SKILL_KISHOT, CLASS_MUTANT, 1);
  spell_level(SKILL_KIKOHO, CLASS_MUTANT, 1);
  spell_level(SKILL_DBALL, CLASS_MUTANT, 1);
  spell_level(SKILL_HFLASH, CLASS_MUTANT, 1);
  spell_level(SKILL_KOUSEN, CLASS_MUTANT, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_MUTANT, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_MUTANT, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_MUTANT, 1);
  spell_level(SKILL_TWIN, CLASS_MUTANT, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_MUTANT, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_MUTANT, 1);
  spell_level(SKILL_GIGABEAM, CLASS_MUTANT, 1);
  spell_level(SKILL_MEDITATE, CLASS_MUTANT, 1);
  spell_level(SKILL_BEAM, CLASS_MUTANT, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_MUTANT, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_MUTANT, 1);
  spell_level(SKILL_BBK, CLASS_MUTANT, 1);
  spell_level(SKILL_HAVOC, CLASS_MUTANT, 1);
  spell_level(SKILL_GALIKGUN, CLASS_MUTANT, 1);
  spell_level(SKILL_HONOO, CLASS_MUTANT, 1);
  spell_level(SKILL_BAKU, CLASS_MUTANT, 1);
  spell_level(SKILL_CRUSHER, CLASS_MUTANT, 1);
  spell_level(SKILL_ERASER, CLASS_MUTANT, 1);
  spell_level(SKILL_KYODAIKA, CLASS_MUTANT, 1);
  spell_level(SKILL_SCATTER, CLASS_MUTANT, 1);
  spell_level(SKILL_GENKI, CLASS_MUTANT, 1);
  spell_level(SKILL_FORELOCK, CLASS_MUTANT, 1);
  spell_level(SKILL_CHIKYUU, CLASS_MUTANT, 1);
  spell_level(SKILL_GENOCIDE, CLASS_MUTANT, 1);
  spell_level(SKILL_DEFLECT, CLASS_MUTANT, 1);
  spell_level(SKILL_POTENTIAL, CLASS_MUTANT, 1);
  spell_level(SKILL_MAJIN, CLASS_MUTANT, 1);
  spell_level(SKILL_MINDREAD, CLASS_MUTANT, 1);
  spell_level(SKILL_DUALWIELD, CLASS_MUTANT, 1);

  /* Half-Breed */
  spell_level(SPELL_FISHING, CLASS_HALF_BREED, 1);
  spell_level(SPELL_PORTAL, CLASS_HALF_BREED, 1);
  spell_level(SPELL_DETECT_INVISIBILTY, CLASS_HALF_BREED, 1);
  spell_level(SPELL_ZANZOKEN, CLASS_HALF_BREED, 1);
  spell_level(SPELL_BARRIER, CLASS_HALF_BREED, 1);
  spell_level(SPELL_HASSHUKEN, CLASS_HALF_BREED, 1);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_HALF_BREED, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_HALF_BREED, 1);
  spell_level(SKILL_KISHOT, CLASS_HALF_BREED, 1);
  spell_level(SKILL_SWEEP, CLASS_HALF_BREED, 1);
  spell_level(SKILL_HIDE, CLASS_HALF_BREED, 1);
  spell_level(SKILL_ZANELB, CLASS_HALF_BREED, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_HALF_BREED, 1);
  spell_level(SKILL_HEEL, CLASS_HALF_BREED, 1);
  spell_level(SKILL_RESCUE, CLASS_HALF_BREED, 1);
  spell_level(SKILL_SWIFTNESS, CLASS_HALF_BREED, 1);
  spell_level(SKILL_MUG, CLASS_HALF_BREED, 1);
  spell_level(SKILL_SENSE, CLASS_HALF_BREED, 1);
  spell_level(SKILL_TAILWHIP, CLASS_HALF_BREED, 1);
  spell_level(SKILL_HEAVEN, CLASS_HALF_BREED, 1);
  spell_level(SKILL_REPAIR, CLASS_HALF_BREED, 1);
  spell_level(SKILL_KAIOKEN, CLASS_HALF_BREED, 1);
  spell_level(SKILL_KNEE, CLASS_HALF_BREED, 1);
  spell_level(SKILL_ELBOW, CLASS_HALF_BREED, 1);
  spell_level(SKILL_ROUNDHOUSE, CLASS_HALF_BREED, 1);
  spell_level(SKILL_HYDRO, CLASS_HALF_BREED, 1);
  spell_level(SKILL_INGEST, CLASS_HALF_BREED, 1);
  spell_level(SKILL_ECLIPSE, CLASS_HALF_BREED, 1);
  spell_level(SKILL_PHEONIX, CLASS_HALF_BREED, 1);
  spell_level(SKILL_ABSORB, CLASS_HALF_BREED, 1);
  spell_level(SKILL_DESTRUCT, CLASS_HALF_BREED, 1);
  spell_level(SKILL_POWERSENSE, CLASS_HALF_BREED, 1);
  spell_level(SKILL_STEALTH, CLASS_HALF_BREED, 1);
  spell_level(SKILL_REGENERATE, CLASS_HALF_BREED, 1);
  spell_level(SKILL_KAMEHAMEHA, CLASS_HALF_BREED, 1);
  spell_level(SKILL_BIGBANG, CLASS_HALF_BREED, 1);
  spell_level(SKILL_FINALFLASH, CLASS_HALF_BREED, 1);
  spell_level(SKILL_CHOUKAME, CLASS_HALF_BREED, 1);
  spell_level(SKILL_DISK, CLASS_HALF_BREED, 1);
  spell_level(SKILL_MASENKO, CLASS_HALF_BREED, 1);
  spell_level(SKILL_RENZO, CLASS_HALF_BREED, 1);
  spell_level(SKILL_MAKANKO, CLASS_HALF_BREED, 1);
  spell_level(SKILL_KISHOT, CLASS_HALF_BREED, 1);
  spell_level(SKILL_KIKOHO, CLASS_HALF_BREED, 1);
  spell_level(SKILL_DBALL, CLASS_HALF_BREED, 1);
  spell_level(SKILL_HFLASH, CLASS_HALF_BREED, 1);
  spell_level(SKILL_KOUSEN, CLASS_HALF_BREED, 1);
  spell_level(SKILL_BURNINGATTACK, CLASS_HALF_BREED, 1);
  spell_level(SKILL_SPIRITBALL, CLASS_HALF_BREED, 1);
  spell_level(SKILL_SEVENDRAGON, CLASS_HALF_BREED, 1);
  spell_level(SKILL_TWIN, CLASS_HALF_BREED, 1);
  spell_level(SKILL_KAKUSANHA, CLASS_HALF_BREED, 1);
  spell_level(SKILL_HELLSPEAR, CLASS_HALF_BREED, 1);
  spell_level(SKILL_GIGABEAM, CLASS_HALF_BREED, 1);
  spell_level(SKILL_MEDITATE, CLASS_HALF_BREED, 1);
  spell_level(SKILL_BEAM, CLASS_HALF_BREED, 1);
  spell_level(SKILL_TSUIHIDAN, CLASS_HALF_BREED, 1);
  spell_level(SKILL_SHOGEKIHA, CLASS_HALF_BREED, 1);
  spell_level(SKILL_BBK, CLASS_HALF_BREED, 1);
  spell_level(SKILL_HAVOC, CLASS_HALF_BREED, 1);
  spell_level(SKILL_GALIKGUN, CLASS_HALF_BREED, 1);
  spell_level(SKILL_HONOO, CLASS_HALF_BREED, 1);
  spell_level(SKILL_BAKU, CLASS_HALF_BREED, 1);
  spell_level(SKILL_CRUSHER, CLASS_HALF_BREED, 1);
  spell_level(SKILL_ERASER, CLASS_HALF_BREED, 1);
  spell_level(SKILL_KYODAIKA, CLASS_HALF_BREED, 1);
  spell_level(SKILL_SCATTER, CLASS_HALF_BREED, 1);
  spell_level(SKILL_GENKI, CLASS_HALF_BREED, 1);
  spell_level(SKILL_FORELOCK, CLASS_HALF_BREED, 1);
  spell_level(SKILL_CHIKYUU, CLASS_HALF_BREED, 1);
  spell_level(SKILL_GENOCIDE, CLASS_HALF_BREED, 1);
  spell_level(SKILL_DEFLECT, CLASS_HALF_BREED, 1);
  spell_level(SKILL_POTENTIAL, CLASS_HALF_BREED, 1);
  spell_level(SKILL_MAJIN, CLASS_HALF_BREED, 1);
  spell_level(SKILL_MINDREAD, CLASS_HALF_BREED, 1);
  spell_level(SKILL_DUALWIELD, CLASS_HALF_BREED, 1);
}


/*
 * This is the exp given to implementors -- it must always be greater
 * than the exp required for immortality, plus at least 20,000 or so.
 */
#define EXP_MAX  9000000000

/* Function to return the exp required for each class/level */
long long level_exp(int chclass, long long level) {
	if (level > LVL_IMPL || level < 0) {
		log("SYSERR: Requesting exp for invalid level!");
		return 0;
	} else {
		return 2000 * level;
	}
}

/* 
 * KAIO titles of male characters.
 */
const char *title_male(int chclass, int level)
{
  if (level <= 0 || level > LVL_IMPL)
    return "the Man";
  if (level == LVL_IMPL)
    return "the Implementor";

  switch (chclass) {

    case CLASS_Human:
    switch (level) {
      case  1: return "the Human";
      case LVL_IMMORT: return "the Immortal Human";
      case LVL_GOD: return "the Avatar of Humans";
      case LVL_GRGOD: return "the God of Humans";
      default: return "the Human";
    }
    break;

    case CLASS_BIODROID:
    switch (level) {
      case  1: return "the Bio-android";
      case LVL_IMMORT: return "the Immortal Bio-android";
      case LVL_GOD: return "the Avatar of Bio-androids";
      case LVL_GRGOD: return "the God of Bio-androids";
      default: return "the Bio-android";
    }
    break;

    case CLASS_KONATSU:
    switch (level) {
      case  1: return "the Konatsu";
      case LVL_IMMORT: return "the Immortal Konatsu";
      case LVL_GOD: return "the Avatar of Konatsus";
      case LVL_GRGOD: return "the God of Konatsus";
      default: return "the Konatsu";
    }
    break;

  case CLASS_demon:
    switch (level) {
      case  1: return "the Demon";
      case LVL_IMMORT: return "the Immortal Demon";
      case LVL_GOD: return "the Avatar of Demons";
      case LVL_GRGOD: return "the God of Demons";
      default: return "the Demon";
    }
    break;
  case CLASS_TRUFFLE:
    switch (level) {
      case  1: return "the Truffle";
      case LVL_IMMORT: return "the Immortal Truffle";
      case LVL_GOD: return "the Avatar of Truffles";
      case LVL_GRGOD: return "the God of Truffles";
      default: return "the Truffle";
    }
    break;
  case CLASS_MUTANT:
    switch (level) {
      case  1: return "the Mutant";
      case LVL_IMMORT: return "the Immortal Mutant";
      case LVL_GOD: return "the Avatar of Mutant";
      case LVL_GRGOD: return "the God of Mutant";
      default: return "the Mutant";
    }
    break;
  case CLASS_KANASSAN:
    switch (level) {
      case  1: return "the Kanassan";
      case LVL_IMMORT: return "the Immortal Kanassan";
      case LVL_GOD: return "the Avatar of Kanassan";
      case LVL_GRGOD: return "the God of Kanassan";
      default: return "the Kanassan";
    }
    break;

    case CLASS_Namek:
    switch (level) {
      case  1: return "the Namek";
      case LVL_IMMORT: return "the Immortal Namek";
      case LVL_GOD: return "the Namek";
      case LVL_GRGOD: return "the God of Nameks";
      default: return "the Namek";
    }
    break;

    case CLASS_icer:
    switch (level) {
      case  1: return "the Icer";      
      case LVL_IMMORT: return "the Immortal Icers";
      case LVL_GOD: return "the Demi God of Icers";
      case LVL_GRGOD: return "the God of Icers";
      default: return "the Icer";
    }
    break;

    case CLASS_MAJIN:
    switch (level) {
      case  1: return "the Majin";      
      case LVL_IMMORT: return "the Immortal Majins";
      case LVL_GOD: return "the Demi God of Majins";
      case LVL_GRGOD: return "the God of Majins";
      default: return "the Majin";
    }
    break;
    
    case CLASS_ANDROID:
    switch (level) {
      case  1: return "the Android";      
      case LVL_IMMORT: return "the Immortal Android";
      case LVL_GOD: return "the Demi God of Android";
      case LVL_GRGOD: return "the God of Android";
      default: return "the Android";
    }
    break;
    
    case CLASS_KAI:
    switch (level) {
      case  1: return "the Kai";      
      case LVL_IMMORT: return "the Immortal Kai";
      case LVL_GOD: return "the Demi God of Kais";
      case LVL_GRGOD: return "the God of Kais";
      default: return "the Kai";
    }
    break;
    
    case CLASS_HALF_BREED:
    switch (level) {
      case  1: return "the Half-Breed";      
      case LVL_IMMORT: return "the Immortal Half-Breed";
      case LVL_GOD: return "the Demi God of Half-Breed";
      case LVL_GRGOD: return "the God of Half-Breed";
      default: return "the Half-Breed";
    }
    break;

    case CLASS_saiyan:
    switch(level) {
      case  1: return "the Saiyan";
      case LVL_IMMORT: return "the Immortal Saiyan";
      case LVL_GOD: return "the Saiyan";
      case LVL_GRGOD: return "the God of Saiyans";
      default: return "the Saiyan";
    }
    break;
  }

  /* KAIO title for classes which do not have titles defined */
  return "the Classless";
}


/* 
 * KAIO titles of female characters.
 */
const char *title_female(int chclass, int level)
{
  if (level <= 0 || level > LVL_IMPL)
    return "the Woman";
  if (level == LVL_IMPL)
    return "the Implementress";

  switch (chclass) {

    case CLASS_Human:
    switch (level) {
      case  1: return "the Human";
      case LVL_IMMORT: return "the Immortal Human";
      case LVL_GOD: return "the Human";
      case LVL_GRGOD: return "the Goddess of Humans";
      default: return "the Human";
    }
    break;

    case CLASS_MAJIN:
    switch (level) {
      case  1: return "the Majin";
      case LVL_IMMORT: return "the Immortal Majin";
      case LVL_GOD: return "the Human";
      case LVL_GRGOD: return "the Goddess of Majin";
      default: return "the Majin";
    }
    break;
    
    case CLASS_ANDROID:
    switch (level) {
      case  1: return "the Android";
      case LVL_IMMORT: return "the Immortal Android";
      case LVL_GOD: return "the Android";
      case LVL_GRGOD: return "the Goddess of Android";
      default: return "the Android";
    }
    break;

    case CLASS_KAI:
    switch (level) {
      case  1: return "the Kai";
      case LVL_IMMORT: return "the Immortal Kai";
      case LVL_GOD: return "the Kai";
      case LVL_GRGOD: return "the Goddess of Kai";
      default: return "the Kai";
    }
    break;
    
    case CLASS_HALF_BREED:
    switch (level) {
      case  1: return "the Half-Breed";
      case LVL_IMMORT: return "the Immortal Half-Breed";
      case LVL_GOD: return "the Android";
      case LVL_GRGOD: return "the Goddess of Half-Breeds";
      default: return "the Half=breed";
    }
    break;

    case CLASS_Namek:
    switch (level) {
      case  1: return "the Namek";
      case LVL_IMMORT: return "the Immortal Namek";
      case LVL_GOD: return "the Namek";
      case LVL_GRGOD: return "the Goddess of Nameks";
      default: return "the Namek";
    }
    break;

    case CLASS_icer:
    switch (level) {
      case  1: return "the Icer";
      case LVL_IMMORT: return "the Immortal Icer";
      case LVL_GOD: return "the Demi Goddess of Icers";
      case LVL_GRGOD: return "the Goddess of Icers";
      default: return "the icer";
    }
    break;

    case CLASS_saiyan:
    switch(level) {
      case  1: return "the Saiyan";
      case LVL_IMMORT: return "the Immortal Saiyan";
      case LVL_GOD: return "the Queen of Saiyan";
      case LVL_GRGOD: return "the Goddess of Saiyans";
      default: return "the saiyan";
    }
    break;
  }

  /* KAIO title for classes which do not have titles defined */
  return "the Classless";
}
