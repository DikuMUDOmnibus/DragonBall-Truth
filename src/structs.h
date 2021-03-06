/* ************************************************************************
*   File: structs.h                                     Part of CircleMUD *
*  Usage: header file for central structures and contstants               *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/*
 * Intended use of this macro is to allow external packages to work with
 * a variety of CircleMUD versions without modifications.  For instance,
 * an IS_CORPSE() macro was introduced in pl13.  Any future code add-ons
 * could take into account the CircleMUD version and supply their own
 * definition for the macro if used on an older version of CircleMUD.
 * You are supposed to compare this with the macro CIRCLEMUD_VERSION()
 * in utils.h.  See there for usage.
 */
#define _CIRCLEMUD	0x03000D /* Major/Minor/Patchlevel - MMmmPP */

/* preamble *************************************************************/

#define NOWHERE    -1    /* nil reference for room-database	*/
#define NOTHING	   -1    /* nil reference for objects		*/
#define NOBODY	   -1    /* nil reference for mobiles		*/

#define SPECIAL(name) \
   int (name)(struct char_data *ch, void *me, int cmd, char *argument)

/* misc editor defines **************************************************/

/* format modes for format_text */
#define FORMAT_INDENT		(1 << 0)


/* room-related defines *************************************************/


/* The cardinal directions: used as index to room_data.dir_option[] */
#define NORTH          0
#define EAST           1
#define SOUTH          2
#define WEST           3
#define UP             4
#define DOWN           5


/* Room flags: used in room_data.room_flags */
/* WARNING: In the world files, NEVER set the bits marked "R" ("Reserved") */
#define ROOM_DARK		(1 << 0)   /* Dark			*/
#define ROOM_DEATH		(1 << 1)   /* Death trap		*/
#define ROOM_NOMOB		(1 << 2)   /* MOBs not allowed		*/
#define ROOM_INDOORS		(1 << 3)   /* Indoors			*/
#define ROOM_PEACEFUL		(1 << 4)   /* Violence not allowed	*/
#define ROOM_SOUNDPROOF		(1 << 5)   /* Shouts, gossip blocked	*/
#define ROOM_NOSENSE		(1 << 6)   /* SENSE won't go through	*/
#define ROOM_NOMAGIC		(1 << 7)   /* Magic not allowed		*/
#define ROOM_TUNNEL		(1 << 8)   /* room for only 1 pers	*/
#define ROOM_PRIVATE		(1 << 9)   /* Can't teleport in		*/
#define ROOM_GODROOM		(1 << 10)  /* LVL_GOD+ only allowed	*/
#define ROOM_HOUSE		(1 << 11)  /* (R) Room is a house	*/
#define ROOM_HOUSE_CRASH	(1 << 12)  /* (R) House needs saving	*/
#define ROOM_ATRIUM		(1 << 13)  /* (R) The door to a house	*/
#define ROOM_OLC		(1 << 14)  /* (R) Modifyable/!compress	*/
#define ROOM_BFS_MARK		(1 << 15)  /* (R) breath-first srch mrk	*/
#define ROOM_GOOD_REGEN         (1 << 16)  /* Good Regen Room           */
#define ROOM_GRAV10             (1 << 17)  /* Gravity X 10              */
#define ROOM_GRAV100            (1 << 18)  /* Gravity X 100             */
#define ROOM_GRAV200            (1 << 19)  /* Gravity X 200             */
#define ROOM_GRAV300            (1 << 20)  /* Gravity X 300             */
#define ROOM_GRAV400            (1 << 21)  /* Gravity X 400             */
#define ROOM_SPIRIT             (1 << 22)  /* Room of Spirit and Time   */
#define ROOM_ND                 (1 << 23)  /* Im too lazy to make notes */
#define ROOM_DOJO               (1 << 24)  /* Dojo for sparing		*/
#define ROOM_TRAINER            (1 << 25)
#define ROOM_DRAIN              (1 << 26)  /* Empty */
#define ROOM_SHORE              (1 << 27)  /* Can fish from here */
#define ROOM_HTANK              (1 << 28)  /* Healing Tank */
#define ROOM_NOQUIT             (1 << 29)
#define ROOM_NORECALL           (1 << 30)  /* Recalling not allowed  */
#define ROOM_UNUSED             (1 << 31)

/* Exit info: used in room_data.dir_option.exit_info */
#define EX_ISDOOR		(1 << 0)   /* Exit is a door		*/
#define EX_CLOSED		(1 << 1)   /* The door is closed	*/
#define EX_LOCKED		(1 << 2)   /* The door is locked	*/
#define EX_PICKPROOF		(1 << 3)   /* Lock can't be picked	*/


/* Sector types: used in room_data.sector_type */
#define SECT_INSIDE          0		   /* Indoors			*/
#define SECT_CITY            1		   /* In a city			*/
#define SECT_FIELD           2		   /* In a field		*/
#define SECT_FOREST          3		   /* In a forest		*/
#define SECT_HILLS           4		   /* In the hills		*/
#define SECT_MOUNTAIN        5		   /* On a mountain		*/
#define SECT_WATER_SWIM      6		   /* Swimmable water		*/
#define SECT_WATER_NOSWIM    7		   /* Water - need a boat	*/
#define SECT_UNDERWATER	     8		   /* Underwater		*/
#define SECT_FLYING	     9		   /* Wheee!			*/
#define SECT_EARTH           10
#define SECT_VEGETA          11
#define SECT_FRIGID          12
#define SECT_KONACK          13
#define SECT_ORAINAS         14
#define SECT_NAMEK           15


/* char and mob-related defines *****************************************/



/* PC classes */
#define CLASS_UNDEFINED -1
#define CLASS_Human      0
#define CLASS_Namek      1
#define CLASS_icer       2
#define CLASS_saiyan     3
#define CLASS_MAJIN      4
#define CLASS_ANDROID    5
#define CLASS_KAI      6
#define CLASS_HALF_BREED 7
#define CLASS_BIODROID   8
#define CLASS_KONATSU    9
#define CLASS_CHANGELING 10
#define CLASS_demon      11
#define CLASS_TRUFFLE    12
#define CLASS_MUTANT      13
#define CLASS_KANASSAN    14
#define CLASS_ANGEL       15


/* And PC classes again, because we can't change the above without futzing
 * the pfile
 */
#define CLASS_HUMAN_B	(1 << CLASS_Human)
#define CLASS_NAMEK_B	(1 << CLASS_Namek)
#define CLASS_ICER_B	(1 << CLASS_icer)
#define CLASS_SAIYAN_B	(1 << CLASS_saiyan)
#define CLASS_ANDROID_B	(1 << CLASS_ANDROID)
#define CLASS_KAI_B	(1 << CLASS_KAI)
#define CLASS_HALF_BREED_B (1 << CLASS_HALF_BREED)
#define CLASS_MAJIN_B	(1 << CLASS_MAJIN)
#define CLASS_BIODROID_B   (1 << CLASS_BIODROID)
#define CLASS_KONATSU_B	(1 << CLASS_KONATSU)
#define CLASS_CHANGELING_B	(1 << CLASS_CHANGELING)
#define CLASS_demon_B	(1 << CLASS_demon)
#define CLASS_TRUFFLE_B	(1 << CLASS_TRUFFLE)
#define CLASS_MUTANT_B	(1 << CLASS_MUTANT)
#define CLASS_KANASSAN_B (1 << CLASS_KANASSAN)
#define CLASS_ANGEL_B (1 << CLASS_ANGEL)
#define CLASS_ALL_B	(1 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8 | 1 << 9 | 1 << 10 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14 | 1 << 15)
#define NUM_CLASSES	 16  /* This must be the number of classes!! */

/* NPC classes (currently unused - feel free to implement!) */
#define CLASS_OTHER       0
#define CLASS_UNDEAD      1
#define CLASS_HUMANOID    2
#define CLASS_ANIMAL      3
#define CLASS_DRAGON      4
#define CLASS_GIANT       5


/* Sex */
#define SEX_NEUTRAL   0
#define SEX_MALE      1
#define SEX_FEMALE    2

/* Eye Color */
#define EYE_WHITE     0
#define EYE_BLACK     1
#define EYE_BLUE      2
#define EYE_GREEN     3
#define EYE_BROWN     4
#define EYE_PURPLE    5
#define EYE_RED       6
#define EYE_YELLOW    7
#define EYE_ICYBLUE   8

/* Hair Color */
#define HAIRC_BLACK     0
#define HAIRC_WHITE     1
#define HAIRC_BLUE      2
#define HAIRC_GREEN     3
#define HAIRC_BROWN     4
#define HAIRC_PURPLE    5
#define HAIRC_RED       6
#define HAIRC_YELLOW    7
#define HAIRC_NONE      8

/* Hair Length */
#define HAIRL_V_SHORT   0
#define HAIRL_SHORT     1
#define HAIRL_MEDIUM    2
#define HAIRL_LONG      3
#define HAIRL_V_LONG    4
#define HAIRL_BALD      5
#define HAIRL_SSPIKE    6
#define HAIRL_MSPIKE    7
#define HAIRL_LSPIKE    8

/*Aura Color*/
#define AURA_BLACK      0
#define AURA_WHITE      1
#define AURA_BLUE       2
#define AURA_GREEN      3
#define AURA_PURPLE     4
#define AURA_RED        5
#define AURA_YELLOW     6
#define AURA_PINK       7

/*clans*/
#define CLAN_EVIL       0
#define CLAN_GOOD       1
#define CLAN_NEUTRAL    2
#define CLAN_NONE       3
#define CLAN_ZF         4
#define CLAN_CK         5

/*build*/
#define BUILD_SKIN      0
#define BUILD_LEAN      1
#define BUILD_MUSC      2
#define BUILD_CHUB      3
#define BUILD_FAT       4

/*gods*/
#define GOD_TK          0
#define GOD_XYL         1
#define GOD_RAIN        2
#define GOD_ERD         3
#define GOD_AEANA       4
#define GOD_HIRO        5
#define GOD_NONE        6

/* Positions */
#define POS_DEAD       0	/* dead			*/
#define POS_MORTALLYW  1	/* mortally wounded	*/
#define POS_INCAP      2	/* incapacitated	*/
#define POS_STUNNED    3	/* stunned		*/
#define POS_SLEEPING   4	/* sleeping		*/
#define POS_RESTING    5	/* resting		*/
#define POS_SITTING    6	/* sitting		*/
#define POS_FIGHTING   7	/* fighting		*/
#define POS_STANDING   8	/* standing		*/
#define POS_FLOATING   9        /* floating               */


/* Player flags: used by char_data.char_specials.act */
#define PLR_KILLER	(1 << 0)   /* Player is a player-killer		*/
#define PLR_icer	(1 << 1)   /* Player is a player-icer		*/
#define PLR_FROZEN	(1 << 2)   /* Player is frozen			*/
#define PLR_DONTSET     (1 << 3)   /* Don't EVER set (ISNPC bit)	*/
#define PLR_WRITING	(1 << 4)   /* Player writing (board/mail/olc)	*/
#define PLR_MAILING	(1 << 5)   /* Player is writing mail		*/
#define PLR_CRASH	(1 << 6)   /* Player needs to be crash-saved	*/
#define PLR_SITEOK	(1 << 7)   /* Player has been site-cleared	*/
#define PLR_NOSHOUT	(1 << 8)   /* Player not allowed to shout/goss	*/
#define PLR_NOTITLE	(1 << 9)   /* Player not allowed to set title	*/
#define PLR_DELETED	(1 << 10)  /* Player deleted - space reusable	*/
#define PLR_LOADROOM	(1 << 11)  /* Player uses nonstandard loadroom	*/
#define PLR_NOWIZLIST	(1 << 12)  /* Player shouldn't be on wizlist	*/
#define PLR_NODELETE	(1 << 13)  /* Player shouldn't be deleted	*/
#define PLR_INVSTART	(1 << 14)  /* Player should enter game wizinvis	*/
#define PLR_CRYO	(1 << 15)  /* Player is cryo-saved (purge prog)	*/
#define PLR_LARM	(1 << 16)  /* Player is cryo-saved (purge prog)	*/
#define PLR_RARM	(1 << 17)  /* Player is cryo-saved (purge prog)	*/
#define PLR_LLEG	(1 << 18)  /* Player is cryo-saved (purge prog)	*/
#define PLR_RLEG	(1 << 19)  /* Player is cryo-saved (purge prog)	*/
#define PLR_STAIL	(1 << 20)  /* Player is cryo-saved (purge prog)	*/
#define PLR_TAIL	(1 << 21)  /* Player is cryo-saved (purge prog)	*/
#define PLR_trans1      (1 << 22)
#define PLR_trans2      (1 << 23)
#define PLR_trans3      (1 << 24)
#define PLR_trans4      (1 << 25)
#define PLR_trans5      (1 << 26) /* Upgrade 5.0 */
#define PLR_trans6      (1 << 27) /* Upgrade 6.0 */
#define PLR_FORELOCK    (1 << 28)
#define PLR_DEFLECT     (1 << 29)
#define PLR_MAJINIZE    (1 << 30)
#define PLR_NOMAJIN     (1 << 31)
#define PLR_BOUNTY      (1 << 32)

/* Mobile flags: used by char_data.char_specials.act */
#define MOB_SPEC         (1 << 0)  /* Mob has a callable spec-proc	*/
#define MOB_SENTINEL     (1 << 1)  /* Mob should not move		*/
#define MOB_SCAVENGER    (1 << 2)  /* Mob picks up stuff on the ground	*/
#define MOB_ISNPC        (1 << 3)  /* (R) Automatically set on all Mobs	*/
#define MOB_AWARE	 (1 << 4)  /* Mob can't be backstabbed		*/
#define MOB_AGGRESSIVE   (1 << 5)  /* Mob hits players in the room	*/
#define MOB_STAY_ZONE    (1 << 6)  /* Mob shouldn't scouter out of zone	*/
#define MOB_WIMPY        (1 << 7)  /* Mob flees if severely injured	*/
#define MOB_AGGR_EVIL	 (1 << 8)  /* auto attack evil PC's		*/
#define MOB_AGGR_GOOD	 (1 << 9)  /* auto attack good PC's		*/
#define MOB_AGGR_NEUTRAL (1 << 10) /* auto attack neutral PC's		*/
#define MOB_MEMORY	 (1 << 11) /* remember attackers if attacked	*/
#define MOB_HELPER	 (1 << 12) /* attack PCs fighting other NPCs	*/
#define MOB_NOMAJIN	 (1 << 13) /* Mob can't be MAJINed		*/
#define MOB_NOYOIKOMINMINKEN	 (1 << 15) /* Mob can't be slept		*/
#define MOB_NOSWEEP	 (1 << 16) /* Mob can't be sweeped (e.g. trees)	*/
#define MOB_NOSOLAR_FLARE	 (1 << 17) /* Mob can't be blinded		*/
#define MOB_NOMIND_FREEZE (1 << 18) /* Mob can't be frozen            */
#define MOB_NOKAMEHAME    (1 << 19)
#define MOB_NOBIGBANG     (1 << 20)
#define MOB_NOFINALFLASH  (1 << 21)
#define MOB_NOCHOUKAME    (1 << 22)
#define MOB_NODISK        (1 << 23)
#define MOB_NOMASENKO     (1 << 24)
#define MOB_NOMAKANKO     (1 << 25)

/* Preference flags: used by char_data.player_specials.pref */
#define PRF_BRIEF       (1 << 0)  /* Room descs won't normally be shown	*/
#define PRF_COMPACT     (1 << 1)  /* No extra CRLF pair before prompts	*/
#define PRF_DEAF	(1 << 2)  /* Can't hear shouts			*/
#define PRF_NOTELL	(1 << 3)  /* Can't receive tells		*/
#define PRF_DISPHP      (1 << 4)  /* Display hit points in prompt       */
#define PRF_DISPMANA    (1 << 5)  /* Display mana points in prompt      */
#define PRF_AUTOEXIT	(1 << 7)  /* Display exits in a room		*/
#define PRF_NOHASSLE	(1 << 8)  /* Aggr mobs won't attack		*/
#define PRF_QUEST	(1 << 9)  /* On quest				*/
#define PRF_SUMMONABLE	(1 << 10) /* Can be summoned			*/
#define PRF_NOREPEAT	(1 << 11) /* No repetition of comm commands	*/
#define PRF_HOLYLIGHT	(1 << 12) /* Can see in dark			*/
#define PRF_COLOR_1	(1 << 13) /* Color (low bit)			*/
#define PRF_COLOR_2	(1 << 14) /* Color (high bit)			*/
#define PRF_NOWIZ	(1 << 15) /* Can't hear wizline			*/
#define PRF_LOG1	(1 << 16) /* On-line System Log (low bit)	*/
#define PRF_LOG2	(1 << 17) /* On-line System Log (high bit)	*/
#define PRF_NOooc	(1 << 18) /* Can't hear ooc channel		*/
#define PRF_NOGOSS	(1 << 19) /* Can't hear gossip channel		*/
#define PRF_NOGRATZ	(1 << 20) /* Can't hear grats channel		*/
#define PRF_ROOMFLAGS	(1 << 21) /* Can see room flags (ROOM_x)	*/
#define PRF_AFK         (1 << 22)
#define PRF_CAMP        (1 << 23)
#define PRF_OOZARU      (1 << 24)
#define PRF_MYSTIC      (1 << 25)
#define PRF_MYSTIC2     (1 << 26)
#define PRF_MYSTIC3     (1 << 27)
#define PRF_AUTOLOOT    (1 << 28)
#define PRF_FLYING      (1 << 29)
#define PRF_TRAINING    (1 << 30)
#define PRF_AUTOSPLIT   (1 << 31)

/* pref 2 flags added for additional preferences */
#define PRF2_TRANS1 	(1 << 0)
#define PRF2_TRANS2     (1 << 1)
#define PRF2_TRANS3     (1 << 2)
#define PRF2_EARTH      (1 << 3)
#define PRF2_FRIGID     (1 << 4)
#define PRF2_VEGETA     (1 << 5)
#define PRF2_ORAINAS    (1 << 6)
#define PRF2_KONACK     (1 << 7)
#define PRF2_NAMEK      (1 << 8)
#define PRF2_MUGG       (1 << 9)
#define PRF2_HEAD       (1 << 10)
#define PRF2_GO         (1 << 11)
#define PRF2_FISHING    (1 << 12)
#define PRF2_FISH1      (1 << 13)
#define PRF2_FISH2      (1 << 14)
#define PRF2_FISH3      (1 << 15)
#define PRF2_FISH4      (1 << 16)
#define PRF2_CAUGHT     (1 << 17)
#define PRF2_REELING    (1 << 18)

/* Clan flags OooooWooooo, hope this works out. */
#define CLN_LEADER1     (1 << 0)
#define CLN_LEADER2     (1 << 1)
#define CLN_LEADER3     (1 << 2)
#define CLN_LEADER4     (1 << 3)
#define CLN_LEADER5     (1 << 4)
#define CLN_LEADER6     (1 << 5)
#define CLN_LEADER7     (1 << 6)
#define CLN_LEADER8     (1 << 7)
/* Support a total of 8 clans at a time for flexibility */
#define CLN_MEMBER1     (1 << 8)
#define CLN_MEMBER2     (1 << 9)
#define CLN_MEMBER3     (1 << 10)
#define CLN_MEMBER4     (1 << 11)
#define CLN_MEMBER5     (1 << 12)
#define CLN_MEMBER6     (1 << 13)
#define CLN_MEMBER7     (1 << 14)
#define CLN_MEMBER8     (1 << 15)
/* Clan member flag for each clan */
#define CLN_NEWBIE      (1 << 16)
#define CLN_NOVICE      (1 << 17)
#define CLN_MEMBER      (1 << 18)
#define CLN_CAPTAIN     (1 << 19)
/* Clan rank flags, will work for each clan */
#define CLN_Death       (1 << 20)
#define CLN_RAGE        (1 << 21)
#define CLN_BLOCK       (1 << 22)
#define CLN_DODGE       (1 << 23)
#define CLN_ADEF        (1 << 24)
#define CLN_VITAL       (1 << 25)
#define CLN_STASIS      (1 << 26)
#define CLN_BLUST       (1 << 27)
/* Extra flags used with the clan ...sorta */

/* Affect bits: used in char_data.char_specials.saved.affected_by */
/* WARNING: In the world files, NEVER set the bits marked "R" ("Reserved") */
#define AFF_SOLAR_FLARE       (1 << 0)	   /* (R) Char is blind		*/
#define AFF_INVISIBLE         (1 << 1)	   /* Char is invisible		*/
#define AFF_DETECT_ALIGN      (1 << 2)	   /* Char is sensitive to align*/
#define AFF_DETECT_INVISIBILTY (1 << 3)	   /* Char can see invis chars  */
#define AFF_DETECT_MAGIC      (1 << 4)	   /* Char is sensitive to magic*/
#define AFF_SENSE_LIFE        (1 << 5)	   /* Char can sense hidden life*/
#define AFF_IREG	      (1 << 6)	   /* Char can walk on water	*/
#define AFF_BARRIER           (1 << 7)	   /* Char protected by sanct.	*/
#define AFF_GROUP             (1 << 8)	   /* (R) Char is grouped	*/
#define AFF_CURSE             (1 << 9)	   /* Char has 8 arms		*/
#define AFF_INFRAVISION       (1 << 10)	   /* Char can see in dark	*/
#define AFF_POISON            (1 << 11)	   /* (R) Char is poisoned	*/
#define AFF_PROTECT_EVIL      (1 << 12)	   /* Char protected from evil  */
#define AFF_CALM              (1 << 13)	   /* Char protected from good  */
#define AFF_YOIKOMINMINKEN    (1 << 14)	   /* (R) Char magically asleep	*/
#define AFF_FRENZY	      (1 << 15)	   /* Char can't be SENSEed	*/
#define AFF_MEDITATING	      (1 << 16)	   /* Room for future expansion	*/
#define AFF_PLATED	      (1 << 17)	   /* Room for future expansion	*/
#define AFF_SWIFTNESS         (1 << 18)	   /* Char can move quietly	*/
#define AFF_HIDE              (1 << 19)	   /* Char is hidden		*/
#define AFF_UNUSED20	      (1 << 20)	   /* Room for future expansion	*/
#define AFF_MAJIN             (1 << 21)	   /* Char is MAJINed		*/
#define AFF_MFROZEN           (1 << 22)    /* (R) Char is Mfrozen       */      
#define AFF_SPAR              (1 << 23)    /* (R) Char is Mfrozen       */  
#define AFF_SFLY              (1 << 24)    /* (R) Char is Mfrozen       */ 
#define AFF_SBLOCK            (1 << 25)    /* (R) Char is Mfrozen       */ 
#define AFF_JOG               (1 << 26)    /* (R) Char is Mfrozen       */
#define AFF_CHARGEL           (1 << 27)    /* (R) Char is Mfrozen       */
#define AFF_CHARGEM           (1 << 28)    /* (R) Char is Mfrozen       */
#define AFF_CHARGEH           (1 << 29)    /* (R) Char is Mfrozen       */
#define AFF_HALTED            (1 << 30)    /* (R) Char is Mfrozen       */
#define AFF_STONED            (1 << 31)

/* Modes of connectedness: used by descriptor_data.state */
#define CON_PLAYING	 0		/* Playing - Nominal state	*/
#define CON_CLOSE	 1		/* Disconnecting		*/
#define CON_GET_NAME	 2		/* By what name ..?		*/
#define CON_NAME_CNFRM	 3		/* Did I get that right, x?	*/
#define CON_PASSWORD	 4		/* Password:			*/
#define CON_NEWPASSWD	 5		/* Give me a password for x	*/
#define CON_CNFPASSWD	 6		/* Please retype password:	*/
#define CON_QSEX	 7		/* SEX?				*/
#define CON_QCLASS	 8		/* Class?			*/
#define CON_RMOTD	 9		/* PRESS RETURN after MOTD	*/
#define CON_MENU	 10		/* Your choice: (main menu)	*/
#define CON_EXDESC	 11		/* Enter a new description:	*/
#define CON_CHPWD_GETOLD 12		/* Changing passwd: get old	*/
#define CON_CHPWD_GETNEW 13		/* Changing passwd: get new	*/
#define CON_CHPWD_VRFY   14		/* Verify new password		*/
#define CON_DELCNF1	 15		/* Delete confirmation 1	*/
#define CON_DELCNF2	 16		/* Delete confirmation 2	*/
#define CON_DISCONNECT	 17		/* In-game disconnection	*/
#define CON_OEDIT        18		/*. OLC mode - object edit     .*/
#define CON_REDIT        19		/*. OLC mode - room edit       .*/
#define CON_ZEDIT        20		/*. OLC mode - zone info edit  .*/
#define CON_MEDIT        21		/*. OLC mode - mobile edit     .*/
#define CON_SEDIT        22		/*. OLC mode - shop edit       .*/
#define CON_TRIGEDIT     23		/*. OLC mode - trigger edit    .*/
#define CON_HEDIT	 24		/*. OLC mode - help editor     .*/
#define CON_AEDIT	 25		/*. OLC mode - action editor   .*/
#define CON_TEXTED	 26		/*. OLC mode - text editor     .*/
#define CON_QEYE	 27		/* Eye color.			*/
#define CON_QHAIRC	 28		/* Hair color.			*/
#define CON_QHAIRL	 29		/* Hair Lenght.			*/
#define CON_QAURA        30
#define CON_QBUILD       31
#define CON_QGOD         32

/* Character equipment positions: used as index for char_data.equipment[] */
/* NOTE: Don't confuse these constants with the ITEM_ bitvectors
   which control the valid places you can wear a piece of equipment */
#define WEAR_LIGHT      0
#define WEAR_FINGER_R   1
#define WEAR_FINGER_L   2
#define WEAR_NECK_1     3
#define WEAR_NECK_2     4
#define WEAR_BODY       5
#define WEAR_HEAD       6
#define WEAR_LEGS       7
#define WEAR_FEET       8
#define WEAR_HANDS      9
#define WEAR_ARMS      10
#define WEAR_SHIELD    11
#define WEAR_ABOUT     12
#define WEAR_WAIST     13
#define WEAR_WRIST_R   14
#define WEAR_WRIST_L   15
#define WEAR_WIELD     16
#define WEAR_HOLD      17
#define WEAR_FLOAT     18
#define WEAR_AURA      19
#define WEAR_LEAR      20
#define WEAR_REAR      21
#define WEAR_FORE      22
#define WEAR_WEDDING   23
#define WEAR_FORELOCK  24
#define WEAR_DUAL      25

#define NUM_WEARS      26	/* This must be the # of eq positions!! */


/* object-related defines ********************************************/


/* Item types: used by obj_data.obj_flags.type_flag */
#define ITEM_LIGHT      1		/* Item is a light source	*/
#define ITEM_SENZU      2		/* Item is a SENZU		*/
#define ITEM_SCOUTER    3		/* Item is a SCOUTER		*/
#define ITEM_STAFF      4		/* Item is a STAFF		*/
#define ITEM_WEAPON     5		/* Item is a weapon		*/
#define ITEM_FLOOD      6		/* Unimplemented		*/
#define ITEM_MISSILE    7		/* Unimplemented		*/
#define ITEM_TREASURE   8		/* Item is a treasure, not gold	*/
#define ITEM_ARMOR      9		/* Item is armor		*/
#define ITEM_SAIBAKIT  10 		/* Item is a SAIBAKIT		*/
#define ITEM_WORN      11		/* Unimplemented		*/
#define ITEM_OTHER     12		/* Misc object			*/
#define ITEM_TRASH     13		/* Trash - shopkeeps won't buy	*/
#define ITEM_TRAP      14		/* Unimplemented		*/
#define ITEM_CONTAINER 15		/* Item is a container		*/
#define ITEM_NOTE      16		/* Item is note 		*/
#define ITEM_DRINKCON  17		/* Item is a drink container	*/
#define ITEM_KEY       18		/* Item is a key		*/
#define ITEM_FOOD      19		/* Item is food			*/
#define ITEM_MONEY     20		/* Item is money (gold)		*/
#define ITEM_PEN       21		/* Item is a pen		*/
#define ITEM_HOLYWATER 22		/* Item is a boat		*/
#define ITEM_FOUNTAIN  23		/* Item is a fountain		*/
#define ITEM_NANO      24		/* Item is a fountain		*/
#define ITEM_dball1    25
#define ITEM_dball2    26
#define ITEM_dball3    27
#define ITEM_dball4    28
#define ITEM_dball5    29
#define ITEM_dball6    30
#define ITEM_dball7    31

/* Take/Wear flags: used by obj_data.obj_flags.wear_flags */
#define ITEM_WEAR_TAKE		(1 << 0)  /* Item can be takes */
#define ITEM_WEAR_FINGER	(1 << 1)  /* Can be worn on finger */
#define ITEM_WEAR_NECK		(1 << 2)  /* Can be worn around neck */
#define ITEM_WEAR_BODY		(1 << 3)  /* Can be worn on body */
#define ITEM_WEAR_HEAD		(1 << 4)  /* Can be worn on head */
#define ITEM_WEAR_LEGS		(1 << 5)  /* Can be worn on legs */
#define ITEM_WEAR_FEET		(1 << 6)  /* Can be worn on feet */
#define ITEM_WEAR_HANDS		(1 << 7)  /* Can be worn on hands */
#define ITEM_WEAR_ARMS		(1 << 8)  /* Can be worn on arms */
#define ITEM_WEAR_SHIELD	(1 << 9)  /* Can be used as a shield */
#define ITEM_WEAR_ABOUT		(1 << 10) /* Can be worn about body */
#define ITEM_WEAR_WAIST 	(1 << 11) /* Can be worn around waist */
#define ITEM_WEAR_WRIST		(1 << 12) /* Can be worn on wrist */
#define ITEM_WEAR_WIELD		(1 << 13) /* Can be wielded */
#define ITEM_WEAR_HOLD		(1 << 14) /* Can be held */
#define ITEM_WEAR_FLOAT         (1 << 15)
#define ITEM_WEAR_AURA          (1 << 16)
#define ITEM_WEAR_LEAR          (1 << 17)
#define ITEM_WEAR_REAR          (1 << 18)
#define ITEM_WEAR_FORE          (1 << 19)
#define ITEM_WEAR_WEDDING       (1 << 20)
#define ITEM_WEAR_FORELOCK      (1 << 21)
#define ITEM_WEAR_DUAL          (1 << 22)

/*Extra object flags: used by obj_data.obj_flags.extra_flags */
#define ITEM_GLOW          (1 << 0)	/* Item is glowing		*/
#define ITEM_HUM           (1 << 1)	/* Item is humming		*/
#define ITEM_NORENT        (1 << 2)	/* Item cannot be rented	*/
#define ITEM_NODONATE      (1 << 3)	/* Hijacked for truffle stasis cells*/
#define ITEM_NOINVIS	   (1 << 4)	/* Item cannot be made invis	*/
#define ITEM_INVISIBLE     (1 << 5)	/* Item is invisible		*/
#define ITEM_ROD           (1 << 6)	/* Item is a fishing rod        */
#define ITEM_NODROP        (1 << 7)	/* Item is cursed: can't drop	*/
#define ITEM_ZANZOKEN      (1 << 8)	/* Item is ZANZOKENed		*/
#define ITEM_ANTI_GOOD     (1 << 9)	/* Not usable by good people	*/
#define ITEM_ANTI_EVIL     (1 << 10)	/* Not usable by evil people	*/
#define ITEM_ANTI_NEUTRAL  (1 << 11)	/* Not usable by neutral people	*/
#define ITEM_ANTI_Human    (1 << 12)	/* Not usable by Humans		*/
#define ITEM_ANTI_Namek    (1 << 13)	/* Not usable by Nameks	        */
#define ITEM_ANTI_icer	   (1 << 14)	/* Not usable by Icers   	*/
#define ITEM_ANTI_saiyan   (1 << 15)	/* Not usable by saiyans	*/
#define ITEM_NOSELL	   (1 << 16)	/* Shopkeepers won't touch it	*/
#define ITEM_ANTI_MAJIN    (1 << 17)	/* Not usable by Majins  	*/
#define ITEM_ANTI_ANDROID  (1 << 18)	/* Not usable by Androids	*/
#define ITEM_ANTI_KAI    (1 << 19)	/* Not usable by Demons  	*/
#define ITEM_ANTI_HALF_BREED  (1 << 20)	/* Not usable by Half-Breeds	*/
#define ITEM_ANTI_BIODROID (1 << 21)
#define ITEM_ANTI_KONATSU  (1 << 22)
#define ITEM_ANTI_demon     (1 << 23)
#define ITEM_ANTI_TRUFFLE  (1 << 24)
#define ITEM_ANTI_MUTANT   (1 << 25)
#define ITEM_ANTI_TUGUIL   (1 << 26)
#define ITEM_DIVINE        (1 << 27)
#define ITEM_UBERDIVINE    (1 << 28)
#define ITEM_FLAG          (1 << 29)
#define ITEM_MEDIDIVINE    (1 << 30)

/* Modifier constants used with obj affects ('A' fields) */
#define APPLY_NONE              0	/* No effect			*/
#define APPLY_STR               1	/* Apply to strength		*/
#define APPLY_DEX               2	/* Apply to dexterity		*/
#define APPLY_INT               3	/* Apply to constitution	*/
#define APPLY_WIS               4	/* Apply to wisdom		*/
#define APPLY_CON               5	/* Apply to constitution	*/
#define APPLY_RAGE		6	/* Apply to charisma		*/
#define APPLY_CLASS             7	/* Reserved			*/
#define APPLY_LEVEL             8	/* Reserved			*/
#define APPLY_AGE               9	/* Apply to age			*/
#define APPLY_CHAR_WEIGHT      10	/* Apply to weight		*/
#define APPLY_CHAR_HEIGHT      11	/* Apply to height		*/
#define APPLY_MANA             12	/* Apply to max mana		*/
#define APPLY_HIT              13	/* Apply to max hit points	*/
#define APPLY_MOVE             14	/* Apply to max move points	*/
#define APPLY_GOLD             15	/* Reserved			*/
#define APPLY_EXP              16	/* Reserved			*/
#define APPLY_AC               17	/* Apply to armor Class		*/
#define APPLY_HITROLL          18	/* Apply to hitroll		*/
#define APPLY_DAMROLL          19	/* Apply to damage roll		*/
#define APPLY_SAVING_PARA      20	/* Apply to save throw: paralz	*/
#define APPLY_SAVING_ROD       21	/* Apply to save throw: rods	*/
#define APPLY_SAVING_PETRI     22	/* Apply to save throw: petrif	*/
#define APPLY_SAVING_BREATH    23	/* Apply to save throw: breath	*/
#define APPLY_SAVING_SPELL     24	/* Apply to save throw: spells	*/
#define APPLY_CHA              25	/* Apply to save throw: spells	*/
#define APPLY_LBS              26	/* Apply to save throw: spells	*/
#define APPLY_UBS              27	/* Apply to save throw: spells	*/
#define APPLY_CLAN             28
#define APPLY_KISKILL          29

#define SPELL_OFFENSIVE		0
#define SPELL_DEFENSIVE		1
#define SPELL_ENHANCE		2
#define SPELL_OTHER             3
#define SPELL_HEALSPELL         4

/* Container flags - value[1] */
#define CONT_CLOSEABLE      (1 << 0)	/* Container can be closed	*/
#define CONT_PICKPROOF      (1 << 1)	/* Container is pickproof	*/
#define CONT_CLOSED         (1 << 2)	/* Container is closed		*/
#define CONT_LOCKED         (1 << 3)	/* Container is locked		*/


/* Some different kind of liquids for use in values of drink containers */
#define LIQ_WATER      0
#define LIQ_BEER       1
#define LIQ_WINE       2
#define LIQ_ALE        3
#define LIQ_DARKALE    4
#define LIQ_WHISKY     5
#define LIQ_LEMONADE   6
#define LIQ_FIREBRT    7
#define LIQ_LOCALSPC   8
#define LIQ_SLIME      9
#define LIQ_MILK       10
#define LIQ_TEA        11
#define LIQ_COFFE      12
#define LIQ_BLOOD      13
#define LIQ_SALTWATER  14
#define LIQ_CLEARWATER 15


/* other miscellaneous defines *******************************************/


/* Player conditions */
#define DRUNK        0
#define FULL         1
#define THIRST       2


/* Sun state for weather_data */
#define SUN_DARK	0
#define SUN_RISE	1
#define SUN_LIGHT	2
#define SUN_SET		3


/* Sky conditions for weather_data */
#define SKY_CLOUDLESS	0
#define SKY_CLOUDY	1
#define SKY_RAINING	2
#define SKY_LIGHTNING	3


/* Rent codes */
#define RENT_UNDEF      0
#define RENT_CRASH      1
#define RENT_RENTED     2
#define RENT_CRYO       3
#define RENT_FORCED     4
#define RENT_TIMEDOUT   5


/* other #defined constants **********************************************/

/*
 * **DO**NOT** SOLAR_FLAREly change the number of levels in your MUD merely by
 * changing these numbers and without changing the rest of the code to match.
 * Other changes throughout the code are required.  See coding.doc for
 * details.
 *
 * LVL_IMPL should always be the HIGHEST possible immortal level, and
 * LVL_IMMORT should always be the LOWEST immortal level.  The number of
 * mortal levels will always be LVL_IMMORT - 1.
 */
#define LVL_1000014     (1000014LL)
#define LVL_1000013     (1000013LL)
#define LVL_1000012	(1000012LL)
#define LVL_IMPL	(1000011LL)
#define LVL_IMPLL	(1000010LL)
#define LVL_COIMP       (1000009LL)
#define LVL_GRGOD       (1000008LL)
#define LVL_CODER	(1000007LL)
#define LVL_AVATAR      (1000006LL)
#define LVL_ARCANE      (1000005LL)
#define LVL_GOD         (1000004LL)
#define LVL_LSSRGOD     (1000003LL)
#define LVL_DEMIGOD     (1000002LL)
#define LVL_IMMORT	(1000001LL)

/* Level of the 'freeze' command */
#define LVL_FREEZE	LVL_GRGOD

#define NUM_OF_DIRS	6	/* number of directions in a room (nsewud) */
#define MAGIC_NUMBER	(0x06)	/* Arbitrary number that won't be in a string */

#define OPT_USEC	100000	/* 10 passes per second */
#define PASSES_PER_SEC	(1000000 / OPT_USEC)
#define RL_SEC		* PASSES_PER_SEC

#define PULSE_ZONE      (10 RL_SEC)
#define PULSE_MOBILE    (10 RL_SEC)
#define PULSE_VIOLENCE  (3 RL_SEC)
#define PULSE_LAGS      (2 RL_SEC)

/* Variables for the output buffering system */
#define MAX_SOCK_BUF            (12 * 164840) /* Size of kernel's sock buf   */
#define MAX_PROMPT_LENGTH       20000          /* Max length of prompt        */
#define GARBAGE_SPACE		500000          /* Space for **OVERFLOW** etc  */
#define SMALL_BUFSIZE		800000        /* Static output buffer size   */
/* Max amount of output that can be buffered */
#define LARGE_BUFSIZE	   (MAX_SOCK_BUF - GARBAGE_SPACE - MAX_PROMPT_LENGTH)

/*
 * --- WARNING ---
 * If you are using a BSD-derived UNIX with MD5 passwords, you _must_
 * make MAX_PWD_LENGTH larger.  A length of 20 should be good. If
 * you leave it at the KAIO value of 10, then any character with
 * a name longer than about 5 characters will be able to log in with
 * _any_ password.  This has not (yet) been changed to ensure pfile
 * compatibility for those unaffected.
 */
#define HISTORY_SIZE		1	/* Keep last 5 commands. */
#define MAX_STRING_LENGTH	10000
#define MAX_INPUT_LENGTH	800	/* Max length per *line* of input */
#define MAX_RAW_INPUT_LENGTH	912	/* Max size of *raw* input */
#define MAX_MESSAGES		60
#define MAX_NAME_LENGTH		20  /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_PWD_LENGTH		10  /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_TITLE_LENGTH	100 /* Used in char_file_u *DO*NOT*CHANGE* */
#define HOST_LENGTH		30  /* Used in char_file_u *DO*NOT*CHANGE* */
#define EXDSCR_LENGTH		240 /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_TONGUE		3   /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_SKILLS		200 /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_AFFECT		32  /* Used in char_file_u *DO*NOT*CHANGE* */
#define MAX_OBJ_AFFECT		6 /* Used in obj_file_elem *DO*NOT*CHANGE* */



#define CAP_MOP1     (1 << 0)   
#define CAP_MOP2     (1 << 1)   
#define CAP_MOP3     (1 << 2)   
#define CAP_MOP4     (1 << 3) 


/**********************************************************************
* Structures                                                          *
**********************************************************************/


typedef signed char		sbyte;
typedef unsigned char		ubyte;
typedef signed short int	sh_int;
typedef unsigned short int	ush_int;
#if !defined(__cplusplus)	/* Anyone know a portable method? */
typedef char			bool;
#endif

#ifndef CIRCLE_WINDOWS
typedef char			byte;
#endif

typedef sh_int	room_vnum;	/* A room's vnum type */
typedef sh_int	obj_vnum;	/* An object's vnum type */
typedef sh_int	mob_vnum;	/* A mob's vnum type */

typedef sh_int	room_rnum;	/* A room's real (internal) number type */
typedef sh_int	obj_rnum;	/* An object's real (internal) num type */
typedef sh_int	mob_rnum;	/* A mobile's real (internal) num type */


/* Extra description: used in objects, mobiles, and rooms */
struct extra_descr_data {
   char	*keyword;                 /* Keyword in look/examine          */
   char	*description;             /* What to see                      */
   struct extra_descr_data *next; /* Next in list                     */
};


/* object-related structures ******************************************/


/* object flags; used in obj_data */
struct obj_flag_data {
   int	value[4];	/* Values of the item (see list)    */
   byte type_flag;	/* Type of item			    */
   int	wear_flags;	/* Where you can wear it	    */
   int	extra_flags;	/* If it hums, glows, etc.	    */
   int	weight;		/* Weigt what else                  */
   int	cost;		/* Value when sold (gp.)            */
   int	cost_per_day;	/* Cost to keep pr. real day        */
   int	timer;		/* Timer for object                 */
   long	bitvector;	/* To set chars bits                */
};


/* Used in obj_file_elem *DO*NOT*CHANGE* */
struct obj_affected_type {
   byte location;      /* Which ability to change (APPLY_XXX) */
   sbyte modifier;     /* How much it changes by              */
};


/* ================== Memory Structure for Objects ================== */
struct obj_data {
   obj_vnum item_number;	/* Where in data-base			*/
   room_rnum in_room;		/* In what room -1 when conta/carr	*/

   struct obj_flag_data obj_flags;/* Object information               */
   struct obj_affected_type affected[MAX_OBJ_AFFECT];  /* affects */

   char	*name;                    /* Title of object :get etc.        */
   char	*description;		  /* When in room                     */
   char	*short_description;       /* when worn/carry/in cont.         */
   char	*action_description;      /* What to write when used          */
   struct extra_descr_data *ex_description; /* extra descriptions     */
   struct char_data *carried_by;  /* Carried by :NULL in room/conta   */
   struct char_data *worn_by;	  /* Worn by?			      */
   sh_int worn_on;		  /* Worn where?		      */

   struct obj_data *in_obj;       /* In what object NULL when none    */
   struct obj_data *contains;     /* Contains objects                 */

   long id;                       /* used by DG triggers              */
   struct trig_proto_list *proto_script; /* list of KAIO triggers  */
   struct script_data *script;    /* script info for the object       */

   struct obj_data *next_content; /* For 'contains' lists             */
   struct obj_data *next;         /* For the object list              */
   sh_int containerNum;           /* for auto_equip                   */
   sh_int where_it_goes;          /* for auto_equip                   */
};
/* ======================================================================= */


/* ====================== File Element for Objects ======================= */
/*                 BEWARE: Changing it will ruin rent files		   */
struct obj_file_elem {
   obj_vnum item_number;

   int	value[4];
   int	extra_flags;
   int	weight;
   int	timer;
   long	bitvector;
   struct obj_affected_type affected[MAX_OBJ_AFFECT];
   sh_int   eq_Bitvector;
   sh_int   where_it_goes;
   sh_int   containerNum;
};


/* header block for rent files.  BEWARE: Changing it will ruin rent files  */
struct rent_info {
   int	time;
   int	rentcode;
   int	net_cost_per_diem;
   int	gold;
   int	account;
   int	nitems;
   int	spare0;
   int	spare1;
   int	spare2;
   int	spare3;
   int	spare4;
   int	spare5;
   int	spare6;
   int  spare8;
};
/* ======================================================================= */


/* room-related structures ************************************************/


struct room_direction_data {
   char	*general_description;       /* When look DIR.			*/

   char	*keyword;		/* for open/close			*/

   sh_int exit_info;		/* Exit info				*/
   obj_vnum key;		/* Key's number (-1 for no key)		*/
   room_rnum to_room;		/* Where direction leads (NOWHERE)	*/
};


/* ================== Memory Structure for room ======================= */
struct room_data {
   room_vnum number;		/* Rooms number	(vnum)		      */
   sh_int zone;                 /* Room zone (for resetting)          */
   int	sector_type;            /* sector type (move/hide)            */
   char	*name;                  /* Rooms name 'You are ...'           */
   char	*description;           /* Shown when entered                 */
   struct extra_descr_data *ex_description; /* for examine/look       */
   struct room_direction_data *dir_option[NUM_OF_DIRS]; /* Directions */
   int room_flags;		/* DEATH,DARK ... etc                 */

   byte light;                  /* Number of lightsources in room     */
   SPECIAL(*func);

   struct trig_proto_list *proto_script; /* list of KAIO triggers  */
   struct script_data *script;  /* script info for the object         */

   struct obj_data *contents;   /* List of items in room              */
   struct char_data *people;    /* List of NPC / PC in room           */
   
};
/* ====================================================================== */


/* char-related structures ************************************************/


/* memory structure for characters */
struct memory_rec_struct {
   long	id;
   struct memory_rec_struct *next;
};

typedef struct memory_rec_struct memory_rec;


/* This structure is purely intended to be an easy way to transfer */
/* and return information about time (real or mudwise).            */
struct time_info_data {
   int hours, day, month;
   sh_int year;
};


/* These data contain information about a players time data */
struct time_data {
   time_t birth;    /* This represents the characters age                */
   time_t logon;    /* Time of the last logon (used to calculate played) */
   int	played;     /* This is the total accumulated time played in secs */
};


/* general player-related info, usually PC's and NPC's */
struct char_player_data {
   char	passwd[MAX_PWD_LENGTH+1]; /* character's password      */
   char	*name;	       /* PC / NPC s name (kill ...  )         */
   char	*short_descr;  /* for NPC 'actions'                    */
   char	*long_descr;   /* for 'look'			       */
   char	*description;  /* Extra descriptions                   */
   char	*title;        /* PC / NPC's title                     */
   byte sex;           /* PC / NPC's SEX                       */
   byte eye;           /* PC / NPC's eye color                 */
   byte hairc;         /* PC / NPC's hair color                */
   byte hairl;         /* PC / NPC's hair lenght               */
   byte aura;
   byte clan;
   byte build;
   byte god;
   byte chclass;       /* PC / NPC's class		       */
   long long level;         /* PC / NPC's level                     */
   int	hometown;      /* PC s Hometown (zone)                 */
   struct time_data time;  /* PC's AGE in days                 */
   ubyte weight;       /* PC / NPC's weight                    */
   ubyte height;       /* PC / NPC's height                    */
};


/* Char's abilities.  Used in char_file_u *DO*NOT*CHANGE* */
struct char_ability_data {
   sbyte str;
   sbyte str_add;      /* 000 - 100 if strength 18             */
   sbyte intel;
   sbyte wis;
   sbyte dex;
   sbyte con;
   sbyte cha;
   sbyte lbs;
   sbyte ubs;
   sbyte rage;
};


/* Char's points.  Used in char_file_u *DO*NOT*CHANGE* */
struct char_point_data {
   long mana;
   long max_mana;     /* Max move for PC/NPC			   */
   long hit;
   long max_hit;      /* Max hit for PC/NPC                      */
   int move;
   int max_move;     /* Max move for PC/NPC                     */

   int armor;        /* Internal -100..100, external -10..10 AC */
   int	gold;           /* Money carried                           */
   int	bank_gold;	/* Gold the char has in a bank account	   */
   long long exp;            /* The experience of the player            */

   int hitroll;       /* Any bonus or penalty to the hit roll    */
   int damroll;       /* Any bonus or penalty to the damage roll */
};


/* 
 * char_special_data_saved: specials which both a PC and an NPC have in
 * common, but which must be saved to the playerfile for PC's.
 *
 * WARNING:  Do not change this structure.  Doing so will ruin the
 * playerfile.  If you want to add to the playerfile, use the spares
 * in player_special_data.
 */
struct char_special_data_saved {
   int	alignment;		/* +-1000 for alignments                */
   long	idnum;			/* player's idnum; -1 for mobiles	*/
   long	act;			/* act flag for NPC's; player flag for PC's */

   long	affected_by;		/* Bitvector for spells/skills affected by */
   sh_int apply_saving_throw[5]; /* Saving throw (Bonuses)		*/
};


/* Special playing constants shared by PCs and NPCs which aren't in pfile */
struct char_special_data {
   struct char_data *fighting;	/* Opponent				*/
   struct char_data *hunting;	/* Char hunted by this char		*/

   byte position;		/* Standing, fighting, sleeping, etc.	*/

   int	carry_weight;		/* Carried weight			*/
   byte carry_items;		/* Number of items carried		*/
   int	timer;			/* Timer for update			*/

   struct char_special_data_saved saved; /* constants saved in plrfile	*/
};


/*
 *  If you want to add new values to the playerfile, do it here.  DO NOT
 * ADD, DELETE OR MOVE ANY OF THE VARIABLES - doing so will change the
 * size of the structure and ruin the playerfile.  However, you can change
 * the names of the spares to something more meaningful, and then use them
 * in your new code.  They will automatically be transferred from the
 * playerfile into memory when players log in.
 */
struct player_special_data_saved {
   byte skills[MAX_SKILLS+1];	/* array of skills plus skill 0		*/
   byte PADDING0;		/* used to be spells_to_learn		*/
   bool talks[MAX_TONGUE];	/* PC s Tongues 0 for NPC		*/
   int	wimp_level;		/* Below this # of hit points, flee!	*/
   long long freeze_level;	/* Level of god who froze char, if any	*/
   long long invis_level;	/* level of invisibility		*/
   room_vnum load_room;		/* Which room to place char in		*/
   long long pref;		/* preference flags for PC's.		*/
   ubyte bad_pws;		/* number of bad password attemps	*/
   sbyte conditions[3];         /* Drunk, full, thirsty			*/

   /* spares below for future expansion.  You can change the names from
      'sparen' to something meaningful, but don't change the order.  */

   ubyte clan_rank;
   ubyte spare1;
   ubyte spare2;
   ubyte spare3;
   ubyte spare4;
   ubyte spare5;
   int spells_to_learn;		/* How many can you learn yet this level*/
   int pc_attacks;
   int clan;
   int tpearn;
   int excp;
   int dtimer;
   int ptimer;
   int trage;
   int fishing;
   int spare15;
   int loginc;
   long	bdy;
   long	pref2;
   long	truec;
   long	lustt;
   long	spare21;
};

/*
 * Specials needed only by PCs, not NPCs.  Space for this structure is
 * not allocated in memory for NPCs, but it is for PCs and the portion
 * of it labelled 'saved' is saved in the playerfile.  This structure can
 * be changed freely; beware, though, that changing the contents of
 * player_special_data_saved will corrupt the playerfile.
 */
struct player_special_data {
   struct player_special_data_saved saved;

   char	*poofin;		/* Description on arrival of a god.     */
   char	*poofout;		/* Description upon a god's exit.       */
   char *contact;               /* name of immortal contact             */
   struct alias *aliases;	/* Character's aliases			*/
   long last_tell;		/* idnum of last tell from		*/
   void *last_olc_targ;		/* olc control				*/
   int last_olc_mode;		/* olc control				*/
};


/* Specials used by NPCs, not PCs */
struct mob_special_data {
   byte last_direction;     /* The last direction the monster went     */
   int	attack_type;        /* The Attack Type Bitvector for NPC's     */
   byte default_pos;        /* KAIO position for NPC                */
   memory_rec *memory;	    /* List of attackers to remember	       */
   byte damnodice;          /* The number of damage dice's	       */
   byte damsizedice;        /* The size of the damage dice's           */
   int wait_state;	    /* Wait state for sweeped mobs	       */   
   int num_attacks;         /* Number of attacks		       */

};


/* An affect structure.  Used in char_file_u *DO*NOT*CHANGE* */
struct affected_type {
   sh_int type;          /* The type of spell that caused this      */
   sh_int duration;      /* For how long its effects will last      */
   sbyte modifier;       /* This is added to apropriate ability     */
   byte location;        /* Tells which ability to change(APPLY_XXX)*/
   long	bitvector;       /* Tells which bits to set (AFF_XXX)       */

   struct affected_type *next;
};


/* Structure used for chars following other chars */
struct follow_type {
   struct char_data *follower;
   struct follow_type *next;
};


/* ================== Structure for player/non-player ===================== */
struct char_data {
   int pfilepos;			 /* playerfile pos		  */
   sh_int nr;                            /* Mob's rnum			  */
   room_rnum in_room;                    /* Location (real room number)	  */
   room_rnum was_in_room;		 /* location for linkdead people  */
   int wait;                             /* wait for how many loops       */

   struct char_player_data player;       /* Normal data                   */
   struct char_ability_data real_abils;	 /* Abilities without modifiers   */
   struct char_ability_data aff_abils;	 /* Abils with spells/stones/etc  */
   struct char_point_data points;        /* Points                        */
   struct char_special_data char_specials;	/* PC/NPC specials	  */
   struct player_special_data *player_specials; /* PC specials		  */
   struct mob_special_data mob_specials;	/* NPC specials		  */

   struct affected_type *affected;       /* affected by what spells       */
   struct obj_data *equipment[NUM_WEARS];/* Equipment array               */

   struct obj_data *carrying;            /* Head of list                  */
   struct descriptor_data *desc;         /* NULL for mobiles              */

   long id;                            /* used by DG triggers             */
   struct trig_proto_list *proto_script; /* list of KAIO triggers      */
   struct script_data *script;         /* script info for the object      */
   struct script_memory *memory;       /* for mob memory triggers         */

   struct char_data *next_in_room;     /* For room->people - list         */
   struct char_data *next;             /* For either monster or ppl-list  */
   struct char_data *next_fighting;    /* For fighting list               */

   struct follow_type *followers;        /* List of chars followers       */
   struct char_data *master;             /* Who is char following?        */
};
/* ====================================================================== */


/* ==================== File Structure for Player ======================= */
/*             BEWARE: Changing it will ruin the playerfile		  */
struct char_file_u {
   /* char_player_data */
   char	name[MAX_NAME_LENGTH+1];
   char	description[EXDSCR_LENGTH];
   char	title[MAX_TITLE_LENGTH+1];
   byte sex;
   byte eye;
   byte hairc;
   byte hairl;
   byte aura;
   byte clan;
   byte build;
   byte god;
   byte chclass;
   long long level;
   sh_int hometown;
   time_t birth;   /* Time of birth of character     */
   int	played;    /* Number of secs played in total */
   ubyte weight;
   ubyte height;

   char	pwd[MAX_PWD_LENGTH+1];    /* character's password */

   struct char_special_data_saved char_specials_saved;
   struct player_special_data_saved player_specials_saved;
   struct char_ability_data abilities;
   struct char_point_data points;
   struct affected_type affected[MAX_AFFECT];

   time_t last_logon;		/* Time (in secs) of last logon */
   char host[HOST_LENGTH+1];	/* host of last logon */
};
/* ====================================================================== */


/* descriptor-related structures ******************************************/


struct txt_block {
   char	*text;
   int aliased;
   struct txt_block *next;
};


struct txt_q {
   struct txt_block *head;
   struct txt_block *tail;
};


struct descriptor_data {
   socket_t	descriptor;	/* file descriptor for socket		*/
   char	host[HOST_LENGTH+1];	/* hostname				*/
   byte	bad_pws;		/* number of bad pw attemps this login	*/
   byte idle_tics;		/* tics idle at password prompt		*/
   int	connected;		/* mode of 'connectedness'		*/
   int	wait;			/* wait for how many loops		*/
   int	desc_num;		/* unique num assigned to desc		*/
   time_t login_time;		/* when the person connected		*/
   char *showstr_head;		/* for keeping SENSE of an internal str	*/
   char **showstr_vector;	/* for paging through texts		*/
   int  showstr_count;		/* number of pages to page through	*/
   int  showstr_page;		/* which page are we currently showing?	*/
   char	**str;			/* for the modify-str system		*/
   size_t max_str;	        /*		-			*/
   char *backstr;		/* added for handling abort buffers	*/
   long	mail_to;		/* name for mail system			*/
   int	has_prompt;		/* is the user at a prompt?             */
   char	inbuf[MAX_RAW_INPUT_LENGTH];  /* buffer for raw input		*/
   char	last_input[MAX_INPUT_LENGTH]; /* the last input			*/
   char small_outbuf[SMALL_BUFSIZE];  /* standard output buffer		*/
   char *output;		/* ptr to the current output buffer	*/
   char **history;		/* History of commands, for ! mostly.	*/
   int	history_pos;		/* Circular array position.		*/
   int  bufptr;			/* ptr to end of current output		*/
   int	bufspace;		/* space left in the output buffer	*/
   struct txt_block *large_outbuf; /* ptr to large buffer, if we need it */
   struct txt_q input;		/* q of unprocessed input		*/
   struct char_data *character;	/* linked to char			*/
   struct char_data *original;	/* original char if switched		*/
   struct descriptor_data *snooping; /* Who is this char snooping	*/
   struct descriptor_data *snoop_by; /* And who is snooping this char	*/
   struct descriptor_data *next; /* link to next descriptor		*/
   struct olc_data *olc;	     /*. OLC info - defined in olc.h   .*/
};


/* other miscellaneous structures ***************************************/


struct msg_type {
   char	*attacker_msg;  /* message to attacker */
   char	*victim_msg;    /* message to victim   */
   char	*room_msg;      /* message to room     */
};


struct message_type {
   struct msg_type die_msg;	/* messages when death			*/
   struct msg_type miss_msg;	/* messages when miss			*/
   struct msg_type hit_msg;	/* messages when hit			*/
   struct msg_type god_msg;	/* messages when hit on god		*/
   struct message_type *next;	/* to next messages of this kind.	*/
};


struct message_list {
   int	a_type;			/* Attack type				*/
   int	number_of_attacks;	/* How many attack messages to chose from. */
   struct message_type *msg;	/* List of messages.			*/
};


struct dex_skill_type {
   sh_int p_pocket;
   sh_int p_locks;
   sh_int traps;
   sh_int SWIFTNESS;
   sh_int hide;
};


struct dex_app_type {
   sh_int reaction;
   sh_int miss_att;
   sh_int defensive;
};


struct str_app_type {
   sh_int tohit;    /* To Hit (THAC0) Bonus/Penalty        */
   sh_int todam;    /* Damage Bonus/Penalty                */
   sh_int carry_w;  /* Maximum weight that can be carrried */
   sh_int wield_w;  /* Maximum weight that can be wielded  */
};


struct wis_app_type {
   byte bonus;       /* how many practices player gains per lev */
};


struct int_app_type {
   byte learn;       /* how many % a player learns a spell/skill */
   sh_int hitp;
};


struct con_app_type {
   sh_int hitp;
   sh_int shock;
};

struct spell_entry {
	char* name;
	int races;
	int cost;
	int type;
};

struct weather_data {
   int	pressure;	/* How is the pressure ( Mb ) */
   int	change;	/* How fast and what way does it change. */
   int	sky;	/* How is the sky. */
   int	sunlight;	/* And how much sun. */
};


struct title_type {
   char	*title_m;
   char	*title_f;
   int	exp;
};


/* element in monster and object index-tables   */
struct index_data {
   int	vnum;		/* virtual number of this mob/obj		*/
   int	number;		/* number of existing units of this mob/obj	*/
   SPECIAL(*func);

   char *farg;         /* string argument for special function     */
   struct trig_data *proto;     /* for triggers... the trigger     */
};

/* linked list for mob/object prototype trigger lists */
struct trig_proto_list {
  int vnum;                             /* vnum of the trigger   */
  struct trig_proto_list *next;         /* next trigger          */
};

/* used in the socials */
struct social_messg {
  int act_nr;
  char *command;               /* holds copy of activating command */
  char *sort_as;             /* holds a copy of a similar command or
                             * abbreviation to sort by for the parser */
  int hide;                  /* ? */
  int min_victim_position;   /* Position of victim */
  int min_char_position;     /* Position of char */
  long long min_level_char;          /* Minimum level of socialing char */

  /* No argument was supplied */
  char *char_no_arg;
  char *others_no_arg;

  /* An argument was there, and a victim was found */
  char *char_found;
  char *others_found;
  char *vict_found;

  /* An argument was there, as well as a body part, and a victim was found */
  char *char_body_found;
  char *others_body_found;
  char *vict_body_found;

  /* An argument was there, but no victim was found */
  char *not_found;

  /* The victim turned out to be the character */
  char *char_auto;
  char *others_auto;

  /* If the char cant be found search the char's inven and do these: */
  char *char_obj_found;
  char *others_obj_found;
};


/*struct char_data *is_playing(char *vict_name)
{
  extern struct descriptor_data *descriptor_list;
  struct descriptor_data *i, *next_i;

  for (i = descriptor_list; i; i = next_i) {
    next_i = i->next;
    if(i->connected == CON_PLAYING && !strcmp(i->character->player.name,CAP(vic
t_name)))
      return i->character;
  }
  return NULL;
}*/
