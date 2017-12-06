#ifndef shapemotion_included
#define shapemotion_included

#include "msp430.h"
#include "buzzer.h"
#include <libTimer.h>

extern  short p1, p2;
extern  short p1Score,p2Score;
int button, button2, button3, button4;
extern int d,e,f;

void score_point(short p1Score, short p2Score);
void scorepoint(short p1Score, short p2Score);
void beat();
void beat2();
void beat3();
void song();
#endif // included
 
