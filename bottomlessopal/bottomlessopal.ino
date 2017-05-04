/** 
 * Copyright (c) 2016 FirstBuild
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal 
 * in the Software without restriction, including without limitation the rights 
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <FiniteStateMachine.h>

#define MAX_FILL_TIME 180000

#define BOT_OPAL_VERSION_MAJOR 0
#define BOT_OPAL_VERSION_MINOR 2
#define BOT_OPAL_VERSION_PATCH 1

extern void stateFillingEnter();
extern void stateFillingUpdate();
extern void stateFillingExit();
extern void stateStandbyEnter();
extern void stateStandbyUpdate();
extern void stateStandbyExit();

//state machine setup 
State stateFilling = State(stateFillingEnter, stateFillingUpdate, stateFillingExit);
State stateStandby = State(stateStandbyEnter, stateStandbyUpdate, stateStandbyExit);
FSM boStateMachine = FSM(stateStandby);     //initialize state machine, start in state: On
//SimpleTimer timerFillExpire;
//int tickEvent;

static const uint8_t FULLREED = 1;
static const uint8_t RELAYOUT = 2;
static const uint8_t EMPTYREED  = 4;
static const uint8_t NUMBER_OF_STATES = 2; //how many states are we cycling through?
static const uint8_t PITCHER_EMPTY  = LOW;
static const uint8_t PITCHER_FULL = LOW;
static const uint16_t WAIT_TIME_BETWEEEN_FILLS = 5000;

unsigned long start_filling_millis = 0;

void setup() {
  //Serial.begin(9600); 
  pinMode(FULLREED, INPUT_PULLUP);
  pinMode(EMPTYREED, INPUT_PULLUP);
  pinMode(RELAYOUT, OUTPUT);
  boStateMachine.transitionTo(stateStandby);
}

void loop(){
  boStateMachine.update();
}

//------------------------------------------
// FILLING
//------------------------------------------
void stateFillingEnter()
{ 
  delay(WAIT_TIME_BETWEEEN_FILLS);
  digitalWrite(RELAYOUT, HIGH);
  start_filling_millis = millis();
  //timerFillExpire.restartTimer(tickEvent);
  //tickEvent = timerFillExpire.after(MAX_FILL_TIME, fillTimerExpire); 
  //tickEvent = timerFillExpire.setTimeout(90 * 1000, fillTimerExpire);
}

//void fillTimerExpire()
//{
//  boStateMachine.transitionTo(stateComplete);
//}

void stateFillingUpdate()
{ 
  unsigned long current_millis = millis();

  if (PITCHER_FULL == digitalRead(FULLREED))
  {
    boStateMachine.immediateTransitionTo(stateStandby);   
  }
  else if (current_millis - start_filling_millis > MAX_FILL_TIME)
  {
    boStateMachine.immediateTransitionTo(stateStandby); 
  }
}

void stateFillingExit()
{ 
  digitalWrite(RELAYOUT, LOW);
  //timerFillExpire.deleteTimer(tickEvent);
}

//------------------------------------------
// STANDBY
//------------------------------------------
void stateStandbyEnter()
{ 
}

void stateStandbyUpdate()
{ 
  //when pitcher returns go to the fill state
  if (PITCHER_EMPTY == digitalRead(EMPTYREED))
  {
    boStateMachine.transitionTo(stateFilling);
  }
}  

void stateStandbyExit()
{ 
}
