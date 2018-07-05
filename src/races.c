
#include "sysdep.h"
#include "conf.h"

#include "structs.h"
#include "interpreter.h"
#include "utils.h"

const char *race_abbrevs[] = {
  "Human  ",
  "Saiyan ",
  "Namek  ",
  "Icer   ",
  "Majin  ",
  "Android",
  "\n"
};

const char *pc_race_types[] = {
  "Human  ",
  "Saiyan ",
  "Namek  ",
  "Icer   ",
  "Majin  ",
  "Android",
  "\n"
};

/* The menu for choosing a race in interpreter.c: */
const char *race_menu =
"\r\n"
"Select a race:\r\n"
"  (1) Human\r\n"
"  (2) Saiyan\r\n"
"  (3) Namek\r\n"
"  (4) Icer\r\n"
"  (5) Majin\r\n"
"  (6) Android\r\n";

/*
 * The code to interpret a race letter (used in interpreter.c when a
 * new character is selecting a race).
 */
int parse_race(char arg)
{
  arg = LOWER(arg);

  switch (arg) {
  case '1':
    return RACE_HUMAN;
    break;
  case '2':
    return RACE_SAIYAN;
    break;
  case '3':
    return RACE_NAMEK;
    break;
  case '4':
    return RACE_ICER;
    break;
  case '5':
    return RACE_MAJIN;
    break;
  case '6':
    return RACE_ANDROID;
    break;
  default:
    return RACE_UNDEFINED;
    break;
  }
}

long find_race_bitvector(char arg)
{
  arg = LOWER(arg);

  switch (arg) {
    case '0':
      return (1 << 0);
      break;
    case '1':
      return (1 << 1);
      break;
    case '2':
      return (1 << 2);
      break;
    case '3':
      return (1 << 3);
      break;
    case '4':
      return (1 << 4);
      break;
    case '5':
      return (1 << 5);
      break;
    default:
      return 0;
      break;
  }
}