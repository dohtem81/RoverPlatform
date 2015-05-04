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

#include "Rover.h"

using namespace std ;

Rover::Rover() {
	// TODO Auto-generated constructor stub

	// left motor configuration
	motor1DriverConfig = new MC3392Config();
	motor1DriverConfig->inPWM1		= BlackLib::EHRPWM1A ;									// P9_16
	motor1DriverConfig->statusFlag	= BlackLib::GPIO_74 ;									// P8_41
	motor1DriverConfig->in2 		= BlackLib::GPIO_72 ;									// P8_43
	motor1DriverConfig->in1 		= BlackLib::GPIO_73 ;									// P8_44

	//right motor configuration
	motor2DriverConfig = new MC3392Config();
	motor2DriverConfig->inPWM1		= BlackLib::EHRPWM1B ;									// P9_14
	motor2DriverConfig->statusFlag	= BlackLib::GPIO_75 ;									// P8_42
	motor2DriverConfig->in2 		= BlackLib::GPIO_71 ;									// P8_46
	motor2DriverConfig->in1 		= BlackLib::GPIO_70 ;									// P8_45

	// motor battery voltage monitor
	motorVBatt						= new BlackLib::BlackADC(BlackLib::AIN0 );

	// current feedback from motors
	motor1Curr 						= new BlackLib::BlackADC(BlackLib::AIN1 );
	motor2Curr 						= new BlackLib::BlackADC(BlackLib::AIN2 );

	drive 							= new DualMC33926(motor1DriverConfig,
			                                          motor2DriverConfig,
			                                          BlackLib::GPIO_46);		// P8_16

	//encoders
	encoderList.push_back(*(new Encoder(BlackLib::GPIO_23, BlackLib::GPIO_45, "LtFt"))) ;	//
	encoderList.push_back(*(new Encoder(BlackLib::GPIO_66, BlackLib::GPIO_69, "LtRe"))) ;	//
	encoderList.push_back(*(new Encoder(BlackLib::GPIO_68, BlackLib::GPIO_67, "RtFt"))) ;	//
	encoderList.push_back(*(new Encoder(BlackLib::GPIO_26, BlackLib::GPIO_44, "RtRe"))) ;	//
	//encoderList.push_back(*(new Encoder(BlackLib::GPIO_45, BlackLib::GPIO_23, "LtFt"))) ;	//
	//encoderList.push_back(*(new Encoder(BlackLib::GPIO_69, BlackLib::GPIO_66, "LtRe"))) ;	//
	//encoderList.push_back(*(new Encoder(BlackLib::GPIO_67, BlackLib::GPIO_68, "RtFt"))) ;	//
	//encoderList.push_back(*(new Encoder(BlackLib::GPIO_44, BlackLib::GPIO_26, "RtRe"))) ;	//

	// start monitoring thread
	pthread_t monitorThreadPtr ;
	int status = pthread_create(&monitorThreadPtr, NULL, &monitorThread, (void *)&encoderList);
	if (status != 0) {
		cout << "error starting encoder monitor; aborting" << endl;
		exit(0);
	}

	// zero speed refs
	int speedRefLeft = 0 ;
	int speedRefRight = 0 ;
	// reset speeds
	drive->setSpeedRef(leftMotor, &speedRefLeft);
	drive->setSpeedRef(rightMotor, &speedRefRight);


	// wait for 3 sec before enabling drive
	cout << "waiting 3s before releasing speed ref and enabling drive..." << endl;
	sleep(3);
	cout << "drive enabled" << endl;
	drive->setEnableDrive();

	// create network object
	network = new ControlNetworkRobot(this);

	// reset flags
	this->calibrationInProgress = false ;
	this->testDriveInProgress   = false ;
	this->calibrationDone		= true ;
	this->testDriveDone			= true ;

	if (testsAtStartup) {
		if (useThreads) {
			// start calibration
			cout << "calibration request" << endl ;
			this->calibrationInProgress = true ;
			pthread_t calibrationThreadPtr ;
			status = pthread_create(&calibrationThreadPtr,
									NULL,
									Rover::nullBiasCalibration,
									(void *)this);
			if (status != 0) {
				cout << "error starting calibration; aborting" << endl;
				exit(0);
			}

			// wait for calibration to finish and start next test
			cout << "waiting for calibration to complete" << endl ;
			int index = 0 ;
			while(!this->calibrationDone || this->calibrationInProgress){
				cout << "waiting for calibration - tick#" << index << endl ;

				struct timespec tim;
				tim.tv_sec = 1;
				tim.tv_nsec = 0;
				nanosleep(&tim, NULL);

				if (index++ >= 2) {
					this->calibrationInProgress = false ;
					cout << "calibration time out. failed." << endl ;
				}
			}

			// starting drive test
			pthread_t testDriveThreadPtr ;
			this->testDriveInProgress = true ;
			status = pthread_create(&testDriveThreadPtr,
									NULL,
									Rover::driveTest,
									(void *)this);
			if (status != 0) {
				cout << "error starting test drive; aborting" << endl;
				exit(0);
			}

			while(this->testDriveInProgress) ;
		} else {
			// start calibration
			cout << "calibration request" << endl ;
			this->calibrationInProgress = true ;
			Rover::nullBiasCalibration((void *)this);

			//this->testDriveInProgress = true ;
			//Rover::driveTest((void *)this) ;

		}
	}

	while(true) {
		struct timespec tim;
		tim.tv_sec = 1;
		tim.tv_nsec = 0;
		nanosleep(&tim, NULL);
	}
}


Rover::~Rover() {
	// TODO Auto-generated destructor stub
}


// ================================================================
//
//	runs null bias calibration
//  *args should be pointer to Rover object
//
// ================================================================
void * Rover::nullBiasCalibration(void *args){
	// get platform
	Rover *platform = (Rover*)(args) ;

	// set the flag that calibration is in progress
	platform->calibrationInProgress = true ;

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

	long rawCountLF, rawCountRF, rawCountLB, rawCountRB ;
	long rawCountLFPrev, rawCountRFPrev, rawCountLBPrev, rawCountRBPrev ;
	std::vector<Encoder>::iterator buffer = platform->encoderList.begin();

	rawCountLFPrev = buffer++->getCountsRaw() ;
	rawCountLBPrev = buffer++->getCountsRaw() ;
	rawCountRFPrev = buffer++->getCountsRaw() ;
	rawCountRBPrev = buffer->getCountsRaw() ;

	int speedRefLeft = 0 ;
	int speedRefRight = 0 ;
	int forwardLeftDeadband  = 0 ;
	int forwardRightDeadband = 0 ;
	int reverseLeftDeadband  = 0 ;
	int reverseRightDeadband = 0 ;
	int speedDelta = 1 ;

	while (platform->calibrationInProgress){

		// get all encoder feedbacks
		std::vector<Encoder>::iterator buffer = platform->encoderList.begin() ;
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

		platform->drive->setSpeedRef(leftMotor, &speedRefLeft);
		platform->drive->setSpeedRef(rightMotor, &speedRefRight);

		// set 'all' flags
		allFWDCalibrated = FWDleftFrontCalibrated && FWDleftBackCalibrated && FWDrightFrontCalibrated && FWDrightBackCalibrated ;
		allBWDCalibrated = BWDleftFrontCalibrated && BWDleftBackCalibrated && BWDrightFrontCalibrated && BWDrightBackCalibrated ;

		if (platform->calibrationInProgress)
			platform->calibrationInProgress = !(allFWDCalibrated && allBWDCalibrated) ;

		// wait a little bit
		struct timespec tim;
		tim.tv_sec = 0;
		tim.tv_nsec = 50000000;
		nanosleep(&tim, NULL);
	}

	// send null bias to objects
	platform->drive->setNullbias(leftMotor, forwardNullbiasDir, &forwardLeftDeadband);
	platform->drive->setNullbias(leftMotor, reverseNullbiasDir, &reverseLeftDeadband);
	platform->drive->setNullbias(rightMotor, forwardNullbiasDir, &forwardRightDeadband);
	platform->drive->setNullbias(rightMotor, reverseNullbiasDir, &reverseRightDeadband);

	cout << "leaving calibration" << endl ;
	platform->calibrationDone = true ;
	return NULL ;
}


// ================================================================
//
//	runs null bias calibration
//  *args should be pointer to Rover object
//
// ================================================================
void * Rover::driveTest(void *args){

	// get platform
	Rover *platform = (Rover*)(args) ;

	// set the flag that calibration is in progress
	platform->testDriveInProgress = true ;

	// this test has folloeing stages
	bool stage1 = true ;		// accel forward to 50%
	bool stage2 = true ;		// drive at 50% for 2s
	bool stage3 = true ;		// decel to 0%
	bool stage4 = true ;		// accel reverse to -50%
	bool stage5 = true ;        // drive at -50% for 2s
	bool stage6 = true ;		// decl to 0%
	bool stage7 = true ;        // test completed

	int speedRef = 0 ;
	int speedTarget = 0 ;
	int speedDelta = 5 ;
	struct timespec tim;

	cout << "starting stage test drive" << endl ;

	while ( stage1 || stage2 || stage3 || stage4 || stage5 || stage6 ) {

		// depending on the stage we set up accel/decel rate and target speed
		if (stage1 || stage2)
			speedTarget = 50 ;
		else if (stage3 || (!stage5 && stage6)) {
			speedTarget = 0 ;
		} else
			speedTarget = -50 ;

		if ( (speedTarget - speedRef) > 0 )
			speedDelta = 5 ;
		else if ( (speedTarget - speedRef) < 0 )
			speedDelta = -5 ;

		// calculate speed ref and send to drives
		speedRef += speedDelta ;
		//platform->drive->setSpeedRef(leftMotor, &speedRef);
		//platform->drive->setSpeedRef(rightMotor, &speedRef);

		// setup timer
		if ((!stage1 && stage2 ) || (!stage3 && stage4) ) {
			tim.tv_sec  = 1;
			tim.tv_nsec = 0;
		} else {
			tim.tv_sec = 0;
			tim.tv_nsec = 50000000;
		}

		cout << "speedRef / target / delta " << speedRef
				<< " / " << speedTarget << " / " << speedDelta << endl ;

		// wait
		nanosleep(&tim, NULL);


		// figure out test step
		if (stage1){
			if (speedRef>=speedTarget) {
				speedRef = speedTarget ;
				stage1 = false ;
				cout << "stage 1 completed" << endl ;
			}
		} else if (stage2) {
			stage2 = false ;
			cout << "stage 2 completed" << endl ;
		} else if (stage3) {
			if (speedRef <= speedTarget) {
				speedRef = speedTarget ;
				stage3 = false ;
				cout << "stage 3 completed" << endl ;
			}
		} else if (stage4) {
			if (speedRef <= speedTarget) {
				speedRef = speedTarget ;
				stage4 = false ;
				cout << "stage 4 completed" << endl ;
			}
		} else if (stage5) {
			stage5 = false ;
			cout << "stage 5 completed" << endl ;
		} else if (stage6) {
			if (speedRef>speedTarget) {
				speedRef = speedTarget ;
				stage6 = false ;
				cout << "stage 6 completed" << endl ;
			}
		}
	}

	speedRef = 0 ;
	platform->drive->setSpeedRef(leftMotor, &speedRef);
	platform->drive->setSpeedRef(rightMotor, &speedRef);

	stage7 = false ;
	cout << "test drive completed" << endl ;


	platform->testDriveInProgress = false ;

	return NULL ;
}


