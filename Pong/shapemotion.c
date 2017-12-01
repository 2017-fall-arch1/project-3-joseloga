/** \file shapemotion.c
 *  \brief This is a simple shape motion demo.
 *  This demo creates two layers containing shapes.
 *  One layer contains a rectangle and the other a circle.
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */  
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>
#include "buzzer.h"
#define GREEN_LED BIT6
static short p1 =0;
static short p2 =0;
static short temp=0;

static int button; /* 0 when SW1 is up */
static int button2;
static int button3;
static int button4;
   

AbRect rect10 = {abRectGetBounds, abRectCheck, {2,15}}; /**< 10x10 rectangle */
AbRect line = {abRectGetBounds, abRectCheck, {0,30}}; /**< 10x10 rectangle */

AbRArrow rightArrow = {abRArrowGetBounds, abRArrowCheck, 30};

AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2-1 , screenHeight/2-1 }
};




Layer line1 = {		/* playing field as a layer */
  (AbShape *) &line,
  {screenWidth/2, screenHeight/2},/**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
0,
    
};



Layer layer3 = {		/**< Layer with the right Paddle */
  (AbShape *)&rect10,
  {(screenWidth-4), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLUE,
  &line1,
};

Layer fieldLayer = {		/* playing field as a layer */
  (AbShape *) &fieldOutline,
  {screenWidth/2, screenHeight/2},/**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
  &layer3
};

Layer layer1 = {		/**< Layer with the left Paddle */
  (AbShape *)&rect10,
  {((screenWidth/8)-13), screenHeight/2}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &fieldLayer,
};

Layer layer11 = {		/**< Layer with the left Paddle */
  (AbShape *)&rect10,
  {((screenWidth/8)-10), 15}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &fieldLayer,
};



Layer layer00 = {		/**< Layer with an ball */
  (AbShape *)&circle3,
  {(screenWidth/2), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
  &layer1,
};


Layer layer0 = {		/**< Layer with an ball */
  (AbShape *)&circle3,
  {(screenWidth/2), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
  &layer1,
};





/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */
MovLayer ml3 = { &layer3, {0,3}, 0 };   //left button downward p2
MovLayer ml4 = { &layer3, {0,-3}, 0 };  //right button upward p2

MovLayer ml11 = { &layer11, {0,3}, 0 };  // left button downard p1
MovLayer ml2 = { &layer1, {0,-3}, 0 };  // right button upward p1
MovLayer ml1 = { &layer1, {0,2}, 0 };  // left button downard p1
MovLayer ml00 = { &layer0, {2,2}, 0};   // ball 

MovLayer ml0 = { &layer0, {2,2}, 0};   // ball 

void movLayerDraw(MovLayer *movLayers, Layer *layers)
{
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */


  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  



//Region fence = {{10,30}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}}; /**< Create a fence region */

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */
void mlAdvanceball(MovLayer *ml, Region *fence, MovLayer *Lpad,MovLayer *Rpad)
{
  Vec2 newPos;
  Vec2 newPosPad;
  Vec2 newPosPad2;
  
  u_char axis;
  Region shapeBoundary;
  Region Leftpad;
  Region Rightpad;

  
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    vec2Add(&newPosPad, &Lpad->layer->posNext, &Lpad->velocity);
    vec2Add(&newPosPad2, &Rpad->layer->posNext, &Rpad->velocity);
    
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    abShapeGetBounds(Lpad->layer->abShape, &newPosPad, &Leftpad);
    abShapeGetBounds(Rpad->layer->abShape, &newPosPad2, &Rightpad);
    
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]+3) ||
	  (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]-3    ) ) {
	
          int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (3*velocity);
          
    }
     
      // left pad                       0,1
       if ((shapeBoundary.topLeft.axes[0] < Leftpad.botRight.axes[0]+1) 
           && (shapeBoundary.botRight.axes[1]-5 < Leftpad.botRight.axes[1])  
            &&  (shapeBoundary.topLeft.axes[1]+9 > Leftpad.topLeft.axes[1])) {
	
          int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (3*velocity);
        
      ml->layer->color= COLOR_RED;
                     
//         drawString5x7(50,120, "GOAL!", COLOR_GREEN, COLOR_GREEN);
         buzzer_set_period(2000);
        __delay_cycles(160000);
    break;
       
            }
      // right pad 
      
         if ((shapeBoundary.botRight.axes[0] > Rightpad.topLeft.axes[0])
           && (shapeBoundary.botRight.axes[1]-5 < Rightpad.botRight.axes[1])  
            &&  (shapeBoundary.topLeft.axes[1]+8 > Rightpad.topLeft.axes[1])) {
	
                int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
                newPos.axes[axis] += (3*velocity);
        
              ml->layer->color= COLOR_BLUE;
                 
         drawString5x7(50,120, "GOAL!", COLOR_BLACK, COLOR_BLACK);

         buzzer_set_period(2000);
        __delay_cycles(160000);
        
        
             break;
       
            }
    
      
      if ((shapeBoundary.topLeft.axes[0] < fence->topLeft.axes[0]+3) ) {
	    p2++; 
        
       buzzer_set_period(6000);
        __delay_cycles(160000);
        drawString5x7(50,120, "GOAL!", COLOR_BLUE, COLOR_BLACK);
   
        break;
    }
    
      if ((shapeBoundary.botRight.axes[0] > fence->botRight.axes[0]-3) ) {
	    p1++;
        
        buzzer_set_period(6000);
        __delay_cycles(160000);
        drawString5x7(50,120, "GOAL!", COLOR_RED, COLOR_BLACK);
       
      
     break;
          
    }
       
       
        
    } 
 
     ml->layer->posNext = newPos;
    // Lpad->layer->posNext = newPosPad;
     
  } /**< for ml */
}



void mlAdvance2(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis])){ 
          
    newPos.axes[axis] += 3;      
    }    
	  if(shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis])  {
        
    newPos.axes[axis] -= 3;
      }	/**< if outside of fence */
    } /**< for axis */
    ml->layer->posNext = newPos;
  }/**< for ml */
}


u_int bgColor = COLOR_BLACK;     /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */

Region fieldFence;		/**< fence around playing field  */


/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */


void score_board(){
    if(p1 == 10) {
        p1=0;
           
    drawString5x7(45,25, "PLAYER1", COLOR_RED, COLOR_BLACK);
    drawString5x7(45,35, "  WIN", COLOR_WHITE, COLOR_BLACK);
    buzzer_set_period(0);
    __delay_cycles(16000000);
    
    WDTCTL=0;
        
        
    } 
    if(p2 == 10) {
        p2=0;
        
    drawString5x7(45,25, "PLAYER2", COLOR_BLUE, COLOR_BLACK);
    drawString5x7(45,35, "  WIN", COLOR_WHITE, COLOR_BLACK);
    __delay_cycles(16000000);
     WDTCTL=0;
        
        
    } 
       
    drawString5x7(50,2, "SCORE", COLOR_WHITE, COLOR_BLACK);
     drawString5x7(25,2, "P1", COLOR_RED, COLOR_BLACK);
      drawString5x7(90,2, "P2", COLOR_BLUE, COLOR_BLACK);
    
    drawChar(ml3.layer->pos.axes[0]-2,ml3.layer->pos.axes[1]-2, p2, COLOR_WHITE, COLOR_BLUE);
    drawChar(ml1.layer->pos.axes[0]-2, ml1.layer->pos.axes[1]-2,p1, COLOR_WHITE, COLOR_RED);
}



void main()
{
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  shapeInit();
  p2sw_init(1);
  buzzer_init();
  
  shapeInit();

  layerInit(&layer0);
  layerDraw(&layer0);

 
 layerGetBounds(&fieldLayer, &fieldFence);


  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */

  
  for(;;) { 
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10);	      /**< CPU OFF */
    }
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    
    redrawScreen = 0;
    movLayerDraw(&ml0, &layer0);
 
     button = (P2IN & BIT0 ) ?  0 : 1; /* 0 when SW1 is up */
     button2 = (P2IN & BIT1 ) ?  0 : 1;
     button3 = (P2IN & BIT2 ) ?  0 : 1;
     button4 = (P2IN & BIT3 ) ?  0 : 1;
    
  if(button){
       movLayerDraw(&ml1, &layer1);
        score_board();
       mlAdvance2(&ml1, &fieldFence);
  }
   if(button2){
       movLayerDraw(&ml2, &layer1);
        score_board();
       mlAdvance2(&ml2, &fieldFence);
    
       
}
   if(button3){
       movLayerDraw(&ml3, &layer3);
        score_board();
       mlAdvance2(&ml3, &fieldFence);
  }
   if(button4){
       movLayerDraw(&ml4, &layer3);
        score_board();
       mlAdvance2(&ml4, &fieldFence);
  }
    if(button2 && button3 ){
        ml0.velocity.axes[0]=2;
        ml0.velocity.axes[1]=2;

        
  }
  
  }       
}
 

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  if (count == 5) {
    mlAdvanceball(&ml0, &fieldFence,&ml1,&ml3);
   
    score_board();
    buzzer_set_period(0); // turn off 
        redrawScreen = 1;
     
    count = 0;
  } 
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}

