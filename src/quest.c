
/* *************************************************************************
*    File: quest.c					Part of CircleMUD  *
* Purpose: To provide special quest-related code.                          *
*                                                                          *
* Morgaelin - quest.c						           *
* Copyright (C) 1997 MS                					   *
************************************************************************* */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "interpreter.h"
#include "db.h"
#include "comm.h"
#include "screen.h"
#include "olc.h"
#include "quest.h"

extern struct aq_data *aquest_table;
extern struct index_data *mob_index;
extern int top_of_aquestt;
extern struct index_data *obj_index;
extern struct room_data *world;
extern long asciiflag_conv(char *flag);

const char *quest_types[] = {
  "Object",
  "Room",
  "Find mob",
  "Kill mob",
  "Save mob",
  "Return object",
  "\n"
};

/* Autoquest flags */
char *aq_flags[] = {
  "REPEATABLE",
  "\n"
};

int real_quest(int vnum)
{
  int rnum;

  for (rnum=0; rnum <= top_of_aquestt; rnum++)
  {
	 if (rnum <= top_of_aquestt)
		if (aquest_table[rnum].virtual==vnum) break;
  }

  if (rnum>top_of_aquestt) rnum = -1;
  return (rnum);
}

int is_complete(struct char_data *ch, int vnum)
{
  int i;

  for (i=0; i <= ch->num_completed_quests; i++)
	 if (ch->completed_quests[i] == vnum)
		return TRUE;

  return FALSE;
}

int find_quest_by_qmnum(int qm, int num)
{
  int i;
  int found=0;

  for (i=0; i <= top_of_aquestt; i++)
	 if (qm == aquest_table[i].mob_vnum) {
		found++;
		if (found == num)
		  return (aquest_table[i].virtual);
	 }

  return -1;
}

/* PARSE_QUEST */
void parse_quest(FILE * quest_f, int nr)
{
  static char line[256];
  static int i = 0, j;
  int retval = 0, t[5];
  char f1[128];

  aquest_table[i].virtual = nr;
  aquest_table[i].mob_vnum = -1;
  aquest_table[i].short_desc = NULL;
  aquest_table[i].desc = NULL;
  aquest_table[i].info = NULL;
  aquest_table[i].ending = NULL;
  aquest_table[i].flags = 0;
  aquest_table[i].type = -1;
  aquest_table[i].target = -1;
  aquest_table[i].exp = 0;
  for (j=0; j < 4; j++)
	 aquest_table[i].value[j] = 0;
  aquest_table[i].next_quest = -1;

  /* begin to parse the data */
  aquest_table[i].short_desc = fread_string(quest_f, buf2);
  aquest_table[i].desc = fread_string(quest_f, buf2);
  aquest_table[i].info = fread_string(quest_f, buf2);
  aquest_table[i].ending = fread_string(quest_f, buf2);

  if (!get_line(quest_f, line) ||
		(retval = sscanf(line, " %d %d %s %d %d %d", t, t+1, f1, t+2, t+3, t + 4)) != 6) {
		  fprintf(stderr, "Format error in numeric line (expected 6, got %d), %s\n", retval, buf2);
		  exit(1);
  }
  aquest_table[i].type = t[0];
  aquest_table[i].mob_vnum = t[1];
  aquest_table[i].flags = asciiflag_conv(f1);
  aquest_table[i].target = t[2];
  aquest_table[i].exp = t[3];
  aquest_table[i].next_quest = t[4];

  if (!get_line(quest_f, line) ||
		(retval = sscanf(line, " %d %d %d %d", t, t+1, t+2, t+3)) != 4) {
		  fprintf(stderr, "Format error in numeric line (expected 4, got %d), %s\n", retval, buf2);
		  exit(1);
  }
  aquest_table[i].value[0] = t[0];
  aquest_table[i].value[1] = t[1];
  aquest_table[i].value[2] = t[2];
  aquest_table[i].value[3] = t[3];

  for (;;) {
	 if (!get_line(quest_f, line)) {
		fprintf(stderr, "Format error in %s\n", buf2);
		exit(1);
	 }
	 switch(*line) {
	 case 'S':
		top_of_aquestt = i++;
		return;
		break;
	 }
  }

} /* parse_quest */


void list_quests(struct char_data *ch, int questmaster)
{
  int i;
  int number = 1;

  sprintf(buf, "The following quests are available:\r\n"
			 "Num  Description\r\n"
 "---  ------------------------------------------------------------------------\r\n");
  for (i = 0; i <= top_of_aquestt; i++)
	 if (questmaster == aquest_table[i].mob_vnum) {
		sprintf(buf, "%s%3d  %s %s\r\n", buf, number, aquest_table[i].desc,
			is_complete(ch, aquest_table[i].virtual) ? "(completed)" : "");
		number++;
	 }
  send_to_char(buf, ch);
}

#ifdef 0
/* Add to int* completed quest vnum */
void add_completed_quest_u(struct char_file_u *player, int num)
{
  int i;
  sh_int *temp;

  CREATE(temp, sh_int, player->num_completed_quests +1);
  for (i=0; i <= player->num_completed_quests; i++)
	 temp[i] = player->completed_quests[i];

  player->num_completed_quests++;
  temp[player->num_completed_quests] = num;

  if (player->completed_quests)
	 free(player->completed_quests);
  player->completed_quests = temp;
}
#endif

void add_completed_quest(struct char_data *player, int num)
{
  sh_int *temp;
  int i;

  CREATE(temp, sh_int, player->num_completed_quests +1);
  for (i=0; i <= player->num_completed_quests; i++)
	 temp[i] = player->completed_quests[i];

  player->num_completed_quests++;
  temp[player->num_completed_quests] = num;

  if (player->completed_quests)
	 free(player->completed_quests);
  player->completed_quests = temp;
}

/* Generic reward character, cleanup stuff fn */
void generic_complete_quest(struct char_data *ch)
{

  GET_EXP(ch) += aquest_table[real_quest((int)GET_QUEST(ch))].exp;
  send_to_char(aquest_table[real_quest((int)GET_QUEST(ch))].ending, ch);
  if (!IS_SET(aquest_table[real_quest((int)GET_QUEST(ch))].flags, AQ_REPEATABLE))
	 add_completed_quest(ch, GET_QUEST(ch));
  if ((aquest_table[real_quest((int)GET_QUEST(ch))].next_quest >= 0) &&
		(aquest_table[real_quest((int)GET_QUEST(ch))].next_quest != GET_QUEST(ch)) &&
		!is_complete(ch, aquest_table[real_quest((int)GET_QUEST(ch))].next_quest)) {
	 GET_QUEST(ch) = aquest_table[real_quest((int)GET_QUEST(ch))].next_quest;
	 send_to_char(aquest_table[real_quest((int)GET_QUEST(ch))].info, ch);
  } else
	 GET_QUEST(ch) = -1;

  save_char(ch, ch->in_room);
}

void autoquest_trigger_check(struct char_data *ch, struct char_data *vict,
						 struct obj_data *object, int type)
{
  struct char_data *i;
  int rnum, found;

  if (IS_NPC(ch))
	 return;

  if (GET_QUEST(ch) < 0)  /* No current quest, skip this */
	 return;

  if (GET_QUEST_TYPE(ch) != type)
	 return;

  if ((rnum = real_quest(GET_QUEST(ch))) < 0)
	 return;

  switch (type) {
	 case AQ_OBJECT:
		if (aquest_table[rnum].target == GET_OBJ_VNUM(object))
		  generic_complete_quest(ch);
		break;
	 case AQ_ROOM:
		if (aquest_table[rnum].target == world[ch->in_room].number)
		  generic_complete_quest(ch);
		break;
	 case AQ_MOB_FIND:
		for (i=world[ch->in_room].people; i; i = i->next_in_room)
		  if (IS_NPC(i))
			 if (aquest_table[rnum].target == GET_MOB_VNUM(i))
				generic_complete_quest(ch);
		break;
	 case AQ_MOB_KILL:
		if (!IS_NPC(ch) && IS_NPC(vict) && (ch != vict))
			 if (aquest_table[rnum].target == GET_MOB_VNUM(vict))
				generic_complete_quest(ch);
		break;
	 case AQ_MOB_SAVE:
		found = TRUE;
		if (ch == vict)
		  found = FALSE;

		for (i = world[ch->in_room].people; i && found; i = i->next_in_room)
			 if (i && IS_NPC(i))
				if ((GET_MOB_VNUM(i) != aquest_table[rnum].target) && !AFF_FLAGGED(i, AFF_MAJIN))
				  found = FALSE;

		if (found)
		  generic_complete_quest(ch);
		break;
	 case AQ_RETURN_OBJ:
		if (IS_NPC(vict) && (GET_MOB_VNUM(vict) == aquest_table[rnum].value[0]))
		  if (object && (GET_OBJ_VNUM(object) == aquest_table[rnum].target))
			 generic_complete_quest(ch);
		break;
	 default:
		log("SYSERR: Invalid quest type passed to autoquest_trigger_check");
		break;
  }
}

SPECIAL(questmaster)
{
  int tmp, num;
  struct char_data *qm = me;

  if (CMD_IS("list")) {
	 if (!*argument)
		list_quests(ch, GET_MOB_VNUM(qm));
	 else {
		if ((num = find_quest_by_qmnum(GET_MOB_VNUM(qm), atoi(argument))) >= 0) {
		  if (aquest_table[real_quest(num)].info)
			 send_to_char(aquest_table[real_quest(num)].info, ch);
		  else
			 send_to_char("There is no further information on that quest.\r\n", ch);
		} else
		  send_to_char("That is not a valid quest number!\r\n", ch);
	 }
	 return 1;
  }

  if (CMD_IS("join")) {
	 if (!*argument) {
		send_to_char("Join what quest?\r\n",ch);
		return 1;
	 }
	 if (GET_QUEST(ch) != -1) {
		send_to_char("You are already part of a quest!\r\n",ch);
		return 1;
	 }

	 tmp = atoi(argument);
	 tmp = find_quest_by_qmnum(GET_MOB_VNUM(qm), tmp);
	 if ((tmp >= 0) && !is_complete(ch, tmp)) {
		send_to_char(aquest_table[real_quest(tmp)].info, ch);
		send_to_char("You are now part of that quest!\r\n", ch);
		GET_QUEST(ch) = tmp;
		return 1;
	 } else if (is_complete(ch, tmp)) {
		send_to_char("You have already completed that quest!\r\n", ch);
		return 1;
	 } else {
		send_to_char("That is not a valid quest!\r\n",ch);
		return 1;
	 }
  }

  return 0;
}

ACMD(do_qstat)
{
  int vnum, rnum;
  char str[MAX_INPUT_LENGTH];
  struct char_data *tmp;

  half_chop(argument, str, argument);
  if (*str) {
	 vnum = atoi(str);
	 rnum = real_quest(vnum);
	 if (rnum < 0) {
		send_to_char("That vnum does not exist.\r\n", ch);
		return;
	 }
	 *buf = '\0';
	 tmp = read_mobile(aquest_table[rnum].mob_vnum, VIRTUAL);
	 sprintbit(aquest_table[rnum].flags, aq_flags, buf2);
	 sprintf(buf, "VNum: [%s%5d%s], RNum: [%5d] -- Questmaster: %s%s%s\r\n",
		CCGRN(ch, C_NRM), vnum, CCNRM(ch, C_NRM), rnum,
		CCYEL(ch, C_NRM), GET_NAME(tmp), CCNRM(ch, C_NRM));
	 sprintf(buf, "%sName: %s\r\n", buf, aquest_table[rnum].short_desc);
	 sprintf(buf, "%sDesc: %s\r\n", buf, aquest_table[rnum].desc);
	 sprintf(buf, "%sInformation:\r\n%s%s%s", buf, CCCYN(ch, C_NRM), aquest_table[rnum].info, CCNRM(ch, C_NRM));
	 sprintf(buf, "%sEnding:\r\n%s%s%s", buf, CCCYN(ch, C_NRM), aquest_table[rnum].ending, CCNRM(ch, C_NRM));
	 sprintf(buf, "%sType : %s, Target: %d, Experience: %d, Next quest: %d\r\n",
		buf, quest_types[aquest_table[rnum].type], aquest_table[rnum].target,
		aquest_table[rnum].exp, aquest_table[rnum].next_quest);
	 sprintf(buf, "%sValue: %d %d %d %d\r\n", buf, aquest_table[rnum].value[0],
		aquest_table[rnum].value[1], aquest_table[rnum].value[2],
		aquest_table[rnum].value[3]);
	 sprintf(buf, "%sFlags: %s\r\n", buf, buf2);
	 send_to_char(buf, ch);

  } else send_to_char("Usage: qstat <vnum>\r\n", ch);
}

ACMD(do_qlist)
{

  int first, last, nr, found = 0;
  char pagebuf[65536];

  strcpy(pagebuf,"");

  two_arguments(argument, buf, buf2);

  if (!*buf) {
	 send_to_char("Usage: qlist <begining number or zone> [<ending number>]\r\n", ch);
	 return;
  }

  first = atoi(buf);
  if (*buf2) last = atoi(buf2);
  else {
	 first *= 100;
	 last = first+99;
  }

  if ((first < 0) || (first > 99999) || (last < 0) || (last > 99999)) {
	 send_to_char("Values must be between 0 and 99999.\n\r", ch);
	 return;
  }

  if (first >= last) {
	 send_to_char("Second value must be greater than first.\n\r", ch);
	 return;
  }

  for (nr = 0; nr <= top_of_aquestt && (aquest_table[nr].virtual <= last); nr++)
  {
	 if (aquest_table[nr].virtual >= first) {
		sprintf(buf, "%5d. [%5d] %s\r\n", ++found,
				  aquest_table[nr].virtual,
				  aquest_table[nr].short_desc);
		strcat(pagebuf, buf);
	 }
  }

  if (!found)
	 send_to_char("No quests were found in those parameters.\n\r", ch);
  else page_string(ch->desc, pagebuf, TRUE);
}
