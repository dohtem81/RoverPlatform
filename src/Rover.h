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

#ifndef ROVER_H_
#define ROVER_H_

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
#include "MotorDriverMC33926.h"

// let compiler know ControlNetworkRobot will be delcared
class ControlNetworkRobot ;

class Rover {
public:
	Rover();
	virtual ~Rover();

	// threads
	static void * nullBiasCalibration(void *) ;
	static void * driveTest(void *) ;

	// public fields
	MC3392Config 				*motor1DriverConfig ;
	MC3392Config 				*motor2DriverConfig ;
	DualMC33926 				*drive;
	BlackLib::BlackADC 			*motorVBatt ;
	BlackLib::BlackADC 			*motor1Curr ;
	BlackLib::BlackADC 			*motor2Curr ;
	std::vector<Encoder>		encoderList ;
	ControlNetworkRobot         *network ;

private:
	bool 						calibrationInProgress ;
	bool                        calibrationDone ;
	bool 						testDriveInProgress ;
	bool                        testDriveDone ;

	bool						useThreads = false ;
	bool                        testsAtStartup = true ;

};

#endif /* ROVER_H_ */
