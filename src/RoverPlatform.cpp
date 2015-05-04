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

#include <iostream>
#include <string>
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include "inotify-cxx.h"
#include "RoverPlatform.hpp"
#include "exampleAndTiming/Timing.h"
#include "MotorDriverMC33926.h"
#include "Encoder.h"
#include "BlackADC.h"
#include "udp_client_server.h"
#include <arpa/inet.h>
#include <cstring>
#include "SpeedControl.h"
#include "ControlNetworkRobot.h"

using namespace std;


// =========================================================================
// main function
// =========================================================================
int main() {

	// create rover object
	new Rover();
	while(1) {};

	/*
	int indexer = 0 ;

	// start main loop
	testIP = false ;
	while(testIP){
		speedRefLeft += speedDelta;
		if (speedRefLeft > 100) {
			speedRefLeft=100;
			speedDelta = -1 ;
			cout << "changing direction" << endl ;
		} else if (speedRefLeft < -100 )
			testIP = false ;
		speedRefRight = speedRefLeft ;
		drive->setSpeedRef(leftMotor, &speedRefLeft);
		drive->setSpeedRef(rightMotor, &speedRefRight);

		struct timespec tim;
		tim.tv_sec = 0;
		tim.tv_nsec = 50000000;
		nanosleep(&tim, NULL);
		indexer++ ;
	}
	*/

	/*
	// calculate rotation
	// robot width
	float wheelDiam = 58 ;
	float mmPerCnt = ( wheelDiam * 3.1415 * 3 ) / 1000 ;
	float platformWidth = 215 ;
	// we want to make 360deg rotation
	float distToTravel = platformWidth * 3.1415 ;
	long countsToTravel = distToTravel / mmPerCnt ;
	long posRef ;
	float kP = 0.5 ;
	int speed ;
	int counter = 0 ;

	bool driveTestIP = true ;
	counter = 0 ;
	distToTravel = 500 ;
	posRef = encoderList.begin()->getCountsRaw() + ( distToTravel / mmPerCnt ) ;
	while ( (driveTestIP) && ( counter < 200 ) ) {
		std::vector<Encoder>::iterator buffer = encoderList.begin() ;
		rawCountLF = buffer->getCountsRaw() ;
		speed = (int)(( posRef - rawCountLF ) * kP ) ;
		//speed = (int)( posRef - rawCountLF ) ;
		if ((( posRef - rawCountLF ) < 20 ) && (( posRef - rawCountLF ) > -20))
			driveTestIP = false ;

		// now we need to scale speedRef to fit within 100% and null bias
		speedRefLeft = speedRefRight = speed ;
		// speedRefRight = speed ;

		drive->setSpeedRef(leftMotor, &speedRefLeft);
		drive->setSpeedRef(rightMotor, &speedRefRight);

		//rotationIP = false ;
		counter++ ;

		struct timespec tim;
		tim.tv_sec = 0;
		tim.tv_nsec = 50000000;
		nanosleep(&tim, NULL);
	}
	*/

	/*
	bool rotationIP = true ;
	// rotation test
	buffer = encoderList.begin() ;
	rawCountLFPrev = buffer++->getCountsRaw() ;
	rawCountLBPrev = buffer++->getCountsRaw() ;
	rawCountRFPrev = buffer++->getCountsRaw() ;
	rawCountRBPrev = buffer->getCountsRaw() ;
	int posRefLeft = rawCountLFPrev +  countsToTravel ;
	int posRefRight = rawCountRFPrev - countsToTravel ;

	counter = 0 ;
	while ( (rotationIP) && ( counter < 200 ) ) {
		std::vector<Encoder>::iterator buffer = encoderList.begin() ;
		rawCountLF = buffer++->getCountsRaw() ;
		buffer++ ;
		rawCountRF = buffer++->getCountsRaw() ;

		speed = (int)(( posRefLeft - rawCountLF ) * ( kP * 5 ) ) ;
		speedRefLeft = speed ;

		speed = (int)(( posRefRight - rawCountRF ) * ( kP * 5 ) ) ;
		speedRefRight = speed ;

		drive->setSpeedRef(leftMotor, &speedRefLeft);
		drive->setSpeedRef(rightMotor, &speedRefRight);

		//cout << speedRefLeft << "\t" << speedRefRight << endl ;
		cout << "motor V: " << ( motorVBatt.getNumericValue() * 0.0057 ) << "[V]\t" ;
		cout << "motor1A: " << ( motor1Curr.getNumericValue() * 5700 / 525) << "[mA]\t" ;
		cout << "motor2A: " << ( motor2Curr.getNumericValue() * 5700 / 525) << "[mA]\t" ;
		cout << endl ;

		if ((( posRefLeft - rawCountLF ) < 20 )&&(( posRefLeft - rawCountLF )>-20)&&(( posRefRight - rawCountRF ) < 20 )&&(( posRefRight - rawCountRF )>-20))
			rotationIP = false ;

		//rotationIP = false ;
		counter++ ;

		struct timespec tim;
		tim.tv_sec = 0;
		tim.tv_nsec = 50000000;
		nanosleep(&tim, NULL);
	}
	*/


	/*
	driveTestIP = true ;
	counter = 0 ;
	distToTravel = -500 ;
	posRef = encoderList.begin()->getCountsRaw() + ( distToTravel / mmPerCnt ) ;
	while ( (driveTestIP) && ( counter < 200 ) ) {
		std::vector<Encoder>::iterator buffer = encoderList.begin() ;
		rawCountLF = buffer->getCountsRaw() ;
		speed = (int)(( posRef - rawCountLF ) * kP ) ;

		if ((( posRef - rawCountLF ) < 20 ) && (( posRef - rawCountLF ) > -20))
			driveTestIP = false ;

		// now we need to scale speedRef to fit within 100% and null bias
		speedRefLeft = speedRefRight = speed ;
		//speedRefRight = speed ;

		drive->setSpeedRef(leftMotor, &speedRefLeft);
		drive->setSpeedRef(rightMotor, &speedRefRight);

		counter++ ;

		struct timespec tim;
		tim.tv_sec = 0;
		tim.tv_nsec = 50000000;
		nanosleep(&tim, NULL);
	}
	*/

	/*
	for (std::vector<Encoder>::iterator buffer = encoderList.begin(); buffer != encoderList.end(); buffer++) {
		cout << buffer->getCountsRaw() << "\t" ;
	}
	cout << endl ;

	speedRefLeft  = 0 ;
	speedRefRight = 0 ;
	drive->setSpeedRef(leftMotor, &speedRefLeft);
	drive->setSpeedRef(rightMotor, &speedRefRight);

	cout << "motor V: " << ( motorVBatt.getNumericValue() * 0.0057 ) << "[V]\t" ;
	cout << "motor1A: " << ( motor1Curr.getNumericValue() * 5700 / 525) << "[mA]\t" ;
	cout << "motor2A: " << ( motor2Curr.getNumericValue() * 5700 / 525) << "[mA]\t" ;
	cout << endl ;
	*/
	cout << "exiting..." << endl ;
	//return 0;
}
