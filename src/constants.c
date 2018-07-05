



/* ************************************************************************
*   File: constants.c                                   Part of CircleMUD *
*  Usage: Numeric and string contants used by the MUD                     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"

cpp_extern const char circlemud_version[] = {
	"CircleMUD, version 3.00 beta patchlevel 14"
};


/* strings corresponding to ordinals/bitvectors in structs.h ***********/


/* (Note: strings for class definitions in class.c instead of here) */


/* cardinal directions */
const char *dirs[] =
{
  "North",
  "East",
  "South",
  "West",
  "Up",
  "Down",
  "\n"
};


/* ROOM_x */
const char *room_bits[] = {
  "DARK",
  "DEATH",
  "!MOB",
  "INDOORS",
  "PEACEFUL",
  "SOUNDPROOF",
  "!SENSE",
  "!MAGIC",
  "1CHARONLY",
  "PRIVATE",
  "GODROOM",
  "HOUSE",
  "HCRSH",
  "ATRIUM",
  "OLC",
  "*",				/* BFS MARK */
  "REGEN",
  "GravityX10",
  "GravityX100",
  "GravityX200",
  "GravityX300",
  "GravityX400",
  "Spirit&Time",
  "Trainer Room",
  "Dojo Spar Room",
  "UNUSED",
  "Drain",
  "Shore",
  "HTANK",
  "No QuitRecall",
  "UNUSED",
  "\n"
};


/* EX_x */
const char *exit_bits[] = {
  "DOOR",
  "CLOSED",
  "LOCKED",
  "PICKPROOF",
  "\n"
};


/* SECT_ */
const char *sector_types[] = {
  "Inside",
  "City",
  "Field",
  "Forest",
  "Hills",
  "Mountains",
  "Water (Swim)",
  "Water (No Swim)",
  "Underwater",
  "In Flight",
  "EARTH",
  "VEGETA",
  "FRIGID",
  "KONACK",
  "ORAINAS",
  "NAMEK",
  "\n"
};

/*
 * SEX_x
 * Not used in sprinttype() so no \n.
 */
const char *genders[] =
{
  "&10neutral&00",
  "&12male&00",
  "&13female&00"
};

const char *hc[] =
{
  "&15white&00",
  "black",
  "&12blue&00",
  "&10green&00",
  "&03brown&00",
  "&13purple&00",
  "&09red&00",
  "&11gold&00",
  "",
  "\n"
};

const char *eyes[] =
{
  "&15white&00",
  "black",
  "&12blue&00",
  "&10green&00",
  "&03brown&00",
  "&13purple&00",
  "&09red&00",
  "&11yellow&00",
  "&14icy blue&00",
  "\n"
};

const char *hl[] =
{
  "very short",
  "short",
  "medium",
  "long",
  "very long",
  "no",
  "short spikey",
  "medium spikey",
  "long spikey",
  "\n"
};

const char *aura[] =
{
  "black",
  "&15white&00",
  "&12blue&00",
  "&10green&00",
  "&03brown&00",
  "&09red&00",
  "&11golden&00",
  "\n"
};

const char *build[] =
{
  "&15skinny&00",
  "&15lean&00",
  "&15muscular&00",
  "&15chubby&00",
  "&15fat&00",
  "\n"
};

const char *god[] =
{
  "&09Tuskino&00",
  "&14Raiden&00",
  "&13Rain&00",
  "&11Erdrick&00",
  "&10Aeana&00",
  "&12Hiro&00",
  "No One",
  "\n"
};
const char *clan[] =
{
  "&09Heart &11of the &16Sword!&00",
};


/* POS_x */
const char *position_types[] = {
  "Dead",
  "Mortally wounded",
  "Incapacitated",
  "Stunned",
  "Sleeping",
  "Resting",
  "Sitting",
  "Fighting",
  "Standing",
  "Floating",
  "\n"
};


/* PLR_x */
const char *player_bits[] = {
  "KILLER",
  "icer",
  "FROZEN",
  "DONTSET",
  "WRITING",
  "MAILING",
  "CSH",
  "SITEOK",
  "NOSHOUT",
  "NOTITLE",
  "DELETED",
  "LOADRM",
  "!WIZL",
  "!DEL",
  "INVST",
  "CRYO",
  "LARM",
  "RARM",
  "LLEG",
  "RLEG",
  "S-TAIL",
  "TAIL",
  "TRANS1",
  "TRANS2",
  "TRANS3",
  "TRANS4",
  "TRANS5",
  "TRANS6",
  "FORELOCK",
  "DEFLECT",
  "MAJINIZED",
  "NOMAJIN",
  "BOUNTY",
  "\n"
};


/* MOB_x */
const char *action_bits[] = {
  "SPEC",
  "SENTINEL",
  "SCAVENGER",
  "ISNPC",
  "AWARE",
  "AGGR",
  "STAY-ZONE",
  "WIMPY",
  "AGGR_EVIL",
  "AGGR_GOOD",
  "AGGR_NEUTRAL",
  "MEMORY",
  "HELPER",
  "!MAJIN",
  "!SUMMN",
  "!SLEEP",
  "!sweep",
  "!SOLAR_FLARE",
  "\n"
};


/* PRF_x */
const char *preference_bits[] = {
  "BRIEF",
  "COMPACT",
  "DEAF",
  "!TELL",
  "D_HP",
  "D_MANA",
  "D_MOVE",
  "AUTOEX",
  "!HASS",
  "QUEST",
  "SUMN",
  "!REP",
  "LIGHT",
  "C1",
  "C2",
  "!WIZ",
  "L1",
  "L2",
  "!OOC",
  "!ROLE",
  "!GTZ",
  "RMFLG",
  "AFK",
  "CAMP",
  "OOZARU",
  "MYSTIC",
  "MYSTIC2",
  "MMYSTIC3",  
  "AUTOLOOT",
  "FLYING",
  "TRAINING",
  "AUTOSPLIT",
  "\n"
};

const char *preference2_bits[]={

  "",
  "trans1",
  "trans2",
  "trans3",
  "earth",
  "frigid",
  "vegeta",
  "orainas",
  "konack",
  "namek",
  "mugg",
  "head",
  "G.Oozaru",
  "FISHING",
  "FISH1",
  "FISH2",
  "FISH3",
  "FISH4",
  "CAUGHT",
  "REELING",
  "\n"
};
const char *cln_bits[]={
  "leader1",
  "leader2",
  "leader3",
  "leader4",
  "leader5",
  "leader6",
  "leader7",
  "leader8",
  "member1",
  "member2",
  "member3",
  "member4",
  "member5",
  "member6",
  "member7",
  "member8",
  "Newbie",
  "Member",
  "Adept",
  "Captain",
  "Dead",
  "RAGE",
  "Block",
  "Dodge",
  "Adef",
  "",
  "",
  "",
  "\n"
};

/* AFF_x */
const char *affected_bits[] =
{
  "SOLAR_FLARE",
  "INVIS",
  "DET-ALIGN",
  "DET-INVIS",
  "DET-MAGIC",
  "SENSE-LIFE",
  "WATWALK",
  "SANCT",
  "GROUP",
  "CURSE",
  "INFRA",
  "POISON",
  "PROT-EVIL",
  "PROT-GOOD",
  "SLEEP",
  "!SENSE",
  "UNUSED",
  "UNUSED",
  "SWIFTNESS",
  "HIDE",
  "UNUSED",
  "MAJIN",
  "POWERUP",
  "MIND_FREEZE",
  "STONED",
  "\n"
};

/* CON_x */
const char *connected_types[] = {
  "Playing",
  "Disconnecting",
  "Get name",
  "Confirm name",
  "Get password",
  "Get new PW",
  "Confirm new PW",
  "Select sex",
  "Select Race",
  "Reading MOTD",
  "Main Menu",
  "Get descript.",
  "Changing PW 1",
  "Changing PW 2",
  "Changing PW 3",
  "Self-Delete 1",
  "Self-Delete 2",
  "Disconnecting",
  "Object editor",
  "Room editor",
  "Zone editor",
  "Mobile editor",
  "Shop editor",
  "Trigger editor",
  "Help editor",
  "Action editor",
  "Text editor",
  "Select Eye Color.",
  "Select Hair Color.",
  "Select Hair Length.",
  "Select Aura Color.",
  "Select Build.",
  "\n"
};


/*
 * WEAR_x - for eq list
 * Not use in sprinttype() so no \n.
 */
const char *where[] = {
  "&09[&12Used as &15Light&09]           &00",
  "&09[&12Worn on &15Finger&09]          &00",
  "&09[&12Worn on &15Finger&09]          &00",
  "&09[&12Worn around &15Neck&09]        &00",
  "&09[&12Worn around &15Neck&09]        &00",
  "&09[&12Worn on &15Body&09]            &00",
  "&09[&12Worn on &15Head&09]            &00",
  "&09[&12Worn on &15Legs&09]            &00",
  "&09[&12Worn on &15Feet&09]            &00",
  "&09[&12Worn on &15Hands&09]           &00",
  "&09[&12Worn on &15Arms&09]            &00",
  "&09[&12Worn on &15Eye&09]             &00",
  "&09[&12Worn about &15Body&09]         &00",
  "&09[&12Worn about &15Waist&09]        &00",
  "&09[&12Worn around &15Wrist&09]       &00",
  "&09[&12Worn around &15Wrist&09]       &00",
  "&09[&12&15Wielded&09]                 &00",
  "&09[&12&15Held&09]                    &00",
  "&09[&15Floating &12Over Head&09]      &00",
  "&09[&12Worn on &15Tail&09]            &00",
  "&09[&12Worn on &15Left Ear&09]        &00",
  "&09[&12Worn on &15Right Ear&09]       &00",
  "&09[&12Bored Into &15Forehead&09]     &00",
  "&09[&12Worn on &15Shoulder&09]        &00",
  "&09[&12Worn on &15Forelock&09]        &00",
  "&09[&12Dual &15Wielded&09]            &00",

};


/* WEAR_x - for stat */
const char *equipment_types[] = {
  "Used as light",
  "Worn on right finger",
  "Worn on left finger",
  "First worn around Neck",
  "Second worn around Neck",
  "Worn on body",
  "Worn on head",
  "Worn on legs",
  "Worn on feet",
  "Worn on hands",
  "Worn on arms",
  "Worn as shield",
  "Worn about body",
  "Worn around waist",
  "Worn around right wrist",
  "Worn around left wrist",
  "Wielded",
  "Held",
  "Floating Over Head",
  "Tail",
  "Worn on Left Ear",
  "Worn on Right Ear",
  "In Forehead",
  "Clan Insignia",
  "Forelock",
  "Dual wield",
  "\n"
};


/* ITEM_x (ordinal object types) */
const char *item_types[] = {
  "UNDEFINED",
  "LIGHT",
  "SENZU",
  "SCOUTER",
  "STAFF",
  "WEAPON",
  "FLOOD",
  "MISSILE",
  "TREASURE",
  "ARMOR",
  "SAIBAKIT",
  "WORN",
  "OTHER",
  "TRASH",
  "TRAP",
  "CONTAINER",
  "NOTE",
  "LIQ CONTAINER",
  "KEY",
  "FOOD",
  "MONEY",
  "PEN",
  "HOLYWATER",
  "FOUNTAIN",
  "Nano",
  "dball1",
  "dball2",
  "dball3",
  "dball4",
  "dball5",
  "dball6",
  "dball7",
  "\n"
};


/* ITEM_WEAR_ (wear bitvector) */
const char *wear_bits[] = {
  "TAKE",
  "FINGER",
  "NECK",
  "BODY",
  "HEAD",
  "LEGS",
  "FEET",
  "HANDS",
  "ARMS",
  "EYE",
  "ABOUT",
  "WAIST",
  "WRIST",
  "WIELD",
  "HOLD",
  "FLOAT",
  "TAIL",
  "LEAR",
  "REAR",
  "FORE",
  "CLAN",
  "FORELOCK",
  "DUALWIELD",
  "\n"
};


/* ITEM_x (extra bits) */
const char *extra_bits[] = {
  "GLOW",
  "HUM",
  "!RENT",
  "STASISCELLS",
  "!INVIS",
  "INVISIBLE",
  "ROD",
  "!DROP",
  "ZANZOKEN",
  "!GOOD",
  "!EVIL",
  "!NEUTRAL",
  "!Human",
  "!Namek",
  "!Icer",
  "!Saiyan",
  "!MAJIN",
  "!SELL",
  "!ANDROID",
  "!KAI",
  "!HALF_BREED",
  "!BIODROID",
  "!KONATSU",
  "!DEMON",
  "!TRUFFLE",
  "!MUTANT",
  "!TRUFFLE",
  "Divine",
  "UBER-Divine",
  "ClanFlag",
  "MIDI-Divine",
  "\n"
};


/* APPLY_x */
const char *apply_types[] = {
  "NONE",
  "STR",
  "DEX",
  "INT",
  "WIS",
  "CON",
  "SPEED",
  "CLASS",
  "LEVEL",
  "AGE",
  "CHAR_WEIGHT",
  "CHAR_HEIGHT",
  "MAXKI",
  "MAXPL",
  "MAXFITNESS",
  "GOLD",
  "EXP",
  "ARMOR",
  "HITROLL",
  "DAMROLL",
  "SAVING_PARA",
  "SAVING_ROD",
  "SAVING_PETRI",
  "SAVING_BREATH",
  "SAVING_SPELL",
  "INTUITION",
  "DON'TUSE",
  "DON'TUSE",
  "CLAN",
  "\n"
};


/* CONT_x */
const char *container_bits[] = {
  "CLOSEABLE",
  "PICKPROOF",
  "CLOSED",
  "LOCKED",
  "\n",
};

/* LIQ_x */
const char *drinks[] =
{
  "water",
  "beer",
  "wine",
  "ale",
  "dark ale",
  "whisky",
  "lemonade",
  "firebreather",
  "local speciality",
  "slime mold juice",
  "milk",
  "tea",
  "coffee",
  "cola",
  "salt water",
  "clear water",
  "\n"
};


/* other constants for liquids ******************************************/


/* one-word alias for each drink */
const char *drinknames[] =
{
  "water",
  "beer",
  "wine",
  "ale",
  "ale",
  "whisky",
  "lemonade",
  "firebreather",
  "local",
  "juice",
  "milk",
  "tea",
  "coffee",
  "blood",
  "salt",
  "water",
  "\n"
};


/* effect of drinks on hunger, thirst, and drunkenness -- see values.doc */
int drink_aff[][3] = {
  {0, 1, 10},
  {3, 2, 5},
  {5, 2, 5},
  {2, 2, 5},
  {1, 2, 5},
  {6, 1, 4},
  {0, 1, 8},
  {10, 0, 0},
  {3, 3, 3},
  {0, 4, -8},
  {0, 3, 6},
  {0, 1, 6},
  {0, 1, 6},
  {0, 2, -1},
  {0, 1, -2},
  {0, 0, 13}
};


/* color of the various drinks */
const char *color_liquid[] =
{
  "clear",
  "brown",
  "clear",
  "brown",
  "dark",
  "golden",
  "red",
  "green",
  "clear",
  "light green",
  "white",
  "brown",
  "black",
  "red",
  "clear",
  "crystal clear"
  "\n"
};


/*
 * level of fullness for drink containers
 * Not used in sprinttype() so no \n.
 */
const char *fullness[] =
{
  "less than half ",
  "about half ",
  "more than half ",
  ""
};


/* str, int, wis, dex, con applies **************************************/


/* [ch] strength apply (all) */
cpp_extern const struct str_app_type str_app[] = {
  {-5, -4, 0, 0},	/* str = 0 */
  {-5, -4, 3, 1},	/* str = 1 */
  {-3, -2, 3, 2},
  {-3, -1, 10, 3},
  {-2, -1, 25, 4},
  {-2, -1, 55, 5},	/* str = 5 */
  {-1, 0, 80, 6},
  {-1, 0, 90, 7},
  {0, 0, 100, 8},
  {0, 0, 100, 9},
  {0, 0, 115, 10},	/* str = 10 */
  {0, 0, 115, 11},
  {0, 0, 140, 12},
  {0, 0, 140, 13},
  {0, 0, 170, 14},
  {0, 0, 170, 15},	/* str = 15 */
  {0, 1, 195, 16},
  {1, 1, 220, 18},
  {1, 2, 255, 20},	/* str = 18 */
  {3, 7, 640, 40},
  {3, 8, 700, 40},	/* str = 20 */
  {4, 9, 810, 40},
  {4, 10, 970, 40},
  {5, 11, 1130, 40},
  {6, 12, 1440, 40},
  {7, 14, 1750, 40},	/* str = 25 */
  {1, 3, 280, 22},	/* str = 18/0 - 18-50 */
  {2, 3, 305, 24},	/* str = 18/51 - 18-75 */
  {2, 4, 330, 26},	/* str = 18/76 - 18-90 */
  {2, 5, 380, 28},	/* str = 18/91 - 18-99 */
  {3, 6, 480, 30}	/* str = 18/100 */
};



/* [dex] skill apply (thieves only) */
cpp_extern const struct dex_skill_type dex_app_skill[] = {
  {-99, -99, -90, -99, -60},	/* dex = 0 */
  {-90, -90, -60, -90, -50},	/* dex = 1 */
  {-80, -80, -40, -80, -45},
  {-70, -70, -30, -70, -40},
  {-60, -60, -30, -60, -35},
  {-50, -50, -20, -50, -30},	/* dex = 5 */
  {-40, -40, -20, -40, -25},
  {-30, -30, -15, -30, -20},
  {-20, -20, -15, -20, -15},
  {-15, -10, -10, -20, -10},
  {-10, -5, -10, -15, -5},	/* dex = 10 */
  {-5, 0, -5, -10, 0},
  {0, 0, 0, -5, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},		/* dex = 15 */
  {0, 5, 0, 0, 0},
  {5, 10, 0, 5, 5},
  {10, 15, 5, 10, 10},		/* dex = 18 */
  {15, 20, 10, 15, 15},
  {15, 20, 10, 15, 15},		/* dex = 20 */
  {20, 25, 10, 15, 20},
  {20, 25, 15, 20, 20},
  {25, 25, 15, 20, 20},
  {25, 30, 15, 25, 25},
  {25, 30, 15, 25, 25}		/* dex = 25 */
};



/* [dex] apply (all) */
cpp_extern const struct dex_app_type dex_app[] = {
  {-7, -7, 6},		/* dex = 0 */
  {-6, -6, 5},		/* dex = 1 */
  {-4, -4, 5},
  {-3, -3, 4},
  {-2, -2, 3},
  {-1, -1, 2},		/* dex = 5 */
  {0, 0, 1},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},		/* dex = 10 */
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, -1},		/* dex = 15 */
  {1, 1, -2},
  {2, 2, -3},
  {2, 2, -4},		/* dex = 18 */
  {3, 3, -4},
  {3, 3, -4},		/* dex = 20 */
  {4, 4, -5},
  {4, 4, -5},
  {4, 4, -5},
  {5, 5, -6},
  {5, 5, -6}		/* dex = 25 */
};



/* [con] apply (all) */
cpp_extern const struct con_app_type con_app[] = {
  {1},
  {2},
  {3},
  {4},
  {5},
  {6},
  {8},
  {8},
  {9},
  {10},
  {10},
  {12},
  {13},
  {14},
  {15},
  {20},		/* con = 0 */
  {25},		/* con = 1 */
  {30},
  {35},
  {40},
  {45},		/* con = 5 */
  {50},
  {55},
  {60},
  {65},
  {70},		/* con = 10 */
  {75},
  {80},
  {85},
  {88},
  {90},		/* con = 15 */
  {95},
  {97},
  {99},		/* con = 18 */
  {99},
  {99},		/* con = 20 */
  {100},
  {100},
  {100},
  {100},
  {125},
  {125},
  {125},
  {150},
  {150},
  {150},          	/* con = 30 */
  {175},
  {175},
  {175},
  {200},
  {200},
  {200},
  {200},
  {300},
  {300},
  {300},
  {400},
  {400},
  {400},
  {500},
  {500},
  {500},
  {500},
  {550},
  {550},
  {550}

};



/* [int] apply (all) */
cpp_extern const struct int_app_type int_app[] = {
  {0},
  {0}

};


/* [wis] apply (all) */

const char *spell_wear_off_msg[] = {
  "RESERVED DB.C",		/* 0 */
  "Your ki shield diminishes.",	/* 1 */
  "!Teleport!",
  "&15You slow down.&00",
  "&15The flash wears away from your eyes.&00",
  "!Destructo Disk!",		/* 5 */
  "!Kikoho!",
  "&15You feel more self-confident.&00",
  "!Masenko!",
  "!Multiform!",
  "!Death Ball!",		/* 10 */
  "!Control Weather!",
  "!Create Food!",
  "!Create Water!",
  "!Cure SOLAR_FLARE!",
  "!Cure Critic!",		/* 15 */
  "Your arms slow down.",
  "&15You feel more optimistic.&00",
  "&15You feel less aware.&00",
  "&15Your eyes stop tingling.&00",
  "The detect magic wears off.",/* 20 */
  "The detect poison wears off.",
  "!Dispel Evil!",
  "!Earthquake!",
  "!Enchant Weapon!",
  "!Energy Drain!",		/* 25 */
  "!Fireball!",
  "!Dondopa!",
  "!Heal!",
  "&15You feel yourself exposed.&00",
  "!Eye Beam!",		/* 30 */
  "!Locate object!",
  "!Magic Missile!",
  "&15You feel less sick.&00",
  "&15You feel less protected.&00",
  "!Remove Curse!",		/* 35 */
  "&15The globe of energy around your body fades.&00",
  "!Eraser Cannon!",
  "&15You feel less tired.&00",
  "!saibamen!",
  "!Summon!",			/* 40 */
  "!Ventriloquate!",
  "!Word of Recall!",
  "!Remove Poison!",
  "&15You feel less aware of your suroundings.&00",
  "!Animate Dead!",		/* 45 */
  "!Dispel Good!",
  "!Group Armor!",
  "!Group Heal!",
  "!Group Recall!",
  "&15Your heat vision seems to fade.&00",	/* 50 */
  "Your feet seem less boyant.",
  "&15Your mind eases as you can move again.&00"
  "!Kishot!",
  "!Makankosappo!",
  "!Chou Kamahameha!",
  "!Big Bang attack!",
  "!Spirit Bomb!",
  "!Daichiretsuhan",
  "&15Your ki blades fade into nothing.&00",
  "&15Your ki blades fade into nothing.&00", /* 60 */
  "&15Your muscles tone down and shrink.&00",
  "&15Your claws melt away to normal size.&00",
  "&15Your defensive skin deteriorates.&00",
  "&15Your defensive skin deteriorates.&00",
  "!Hurricane Slash!",
  "!SOMETHING!",
  "!SOMETHING!",
  "!SOMETHING!",
  "!SOMETHING!",
  "!SOMETHING!", /* 70 */
  "!SOMETHING!",
  "!SOMETHING!",
  "!SOMETHING!",
  "!SOMETHING!",
  "!SOMETHING!",
  "&15The enchantment wears off.&00",
  "&15You are no longer stoned, damnit...&00",
  "&15You are no longer stoned, damnit...&00",
  "&15You are no longer stoned, damnit...&00",
  "&15You are no longer stoned, damnit...&00",
  "&15Your bone shells are absorbed back into your skin&00",
  "",
  "",
  "",
  "",
  "",
  "&15Your blood frenzy subsides....&00",
  "",
  "&15Your mental defenses subside.&00",
  "",
  "&15Your metabolism slows down.&00",
  "",
  "&15Your inner focus has worn off.&00",
  "",
  ""
  
};



const char *npc_class_types[] = {
  "Normal",
  "Undead",
  "\n"
};



int rev_dir[] =
{
  2,
  3,
  0,
  1,

#if defined(OASIS_MPROG)
/*
 * Definitions necessary for MobProg support in OasisOLC
 */
const char *mobprog_types[] = {
  "INFILE",
  "ACT",
  "SPEECH",
  "RAND",
  "FIGHT",
  "DEATH",
  "HITPRCNT",
  "ENTRY",
  "GREET",
  "ALL_GREET",
  "GIVE",
  "BRIBE",
  "\n"
};
#endif
  5,
  4
};


int movement_loss[] =
{
  1,	/* Inside     */
  1,	/* City       */
  2,	/* Field      */
  3,	/* Forest     */
  4,	/* Hills      */
  6,	/* Mountains  */
  4,	/* Swimming   */
  1,	/* Unswimable */
  1,	/* Flying     */
  5     /* Underwater */
};

/* Not used in sprinttype(). */
const char *weekdays[] = {
  "the Day of Iovan",
  "the Day of Kuvoc",
  "the Day of Taishyr",
  "the Day of Necrid",
  "the Day of Solbet",
  "the Day of Xyron",
  "the Day of Mhurzin"
};


/* Not used in sprinttype(). */
const char *month_name[] = {
  "Month of Iovan",		/* 0 */
  "Month of Taishyr",
  "Month of Necrid",
  "Month of Xyron",
  "Month of Solbet",
  "Month of Mhurzin",
  "Month of Shigeru",
  "Month of Kuvoc",
  "Month of Iovan's Strength",
  "Month of Taishyr's Pride",
  "Month of Kuvoc's Fury",
  "Month of Solbet's Confidence",
  "Month of Necrid's Wisdom",
  "Month of Xyron's Knowledge",
  "Month of Mhurzins's Understanding",
  "Month of Shigeru's Endurance",
  "Month of the Gods",
};
