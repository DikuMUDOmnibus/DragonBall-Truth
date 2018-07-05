

/* ************************************************************************
*   File: spec_procs.c                                  Part of CircleMUD *
*  Usage: implementation of special procedures for mobiles/objects/rooms  *
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
#include "dg_scripts.h"

/*   external vars  */
extern struct room_data *world;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct time_info_data time_info;
extern struct spell_info_type spell_info[];
extern struct int_app_type int_app[];
extern int guild_info[][3];
extern int top_of_world;
extern struct spell_entry spells[];

/* extern functions */
void add_follower(struct char_data * ch, struct char_data * leader);
void play_slots(struct char_data *ch);
void play_craps(struct char_data *ch, struct char_data *dealer, int bet);
ACMD(do_drop);
ACMD(do_gen_door);
ACMD(do_say);

/* local functions */
void sort_spells(void);
const char *how_good(int percent);
void list_skills(struct char_data * ch);
SPECIAL(gamble_slot_machine);
SPECIAL(gamble_craps);
SPECIAL(guild);
SPECIAL(dump);
SPECIAL(mayor);
void npc_steal(struct char_data * ch, struct char_data * victim);
SPECIAL(snake);
SPECIAL(icer);
SPECIAL(Human);
SPECIAL(guild_guard);
SPECIAL(puff);
SPECIAL(fido);
SPECIAL(janitor);
SPECIAL(cityguard);
SPECIAL(pet_shops);
SPECIAL(bank);

/* ********************************************************************
*  Special procedures for mobiles                                     *
******************************************************************** */

int spell_sort_info[MAX_SKILLS+1];

void sort_spells(void)
{
  int a, b, tmp;

  /* initialize array */
  for (a = 1; a < MAX_SKILLS; a++)
    spell_sort_info[a] = a;

  /* Sort.  'a' starts at 1, not 0, to remove 'RESERVED' */
  for (a = 1; a < MAX_SKILLS - 1; a++)
    for (b = a + 1; b < MAX_SKILLS; b++)
      if (strcmp(spells[spell_sort_info[a]].name, spells[spell_sort_info[b]].name) > 0) {
	tmp = spell_sort_info[a];
	spell_sort_info[a] = spell_sort_info[b];
	spell_sort_info[b] = tmp;
      }
}


const char *how_good(int percent)
{
  if (percent < 0)
    return " error)";
  if (percent == 0)
    return " &12(&01not learned&12)&00";
  if (percent <= 10)
    return " &09(&03awful&09)&00";
  if (percent <= 20)
    return " &09(&04bad&00)&00";
  if (percent <= 40)
    return " &09(&05poor&09)&00";
  if (percent <= 55)
    return " &09(&06moderate&09)&00";
  if (percent <= 70)
    return " &09(&07good&09)&00";
  if (percent <= 80)
    return " &09(learned)&00";
  if (percent <= 85)
    return " &09(&12perfected&09)&00";

  return " &09(&12perfected&09)&00";
}

const char *prac_types[] = {
  "spell",
  "skill"
};

#define LEARNED_LEVEL	0	/* % known which is considered "learned" */
#define MAX_PER_PRAC	1	/* max percent gain in skill per practice */
#define MIN_PER_PRAC	2	/* min percent gain in skill per practice */
#define PRAC_TYPE	3	/* should it say 'spell' or 'skill'?	 */

/* actual prac_params are in class.c */
extern int prac_params[4][NUM_CLASSES];

#define LEARNED(ch) (prac_params[LEARNED_LEVEL][(int)GET_CLASS(ch)])
#define MINGAIN(ch) (prac_params[MIN_PER_PRAC][(int)GET_CLASS(ch)])
#define MAXGAIN(ch) (prac_params[MAX_PER_PRAC][(int)GET_CLASS(ch)])
#define SPLSKL(ch) (prac_types[prac_params[PRAC_TYPE][(int)GET_CLASS(ch)]])

void list_skills(struct char_data * ch)
{
  int i, sortpos;

  if (!GET_PRACTICES(ch))
    strcpy(buf, "&15You have no practice sessions remaining.\r\n");
  else
    sprintf(buf, "&15You have &09%d&15 practice session%s remaining.&00\r\n",
	    GET_PRACTICES(ch), (GET_PRACTICES(ch) == 1 ? "" : "s"));

  sprintf(buf + strlen(buf), "&15You know of the following %ss:&00\r\n", SPLSKL(ch));

  strcpy(buf2, buf);

  for (sortpos = 1; sortpos < MAX_SKILLS; sortpos++) {
    i = spell_sort_info[sortpos];
    if (strlen(buf2) >= MAX_STRING_LENGTH - 32) {
      strcat(buf2, "**OVERFLOW**\r\n");
      break;
    }
    if (GET_LEVEL(ch) >= spell_info[i].min_level[(int) GET_CLASS(ch)]) {
      sprintf(buf, "&14%-20s %s&00\r\n", spells[i].name, how_good(GET_SKILL(ch, i)));
      strcat(buf2, buf);
    }
  }

  page_string(ch->desc, buf2, 1);
}


SPECIAL(guild)
{
  int skill_num, percent;

  if (IS_NPC(ch) || !CMD_IS("practice"))
    return 0;

  skip_spaces(&argument);

  if (!*argument) {
    list_skills(ch);
    return 1;
  }
  if (GET_PRACTICES(ch) <= 0) {
    send_to_char("You do not seem to be able to practice now.\r\n", ch);
    return 1;
  }

  skill_num = find_skill_num(argument);

  if (skill_num < 1 ||
      GET_LEVEL(ch) < spell_info[skill_num].min_level[(int) GET_CLASS(ch)]) {
    sprintf(buf, "You do not know of that %s.\r\n", SPLSKL(ch));
    send_to_char(buf, ch);
    return 1;
  }
  if (GET_SKILL(ch, skill_num) >= LEARNED(ch)) {
    send_to_char("You are already learned in that area.\r\n", ch);
    return 1;
  }
  send_to_char("You train for a while...\r\n", ch);
  GET_PRACTICES(ch)--;

  percent = GET_SKILL(ch, skill_num);
  percent += MIN(MAXGAIN(ch), MAX(MINGAIN(ch), int_app[GET_INT(ch)].learn));

  SET_SKILL(ch, skill_num, MIN(LEARNED(ch), percent));

  if (GET_SKILL(ch, skill_num) >= LEARNED(ch))
    send_to_char("You are now learned in that technique.\r\n", ch);

  return 1;
}
SPECIAL(gamble_craps)
{
    int bet;
    struct char_data *dealer;
      if (!CMD_IS("rolldice"))
         return(FALSE);
    play_craps(ch, dealer, bet);
    return(TRUE);
}
SPECIAL(gamble_slot_machine)
{
      if (!CMD_IS("bet"))
         return(FALSE);
      play_slots(ch);
      return(TRUE);
}
void play_craps(struct char_data *ch, struct char_data *dealer, int bet)
{
     int  die1, die2, mark = 0, last = 0, num = 0, first = 0, firstg = 0;
     bool won = FALSE, firstime = TRUE;
     bet = 5000;

     if (GET_GOLD(ch) < bet) {
        act("$n &15says&00&07, '&15I'm sorry sir but you don't have the required 5000 zenni&00&07.'&00",
            FALSE, dealer, 0, ch, TO_VICT);
        return;
     } else if (bet > 10000) {
        act("$n says, 'I'm sorry sir but the limit at this table is 10000 gold pieces.'",
            FALSE, dealer, 0, ch, TO_VICT);
        return;
     } else {
        GET_GOLD(ch) -= bet;
        act("&14$N&15 places $S bet on the table.&00", FALSE, 0, 0, ch, TO_NOTVICT);
        act("&15You place your bet on the table.&00", FALSE, ch, 0, 0, TO_CHAR);
     }

     act("&14$n &15hands &10$N&15 the dice and says, 'roll 'em'&00", FALSE, dealer, 0, ch, TO_NOTVICT);
     act("&14$n&15 hands you the dice and says, 'roll 'em'&00", FALSE, dealer, 0, ch, TO_VICT);

     while (won != TRUE) {
       die1 = number(1, 6);
       die2 = number(1, 6);
      
       if (number(1, 20) >= 16 && firstime) {
        die1 = 6;
        die2 = 6;
       }
       else if (number(1, 20) >= 15 && firstime) {
        die1 = 1;
        die2 = 2;
       }
       mark = die1 + die2;
       sprintf(buf, "&14$n &15says&00&07, '&14$N &15rolls &11%d &15and &11%d&15, totalling &12%d&15.&00",
              die1, die2, mark);
       act(buf, FALSE, dealer, 0, ch, TO_ROOM);
       if (first == 0) {
        firstg = mark;
        first = firstg;
       }
       if ((mark == 7  || mark == 11) && firstime) {
          /* win on first roll of the dice! 3x bet */
          act("&14$n &15says&00&07, '&15Congratulations &14$N&15, you win!&00&07'&00", FALSE, dealer, 0, ch, TO_ROOM);
          act("$n &15hands &14$N&15 a stack of zenni.&00", FALSE, dealer, 0, ch, TO_NOTVICT);
          sprintf(buf, "$n &15hands you &11%d&15 zenni.&00", (bet*6));
          act(buf, FALSE, dealer, 0, ch, TO_VICT);
          GET_GOLD(ch) += (bet*6);
          WAIT_STATE(ch, PULSE_VIOLENCE * 1);
          won = TRUE;
       } else if (mark == 3 || mark == 12 && firstime) {
          /* player loses on first roll */
          act("$n&15 says&00&07, '&15Sorry &14$N&15, you lose.&00&07'&15", FALSE, dealer, 0, ch, TO_ROOM);
          act("$n&15 takes &10$N's &15bet from the table.&00", FALSE, dealer, 0, ch, TO_NOTVICT);
          act("$n &15takes your bet from the table.&00", FALSE, dealer, 0, ch, TO_VICT);
          won = TRUE;
          WAIT_STATE(ch, PULSE_VIOLENCE * 1);
       } else if ((mark == first) && !firstime) {
          /* player makes $s mark! 2x bet */
          act("$n &15says&00&07, '&15Congratulations &14$N&15, you win!&00&07'&15", FALSE, dealer, 0, ch, TO_ROOM);
          act("$n &15hands &14$N&15 some gold pieces.&00", FALSE, dealer, 0, ch, TO_NOTVICT);
          sprintf(buf, "$n &15hands you &11%d &15zenni.&00", (bet*4));
          act(buf, FALSE, dealer, 0, ch, TO_VICT);
          GET_GOLD(ch) += (bet*4);
          won = TRUE;
          WAIT_STATE(ch, PULSE_VIOLENCE * 1);
       } else if ((mark == 7 || mark == 11) && !firstime) {
          act("$n&15 says&00&07, '&15Sorry &14$N&15, you lose.&00&07'&15", FALSE, dealer, 0, ch, TO_ROOM);
          act("$n&15 takes &10$N's &15bet from the table.&00", FALSE, dealer, 0, ch, TO_NOTVICT);
          act("$n &15takes your bet from the table.&00", FALSE, dealer, 0, ch, TO_VICT);
          won = TRUE;
          WAIT_STATE(ch, PULSE_VIOLENCE * 1);
       } else if (num <= 9) {
          sprintf(buf, "$n &15says&00&07, '&14$N's &15mark is &11%d&15.  Roll 'em again $N!&00&07'&00",
                  mark);
          act(buf, FALSE, dealer, 0, ch, TO_ROOM);
          firstime = FALSE;
          last = mark;
          num += 1;
          won = FALSE;
       }
        else {
          act("$n&15 says&00&07, '&15Sorry &14$N&15, too many rolls, you lose.&00&07'&15", FALSE, dealer, 0, ch, TO_ROOM);
          act("$n&15 takes &10$N's &15bet from the table.&00", FALSE, dealer, 0, ch, TO_NOTVICT);
          act("$n &15takes your bet from the table.&00", FALSE, dealer, 0, ch, TO_VICT);
          won = TRUE;
          WAIT_STATE(ch, PULSE_VIOLENCE * 1);
        }
     }
     return;
}
void play_slots(struct char_data *ch)
{
      int num1, num2, num3, win = 0;

      char *slot_msg[] = {
      "*YOU SHOULDN'T SEE THIS*",
      "&167 &09S&00&01t&00&03a&11r &11D&00&03r&00&01a&09g&11o&00&03n&00&01b&09a&11l&00&03l&00", /* 1 */
      "&166 &09S&00&01t&00&03a&11r &11D&00&03r&00&01a&09g&11o&00&03n&00&01b&09a&11l&00&03l&00",
      "&165 &09S&00&01t&00&03a&11r &11D&00&03r&00&01a&09g&11o&00&03n&00&01b&09a&11l&00&03l&00",
      "&164 &09S&00&01t&00&03a&11r &11D&00&03r&00&01a&09g&11o&00&03n&00&01b&09a&11l&00&03l&00",
      "&163 &09S&00&01t&00&03a&11r &11D&00&03r&00&01a&09g&11o&00&03n&00&01b&09a&11l&00&03l&00", /* 5 */
      "&162 &09S&00&01t&00&03a&11r &11D&00&03r&00&01a&09g&11o&00&03n&00&01b&09a&11l&00&03l&00",
      "&161 &09S&00&01t&00&03a&11r &11D&00&03r&00&01a&09g&11o&00&03n&00&01b&09a&11l&00&03l&00",
      "&00&02S&10a&11i&00&02b&10a&00",
      "&11L&00&03a&00&06u&14gh&00&06i&00&03n&11g &09R&00&01o&00&02s&10h&16i&00",
      "&00&03S&00&06e&00&03n&00&06z&00&03u &16bean&00",
      "&00&03N&11i&00&03m&16b&11u&00&03s&00",
      "&00&03B&16u&15b&00&01b&00&03l&16e&15s&00",
      "&12P&00&04u&16a&00&04r&00",
      "&13O&00&05o&16lo&00&05n&13g&00",              /* 10 */
      "&16H&15e&00&07r&00&03cu&16l&15e&00",
      };

      if (GET_GOLD(ch) < 20000) {
         send_to_char("You need 20000 zenni to use the slot machine!\r\n", ch);
         return;
      }
      else
         GET_GOLD(ch) -= 20000;

      act("&14$n &15puts 20000 zenni into the slot machine&00...", FALSE, ch, 0, 0, TO_ROOM);
      send_to_char("&15You put 20000 zenni into the slot machine&00...\r\n", ch);
      act("&15$e pulls on the lever of the slot machine.&00", FALSE, ch, 0, 0, TO_ROOM);
      send_to_char("&15You pull on the lever of the slot machine.&00\r\n", ch);

     /* very simple roll 3 random numbers from 1 to 10 */
      num1 = number(1, 15);
      num2 = number(1, 15);
      num3 = number(1, 15);
      if (num1 <= 14 && (num2 == 15 || num3 == 15)) {
       if (num2 == 15) {
       num2 = number(1, 14);
       }
       if (num3 == 15) {
       num3 = number(1, 14);
       }
      }
      if (num2 <= 14 && (num1 == 15 || num3 == 15)) {
       if (num1 == 15) {
        num1 = number(1, 14);
       }
       if (num3 == 15) {
        num3 = number(1, 14);
       }
      }
      if (num3 <= 14 && (num1 == 15 || num2 == 15)) {
       if (num2 == 15) {
        num1 = number(1, 14);
       }
       if (num3 == 15) {
        num2 = number(1, 14);
       }
      }
      if ((num1 == 15) && num1 == num2 && num1 == num3) {
       send_to_char("&15Hercule's voice says&00&07, '&15Can't beat the champ!&00&07'&00\r\n", ch);
       win -= 50000;
      }
      else if (num1 == num2 && num2 == num3) {
      /* all 3 are equal! Woohoo! */
         if (num1 == 1)
            win += 5000000;
         else if (num1 == 2)
            win += 2000000;
         else if (num1 == 3)
            win += 1800000;
         else if (num1 == 4)
            win += 1700000;
         else if (num1 == 5)
            win += 1600000;
         else if (num1 == 6)
            win += 1500000;
         else if (num1 == 7)
            win += 1450000;
         else if (num1 == 8)
            win += 1350000;
         else if (num1 == 9)
            win += 1325000;
         else if (num1 == 10)
            win += 1300000;
         else if (num1 == 11)
            win += 1275000;
         else if (num1 == 12)
            win += 1250000;
         else if (num1 == 13)
            win += 1225000;
         else if (num1 == 14)
            win += 1200000;
      }

      sprintf(buf, "&15You got &14%s&15, &14%s&15, and &14%s&15, ", slot_msg[num1], slot_msg[num2], slot_msg[num3]);
      if (win > 1) {
         sprintf(buf, "%syou win &11%d &15zenni&00!\r\n", buf, win);
         win += 25000;
      }
      else if (win == 1) {
         sprintf(buf, "%syou win &111&15 zenni!&00\r\n", buf);
      }
      else {
         sprintf(buf, "%syou lose.&00\r\n", buf);
      }
      send_to_char(buf, ch);
      GET_GOLD(ch) += win;
      WAIT_STATE(ch, PULSE_VIOLENCE * 1);
      return;
}
SPECIAL(dump)
{
  struct obj_data *k;
  int value = 0;

  for (k = world[ch->in_room].contents; k; k = world[ch->in_room].contents) {
    act("$p vanishes in a puff of smoke!", FALSE, 0, k, 0, TO_ROOM);
    extract_obj(k);
  }

  if (!CMD_IS("drop"))
    return 0;

  do_drop(ch, argument, cmd, 0);

  for (k = world[ch->in_room].contents; k; k = world[ch->in_room].contents) {
    act("$p vanishes in a puff of smoke!", FALSE, 0, k, 0, TO_ROOM);
    value += MAX(1, MIN(50, GET_OBJ_COST(k) / 10));
    extract_obj(k);
  }

  if (value) {
    act("You are awarded for outstanding performance.", FALSE, ch, 0, 0, TO_CHAR);
    act("$n has been awarded for being a good citizen.", TRUE, ch, 0, 0, TO_ROOM);

    if (GET_LEVEL(ch) < 3)
      gain_exp(ch, value);
    else
      GET_GOLD(ch) += value;
  }
  return 1;
}


SPECIAL(mayor)
{
  const char open_path[] =
	"W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";
  const char close_path[] =
	"W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";

  static const char *path;
  static int index;
  static bool move = FALSE;

  if (!move) {
    if (time_info.hours == 6) {
      move = TRUE;
      path = open_path;
      index = 0;
    } else if (time_info.hours == 20) {
      move = TRUE;
      path = close_path;
      index = 0;
    }
  }
  if (cmd || !move || (GET_POS(ch) < POS_SLEEPING) ||
      (GET_POS(ch) == POS_FIGHTING))
    return FALSE;

  switch (path[index]) {
  case '0':
  case '1':
  case '2':
  case '3':
    perform_move(ch, path[index] - '0', 1);
    break;

  case 'W':
    GET_POS(ch) = POS_STANDING;
    act("$n awakens and groans loudly.", FALSE, ch, 0, 0, TO_ROOM);
    break;

  case 'S':
    GET_POS(ch) = POS_SLEEPING;
    act("$n lies down and instantly falls asleep.", FALSE, ch, 0, 0, TO_ROOM);
    break;

  case 'a':
    act("$n says 'Hello Honey!'", FALSE, ch, 0, 0, TO_ROOM);
    act("$n smirks.", FALSE, ch, 0, 0, TO_ROOM);
    break;

  case 'b':
    act("$n says 'What a view!  I must get something done about that dump!'",
	FALSE, ch, 0, 0, TO_ROOM);
    break;

  case 'c':
    act("$n says 'Vandals!  Youngsters nowadays have no respect for anything!'",
	FALSE, ch, 0, 0, TO_ROOM);
    break;

  case 'd':
    act("$n says 'Good day, citizens!'", FALSE, ch, 0, 0, TO_ROOM);
    break;

  case 'e':
    act("$n says 'I hereby declare the bazaar open!'", FALSE, ch, 0, 0, TO_ROOM);
    break;

  case 'E':
    act("$n says 'I hereby declare Midgaard closed!'", FALSE, ch, 0, 0, TO_ROOM);
    break;

  case 'O':
    do_gen_door(ch, "gate", 0, SCMD_UNLOCK);
    do_gen_door(ch, "gate", 0, SCMD_OPEN);
    break;

  case 'C':
    do_gen_door(ch, "gate", 0, SCMD_CLOSE);
    do_gen_door(ch, "gate", 0, SCMD_LOCK);
    break;

  case '.':
    move = FALSE;
    break;

  }

  index++;
  return FALSE;
}


/* ********************************************************************
*  General special procedures for mobiles                             *
******************************************************************** */


void npc_steal(struct char_data * ch, struct char_data * victim)
{
  int gold;

  if (IS_NPC(victim))
    return;
  if (GET_LEVEL(victim) >= LVL_IMMORT)
    return;

  if (AWAKE(victim) && (number(0, GET_LEVEL(ch)) == 0)) {
    act("You discover that $n has $s hands in your wallet.", FALSE, ch, 0, victim, TO_VICT);
    act("$n tries to steal gold from $N.", TRUE, ch, 0, victim, TO_NOTVICT);
  } else {
    /* Steal some gold coins */
    gold = (int) ((GET_GOLD(victim) * number(1, 10)) / 100);
    if (gold > 0) {
      GET_GOLD(ch) += gold;
      GET_GOLD(victim) -= gold;
    }
  }
}


SPECIAL(snake)
{
  if (cmd)
    return FALSE;

  if (GET_POS(ch) != POS_FIGHTING)
    return FALSE;

  if (FIGHTING(ch) && (FIGHTING(ch)->in_room == ch->in_room) &&
      (number(0, 42 - GET_LEVEL(ch)) == 0)) {
    act("$n bites $N!", 1, ch, 0, FIGHTING(ch), TO_NOTVICT);
    act("$n bites you!", 1, ch, 0, FIGHTING(ch), TO_VICT);
    call_magic(ch, FIGHTING(ch), 0, SPELL_POISON, GET_LEVEL(ch), focus_SPELL);
    return TRUE;
  }
  return FALSE;
}


SPECIAL(icer)
{
  struct char_data *cons;

  if (cmd)
    return FALSE;

  if (GET_POS(ch) != POS_STANDING)
    return FALSE;

  for (cons = world[ch->in_room].people; cons; cons = cons->next_in_room)
    if (!IS_NPC(cons) && (GET_LEVEL(cons) < LVL_IMMORT) && (!number(0, 4))) {
      npc_steal(ch, cons);
      return TRUE;
    }
  return FALSE;
}


SPECIAL(Human)
{
  struct char_data *vict;

  if (cmd || GET_POS(ch) != POS_FIGHTING)
    return FALSE;

  /* pseudo-randomly choose someone in the room who is fighting me */
  for (vict = world[ch->in_room].people; vict; vict = vict->next_in_room)
    if (FIGHTING(vict) == ch && !number(0, 4))
      break;

  /* if I didn't pick any of those, then just slam the guy I'm fighting */
  if (vict == NULL && IN_ROOM(FIGHTING(ch)) == IN_ROOM(ch))
    vict = FIGHTING(ch);

  /* Hm...didn't pick anyone...I'll wait a round. */
  if (vict == NULL)
    return TRUE;

  if ((GET_LEVEL(ch) > 13) && (number(0, 10) == 0))
    focus_spell(ch, vict, NULL, SPELL_YOIKOMINMINKEN);

  if ((GET_LEVEL(ch) > 7) && (number(0, 8) == 0))
    focus_spell(ch, vict, NULL, SPELL_SOLAR_FLARE);

  if (number(0, 4))
    return TRUE;

  switch (GET_LEVEL(ch)) {
  case 4:
  case 5:
  case 6:
  case 8:
  case 9:
  case 10:
  default:
    focus_spell(ch, vict, NULL, SPELL_SAIBA);
    break;
  }
  return TRUE;

}


/* ********************************************************************
*  Special procedures for mobiles                                      *
******************************************************************** */

SPECIAL(guild_guard)
{
  int i;
  struct char_data *guard = (struct char_data *) me;
  const char *buf = "The guard humiliates you, and blocks your way.\r\n";
  const char *buf2 = "The guard humiliates $n, and blocks $s way.";

  if (!IS_MOVE(cmd) || AFF_FLAGGED(guard, AFF_SOLAR_FLARE))
    return FALSE;

  if (GET_LEVEL(ch) >= LVL_IMMORT)
    return FALSE;

  for (i = 0; guild_info[i][0] != -1; i++) {
    if ((IS_NPC(ch) || GET_CLASS(ch) != guild_info[i][0]) &&
	GET_ROOM_VNUM(IN_ROOM(ch)) == guild_info[i][1] &&
	cmd == guild_info[i][2]) {
      send_to_char(buf, ch);
      act(buf2, FALSE, ch, 0, 0, TO_ROOM);
      return TRUE;
    }
  }

  return FALSE;
}



SPECIAL(puff)
{
  if (cmd)
    return (0);

  switch (number(0, 60)) {
  case 0:
    do_say(ch, "My god!  I'm stoned off my ass!", 0, 0);
    return (1);
  case 1:
    do_say(ch, "How'd all that weed get up here?", 0, 0);
    return (1);
  case 2:
    do_say(ch, "I'm a very stoned Makyo.", 0, 0);
    return (1);
  case 3:
    do_say(ch, "I've got a peaceful, easy feeling..good weed.", 0, 0);
    return (1);
  default:
    return (0);
  }
}


SPECIAL(fido)
{

  struct obj_data *i, *temp, *next_obj;

  if (cmd || !AWAKE(ch))
    return (FALSE);

  for (i = world[ch->in_room].contents; i; i = i->next_content) {
    if (IS_CORPSE(i)) {
      act("$n savagely devours a corpse.", FALSE, ch, 0, 0, TO_ROOM);
      for (temp = i->contains; temp; temp = next_obj) {
	next_obj = temp->next_content;
	obj_from_obj(temp);
	obj_to_room(temp, ch->in_room);
      }
      extract_obj(i);
      return (TRUE);
    }
  }
  return (FALSE);
}



SPECIAL(janitor)
{
  struct obj_data *i;

  if (cmd || !AWAKE(ch))
    return (FALSE);

  for (i = world[ch->in_room].contents; i; i = i->next_content) {
    if (!CAN_WEAR(i, ITEM_WEAR_TAKE))
      continue;
    if (GET_OBJ_TYPE(i) != ITEM_DRINKCON && GET_OBJ_COST(i) >= 15)
      continue;
    act("$n picks up some trash.", FALSE, ch, 0, 0, TO_ROOM);
    obj_from_room(i);
    obj_to_char(i, ch);
    return TRUE;
  }

  return FALSE;
}


SPECIAL(cityguard)
{
  struct char_data *tch, *evil;
  int max_evil;

  if (cmd || !AWAKE(ch) || FIGHTING(ch))
    return FALSE;

  max_evil = 1000;
  evil = 0;

  for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
    if (!IS_NPC(tch) && CAN_SEE(ch, tch) && PLR_FLAGGED(tch, PLR_KILLER)) {
      act("$n screams 'HEY!!!  You're one of those PLAYER KILLERS!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
      hit(ch, tch, TYPE_UNDEFINED);
      return (TRUE);
    }
  }

  for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
    if (!IS_NPC(tch) && CAN_SEE(ch, tch) && PLR_FLAGGED(tch, PLR_icer)){
      act("$n screams 'HEY!!!  You're one of those PLAYER THIEVES!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
      hit(ch, tch, TYPE_UNDEFINED);
      return (TRUE);
    }
  }

  for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
    if (CAN_SEE(ch, tch) && FIGHTING(tch)) {
      if ((GET_ALIGNMENT(tch) < max_evil) &&
	  (IS_NPC(tch) || IS_NPC(FIGHTING(tch)))) {
	max_evil = GET_ALIGNMENT(tch);
	evil = tch;
      }
    }
  }

  if (evil && (GET_ALIGNMENT(FIGHTING(evil)) >= 0)) {
    act("$n screams 'PROTECT THE INNOCENT!  BANZAI!  CHARGE!  ARARARAGGGHH!'", FALSE, ch, 0, 0, TO_ROOM);
    hit(ch, evil, TYPE_UNDEFINED);
    return (TRUE);
  }
  return (FALSE);
}


#define PET_PRICE(pet) (GET_LEVEL(pet) * 300)

SPECIAL(pet_shops)
{
  char buf[MAX_STRING_LENGTH], pet_name[256];
  int pet_room;
  struct char_data *pet;

  pet_room = ch->in_room + 1;

   if (CMD_IS("kljgl")) {
    send_to_char("Available pets are:\r\n", ch);
    for (pet = world[pet_room].people; pet; pet = pet->next_in_room) {
      sprintf(buf, "%9Ld - %s\r\n", PET_PRICE(pet), GET_NAME(pet));
      send_to_char(buf, ch);
    }
    return (TRUE);
  } else if (CMD_IS("kjglkh")) {

    argument = one_argument(argument, buf);
    argument = one_argument(argument, pet_name);
   
    if (!(pet = get_char_room(buf, pet_room))) {
      send_to_char("There is no such pet!\r\n", ch);
      return (TRUE);
    }
    if (GET_GOLD(ch) < PET_PRICE(pet)) {
      send_to_char("You don't have enough gold!\r\n", ch);
      return (TRUE);
    }
    GET_GOLD(ch) -= PET_PRICE(pet);

    pet = read_mobile(GET_MOB_RNUM(pet), REAL);
    GET_EXP(pet) = 0;
    SET_BIT(AFF_FLAGS(pet), AFF_MAJIN);

    if (*pet_name) {
      sprintf(buf, "%s %s", pet->player.name, pet_name);
      /* free(pet->player.name); don't free the prototype! */
      pet->player.name = str_dup(buf);

      sprintf(buf, "%sA small sign on a chain around the neck says 'My name is %s'\r\n",
	      pet->player.description, pet_name);
      /* free(pet->player.description); don't free the prototype! */
      pet->player.description = str_dup(buf);
    }
    char_to_room(pet, ch->in_room);
    add_follower(pet, ch);
    load_mtrigger(pet);

    /* Be certain that pets can't get/carry/use/wield/wear items */
    IS_CARRYING_W(pet) = 1000;
    IS_CARRYING_N(pet) = 100;

    send_to_char("May you enjoy your pet.\r\n", ch);
    act("$n buys $N as a pet.", FALSE, ch, 0, pet, TO_ROOM);

    return 1;
  }
  /* All commands except list and buy */
  return 0;
}




/* ********************************************************************
*  Special procedures for objects                                     *
******************************************************************** */
SPECIAL(bank)
{
  int amount;

  if (CMD_IS("balance")) {
    if (GET_BANK_GOLD(ch) > 0)
      sprintf(buf, "Your current balance is %d coins.\r\n",
	      GET_BANK_GOLD(ch));
    else
      sprintf(buf, "You currently have no money deposited.\r\n");
    send_to_char(buf, ch);
    return 1;
  } else if (CMD_IS("depositall")) {
    if (GET_BANK_GOLD(ch) >= GET_LEVEL(ch) * 1500) {
     send_to_char("Your account is already full, get some more experience before you will be trusted with a larger account.", ch);
     return 1;
    }
    else if (GET_BANK_GOLD(ch) + GET_GOLD(ch) >= GET_LEVEL(ch) * 1500) {
     send_to_char("That would overfill your account, try again.\r\n", ch);
     return 1;
    }
    else if (GET_GOLD(ch) > GET_LEVEL(ch) * 1500) {
     GET_BANK_GOLD(ch) += GET_GOLD(ch) - (GET_LEVEL(ch) * 1500);
    }
    else if (GET_GOLD(ch) < GET_LEVEL(ch) * 1500) {
     GET_BANK_GOLD(ch) += GET_GOLD(ch);
    }
    sprintf(buf, "You deposit %d coins.\r\n", GET_GOLD(ch));
    GET_GOLD(ch) -= GET_GOLD(ch);
    send_to_char(buf, ch);
    act("$n makes a bank transaction.", TRUE, ch, 0, FALSE, TO_ROOM);
    return 1;
  } else if (CMD_IS("withdrawall")) {
    if (GET_BANK_GOLD(ch) < 10) {
     send_to_char("You need at least 10 zenni to make a withdraw.", ch);
     return;
    }
    if (GET_BANK_GOLD(ch) <= 4999) {
     GET_GOLD(ch) += GET_BANK_GOLD(ch);
     sprintf(buf, "You withdraw %d coins. You lose 10 in withdraw fees.\r\n", GET_BANK_GOLD(ch));
     GET_BANK_GOLD(ch) -= GET_BANK_GOLD(ch);
     GET_GOLD(ch) -= 10;
     send_to_char(buf, ch);
     act("$n makes a bank transaction.", TRUE, ch, 0, FALSE, TO_ROOM);
     return 1;
    }
    else {
    GET_GOLD(ch) += GET_BANK_GOLD(ch);
    sprintf(buf, "You withdraw %d coins. You lose %d in withdraw fees.\r\n", GET_BANK_GOLD(ch), GET_BANK_GOLD(ch) / 500);
    GET_BANK_GOLD(ch) -= GET_BANK_GOLD(ch);
    GET_GOLD(ch) -= GET_GOLD(ch) / 500;
    send_to_char(buf, ch);
    act("$n makes a bank transaction.", TRUE, ch, 0, FALSE, TO_ROOM);
    return 1;
    }
  } else if (CMD_IS("deposit")) {
    if ((amount = atoi(argument)) <= 0) {
      send_to_char("How much do you want to deposit?\r\n", ch);
      return 1;
    }
    if (GET_GOLD(ch) < amount) {
      send_to_char("You don't have that many coins!\r\n", ch);
      return 1;
    }
    if (GET_LEVEL(ch) * 1500 < amount) {
     send_to_char("Sorry, you do not have enough experience to hold an account that large.\r\n", ch);
     return 1;
    }
    if (GET_BANK_GOLD(ch) >= GET_LEVEL(ch) * 1500) {
     send_to_char("Sorry, you do not have enough experience to add anymore to your account. It is full.", ch);
     return 1;     
    }
    if (GET_BANK_GOLD(ch) + amount > GET_LEVEL(ch) * 1500 && amount > GET_LEVEL(ch) * 1500) {
     amount -= (GET_LEVEL(ch) * 1500);
     GET_GOLD(ch) -= amount;
     GET_BANK_GOLD(ch) += amount;
     sprintf(buf, "You deposit %d coins, and your account is now full.\r\n", amount);
     send_to_char(buf, ch);
     act("$n makes a bank transaction.", TRUE, ch, 0, FALSE, TO_ROOM);
     return 1;
    }
    if (GET_BANK_GOLD(ch) + amount > GET_LEVEL(ch) * 1500 && amount < GET_LEVEL(ch) * 1500) {
     amount -= ((GET_LEVEL(ch) * 1500) - GET_BANK_GOLD(ch));
     GET_GOLD(ch) -= amount;
     GET_BANK_GOLD(ch) += amount;
     sprintf(buf, "You deposit %d coins, and your account is now full.\r\n", amount);
     send_to_char(buf, ch);
     act("$n makes a bank transaction.", TRUE, ch, 0, FALSE, TO_ROOM);
     return 1;
    }
    else {
    GET_GOLD(ch) -= amount;
    GET_BANK_GOLD(ch) += amount;
    sprintf(buf, "You deposit %d coins.\r\n", amount);
    send_to_char(buf, ch);
    act("$n makes a bank transaction.", TRUE, ch, 0, FALSE, TO_ROOM);
    return 1;
    }
  } else if (CMD_IS("withdraw")) {
    if ((amount = atoi(argument)) <= 0) {
      send_to_char("How much do you want to withdraw?\r\n", ch);
      return 1;
    }
    if (GET_BANK_GOLD(ch) < amount) {
      send_to_char("You don't have that many coins deposited!\r\n", ch);
      return 1;
    }
    if (GET_BANK_GOLD(ch) < 100) {
     send_to_char("You need at least 100 zenni to make a withdraw.", ch);
     return;
    }
    if (GET_BANK_GOLD(ch) <= 4999) {
     GET_GOLD(ch) += amount - 10;
     sprintf(buf, "You withdraw %d coins. You lose 10 in withdraw fees.\r\n", GET_BANK_GOLD(ch));
     GET_BANK_GOLD(ch) -= amount;
     send_to_char(buf, ch);
     act("$n makes a bank transaction.", TRUE, ch, 0, FALSE, TO_ROOM);
     return 1;
    }
    else {
    GET_GOLD(ch) += amount - (amount / 500);
    GET_BANK_GOLD(ch) -= amount;
    sprintf(buf, "You withdraw %d coins. You pay %d in withdraw fees.\r\n", amount, amount / 500);
    send_to_char(buf, ch);
    act("$n makes a bank transaction.", TRUE, ch, 0, FALSE, TO_ROOM);
    return 1;
   }
  } else
    return 0;
}

SPECIAL(pop_dispenser)
{
 struct obj_data *obj = me, *drink;
 int give_coke = 1206; /* Vnum of the can of coke */
 if (CMD_IS("list"))
     {
     send_to_char("To buy a coke, type 'buy coke'.\r\n", ch);
     return (TRUE);
     }
 else if (CMD_IS("buy")) {
          if (GET_GOLD(ch) < 25) {
          send_to_char("You don't have enough gold!\r\n", ch);
          return (TRUE);
          } else {
            drink = read_object(give_coke, VIRTUAL);
            obj_to_char(drink, ch);
            send_to_char("You insert your money into the machine\r\n",ch);
            GET_GOLD(ch) -= 25; /* coke costs 25 gold */
            act("$n gets a pop can from $p.", FALSE, ch, obj, 0, TO_ROOM);
            send_to_char("You get a pop can from the machine.\r\n",ch);
          }
          return 1;
 }
 return 0;
}



