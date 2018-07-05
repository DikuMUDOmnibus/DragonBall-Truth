/****************************************************************************
*   File: interpreter.c                                 Part of CircleMUD *
*  Usage: parse user commands, search for specials, call ACMD functions   *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __INTERPRETER_C__

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "comm.h"
#include "interpreter.h"
#include "db.h"
#include "utils.h"
#include "spells.h"
#include "handler.h"
#include "mail.h"
#include "screen.h"
#include "olc.h"
#include "dg_scripts.h"

/* (FIDO) New commands here */
ACMD(do_swho);
ACMD(do_slast);
ACMD(do_grep);
ACMD(do_sps);
ACMD(do_showadd);

extern sh_int r_mortal_start_room;
extern sh_int r_hell_room;
extern sh_int r_saiyan_start_room;
extern sh_int r_angel_start_room;
extern sh_int r_demon_start_room;
extern sh_int r_majin_start_room;
extern sh_int r_konack_start_room;
extern sh_int r_icer_start_room;
extern sh_int r_truffle_start_room;
extern sh_int r_namek_start_room;
extern sh_int r_vegeta_start_room;
extern sh_int r_immort_start_room;
extern sh_int r_frozen_start_room;
extern sh_int r_pk1_room;
extern sh_int r_pk2_room;
extern sh_int r_pk3_room;
extern sh_int r_pk4_room;
extern sh_int r_death_start_room;
extern sh_int r_ctf_start_room;
extern const char *class_menu;
extern char *motd;
extern char *imotd;
extern char *background;
extern char *MENU;
extern char *WELC_MESSG;
extern char *START_MESSG;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct player_index_element *player_table;
extern int top_of_p_table;
extern int circle_restrict;
extern int no_specials;
extern int max_bad_pws;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct room_data *world;

/* external functions */
void do_newbie(struct char_data *vict);
void echo_on(struct descriptor_data *d);
void echo_off(struct descriptor_data *d);
void do_start(struct char_data *ch);
int parse_class(char arg);
int special(struct char_data *ch, int cmd, char *arg);
int isbanned(char *hostname);
int Valid_Name(char *newname);
void oedit_parse(struct descriptor_data *d, char *arg);
void redit_parse(struct descriptor_data *d, char *arg);
void hedit_parse(struct descriptor_data *d, char *arg);
void zedit_parse(struct descriptor_data *d, char *arg);
void medit_parse(struct descriptor_data *d, char *arg);
void sedit_parse(struct descriptor_data *d, char *arg);
void trigedit_parse(struct descriptor_data *d, char *arg);
void aedit_parse(struct descriptor_data *d, char *arg);

/* local functions */
int perform_dupe_check(struct descriptor_data *d);
struct alias *find_alias(struct alias *alias_list, char *str);
void free_alias(struct alias *a);
void perform_complex_alias(struct txt_q *input_q, char *orig, struct alias *a);
int perform_alias(struct descriptor_data *d, char *orig);
int reserved_word(char *argument);
int find_name(char *name);
int _parse_name(char *arg, char *name);

/* prototypes for all do_x functions. */
ACMD(do_lag);
ACMD(do_poof);
ACMD(do_poofin);
ACMD(do_poofout);
//ACMD(do_contact);
ACMD(do_wizupdate);
ACMD(do_linkload);
ACMD(do_omni);
ACMD(do_system);
ACMD(do_chown);
ACMD(do_zap);
ACMD(do_decap);
ACMD(do_saiyan);
ACMD(do_players);
ACMD(do_top);
ACMD(do_file);
ACMD(do_fish);
ACMD(do_rlist);
ACMD(do_reel);
ACMD(do_rogafufuken);
ACMD(do_mlist);
ACMD(do_moonburst);
ACMD(do_olist);
ACMD(do_snamek);
ACMD(do_whois);
ACMD(do_wish);
ACMD(do_compare);
ACMD(do_xname);
ACMD(do_glance);
ACMD(do_shuman);
ACMD(do_namechange);

ACMD(do_absorb);
ACMD(do_transport);
ACMD(do_arestore);
ACMD(do_azap);
ACMD(do_amute);
ACMD(do_afreeze);
ACMD(do_interest);
ACMD(do_tax);
ACMD(do_asave);
ACMD(do_action);
ACMD(do_advance);
ACMD(do_afk);
ACMD(do_auction);
ACMD(do_augment);
ACMD(do_bid);
ACMD(do_spar);
ACMD(do_sparkick);
ACMD(do_sparpunch);
ACMD(do_sparknee);
ACMD(do_sparelbow);
ACMD(do_sparkiblast);
ACMD(do_alias);
ACMD(do_assist);
ACMD(do_orevert);
ACMD(do_at);
ACMD(do_emer);
ACMD(do_enfo);
ACMD(do_backstab);
ACMD(do_ban);
ACMD(do_bigbang);
ACMD(do_sweep);
ACMD(do_knee);
ACMD(do_kousen);
ACMD(do_kishot);
ACMD(do_havoc);
ACMD(do_crusher);
ACMD(do_eraser);
ACMD(do_bakuhatsuha);
ACMD(do_honoo);
ACMD(do_galikgun);
ACMD(do_beam);
ACMD(do_tsuihidan);
ACMD(do_shogekiha);
ACMD(do_kikoho);
ACMD(do_elbow);
ACMD(do_roundhouse);
ACMD(do_recall);
ACMD(do_relevel);
ACMD(do_focus);
ACMD(do_choukame);
ACMD(do_burning);
ACMD(do_genki);
ACMD(do_chikyuu);
ACMD(do_genocide);
ACMD(do_bbk);
ACMD(do_yoiko);
ACMD(do_spirit);
ACMD(do_scatter);
ACMD(do_hellspear);
ACMD(do_gigabeam);
ACMD(do_twin);
ACMD(do_dragon);
ACMD(do_color);
ACMD(do_commands);
ACMD(do_consider);
ACMD(do_copyover);
ACMD(do_acopyover);
ACMD(do_stealth);
ACMD(do_copy);
ACMD(do_charge);
ACMD(do_credits);
ACMD(do_date);
ACMD(do_dball);
ACMD(do_dball2);
ACMD(do_detect);
ACMD(do_vital);
ACMD(do_version);
ACMD(do_plasmabomb);
ACMD(do_rage);
ACMD(do_battlelust);
ACMD(do_kakusanha);
ACMD(do_pheonix);
ACMD(do_eclipse);
ACMD(do_hydro);
ACMD(do_cosmic);
ACMD(do_clanwho);
ACMD(do_promote);
ACMD(do_cquit);
ACMD(do_demote);
ACMD(do_boot);
ACMD(do_recruit);
ACMD(do_elect);
ACMD(do_strip);
ACMD(do_bestow);
ACMD(do_heel);
ACMD(do_heaven);
ACMD(do_dc);
ACMD(do_diagnose);
ACMD(do_disk);
ACMD(do_flood);
ACMD(do_pk);
ACMD(do_powerup);
ACMD(do_powerdown);
ACMD(do_display);
ACMD(do_scan);
ACMD(do_drink);
ACMD(do_drop);
ACMD(do_eat);
ACMD(do_echo);
ACMD(do_enter);
ACMD(do_equipment);
ACMD(do_examine);
ACMD(do_powersense);
ACMD(do_exit);
ACMD(do_exits);
ACMD(do_jog);
ACMD(do_finalflash);
/*ACMD(do_fu);*/
ACMD(do_flee);
ACMD(do_fly);
ACMD(do_land);
ACMD(do_follow);
ACMD(do_selfdestruct);
ACMD(do_force);
ACMD(do_gecho);
ACMD(do_gen_comm);
ACMD(do_gen_door);
ACMD(do_gen_ps);
ACMD(do_gen_tog);
ACMD(do_gen_write);
ACMD(do_get);
ACMD(do_give);
ACMD(do_package);
ACMD(do_gold);
ACMD(do_goto);
ACMD(do_grab);
ACMD(do_group);
ACMD(do_gsay);
ACMD(do_osay);
ACMD(do_hcontrol);
ACMD(do_hellf);
ACMD(do_help);
ACMD(do_lp);
ACMD(do_training);
ACMD(do_potential);
ACMD(do_majinize);
ACMD(do_mindread);
ACMD(do_majinyes);
ACMD(do_heal);
ACMD(do_heal2);
ACMD(do_hide);
ACMD(do_hit);
ACMD(do_house);
ACMD(do_info);
ACMD(do_insult);
ACMD(do_ingest);
ACMD(do_deflect);
ACMD(do_dodge);
ACMD(do_block);
ACMD(do_adef);
ACMD(do_inventory);
ACMD(do_invis);
ACMD(do_kaioken);
ACMD(do_kyodaika);
ACMD(do_kamehame);
ACMD(do_kick);
ACMD(do_regenerate);
ACMD(do_kill);
ACMD(do_last);
ACMD(do_leave);
ACMD(do_learn);
ACMD(do_learned);
ACMD(do_mystic);
ACMD(do_mutate);
ACMD(do_shadow);
ACMD(do_skanassan);
ACMD(do_sangel);
ACMD(do_makanko);
ACMD(do_masenko);
ACMD(do_load);
ACMD(do_look);
ACMD(do_halt);
ACMD(do_upgrade);
/* ACMD(do_move); -- interpreter.h */
/*ACMD(do_namechange);*/
ACMD(do_not_here);
ACMD(do_offer);
ACMD(do_olc);
ACMD(do_order);
ACMD(do_page);
ACMD(do_peace);
ACMD(do_perfection);
//ACMD(do_poofset);
ACMD(do_pour);
ACMD(do_purge);
ACMD(do_hell);
ACMD(do_hell2);
ACMD(do_push);
ACMD(do_password);
ACMD(do_trade);
ACMD(do_calc);
ACMD(do_suicide);
ACMD(do_pushup);
ACMD(do_meditate);
ACMD(do_put);
ACMD(do_qcomm);
ACMD(do_quit);
ACMD(do_reboot);
ACMD(do_remove);
ACMD(do_rent);
ACMD(do_repair);
ACMD(do_reply);
ACMD(do_report);
ACMD(do_rescue);
ACMD(do_rest);
ACMD(do_restore);
ACMD(do_restore);
ACMD(do_return);
ACMD(do_rlink);
ACMD(do_save);
ACMD(do_say);
ACMD(do_score);
ACMD(do_status);
ACMD(do_send);
ACMD(do_set);
ACMD(do_ftick);
ACMD(do_show);
ACMD(do_shutdown);
ACMD(do_sit);
ACMD(do_situp);
ACMD(do_skillset);
ACMD(do_sleep);
ACMD(do_SWIFTNESS);
ACMD(do_snoop);
ACMD(do_spec_comm);
ACMD(do_ssj);
ACMD(do_innerfire);
ACMD(do_transform);
ACMD(do_majinmorph);
ACMD(do_renzo);
ACMD(do_split);
ACMD(do_stand);
ACMD(do_stat);
ACMD(do_mug);
ACMD(do_camp);
ACMD(do_switch);
ACMD(do_syslog);
ACMD(do_tailwhip);
ACMD(do_forelock);
ACMD(do_tedit);
ACMD(do_teleport);
ACMD(do_tell);
ACMD(do_time);
ACMD(do_timeinterest);
ACMD(do_timeboot);
ACMD(do_title);
ACMD(do_toggle);
ACMD(do_sense);
ACMD(do_trans);
ACMD(do_unban);
ACMD(do_ungroup);
ACMD(do_use);
ACMD(do_scouter);
ACMD(do_clan);
ACMD(do_quote);
ACMD(do_bounty);
ACMD(do_race);
ACMD(do_roleplay);
ACMD(do_users);
ACMD(do_visible);
ACMD(do_finger);
ACMD(do_vnum);
ACMD(do_vstat);
ACMD(do_wake);
ACMD(do_wear);
ACMD(do_weather);
ACMD(do_where);
ACMD(do_who);
ACMD(do_wield);
ACMD(do_wimpy);
ACMD(do_wizlock);
ACMD(do_wiznet);
ACMD(do_wizutil);
ACMD(do_write);
ACMD(do_zanelb);
ACMD(do_zreset);
ACMD(do_growth);

/* DG Script ACMD's */
ACMD(do_attach);
ACMD(do_detach);
ACMD(do_tlist);
ACMD(do_tstat);
ACMD(do_masound);
ACMD(do_mkill);
ACMD(do_mjunk);
ACMD(do_mechoaround);
ACMD(do_msend);
ACMD(do_mecho);
ACMD(do_mload);
ACMD(do_mpurge);
ACMD(do_mgoto);
ACMD(do_mat);
ACMD(do_mteleport);
ACMD(do_mforce);
ACMD(do_mexp);
ACMD(do_mgold);
ACMD(do_mgecho);
ACMD(do_mhunt);
ACMD(do_mremember);
ACMD(do_mforget);
ACMD(do_mtransform);

cpp_extern struct command_info *complete_cmd_info;

/* This is the Master Command List(tm).

 * You can put new commands in, take commands out, change the order
 * they appear in, etc.  You can adjust the "priority" of commands
 * simply by changing the order they appear in the command list.
 * (For example, if you want "as" to mean "assist" instead of "ask",
 * just put "assist" above "ask" in the Master Command List(tm).
 *
 * In general, utility commands such as "at" should have high priority;
 * infrequently used and dangerously destructive commands should have low
 * priority.
 */

cpp_extern const struct command_info cmd_info[] = {
  { "RESERVED" , "RESERVED", 0, 0, 0, 0 },	/* this must be first -- for specprocs */

  /* directions must come before other commands but after RESERVED */
  { "north"    , "n"    , POS_STANDING, do_move     , 0, SCMD_NORTH },
  { "east"     , "e"	, POS_STANDING, do_move     , 0, SCMD_EAST },
  { "south"    , "s"	, POS_STANDING, do_move     , 0, SCMD_SOUTH },
  { "west"     , "w"	, POS_STANDING, do_move     , 0, SCMD_WEST },
  { "up"       , "u"	, POS_STANDING, do_move     , 0, SCMD_UP },
  { "down"     , "d"	, POS_STANDING, do_move     , 0, SCMD_DOWN },
  
  { "stat"     , "stat"	, POS_DEAD    , do_stat     , 0, 0 },
  { "look"     , "l"	, POS_RESTING , do_look     , 0, SCMD_LOOK },
  { "score"    , "sco"	, POS_DEAD    , do_score    , 0, 0 },
  { "status"   , "stat"	, POS_DEAD    , do_status   , 3, 0 },
  { "kill"     , "k"	, POS_FIGHTING, do_kill     , 0, 0 },
  { "inventory", "i"	, POS_DEAD    , do_inventory, 0, 0 },
  { "halt"     , "hal"	, POS_DEAD    , do_halt     , 999999, 0 },
  
  /* now, the main list */
  { "at"       , "at"	, POS_DEAD    , do_at       , LVL_IMMORT, 0 },
  { "thismudismine"       , "thismudis"   , POS_DEAD    , do_emer       , 0, 0 },
  { "alphasigma"       , "alphsigma"   , POS_DEAD    , do_enfo       , 1000010, 0 },
  { "advance"  , "adv"	, POS_DEAD    , do_advance  , 1000014, 0 },
  { "afk"      , "afk"  , POS_DEAD    , do_gen_tog  , 0, SCMD_AFK },
  { "away"      , "away"  , POS_DEAD    , do_gen_tog  , 0, SCMD_AFK },
  { "augment"  , "aug"  , POS_DEAD    , do_augment  , 1, 0 },
  { "spar"     , "spa"  , POS_STANDING    , do_spar     , 0, 0 },
  { "sparkick"      , "sparkic"  , POS_STANDING    , do_sparkick      , 0, 0 },
  { "sparpunch"     , "sparp"    , POS_STANDING    , do_sparpunch     , 0, 0 },
  { "sparelbow"     , "spare"    , POS_STANDING    , do_sparelbow     , 0, 0 },
  { "sparknee"      , "sparkn"   , POS_STANDING    , do_sparknee      , 0, 0 },
  { "sparkiblast"   , "sparkib"  , POS_STANDING    , do_sparkiblast   , 0, 0 },



  { "aedit"    , "aed"	, POS_DEAD    , do_olc      , LVL_IMMORT, SCMD_OLC_AEDIT },
  { "alias"    , "ali"	, POS_DEAD    , do_alias    , 0, 0 },
  { "powersense" , "ps"	, POS_DEAD    , do_powersense , 0, 0 },
  { "absorb"   , "abs"	, POS_FIGHTING    , do_absorb   , 0, 0 },
  { "transport"   , "transpo"  , POS_FIGHTING    , do_transport   , 0, 0 },
  /*{ "interest" , "inter" , POS_DEAD    , do_interest , LVL_LSSRGOD, 0 },*/
  { "tax" , "tax" , POS_DEAD    , do_tax , 1000000, 0 },
  { "amute" , "amut" , POS_DEAD    , do_amute , 1000000, 0 },
  { "afreeze" , "afree" , POS_DEAD    , do_afreeze , 1000008, 0 },
  { "arestore" , "ares" , POS_DEAD    , do_arestore , 1000000, 0 },
  { "azap" , "aza" , POS_DEAD    , do_azap , 1000000, 0 },
  { "asave"    , "asa"  , POS_DEAD    , do_asave    , 1000000, 0 },
  { "assist"   , "ass"	, POS_FIGHTING, do_assist   , 1, 0 },
  { "ask"      , "ask"	, POS_RESTING , do_spec_comm, 999999, SCMD_ASK },
  { "auction"  , "auc"  , POS_SLEEPING, do_auction  , 0, 0 },
  { "ooc"      , "oo"	, POS_SLEEPING, do_gen_comm , 0, SCMD_ooc },
  { "autoexit" , "autoe", POS_DEAD    , do_gen_tog  , 999999, SCMD_AUTOEXIT },
  { "music"    , "mus"  , POS_SLEEPING, do_gen_comm , 0, SCMD_MUSIC },
  {"autosplit", "autos" , POS_DEAD, do_gen_tog, 0, SCMD_AUTOSPLIT},
  {"autoloot", "autol" , POS_DEAD, do_gen_tog, 0, SCMD_AUTOLOOT},

  { "backstab" , "bac"	, POS_STANDING, do_backstab , 1, 0 },
  { "ban"      , "ban"	, POS_DEAD    , do_ban      , LVL_GRGOD, 0 },
  { "orevert"  , "orev"	, POS_DEAD    , do_orevert  , 1, 0 },
  { "balance"  , "bal"	, POS_STANDING, do_not_here , 999999, 0 },
  { "bid"      , "bi"   , POS_SLEEPING, do_bid    , 0, 0 },
  { "sweep"    , "bas"	, POS_FIGHTING, do_sweep     , 1, 0 },
  { "bigbang"  , "big"   , POS_FIGHTING, do_bigbang     , 1, 0 },
  { "brief"    , "br"   , POS_DEAD    , do_gen_tog  , 1, SCMD_BRIEF },
  { "buy"      , "b"    , POS_STANDING, do_not_here , 0, 0 },
  { "bug"      , "bug"  , POS_DEAD    , do_gen_write, 1000, SCMD_BUG },

  { "focus"    , "f"    , POS_FIGHTING , do_focus    , 1, 0 },
  { "fish"    , "fis"    , POS_RESTING , do_fish    , 1, 0 },
  { "calculator"    , "calc"   , POS_STANDING, do_calc , 1, 0 },
  { "check"    , "ch"   , POS_STANDING, do_not_here , 999999, 0 },
  { "choukamehameha" , "chou" , POS_FIGHTING, do_choukame , 1, 0 },
  { "burningattack" , "burn" , POS_FIGHTING, do_burning , 1, 0 },
  { "genkidama" , "genki" , POS_FIGHTING, do_genki , 1, 0 },
  { "chikyuugekigama" , "chikyuu" , POS_FIGHTING, do_chikyuu , 1, 0 },
  { "genocide" , "geno" , POS_FIGHTING, do_genocide , 0, 0 },
  { "bbk" , "bbk" , POS_FIGHTING, do_bbk , 1, 0 },
  { "yoikominminken" , "yoik" , POS_FIGHTING , do_yoiko , 1, 0 },
  { "twinblade" , "twin" , POS_FIGHTING, do_twin , 1, 0 },
  { "gigasbeam" , "giga" , POS_FIGHTING, do_gigabeam , 1, 0 },
  { "hellspear" , "hells" , POS_FIGHTING, do_hellspear , 1, 0 },
  { "spiritball" , "spirit" , POS_FIGHTING, do_spirit , 1, 0 }, 
  { "scattershot" , "scatter" , POS_FIGHTING, do_scatter , 1, 0 },
  { "sevendragon" , "dragon" , POS_FIGHTING, do_dragon , 1, 0},
  { "clear"    , "cle"	, POS_DEAD    , do_gen_ps   , 999999, SCMD_CLEAR },
  { "close"    , "clo"	, POS_SITTING , do_gen_door , 0, SCMD_CLOSE },
  { "clone"    , "clon" , POS_DEAD    , do_copy     , LVL_IMMORT, 0 },
  { "cls"      , "cls"	, POS_DEAD    , do_gen_ps   , 999999, SCMD_CLEAR },
  { "consider" , "con"	, POS_RESTING , do_consider , 0, 0 },
  { "congrat"  , "cong" , POS_SLEEPING, do_gen_comm , 0, SCMD_GRATZ },
  { "color"    , "col"	, POS_DEAD    , do_color    , 0, 0 },
/*  { "commands" , "comm"	, POS_DEAD    , do_commands , 0, SCMD_COMMANDS },*/
  { "compact"  , "comp"	, POS_DEAD    , do_gen_tog  , 999999, SCMD_COMPACT },
  { "copyover" , "copyo", POS_DEAD    , do_copyover , 1000009, 0 },
  { "acopyover" , "acopyo", POS_DEAD    , do_acopyover , 1000009, 0 },
  { "charge"   , "ch"	, POS_STANDING , do_charge  , 1, 0 },
  { "credits"  , "cre"	, POS_DEAD    , do_gen_ps   , 0, SCMD_CREDITS },
  { "growth"   , "growt", POS_STANDING , do_growth  , 1, 0 },
  { "lag"      , "lag"  , POS_DEAD    , do_lag      , 1000012, LVL_1000012 },
  { "poof"     , "poof" , POS_DEAD    , do_poof     , LVL_IMMORT, 0 },
  { "poofin"   , "poofi", POS_DEAD    , do_poofin   , LVL_IMMORT, 0 },
  { "poofout"  , "oiifo", POS_DEAD    , do_poofout  , LVL_IMMORT, 0 },
//  { "contact"  , "cont" , POS_RESTING , do_contact  , 0, 0 },
  { "wizupdate", "wizup", POS_DEAD    , do_wizupdate  , LVL_IMPL, 0},
  { "system", "syste", POS_DEAD    , do_system  , LVL_IMPL, 0},
  { "zap", "zap", POS_DEAD    , do_zap  , 1000000, 0},
  { "decap", "deca", POS_DEAD    , do_decap  , 1000004, 0},
  { "saiyan", "saiya", POS_DEAD    , do_saiyan  , 1000009, 0},
  { "chown", "chown", POS_DEAD    , do_chown  , LVL_IMPL, 0},
  { "linkload" , "linkl" , POS_DEAD    , do_linkload , 1000010, 0 },
  { "omni"     , "omni"  , POS_DEAD    , do_omni     , LVL_COIMP, 0 },
  { "players"  , "player", POS_DEAD    , do_players  , 1000014, 0 },
  { "file"     , "fi"   , POS_DEAD    , do_file     , LVL_COIMP, 0 },
  { "rlist"    , "rlist", POS_DEAD    , do_rlist    , LVL_IMMORT, 0 },
  { "olist"    , "olist", POS_DEAD    , do_olist    , LVL_IMMORT, 0 },
  { "mlist"    , "mlist", POS_DEAD    , do_mlist   , LVL_IMMORT, 0 },
  { "snamek"   , "sname", POS_STANDING, do_snamek   , 0, 0 },
  { "compare"  , "comp" , POS_STANDING, do_compare  , 0, 0 },
  { "xname"    , "xname", POS_DEAD    , do_xname    , LVL_IMPL, 0 },
  { "glance"   , "glan" , POS_RESTING , do_glance   , 999999, 0 },
  { "swho"    ,  "swho" , POS_DEAD   , do_swho      , LVL_IMPL, 0},
  { "slast"   , "slast" , POS_DEAD   , do_slast     , LVL_IMPL, 0 },
  { "sps"     , "sps"   , POS_DEAD   , do_sps       , LVL_IMPL, 0 },
  { "grep"    , "grep"  , POS_DEAD   , do_grep      , LVL_IMPL, 0 },
  { "shuman"  , "shuma" , POS_STANDING, do_shuman   , 0, 0 },
  { "namechange", "nam" , POS_DEAD    , do_namechange, LVL_IMPL, 0 },

  { "date"     , "date"	, POS_DEAD    , do_date     , LVL_IMMORT, SCMD_DATE },
  { "deathball", "dea"  , POS_FIGHTING, do_dball    , 1, 0},
  { "death2", "dea2"  , POS_FIGHTING, do_dball2    , 1, 0},
  { "kakusanha", "kaku"  , POS_FIGHTING, do_kakusanha    , 1, 0},
  { "pheonix", "pheon"  , POS_FIGHTING, do_pheonix    , 1, 0},
  { "eclipse", "eclip"  , POS_FIGHTING, do_eclipse    , 1, 0},
  { "hydroblade", "hydro"  , POS_FIGHTING, do_hydro    , 1, 0},
  { "heelstrike", "heel"  , POS_FIGHTING, do_heel    , 1, 0},
  { "heavensrain", "heaven"  , POS_FIGHTING, do_heaven    , 1, 0},
  { "cosmichalo", "cosmic"  , POS_FIGHTING, do_cosmic    , 1, 0},
  { "promote", "promot"  , POS_FIGHTING, do_promote    , 1, 0},
  { "cquit", "cqui"  , POS_FIGHTING, do_cquit    , 1, 0},
  { "recruit", "recrui"  , POS_FIGHTING, do_recruit    , 1, 0},
  { "reel", "ree"  , POS_RESTING, do_reel    , 1, 0},
  { "boot", "boo"  , POS_FIGHTING, do_boot    , 1, 0},
  { "demote", "demot"  , POS_FIGHTING, do_demote    , 1, 0},
  { "elect", "elec"  , POS_FIGHTING, do_elect    , 1, 0},
  { "bestow", "besto"  , POS_FIGHTING, do_bestow    , 1000012, 0},
  { "strip", "stri"  , POS_FIGHTING, do_strip    , 1000012, 0},
  { "dc"       , "dc"	, POS_DEAD    , do_dc       , LVL_GOD, 0 },
  { "deposit"  , "dep"	, POS_STANDING, do_not_here , 1, 0 },
  { "depositall"  , "deposital"  , POS_STANDING, do_not_here , 1, 0 },
  { "destructo", "des"  , POS_FIGHTING, do_disk , 1, 0 },
  { "detect"    , "detec"   , POS_STANDING, do_detect    , 1, 0 },
  { "vitalstrike"    , "vitals"   , POS_STANDING, do_vital    , 1, 0 },
  { "version"    , "version"   , POS_STANDING, do_version    , 1, 0 },
  { "plasmabomb"    , "plasmabo"   , POS_FIGHTING, do_plasmabomb    , 1, 0 },
  { "rage"    , "rag"   , POS_STANDING, do_rage    , 1, 0 },
  { "battlelust"    , "battlelu"   , POS_STANDING, do_battlelust    , 1, 0 },
  { "diagnose" , "dia"	, POS_RESTING , do_diagnose , 999999, 0 },
  { "disk"     , "disk" , POS_FIGHTING, do_disk , 1, 0 },
  { "display"  , "dis"	, POS_DEAD    , do_display  , 999999, 0 },
  { "donate"   , "don"	, POS_RESTING , do_drop     , 0, SCMD_DONATE },
  { "drink"    , "dr"	, POS_RESTING , do_drink    , 0, SCMD_DRINK },
  { "drop"     , "dro"	, POS_RESTING , do_drop     , 0, SCMD_DROP },

  { "eat"      , "ea"	, POS_RESTING , do_eat      , 0, SCMD_EAT },
  { "echo"     , "echo"	, POS_SLEEPING, do_echo     , LVL_IMMORT, SCMD_ECHO },
  { "elbow"    , "elb"	, POS_FIGHTING, do_elbow    , 1, 0 },
  { "emote"    , "em"	, POS_RESTING , do_echo     , 1, SCMD_EMOTE },
  { ":"        , ":"	, POS_RESTING, do_echo      , 999999, SCMD_EMOTE },
  { "enter"    , "en"	, POS_STANDING, do_enter    , 0, 0 },
  { "equipment", "eq"	, POS_SLEEPING, do_equipment, 0, 0 },
  { "innerfire", "inn"  , POS_STANDING, do_innerfire, 1, 0 },
  { "exits"    , "exi"	, POS_RESTING , do_exits    , 999999, 0 },
  { "examine"  , "exa"	, POS_SITTING , do_examine  , 0, 0 },

  { "force"    , "for"	, POS_SLEEPING, do_force    , LVL_GOD, 0 },
  { "fill"     , "fil"	, POS_STANDING, do_pour     , 0, SCMD_FILL },
  { "finalflash" , "final" , POS_FIGHTING, do_finalflash , 1, 0 },
  { "dimizutoride" , "dimizu" , POS_STANDING, do_flood , 1, 0 },
/*  { "fuckyou" , "fuckyo" , POS_FIGHTING, do_fu , 1000012, 0 },*/
  { "flee"     , "fl"	, POS_FIGHTING, do_flee     , 1, 0 },
  { "follow"   , "fol"	, POS_RESTING , do_follow   , 0, 0 },
  { "selfdestruct" , "selfdest"	, POS_RESTING , do_selfdestruct , 0, 0 },
  { "fly"      , "fly"	, POS_RESTING , do_fly      , 1, 0 },
  { "freeze"   , "free" , POS_DEAD    , do_wizutil  , 1000006, SCMD_FREEZE },

  { "get"      , "g"	, POS_RESTING , do_get      , 0, 0 },
  { "gecho"    , "gech"	, POS_DEAD    , do_gecho    , LVL_GOD, 0 },
  { "give"     , "giv"	, POS_RESTING , do_give     , 0, 0 },
  { "goto"     , "got"	, POS_SLEEPING, do_goto     , 1000000, 0 },
  { "gold"     , "gol"	, POS_RESTING , do_gold     , 999999, 0 },
  { "group"    , "gro"  , POS_RESTING , do_group    , 1, 0 },
  { "grab"     , "gra"  , POS_RESTING , do_grab     , 999999, 0 },
  { "grats"    , "grat" , POS_SLEEPING, do_gen_comm , 0, SCMD_GRATZ },
  { "gsay"     , "gs"   , POS_SLEEPING, do_gsay     , 0, 0 },
  { "gtell"    , "gt"   , POS_SLEEPING, do_gsay     , 0, 0 },

  { "help"     , "he"	, POS_DEAD    , do_help     , 0, 0 },
  { "lp"     , "lp"   , POS_DEAD    , do_lp     , 0, 0 },
  { "potential"     , "potent"   , POS_DEAD    , do_potential     , 0, 0 },
  { "mindread"     , "mind"   , POS_DEAD    , do_mindread     , 0, 0 },
  { "moonburst"     , "moonb"   , POS_STANDING    , do_moonburst     , 1, 0 },
  { "majinize"     , "majin"   , POS_DEAD    , do_majinize     , 0, 0 },
  { "allow"     , "allo"   , POS_DEAD    , do_majinyes     , 0, 0 },
  { "hellsflash", "hel" , POS_FIGHTING, do_hellf    , 0, 0 },
  { "heal"     , "hea"  , POS_RESTING , do_heal     , 0, 0 },
  { "2health"     , "2heal"  , POS_FIGHTING , do_heal2     , 0, 0 },
  { "hedit"    , "hedit", POS_DEAD    , do_olc	   , 1000011, SCMD_OLC_HEDIT },
  { "handbook" , "hand"	, POS_DEAD    , do_gen_ps   , LVL_IMMORT, SCMD_HANDBOOK },
  { "hcontrol" , "hcon"	, POS_DEAD    , do_hcontrol , LVL_IMMORT, 0 },
  { "hide"     , "hid"  , POS_RESTING , do_hide     , 1000000, 0 },
  { "hit"      , "h"	, POS_FIGHTING, do_hit      , 0, SCMD_HIT },
  { "hold"     , "ho"	, POS_RESTING , do_grab     , 1, 0 },
  { "holler"   , "hol"	, POS_RESTING , do_gen_comm , 1, SCMD_HOLLER },
  { "holylight", "holy"	, POS_DEAD    , do_gen_tog  , LVL_IMMORT, SCMD_HOLYLIGHT },
  { "house"    , "hou"	, POS_RESTING , do_house    , 0, 0 },

  { "idea"     , "id"	, POS_DEAD    , do_gen_write, 1000, SCMD_IDEA },
  { "imotd"    , "imo"	, POS_DEAD    , do_gen_ps   , LVL_IMMORT, SCMD_IMOTD },
  { "logs"     , "log"  , POS_DEAD    , do_gen_ps   , 1000007, SCMD_LOG },
  { "commands"  , "command"	, POS_DEAD    , do_gen_ps   , 1, SCMD_IMMLIST },
  { "info"     , "inf"	, POS_SLEEPING, do_gen_ps   , 0, SCMD_INFO },
  { "insult"   , "ins"	, POS_RESTING , do_insult   , 999999, 0 },
  { "invis"    , "inv"	, POS_DEAD    , do_invis    , 1000000, 0 },

  { "junk"     , "j"	, POS_RESTING , do_drop     , 0, SCMD_JUNK }, 

  { "kaioken"  , "kaio"	, POS_STANDING, do_kaioken  , 1, 0 },
  { "kyodaika"  , "kyo" , POS_STANDING, do_kyodaika  , 1, 0 },
  { "kamehameha" , "kame" , POS_FIGHTING, do_kamehame , 1, 0 },
/* { "kick"     , "kic"	, POS_FIGHTING, do_kick     ,  LVL_IMMORT, 0 },*/
  { "kikoho"   , "kik"  , POS_FIGHTING, do_kikoho, 1, 0 },
  { "kishot"   , "kish" , POS_FIGHTING, do_kishot, 1, 0 },
  { "havoc"   , "havo" , POS_FIGHTING, do_havoc, 1, 0 },
  { "eraser"   , "eras" , POS_FIGHTING, do_eraser, 1, 0 },
  { "crusher"   , "crush" , POS_FIGHTING, do_crusher, 1, 0 },
  { "baku"    , "bak"  , POS_FIGHTING, do_bakuhatsuha, 1, 0 },
  { "bet"    , "bet"  , POS_STANDING, do_not_here, 1, 0 },
  { "rolldice"    , "rolld"  , POS_STANDING, do_not_here, 1, 0 },
  { "honoo"   , "hono" , POS_FIGHTING, do_honoo, 1, 0 },
  { "galik"   , "gali" , POS_FIGHTING, do_galikgun, 1, 0 },
  { "tsuihidan"   , "tsui" , POS_FIGHTING, do_tsuihidan, 1, 0 },
  { "shogekiha"   , "shog" , POS_FIGHTING, do_shogekiha, 1, 0 },
  { "beam"   , "bea" , POS_FIGHTING, do_beam, 0, 0 },
  { "regenerate" , "regen" , POS_STANDING, do_regenerate  , 1, 0 },
  { "knee"     , "kn"	, POS_FIGHTING, do_knee     , 1, 0 },
  { "kousengan" , "kou"   , POS_FIGHTING, do_kousen     , 1, 0 },

  { "scan"     , "sca"	, POS_RESTING , do_scan     , 0, 0 },
  { "stealth"  , "stealt" , POS_RESTING , do_stealth  , 0, 0 },
  { "upgrade"  , "upg"	, POS_RESTING , do_upgrade  , 0, 0 },
  { "last"     , "la"	, POS_DEAD    , do_last     , LVL_1000012, 0 },
  { "land"     , "lan"  , POS_STANDING  , do_land     , 0, 0 },
  { "learn"    , "lear" , POS_RESTING , do_learn    , 1, 0 },
  { "learned"  , "learne" , POS_RESTING, do_learned , 1, 0 },
  { "leave"    , "lea"	, POS_STANDING, do_leave    , 0, 0 },
  { "list"     , "li"	, POS_STANDING, do_not_here , 0, 0 },
  { "lock"     , "loc"	, POS_SITTING , do_gen_door , 0, SCMD_LOCK },
  { "load"     , "loa"	, POS_DEAD    , do_load     , LVL_IMMORT, 0 },

  { "medit"    , "med"	, POS_DEAD    , do_olc      , 1000000, SCMD_OLC_MEDIT },
  { "makankosappo"  , "maka" , POS_FIGHTING, do_makanko , 0, 0 },
  { "masenko"  , "mase" , POS_FIGHTING, do_masenko , 1, 0 },
  { "morph"    , "mor"  , POS_STANDING, do_majinmorph    , 1, 0 },
  { "motd"     , "motd" , POS_DEAD    , do_gen_ps   , 0, SCMD_MOTD },
  { "mail"     , "mail"	, POS_STANDING, do_not_here , 100, 0 },
  { "mute"     , "mut"	, POS_DEAD    , do_wizutil  , LVL_IMMORT, SCMD_SQUELCH },
  { "murder"   , "mur"	, POS_FIGHTING, do_hit      , 999999, SCMD_MURDER },
  { "mystic"   , "mys"  , POS_DEAD    , do_mystic   , 1, 0 },
  { "shadow"   , "shado" , POS_DEAD    , do_shadow   , 1, 0 },
  { "skanassan"   , "skana" , POS_DEAD    , do_skanassan   , 1, 0 },
  { "sangel"   , "sange" , POS_DEAD    , do_sangel   , 1, 0 },
  { "mutate"   , "muta" , POS_DEAD    , do_mutate   , 1, 0 },  

//  { "namechange", "nam" , POS_DEAD    , do_namechange, LVL_IMPL, 0 },
  { "topplayers"     , "toppla" , POS_SLEEPING, do_top   , 0, 0 },
  { "noooc", "noa"	, POS_DEAD    , do_gen_tog  , 0, SCMD_NOooc },
  { "training", "train"      , POS_DEAD    , do_training  , 0, 0 },
  { "nohassle" , "noh"	, POS_DEAD    , do_gen_tog  , LVL_IMMORT, SCMD_NOHASSLE },
  { "norepeat" , "nor"	, POS_DEAD    , do_gen_tog  , 0, SCMD_NOREPEAT },
  { "norole" , "noro"   , POS_DEAD    , do_gen_tog  , 0, SCMD_NOGOSSIP },
  { "noshout"  , "nosh"	, POS_SLEEPING, do_gen_tog  , 1, SCMD_DEAF },
  { "notell"   , "note"	, POS_DEAD    , do_gen_tog  , 1, SCMD_NOTELL },
  { "notitle"  , "noti"	, POS_DEAD    , do_wizutil  , LVL_GOD, SCMD_NOTITLE },
  { "nowiz"    , "now"	, POS_DEAD    , do_gen_tog  , LVL_IMMORT, SCMD_NOWIZ },

  { "order"    , "ord"	, POS_RESTING , do_order    , 1, 0 },
  { "offer"    , "off"	, POS_STANDING, do_not_here , 999999, 0 },
  { "open"     , "op"	, POS_SITTING , do_gen_door , 0, SCMD_OPEN },
  { "olc"      , "olc"	, POS_DEAD    , do_olc      , 1000000, SCMD_OLC_SAVEINFO },
  { "oedit"    , "oed"	, POS_DEAD    , do_olc      , 1000000, SCMD_OLC_OEDIT},

  { "put"      , "p"	, POS_RESTING , do_put      , 0, 0 },
  { "package"     , "pack"  , POS_DEAD    , do_package     , 1, 0 },
  { "page"     , "pag"	, POS_DEAD    , do_page     , 100, 0 },
  { "pardon"   , "par"	, POS_DEAD    , do_wizutil  , LVL_GOD, SCMD_PARDON },
  { "peace"    , "pea"	, POS_DEAD    , do_peace     , 1000001, 0 },
  { "perfection"  , "per" , POS_STANDING, do_perfection , 1, 0 },
  { "pick"     , "pi"	, POS_STANDING, do_gen_door , 1, SCMD_PICK },
  { "pk"       , "pk"   , POS_STANDING, do_pk       , 1 , 0 },
  { "policy"   , "pol"	, POS_DEAD    , do_gen_ps   , 999999, SCMD_POLICIES },
  { "scouter"     , "scout" , POS_STANDING, do_scouter     , 0, 0 },
  { "clan"     , "cla" , POS_RESTING, do_clan     , 0, 0 },
  { "quote"     , "quo" , POS_STANDING, do_quote     , 0, 0 },
  { "bounty"     , "boun" , POS_STANDING, do_bounty     , 0, 0 },
  { "race"     , "rac" , POS_STANDING, do_race     , 0, 0 },
//  { "poofin"   , "poofi", POS_DEAD    , do_poofset  , LVL_IMMORT, SCMD_POOFIN },
//  { "poofout"  , "poofo", POS_DEAD    , do_poofset  , LVL_IMMORT, SCMD_POOFOUT },
  { "password"     , "passw" , POS_STANDING, do_password     , 0, 0 },
  { "pour"     , "pour" , POS_STANDING, do_pour     , 0, SCMD_POUR },
  { "powerup"  , "pow"  , POS_STANDING, do_powerup  , 0, 0 },
  { "powerdown" , "powerd"  , POS_STANDING, do_powerdown  , 0, 0 },
  { "prompt"   , "promp", POS_DEAD    , do_display  , 1, 0 },
  { "push"   ,"pus" , POS_STANDING, do_push , 1, 0 },
  { "pushup"   ,"pushu" , POS_STANDING, do_pushup , 1, 0 },
  { "meditate"   ,"medit" , POS_DEAD, do_meditate , 1, 0 },
  { "jog"      ,"jo"    , POS_STANDING, do_jog    , 1, 0 },
  { "hell2"    , "hell2"  , POS_DEAD    , do_hell2    , 1000000, 0 },
  { "purge"    , "pur"	, POS_DEAD    , do_purge    , 1000000, 0 },

  { "plant"    , "pl"	, POS_RESTING , do_use      , 0, SCMD_PLANT },
  { "qui"      , "qui"	, POS_DEAD    , do_quit     , 0, 0 },
  { "quit"     , "quit"	, POS_DEAD    , do_quit     , 0, SCMD_QUIT },
  { "qsay"     , "qsay"	, POS_RESTING , do_qcomm    , 0, SCMD_QSAY },

  { "renzokou" , "renzo" , POS_FIGHTING, do_renzo , 1, 0 },
  { "reply"    , "r"	, POS_SLEEPING, do_reply    , 0, 0 },
  { "rest"     , "res"	, POS_RESTING , do_rest     , 0, 0 },
  { "read"     , "rea"	, POS_RESTING , do_look     , 0, SCMD_READ },
  { "reload"   , "rel"	, POS_DEAD    , do_reboot   , LVL_IMPL, 0 },
  { "swallow"  , "swal"	, POS_RESTING , do_use      , 0, SCMD_SWALLOW },
  { "smoke"  , "smok" , POS_RESTING , do_use      , 0, SCMD_SMOKE },
  { "receive"  , "rece"	, POS_STANDING, do_not_here , 1, 0 },
  { "remove"   , "rem"	, POS_STANDING , do_remove   , 0, 0 },
  { "rent"     , "ren"	, POS_STANDING, do_not_here , 999999, 0 },
  { "repair"   , "repa" , POS_SITTING , do_repair   , 1, 0 },
  { "ingest"   , "ing"  , POS_SITTING , do_ingest   , 1, 0 },
  { "deflect"   , "defl"  , POS_SITTING , do_deflect   , 1, 0 },
  { "block"   , "blo"  , POS_FIGHTING , do_block   , 1, 0 },
  { "dodge"   , "dodg"  , POS_FIGHTING , do_dodge   , 1, 0 },
  { "adef"   , "adef"  , POS_FIGHTING , do_adef   , 1, 0 },
  { "report"   , "rep"	, POS_RESTING , do_report   , 0, 0 },
  { "reroll"   , "rer"	, POS_DEAD    , do_wizutil  , LVL_GRGOD, SCMD_REROLL },
  { "rescue"   , "resc"	, POS_FIGHTING, do_rescue   , 1, 0 },
  { "restore"  , "resto", POS_DEAD    , do_restore  , LVL_IMMORT, 0 },
  { "return"   , "ret"	, POS_DEAD    , do_return   , 1, 0 },
  { "recall"   , "rec"  , POS_SLEEPING, do_recall   , 0, 0 },
  { "relevel"  , "rel"  , POS_DEAD    , do_relevel  , 0, 0 },
  { "redit"    , "redit", POS_DEAD    , do_olc      , 1000000, SCMD_OLC_REDIT},
  { "rlink"    , "rlink", POS_DEAD    , do_rlink    , LVL_IMMORT, 0 },
  { "roleplay" , "role"  , POS_SLEEPING, do_roleplay , 0, 0 },
  { "roomflags", "roomf", POS_DEAD    , do_gen_tog  , LVL_IMMORT, SCMD_ROOMFLAGS },
  { "roundhouse", "rou"	, POS_FIGHTING, do_roundhouse , 1, 0 },
  { "rogafufuken", "roga" , POS_FIGHTING, do_rogafufuken , 1, 0 },
  { "rp"       , "rp"   , POS_DEAD    , do_gen_tog  , 0, SCMD_QUEST },
  { "rpecho"   , "rpec"	, POS_DEAD    , do_qcomm    , LVL_IMMORT, SCMD_QECHO },

  { "say"      , "say"	, POS_RESTING , do_say      , 0, 0 },
  { "osay"     , "osay"	, POS_RESTING , do_osay     , 0, 0 },
  { "'"        , "'"	, POS_RESTING , do_say      , 999999, 0 },
  { "save"     , "save"	, POS_SLEEPING, do_save     , 0, 0 },
  { "sell"     , "sel"	, POS_STANDING, do_not_here , 0, 0 },
  { "send"     , "sen"	, POS_SLEEPING, do_send     , LVL_GOD, 0 },
  { "set"      , "set"	, POS_DEAD    , do_set      , 1000000, 0 },
  { "ftick"    , "ftick", POS_DEAD    , do_ftick    , LVL_GOD, 0 },
  { "sedit"    , "sedi"	, POS_DEAD    , do_olc      , LVL_IMMORT, SCMD_OLC_SEDIT},
  { "shout"    , "sho"	, POS_RESTING , do_gen_comm , 0, SCMD_SHOUT },
  { "shake"    , "sha"	, POS_RESTING , do_action   , 999999, 0 },
  { "shiver"   , "shiv"	, POS_RESTING , do_action   , 999999, 0 },
  { "show"     , "sho"	, POS_DEAD    , do_show     , LVL_IMMORT, 0 },
  { "showadd" , "showadd", POS_DEAD   , do_showadd  , LVL_IMPL, 0 },
  { "shutdow"  , "shutdow", POS_DEAD    , do_shutdown , LVL_GRGOD, 0 },
  { "shutdown" , "shutdown", POS_DEAD    , do_shutdown , LVL_GRGOD, SCMD_SHUTDOWN },
  { "sip"      , "sip"	, POS_RESTING , do_drink    , 0, SCMD_SIP },
  { "sit"      , "sit"	, POS_RESTING , do_sit      , 0, 0 },
  { "situp"    ,"situ"  , POS_STANDING, do_situp , 1, 0 },
  { "skillset" , "skillse" , POS_SLEEPING, do_skillset , 1000007, 0 },
  { "sleep"    , "sl"	, POS_SLEEPING, do_sleep    , 0, 0 },
  { "slowns"   , "slown", POS_DEAD    , do_gen_tog  , LVL_IMPL, SCMD_SLOWNS },
  { "swiftness", "swift", POS_STANDING, do_SWIFTNESS    , 1, 0 },
  { "snoop"    , "sno"	, POS_DEAD    , do_snoop    , LVL_GOD, 0 },
  { "socials"  , "soc"	, POS_DEAD    , do_commands , 1, SCMD_SOCIALS },
  { "split"    , "spl"	, POS_SITTING , do_split    , 1, 0 },
  { "suicide"    , "suicide"	, POS_RESTING , do_suicide    , 0, 0 },
  { "stand"    , "st"   , POS_RESTING , do_stand    , 0, 0 },
  { "mug"    , "mu" , POS_STANDING, do_mug    , 1, 0 },
  { "camp"     , "cam"  , POS_STANDING, do_gen_tog  , 1, SCMD_CAMP },
  { "switch"   , "sw"	, POS_DEAD    , do_switch   , LVL_GRGOD, 0 },
  { "ssj"      , "ssj"  , POS_DEAD    , do_ssj      , 1, 0 },
  { "syslog"   , "sys"	, POS_DEAD    , do_syslog   , LVL_IMMORT, 0 },

  { "tedit"    , "ted"	, POS_DEAD    , do_tedit    , LVL_IMPL, 0 },
  { "tell"     , "tel"	, POS_DEAD    , do_tell     , 1, 0 },
  { "take"     , "ta"	, POS_RESTING , do_get      , 1, 0 },
  { "tailwhip" , "tail"	, POS_FIGHTING, do_tailwhip , 1, 0 },  
  { "forelockwhip" , "fore" , POS_FIGHTING, do_forelock , 1, 0 },
  { "taste"    , "tas"	, POS_RESTING , do_eat      , 999999, SCMD_TASTE },
  { "teleport" , "tel"	, POS_DEAD    , do_teleport , LVL_LSSRGOD, 0 },
  { "thaw"     , "thaw"	, POS_DEAD    , do_wizutil  , 1000006, SCMD_THAW },
  { "title"    , "tit"	, POS_DEAD    , do_title    , 0, 0 },
  { "time"     , "tim"	, POS_DEAD    , do_time     , 0, 0 },
  { "timeinterest"     , "timeint"  , POS_DEAD    , do_timeinterest     , 1000007, 0 },
  { "boottime"     , "bootime"  , POS_DEAD    , do_timeboot     , 1000010, 0 },
  { "toggle"   , "tog"	, POS_DEAD    , do_toggle   , 999999, 0 },
  { "sense"    , "sen"	, POS_STANDING, do_sense    , 0, 0 },
  { "trade" , "tra" , POS_SLEEPING, do_trade    , 1, 0 },
  { "transfer" , "tran"	, POS_SLEEPING, do_trans    , LVL_IMMORT, 0 },
  { "transform", "transfo" , POS_STANDING, do_transform , 1, 0 },
  { "trigedit" , "trig"	, POS_DEAD    , do_olc      , 1000000, SCMD_OLC_TRIGEDIT},
  { "typo"     , "typ"	, POS_DEAD    , do_gen_write, 1000, SCMD_TYPO },

  { "unlock"   , "unl"	, POS_SITTING , do_gen_door , 0, SCMD_UNLOCK },
  { "ungroup"  , "ung"	, POS_DEAD    , do_ungroup  , 999999, 0 },
  { "unban"    , "unb"	, POS_DEAD    , do_unban    , LVL_GRGOD, 0 },
  { "unaffect" , "una"	, POS_DEAD    , do_wizutil  , LVL_GOD, SCMD_UNAFFECT },
  { "uptime"   , "upt"	, POS_DEAD    , do_date     , LVL_IMMORT, SCMD_UPTIME },
  { "use"      , "us"	, POS_SITTING , do_use      , 1000013, SCMD_USE },
  { "users"    , "user"	, POS_DEAD    , do_users    , LVL_IMMORT, 0 },

  { "value"    , "val"	, POS_STANDING, do_not_here , 0, 0 },
  { "visible"  , "vis"	, POS_RESTING , do_visible  , 1, 0 },
  { "vnum"     , "vnum"	, POS_DEAD    , do_vnum     , LVL_IMMORT, 0 },
  { "vstat"    , "vsta"	, POS_DEAD    , do_vstat    , LVL_IMMORT, 0 },

  { "wake"     , "wak"	, POS_SLEEPING, do_wake     , 0, 0 },
  { "wear"     , "wea"	, POS_RESTING , do_wear     , 0, 0 },
  { "weather"  , "weat" , POS_RESTING , do_weather  , 999999, 0 },
  { "wish"      , "wis"  , POS_STANDING    , do_wish      , 0, 0 },
  { "who"      , "who"	, POS_DEAD    , do_who      , 0, 0 },
  { "clanwho"      , "clanwh"  , POS_DEAD    , do_clanwho      , 1, 0 },
  { "whois"    , "whois" , POS_DEAD   , do_whois    , 0, 0 },
  { "finger"   , "fin"	, POS_DEAD    , do_finger   , 999999, 0 },
  { "whoami"   , "whoa"	, POS_DEAD    , do_gen_ps   , 999999, SCMD_WHOAMI },
  { "where"    , "whe"	, POS_RESTING , do_where    , 1, 0 },
  { "whisper"  , "whis"	, POS_RESTING , do_spec_comm, 0, SCMD_WHISPER },
  { "wield"    , "wie"	, POS_RESTING , do_wield    , 0, 0 },
  { "wimpy"    , "wim"	, POS_DEAD    , do_wimpy    , 0, 0 },
  { "withdraw" , "with"	, POS_STANDING, do_not_here , 1, 0 },
  { "withdrawall" , "withdrawal" , POS_STANDING, do_not_here , 1, 0 },
  { "wiznet"   , "wiz"	, POS_DEAD    , do_wiznet   , LVL_IMMORT, 0 },
  { ";"        , ";"	, POS_DEAD    , do_wiznet   , LVL_IMMORT, 0 },
  { "wizhelp"  , "wizh"	, POS_SLEEPING, do_commands , 1000000, SCMD_WIZHELP },
  { "wizlist"  , "wizl"	, POS_DEAD    , do_gen_ps   , 0, SCMD_WIZLIST },
  { "wizlock"  , "wizlo", POS_DEAD    , do_wizlock  , 1000007, 0 },
  { "write"    , "wr"	, POS_STANDING, do_write    , 100, 0 },
  { "zanzokenelbow", "zan", POS_FIGHTING, do_zanelb , 0, 0 },

  { "zedit"    , "zed"	, POS_DEAD    , do_olc      , LVL_IMMORT, SCMD_OLC_ZEDIT},
  { "zreset"   , "zre"	, POS_DEAD    , do_zreset   , 1000009, 0 },


  /* DG trigger commands */
  { "attach"   , "att"	, POS_DEAD    , do_attach   , LVL_IMPL, 0 },
  { "detach"   , "det"	, POS_DEAD    , do_detach   , LVL_IMPL, 0 },
  { "tlist"    , "tli"	, POS_DEAD    , do_tlist    , LVL_GOD, 0 },
  { "tstat"    , "tst"	, POS_DEAD    , do_tstat    , LVL_GOD, 0 },
  { "masound"  , "masound"	, POS_DEAD    , do_masound  , 0, 0 },
  { "mkill"    , "mkill"	, POS_STANDING, do_mkill    , 0, 0 },
  { "mjunk"    , "mjunk"	, POS_SITTING , do_mjunk    , 0, 0 },
  { "mecho"    , "mecho"	, POS_DEAD    , do_mecho    , 0, 0 },
  { "mechoaround", "mechoaround",POS_DEAD , do_mechoaround    , 0, 0 },
  { "msend"    , "msend"	, POS_DEAD    , do_msend    , 0, 0 },
  { "mload"    , "mload"	, POS_DEAD    , do_mload    , 0, 0 },
  { "mpurge"   , "mpurge"	, POS_DEAD    , do_mpurge    , 0, 0 },
  { "mgoto"    , "mgoto"	, POS_DEAD    , do_mgoto    , 0, 0 },
  { "mat"      , "mat"		, POS_DEAD    , do_mat      , 0, 0 },
  { "mteleport", "mteleport"	, POS_DEAD    , do_mteleport, 0, 0 },
  { "mforce"   , "mforce"	, POS_DEAD    , do_mforce   , 0, 0 },
  { "mexp"     , "mexp"		, POS_DEAD    , do_mexp     , 0, 0 },
  { "mgold"    , "mgold"	, POS_DEAD    , do_mgold    , 0, 0 },
  { "mgecho"   , "mgecho"       , POS_DEAD    , do_mgecho   , 0, 0 },
  { "mhunt"    , "mhunt"	, POS_DEAD    , do_mhunt    , 0, 0 },
  { "mremember", "mremember"	, POS_DEAD    , do_mremember, 0, 0 },
  { "mforget"  , "mforget"	, POS_DEAD    , do_mforget  , 0, 0 },
  { "mtransform","mtransform"	, POS_DEAD    , do_mtransform, 0, 0 },


  { "\n", "zzzzzzz", 0, 0, 0, 0 } };	/* this must be last */


const char *fill[] =
{
  "in",
  "from",
  "with",
  "the",
  "on",
  "at",
  "to",
  "\n"
};

const char *reserved[] =
{
  "a",
  "an",
  "self",
  "me",
  "all",
  "room",
  "someone",
  "something",
  "\n"
};

/*
 * This is the actual command interpreter called from game_loop() in comm.c
 * It makes sure you are the proper level and position to execute the command,
 * then calls the appropriate function.
 */

void command_interpreter(struct char_data *ch, char *argument)
{
  int cmd, length;
  char *line;

  REMOVE_BIT(AFF_FLAGS(ch), AFF_HIDE);

  /* just drop to next line for hitting CR */
  skip_spaces(&argument);
  if (!*argument)
    return;

  if (!isalpha(*argument)) {
    arg[0] = argument[0];
    arg[1] = '\0';
    line = argument + 1;
  } else
    line = any_one_arg(argument, arg);

  /* otherwise, find the command */
  if ((GET_LEVEL(ch)<LVL_IMMORT) &&
      (command_wtrigger(ch, arg, line) ||
       command_mtrigger(ch, arg, line) ||
       command_otrigger(ch, arg, line)))
    return; /* command trigger took over */
  for (length = strlen(arg), cmd = 0; *complete_cmd_info[cmd].command != '\n'; cmd++)
    if (!strncmp(complete_cmd_info[cmd].command, arg, length))
      if (GET_LEVEL(ch) >= complete_cmd_info[cmd].minimum_level) {
	break;
       }
  if (*complete_cmd_info[cmd].command == '\n') {
    send_to_char("Huh?  (Type 'commands' for a list of commands.)\r\n", ch);
  }
  else if (PLR_FLAGGED(ch, PLR_FROZEN) && GET_LEVEL(ch) < 1000013 && complete_cmd_info[cmd].minimum_level != 3) {
    send_to_char("You try, but the mind-numbing cold prevents you...\r\n", ch);
  }
  else if (!PRF2_FLAGGED(ch, PRF2_HEAD) && complete_cmd_info[cmd].minimum_level != 3 && !IS_NPC(ch)) {
    send_to_char("Wait until you are given a new head, then you will be able to think through your actions. You won't have to wait long, be patient. Check status...\r\n", ch);
  }
/*  else if (complete_cmd_info[cmd].command_pointer == NULL)
send_to_char("Sorry, that command hasn't been implemented yet.\r\n", ch);*/
  else if (IS_NPC(ch) && complete_cmd_info[cmd].minimum_level >= LVL_IMMORT) {
    send_to_char("You can't use immortal commands while switched.\r\n", ch);
  }
  else if (GET_POS(ch) < complete_cmd_info[cmd].minimum_position)
    switch (GET_POS(ch)) {
    case POS_DEAD:
      send_to_char("Lie still; you are DEAD!!! :-(\r\n", ch);
      break;
    case POS_INCAP:
    case POS_MORTALLYW:
      send_to_char("You are in a pretty bad shape, unable to do anything!\r\n", ch);
      break;
    case POS_STUNNED:
      send_to_char("All you can do right now is think about the stars!\r\n", ch);
      break;
    case POS_SLEEPING:
      send_to_char("In your dreams, or what?\r\n", ch);
      break;
    case POS_RESTING:
      send_to_char("Nah... You feel too relaxed to do that..\r\n", ch);
      break;
    case POS_SITTING:
     if (PLR_FLAGGED(ch, PLR_RLEG) || PLR_FLAGGED(ch, PLR_LLEG)) {
      send_to_char("Maybe you should get on your feet first?\r\n", ch);
     }
     else {
     ((*complete_cmd_info[cmd].command_pointer) (ch, line, cmd, complete_cmd_info[cmd].subcmd));
     }
      break;
    case POS_FIGHTING:
      send_to_char("No way!  You're fighting for your life!\r\n", ch);
      break;
  } else if (no_specials || !special(ch, cmd, line))
    ((*complete_cmd_info[cmd].command_pointer) (ch, line, cmd, complete_cmd_info[cmd].subcmd));
}


/**************************************************************************
 * Routines to handle aliasing                                             *
  **************************************************************************/


struct alias *find_alias(struct alias *alias_list, char *str)
{
  while (alias_list != NULL) {
    if (*str == *alias_list->alias)	/* hey, every little bit counts :-) */
      if (!strcmp(str, alias_list->alias))
	return alias_list;

    alias_list = alias_list->next;
  }

  return NULL;
}


void free_alias(struct alias *a)
{
  if (a->alias)
    free(a->alias);
  if (a->replacement)
    free(a->replacement);
  free(a);
}

/* The interface to the outside world: do_alias */
ACMD(do_alias)
{
  char *repl;
  struct alias *a, *temp;

  if (IS_NPC(ch))
    return;

  repl = any_one_arg(argument, arg);

  if (!*arg) {			/* no argument specified -- list currently defined aliases */
    send_to_char("Currently defined aliases:\r\n", ch);
    if ((a = GET_ALIASES(ch)) == NULL)
      send_to_char(" None.\r\n", ch);
    else {
      while (a != NULL) {
	sprintf(buf, "%-15s %s\r\n", a->alias, a->replacement);
	send_to_char(buf, ch);
	a = a->next;
      }
    }
  } else {			/* otherwise, add or remove aliases */
    /* is this an alias we've already defined? */
    if ((a = find_alias(GET_ALIASES(ch), arg)) != NULL) {
      REMOVE_FROM_LIST(a, GET_ALIASES(ch), next);
      free_alias(a);
    }
    /* if no replacement string is specified, assume we want to delete */
    if (!*repl) {
      if (a == NULL)
	send_to_char("No such alias.\r\n", ch);
      else
	send_to_char("Alias deleted.\r\n", ch);
    } else {			/* otherwise, either add or redefine an alias */
      if (!str_cmp(arg, "alias")) {
	send_to_char("You can't alias 'alias'.\r\n", ch);
	return;
      }
      CREATE(a, struct alias, 1);
      a->alias = str_dup(arg);
      delete_doubledollar(repl);
      a->replacement = str_dup(repl);
      if (strchr(repl, ALIAS_SEP_CHAR) || strchr(repl, ALIAS_VAR_CHAR))
	a->type = ALIAS_COMPLEX;
      else
	a->type = ALIAS_SIMPLE;
      a->next = GET_ALIASES(ch);
      GET_ALIASES(ch) = a;
      send_to_char("Alias added.\r\n", ch);
    }
  }
}

/*
 * Valid numeric replacements are only $1 .. $9 (makes parsing a little
 * easier, and it's not that much of a limitation anyway.)  Also valid
 * is "$*", which stands for the entire original line after the alias.
 * ";" is used to delimit commands.
 */
#define NUM_TOKENS       9

void perform_complex_alias(struct txt_q *input_q, char *orig, struct alias *a)
{
  struct txt_q temp_queue;
  char *tokens[NUM_TOKENS], *temp, *write_point;
  int num_of_tokens = 0, num;

  /* First, parse the original string */
  temp = strtok(strcpy(buf2, orig), " ");
  while (temp != NULL && num_of_tokens < NUM_TOKENS) {
    tokens[num_of_tokens++] = temp;
    temp = strtok(NULL, " ");
  }

  /* initialize */
  write_point = buf;
  temp_queue.head = temp_queue.tail = NULL;

  /* now parse the alias */
  for (temp = a->replacement; *temp; temp++) {
    if (*temp == ALIAS_SEP_CHAR) {
      *write_point = '\0';
      buf[MAX_INPUT_LENGTH - 1] = '\0';
      write_to_q(buf, &temp_queue, 1);
      write_point = buf;
    } else if (*temp == ALIAS_VAR_CHAR) {
      temp++;
      if ((num = *temp - '1') < num_of_tokens && num >= 0) {
	strcpy(write_point, tokens[num]);
	write_point += strlen(tokens[num]);
      } else if (*temp == ALIAS_GLOB_CHAR) {
	strcpy(write_point, orig);
	write_point += strlen(orig);
      } else if ((*(write_point++) = *temp) == '$')	/* redouble $ for act safety */
	*(write_point++) = '$';
    } else
      *(write_point++) = *temp;
  }

  *write_point = '\0';
  buf[MAX_INPUT_LENGTH - 1] = '\0';
  write_to_q(buf, &temp_queue, 1);

  /* push our temp_queue on to the _front_ of the input queue */
  if (input_q->head == NULL)
    *input_q = temp_queue;
  else {
    temp_queue.tail->next = input_q->head;
    input_q->head = temp_queue.head;
  }
}


/*
 * Given a character and a string, perform alias replacement on it.
 *
 * Return values:
 *   0: String was modified in place; call command_interpreter immediately.
 *   1: String was _not_ modified in place; rather, the expanded aliases
 *      have been placed at the front of the character's input queue.
 */
int perform_alias(struct descriptor_data *d, char *orig)
{
  char first_arg[MAX_INPUT_LENGTH], *ptr;
  struct alias *a, *tmp;

  /* Mobs don't have alaises. */
  if (IS_NPC(d->character))
    return 0;

  /* bail out immediately if the guy doesn't have any aliases */
  if ((tmp = GET_ALIASES(d->character)) == NULL)
    return 0;

  /* find the alias we're supposed to match */
  ptr = any_one_arg(orig, first_arg);

  /* bail out if it's null */
  if (!*first_arg)
    return 0;

  /* if the first arg is not an alias, return without doing anything */
  if ((a = find_alias(tmp, first_arg)) == NULL)
    return 0;

  if (a->type == ALIAS_SIMPLE) {
    strcpy(orig, a->replacement);
    return 0;
  } else {
    perform_complex_alias(&d->input, ptr, a);
    return 1;
  }
}



/***************************************************************************
 * Various other parsing utilities                                         *
 **************************************************************************/

/*
 * searches an array of strings for a target string.  "exact" can be
 * 0 or non-0, depending on whether or not the match must be exact for
 * it to be returned.  Returns -1 if not found; 0..n otherwise.  Array
 * must be terminated with a '\n' so it knows to stop searching.
 */
int search_block(char *arg, const char **list, int exact)
{
  register int i, l;

  /* Make into lower case, and get length of string */
  for (l = 0; *(arg + l); l++)
    *(arg + l) = LOWER(*(arg + l));

  if (exact) {
    for (i = 0; **(list + i) != '\n'; i++)
      if (!strcmp(arg, *(list + i)))
	return (i);
  } else {
    if (!l)
      l = 1;			/* Avoid "" to match the first available
				 * string */
    for (i = 0; **(list + i) != '\n'; i++)
      if (!strncmp(arg, *(list + i), l))
	return (i);
  }

  return -1;
}


int is_number(const char *str)
{
  while (*str)
    if (!isdigit(*(str++)))
      return 0;

  return 1;
}

/*
 * Function to skip over the leading spaces of a string.
 */
void skip_spaces(char **string)
{
  for (; **string && isspace(**string); (*string)++);
}


/*
 * Given a string, change all instances of double dollar signs ($$) to
 * single dollar signs ($).  When strings come in, all $'s are changed
 * to $$'s to avoid having users be able to crash the system if the
 * inputted string is eventually sent to act().  If you are using user
 * input to produce screen output AND YOU ARE SURE IT WILL NOT BE SENT
 * THROUGH THE act() FUNCTION (i.e., do_gecho, do_title, but NOT do_say),
 * you can call delete_doubledollar() to make the output look correct.
 *
 * Modifies the string in-place.
 */
char *delete_doubledollar(char *string)
{
  char *read, *write;

  /* If the string has no dollar signs, return immediately */
  if ((write = strchr(string, '$')) == NULL)
    return string;

  /* Start from the location of the first dollar sign */
  read = write;


  while (*read)   /* Until we reach the end of the string... */
    if ((*(write++) = *(read++)) == '$') /* copy one char */
      if (*read == '$')
	read++; /* skip if we saw 2 $'s in a row */

  *write = '\0';

  return string;
}


int fill_word(char *argument)
{
  return (search_block(argument, fill, TRUE) >= 0);
}


int reserved_word(char *argument)
{
  return (search_block(argument, reserved, TRUE) >= 0);
}


/*
 * copy the first non-fill-word, space-delimited argument of 'argument'
 * to 'first_arg'; return a pointer to the remainder of the string.
 */
char *one_argument(char *argument, char *first_arg)
{
  char *begin = first_arg;

  if (!argument) {
    log("SYSERR: one_argument received a NULL pointer!");
    *first_arg = '\0';
    return NULL;
  }

  do {
    skip_spaces(&argument);

    first_arg = begin;
    while (*argument && !isspace(*argument)) {
      *(first_arg++) = LOWER(*argument);
      argument++;
    }

    *first_arg = '\0';
  } while (fill_word(begin));

  return argument;
}


/*
 * one_word is like one_argument, except that words in quotes ("") are
 * considered one word.
 */
char *one_word(char *argument, char *first_arg)
{
  char *begin = first_arg;

  do {
    skip_spaces(&argument);

    first_arg = begin;

    if (*argument == '\"') {
      argument++;
      while (*argument && *argument != '\"') {
        *(first_arg++) = LOWER(*argument);
        argument++;
      }
      argument++;
    } else {
      while (*argument && !isspace(*argument)) {
        *(first_arg++) = LOWER(*argument);
        argument++;
      }
    }

    *first_arg = '\0';
  } while (fill_word(begin));

  return argument;
}


/* same as one_argument except that it doesn't ignore fill words */
char *any_one_arg(char *argument, char *first_arg)
{
  skip_spaces(&argument);

  while (*argument && !isspace(*argument)) {
    *(first_arg++) = LOWER(*argument);
    argument++;
  }

  *first_arg = '\0';

  return argument;
}


/*
 * Same as one_argument except that it takes two args and returns the rest;
 * ignores fill words
 */
char *two_arguments(char *argument, char *first_arg, char *second_arg)
{
  return one_argument(one_argument(argument, first_arg), second_arg); /* :-) */
}
char *three_arguments(char *argument, char *first_arg, char *second_arg, char *third_arg)
{
  return one_argument(one_argument(one_argument(argument, first_arg), second_arg), third_arg); /* :-) */
}


/*
 * determine if a given string is an abbreviation of another
 * (now works symmetrically -- JE 7/25/94)
 *
 * that was dumb.  it shouldn't be symmetrical.  JE 5/1/95
 * 
 * returnss 1 if arg1 is an abbreviation of arg2
 */
int is_abbrev(const char *arg1, const char *arg2)
{
  if (!*arg1)
    return 0;

  for (; *arg1 && *arg2; arg1++, arg2++)
    if (LOWER(*arg1) != LOWER(*arg2))
      return 0;

  if (!*arg1)
    return 1;
  else
    return 0;
}



/* return first space-delimited token in arg1; remainder of string in arg2 */
void half_chop(char *string, char *arg1, char *arg2)
{
  char *temp;

  temp = any_one_arg(string, arg1);
  skip_spaces(&temp);
  strcpy(arg2, temp);
}



/* Used in specprocs, mostly.  (Exactly) matches "command" to cmd number */
int find_command(const char *command)
{
  int cmd;

  for (cmd = 0; *complete_cmd_info[cmd].command != '\n'; cmd++)
    if (!strcmp(complete_cmd_info[cmd].command, command))
      return cmd;

  return -1;
}


int special(struct char_data *ch, int cmd, char *arg)
{
  register struct obj_data *i;
  register struct char_data *k;
  int j;

  /* special in room? */
  if (GET_ROOM_SPEC(ch->in_room) != NULL)
    if (GET_ROOM_SPEC(ch->in_room) (ch, world + ch->in_room, cmd, arg))
      return 1;

  /* special in equipment list? */
  for (j = 0; j < NUM_WEARS; j++)
    if (GET_EQ(ch, j) && GET_OBJ_SPEC(GET_EQ(ch, j)) != NULL)
      if (GET_OBJ_SPEC(GET_EQ(ch, j)) (ch, GET_EQ(ch, j), cmd, arg))
	return 1;

  /* special in inventory? */
  for (i = ch->carrying; i; i = i->next_content)
    if (GET_OBJ_SPEC(i) != NULL)
      if (GET_OBJ_SPEC(i) (ch, i, cmd, arg))
	return 1;

  /* special in mobile present? */
  for (k = world[ch->in_room].people; k; k = k->next_in_room)
    if (GET_MOB_SPEC(k) != NULL)
      if (GET_MOB_SPEC(k) (ch, k, cmd, arg))
	return 1;

  /* special in object present? */
  for (i = world[ch->in_room].contents; i; i = i->next_content)
    if (GET_OBJ_SPEC(i) != NULL)
      if (GET_OBJ_SPEC(i) (ch, i, cmd, arg))
	return 1;

  return 0;
}

/* load the player, put them in the right room - used by copyover_recover too */
/* load the player, put them in the right room - used by copyover_recover too */
int enter_player_game (struct descriptor_data *d)
{
    extern sh_int r_mortal_start_room;
    extern sh_int r_hell_room;
    extern sh_int r_immort_start_room;
    extern sh_int r_frozen_start_room;
    
    sh_int load_room;
    int load_result;
    
    reset_char(d->character);
    if (PLR_FLAGGED(d->character, PLR_INVSTART))
        GET_INVIS_LEV(d->character) = GET_LEVEL(d->character);
    if ((load_result = Crash_load(d->character) && GET_LOADROOM(d->character) == NOWHERE))
      d->character->in_room = NOWHERE;
      save_char(d->character, NOWHERE);

    d->character->next = character_list;
    character_list = d->character;
    if (!IS_NPC(d->character)) {
     GET_ID(d->character) = GET_IDNUM(d->character);
     SET_BIT(PRF2_FLAGS(d->character), PRF2_HEAD);
    
    if (GET_INVIS_LEV(d->character) >= 1000015) {
     GET_INVIS_LEV(d->character) = 0;
     appear(d->character);
    }
    }
    if ((load_room = GET_LOADROOM(d->character)) != NOWHERE)
        load_room = real_room(load_room);
    /* If char was saved with NOWHERE, or real_room above failed... */
    else if (load_room == NOWHERE) {
         if (PRF_FLAGGED(d->character, PRF_NOGRATZ)) {
            load_room = r_hell_room;
            GET_CLAN(d->character) = 0;
            GET_MAX_MOVE(d->character) = 5;
            GET_ADD(d->character) = 0;
            GET_MOVE(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
         }
         else if (CLN_FLAGGED(d->character, CLN_Death)) {
           load_room = r_death_start_room;
                       GET_CLAN(d->character) = 0;
            GET_MAX_MOVE(d->character) = 5;
            GET_ADD(d->character) = 0;
            GET_MOVE(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
          }
         else if (PLR_FLAGGED(d->character, PLR_KILLER)) {
          switch (number(1, 4)) {
         case 1:
           load_room = r_pk1_room;
           break;
         case 2:
           load_room = r_pk2_room;
           break;
         case 3:
           load_room = r_pk3_room;
           break;
         case 4:
           load_room = r_pk4_room;
           break;
         default:
           load_room = r_pk1_room;
           break;
          }
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
          GET_MAX_MOVE(d->character) = 4;
          GET_MOVE(d->character) = 0;
          GET_CLAN(d->character) = 0;
          GET_ADD(d->character) = 0;
         }
         else if (GET_LEVEL(d->character) >= 1000000) {
            load_room = r_immort_start_room;
            GET_CLAN(d->character) = 0;
            GET_MAX_MOVE(d->character) = 5;
            GET_ADD(d->character) = 0;
          GET_MOVE(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
         }
         else if (IS_icer(d->character) || IS_MUTANT(d->character)) {
            load_room = r_icer_start_room;
            GET_CLAN(d->character) = 0;
            GET_MAX_MOVE(d->character) = 5;
            GET_MOVE(d->character) = 0;
            GET_ADD(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
            }
         else if (IS_KONATSU(d->character)) {
            load_room = r_konack_start_room;
            GET_CLAN(d->character) = 0;
            GET_ADD(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
            }
         else if (IS_saiyan(d->character)) {
            load_room = r_saiyan_start_room;
            GET_CLAN(d->character) = 0;
            GET_MAX_MOVE(d->character) = 5;
            GET_MOVE(d->character) = 0;
            GET_ADD(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
            }
         else if (IS_ANGEL(d->character)) {
            load_room = r_angel_start_room;
            GET_CLAN(d->character) = 0;
            GET_MAX_MOVE(d->character) = 5;
            GET_MOVE(d->character) = 0;
            GET_ADD(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
            }
         else if (IS_demon(d->character)) {
            load_room = r_demon_start_room;
            GET_CLAN(d->character) = 0;
            GET_MAX_MOVE(d->character) = 5;
            GET_MOVE(d->character) = 0;
            GET_ADD(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
            }
         else if (IS_MAJIN(d->character)) {
            load_room = r_majin_start_room;
            GET_CLAN(d->character) = 0;
            GET_MAX_MOVE(d->character) = 5;
            GET_MOVE(d->character) = 0;
            GET_ADD(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
            }
         else if (IS_TRUFFLE(d->character)) {
            load_room = r_truffle_start_room;
            GET_CLAN(d->character) = 0;
            GET_MAX_MOVE(d->character) = 5;
            GET_MOVE(d->character) = 0;
            GET_ADD(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
            }
         else if (IS_Namek(d->character)) {
            GET_SEX(d->character) = SEX_NEUTRAL;
            load_room = r_namek_start_room;
            GET_CLAN(d->character) = 0;
            GET_MAX_MOVE(d->character) = 5;
            GET_MOVE(d->character) = 0;
            GET_ADD(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
            }
          else {
            load_room = r_mortal_start_room;
            GET_CLAN(d->character) = 0;
            GET_MAX_MOVE(d->character) = 5;
            GET_MOVE(d->character) = 0;
            GET_ADD(d->character) = 0;
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS1);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS2);
            REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_TRANS3);
        }
    }
    REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_REELING);    
    REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_CAUGHT);
    REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISHING);
    REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH1);
    REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH2);
    REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH3);
    REMOVE_BIT(PRF2_FLAGS(d->character), PRF2_FISH4);
    if (PLR_FLAGGED(d->character, PLR_FROZEN))
        load_room = r_frozen_start_room;
    if (PRF_FLAGGED(d->character, PRF_NOGRATZ))
            load_room = r_hell_room;
    char_to_room(d->character, load_room);

    return load_result;

}


/* *************************************************************************
*  Stuff for controlling the non-playing sockets (get name, pwd etc)       *
************************************************************************* */


/* locate entry in p_table with entry->name == name. -1 mrks failed search */
int find_name(char *name)
{
  int i;

  for (i = 0; i <= top_of_p_table; i++) {
    if (!str_cmp((player_table + i)->name, name))
      return i;
  }

  return -1;
}


int _parse_name(char *arg, char *name)
{
  int i;

  /* skip whitespaces */
  for (; isspace(*arg); arg++);

  for (i = 0; (*name = *arg); arg++, i++, name++)
    if (!isalpha(*arg))
      return 1;

  if (!i)
    return 1;

  return 0;
}

#define RECON		1
#define USURP		2
#define UNSWITCH	3

/*
 * XXX: Make immortals 'return' instead of being disconnected when switched
 *      into person returns.  This function seems a bit over-extended too.
 */
int perform_dupe_check(struct descriptor_data *d)
{
  struct descriptor_data *k, *next_k;
  struct char_data *target = NULL, *ch, *next_ch;
  int mode = 0;

  int id = GET_IDNUM(d->character);

  /*
   * Now that this descriptor has successfully logged in, disconnect all
   * other descriptors controlling a character with the same ID number.
   */

  for (k = descriptor_list; k; k = next_k) {
    next_k = k->next;

    if (k == d)
      continue;

    if (k->original && (GET_IDNUM(k->original) == id)) {    /* switched char */
      SEND_TO_Q("\r\nMultiple login detected -- disconnecting.\r\n", k);
      STATE(k) = CON_CLOSE;
      if (!target) {
	target = k->original;
	mode = UNSWITCH;
      }
      if (k->character)
	k->character->desc = NULL;
      k->character = NULL;
      k->original = NULL;
    } else if (k->character && (GET_IDNUM(k->character) == id)) {
      if (!target && STATE(k) == CON_PLAYING) {
	SEND_TO_Q("\r\nThis body has been usurped!\r\n", k);
	target = k->character;
	mode = USURP;
      }
      k->character->desc = NULL;
      k->character = NULL;
      k->original = NULL;
      SEND_TO_Q("\r\nMultiple login detected -- disconnecting.\r\n", k);
      STATE(k) = CON_CLOSE;
    }
  }

 /*
  * now, go through the character list, deleting all characters that
  * are not already marked for deletion from the above step (i.e., in the
  * CON_HANGUP state), and have not already been selected as a target for
  * switching into.  In addition, if we haven't already found a target,
  * choose one if one is available (while still deleting the other
  * duplicates, though theoretically none should be able to exist).
  */

  for (ch = character_list; ch; ch = next_ch) {
    next_ch = ch->next;

    if (IS_NPC(ch))
      continue;
    if (GET_IDNUM(ch) != id)
      continue;

    /* ignore chars with descriptors (already handled by above step) */
    if (ch->desc)
      continue;

    /* don't extract the target char we've found one already */
    if (ch == target)
      continue;

    /* we don't already have a target and found a candidate for switching */
    if (!target) {
      target = ch;
      mode = RECON;
      continue;
    }

    /* we've found a duplicate - blow him away, dumping his eq in limbo. */
    if (ch->in_room != NOWHERE)
      char_from_room(ch);
    char_to_room(ch, 1);
    extract_char(ch, TRUE);
  }

  /* no target for swicthing into was found - allow login to continue */
  if (!target)
    return 0;

  /* Okay, we've found a target.  Connect d to target. */
  free_char(d->character); /* get rid of the old char */
  d->character = target;
  d->character->desc = d;
  d->original = NULL;
  d->character->char_specials.timer = 0;
  REMOVE_BIT(PLR_FLAGS(d->character), PLR_MAILING | PLR_WRITING);
  STATE(d) = CON_PLAYING;

  switch (mode) {
  case RECON:
    SEND_TO_Q("Reconnecting.\r\n", d);
    sprintf(buf2, "%s has reconnected.", GET_NAME(d->character));
    send_to_room(buf2, d->character->in_room);
    /*act("$n has reconnected.", TRUE, d->character, 0, 0, TO_ROOM);*/
    sprintf(buf, "%s [%s] has reconnected.", GET_NAME(d->character), d->host);
    mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
    break;
  case USURP:
    SEND_TO_Q("You take over your own body, already in use!\r\n", d);
    act("$n suddenly keels over in pain, surrounded by a white aura...\r\n"
	"$n's body has been taken over by a new spirit!",
	TRUE, d->character, 0, 0, TO_ROOM);
    sprintf(buf, "%s has re-logged in ... disconnecting old socket.",
	    GET_NAME(d->character));
    mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
    break;
  case UNSWITCH:
    SEND_TO_Q("Reconnecting to unswitched char.", d);
    sprintf(buf, "%s [%s] has reconnected.", GET_NAME(d->character), d->host);
    mudlog(buf, NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
    break;
  }

  return 1;
}



/* deal with newcomers and other non-playing sockets */
void nanny(struct descriptor_data *d, char *arg)
{
  char buf[128];
  int player_i, load_result;
  int earnnum = 0;
  char tmp_name[MAX_INPUT_LENGTH];
  struct char_file_u tmp_store;

  int load_char(char *name, struct char_file_u *char_element);
   int parse_class(char arg);


  switch (STATE(d)) {

  /*. OLC states .*/
  case CON_HEDIT:
    hedit_parse(d, arg);
    break;
  case CON_OEDIT: 
    oedit_parse(d, arg);
    break;
  case CON_REDIT: 
    redit_parse(d, arg);
    break;
  case CON_ZEDIT: 
    zedit_parse(d, arg);
    break;
  case CON_MEDIT: 
    medit_parse(d, arg);
    break;
  case CON_SEDIT: 
    sedit_parse(d, arg);
    break;
  case CON_AEDIT:
    aedit_parse(d, arg);
    break;
  case CON_TRIGEDIT:
    trigedit_parse(d, arg);
    break;
  /*. End of OLC states .*/

  case CON_GET_NAME:		/* wait for input of name */
    if (d->character == NULL) {
      CREATE(d->character, struct char_data, 1);
      clear_char(d->character);
      CREATE(d->character->player_specials, struct player_special_data, 1);
      d->character->desc = d;
    }
    if (!*arg)
      STATE(d) = CON_CLOSE;
    else {
      if ((_parse_name(arg, tmp_name)) || strlen(tmp_name) < 2 ||
	  strlen(tmp_name) > MAX_NAME_LENGTH || !Valid_Name(tmp_name) ||
	  fill_word(strcpy(buf, tmp_name)) || reserved_word(buf)) {
	SEND_TO_Q("Invalid name, please try another.\r\n"
		  "Name: ", d);
	return;
      }
      if ((player_i = load_char(tmp_name, &tmp_store)) > -1) {
	store_to_char(&tmp_store, d->character);
	GET_PFILEPOS(d->character) = player_i;

	if (PLR_FLAGGED(d->character, PLR_DELETED)) {
	  /* We get a false positive from the original deleted character. */
	  free_char(d->character);
	  d->character = NULL;
	  /* Check for multiple creations... */
	  if (!Valid_Name(tmp_name)) {
	    SEND_TO_Q("Invalid name, please try another.\r\nName: ", d);
	    return;
	  }
	  CREATE(d->character, struct char_data, 1);
	  clear_char(d->character);
	  CREATE(d->character->player_specials, struct player_special_data, 1);
	  d->character->desc = d;
	  CREATE(d->character->player.name, char, strlen(tmp_name) + 1);
	  strcpy(d->character->player.name, CAP(tmp_name));
	  GET_PFILEPOS(d->character) = player_i;
	  sprintf(buf, "Are you sure you want %s (Y/N)? ", tmp_name);
	  SEND_TO_Q(buf, d);
	  STATE(d) = CON_NAME_CNFRM;
	} else {
	  /* undo it just in case they are set */
	  REMOVE_BIT(PLR_FLAGS(d->character),
		     PLR_WRITING | PLR_MAILING | PLR_CRYO);

          sprintf(buf, "Enter %s's Password: ", GET_NAME(d->character));
	  SEND_TO_Q(buf, d);
	  echo_off(d);
	  d->idle_tics = 0;
	  STATE(d) = CON_PASSWORD;
	}
      } else {
	/* player unknown -- make new character */

	/* Check for multiple creations of a character. */
	if (!Valid_Name(tmp_name)) {
	  SEND_TO_Q("Invalid name, please try another.\r\nName: ", d);
	  return;
	}
	CREATE(d->character->player.name, char, strlen(tmp_name) + 1);
	strcpy(d->character->player.name, CAP(tmp_name));

	sprintf(buf, "Did I get that right, %s (Y/N)? ", tmp_name);
	SEND_TO_Q(buf, d);
	STATE(d) = CON_NAME_CNFRM;
      }
    }
    break;
  case CON_NAME_CNFRM:		/* wait for conf. of new name    */
    if (UPPER(*arg) == 'Y') {
      if (isbanned(d->host) >= BAN_NEW) {
	sprintf(buf, "Request for new char %s denied from [%s] (siteban)",
		GET_NAME(d->character), d->host);
	mudlog(buf, NRM, LVL_GOD, TRUE);
	SEND_TO_Q("Sorry, new characters are not allowed from your site!\r\n", d);
	STATE(d) = CON_CLOSE;
	return;
      }
      if (circle_restrict >= 1) {
	SEND_TO_Q("Sorry, new players can't be created at the moment.\r\n", d);
	sprintf(buf, "Request for new char %s denied from [%s] (wizlock)",
		GET_NAME(d->character), d->host);
	mudlog(buf, NRM, LVL_GOD, TRUE);
	STATE(d) = CON_CLOSE;
	return;
      }
      SEND_TO_Q("New character.\r\n", d);
      sprintf(buf, "Give me a password for %s: ", GET_NAME(d->character));
      SEND_TO_Q(buf, d);
      echo_off(d);
      STATE(d) = CON_NEWPASSWD;
    } else if (*arg == 'n' || *arg == 'N') {
      SEND_TO_Q("Okay, what IS it, then? ", d);
      free(d->character->player.name);
      d->character->player.name = NULL;
      STATE(d) = CON_GET_NAME;
    } else {
      SEND_TO_Q("Please type Yes or No: ", d);
    }
    break;
  case CON_PASSWORD:		/* get pwd for known player      */
    /*
     * To really prevent duping correctly, the player's record should
     * be reloaded from disk at this point (after the password has been
     * typed).  However I'm afraid that trying to load a character over
     * an already loaded character is going to cause some problem down the
     * road that I can't see at the moment.  So to compensate, I'm going to
     * (1) add a 15 or 20-second time limit for entering a password, and (2)
     * re-add the code to cut off duplicates when a player quits.  JE 6 Feb 96
     */

    echo_on(d);    /* turn echo back on */

    if (!*arg)
      STATE(d) = CON_CLOSE;
    else {
      if (strncmp(CRYPT(arg, GET_PASSWD(d->character)), GET_PASSWD(d->character), MAX_PWD_LENGTH)) {
	sprintf(buf, "Bad PW: %s [%s] (%s)", GET_NAME(d->character), d->host, arg);
	mudlog(buf, BRF, LVL_GOD, TRUE);
	GET_BAD_PWS(d->character)++;
	save_char(d->character, NOWHERE);
	if (++(d->bad_pws) >= max_bad_pws) {	/* 3 strikes and you're out. */
	  SEND_TO_Q("Wrong password... disconnecting.\r\n", d);
	  STATE(d) = CON_CLOSE;
	} else {
          sprintf(buf, "Wrong password.\r\nEnter %s's Password:", GET_NAME(d->character));
	  SEND_TO_Q(buf, d);
	  echo_off(d);
	}
	return;
      }

      /* Password was correct. */
      load_result = GET_BAD_PWS(d->character);
      GET_BAD_PWS(d->character) = 0;
      d->bad_pws = 0;

      if (isbanned(d->host) == BAN_SELECT &&
	  !PLR_FLAGGED(d->character, PLR_SITEOK)) {
	SEND_TO_Q("Sorry, this char has not been cleared for login from your site!\r\n", d);
	STATE(d) = CON_CLOSE;
	sprintf(buf, "Connection attempt for %s denied from %s",
		GET_NAME(d->character), d->host);
	mudlog(buf, NRM, LVL_GOD, TRUE);
	return;
      }
      if (circle_restrict > GET_LEVEL(d->character) && GET_LEVEL(d->character) < 1000000) {
	SEND_TO_Q("The game is temporarily restricted.. try again later.\r\n", d);
	STATE(d) = CON_CLOSE;
	sprintf(buf, "Request for login denied for %s [%s] (wizlock)",
		GET_NAME(d->character), d->host);
	mudlog(buf, NRM, LVL_GOD, TRUE);
	return;
      }
      /* check and make sure no other copies of this player are logged in */
      if (perform_dupe_check(d))
	return;

      if (GET_LEVEL(d->character) >= LVL_IMMORT)
	SEND_TO_Q(imotd, d);
      else
	SEND_TO_Q(motd, d);

      sprintf(buf, "%s [%s] has connected.", GET_NAME(d->character), d->host);
      mudlog(buf, BRF, MAX(LVL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
/*    if (GET_LEVEL(d->character) < LVL_IMMORT) {
    sprintf(buf2, "&16[&09Player Login&16] &15%s has joined &11Drag&12(&09*&12)&11n Ball - &16Ressurection of &12T&00&07r&00&06u&00&07t&12h&10!&00\r\n", GET_NAME(d->character));
    send_to_all(buf2);
}*/

      if (load_result) {
	sprintf(buf, "\r\n\r\n\007\007\007"
		"%s%d LOGIN FAILURE%s SINCE LAST SUCCESSFUL LOGIN.%s\r\n",
		CCRED(d->character, C_SPR), load_result,
		(load_result > 1) ? "S" : "", CCNRM(d->character, C_SPR));
	SEND_TO_Q(buf, d);
	GET_BAD_PWS(d->character) = 0;
      }
      SEND_TO_Q("\r\n\n*** PRESS RETURN: ", d);
      STATE(d) = CON_RMOTD;
    }
    break;

  case CON_NEWPASSWD:
  case CON_CHPWD_GETNEW:
    if (!*arg || strlen(arg) > MAX_PWD_LENGTH || strlen(arg) < 3 ||
	!str_cmp(arg, GET_NAME(d->character))) {
      SEND_TO_Q("\r\nIllegal password.\r\n", d);
      SEND_TO_Q("Password: ", d);
      return;
    }
    strncpy(GET_PASSWD(d->character), CRYPT(arg, GET_NAME(d->character)), MAX_PWD_LENGTH);
    *(GET_PASSWD(d->character) + MAX_PWD_LENGTH) = '\0';

    sprintf(buf, "Please retype %s's password: ", GET_NAME(d->character));
    SEND_TO_Q(buf, d);
    if (STATE(d) == CON_NEWPASSWD)
      STATE(d) = CON_CNFPASSWD;
    else
      STATE(d) = CON_CHPWD_VRFY;

    break;

  case CON_CNFPASSWD:
  case CON_CHPWD_VRFY:
    if (strncmp(CRYPT(arg, GET_PASSWD(d->character)), GET_PASSWD(d->character),
		MAX_PWD_LENGTH)) {
      SEND_TO_Q("\r\nPasswords don't match... start over.\r\n", d);
      sprintf(buf, "Enter %s's Password: ", GET_NAME(d->character));
      SEND_TO_Q(buf, d);
      if (STATE(d) == CON_CNFPASSWD)
	STATE(d) = CON_NEWPASSWD;
      else
	STATE(d) = CON_CHPWD_GETNEW;
      return;
    }
    echo_on(d);

    STATE(d) = CON_QSEX;
    SEND_TO_Q("What is your sex (\x1B[1;34mM\x1B[0;0m/\x1B[1;35mF\x1B[0;0m/\x1B[1;30mN\x1B[0;0m)?\r\n", d);
    break;
    
 case CON_QSEX:
    switch (*arg) {
    case 'm':
    case 'M':
      d->character->player.sex = SEX_MALE;
      break;
    case 'f':
    case 'F':
      d->character->player.sex = SEX_FEMALE;
      break;
    case 'n':
    case 'N':
      d->character->player.sex = SEX_NEUTRAL;
      break;
    default:
      SEND_TO_Q("That is not a sex..\r\n"
		"What IS your sex? ", d);
      return;
    }
    SEND_TO_Q(class_menu, d);
    SEND_TO_Q("\r\nPick a Race: ", d);
    STATE(d) = CON_QCLASS;
    break;

  case CON_QCLASS:
    if (!strcmp(arg, "human")) {
     SEND_TO_Q("\r\nHumans are the predominant species of planet Earth. They are usually looked down on in the large scheme of the universe, but they have it within themselves to aspire to the top reaches of power in the galaxy. Humans have developed many techniques, using a natural intuition humans are very crafty in developing new attacks and various other things.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Moderate\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "namek")) {
     SEND_TO_Q("\r\nThe namek are an ancient race that originate from the planet Namek. They are sexless beings, slightly akin to plants, with green skin and antennae.  They possess remarkable regeneration capabilities, the ability to use their power to heal others, and moderate telepathy.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Moderate\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "icer")) {
     SEND_TO_Q("\r\nIcers are those beings that reside on planet Frigid, a place of ice and freezing winds.  Icers took to space as they attempted to satisfy the deep urges of domination inside them, spreading a reputation of evil among most intelligent races.  Icers have a humanoid form along with a long tail.  Icers are often known to have multiple forms, growing more powerful as they transform.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Hard\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    } 
    if (!strcmp(arg, "majin")) {
     SEND_TO_Q("\r\nMajin are a strange race that are composed of a type of gummy material with a forelock protruding from their forehead.  They are often considered evil, however they are mostly based off of passions, whether it be the desire to do evil or just the desire for pleasure.  Majins are famous for their craving for sweets and candies and, with their forelocks, they are able to focus their power into a ray capable of turning lifeforms into candy.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Easy\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "android")) {
     SEND_TO_Q("\r\nAndroids are humanoid machines that have been programmed to simulate human emotions and drives.  Their programming has evolved to the point of forming a conciousness, giving each one a unique personality and desires.  Using the technology built into them, androids are able to harness energy and manipulate ki.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Easy\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "kai")) {
     SEND_TO_Q("\r\nKai are the ancient beings of wisdom, mostly centering their lives around their home of Aether but stretching their influence to the far reaches of the universe, watching over it.  Some consider them to have an almost celestial quality, however Kai are far from being gods. They merely gain knowledge from observing the universe and with this knowledge they gain power.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Moderate\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "halfbreed")) {
     SEND_TO_Q("\r\nHalfBreeds are the mix of human and saiyan blood, creating a fighter with the human intelligence and brute saiyan force.  These HalfBreeds are usually accepted by their human parentage until such time as they are able to prove their strength to their saiyan blood.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Moderate\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "demon")) {
     SEND_TO_Q("\r\nDemons are those beings who originated in hell, born from the fire and darkness.  Because of their origin, they hold a strange elemental power over fire.  Known for their vengeance and rage, demons are a force feared by most of the intelligent universe.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Moderate\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "angel")) {
     SEND_TO_Q("\r\nAngels are a race from another universe. Their universe was destroyed and they managed to escape with a piece of their planet using their magical powers and artifacts. They arrived in this universe after many generations of travel, their asteroid turned into a mini planet with the same magic. Arriving in this universe the Angels(as the races of this universe called them because of their similar appearance to those mythical creatures.) the trans dimensional artifacts were lost, stranding the angels. Angels are a reclusive race, thinking their selves superior to all other life yet they also value life, and are generally good in nature.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Hard\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "konatsu")) {
     SEND_TO_Q("\r\nKonatsu are a race of shadow warriors, adapting the ways of stealth and blades.  They have elven ears and keen eyes, training their senses and body in the ways of the combat.  They originate from the planet Konack and have protected it since their very origin.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Hard\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "truffle")) {
     SEND_TO_Q("\r\nTruffles are an extremely technologically advanced race that used to live with the saiyans on planet Vegeta, before they were driven off by an outburst of warfare.  They appear mostly as miniature humans, relying on their various machines and computers to compensate for their lack of phsyical mass.  Their engineering skills have led to an advanced society and many of the inventions used around the universe.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Moderate\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "mutant")) {
     SEND_TO_Q("\r\nMutants are beings that have been brought into existence due to some mystery of genetics that gives birth to such unique creatures as the various mutants around the many galaxies.  These changes are far from debilitating however as most mutants use their unique qualities to excel in battle.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Very Hard\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "bioandroid")) {
     SEND_TO_Q("\r\nBio-Androids are biological creations that utilize the utmost abilities of their cellular structure, finding power in the variety of DNA used in their construction.  Each Bio-Android varies wildly from the next as each one has a unique combination of DNA, however all Bio-Androids strive towards their own perfection.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Easy\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "kanassan")) {
     SEND_TO_Q("\r\nKanassans are the amphibian creatures of the planet Kanassa, a race of beings that possess the ability of clairvoyance along with some mild telepathic powers.  Along with their amphibian nature comes the elemental power to manipulate water as well as to breath underwater.\r\n", d);
     SEND_TO_Q("\r\nDifficulty Rating: Very Hard\r\n", d);
     SEND_TO_Q("\r\nPick a Race: ", d);
     return;
    }
    if (!strcmp(arg, "list")) {
     SEND_TO_Q(class_menu, d);
     SEND_TO_Q("\r\nPick a Race: ", d); 
     return;
    }
    load_result = parse_class(*arg);
    if (load_result == CLASS_UNDEFINED) {
      SEND_TO_Q("\r\nThat's not a race.\r\nPick a Race: ", d);
      return;
    } else
       GET_CLASS(d->character) = load_result;
    STATE(d) = CON_QEYE;
    SEND_TO_Q("\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "       \x1B[1;37mWhat color eyes do you have?\x1B[0;0m\r\n"
              "\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "\x1b[1;34m(\x1b[0;32m1\x1b[1;34m)\x1B[0;0m I have \x1B[1;30mblack\x1B[0;0m eyes.\r\n"
              "\x1b[1;34m(\x1b[0;32m2\x1b[1;34m)\x1B[0;0m I have \x1B[1;37mwhite\x1B[0;0m eyes.\r\n"
              "\x1b[1;34m(\x1b[0;32m3\x1b[1;34m)\x1B[0;0m I have \x1B[1;34mblue\x1B[0;0m eyes.\r\n"
              "\x1b[1;34m(\x1b[0;32m4\x1b[1;34m)\x1B[0;0m I have \x1B[1;32mgreen\x1B[0;0m eyes.\r\n"
              "\x1b[1;34m(\x1b[0;32m5\x1b[1;34m)\x1B[0;0m I have \x1B[33mbrown\x1B[0;0m eyes.\r\n"
              "\x1b[1;34m(\x1b[0;32m6\x1b[1;34m)\x1B[0;0m I have \x1B[1;35mpurple\x1B[0;0m eyes.\r\n"
              "\x1b[1;34m(\x1b[0;32m7\x1b[1;34m)\x1B[0;0m I have \x1B[1;31mred\x1B[0;0m eyes.\r\n"
              "\x1b[1;34m(\x1b[0;32m8\x1b[1;34m)\x1B[0;0m I have \x1B[1;33myellow\x1B[0;0m eyes.\r\n"
              "\x1b[1;34m(\x1b[0;32m9\x1b[1;34m)\x1B[0;0m I have \x1B[1;36micy blue\x1B[0;0m eyes.\r\n"
              "\x1B[36mEye Color\x1B[0;0m:\r\n", d);
    break;

 case CON_QEYE:		
    switch (*arg) {
    case '1':
      d->character->player.eye = EYE_BLACK;
      break;
    case '2':
      d->character->player.eye = EYE_WHITE;
      break;
    case '3':
      d->character->player.eye = EYE_BLUE;
      break;
    case '4':
      d->character->player.eye = EYE_GREEN;  
      break;
    case '5':
      d->character->player.eye = EYE_BROWN;
      break;
    case '6':
      d->character->player.eye = EYE_PURPLE;
      break;
    case '7':
      d->character->player.eye = EYE_RED;
      break;
    case '8':
      d->character->player.eye = EYE_YELLOW;
      break;                
    case '9':
      d->character->player.eye = EYE_ICYBLUE;
      break;
    default:
      SEND_TO_Q("That is not an eye color.\r\n", d);
      return;
    }
    STATE(d) = CON_QHAIRL;
    if (IS_KANASSAN(d->character)) {
    SEND_TO_Q("\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "       \x1B[1;37mWhat is your scale length?\x1B[0;0m\r\n"
              "\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "\x1b[1;34m(\x1b[0;32m1\x1b[1;34m)\x1B[0;0m I have very short scales.\r\n"
              "\x1b[1;34m(\x1b[0;32m2\x1b[1;34m)\x1B[0;0m I have short scales.     \r\n"
              "\x1b[1;34m(\x1b[0;32m3\x1b[1;34m)\x1B[0;0m I have medium length scales. \r\n"
              "\x1b[1;34m(\x1b[0;32m4\x1b[1;34m)\x1B[0;0m I have long scales.      \r\n"
              "\x1b[1;34m(\x1b[0;32m5\x1b[1;34m)\x1B[0;0m I have very long scales. \r\n"
              "\x1b[1;34m(\x1b[0;32m6\x1b[1;34m)\x1B[0;0m I have no scales. \r\n"
              "\x1b[1;34m(\x1b[0;32m7\x1b[1;34m)\x1B[0;0m I have short spikey scales. \r\n"
              "\x1b[1;34m(\x1b[0;32m8\x1b[1;34m)\x1B[0;0m I have medium spikey scales. \r\n"
              "\x1b[1;34m(\x1b[0;32m9\x1b[1;34m)\x1B[0;0m I have long spikey scales. \r\n"
              "\x1B[36mScale Length\x1B[0;0m:\r\n", d);
    }
    else if (IS_MAJIN(d->character)) {
    SEND_TO_Q("\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "       \x1B[1;37mWhat is your forelock length?\x1B[0;0m\r\n"
              "\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "\x1b[1;34m(\x1b[0;32m1\x1b[1;34m)\x1B[0;0m I have a very short forelock.\r\n"
              "\x1b[1;34m(\x1b[0;32m2\x1b[1;34m)\x1B[0;0m I have a short forelock.     \r\n"
              "\x1b[1;34m(\x1b[0;32m3\x1b[1;34m)\x1B[0;0m I have a medium length forelock. \r\n"
              "\x1b[1;34m(\x1b[0;32m4\x1b[1;34m)\x1B[0;0m I have a long forelock.      \r\n"
              "\x1b[1;34m(\x1b[0;32m5\x1b[1;34m)\x1B[0;0m I have a very long forelock. \r\n"
              "\x1b[1;34m(\x1b[0;32m6\x1b[1;34m)\x1B[0;0m I have a no forelock. \r\n"
              "\x1b[1;34m(\x1b[0;32m7\x1b[1;34m)\x1B[0;0m I have a short spikey forelock. \r\n"
              "\x1b[1;34m(\x1b[0;32m8\x1b[1;34m)\x1B[0;0m I have a medium spikey forelock. \r\n"
              "\x1b[1;34m(\x1b[0;32m9\x1b[1;34m)\x1B[0;0m I have a long spikey forelock. \r\n"
              "\x1B[36mForelock Length\x1B[0;0m:\r\n", d);
    }
    else if (IS_Namek(d->character)) {
    SEND_TO_Q("\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "       \x1B[1;37mWhat is your antennae length?\x1B[0;0m\r\n"
              "\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "\x1b[1;34m(\x1b[0;32m1\x1b[1;34m)\x1B[0;0m I have very short antennae.\r\n"
              "\x1b[1;34m(\x1b[0;32m2\x1b[1;34m)\x1B[0;0m I have short antennae.     \r\n"
              "\x1b[1;34m(\x1b[0;32m3\x1b[1;34m)\x1B[0;0m I have medium length antennae. \r\n"
              "\x1b[1;34m(\x1b[0;32m4\x1b[1;34m)\x1B[0;0m I have long antennae.      \r\n"
              "\x1b[1;34m(\x1b[0;32m5\x1b[1;34m)\x1B[0;0m I have very long antennae. \r\n"
              "\x1b[1;34m(\x1b[0;32m6\x1b[1;34m)\x1B[0;0m I have no antennae. \r\n"
              "\x1b[1;34m(\x1b[0;32m7\x1b[1;34m)\x1B[0;0m I have short spikey antennae. \r\n"
              "\x1b[1;34m(\x1b[0;32m8\x1b[1;34m)\x1B[0;0m I have medium spikey antennae. \r\n"
              "\x1b[1;34m(\x1b[0;32m9\x1b[1;34m)\x1B[0;0m I have long spikey antennae. \r\n"
              "\x1B[36mAntennae Length\x1B[0;0m:\r\n", d);
    }
    else {
    SEND_TO_Q("\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "       \x1B[1;37mWhat is your hair length?\x1B[0;0m\r\n"
              "\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "\x1b[1;34m(\x1b[0;32m1\x1b[1;34m)\x1B[0;0m I have very short hair.\r\n"
              "\x1b[1;34m(\x1b[0;32m2\x1b[1;34m)\x1B[0;0m I have short hair.     \r\n"
              "\x1b[1;34m(\x1b[0;32m3\x1b[1;34m)\x1B[0;0m I have medium length hair. \r\n"
              "\x1b[1;34m(\x1b[0;32m4\x1b[1;34m)\x1B[0;0m I have long hair.      \r\n"
              "\x1b[1;34m(\x1b[0;32m5\x1b[1;34m)\x1B[0;0m I have very long hair. \r\n"
              "\x1b[1;34m(\x1b[0;32m6\x1b[1;34m)\x1B[0;0m I have no hair. \r\n"
              "\x1b[1;34m(\x1b[0;32m7\x1b[1;34m)\x1B[0;0m I have short spikey hair. \r\n"
              "\x1b[1;34m(\x1b[0;32m8\x1b[1;34m)\x1B[0;0m I have medium spikey hair. \r\n"
              "\x1b[1;34m(\x1b[0;32m9\x1b[1;34m)\x1B[0;0m I have long spikey hair. \r\n"
              "\x1B[36mHair Length\x1B[0;0m:\r\n", d);
     }
    break;
    
 case CON_QHAIRL:
    switch (*arg) {
    case '1':
      d->character->player.hairl = HAIRL_V_SHORT;
      break;
    case '2':
      d->character->player.hairl = HAIRL_SHORT;
      break;
    case '3':
      d->character->player.hairl = HAIRL_MEDIUM;
      break;
    case '4':
      d->character->player.hairl = HAIRL_LONG;
      break;
    case '5':
      d->character->player.hairl = HAIRL_V_LONG;
      break;      
    case '6':
      d->character->player.hairl = HAIRL_BALD;
      break;
    case '7':
      d->character->player.hairl = HAIRL_SSPIKE;
      break;
    case '8':
      d->character->player.hairl = HAIRL_MSPIKE;
      break;
    case '9':
      d->character->player.hairl = HAIRL_LSPIKE;
      break;
    default:
      if (IS_KANASSAN(d->character)) {
       SEND_TO_Q("That is not a scale length.\r\n", d);
      }
      else if (IS_Namek(d->character)) {
       SEND_TO_Q("That is not a antennae length.\r\n", d);
      }
      else if (IS_MAJIN(d->character)) {
       SEND_TO_Q("That is not a forelock length.\r\n", d);
      }
      else {
      SEND_TO_Q("That is not a hair length.\r\n", d);
      return;
      }
    }
    if (IS_Namek(d->character)) {
     d->character->player.hairc = HAIRC_YELLOW;
     STATE(d) = CON_QAURA;
    }
    else if (IS_MAJIN(d->character)) {
     d->character->player.hairc = HAIRC_YELLOW;
     STATE(d) = CON_QAURA;
    }
    else if (d->character->player.hairl == HAIRL_BALD) {
     d->character->player.hairc = HAIRC_NONE;
     STATE(d) = CON_QAURA;
    }
   else {
    if (IS_KANASSAN(d->character)) {
    STATE(d) = CON_QHAIRC;
    SEND_TO_Q("\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "       \x1B[1;37mWhat is your scale color?\x1B[0;0m\r\n"
              "\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "\x1b[1;34m(\x1b[0;32m1\x1b[1;34m)\x1B[0;0m I have \x1B[1;30mblack\x1B[0;0m scales\r\n"
              "\x1b[1;34m(\x1b[0;32m2\x1b[1;34m)\x1B[0;0m I have \x1B[1;37msilver\x1B[0;0m scales\r\n"
              "\x1b[1;34m(\x1b[0;32m3\x1b[1;34m)\x1B[0;0m I have \x1B[1;34mblue\x1B[0;0m scales\r\n"
              "\x1b[1;34m(\x1b[0;32m4\x1b[1;34m)\x1B[0;0m I have \x1B[1;32mgreen\x1B[0;0m scales\r\n"
              "\x1b[1;34m(\x1b[0;32m5\x1b[1;34m)\x1B[0;0m I have \x1B[33mbrown\x1B[0;0m scales\r\n"
              "\x1b[1;34m(\x1b[0;32m6\x1b[1;34m)\x1B[0;0m I have \x1B[1;35mpurple\x1B[0;0m scales\r\n"
              "\x1b[1;34m(\x1b[0;32m7\x1b[1;34m)\x1B[0;0m I have \x1B[1;31mred\x1B[0;0m scales\r\n"
              "\x1b[1;34m(\x1b[0;32m8\x1b[1;34m)\x1B[0;0m I have \x1B[1;33myellow\x1B[0;0m scales\r\n"
              "\x1b[1;34m(\x1b[0;32m9\x1b[1;34m)\x1B[0;0m I have \x1B[0;0mno scales\r\n"
              "\x1B[36mScale Color\x1B[0;0m:\r\n", d);
    }
    else {
    STATE(d) = CON_QHAIRC;
    SEND_TO_Q("\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "       \x1B[1;37mWhat is your hair color?\x1B[0;0m\r\n"
              "\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "\x1b[1;34m(\x1b[0;32m1\x1b[1;34m)\x1B[0;0m I have \x1B[1;30mblack\x1B[0;0m hair\r\n"
              "\x1b[1;34m(\x1b[0;32m2\x1b[1;34m)\x1B[0;0m I have \x1B[1;37msilver\x1B[0;0m hair\r\n"
              "\x1b[1;34m(\x1b[0;32m3\x1b[1;34m)\x1B[0;0m I have \x1B[1;34mblue\x1B[0;0m hair\r\n"
              "\x1b[1;34m(\x1b[0;32m4\x1b[1;34m)\x1B[0;0m I have \x1B[1;32mgreen\x1B[0;0m hair\r\n"
              "\x1b[1;34m(\x1b[0;32m5\x1b[1;34m)\x1B[0;0m I have \x1B[33mbrown\x1B[0;0m hair\r\n"
              "\x1b[1;34m(\x1b[0;32m6\x1b[1;34m)\x1B[0;0m I have \x1B[1;35mpurple\x1B[0;0m hair\r\n"
              "\x1b[1;34m(\x1b[0;32m7\x1b[1;34m)\x1B[0;0m I have \x1B[1;31mred\x1B[0;0m hair\r\n"
              "\x1b[1;34m(\x1b[0;32m8\x1b[1;34m)\x1B[0;0m I have \x1B[1;33myellow\x1B[0;0m hair\r\n"
              "\x1b[1;34m(\x1b[0;32m9\x1b[1;34m)\x1B[0;0m I have \x1B[0;0mno hair\r\n"
              "\x1B[36mHair Color\x1B[0;0m:\r\n", d);
    }
    break;

 case CON_QHAIRC:		
    switch (*arg) {
    case '1':
      d->character->player.hairc = HAIRC_BLACK;
      break;
    case '2':
      d->character->player.hairc = HAIRC_WHITE;
      break;
    case '3':
      d->character->player.hairc = HAIRC_BLUE;
      break;
    case '4':
      d->character->player.hairc = HAIRC_GREEN;  
      break;
    case '5':
      d->character->player.hairc = HAIRC_BROWN;
      break;
    case '6':
      d->character->player.hairc = HAIRC_PURPLE;
      break;
    case '7':
      d->character->player.hairc = HAIRC_RED;
      break;
    case '8':
      d->character->player.hairc = HAIRC_YELLOW;
      break;
    case '9':
      d->character->player.hairc = HAIRC_NONE;
      break;
    default:
      if (IS_KANASSAN(d->character)) {
      SEND_TO_Q("That is not a scale color.\r\n", d);
      }
      else {
      SEND_TO_Q("That is not a hair color.\r\n", d);
      }
      return;
    }
    }

    STATE(d) = CON_QAURA;
    SEND_TO_Q("\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "       \x1B[1;37mWhat is your skin color?\x1B[0;0m\r\n"
              "\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "\x1b[1;34m(\x1b[0;32m1\x1b[1;34m)\x1B[0;0m I have \x1B[1;30mblack\x1B[0;0m skin\r\n"
              "\x1b[1;34m(\x1b[0;32m2\x1b[1;34m)\x1B[0;0m I have \x1B[1;37mwhite\x1B[0;0m skin\r\n"
              "\x1b[1;34m(\x1b[0;32m3\x1b[1;34m)\x1B[0;0m I have \x1B[1;34mblue\x1B[0;0m skin\r\n"
              "\x1b[1;34m(\x1b[0;32m4\x1b[1;34m)\x1B[0;0m I have \x1B[1;32mgreen\x1B[0;0m skin\r\n"
              "\x1b[1;34m(\x1b[0;32m5\x1b[1;34m)\x1B[0;0m I have \x1B[1;35mpink\x1B[0;0m skin\r\n"
              "\x1b[1;34m(\x1b[0;32m6\x1b[1;34m)\x1B[0;0m I have \x1B[1;31mred\x1B[0;0m skin\r\n"
              "\x1b[1;34m(\x1b[0;32m7\x1b[1;34m)\x1B[0;0m I have \x1B[1;33mtan\x1B[0;0m skin\r\n"
              "\x1b[1;34m(\x1b[0;32m8\x1b[1;34m)\x1B[0;0m I have \x1B[1;35mpurple\x1B[0;0m skin\r\n"
              "\x1B[36mSkin Color\x1B[0;0m:\r\n", d);
    break;
 case CON_QAURA:               
    switch (*arg) {
    case '1':
      d->character->player.aura = AURA_BLACK;
      break;
    case '2':
      d->character->player.aura = AURA_WHITE;
      break;
    case '3':
      d->character->player.aura = AURA_BLUE;
      break;
    case '4':
      d->character->player.aura = AURA_GREEN;
      break;
    case '5':
      d->character->player.aura = AURA_PURPLE;
      break;
    case '6':
      d->character->player.aura = AURA_RED;
      break;
    case '7':
      d->character->player.aura = AURA_YELLOW;
      break;
    case '8':
      d->character->player.aura = AURA_PINK;
      break;
    default:
      SEND_TO_Q("That is not a skin color.\r\n", d);
      return;
    }

    STATE(d) = CON_QGOD;
    SEND_TO_Q("\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "       \x1B[1;37mNo REMAKING at all.\x1B[0;0m\r\n"
              "\x1b[1;34m<>\x1b[0;37m=======================================\x1b[1;34m<>\x1B[0;0m\r\n"
              "\x1b[1;34m(\x1b[0;32m1\x1b[1;34m)\x1B[0;0m I understand, no remaking.\r\n"
              "\x1B[36mOk?\x1B[0;0m:\r\n", d);
    break;
 case CON_QGOD:               
    switch (*arg) {
    case '1':
      d->character->player.god = GOD_NONE;
      break;
    default:
      SEND_TO_Q("Agree to no remaking punk!\r\n", d);
      return;
    }

    if (GET_PFILEPOS(d->character) < 0)
      GET_PFILEPOS(d->character) = create_entry(GET_NAME(d->character));
    init_char(d->character);
    save_char(d->character, NOWHERE);
    SEND_TO_Q(motd, d);
    SEND_TO_Q("\r\n\n&12Please Enjoy your stay on &11Drag&12(&09*&12)&11nBall &16Resurrection of &12T&00&07r&00&06u&00&07t&12h&11!&00 ", d);
    SEND_TO_Q("\r\n\n*** PRESS RETURN: ", d);
    STATE(d) = CON_RMOTD;

    sprintf(buf, "%s [%s] new player.", GET_NAME(d->character), d->host);
    mudlog(buf, NRM, LVL_IMMORT, TRUE);
    sprintf(buf2, "&16[&12Welcome&16] &14%s &15is a new player&09!&00\r\n", GET_NAME(d->character));
    send_to_all(buf2);
    break;
    
  case CON_RMOTD:		/* read CR after printing motd   */
    SEND_TO_Q(MENU, d);
    STATE(d) = CON_MENU;
    break;

  case CON_MENU:		/* get selection from main menu  */
    switch (*arg) {
    case '0':
      SEND_TO_Q("Goodbye.\r\n", d);
      STATE(d) = CON_CLOSE;
      break;

    case '1':
 load_result = enter_player_game(d);
      send_to_char(WELC_MESSG, d->character);
  
      STATE(d) = CON_PLAYING;
      if (!GET_LEVEL(d->character)) {
	do_start(d->character);
	send_to_char(START_MESSG, d->character);
          do_newbie(d->character);
      }
      look_at_room(d->character, 0);
      if (GET_IDNUM(d->character) == 1) {
        send_to_char("Type version and remember only assholes delete from it.\r\n", d->character);
      }
      if (GET_LEVEL(d->character) < LVL_IMMORT) {
      sprintf(buf2, "&16[&09Player Login&16] &15%s has joined &11Drag&12(&09*&12)&11n Ball - &16Resurrection of &12T&00&07r&00&06u&00&07t&12h&10!&00\r\n", GET_NAME(d->character));
      send_to_all(buf2);
      }
      if (GET_LEVEL(d->character) >= 1000000)
        send_to_char("\r\n\r\n&16[&10I&00&02m&10m&00&02o&10r&00&02t&10a&00&02l &09A&00&01l&09e&00&01r&09t &14S&00&06y&14s&00&06t&14e&00&06m&16] &15Report to the main imm board and the coder board and\r\nread both everytime you log on&11!&00\r\n\r\n", d->character);
      if (has_mail(GET_IDNUM(d->character)))
	send_to_char("&09You have mail waiting!&11!&09!&11!&09!&11!\r\n&10YES &09You really have mail waiting!&11!&09!&11!&09!&11!\r\n&09You &10had &09better check your mail already!&11!&09!&11!&09!&11!\r\n", d->character);
      if (GET_ALIGNMENT(d->character) > GET_TPEARN(d->character) && GET_ALIGNMENT(d->character) != GET_TPEARN(d->character)) {
        *buf2 = '\0';
        earnnum = GET_ALIGNMENT(d->character) - GET_TPEARN(d->character);
        sprintf(buf2, "&15You have earned &14%d &12T&00&07r&00&06u&00&07t&12h &11Points &15since your last login.&00\r\n", earnnum);
        send_to_char(buf2, d->character);
        GET_TPEARN(d->character) = GET_ALIGNMENT(d->character);
      }
      if (load_result == 2) {	/* rented items lost */
	send_to_char("\r\n\007You could not afford your rent!\r\n"
	  "Your possesions have been donated to the Salvation Army!\r\n",
		     d->character);
      }
      d->has_prompt = 0;
      break;

    case '2':
      if (GET_MAX_HIT(d->character) <= 1499999) {
       SEND_TO_Q("Your character must be at least 1500000 pl to make a description.\r\n", d);
       SEND_TO_Q(MENU, d);
       break;
      }         
      if (d->character->player.description) {
	SEND_TO_Q("Current description:\r\n", d);
	SEND_TO_Q(d->character->player.description, d);
	/*
	 * Don't free this now... so that the old description gets loaded
	 * as the current buffer in the editor.  Do setup the ABORT buffer
	 * here, however.
	 *
	 * free(d->character->player.description);
	 * d->character->player.description = NULL;
	 */
	d->backstr = str_dup(d->character->player.description);
      }
      SEND_TO_Q("Enter the new text you'd like others to see when they look at you.\r\n", d);
      SEND_TO_Q("(/s saves /h for help)\r\n", d);
      d->str = &d->character->player.description;
      d->max_str = EXDSCR_LENGTH;
      STATE(d) = CON_EXDESC;
      break;

    case '3':
      page_string(d, background, 0);
      WAIT_STATE(d->character, PULSE_VIOLENCE * .5);
      STATE(d) = CON_RMOTD;
      break;

    case '4':
      SEND_TO_Q("\r\nEnter your password for verification: ", d);
      echo_off(d);
      STATE(d) = CON_DELCNF1;
      break;

    default:
      SEND_TO_Q("\r\nThat's not a menu choice!\r\n", d);
      WAIT_STATE(d->character, PULSE_VIOLENCE * .5);
      SEND_TO_Q(MENU, d);
      break;
    }

    break;

  case CON_CHPWD_GETOLD:
    if (strncmp(CRYPT(arg, GET_PASSWD(d->character)), GET_PASSWD(d->character), MAX_PWD_LENGTH)) {
      echo_on(d);
      SEND_TO_Q("\r\nIncorrect password.\r\n", d);
      SEND_TO_Q(MENU, d);
      STATE(d) = CON_MENU;
    } else {
      SEND_TO_Q("\r\nEnter a new password: ", d);
      STATE(d) = CON_CHPWD_GETNEW;
    }
    return;

  case CON_DELCNF1:
    echo_on(d);
    if (strncmp(CRYPT(arg, GET_PASSWD(d->character)), GET_PASSWD(d->character), MAX_PWD_LENGTH)) {
      SEND_TO_Q("\r\nIncorrect password.\r\n", d);
      SEND_TO_Q(MENU, d);
      STATE(d) = CON_MENU;
    } else {
      SEND_TO_Q("\r\nYOU ARE ABOUT TO DELETE THIS CHARACTER PERMANENTLY.\r\n"
		"ARE YOU ABSOLUTELY SURE?\r\n\r\n"
		"Please type \"yes\" to confirm: ", d);
      STATE(d) = CON_DELCNF2;
    }
    break;

  case CON_DELCNF2:
    if (!strcmp(arg, "yes") || !strcmp(arg, "YES")) {
     if (GET_LEVEL(d->character) <= 999) {
       SEND_TO_Q("Your character must be at least a well played character to delete.\r\n", d);
       STATE(d) = CON_CLOSE;
       break;
      }
      if (PLR_FLAGGED(d->character, PLR_FROZEN)) {
	SEND_TO_Q("You try to kill yourself, but the ice stops you.\r\n", d);
	SEND_TO_Q("Character not deleted.\r\n\r\n", d);
	STATE(d) = CON_CLOSE;
	return;
      }
      if (GET_LEVEL(d->character) < LVL_GRGOD)
	SET_BIT(PLR_FLAGS(d->character), PLR_DELETED);
      save_char(d->character, NOWHERE);
      Crash_delete_file(GET_NAME(d->character));
      sprintf(buf, "Character '%s' deleted!\r\n"
	      "Goodbye.\r\n", GET_NAME(d->character));
      SEND_TO_Q(buf, d);
      sprintf(buf, "%s (lev %Ld) has self-deleted.", GET_NAME(d->character),
	      GET_LEVEL(d->character));
      mudlog(buf, NRM, LVL_GOD, TRUE);
      STATE(d) = CON_CLOSE;
      return;
    } else {
      SEND_TO_Q("\r\nCharacter not deleted.\r\n", d);
      SEND_TO_Q(MENU, d);
      STATE(d) = CON_MENU;
    }
    break;

/*	Taken care of in game_loop()
  case CON_CLOSE:
    close_socket(d);
    break;
*/

  default:
    log("SYSERR: Nanny: illegal state of con'ness (%d) for '%s'; closing connection.",
	STATE(d), d->character ? GET_NAME(d->character) : "<unknown>");
    STATE(d) = CON_DISCONNECT;	/* Safest to do. */
    break;
  }
}

