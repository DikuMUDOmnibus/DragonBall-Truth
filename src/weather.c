/*************************************************************************
*   File: weather.c                                     Part of CircleMUD *
*  Usage: functions handling time and the weather                         *
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

extern struct time_info_data time_info;
extern struct descriptor_data *descriptor_list;
extern struct room_data *world;
void weather_and_time(int mode);
void another_hour(int mode);
void weather_change(void);


void weather_and_time(int mode)
{
  another_hour(mode);
  if (mode)
    weather_change();
}



void another_hour(int mode)
{
  struct descriptor_data *d; 
  time_info.hours++;
 
  if (mode) {
	 switch (time_info.hours) {
	 case 5:
		weather_info.sunlight = SUN_RISE;
		send_to_outdoor("The &11sun &00&07rises in the east.&00\r\n");
		break;
	 case 6:
		weather_info.sunlight = SUN_LIGHT;
		send_to_outdoor("The day has begun.\r\n");
		break;
	 case 4:
		for(d = descriptor_list;d;d = d->next) {
    if (STATE(d) != CON_PLAYING) {
        continue;
    }
    if (!d->character) {
        continue;
    }
if (time_info.day == 21 && PRF_FLAGGED(d->character, PRF_OOZARU))              {
REMOVE_BIT(PRF_FLAGS(d->character), PRF_OOZARU);
act("&12$n starts to grow smaller and the hair seems to just absorb back into there skin. they have become Saiyan once more &00.", FALSE, d->character, 0, 0, TO_ROOM);
send_to_char("&12Your rage calms down and you begin to slowly shrink as the hair on your body disolves and you pass out on the ground.&00\n\r", d->character);
GET_POS(d->character) = 4;
   GET_MAX_HIT(d->character) -= 1000000;
   GET_MAX_MANA(d->character) -= 1000000;
}}

    break;
	 case 20:
         for(d = descriptor_list;d;d = d->next) {
    if (STATE(d) != CON_PLAYING) {
        continue;
    }
    if (!d->character) {
        continue;
    }
if (time_info.day == 18 && PLR_FLAGGED(d->character, PLR_STAIL) && !PRF2_FLAGGED(d->character, PRF2_GO)) {
SET_BIT(PRF_FLAGS(d->character), PRF_OOZARU);
send_to_char("&09You look at the moon and begin to lose control and\rrage takes over your mind,\r\n you see nothing but red and begin to \rtransform, you begin to grow and hair develops all over your body!\r\nYou grow into a large monkey form!&00\n\r", d->character);
act("&09$n gazes towards the moon and starts to shake in uncontrollable rage,\rthey start to grow hair and become taller. They have become the dreaded oozaru!!&00", FALSE, d->character, 0, 0, TO_ROOM);
  GET_MAX_HIT(d->character) += 1000000; GET_MAX_MANA(d->character) += 1000000;
}}
 break;
 
	 case 21:
      weather_info.sunlight = SUN_SET;
      send_to_outdoor("The &11sun&00&07 slowly disappears in the west.\r\n");
      break;
    case 22:
      weather_info.sunlight = SUN_DARK;
      send_to_outdoor("The &16night&00 has begun.\r\n");
      break;
    default:
      break;
    }
  }


  if (time_info.hours > 23) {	/* Changed by HHS due to bug ??? */
    time_info.hours -= 24;
    time_info.day++;

    if (time_info.day > 34) {
      time_info.day = 0;
      time_info.month++;

      if (time_info.month > 16) {
	time_info.month = 0;
	time_info.year++;

      }
    }
  }
}


void weather_change(void)
{
  int diff, change;
  if ((time_info.month >= 9) && (time_info.month <= 16))
    diff = (weather_info.pressure > 985 ? -2 : 2);
  else
    diff = (weather_info.pressure > 1015 ? -2 : 2);

  weather_info.change += (dice(1, 4) * diff + dice(2, 6) - dice(2, 6));

  weather_info.change = MIN(weather_info.change, 12);
  weather_info.change = MAX(weather_info.change, -12);

  weather_info.pressure += weather_info.change;

  weather_info.pressure = MIN(weather_info.pressure, 1040);
  weather_info.pressure = MAX(weather_info.pressure, 960);

  change = 0;

  switch (weather_info.sky) {
  case SKY_CLOUDLESS:
    if (weather_info.pressure < 990)
      change = 1;
    else if (weather_info.pressure < 1010)
      if (dice(1, 4) == 1)
	change = 1;
    break;
  case SKY_CLOUDY:
    if (weather_info.pressure < 970)
      change = 2;
    else if (weather_info.pressure < 990) {
      if (dice(1, 4) == 1)
	change = 2;
      else
	change = 0;
    } else if (weather_info.pressure > 1030)
      if (dice(1, 4) == 1)
	change = 3;

    break;
  case SKY_RAINING:
    if (weather_info.pressure < 970) {
      if (dice(1, 4) == 1)
	change = 4;
      else
	change = 0;
    } else if (weather_info.pressure > 1030)
      change = 5;
    else if (weather_info.pressure > 1010)
      if (dice(1, 4) == 1)
	change = 5;

    break;
  case SKY_LIGHTNING:
    if (weather_info.pressure > 1010)
      change = 6;
    else if (weather_info.pressure > 990)
      if (dice(1, 4) == 1)
	change = 6;

    break;
  default:
    change = 0;
    weather_info.sky = SKY_CLOUDLESS;
    break;
  }

  switch (change) {
  case 0:
    break;
  case 1:
    send_to_outdoor("The sky starts to get &16c&15l&00&07ou&15d&16y&00.\r\n");
    weather_info.sky = SKY_CLOUDY;
    break;
  case 2:
    send_to_outdoor("It starts to &14r&00&06a&12i&00&04n&00.\r\n");
    weather_info.sky = SKY_RAINING;
    break;
  case 3:
    send_to_outdoor("The &16c&15l&00&07ou&15d&16s &00&07disappear.&00\r\n");
    weather_info.sky = SKY_CLOUDLESS;
    break;
  case 4:
    send_to_outdoor("&00&07L&11i&00&03g&00&07h&11t&00&03n&00&07i&11n&00&03g&00&07 starts to show in the sky.&00\r\n");
    weather_info.sky = SKY_LIGHTNING;
    break;
  case 5:
    send_to_outdoor("The &14r&00&06a&12i&00&04n&00&07 stops.&00\r\n");
    weather_info.sky = SKY_CLOUDY;
    break;
  case 6:
    send_to_outdoor("The &00&07l&11i&00&03g&00&07h&11t&00&03n&00&07i&11n&00&03g&00&07 stops.\r\n");
    weather_info.sky = SKY_RAINING;
    break;
  default:
    break;
  }
}
