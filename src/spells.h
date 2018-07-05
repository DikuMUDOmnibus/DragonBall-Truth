



/* ************************************************************************
*   File: spells.h                                      Part of CircleMUD *
*  Usage: header file: constants and fn prototypes for spell system       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define DEFAULT_STAFF_LVL	12
#define DEFAULT_SCOUTER_LVL	12

#define focus_UNDEFINED	-1
#define focus_SPELL	0
#define focus_SAIBAKIT	1
#define focus_SCOUTER	2
#define focus_STAFF	3
#define focus_SENZU	4
#define focus_NANO      5
#define focus_HOLY      6
#define focus_LIMB      7
#define focus_STONED    8

#define MAG_DAMAGE	(1 << 0)
#define MAG_AFFECTS	(1 << 1)
#define MAG_UNAFFECTS	(1 << 2)
#define MAG_POINTS	(1 << 3)
#define MAG_ALTER_OBJS	(1 << 4)
#define MAG_GROUPS	(1 << 5)
#define MAG_MASSES	(1 << 6)
#define MAG_AREAS	(1 << 7)
#define MAG_SUMMONS	(1 << 8)
#define MAG_CREATIONS	(1 << 9)
#define MAG_MANUAL	(1 << 10)


#define TYPE_UNDEFINED               -1
#define SPELL_RESERVED_DBC            0  /* SKILL NUMBER ZERO -- RESERVED */

/* PLAYER SPELLS -- Numbered from 1 to MAX_SPELLS */

#define SPELL_armor                   1 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_TELEPORT                2 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_ZANZOKEN                3 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SOLAR_FLARE             4 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_MAJIN                   7 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_HOLYWATER               8
#define SPELL_MULTIFORM               9 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CONTROL_WEATHER        11 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CREATE_FOOD            12 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CREATE_WATER           13 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURE_SOLAR_FLARE       14 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURE_CRITIC            15 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_HASSHUKEN              16 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_CURSE                  17 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_ALIGN           18 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_INVISIBILTY     19 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_MAGIC           20 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DETECT_POISON          21 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_BURNING_SOULS          24 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SAIBA                  26 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_HEAL                   28 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_INVISIBLE              29 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_LOCATE_OBJECT          31 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_POISON                 33 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_PROT_FROM_EVIL         34 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_REMOVE_CURSE           35 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_BARRIER                36 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_YOIKOMINMINKEN         38 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SENZU                  39 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SUMMON                 40 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_LIMB                   41 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_WORD_OF_RECALL         42 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_REMOVE_POISON          43 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_SENSE_LIFE             44 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_ANIMATE_DEAD	     45 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_DISPEL_GOOD	     46 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_GROUP_armor	     47 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_GROUP_HEAL	     48 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_GROUP_RECALL	     49 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_INFRAVISION	     50 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_WATERWALK		     51 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_MIND_FREEZE	     52 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_KI_SHOT                53 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_TSURUGI                59 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_REPRODUCE              60 /* Reserved Skill[] DO NOT CHANGE */
#define SPELL_KYODAIKA               61 /* Super namek! */
#define SPELL_BLADEMORPH             62 /* Turn hands to blades */
#define SPELL_MUTATE                 63 /* Turn skin defensive */
#define SPELL_SWORD                  64 /* Turn skin defensive */

/* Insert new spells here, up to MAX_SPELLS */
#define SPELL_SPEAR                  65
#define SPELL_BUILDSHIELD            66
#define SPELL_NANO                   67
#define SPELL_CANDYBEAM              68
#define SPELL_STONE_SPIT             69
#define SPELL_BUILDNANO              70
#define SPELL_GROUP_INVISIBLE        71
#define SPELL_BOOST                  72
#define SPELL_GROUP_BOOST            73
#define SPELL_PORTAL                 74
#define SPELL_MAGIC                  75
#define SPELL_RESTORE                76
#define SPELL_STONED                 77
#define SPELL_FORMWRIST              78
#define SPELL_FORMPANTS              79
#define SPELL_PLATED                 80
#define SPELL_DETECT                 81
#define SPELL_RAGE                   82
#define SPELL_BLOCK                  83
#define SPELL_DODGE                  84
#define SPELL_ADEF                   85
#define SPELL_FRENZY                 86
#define SPELL_VITAL                  87
#define SPELL_CALM                   88
#define SPELL_LIMBREGO               89
#define SPELL_IREG                   90
#define SPELL_PBOMB                  91
#define SPELL_STASIS                 92
#define SPELL_MOONB                  93
#define SPELL_FOCUS                  94
#define SPELL_BLUST                  95
#define SPELL_FLOOD                  96
#define SPELL_ROGA                   97
#define SPELL_FISHING                98

#define MAX_SPELLS		    130

/* PLAYER SKILLS - Numbered from MAX_SPELLS+1 to MAX_SKILLS */
#define SKILL_BACKSTAB              131 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_SWEEP                 132 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_HIDE                  133 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_ZANELB                134 /* Zanzoken Elbow */
#define SKILL_PICK_LOCK             135 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_HEEL                 136 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_RESCUE                137 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_SWIFTNESS             138 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_MUG                   139 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_SENSE		    140 /* Reserved Skill[] DO NOT CHANGE */
#define SKILL_TAILWHIP		    141 /* Icers hit with tail            */
#define SKILL_HEAVEN                142
#define SKILL_REPAIR                143 /* Androids repair themselves     */
#define SKILL_KAIOKEN               144 /* Self Explanitory               */
#define SKILL_KNEE                  145 /* Self Explanitory               */
#define SKILL_ELBOW                 146 /* Self Explanitory               */
#define SKILL_ROUNDHOUSE            147 /* Self Explanitory               */
#define SKILL_COSMIC                148 /* Self Explanitory               */
#define SKILL_HYDRO                 149 /* Self Explanitory               */
#define SKILL_INGEST                150 /* Self Explanitory               */
#define SKILL_ECLIPSE               151 /* Self Explanitory               */
#define SKILL_PHEONIX               152 /* Self Explanitory               */
#define SKILL_ABSORB                153 /* Self Explanitory               */
#define SKILL_DESTRUCT              154 /* Self Explanitory               */
#define SKILL_POWERSENSE            155 /* Self Explanitory               */
#define SKILL_STEALTH               156 /* Self Explanitory               */
#define SKILL_REGENERATE            157 /* Self Explanitory               */
#define SKILL_KAMEHAMEHA            158 /* Saiyans and Half Ki attack   */
#define SKILL_BIGBANG               159/*Saiyans and Half Ki attack */
#define SKILL_FINALFLASH            160/*Saiyans and half ki attack */  
#define SKILL_CHOUKAME              161/*Saiyans and Half Ki attack */
#define SKILL_DISK                  162/*Humans Ki attack */
#define SKILL_MASENKO               163/*Half Breeds attack?*/
#define SKILL_RENZO                 164
#define SKILL_MAKANKO               165
#define SKILL_KISHOT                166
#define SKILL_KIKOHO                167
#define SKILL_DBALL                 168
#define SKILL_HFLASH                169
#define SKILL_KOUSEN                170
#define SKILL_YOIKOMINMINKEN        171
/* Define new Skills here, up to 200 */
#define SKILL_BURNINGATTACK         172
#define SKILL_SPIRITBALL            173
#define SKILL_SEVENDRAGON           174
#define SKILL_TWIN                  175
#define SKILL_KAKUSANHA             176
#define SKILL_HELLSPEAR             177
#define SKILL_GIGABEAM              178
#define SKILL_MEDITATE              179
#define SKILL_BEAM                  180
#define SKILL_TSUIHIDAN             181
#define SKILL_SHOGEKIHA             182
#define SKILL_BBK                   183
#define SKILL_HAVOC                 184
#define SKILL_GALIKGUN              185
#define SKILL_HONOO                 186
#define SKILL_BAKU                  187
#define SKILL_CRUSHER               188
#define SKILL_ERASER                189
#define SKILL_KYODAIKA              190
#define SKILL_SCATTER               191
#define SKILL_GENKI                 192
#define SKILL_FORELOCK              193
#define SKILL_CHIKYUU               194
#define SKILL_GENOCIDE              195
#define SKILL_DEFLECT               196
#define SKILL_POTENTIAL             197
#define SKILL_MAJIN                 198
#define SKILL_MINDREAD              199
#define SKILL_DUALWIELD             200
/*
 *  NON-PLAYER AND OBJECT SPELLS AND SKILLS
 *  The practice levels for the spells and skills below are _not_ recorded
 *  in the playerfile; therefore, the intended use is for spells and skills
 *  associated with objects (such as SPELL_IDENTIFY used with SENZUs of
 *  IDENTIFY) or non-players (such as NPC-only spells).
 */
#define SPELL_POWER_SENSE            201
#define SPELL_IDENTIFY               220
#define SPELL_FIRE_BREATH            221
#define SPELL_GAS_BREATH             222
#define SPELL_FROST_BREATH           223
#define SPELL_ACID_BREATH            224
#define SPELL_LIGHTNING_BREATH       225
#define SKILL_DBALL2                 226

#define TOP_SPELL_DEFINE	     299
/* NEW NPC/OBJECT SPELLS can be inserted here up to 299 */

/* WEAPON ATTACK TYPES */

#define TYPE_HIT                     300
#define TYPE_STING                   301
#define TYPE_WHIP                    302
#define TYPE_SLASH                   303
#define TYPE_BITE                    304
#define TYPE_BLUDGEON                305
#define TYPE_CRUSH                   306
#define TYPE_POUND                   307
#define TYPE_CLAW                    308
#define TYPE_MAUL                    309
#define TYPE_THRASH                  310
#define TYPE_PIERCE                  311
#define TYPE_BLAST		     312
#define TYPE_PUNCH		     313
#define TYPE_STAB		     314

/* new attack types can be added here - up to TYPE_SUFFERING */
#define TYPE_SUFFERING		     399



#define SAVING_PARA   0
#define SAVING_ROD    1
#define SAVING_PETRI  2
#define SAVING_BREATH 3
#define SAVING_SPELL  4


#define TAR_IGNORE        1
#define TAR_CHAR_ROOM     2
#define TAR_CHAR_WORLD    4
#define TAR_FIGHT_SELF    8
#define TAR_FIGHT_VICT   16
#define TAR_SELF_ONLY    32 /* Only a check, use with i.e. TAR_CHAR_ROOM */
#define TAR_NOT_SELF     64 /* Only a check, use with i.e. TAR_CHAR_ROOM */
#define TAR_OBJ_INV     128
#define TAR_OBJ_ROOM    256
#define TAR_OBJ_WORLD   512
#define TAR_OBJ_EQUIP  1024

struct spell_info_type {
   byte min_position;	/* Position for focuser	 */
   int mana_min;	/* Min amount of mana used by a spell (highest lev) */
   int mana_max;	/* Max amount of mana used by a spell (lowest lev) */
   int mana_change;	/* Change in mana used by spell from lev to lev */

   int min_level[NUM_CLASSES];
   int routines;
   byte violent;
   int targets;         /* See below for use with TAR_XXX  */
};

/* Possible Targets:

   bit 0 : IGNORE TARGET
   bit 1 : PC/NPC in room
   bit 2 : PC/NPC in world
   bit 3 : Object held
   bit 4 : Object in inventory
   bit 5 : Object in room
   bit 6 : Object in world
   bit 7 : If fighting, and no argument, select tar_char as self
   bit 8 : If fighting, and no argument, select tar_char as victim (fighting)
   bit 9 : If no argument, select self, if argument check that it IS self.

*/

#define SPELL_TYPE_SPELL   0
#define SPELL_TYPE_SAIBAKIT  1
#define SPELL_TYPE_SCOUTER    2
#define SPELL_TYPE_STAFF   3
#define SPELL_TYPE_SENZU  4


/* Attacktypes with grammar */

struct attack_hit_type {
   const char	*singular;
   const char	*plural;
};



#define ASPELL(spellname) \
void	spellname(int level, struct char_data *ch, \
		  struct char_data *victim, struct obj_data *obj)

#define MANUAL_SPELL(spellname)	spellname(level, focuser, cvict, ovict);

ASPELL(spell_create_water);
ASPELL(spell_recall);
ASPELL(spell_teleport);
ASPELL(spell_summon);
ASPELL(spell_locate_object);
ASPELL(spell_MAJIN);
ASPELL(spell_information);
ASPELL(spell_IDENTIFY);
ASPELL(spell_POWER_SENSE);
ASPELL(spell_BURNING_SOULS);
ASPELL(spell_detect_poison);

/* basic magic calling functions */

int find_skill_num(char *name);

int mag_damage(int level, struct char_data *ch, struct char_data *victim,
  int spellnum, int savetype);

void mag_affects(int level, struct char_data *ch, struct char_data *victim,
  int spellnum, int savetype);

void mag_groups(int level, struct char_data *ch, int spellnum, int savetype);

void mag_masses(int level, struct char_data *ch, int spellnum, int savetype);

void mag_areas(int level, struct char_data *ch, int spellnum, int savetype);

void mag_summons(int level, struct char_data *ch, struct obj_data *obj,
 int spellnum, int savetype);

void mag_points(int level, struct char_data *ch, struct char_data *victim,
 int spellnum, int savetype);

void mag_unaffects(int level, struct char_data *ch, struct char_data *victim,
  int spellnum, int type);

void mag_alter_objs(int level, struct char_data *ch, struct obj_data *obj,
  int spellnum, int type);

void mag_creations(int level, struct char_data *ch, int spellnum);

int	call_magic(struct char_data *focuser, struct char_data *cvict,
  struct obj_data *ovict, int spellnum, int level, int focustype);

void	mag_objectmagic(struct char_data *ch, struct obj_data *obj,
			char *argument);

int	focus_spell(struct char_data *ch, struct char_data *tch,
  struct obj_data *tobj, int spellnum);


/* other prototypes */
void spell_level(int spell, int chclass, long long level);
void init_spell_levels(void);
const char *skill_name(int num);


