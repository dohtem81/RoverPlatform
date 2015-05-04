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

#include "MotorDriverMC33926.h"
#include "BlackPWM.h"
#include "BlackGPIO.h"
#include "BlackADC.h"

// instance counter
int MotorDriverMC33926::instanceCounter;
BlackLib::BlackGPIO *MotorDriverMC33926::enable;


// class constructor
// ----------------------------------------------------------------------------------------------------
MotorDriverMC33926::MotorDriverMC33926(MC3392Config *config, std::string name){
	// set up name
	if (name.length() > 0)
		this->myName = name ;
	else {
		this->myName = "MD_MC33926#" + std::to_string (MotorDriverMC33926::instanceCounter++);
	}

	/*
	// set up all (file) names
	this->in2FilePath			=	config->in2FilePath;
	this->in1FilePath			=	config->in1FilePath;
	this->inPWMD2				=	config->inPWMD2;
	this->inPWMD1				=	config->inPWMD1;
	this->out2FilePath			=	config->out2FilePath;
	this->out1FilePath			=	config->out1FilePath;
	this->StatusFlagFilePath	=	config->StatusFlagFilePath;
	this->FeedbackFilePath		=	config->FeedbackFilePath;
	*/

	//initialize PWMs
	PWM1 = new BlackLib::BlackPWM(config->inPWM1) ;

	// preset values for PWMs
	PWM1->setDutyPercent(0.0);
	PWM1->setPeriodTime(5, BlackLib::milisecond);

	// set up GPIOs
	// hardware inputs
	//d1 = new BlackLib::BlackGPIO(	config->d1,
	//							BlackLib::input);
	in2 = new BlackLib::BlackGPIO(	config->in2,
								BlackLib::output);
	in1 = new BlackLib::BlackGPIO(	config->in1,
								BlackLib::output);

	// hardware outputs
	//out2 = new BlackLib::BlackGPIO(config->out2,
	//							BlackLib::input,
	//							BlackLib::SecureMode);
	//out1 = new BlackLib::BlackGPIO(config->out1,
	//							BlackLib::input,
	//							BlackLib::SecureMode);
	statusFlag = new BlackLib::BlackGPIO(	config->statusFlag,
										BlackLib::input,
										BlackLib::FastMode);
	//*currentFeedbackRaw;

}


// method setting speed reference
// ----------------------------------------------------------------------------------------------------
void MotorDriverMC33926::setSpeedReference(int *speedRef){

	// add null bias depending on direction
	if (*speedRef > 0)
		*speedRef += this->forwardNullbias ;
	else
		*speedRef += this->reverseNullbias ;

	// this reference is in %
	// if reference is outside [-100 - 100] range, clamp it
	if (*speedRef < MIN_SPEED_REF)
		*speedRef = MIN_SPEED_REF;
	else if (*speedRef > MAX_SPEED_REF)
		*speedRef = MAX_SPEED_REF ;

	// now transfer speedRef to class member variable
	//this->ref_speed = (float)*speedRef/100 * this->max_speed ;
	this->ref_speed = *speedRef ;

	// call for update outputs
	this->updateOutputs();

}


// updating outputs
// ----------------------------------------------------------------------------------------------------
void MotorDriverMC33926::updateOutputs(){

	// update speed reference section
	// ------------------------------
	// figure out direction
	// if speed ref is 0 don't change direction bit
	if (this->ref_speed > 0)
		this->ref_direction = DIR_FORWARD ;
	else if (this->ref_speed < 0)
		this->ref_direction = DIR_REVERSE ;

	// sets direction in1, in2 pins
	float reference ;
	if (this->ref_direction == DIR_FORWARD){
		this->in1->setValue(BlackLib::high) ;
		this->in2->setValue(BlackLib::low) ;
		reference = this->ref_speed ;
	} else {
		this->in1->setValue(BlackLib::low) ;
		this->in2->setValue(BlackLib::high) ;
		reference = 0 - this->ref_speed ;
	}

	// now write to PWM and d1 pin
	//this->d1->setValue(BlackLib::low);
	this->PWM1->setDutyPercent(reference);
	this->PWM1->setPeriodTime(5, BlackLib::milisecond);

	// outputs update, return
	return ;

}


// returns object name
// ----------------------------------------------------------------------------------------------------
std::string MotorDriverMC33926::getMyName() {
	return this->myName ;
}

// sets selected null bias value
// ----------------------------------------------------------------------------------------------------
void MotorDriverMC33926::setNullbias(nullbiasSelector selector, int* value){
	if (selector == forwardNullbiasDir){
		this->forwardNullbias = *value ;
		return ;
	} else {
		this->reverseNullbias = *value ;
		return ;
	}
}


// gets speed reference for selected motor
// ----------------------------------------------------------------------------------------------------
int MotorDriverMC33926::getSpeedReference() {
	return (int)this->ref_speed ;
}

// ============================================================================================================================================
// class definition
// ============================================================================================================================================
DualMC33926::DualMC33926(MC3392Config *config1, MC3392Config *config2, BlackLib::gpioName enablePin){
	motor1Driver = new MotorDriverMC33926(config1);
	motor2Driver = new MotorDriverMC33926(config2);

	enabled = false ;
	this->enablePin = new BlackLib::BlackGPIO(enablePin, BlackLib::output);
	this->enablePin->setValue(BlackLib::low);
}


MotorDriverMC33926* DualMC33926::getMotorPointer(motorSelector sel) {
	switch (sel){
	case leftMotor:
		return motor1Driver ;
		break ;
	case rightMotor:
		return motor2Driver ;
		break ;
	}

	return NULL ;
}


// sets speed ref indicated by motorSelector parameter
// ----------------------------------------------------------------------------------------------------
void DualMC33926::setSpeedRef(motorSelector sel, int *speedRef){
	getMotorPointer(sel)->setSpeedReference(speedRef);

	// set enable pin
	if (this->enabled)
		this->enablePin->setValue(BlackLib::low);
	else
		this->enablePin->setValue(BlackLib::high);

}


// sets enable bit
// ----------------------------------------------------------------------------------------------------
void DualMC33926::setEnableDrive(){
	this->enablePin->setValue(BlackLib::high);
}

// unsets enable bit
// ----------------------------------------------------------------------------------------------------
void DualMC33926::setDisableDrive(){
	this->enablePin->setValue(BlackLib::low);
}

// sets nullbias indicated by motorSelector parameter and direction
// ----------------------------------------------------------------------------------------------------
void DualMC33926::setNullbias(motorSelector motorSel, nullbiasSelector nbSel, int *nullbias){
	getMotorPointer(motorSel)->setNullbias(nbSel, nullbias);
}

// gets speed reference for selected motor
// ----------------------------------------------------------------------------------------------------
int DualMC33926::getSpeedReference(motorSelector sel) {
	return getMotorPointer(sel)->getSpeedReference() ;
}

// return status of drive enable bit
// ----------------------------------------------------------------------------------------------------
bool DualMC33926::isEnabledDrive(){
	//return (this->enablePin->getValue() == "1");
	return true ;
}
