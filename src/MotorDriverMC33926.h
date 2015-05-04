/*
 *   This file is part of RoverPlatform.
 *
 *   RoverPlatform is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   any later version.
 *
 *   RoverPlatform is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with RoverPlatform.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   For any comment or suggestion please contact the author of that software
 *   at pedziwiatr.piotr@gmail.com
 */

#ifndef MOTORDRIVERMC33926_H_
#define MOTORDRIVERMC33926_H_

#include <string>

#include "BlackPWM.h"
#include "BlackGPIO.h"
#include "BlackADC.h"

// constant parameters
// -------------------------------------------------------------------------------------------------------------------------------------------
const float					MAX_SPEED_REF			=  70.0 ;
const float					MIN_SPEED_REF			= -70.0 ;
const int					DIR_FORWARD				= 1 ;
const int					DIR_REVERSE				= 0 ;

// enumerations
// -------------------------------------------------------------------------------------------------------------------------------------------
enum motorSelector { 	leftMotor = 0,
						rightMotor = 1 };

enum nullbiasSelector {	forwardNullbiasDir = 0,
						reverseNullbiasDir = 1 };


// this structure used for MC unit configuration
// -------------------------------------------------------------------------------------------------------------------------------------------
struct _MC33926Config {

	// inputs
	BlackLib::gpioName		in2;
	BlackLib::gpioName		in1;
	BlackLib::gpioName		d1;
	BlackLib::pwmName		inPWM1;

	//outputs
	BlackLib::gpioName		out2;
	BlackLib::gpioName		out1;
	BlackLib::gpioName		statusFlag;
	BlackLib::adcName		feedback;

} typedef MC3392Config;

/*
// converts string PWM name defined in BlackDef.h into enum value
// inputs are:
//			pwmMapName is a string specifying which PWM to use (refer to BlackDef.h)
//			pwmName	   is a integer value paired with enumeration type (refer to BlackDef.h)
// return result - -1 failed, 0 -success
int inline pwmMapNameToName(std::string* pwmMapName, BlackLib::pwmName *pwmName){
	// loop through map and compare pwmMapName
	for (int i=0; i<7; i++)
		if (pwmMapName->compare(BlackLib::pwmNameMap[i]) == 0 ) {
			*pwmName = (BlackLib::pwmName)i ;
			return 0;
		}

	return -1 ;
}
*/


// ============================================================================================================================================
// class declaration
// ============================================================================================================================================
class MotorDriverMC33926 {
public :
	// inputs:
	// --------
	// IN2 			HIGH 	The logic input control of OUT2. PWM can be applied to this pin (typically done with both disable lines inactive).
	// IN1 			HIGH 	The logic input control of OUT1. PWM can be applied to this pin (typically done with both disable lines inactive).
	// PWM / !D2 	LOW 	Inverted disable input: when D2 is low, OUT1 and OUT2 are set to high impedance. A D2 PWM duty cycle of
	//                      70% gives a motor duty cycle of 70%. Typically, only one of the two disable pins is used, but the default is for
	//						both disable pins to be active.
	// !PWM / D1 	HIGH 	Disable input: when D1 is high, OUT1 and OUT2 are set to high impedance. A D1 PWM duty cycle of 70% gives a motor
	//						duty cycle of 30%. Typically, only one of the two disable pins is used, but the default is for both disable pins
	//						to be active.
	//
	// outputs:
	// --------
	// OUT2 		HIGH 	The motor output pin controlled by IN2.
	// OUT1 		HIGH 	The motor output pin controlled by IN1.
	// !SF 			HIGH 	Status flag output: an over-current (short circuit) or over-temperature event will cause SF to be latched LOW.
	//						If either of the disable pins (D1 or D2) are disabling the outputs, SF will also be LOW. Otherwise, this pin is weakly
	//						pulled high. This allows the two SF pins on the board to be tied together and connected to a single MCU input.
	// FB			LOW 	The FB output provides analog current-sense feedback of approximately 525 mV per amp.


	// constructor
	MotorDriverMC33926(MC3392Config*, std::string name = "") ;

	// identity related
	std::string getMyName();

	// speed reference
	void setSpeedReference(int*);		// sets speed ref in % (0-100)
	int getSpeedReference();			// gets speed ref in % (0-100)
	void getSpeedReference(float*);
	//float getSpeedReference();
	void setNullbias(nullbiasSelector, int*) ;


private :
	// instance counter declaration
	static int						instanceCounter ;
	static BlackLib::BlackGPIO		*enable;

	// object name
	std::string				myName ;

	// inputs
	std::string				in2FilePath;
	std::string				in1FilePath;
	std::string				inPWMD2;
	std::string				inPWMD1;

	//outputs
	std::string				out2FilePath;
	std::string				out1FilePath;
	std::string				StatusFlagFilePath;
	std::string				FeedbackFilePath;

	// constants
	const float				max_speed = MAX_SPEED_REF ;
	const float				min_speed = MIN_SPEED_REF ;

	// several parameters are controlled like speed, direction by inputs
	float					ref_speed ;
	int						ref_direction ;
	bool					status_OverCurrent;
	bool					status_OverTemperature;
	int                     forwardNullbias ;
	int                     reverseNullbias ;

	// values from files
	long					hw_duty;
	long					hw_period;

	// hardware inputs
	BlackLib::BlackPWM		*PWM1;
	BlackLib::BlackGPIO		*in2;
	BlackLib::BlackGPIO		*in1;

	// hardware outputs
	BlackLib::BlackGPIO		*d1;
	BlackLib::BlackGPIO		*out2;
	BlackLib::BlackGPIO		*out1;
	BlackLib::BlackGPIO		*statusFlag;
	BlackLib::BlackADC		*currentFeedbackRaw;

	void updateOutputs();

};


// ============================================================================================================================================
// class declaration
// ============================================================================================================================================
class DualMC33926 {
public:
	DualMC33926(MC3392Config*, MC3392Config*, BlackLib::gpioName) ;
	void setSpeedRef(motorSelector, int*);
	void setEnableDrive();
	void setDisableDrive();
	bool isEnabledDrive();
	void setNullbias(motorSelector, nullbiasSelector, int*);
	int  getSpeedReference(motorSelector);

private:
	MotorDriverMC33926			*motor1Driver ;
	MotorDriverMC33926			*motor2Driver ;
	BlackLib::BlackGPIO			*enablePin ;
	bool						enabled ;

	MotorDriverMC33926* getMotorPointer(motorSelector);
};

#endif /* MOTORDRIVERMC33926_H_ */

