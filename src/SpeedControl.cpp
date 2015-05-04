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

#include "SpeedControl.h"
#include "Encoder.h"
#include "MotorDriverMC33926.h"
#include <iostream>

using namespace std ;

SpeedControl::SpeedControl(DualMC33926 *drive, vector<Encoder>* encoderList) {
	// TODO Auto-generated constructor stub

	// things need to be done here:
	//	- check for calibration data
	//		1. if exists and not forced skip calibration
	//		2. if force do to calibration or calibration data does not exist perform
	//			dead band test
	//	- start control loop

	this->drive = drive ;
	this->encoderList = encoderList ;

	// perform calibration
	this->calibrateFunction();

}

SpeedControl::~SpeedControl() {
	// TODO Auto-generated destructor stub
}

bool SpeedControl::calibrateFunction(){

	// some general stuff for speed control
	int speedRefLeft = 0 ;
	int speedRefRight = 0 ;
	int speedDelta = 1 ;

	// find dead bands for both motors
	bool calibrationIP = true ;

	bool allFWDCalibrated        = false ;
	bool FWDleftFrontCalibrated  = false ;
	bool FWDrightFrontCalibrated = false ;
	bool FWDleftBackCalibrated   = false ;
	bool FWDrightBackCalibrated  = false ;
	bool allBWDCalibrated        = false ;
	bool BWDleftFrontCalibrated  = false ;
	bool BWDrightFrontCalibrated = false ;
	bool BWDleftBackCalibrated   = false ;
	bool BWDrightBackCalibrated  = false ;

	int forwardLeftDeadband  = 0 ;
	int forwardRightDeadband = 0 ;
	int reverseLeftDeadband  = 0 ;
	int reverseRightDeadband = 0 ;

	long rawCountLF, rawCountRF, rawCountLB, rawCountRB ;
	long rawCountLFPrev, rawCountRFPrev, rawCountLBPrev, rawCountRBPrev ;

	std::vector<Encoder>::iterator buffer = encoderList->begin() ;
	rawCountLFPrev = buffer++->getCountsRaw() ;
	rawCountLBPrev = buffer++->getCountsRaw() ;
	rawCountRFPrev = buffer++->getCountsRaw() ;
	rawCountRBPrev = buffer->getCountsRaw() ;

	while (calibrationIP){

		// get all encoder feedbacks
		std::vector<Encoder>::iterator buffer = encoderList->begin() ;
		rawCountLF = buffer++->getCountsRaw() ;
		rawCountLB = buffer++->getCountsRaw() ;
		rawCountRF = buffer++->getCountsRaw() ;
		rawCountRB = buffer->getCountsRaw() ;

		// first check forward dead band
		if (!allFWDCalibrated) {
			// check if there was a change in pos fbk
			if ((rawCountLF > rawCountLFPrev) && !FWDleftFrontCalibrated ) {
				FWDleftFrontCalibrated = true ;
				cout << "snapshot for LF>> @" << speedRefLeft << endl ;
				if ( speedRefLeft > forwardLeftDeadband)
					forwardLeftDeadband = speedRefLeft ;
			}
			if ((rawCountLB > rawCountLBPrev) && !FWDleftBackCalibrated ) {
				FWDleftBackCalibrated = true ;
				cout << "snapshot for LB>> @" << speedRefLeft << endl ;
				if ( speedRefLeft > forwardLeftDeadband)
					forwardLeftDeadband = speedRefLeft ;
			}
			if ((rawCountRF > rawCountRFPrev) && !FWDrightFrontCalibrated ) {
				FWDrightFrontCalibrated = true ;
				cout << "snapshot for RF>> @" << speedRefRight << endl ;
				if ( speedRefRight > forwardRightDeadband)
					forwardRightDeadband = speedRefRight ;
			}
			if ((rawCountRB > rawCountRBPrev) && !FWDrightBackCalibrated ) {
				FWDrightBackCalibrated = true ;
				cout << "snapshot for RB>> @" << speedRefRight << endl ;
				if ( speedRefRight > forwardRightDeadband)
					forwardRightDeadband = speedRefRight ;
			}

			speedDelta = 1 ;
			speedRefLeft += speedDelta ;
			speedRefRight += speedDelta ;

			if (FWDleftFrontCalibrated && FWDleftBackCalibrated)
				speedRefLeft = 0 ;

			if (FWDrightFrontCalibrated && FWDrightBackCalibrated)
				speedRefRight = 0 ;

		}

		if ((!allBWDCalibrated) && allFWDCalibrated) {
			if ((rawCountLF < rawCountLFPrev) && !BWDleftFrontCalibrated ) {
				BWDleftFrontCalibrated = true ;
				cout << "snapshot for LF<< @" << speedRefLeft << endl ;
				if ( speedRefLeft < reverseLeftDeadband)
					reverseLeftDeadband = speedRefLeft ;
			}
			if ((rawCountLB < rawCountLBPrev) && !BWDleftBackCalibrated ) {
				BWDleftBackCalibrated = true ;
				cout << "snapshot for LB<< @" << speedRefLeft << endl ;
				if ( speedRefLeft < reverseLeftDeadband)
					reverseLeftDeadband = speedRefLeft ;
			}
			if ((rawCountRF < rawCountRFPrev) && !BWDrightFrontCalibrated ) {
				BWDrightFrontCalibrated = true ;
				cout << "snapshot for RF<< @" << speedRefRight << endl ;
				if ( speedRefRight < reverseRightDeadband)
					reverseRightDeadband = speedRefRight ;
			}
			if ((rawCountRB < rawCountRBPrev) && !BWDrightBackCalibrated ) {
				BWDrightBackCalibrated = true ;
				cout << "snapshot for RB<< @" << speedRefRight << endl ;
				if ( speedRefRight < reverseRightDeadband)
					reverseRightDeadband = speedRefRight ;
			}

			speedDelta = -1 ;
			speedRefLeft += speedDelta ;
			speedRefRight += speedDelta ;

			if (BWDleftFrontCalibrated && BWDleftBackCalibrated)
				speedRefLeft = 0 ;

			if (BWDrightFrontCalibrated && BWDrightBackCalibrated)
				speedRefRight = 0 ;
		}

		rawCountLFPrev = rawCountLF ;
		rawCountLBPrev = rawCountLB ;
		rawCountRFPrev = rawCountRF ;
		rawCountRBPrev = rawCountRB ;

		drive->setSpeedRef(leftMotor, &speedRefLeft);
		drive->setSpeedRef(rightMotor, &speedRefRight);

		// set 'all' flags
		allFWDCalibrated = FWDleftFrontCalibrated && FWDleftBackCalibrated && FWDrightFrontCalibrated && FWDrightBackCalibrated ;
		allBWDCalibrated = BWDleftFrontCalibrated && BWDleftBackCalibrated && BWDrightFrontCalibrated && BWDrightBackCalibrated ;

		// wait a little bit
		struct timespec tim;
		tim.tv_sec = 0;
		tim.tv_nsec = 50000000;
		nanosleep(&tim, NULL);
		calibrationIP = !(allFWDCalibrated && allBWDCalibrated) ;
	}

	cout << "calibration done " << endl ;
	cout << forwardLeftDeadband << "\t||\t" << forwardRightDeadband << endl ;
	cout << reverseLeftDeadband << "\t||\t" << reverseRightDeadband << endl ;

	// send null bias to objects
	drive->setNullbias(leftMotor, forwardNullbiasDir, &forwardLeftDeadband);
	drive->setNullbias(leftMotor, reverseNullbiasDir, &reverseLeftDeadband);
	drive->setNullbias(rightMotor, forwardNullbiasDir, &forwardRightDeadband);
	drive->setNullbias(rightMotor, reverseNullbiasDir, &reverseRightDeadband);

	// now save into config file
	fstream fd("speedControl.cfg");
	if (!fd.is_open()){
		cout << "failed to open config file speedControl.cfg" << endl;
		return true ;
	}

	fd << "forwardLeftDeadband " << forwardLeftDeadband << endl ;
	fd << "reverseLeftDeadband " << reverseLeftDeadband << endl ;
	fd << "forwardRightDeadband " << forwardRightDeadband << endl ;
	fd << "reverseRightDeadband " << reverseRightDeadband << endl ;

	fd.close();

	return true ;
}

