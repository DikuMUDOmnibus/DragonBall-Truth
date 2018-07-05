#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "screen.h"
#include "constants.h"

#include "auction.h"

extern struct descriptor_data *descriptor_list;
extern struct room_data *world;

/*
 * auction_output : takes two strings and dispenses them to everyone connected
 *		based on if they have color on or not.  Note that the buf's are
 *		commonly used *color and *black so I allocate my own buffer.
 */
void auction_output(char *color, char *black)
{
  char buffer[MAX_STRING_LENGTH];
  struct descriptor_data *d;

  for (d = descriptor_list; d; d = d->next)
    if (!d->connected && d->character &&
       !PLR_FLAGGED(d->character, PLR_WRITING) &&
       !ROOM_FLAGGED(d->character->in_room, ROOM_SOUNDPROOF)) {
      sprintf(buffer, "%s&16[%sAUCTION:%s %s%s&16]%s\r\n",
	CCMAG(d->character,C_NRM),CCCYN(d->character,C_NRM),
	CCNRM(d->character,C_NRM),
	(COLOR_LEV(d->character) > C_NRM) ? color : black,
	CCMAG(d->character,C_NRM),CCNRM(d->character,C_NRM));
      send_to_char(buffer, d->character);
    }
}

void auction_update(void)
{
  int i, found;
  if (auction.ticks == AUC_NONE) /* No auction */
    return;  

  /* Seller left! */
  if (!get_ch_by_id(auction.seller)) {
    if (auction.obj)
      extract_obj(auction.obj);
    auction_reset();
    return;
  }

  /* If there is an auction but it's not sold yet */
  if (auction.ticks >= AUC_BID && auction.ticks <= AUC_SOLD) {
    /* If there is a bidder and it's not sold yet */
    if (get_ch_by_id(auction.bidder) && (auction.ticks < AUC_SOLD)) {
      /* Non colored message */
      sprintf(buf, "%s&14 is going %s%s%s&14 to %s &14for %ld &14coin%s&11.",
          auction.obj->short_description,
          auction.ticks == AUC_BID ? "&14once&00" : "",
          auction.ticks == AUC_ONCE ? "&14twice&00" : "",
          auction.ticks == AUC_TWICE ? "&14for the last call&00" : "",
          get_ch_by_id(auction.bidder)->player.name,
          auction.bid, auction.bid != 1 ? "s" : " ");
      /* Colored message */
      sprintf(buf2, "\x1B[1;37m%s\x1B[35m&14 is going \x1B[1;37m%s%s%s\x1B[35m&14 to \x1B[1;37m%s\x1B[35m&14 for \x1B[1;37m%ld\x1B[35m &14coin%s&11.",
          auction.obj->short_description,
          auction.ticks == AUC_BID ? "&14once&00" : "",
          auction.ticks == AUC_ONCE ? "&14twice&00" : "",
          auction.ticks == AUC_TWICE ? "&14for the last call&00" : "",
          get_ch_by_id(auction.bidder)->player.name,
          auction.bid, auction.bid != 1 ? "s" : " ");
      /* send the output */
      auction_output(buf2, buf);
      /* Increment timer */
      auction.ticks++;
      *buf2 = '\0';
      *buf1 = '\0';
       send_to_all("&15Auction Stats&16:&00");
       for (i = 0; i < MAX_OBJ_AFFECT; i++) {
         if (auction.obj->affected[i].modifier) {
           sprinttype(auction.obj->affected[i].location, apply_types, buf1);
           sprintf(buf2, "&00%s &11%+d &15to &14%s", found++ ? "," : "",
                   auction.obj->affected[i].modifier, buf1);
           send_to_all(buf2);
         }
       }
       send_to_all("\r\n");
      return;
    }

    /* If there is no bidder and we ARE in the sold state */
    if (!get_ch_by_id(auction.bidder) && (auction.ticks == AUC_SOLD)) {
      /* Colored message */
      sprintf(buf2, "\x1B[1;37m%s\x1B[35m &14is \x1B[1;37m&15SOLD\x1B[35m &14to \x1B[1;37mno one\x1B[35m &14for \x1B[1;37m%ld\x1B[35m &14coin%s&11.",
          auction.obj->short_description,
          auction.bid, auction.bid != 1 ? "s" : " ");
      /* No color message */
      sprintf(buf, "%s &14is &15SOLD&14 to no one for %ld &14coin%s&11.",
          auction.obj->short_description,
          auction.bid,
          auction.bid != 1 ? "s" : " ");
      /* Send the output away */
      auction_output(buf2, buf);
      /* Give the poor fellow his unsold goods back */
      obj_to_char(auction.obj, get_ch_by_id(auction.seller));
      /* Reset the auction for next time */
      auction_reset();
      return;
    }

    /* If there is no bidder and we are not in the sold state */
    if (!get_ch_by_id(auction.bidder) && (auction.ticks < AUC_SOLD)) {
      /* Colored output message */
      sprintf(buf2, "\x1B[1;37m%s\x1B[35m &14is going \x1B[1;37m%s%s%s\x1B[35m &14to \x1B[1;37mno one\x1B[35m &14for \x1B[1;37m%ld\x1B[35m &14coin%s&11.",
          auction.obj->short_description,
          auction.ticks == AUC_BID ? "&14once&00" : "",
          auction.ticks == AUC_ONCE ? "&14twice&00" : "",
          auction.ticks == AUC_TWICE ? "&14for the last call&00" : "",
          auction.bid, auction.bid != 1 ? "s" : "");
      /* No color output message */
      sprintf(buf, "%s &14is going %s%s%s &14to no one for %ld &14coin%s&11.",
          auction.obj->short_description,
          auction.ticks == AUC_BID ? "&14once&00" : "",
          auction.ticks == AUC_ONCE ? "&14twice&00" : "",
          auction.ticks == AUC_TWICE ? "&14for the last call&00" : "",
          auction.bid, auction.bid != 1 ? "s" : "");
      /* Send output away */
      auction_output(buf2, buf);
      *buf2 = '\0';
      *buf1 = '\0';
       send_to_all("&15Auction Stats&16:&00");
       for (i = 0; i < MAX_OBJ_AFFECT; i++) {
         if (auction.obj->affected[i].modifier) {
           sprinttype(auction.obj->affected[i].location, apply_types, buf1);
           sprintf(buf2, "&00%s &11%+d &15to &14%s", found++ ? "," : "",
                   auction.obj->affected[i].modifier, buf1);
           send_to_all(buf2);
         }
       }
       send_to_all("\r\n");
      /* Increment timer */
      auction.ticks++;
      return;
    }

    /* Sold */
    if (get_ch_by_id(auction.bidder) && (auction.ticks >= AUC_SOLD)) {
      /* Get pointers to the bidder and seller */
      struct char_data *seller = get_ch_by_id(auction.seller);
      struct char_data *bidder = get_ch_by_id(auction.bidder);

      /* Colored output */
      sprintf(buf2, "\x1B[1;37m%s\x1B[35m &14is \x1B[1;37m&15SOLD\x1B[35m &14to \x1B[1;37m%s\x1B[35m &14for \x1B[1;37m%ld\x1B[35m &14coin%s&11.",
          auction.obj->short_description ? auction.obj->short_description : "something",
          bidder->player.name ? bidder->player.name : "someone",
          auction.bid, auction.bid != 1 ? "s" : "");
      /* Non color output */
      sprintf(buf, "%s &14is &15SOLD &14to %s for %ld coin%s&11.",
          auction.obj->short_description ? auction.obj->short_description : "something",
          bidder->player.name ? bidder->player.name : "someone",
          auction.bid, auction.bid != 1 ? "s" : "");
      /* Send the output */
      auction_output(buf2, buf);
  
      /* If the seller is still around we give him the money */
      if (seller) {
	GET_GOLD(seller) += auction.bid;
        act("Congrats! You have sold $p!", FALSE, seller, auction.obj, 0, TO_CHAR);
      }
      /* If the bidder is here he gets the object */
      if (bidder) {
	obj_to_char(auction.obj, bidder);
	act("Congrats! You now have $p!", FALSE, bidder, auction.obj, 0, TO_CHAR);
      }
      /* Restore the status of the auction */
      auction_reset();
      return;
    }
  }
  return;
}

/*
 * do_bid : user interface to place a bid.
 */
ACMD(do_bid)
{
  long bid;

  /* NPC's can not bid or auction if charmed */
  if (ch->master && AFF_FLAGGED(ch, AFF_MAJIN))
    return;

  /* There isn't an auction */
  if (auction.ticks == AUC_NONE) {
    send_to_char("Nothing is up for sale.\r\n", ch);
    return;
  }

  one_argument(argument, buf);
  bid = atoi(buf);

  /* They didn't type anything else */
  if (!*buf) {
    sprintf(buf2, "Current bid: %ld coin%s\r\n", auction.bid,
        auction.bid != 1 ? "s." : ".");
    send_to_char(buf2, ch);
  /* The current bidder is this person */
  } else if (ch == get_ch_by_id(auction.bidder))
    send_to_char("You're trying to outbid yourself.\r\n", ch);
  /* The seller is the person who tried to bid */
  else if (ch == get_ch_by_id(auction.seller))
    send_to_char("You can't bid on your own item.\r\n", ch);
  /* Tried to auction below the minimum */
  else if ((bid < auction.bid) && !get_ch_by_id(auction.bidder)) {
    sprintf(buf2, "The minimum is currently %ld coins.\r\n", auction.bid);
    send_to_char(buf2, ch);
  /* Tried to bid below the minimum where there is a bid, 5% increases */
  } else if ((bid < (auction.bid * 1.05) && get_ch_by_id(auction.bidder)) || bid == 0) {
    sprintf(buf2, "Try bidding at least 5%% over the current bid of %ld. (%.0f coins).\r\n",
        auction.bid, auction.bid * 1.05 + 1);
    send_to_char(buf2, ch);
  /* Not enough gold on hand! */
  } else if (GET_GOLD(ch) < bid) {
    sprintf(buf2, "You have only %d coins on hand.\r\n", GET_GOLD(ch));
    send_to_char(buf2, ch);
  /* it's an ok bid */
  } else {
    /* Give last bidder money back if he's around! */
    if (get_ch_by_id(auction.bidder))
      GET_GOLD(get_ch_by_id(auction.bidder)) += auction.bid;
    /* This is the bid value */
    auction.bid = bid;
    /* The bidder is this guy */
    auction.bidder = GET_IDNUM(ch);
    /* This resets the auction to first chance bid */
    auction.ticks = AUC_BID;
    /* Get money from new bidder. */
    GET_GOLD(get_ch_by_id(auction.bidder)) -= auction.bid;
    /* Prepare colored message */
    sprintf(buf2, "\x1B[1;37m%s\x1B[35m&14 bids \x1B[1;37m%ld\x1B[35m &14coin%s on \x1B[1;37m%s\x1B[35m&11.",
	get_ch_by_id(auction.bidder)->player.name,
	auction.bid,
	auction.bid!=1 ? "s" :"",
	auction.obj->short_description);
    /* Prepare non-colored message */
    sprintf(buf, "%s bids %ld coin%s on %s.",
	get_ch_by_id(auction.bidder)->player.name,
	auction.bid,
	auction.bid!=1 ? "s" :"",
	auction.obj->short_description);
    /* Send the output away */
    auction_output(buf2, buf);
  }
}

/*
 * do_auction : user interface for placing an item up for sale
 */
ACMD(do_auction)
{
  struct obj_data *obj;
  struct char_data *seller;
  int i, found;

  /* NPC's can not bid or auction if charmed */
  if (ch->master && AFF_FLAGGED(ch, AFF_MAJIN))
    return;

  two_arguments(argument, buf1, buf2);

  seller = get_ch_by_id(auction.seller);

  /* There is nothing they typed */
  if (!*buf1)
    send_to_char("Auction what for what minimum?\r\n", ch);
  /* Hrm...logic error? */
  else if (auction.ticks != AUC_NONE) {
    /* If seller is no longer present, auction continues with no seller */
    if (seller) {
      sprintf(buf2, "%s is currently auctioning %s for %ld coins.\r\n",
        get_ch_by_id(auction.seller)->player.name,
        auction.obj->short_description, auction.bid);
      send_to_char(buf2, ch);
    } else {
      sprintf(buf2, "No one is currently auctioning %s for %ld coins.\r\n",
         auction.obj->short_description, auction.bid);
      send_to_char(buf2, ch);
    }
  /* Person doesn't have that item */
  } else if ((obj = get_obj_in_list_vis(ch, buf1, ch->carrying)) == NULL)
    send_to_char("You don't seem to have that to sell.\r\n", ch);
  /* Can not auction corpses because they may decompose */
  else if ((GET_OBJ_TYPE(obj) == ITEM_CONTAINER) && (GET_OBJ_VAL(obj, 3)))
     send_to_char("You can not auction corpses.\n\r", ch);
  else if (IS_OBJ_STAT(obj, ITEM_NODONATE))
    send_to_char("You can't auction that.\r\n", ch);
  /* It's valid */
  else {
    /* First bid attempt */
    auction.ticks = AUC_BID;
    /* This guy is selling it */
    auction.seller = GET_IDNUM(ch);
    /* Can not make the minimum less than 0 --KR */
    auction.bid = (atoi(buf2) > 0 ? atoi(buf2) : 1);
    /* Pointer to object */
    auction.obj = obj;
    /* Get the object from the character, so they cannot drop it! */
    obj_from_char(auction.obj);
    /* Prepare color message for those with it */
    sprintf(buf2, "\x1B[1;37m%s\x1B[35m &14puts \x1B[1;37m%s\x1B[35m &14up for sale, minimum bid \x1B[1;37m%ld\x1B[35m &14coin%s&11",
	get_ch_by_id(auction.seller)->player.name,
	auction.obj->short_description,
	auction.bid, auction.bid != 1 ? "s." : ".");
    /* Make a message sans-color for those whole have it off */
    sprintf(buf, "%s &14puts %s &14up for sale, minimum bid %ld &14coin%s&11",
	get_ch_by_id(auction.seller)->player.name,
	auction.obj->short_description,
	auction.bid, auction.bid != 1 ? "s&11." : ".");
    /* send out the messages */
    auction_output(buf2, buf);
      *buf2 = '\0';
      *buf1 = '\0';
       send_to_all("&15Auction Stats&16:&00");
       for (i = 0; i < MAX_OBJ_AFFECT; i++) {
         if (auction.obj->affected[i].modifier) {
           sprinttype(auction.obj->affected[i].location, apply_types, buf1);
           sprintf(buf2, "&00%s &11%+d &15to &14%s", found++ ? "," : "",
                   auction.obj->affected[i].modifier, buf1);
           send_to_all(buf2);
         }
       }
       send_to_all("\r\n");
  }
}

/*
 * auction_reset : returns the auction structure to a non-bidding state
 */
void auction_reset(void)
{
  auction.bidder = -1;
  auction.seller = -1;
  auction.obj = NULL;
  auction.ticks = AUC_NONE;
  auction.bid = 0;
}

/*
 * get_ch_by_id : given an ID number, searches every descriptor for a
 *		character with that number and returns a pointer to it.
 */
struct char_data *get_ch_by_id(long idnum)
{
  struct descriptor_data *d;

  for (d = descriptor_list; d; d = d->next)
    if (d && d->character && GET_IDNUM(d->character) == idnum)
      return (d->character);

  return NULL;
}
