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
#ifndef TIMERFIVE_h
#define TIMERFIVE_h

#include <avr/io.h>
#include <avr/interrupt.h>

#define RESOLUTION 65536    // Timer5 is 16 bit

class TimerFive
{
	public:
  
	// properties
		unsigned int pwmPeriod;
		unsigned char clockSelectBits;
		char oldSREG;					// To hold Status Register while ints disabled

	// methods
		void initialize(long microseconds=1000000);
		void attachInterrupt(void (*isr)());
		void start();
		void stop();
		void clear();

		//void setPwmDuty(char pin, int duty);
		void setPwmDuty(int duty);	//Set the same PWM Duty Cycle to A-B pins
		void set_C_PwmDuty(int duty);	//Set the same PWM Duty Cycle to C pin
		void (*isrCallback)();
	
	private:
		void setPeriod(long microseconds);

  
};

extern TimerFive Timer5;
#endif