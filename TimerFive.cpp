/*
 *  Interrupt and PWM utilities for 16 bit Timer5 on ATmega2560
 *  Modified from TimerOne project http://code.google.com/p/arduino-timerone/
 *
 *  This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  See GitHub project https://github.com/FireFlowController/TimerFive for latest version
 */
#ifndef TIMERFIVE_cpp
#define TIMERFIVE_cpp

#include "TimerFive.h"

TimerFive Timer5;              // preinstatiate

ISR(TIMER5_OVF_vect){          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
    Timer5.isrCallback();
}

void TimerFive::initialize(long microseconds){
  TCCR5A = 0;				//Clear control register A 
  TCCR5B = _BV(WGM53);		//Set mode 8: phase and frequency correct pwm, stop the timer
  DDRL |= _BV(PORTL3) | _BV(PORTL4) | _BV(PORTL5);	//Make Timer5 pins output
  setPeriod(microseconds);	//Set Timer period
}


void TimerFive::setPeriod(long microseconds){

  long cycles = (F_CPU / 2000000) * microseconds;								// the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  if(cycles < RESOLUTION)              clockSelectBits = _BV(CS50);				// no prescale, full xtal
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS51);				// prescale by /8
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS51) | _BV(CS50);	// prescale by /64
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS52);				// prescale by /256
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS52) | _BV(CS50);	// prescale by /1024
  else	cycles = RESOLUTION - 1, clockSelectBits = _BV(CS52) | _BV(CS50);		// request was out of bounds, set as maximum

  oldSREG = SREG;	
  cli();	// Disable interrupts for 16 bit register access 							
  ICR5 = pwmPeriod = cycles;	// ICR5 is TOP in pfc mode
  SREG = oldSREG;
}

void TimerFive::setPwmDuty(int duty){ //Set the same PWM Duty Cycle to all pins
  unsigned long dutyCycle = pwmPeriod;
  
  dutyCycle *= duty;
  dutyCycle >>= 10;
  
  oldSREG = SREG;
  cli();
  OCR5A = dutyCycle;
  OCR5B = dutyCycle;
  SREG = oldSREG;
}

void TimerFive::set_C_PwmDuty(int duty){
  unsigned long dutyCycle = pwmPeriod;
  
  dutyCycle *= duty;
  dutyCycle >>= 10;
  
  oldSREG = SREG;
  cli();
  OCR5C = dutyCycle;
  SREG = oldSREG;
}

void TimerFive::attachInterrupt(void (*isr)()){
  isrCallback = isr;	// register the user's callback with the real ISR
  TIFR5 = _BV(TOV5)| _BV(OCF5A);	//Write 1 to OV flag to avoid phantom interrupt
  TIMSK5 = _BV(TOIE5) | _BV(OCIE5A);	// sets the timer overflow interrupt enable bit				
}

void TimerFive::start(){
  TCCR5A |= (_BV(COM5A1)|_BV(COM5B1));//|_BV(COM5C1));	//Enable PWM outputs
  TCNT5 = 1;	//Clear Counter
  TCCR5B |= clockSelectBits;	//Start Timer5
}

void TimerFive::clear(){
  TCCR5B = _BV(WGM53)|_BV(WGM52);	//Set mode 12: CTC
  OCR5A = 0;
  OCR5B = 0;
  OCR5C = 0;
  TCCR5B = _BV(WGM53);	//Set mode 8: PFC
}

void TimerFive::stop(){
  TCCR5A &= ~(_BV(COM5A1)|_BV(COM5B1)|_BV(COM5C1));	//Disable All PWM outputs
  TCCR5B &= ~(_BV(CS50) | _BV(CS51) | _BV(CS52));	// clears all clock selects bits
  oldSREG = SREG;
  cli();
  TCNT5 = 0;	//Clear Counter without getting OV interrupt
  TIFR5 = _BV(TOV5);	//Write 1 to OV flag to avoid phantom interrupt
  SREG = oldSREG;
}

#endif