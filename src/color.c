#include "conf.h"

#define CNUL  ""

/*             Plain Colors            */
#define CNRM  "\x1B[0;0m"
/* %00 */
#define CRED  "\x1B[31m"
/* %01 */
#define CGRN  "\x1B[32m"
/* %02 */
#define CYEL  "\x1B[33m"
/* %03 */
#define CBLU  "\x1B[34m"
/* %04 */
#define CMAG  "\x1B[35m"
/* %05 */
#define CCYN  "\x1B[36m"
/* %06 */
#define CWHT  "\x1B[37m"
/* %07 */
#define CBLK  "\x1B[30m"
/* %08 */

/*              Bold Colors            */
#define BRED  "\x1B[1;31m"
/* %09 */
#define BGRN  "\x1B[1;32m"
/* %10 */
#define BYEL  "\x1B[1;33m"
/* %11 */
#define BBLU  "\x1B[1;34m"
/* %12 */
#define BMAG  "\x1B[1;35m"
/* %13 */
#define BCYN  "\x1B[1;36m"
/* %14 */
#define BWHT  "\x1B[1;37m"
/* %15 */
#define BBLK  "\x1B[1;30m"
/* %16 */

/*             Backgrounds             */
#define BKRED  "\x1B[41m"
/* %17 */
#define BKGRN  "\x1B[42m"
/* %18 */
#define BKYEL  "\x1B[43m"
/* %19 */
#define BKBLU  "\x1B[44m"
/* %20 */
#define BKMAG  "\x1B[45m"
/* %21 */
#define BKCYN  "\x1B[46m"
/* %22 */
#define BKWHT  "\x1B[47m"
/* %23 */
#define BKBLK  "\x1B[40m"
/* %24 */

/*         Underline, Flashing         */
#define UNDER  "\x1B[4m"
/* %25 */
#define FLASH  "\x1B[5m"
/* %26 */

#define CSIL  "\x1B[38m"
/* %15 */
#define BSIL  "\x1B[1;38m"
/* %15 */

const char *COLORLIST[] = {CNRM, CRED, CGRN, CYEL, CBLU, CMAG, CCYN, CWHT, CBLK
,



   BRED, BGRN, BYEL, BBLU, BMAG, BCYN, BWHT, BBLK,



   BKRED,BKGRN,BKYEL,BKBLU,BKMAG,BKCYN,BKWHT,BKBLK,



   UNDER,FLASH, CSIL, BSIL};
#define MAX_COLORS 28


int isnum(char i)
{
  return( (i >= '0') && (i <= '9') );
}

void proc_color(char *inbuf, int color)
{

register int j=0,p=0;

int c,k,max;

char out_buf[2008576];


if(inbuf[0] == '\0') return;


while(inbuf[j]!='\0')

{


if(  (inbuf[j]=='&') &&



isnum(inbuf[j+1]) && isnum(inbuf[j+2]) )


{



c=(inbuf[j+1]-'0')*10 + inbuf[j+2]-'0';



if(c>MAX_COLORS) c = 0;



max=strlen(COLORLIST[c]);



j+=3;



if(color)



   for(k=0;k<max;k++)



      { out_buf[p] = COLORLIST[c][k]; p++; }


 }


 else {out_buf[p] = inbuf[j]; j++;p++;}

}


out_buf[p] = '\0';


strcpy(inbuf, out_buf);
}

