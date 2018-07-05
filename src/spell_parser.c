/* ************************************************************************
*   File: spell_parser.c                                Part of CircleMUD *
*  Usage: top-level magic routines; outside points of entry to magic sys. *
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
#include "interpreter.h"
#include "spells.h"
#include "handler.h"
#include "comm.h"
#include "db.h"

struct spell_info_type spell_info[TOP_SPELL_DEFINE + 1];

#define SINFO spell_info[spellnum]

extern struct room_data *world;

/* local functions */
void say_spell(struct char_data * ch, int spellnum, struct char_data * tch, struct obj_data * tobj);
void spello(int spl, int max_mana, int min_mana, int mana_change, int minpos, int targets, int violent, int routines);
int mag_manacost(struct char_data * ch, int spellnum);
ACMD(do_focus);
void unused_spell(int spl);
void mag_assign_spells(void);

/*
 * This arrangement is pretty stupid, but the number of skills is limited by
 * the playerfile.  We can arbitrarily increase the number of skills by
 * increasing the space in the playerfile. Meanwhile, this should provide
 * ample slots for skills.
 */

typedef struct spell_entry spell_entry;

const spell_entry spells[] =
{
  {"!RESERVED!", 0, 0, 0},			/* 0 - reserved */

  /* SPELLS */

  {"!UNUSED!", 0, 0, 0},		/* 1 */
  {"!UNUSED!", 0, 0, 0},
  {"zanzoken", CLASS_ALL_B, 175, 2},
  {"solarflare", CLASS_HUMAN_B | CLASS_SAIYAN_B | CLASS_BIODROID_B, 225, 3},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"holywater", 0, 0, 0},
  {"multiform", CLASS_HUMAN_B | CLASS_NAMEK_B | CLASS_BIODROID_B, 150, 2},
  {"!UNUSED!", 0, 0, 0},		/* 10 */
  {"!UNUSED!", 0, 0, 0},
  {"formgi", CLASS_NAMEK_B, 200, 0},
  {"!UNUSED!", 0, 0, 0},
  {"removeblindness", CLASS_KAI_B, 225, 2},
  {"!UNUSED!", 0, 0, 0},
  {"hasshuken", CLASS_SAIYAN_B | CLASS_HALF_BREED_B | CLASS_HUMAN_B, 70, 2},
  {"curse", CLASS_demon_B, 300, 3},
  {"!UNUSED!", 0, 0, 0},
  {"truesight", CLASS_ALL_B, 225, 2},
  {"!UNUSED!", 0, 0, 0},	/* 20 */
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"saibamen", 0, 1, 3},
  {"!UNUSED!", 0, 0, 0},
  {"heal", CLASS_NAMEK_B | CLASS_KAI_B | CLASS_MAJIN_B, 400, 2},
  {"invisibility", CLASS_KONATSU_B, 300, 2},
  {"!UNUSED!", 0, 0, 0},		/* 30 */
  {"!UNUSED!", 0, 0, 3},
  {"!UNUSED!", 0, 0, 0},
  {"poison", CLASS_MUTANT_B, 175, 3},
  {"protection", CLASS_KAI_B | CLASS_NAMEK_B | CLASS_KONATSU_B, 225, 2},
  {"removecurse", CLASS_KAI_B, 300, 2},
  {"barrier", CLASS_HUMAN_B | CLASS_HALF_BREED_B | CLASS_BIODROID_B | CLASS_SAIYAN_B | 
CLASS_NAMEK_B | CLASS_ICER_B | CLASS_ANDROID_B | CLASS_MAJIN_B | CLASS_demon_B, 300, 2},
  {"!UNUSED!", 0, 0, 0},
  {"yoikominminken", CLASS_HUMAN_B, 200, 3},
  {"senzubean", 0, 0, 3},
  {"!UNUSED!", 0, 0, 0},		 /* 40 */
  {"limbs", 0, 0, 3},
  {"!UNUSED!", 0, 0, 0},
  {"removepoison", CLASS_KAI_B, 175, 2},
  {"!UNUSED!", 0, 0, 0},
  {"animatedead", CLASS_demon_B, 100, 2},
  {"dispelgood", 0, 1, 3},
  {"groupbarrier", CLASS_TRUFFLE_B, 800, 2},
  {"groupheal", CLASS_MUTANT_B | CLASS_TRUFFLE_B | CLASS_MAJIN_B | CLASS_NAMEK_B, 400, 2},
  {"!UNUSED!", 0, 0, 0},
  {"infravision", CLASS_ICER_B | CLASS_BIODROID_B | CLASS_KONATSU_B | CLASS_ANDROID_B, 55, 2},	 /* 50 */
  {"!UNUSED!", 0, 0, 0}, /* 50 */
  {"mindfreeze", CLASS_KAI_B, 850, 3},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},		
  {"kinotsurugi", CLASS_SAIYAN_B, 100, 2},
  {"reproduce", CLASS_NAMEK_B, 100, 2},		/* 60 */
  {"!UNUSED!", 0, 0, 3},
  {"blademorph", CLASS_MUTANT_B, 125, 2},
  {"mutateskin", CLASS_MUTANT_B, 200, 2},
  {"formsword", CLASS_KONATSU_B, 50, 2},
  {"formspear", CLASS_demon_B, 250, 2},  /* 64 */
  {"buildshield", CLASS_TRUFFLE_B, 550, 2},
  {"nano", 0, 0, 3},
  {"candybeam", CLASS_MAJIN_B, 475, 2},
  {"!UNUSED!", 0, 0, 0},
  {"buildnanobean", CLASS_TRUFFLE_B, 750, 2},/* 69 */
  {"groupinvisible", CLASS_KONATSU_B, 400, 2},
  {"boost", 0, 0, 0},
  {"groupboost", CLASS_HUMAN_B, 1000, 3},
  {"portal", 0, 0, 3},
  {"enchant", CLASS_ANGEL_B, 300, 3},/* 74 */
  {"restore", CLASS_ANGEL_B, 1400, 3},
  {"stoned", 0, 0, 3},
  {"formwristband", CLASS_NAMEK_B, 300, 0},
  {"formpants", CLASS_NAMEK_B, 300, 0},
  {"platedskin", CLASS_MUTANT_B, 2000, 0},/* 79 */
  {"detect", CLASS_HUMAN_B | CLASS_HALF_BREED_B | CLASS_KONATSU_B | CLASS_BIODROID_B | CLASS_SAIYAN_B | CLASS_NAMEK_B | CLASS_KAI_B | CLASS_TRUFFLE_B | CLASS_MAJIN_B, 800, 2},
  {"rage", CLASS_HALF_BREED_B, 500, 0},
  {"block", CLASS_ALL_B, 500, 0},
  {"dodge", CLASS_ALL_B, 500, 0},
  {"alloutdefense", CLASS_ALL_B, 500, 0},/* 84 */
  {"bloodfrenzy", CLASS_ICER_B | CLASS_MUTANT_B, 800, 0},
  {"vitalstrike", CLASS_KONATSU_B, 900, 0},
  {"calm", CLASS_NAMEK_B, 1200, 0},
  {"limbrego", CLASS_KAI_B, 850, 0},
  {"instantregenerate", CLASS_BIODROID_B | CLASS_MAJIN_B, 850, 0},/* 89 */
  {"plasmabomb", CLASS_ANDROID_B, 5000, 0},
  {"stasiscells", CLASS_TRUFFLE_B, 2500, 0},
  {"moonburst", CLASS_SAIYAN_B, 4000, 0},
  {"innerfocus", CLASS_NAMEK_B, 500, 0},
  {"battlelust", CLASS_HALF_BREED_B, 4000, 0},/* 94 */
  {"dimizutoride", CLASS_KANASSAN_B, 5000, 0},
  {"rogafufuken", CLASS_HUMAN_B, 1000, 0},
  {"fishing", 0, 0, 3},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},/* 99 */
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},/* 104 */
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},/* 109 */
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},/* 114 */
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},/* 119 */
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},/* 124 */
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},		/* 129 */

  /* SKILLS */

  {"backstab", CLASS_KONATSU_B, 300, 3},		/* 130 */
  {"sweep", CLASS_ALL_B, 100, 3},
  {"hide", 0, 0, 3},
  {"zanzokenelbow", CLASS_ALL_B, 375, 3},
  {"picklock", CLASS_KONATSU_B, 450, 2},
  {"heel strike", CLASS_ALL_B, 600, 3},
  {"rescue", CLASS_ALL_B, 65, 2},
  {"swiftness", CLASS_HUMAN_B | CLASS_SAIYAN_B | CLASS_NAMEK_B | CLASS_KONATSU_B, 120, 2},
  {"mug", CLASS_KONATSU_B, 7500, 3},
  {"sense", CLASS_HUMAN_B | CLASS_HALF_BREED_B | CLASS_KONATSU_B | CLASS_BIODROID_B | CLASS_SAIYAN_B | CLASS_NAMEK_B | CLASS_KAI_B | CLASS_TRUFFLE_B | CLASS_MAJIN_B, 30, 2},		/* 140 */
  {"tailwhip", CLASS_ICER_B | CLASS_BIODROID_B, 345, 3},
  {"heavensrain", CLASS_ANGEL_B, 3000, 3},
  {"repair", CLASS_ANDROID_B, 100, 2},
  {"kaioken", CLASS_SAIYAN_B | CLASS_KAI_B, 200, 2},
  {"knee", CLASS_ALL_B, 25, 3},/* 144 */
  {"elbow", CLASS_ALL_B, 25, 3},
  {"roundhouse", CLASS_ALL_B, 25, 3},
  {"cosmichalo", CLASS_ANGEL_B, 3000, 3},
  {"hydroblade", CLASS_KANASSAN_B, 825, 3},
  {"ingest", CLASS_MAJIN_B, 100, 3},
  {"hydroeclipsecannon", CLASS_KANASSAN_B, 2000, 3},
  {"risingpheonixslash", CLASS_KONATSU_B, 2400, 3}, /* 51 */
  {"absorb", CLASS_ANDROID_B | CLASS_BIODROID_B, 100, 3},
  {"selfdestruct", CLASS_ANDROID_B | CLASS_BIODROID_B | CLASS_HUMAN_B | CLASS_TRUFFLE_B | CLASS_MAJIN_B, 100, 3},/* 150 */
  {"powersense", CLASS_ALL_B, 50, 2}, /* 155 */
  {"stealth", CLASS_KONATSU_B, 150, 2},
  {"regenerate", CLASS_TRUFFLE_B | CLASS_MAJIN_B | CLASS_NAMEK_B | CLASS_BIODROID_B, 350, 3},
  {"kamehameha", CLASS_BIODROID_B | CLASS_HALF_BREED_B | CLASS_SAIYAN_B | CLASS_HUMAN_B, 100, 3},
  {"big bang", CLASS_SAIYAN_B, 1200, 3},
  {"finalflash", CLASS_SAIYAN_B, 500, 3}, /* 160 */
  {"choukamehameha", CLASS_BIODROID_B | CLASS_HALF_BREED_B | CLASS_SAIYAN_B, 1200, 3},
  {"destructodisk", CLASS_ANDROID_B | CLASS_ICER_B | CLASS_HUMAN_B | CLASS_ANGEL_B, 500, 3},
  {"masenko", CLASS_HALF_BREED_B | CLASS_ANGEL_B, 250, 3},
  {"renzokou energy dan", CLASS_ALL_B, 400, 3},
  {"makankosappo", CLASS_NAMEK_B | CLASS_BIODROID_B, 650, 3},/* 165 */
  {"kishot", CLASS_ALL_B, 45, 3},
  {"kikoho", CLASS_HUMAN_B | CLASS_HALF_BREED_B, 300, 3},
  {"deathball", CLASS_ICER_B, 2000, 3},
  {"hellsflash", CLASS_ANDROID_B, 1750, 3},
  {"kousengan", CLASS_ICER_B | CLASS_NAMEK_B | CLASS_MAJIN_B, 700, 3},
  {"combatyoik", CLASS_HUMAN_B, 600, 3},
  {"burningattack", CLASS_HALF_BREED_B, 2000, 3}, /* 172 */
  {"spiritball", CLASS_HUMAN_B, 2000, 3},
  {"danceofsevendragons", CLASS_HALF_BREED_B | CLASS_SAIYAN_B, 5000, 3},/* 175 */
  {"twin blade slash", CLASS_KONATSU_B, 1500, 3},
  {"kakusanha", CLASS_HUMAN_B, 2000, 3},
  {"hellspearblast", CLASS_demon_B, 2500, 3},
  {"gigasbeam", CLASS_TRUFFLE_B, 2000, 3},
  {"meditate", CLASS_NAMEK_B | CLASS_demon_B | CLASS_KAI_B, 4550, 2},
  {"beam", CLASS_ALL_B, 90, 3},/* 180 */
  {"tsuihidan", CLASS_ALL_B, 110, 3},
  {"shogekiha", CLASS_ALL_B, 160, 3},
  {"bigbangkame", CLASS_SAIYAN_B, 10000, 3},
  {"havocblast", CLASS_ALL_B, 6000, 3},
  {"galikgun", CLASS_SAIYAN_B, 240, 3},/* 185 */
  {"honoo", CLASS_demon_B, 640, 3},
  {"bakuhatsuha", CLASS_SAIYAN_B, 540, 3},
  {"crusher", CLASS_MUTANT_B, 2000, 3},
  {"erasercannon", CLASS_MUTANT_B | CLASS_SAIYAN_B, 460, 3},
  {"kyodaika", CLASS_NAMEK_B, 320, 3},/* 190 */
  {"scattershot", CLASS_NAMEK_B, 2000, 3},
  {"genkidama", CLASS_KAI_B, 3000, 3},
  {"forelock whip", CLASS_MAJIN_B, 350, 3},
  {"chikyuugekigama", CLASS_MAJIN_B, 2000, 3},
  {"genocide", CLASS_MAJIN_B, 1500, 3},/* 195 */
  {"deflect", CLASS_ALL_B, 1500, 2},
  {"potential release", CLASS_NAMEK_B | CLASS_KAI_B, 500, 2},
  {"majinize", CLASS_MAJIN_B, 500, 2},
  {"mindread", CLASS_KAI_B, 250, 2},
  {"dualwield", CLASS_KONATSU_B, 1000, 2},	/* 200 */

  /* OBJECT SPELLS AND NPC SPELLS/SKILLS */

  {"scouter", 0, 0, 0},
  {"!UNUSED!", 0, 0, 3},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"!UNUSED!", 0, 0, 0},
  {"identify", 0, 0, 0},			/* 201 */
  {"firebreath", 0, 0, 0},
  {"gasbreath", 0, 0, 0},
  {"frost breath", 0, 0, 0},
  {"acid breath", 0, 0, 0},
  {"lightning breath", 0, 0, 0},
  {"death2", 0, 0, 0},

  {"\n", 0, 0, 0}					/* the end */
};


struct syllable {
  const char *org;
  const char *news;
};


struct syllable syls[] = {
  {" ", " "},
  {"ar", "abra"},
  {"ate", "i"},
  {"cau", "kada"},
  {"SOLAR_FLARE", "nose"},
  {"bur", "mosa"},
  {"cu", "judi"},
  {"de", "oculo"},
  {"dis", "mar"},
  {"ect", "kamina"},
  {"en", "uns"},
  {"gro", "cra"},
  {"light", "dies"},
  {"lo", "hi"},
  {"magi", "kari"},
  {"mon", "bar"},
  {"mor", "zak"},
  {"move", "sido"},
  {"ness", "lacri"},
  {"ning", "illa"},
  {"per", "duda"},
  {"ra", "gru"},
  {"re", "candus"},
  {"son", "sabru"},
  {"tect", "infra"},
  {"tri", "cula"},
  {"ven", "nofo"},
  {"word of", "inset"},
  {"a", "i"}, {"b", "v"}, {"c", "q"}, {"d", "m"}, {"e", "o"}, {"f", "y"}, {"g", "t"},
  {"h", "p"}, {"i", "u"}, {"j", "y"}, {"k", "t"}, {"l", "r"}, {"m", "w"}, {"n", "b"},
  {"o", "a"}, {"p", "s"}, {"q", "d"}, {"r", "f"}, {"s", "g"}, {"t", "h"}, {"u", "e"},
  {"v", "z"}, {"w", "x"}, {"x", "n"}, {"y", "l"}, {"z", "k"}, {"", ""}
};


int mag_manacost(struct char_data * ch, int spellnum)
{
  int mana;

  mana = MAX(SINFO.mana_max - (SINFO.mana_change *
		    (GET_LEVEL(ch) - SINFO.min_level[(int) GET_CLASS(ch)])),
	     SINFO.mana_min);

  return mana;
}


/* say_spell erodes buf, buf1, buf2 */
void say_spell(struct char_data * ch, int spellnum, struct char_data * tch,
	            struct obj_data * tobj)
{
  char lbuf[256];

  struct char_data *i;
  int j, ofs = 0;

  *buf = '\0';
  strcpy(lbuf, spells[spellnum].name);

  while (*(lbuf + ofs)) {
    for (j = 0; *(syls[j].org); j++) {
      if (!strncmp(syls[j].org, lbuf + ofs, strlen(syls[j].org))) {
	strcat(buf, syls[j].news);
	ofs += strlen(syls[j].org);
      }
    }
  }

  if (tch != NULL && tch->in_room == ch->in_room) {
    if (tch == ch)
      sprintf(lbuf, "$n concentrates and gathers $s energy.");
    else
      sprintf(lbuf, "$n gathers energy and stares at $N.");
  } else if (tobj != NULL &&
	     ((tobj->in_room == ch->in_room) || (tobj->carried_by == ch)))
    sprintf(lbuf, "$n gathers their Ki as they stare at $p.");
  else
    sprintf(lbuf, "$n concentrates and gathers $s energy.");

  sprintf(buf1, lbuf, spells[spellnum]);
  sprintf(buf2, lbuf, buf);

  for (i = world[ch->in_room].people; i; i = i->next_in_room) {
    if (i == ch || i == tch || !i->desc || !AWAKE(i))
      continue;
    if (GET_CLASS(ch) == GET_CLASS(i))
      perform_act(buf1, ch, tobj, tch, i);
    else
      perform_act(buf2, ch, tobj, tch, i);
  }

  if (tch != NULL && tch != ch && tch->in_room == ch->in_room) {
    sprintf(buf1, "$n stares at you and starts gathering energy.");
    act(buf1, FALSE, ch, NULL, tch, TO_VICT);
  }
}


const char *skill_name(int num)
{
  int i = 0;

  if (num <= 0) {
    if (num == -1)
      return "UNUSED";
    else
      return "UNDEFINED";
  }

  while (num && spells[i].name[0] != '\n') {
    num--;
    i++;
  }

  if (spells[i].name[0] != '\n')
    return spells[i].name;
  else
    return "UNDEFINED";
}

	 
int find_skill_num(char *name)
{
  int index = 0, ok;
  char *temp, *temp2;
  char first[256], first2[256];

  while (spells[++index].name[0] != '\n') {
    if (is_abbrev(name, spells[index].name))
      return index;

    ok = 1;
    /* It won't be changed, but other uses of this function elsewhere may. */
    temp = any_one_arg((char*)(spells[index].name), first);
    temp2 = any_one_arg(name, first2);
    while (*first && *first2 && ok) {
      if (!is_abbrev(first2, first))
	ok = 0;
      temp = any_one_arg(temp, first);
      temp2 = any_one_arg(temp2, first2);
    }

    if (ok && !*first2)
      return index;
  }

  return -1;
}



/*
 * This function is the very heart of the entire magic system.  All
 * invocations of all types of magic -- objects, spoken and unspoken PC
 * and NPC spells, the works -- all come through this function eventually.
 * This is also the entry point for non-spoken or unrestricted spells.
 * Spellnum 0 is legal but silently ignored here, to make callers simpler.
 */
int call_magic(struct char_data * focuser, struct char_data * cvict,
	     struct obj_data * ovict, int spellnum, int level, int focustype)
{
  int savetype;

  if (spellnum < 1 || spellnum > TOP_SPELL_DEFINE)
    return 0;

  if (ROOM_FLAGGED(focuser->in_room, ROOM_NOMAGIC)) {
    send_to_char("Your ki fizzles out and dies.\r\n", focuser);
    act("$n's energy fizzles out and dies.", FALSE, focuser, 0, 0, TO_ROOM);
    return 0;
  }
  if (ROOM_FLAGGED(focuser->in_room, ROOM_PEACEFUL) &&
      (SINFO.violent || IS_SET(SINFO.routines, MAG_DAMAGE))) {
    send_to_char("A flash of white light fills the room, dispelling your "
		 "violent energy!\r\n", focuser);
    act("White light from no particular source suddenly fills the room, "
	"then vanishes.", FALSE, focuser, 0, 0, TO_ROOM);
    return 0;
  }
  /* determine the type of saving throw */
  switch (focustype) {
  case focus_STAFF:
  case focus_SENZU:
  case focus_STONED:
  case focus_LIMB:
  case focus_HOLY:
  case focus_NANO:
  case focus_SAIBAKIT:
  case focus_SCOUTER:
    savetype = SAVING_ROD;
    break;
  case focus_SPELL:
    savetype = SAVING_SPELL;
    break;
  default:
    savetype = SAVING_BREATH;
    break;
  }


  /*
   * Hm, target could die here.  Wonder if we should move this down lower to
   * give the other spells a chance to go off first? -gg 6/24/98
   */
  if (IS_SET(SINFO.routines, MAG_DAMAGE))
    if (mag_damage(level, focuser, cvict, spellnum, savetype) == -1)
      return 1;

  if (IS_SET(SINFO.routines, MAG_AFFECTS))
    mag_affects(level, focuser, cvict, spellnum, savetype);

  if (IS_SET(SINFO.routines, MAG_UNAFFECTS))
    mag_unaffects(level, focuser, cvict, spellnum, savetype);

  if (IS_SET(SINFO.routines, MAG_POINTS))
    mag_points(level, focuser, cvict, spellnum, savetype);

  if (IS_SET(SINFO.routines, MAG_ALTER_OBJS))
    mag_alter_objs(level, focuser, ovict, spellnum, savetype);

  if (IS_SET(SINFO.routines, MAG_GROUPS))
    mag_groups(level, focuser, spellnum, savetype);

  if (IS_SET(SINFO.routines, MAG_MASSES))
    mag_masses(level, focuser, spellnum, savetype);

  if (IS_SET(SINFO.routines, MAG_AREAS))
    mag_areas(level, focuser, spellnum, savetype);

  if (IS_SET(SINFO.routines, MAG_SUMMONS))
    mag_summons(level, focuser, ovict, spellnum, savetype);

  if (IS_SET(SINFO.routines, MAG_CREATIONS))
    mag_creations(level, focuser, spellnum);

  if (IS_SET(SINFO.routines, MAG_MANUAL))
    switch (spellnum) {
    case SPELL_MAJIN:		MANUAL_SPELL(spell_MAJIN); break;
    case SPELL_CREATE_WATER:	MANUAL_SPELL(spell_create_water); break;
    case SPELL_BURNING_SOULS:   MANUAL_SPELL(spell_BURNING_SOULS); break;
    case SPELL_IDENTIFY:	MANUAL_SPELL(spell_IDENTIFY); break;
    case SPELL_LOCATE_OBJECT:   MANUAL_SPELL(spell_locate_object); break;
    case SPELL_SUMMON:		MANUAL_SPELL(spell_summon); break;
    case SPELL_WORD_OF_RECALL:  MANUAL_SPELL(spell_recall); break;
    case SPELL_TELEPORT:	MANUAL_SPELL(spell_teleport); break;
    case SPELL_POWER_SENSE:	MANUAL_SPELL(spell_POWER_SENSE); break;
    case SPELL_PORTAL:     MANUAL_SPELL(spell_gate); break;
    }

  return 1;
}

/*
 * mag_objectmagic: This is the entry-point for all magic items.  This should
 * only be called by the 'PLANT', 'use', 'SWALLOW', etc. routines.
 *
 * For reference, object values 0-3:
 * STAFF  - [0]	level	[1] max charges	[2] num charges	[3] spell num
 * SCOUTER   - [0]	level	[1] max charges	[2] num charges	[3] spell num
 * SENZU - [0]	level	[1] spell num	[2] spell num	[3] spell num
 * SAIBAKIT - [0] level	[1] spell num	[2] spell num	[3] spell num
 *
 * Staves and SCOUTERs will default to level 14 if the level is not specified;
 * the DikuMUD format did not specify STAFF and SCOUTER levels in the world
 * files (this is a CircleMUD enhancement).
 */

void mag_objectmagic(struct char_data * ch, struct obj_data * obj,
		          char *argument)
{
  int i, k;
  struct char_data *tch = NULL, *next_tch;
  struct obj_data *tobj = NULL;

  one_argument(argument, arg);

  k = generic_find(arg, FIND_CHAR_ROOM | FIND_OBJ_INV | FIND_OBJ_ROOM |
		   FIND_OBJ_EQUIP, ch, &tch, &tobj);

  switch (GET_OBJ_TYPE(obj)) {
  case ITEM_STAFF:
    act("You begin using $p.", FALSE, ch, obj, 0, TO_CHAR);
    if (obj->action_description)
      act(obj->action_description, FALSE, ch, obj, 0, TO_ROOM);
    else
      act("$n uses $p.", FALSE, ch, obj, 0, TO_ROOM);

    if (GET_OBJ_VAL(obj, 2) <= 0) {
      act("It seems powerless.", FALSE, ch, obj, 0, TO_CHAR);
      act("Nothing seems to happen.", FALSE, ch, obj, 0, TO_ROOM);
    } else {
      GET_OBJ_VAL(obj, 2)--;
      WAIT_STATE(ch, PULSE_VIOLENCE);
      for (tch = world[ch->in_room].people; tch; tch = next_tch) {
	next_tch = tch->next_in_room;
	if (ch == tch)
	  continue;
	if (GET_OBJ_VAL(obj, 0))
	  call_magic(ch, tch, NULL, GET_OBJ_VAL(obj, 3),
		     GET_OBJ_VAL(obj, 0), focus_STAFF);
	else
	  call_magic(ch, tch, NULL, GET_OBJ_VAL(obj, 3),
		     DEFAULT_STAFF_LVL, focus_STAFF);
      }
    }
    break;
  case ITEM_SCOUTER:
    if (k == FIND_CHAR_ROOM) {
      if (tch == ch) {
	act("You point $p at yourself.", FALSE, ch, obj, 0, TO_CHAR);
	act("$n points $p at $mself.", FALSE, ch, obj, 0, TO_ROOM);
      } else {
	act("You point $p at $N and press a button.", FALSE, ch, obj, tch, TO_CHAR);
	if (obj->action_description != NULL)
	  act(obj->action_description, FALSE, ch, obj, tch, TO_ROOM);
	else
	  act("$n points $p at $N and presses a button.", TRUE, ch, obj, tch, TO_ROOM);
      }
    } else if (tobj != NULL && (IS_NPC(tch) || IS_Human(tch) || IS_Namek(tch) || IS_saiyan(tch) ||  IS_icer(tch) || IS_MAJIN(tch) || IS_BIODROID(tch) || IS_ANDROID(tch) || IS_MUTANT(tch) || IS_KAI(tch) || IS_KONATSU(tch) || IS_TRUFFLE(tch) || IS_demon(tch) || IS_KANASSAN(tch)  || IS_HALF_BREED(tch))) {
      act("You point $p at $P and press a button.", FALSE, ch, obj, tobj, TO_CHAR);
      if (obj->action_description != NULL && (IS_NPC(tch) || IS_Human(tch) || IS_Namek(tch) || IS_saiyan(tch) ||  IS_icer(tch) || IS_MAJIN(tch) || IS_BIODROID(tch) || IS_ANDROID(tch) || IS_MUTANT(tch) || IS_KAI(tch) || IS_KONATSU(tch) || IS_TRUFFLE(tch) || IS_demon(tch) || IS_KANASSAN(tch) || IS_HALF_BREED(tch)))
	act(obj->action_description, FALSE, ch, obj, tobj, TO_ROOM);
      else if (IS_NPC(tch) || IS_Human(tch) || IS_Namek(tch) || IS_saiyan(tch) || IS_icer(tch) || IS_MAJIN(tch) || IS_BIODROID(tch) || IS_ANDROID(tch) || IS_MUTANT(tch) || IS_KAI(tch) || IS_KONATSU(tch) || IS_TRUFFLE(tch) || IS_demon(tch) || IS_KANASSAN(tch) || IS_HALF_BREED(tch))
	act("$n points $p at $P and presses a button.", TRUE, ch, obj, tobj, TO_ROOM);
    } else {
      act("At what should $p be pointed?", FALSE, ch, obj, NULL, TO_CHAR);
      return;
    }

    if (GET_OBJ_VAL(obj, 2) <= 0) {
      act("It seems powerless.", FALSE, ch, obj, 0, TO_CHAR);
      act("Nothing seems to happen.", FALSE, ch, obj, 0, TO_ROOM);
      return;
    }
    GET_OBJ_VAL(obj, 2)--;
    WAIT_STATE(ch, PULSE_VIOLENCE);
    if (GET_OBJ_VAL(obj, 0))
      call_magic(ch, tch, tobj, GET_OBJ_VAL(obj, 3),
		 GET_OBJ_VAL(obj, 0), focus_SCOUTER);
    else
      call_magic(ch, tch, tobj, GET_OBJ_VAL(obj, 3),
		 DEFAULT_SCOUTER_LVL, focus_SCOUTER);
    break;

  case ITEM_TRASH:
    if (*arg) {
      if (!k) {
        act("There is nothing to here to affect with $p.", FALSE,
            ch, obj, NULL, TO_CHAR);
        return;
      }
    } else
      tch = ch;

    act("&15You smoke $p.", TRUE, ch, obj, 0, TO_CHAR);
    if (obj->action_description)
      act(obj->action_description, FALSE, ch, obj, NULL, TO_ROOM);
    else
      act("&14$n &15smokes $p.", FALSE, ch, obj, NULL, TO_ROOM);

    WAIT_STATE(ch, PULSE_VIOLENCE);
    for (i = 1; i < 4; i++)
      if (!(call_magic(ch, tch, tobj, GET_OBJ_VAL(obj, i),
                       GET_OBJ_VAL(obj, 0), focus_STONED)))
        break;
    if (obj != NULL)
      extract_obj(obj);
    break;

  case ITEM_HOLYWATER:
    if (*arg) {
      if (!k) {
        act("There is nothing to here to affect with $p.", FALSE,
            ch, obj, NULL, TO_CHAR);
        return;
      }
    } else
      tch = ch;

    act("You swallow $p.", TRUE, ch, obj, 0, TO_CHAR);
    if (obj->action_description)
      act(obj->action_description, FALSE, ch, obj, NULL, TO_ROOM);
    else
      act("$n swallows $p.", FALSE, ch, obj, NULL, TO_ROOM);

    WAIT_STATE(ch, PULSE_VIOLENCE);
    for (i = 1; i < 4; i++)
      if (!(call_magic(ch, tch, tobj, GET_OBJ_VAL(obj, i),
                       GET_OBJ_VAL(obj, 0), focus_HOLY)))
        break;
    if (obj != NULL)
      extract_obj(obj);
    break;


  case ITEM_NANO:
    if (*arg) {
      if (!k) {
	act("There is nothing to here to affect with $p.", FALSE,
	    ch, obj, NULL, TO_CHAR);
	return;
      }
    } else
      tch = ch;

    act("You swallow $p.", TRUE, ch, obj, 0, TO_CHAR);
    if (obj->action_description)
      act(obj->action_description, FALSE, ch, obj, NULL, TO_ROOM);
    else
      act("$n swallows $p.", FALSE, ch, obj, NULL, TO_ROOM);

    WAIT_STATE(ch, PULSE_VIOLENCE);
    for (i = 1; i < 4; i++)
      if (!(call_magic(ch, tch, tobj, GET_OBJ_VAL(obj, i),
                       GET_OBJ_VAL(obj, 0), focus_NANO)))
        break;
    if (obj != NULL)
      extract_obj(obj);
    break;

  case ITEM_SENZU:
    if (*arg) {
      if (!k) {
        act("There is nothing to here to affect with $p.", FALSE,
            ch, obj, NULL, TO_CHAR);
        return;
      }
    } else
      tch = ch;

    act("You swallow $p.", TRUE, ch, obj, 0, TO_CHAR);
    if (obj->action_description)
      act(obj->action_description, FALSE, ch, obj, NULL, TO_ROOM);
    else
      act("$n swallows $p.", FALSE, ch, obj, NULL, TO_ROOM);

    WAIT_STATE(ch, PULSE_VIOLENCE);
    for (i = 1; i < 4; i++)
      if (!(call_magic(ch, tch, tobj, GET_OBJ_VAL(obj, i),
		       GET_OBJ_VAL(obj, 0), focus_SENZU)))
	break;

    if (obj != NULL)
      extract_obj(obj);
    break;
  case ITEM_SAIBAKIT:
    tch = ch;
    act("You plant $p.", FALSE, ch, obj, NULL, TO_CHAR);
    if (obj->action_description)
      act(obj->action_description, FALSE, ch, obj, NULL, TO_ROOM);
    else
      act("$n plants $p.", TRUE, ch, obj, NULL, TO_ROOM);

    WAIT_STATE(ch, PULSE_VIOLENCE);
    for (i = 1; i < 4; i++)
      if (!(call_magic(ch, ch, NULL, GET_OBJ_VAL(obj, i),
		       GET_OBJ_VAL(obj, 0), focus_SAIBAKIT)))
	break;

    if (obj != NULL)
      extract_obj(obj);
    break;
  default:
    log("SYSERR: Unknown object_type %d in mag_objectmagic.",
	GET_OBJ_TYPE(obj));
    break;
  }
}


/*
 * focus_spell is used generically to focus any spoken spell, assuming we
 * already have the target char/obj and spell number.  It checks all
 * restrictions, etc., prints the words, etc.
 *
 * Entry point for NPC focuss.  Recommended entry point for spells focus
 * by NPCs via specprocs.
 */

int focus_spell(struct char_data * ch, struct char_data * tch,
	           struct obj_data * tobj, int spellnum)
{
  if (spellnum < 0 || spellnum > TOP_SPELL_DEFINE) {
    log("SYSERR: focus_spell trying to call spellnum %d/%d.\n", spellnum,
	TOP_SPELL_DEFINE);
    return 0;
  }
    
  if (GET_POS(ch) < SINFO.min_position) {
    switch (GET_POS(ch)) {
      case POS_SLEEPING:
      send_to_char("You dream about great powers.\r\n", ch);
      break;
    case POS_RESTING:
      send_to_char("You cannot concentrate while resting.\r\n", ch);
      break;
    case POS_SITTING:
      send_to_char("You can't do this sitting!\r\n", ch);
      break;
    case POS_FIGHTING:
      send_to_char("Impossible!  You can't concentrate enough!\r\n", ch);
      break;
    default:
      send_to_char("You can't do much of anything like this!\r\n", ch);
      break;
    }
    return 0;
  }
  if (AFF_FLAGGED(ch, AFF_MAJIN) && (ch->master == tch)) {
    send_to_char("You you might hurt your master!\r\n", ch);
    return 0;
  }
  if ((tch != ch) && IS_SET(SINFO.targets, TAR_SELF_ONLY)) {
    send_to_char("You can only use this upon yourself!\r\n", ch);
    return 0;
  }
  if ((tch == ch) && IS_SET(SINFO.targets, TAR_NOT_SELF)) {
    send_to_char("You cannot use this on yourself!\r\n", ch);
    return 0;
  }
  if (IS_SET(SINFO.routines, MAG_GROUPS) && !AFF_FLAGGED(ch, AFF_GROUP)) {
    send_to_char("You can't use this if you're not in a group!\r\n",ch);
    return 0;
  }
  send_to_char(OK, ch);
  say_spell(ch, spellnum, tch, tobj);

  return (call_magic(ch, tch, tobj, spellnum, GET_LEVEL(ch), focus_SPELL));
}


/*
 * do_focus is the entry point for PC-focused spells.  It parses the arguments,
 * determines the spell number and finds a target, throws the die to see if
 * the spell can be focus, checks for sufficient mana and subtracts it, and
 * passes control to focus_spell().
 */

ACMD(do_focus)
{
  struct char_data *tch = NULL;
  struct obj_data *tobj = NULL;
  char *s, *t;
  int mana, spellnum, i, target = 0;


  /* get: blank, spell name, target name */
  s = strtok(argument, "'");

  if (s == NULL) {
    send_to_char("Focus What Where?\r\n", ch);
    return;
  }
  s = strtok(NULL, "'");
  if (s == NULL) {
    send_to_char("Need to be in symbols: ''\r\n", ch);
    return;
  }
  t = strtok(NULL, "\0");

  /* spellnum = search_block(s, spells, 0); */
  spellnum = find_skill_num(s);

  if ((spellnum < 1) || (spellnum > MAX_SPELLS)) {
    send_to_char("Focus what?!?\r\n", ch);
    return;
  }
   if (GET_LEVEL(ch) < SINFO.min_level[(int) GET_CLASS(ch)]) {
    send_to_char("You do not know how to do that!\r\n", ch);
    return;
   }
  if (GET_SKILL(ch, spellnum) == 0) {
    send_to_char("You are unfamiliar with that technique.\r\n", ch);
    return;
  }
  /* Find the target */
  if (t != NULL) {
    one_argument(strcpy(arg, t), t);
    skip_spaces(&t);
  }
  if (IS_SET(SINFO.targets, TAR_IGNORE)) {
    target = TRUE;
  } else if (t != NULL && *t) {
    if (!target && (IS_SET(SINFO.targets, TAR_CHAR_ROOM))) {
      if ((tch = get_char_room_vis(ch, t)) != NULL)
	target = TRUE;
    }
    if (!target && IS_SET(SINFO.targets, TAR_CHAR_WORLD))
      if ((tch = get_char_vis(ch, t)))
	target = TRUE;

    if (!target && IS_SET(SINFO.targets, TAR_OBJ_INV))
      if ((tobj = get_obj_in_list_vis(ch, t, ch->carrying)))
	target = TRUE;

    if (!target && IS_SET(SINFO.targets, TAR_OBJ_EQUIP)) {
      for (i = 0; !target && i < NUM_WEARS; i++)
	if (GET_EQ(ch, i) && isname(t, GET_EQ(ch, i)->name)) {
	  tobj = GET_EQ(ch, i);
	  target = TRUE;
	}
    }
    if (!target && IS_SET(SINFO.targets, TAR_OBJ_ROOM))
      if ((tobj = get_obj_in_list_vis(ch, t, world[ch->in_room].contents)))
	target = TRUE;

    if (!target && IS_SET(SINFO.targets, TAR_OBJ_WORLD))
      if ((tobj = get_obj_vis(ch, t)))
	target = TRUE;

  } else {			/* if target string is empty */
    if (!target && IS_SET(SINFO.targets, TAR_FIGHT_SELF))
      if (FIGHTING(ch) != NULL) {
	tch = ch;
	target = TRUE;
      }
    if (!target && IS_SET(SINFO.targets, TAR_FIGHT_VICT))
      if (FIGHTING(ch) != NULL) {
	tch = FIGHTING(ch);
	target = TRUE;
      }
    /* if no target specified, and the spell isn't violent, default to self */
    if (!target && IS_SET(SINFO.targets, TAR_CHAR_ROOM) &&
	!SINFO.violent) {
      tch = ch;
      target = TRUE;
    }
    if (!target) {
      sprintf(buf, "Upon %s should the technique be used?\r\n",
	 IS_SET(SINFO.targets, TAR_OBJ_ROOM | TAR_OBJ_INV | TAR_OBJ_WORLD) ?
	      "what" : "who");
      send_to_char(buf, ch);
      return;
    }
  }

  if (target && (tch == ch) && SINFO.violent) {
    send_to_char("You shouldn't focus that on yourself -- could be bad for your health!\r\n", ch);
    return;
  }
  if (!target) {
    send_to_char("Cannot find the target of your technique!\r\n", ch);
    return;
  }
  mana = mag_manacost(ch, spellnum);
  if ((mana > 0) && (GET_MANA(ch) < mana) && (GET_LEVEL(ch) < LVL_IMMORT)) {
    send_to_char("You haven't the energy to use that technique!\r\n", ch);
    return;
  }

  /* You throws the dice and you takes your chances.. 101% is total failure */
  if (number(0, 101) > GET_SKILL(ch, spellnum)) {
    WAIT_STATE(ch, PULSE_VIOLENCE);
    if (!tch || !skill_message(0, ch, tch, spellnum))
      send_to_char("You lost your concentration!\r\n", ch);
    if (mana > 0)
      GET_MANA(ch) = MAX(0, MIN(GET_MAX_MANA(ch), GET_MANA(ch) - (mana / 2)));
    if (SINFO.violent && tch && IS_NPC(tch))
      hit(tch, ch, TYPE_UNDEFINED);
  } else { /* focus spell returns 1 on success; subtract mana & set waitstate */
    if (focus_spell(ch, tch, tobj, spellnum)) {
      WAIT_STATE(ch, PULSE_VIOLENCE);
      if (mana > 0)
	GET_MANA(ch) = MAX(0, MIN(GET_MAX_MANA(ch), GET_MANA(ch) - mana));
    }
  }
}



void spell_level(int spell, int chclass, long long level)
{
  int bad = 0;

  if (spell < 0 || spell > TOP_SPELL_DEFINE) {
    log("SYSERR: attempting assign to illegal spellnum %d/%d", spell, TOP_SPELL_DEFINE);
    return;
  }

  if (chclass < 0 || chclass >= NUM_CLASSES) {
    log("SYSERR: assigning '%s' to illegal class %d/%d.", skill_name(spell),
		chclass, NUM_CLASSES - 1);
    bad = 1;
  }

  if (level < 1 || level > LVL_IMPL) {
    log("SYSERR: assigning '%s' to illegal level %Ld/%Ld.", skill_name(spell),
		level, LVL_IMPL);
    bad = 1;
  }

  if (!bad)    
    spell_info[spell].min_level[chclass] = level;
}


/* Assign the spells on boot up */
void spello(int spl, int max_mana, int min_mana, int mana_change, int minpos,
	         int targets, int violent, int routines)
{
  int i;

  for (i = 0; i < NUM_CLASSES; i++)
    spell_info[spl].min_level[i] = LVL_IMMORT;
  spell_info[spl].mana_max = max_mana;
  spell_info[spl].mana_min = min_mana;
  spell_info[spl].mana_change = mana_change;
  spell_info[spl].min_position = minpos;
  spell_info[spl].targets = targets;
  spell_info[spl].violent = violent;
  spell_info[spl].routines = routines;
}


void unused_spell(int spl)
{
  int i;

  for (i = 0; i < NUM_CLASSES; i++)
    spell_info[spl].min_level[i] = LVL_IMPL + 1;
  spell_info[spl].mana_max = 0;
  spell_info[spl].mana_min = 0;
  spell_info[spl].mana_change = 0;
  spell_info[spl].min_position = 0;
  spell_info[spl].targets = 0;
  spell_info[spl].violent = 0;
  spell_info[spl].routines = 0;
}

#define skillo(skill) spello(skill, 0, 0, 0, 0, 0, 0, 0);


/*
 * Arguments for spello calls:
 *
 * spellnum, maxmana, minmana, manachng, minpos, targets, violent?, routines.
 *
 * spellnum:  Number of the spell.  Usually the symbolic name as defined in
 * spells.h (such as SPELL_HEAL).
 *
 * maxmana :  The maximum mana this spell will take (i.e., the mana it
 * will take when the player first gets the spell).
 *
 * minmana :  The minimum mana this spell will take, no matter how high
 * level the focuser is.
 *
 * manachng:  The change in mana for the spell from level to level.  This
 * number should be positive, but represents the reduction in mana cost as
 * the focuser's level increases.
 *
 * minpos  :  Minimum position the focuser must be in for the spell to work
 * (usually fighting or standing). targets :  A "list" of the valid targets
 * for the spell, joined with bitwise OR ('|').
 *
 * violent :  TRUE or FALSE, depending on if this is considered a violent
 * spell and should not be focus in PEACEFUL rooms or on yourself.  Should be
 * set on any spell that inflicts damage, is considered aggressive (i.e.
 * MAJIN, curse), or is otherwise nasty.
 *
 * routines:  A list of magic routines which are associated with this spell
 * if the spell uses spell templates.  Also joined with bitwise OR ('|').
 *
 * See the CircleMUD documentation for a more detailed description of these
 * fields.
 */

/*
 * NOTE: SPELL LEVELS ARE NO LONGER ASSIGNED HERE AS OF Circle 3.0 bpl9.
 * In order to make this cleaner, as well as to make adding new classes
 * much easier, spell levels are now assigned in class.c.  You only need
 * a spello() call to define a new spell; to decide who gets to use a spell
 * or skill, look in class.c.  -JE 5 Feb 1996
 */

void mag_assign_spells(void)
{
  int i;

  /* Do not change the loop below */
  for (i = 1; i <= TOP_SPELL_DEFINE; i++)
    unused_spell(i);
  /* Do not change the loop above */

  spello(SPELL_ANIMATE_DEAD, 35, 10, 3, POS_STANDING,
	TAR_OBJ_ROOM, FALSE, MAG_SUMMONS);

  spello(SPELL_armor, 30, 15, 3, POS_FIGHTING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_TSURUGI, 95, 72, 3, POS_FIGHTING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);
 
  spello(SPELL_MAGIC, 95, 72, 3, POS_FIGHTING,
        TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_MUTATE, 125, 100, 3, POS_FIGHTING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_PLATED, 125, 100, 3, POS_FIGHTING,
        TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);
  
  spello(SPELL_FRENZY, 125, 100, 3, POS_FIGHTING,
        TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_CALM, 125, 100, 3, POS_FIGHTING,
        TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_MUTATE, 125, 100, 3, POS_FIGHTING,
        TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_KYODAIKA, 105, 83, 3, POS_FIGHTING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);
 
  spello(SPELL_BLADEMORPH, 105, 83, 3, POS_FIGHTING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_STONED, 65, 25, 3, POS_STANDING,
        TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS | MAG_ALTER_OBJS);

  spello(SPELL_ZANZOKEN, 65, 25, 3, POS_STANDING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS | MAG_ALTER_OBJS);

  spello(SPELL_FOCUS, 65, 25, 3, POS_STANDING,
        TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS | MAG_ALTER_OBJS);

  spello(SPELL_HASSHUKEN, 65, 25, 3, POS_STANDING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS | MAG_ALTER_OBJS);

  spello(SPELL_SOLAR_FLARE, 35, 25, 1, POS_STANDING,
	TAR_CHAR_ROOM | TAR_NOT_SELF, FALSE, MAG_AFFECTS);

  spello(SPELL_MAJIN, 75, 50, 2, POS_STANDING,
	TAR_CHAR_ROOM | TAR_NOT_SELF, TRUE, MAG_MANUAL);

  spello(SPELL_MULTIFORM, 300, 210, 5, POS_STANDING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_SUMMONS);
  
  spello(SPELL_REPRODUCE, 250, 190, 5, POS_STANDING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_SUMMONS);
  
  spello(SPELL_PORTAL, 50, 50, 0, POS_STANDING,
         TAR_CHAR_WORLD | TAR_NOT_SELF, FALSE, MAG_MANUAL);

  spello(SPELL_SAIBA, 300, 210, 5, POS_STANDING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_SUMMONS);

  spello(SPELL_CONTROL_WEATHER, 75, 25, 5, POS_STANDING,
	TAR_IGNORE, FALSE, MAG_MANUAL);

  spello(SPELL_CREATE_FOOD, 30, 5, 4, POS_STANDING,
	TAR_IGNORE, FALSE, MAG_CREATIONS);
 
  spello(SPELL_FORMWRIST, 30, 5, 4, POS_STANDING,
        TAR_IGNORE, FALSE, MAG_CREATIONS);

  spello(SPELL_FORMPANTS, 30, 5, 4, POS_STANDING,
        TAR_IGNORE, FALSE, MAG_CREATIONS);

  spello(SPELL_CREATE_WATER, 30, 5, 4, POS_STANDING,
	TAR_OBJ_INV | TAR_OBJ_EQUIP, FALSE, MAG_MANUAL);

  spello(SPELL_CURE_SOLAR_FLARE, 30, 5, 2, POS_STANDING,
	TAR_CHAR_ROOM, FALSE, MAG_UNAFFECTS);

  spello(SPELL_CURE_CRITIC, 30, 10, 2, POS_FIGHTING,
	TAR_CHAR_ROOM, FALSE, MAG_POINTS);

  spello(SPELL_CURSE, 80, 50, 2, POS_STANDING,
	TAR_CHAR_ROOM | TAR_OBJ_INV, TRUE, MAG_AFFECTS | MAG_ALTER_OBJS);

  spello(SPELL_DETECT_ALIGN, 20, 10, 2, POS_STANDING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_DETECT_INVISIBILTY, 20, 10, 2, POS_STANDING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_DETECT_MAGIC, 20, 10, 2, POS_STANDING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_DETECT_POISON, 15, 5, 1, POS_STANDING,
	TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_ROOM, FALSE, MAG_MANUAL);

  spello(SPELL_DISPEL_GOOD, 40, 25, 3, POS_FIGHTING,
	TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_DAMAGE);

  spello(SPELL_BURNING_SOULS, 150, 100, 10, POS_STANDING,
	TAR_OBJ_INV | TAR_OBJ_EQUIP, FALSE, MAG_MANUAL);

  spello(SPELL_GROUP_BOOST, 50, 30, 2, POS_STANDING,
        TAR_IGNORE, FALSE, MAG_GROUPS);
  
  spello(SPELL_GROUP_INVISIBLE, 50, 30, 2, POS_STANDING,
        TAR_IGNORE, FALSE, MAG_GROUPS);

  spello(SPELL_LOCATE_OBJECT, 25, 20, 1, POS_STANDING,
        TAR_OBJ_WORLD, FALSE, MAG_MANUAL);

  spello(SPELL_GROUP_armor, 50, 30, 2, POS_STANDING,
	TAR_IGNORE, FALSE, MAG_GROUPS);

  spello(SPELL_GROUP_HEAL, 80, 60, 5, POS_FIGHTING,
	TAR_IGNORE, FALSE, MAG_GROUPS);
    
  spello(SPELL_SWORD, 30, 5, 4, POS_STANDING,
        TAR_IGNORE, FALSE, MAG_CREATIONS);

  spello(SPELL_STASIS, 30, 5, 4, POS_STANDING,
        TAR_IGNORE, FALSE, MAG_CREATIONS);

  spello(SPELL_SPEAR, 30, 5, 4, POS_STANDING,
        TAR_IGNORE, FALSE, MAG_CREATIONS);
  
  spello(SPELL_BUILDNANO, 30, 5, 4, POS_STANDING,
        TAR_IGNORE, FALSE, MAG_CREATIONS);

  spello(SPELL_CANDYBEAM, 30, 5, 4, POS_STANDING,
        TAR_IGNORE, FALSE, MAG_CREATIONS);

  spello(SPELL_BUILDSHIELD, 30, 5, 4, POS_STANDING,
        TAR_IGNORE, FALSE, MAG_CREATIONS);

  spello(SPELL_BOOST, 60, 40, 3, POS_FIGHTING,
        TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_UNAFFECTS);
 
  spello(SPELL_HEAL, 60, 40, 3, POS_FIGHTING,
	TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_UNAFFECTS);
  
  spello(SPELL_LIMBREGO, 60, 40, 3, POS_STANDING,
        TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_UNAFFECTS);

  spello(SPELL_RESTORE, 60, 40, 3, POS_FIGHTING,
        TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_UNAFFECTS);

  spello(SPELL_SENZU, 60, 40, 3, POS_FIGHTING,
	TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_UNAFFECTS);
  
  spello(SPELL_LIMB, 60, 40, 3, POS_FIGHTING,
        TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_UNAFFECTS);

  spello(SPELL_NANO, 60, 40, 3, POS_FIGHTING,
        TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_UNAFFECTS);

  spello(SPELL_HOLYWATER, 60, 40, 3, POS_FIGHTING,
        TAR_CHAR_ROOM, FALSE, MAG_POINTS | MAG_UNAFFECTS);  
 
  spello(SPELL_INFRAVISION, 25, 10, 1, POS_STANDING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_IREG, 25, 10, 1, POS_STANDING,
        TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_INVISIBLE, 35, 25, 1, POS_STANDING,
	TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_ROOM, FALSE, MAG_AFFECTS | MAG_ALTER_OBJS);

  spello(SPELL_POISON, 50, 20, 3, POS_STANDING,
	TAR_CHAR_ROOM | TAR_NOT_SELF | TAR_OBJ_INV, TRUE, MAG_AFFECTS | MAG_ALTER_OBJS);

  spello(SPELL_PROT_FROM_EVIL, 40, 10, 3, POS_STANDING,
	TAR_CHAR_ROOM , FALSE, MAG_AFFECTS);
	
  spello(SPELL_REMOVE_CURSE, 45, 25, 5, POS_STANDING,
	TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_EQUIP, FALSE,
	MAG_UNAFFECTS | MAG_ALTER_OBJS);

  spello(SPELL_BARRIER, 110, 85, 5, POS_STANDING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_YOIKOMINMINKEN, 40, 25, 5, POS_STANDING,
	TAR_CHAR_ROOM, TRUE, MAG_AFFECTS);

  spello(SPELL_SUMMON, 75, 50, 3, POS_STANDING,
	TAR_CHAR_WORLD | TAR_NOT_SELF, FALSE, MAG_MANUAL);

  spello(SPELL_TELEPORT, 75, 50, 3, POS_STANDING,
	TAR_CHAR_ROOM, FALSE, MAG_MANUAL);

  spello(SPELL_WORD_OF_RECALL, 20, 10, 2, POS_FIGHTING,
	TAR_CHAR_ROOM, FALSE, MAG_MANUAL);

  spello(SPELL_REMOVE_POISON, 40, 8, 4, POS_STANDING,
	TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_ROOM, FALSE, MAG_UNAFFECTS | MAG_ALTER_OBJS);

  spello(SPELL_SENSE_LIFE, 20, 10, 2, POS_STANDING,
	TAR_CHAR_ROOM | TAR_SELF_ONLY, FALSE, MAG_AFFECTS);

  spello(SPELL_STONE_SPIT, 250, 30, 2, POS_FIGHTING,
                  TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_AFFECTS);
	
  spello(SPELL_MIND_FREEZE, 250, 30, 2, POS_FIGHTING,
		  TAR_CHAR_ROOM | TAR_FIGHT_VICT, TRUE, MAG_AFFECTS);


  /* NON-focusable spells should appear here */
  spello(SPELL_IDENTIFY, 0, 0, 0, 0,
	TAR_CHAR_ROOM | TAR_OBJ_INV | TAR_OBJ_ROOM, FALSE, MAG_MANUAL);
  
  spello(SPELL_POWER_SENSE, 0, 0, 0, 0,
	TAR_CHAR_ROOM, FALSE, MAG_MANUAL);

  /*
   * Declaration of skills - this actually doesn't do anything except
   * set it up so that immortals can use these skills by default.  The
   * min level to use the skill for other classes is set up in class.c.
   */

  skillo(SKILL_BACKSTAB);
  skillo(SKILL_SWEEP);
  skillo(SKILL_HIDE);
  skillo(SKILL_PICK_LOCK);
  skillo(SKILL_HEEL);
  skillo(SKILL_RESCUE);
  skillo(SKILL_SWIFTNESS);
  skillo(SKILL_MUG);
  skillo(SKILL_SENSE);
  skillo(SKILL_TAILWHIP);
  skillo(SKILL_REPAIR);
  skillo(SKILL_KAIOKEN);
  skillo(SKILL_COSMIC);
  skillo(SKILL_INGEST);
  skillo(SKILL_ABSORB);
  skillo(SKILL_DESTRUCT);
  skillo(SKILL_POWERSENSE);
  skillo(SKILL_STEALTH);

}
