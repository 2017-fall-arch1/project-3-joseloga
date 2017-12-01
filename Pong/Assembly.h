#ifndef shapemotion_included
#define shapemotion_included

#include "msp430.h"


extern  short p1 ;
extern  short p2 ;

extern  short p1Score;
extern  short p2Score;

 int button, button2, button3, button4;
   
void score_point(short p1Score, short p2Score);
#endif // included
 
