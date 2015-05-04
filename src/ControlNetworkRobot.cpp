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

#include "ControlNetworkRobot.h"

ControlNetworkRobot::ControlNetworkRobot(Rover *platfrom) {
	// TODO Auto-generated constructor stub

	// zero out page
	this->ntInPage.speedRefBalance = 0 ;
	this->ntInPage.speedRefEnable = 0 ;
	this->ntInPage.speedRefValue = 0 ;


	// start transmitting thread
	int status = pthread_create(&this->transmitThreadPtr,
								NULL,
								ControlNetworkRobot::commTransmitThread,
								(void *)(platfrom));
	if (status != 0) {
		std::cout << "error starting transmitting thread; aborting" << std::endl;
		exit(0);
	}

	// start receiving thread
	status = pthread_create(&this->receiveThreadPtr,
								NULL,
								ControlNetworkRobot::commReceiveThread,
								(void *)(platfrom));
	if (status != 0) {
		std::cout << "error starting receiving thread; aborting" << std::endl;
		exit(0);
	}

}

ControlNetworkRobot::~ControlNetworkRobot() {
	// TODO Auto-generated destructor stub
}


void * ControlNetworkRobot::commTransmitThread(void *args) {

	// cast void into Rover type
	Rover *platform = (Rover *)(args) ;
	// sockets
	udp_client_server::udp_client		*socketOut ;

	struct timespec tim;
	tim.tv_sec = 0;
	tim.tv_nsec = 100000000;

	//socketOut = new udp_client_server::udp_client("127.0.0.1", 60001);
	socketOut = new udp_client_server::udp_client("192.168.1.103", 60001);
	//socketOut = new udp_client_server::udp_client("192.168.7.1", 60001);
	std::cout << "TX thread reporting" << std::endl;
	while(1){

		try {

			//ntOutPage.erf_aIn 		=
			//ntOutPage.erf_bIn ;
			//ntOutPage.erf_cnt ;
			//ntOutPage.erb_aIn ;
			//ntOutPage.erb_bIn ;
			//ntOutPage.erb_cnt ;
			//ntOutPage.elf_aIn ;
			//ntOutPage.elf_bIn ;
			//ntOutPage.elf_cnt ;
			//ntOutPage.elb_aIn ;
			//ntOutPage.elb_bIn ;
			//ntOutPage.elb_cnt ;
			platform->network->ntOutPage.mtr_spdref 	= platform->drive->getSpeedReference(leftMotor) ;
			platform->network->ntOutPage.mtr_curfbk		= platform->motor1Curr->getNumericValue() * 5700 / 525 ;
			platform->network->ntOutPage.mtr_spdfbk		= platform->motor1DriverConfig->feedback ;
			platform->network->ntOutPage.mtr_status		= platform->motor1DriverConfig->statusFlag ;
			platform->network->ntOutPage.mtr_in1		= platform->motor1DriverConfig->in1 ;
			platform->network->ntOutPage.mtr_in2		= platform->motor1DriverConfig->in2 ;
			platform->network->ntOutPage.mtl_spdref 	= platform->drive->getSpeedReference(rightMotor) ;
			platform->network->ntOutPage.mtl_curfbk		= platform->motor2Curr->getNumericValue() * 5700 / 525 ;
			platform->network->ntOutPage.mtl_spdfbk		= platform->motor2DriverConfig->feedback ;
			platform->network->ntOutPage.mtl_status		= platform->motor2DriverConfig->statusFlag ;
			platform->network->ntOutPage.mtl_in1		= platform->motor2DriverConfig->in1 ;
			platform->network->ntOutPage.mtl_in2		= platform->motor2DriverConfig->in2 ;
			platform->network->ntOutPage.drive_enabled	= platform->drive->isEnabledDrive() ;
			platform->network->ntOutPage.vbatt_volt		= platform->motorVBatt->getNumericValue() * 0.0057 ;
			platform->network->ntOutPage.ctrl_volt		= 0.0 ;

			std::vector<Encoder>::iterator buffer = platform->encoderList.begin() ;

			platform->network->ntOutPage.erf_rawCounts	= buffer->getCountsRaw() ;
			platform->network->ntOutPage.erb_rawCounts	= buffer[1].getCountsRaw() ;
			platform->network->ntOutPage.elf_rawCounts	= buffer[2].getCountsRaw() ;
			platform->network->ntOutPage.elb_rawCounts	= buffer[3].getCountsRaw() ;
		} catch (...){

		}

		socketOut->send((const char*)&platform->network->ntOutPage,
						sizeof(platform->network->ntOutPage)) ;

		// wait a little bit
		nanosleep(&tim, NULL);

	}

	return NULL ;
}


void * ControlNetworkRobot::commReceiveThread(void *args) {
	// cast void into Rover type
	Rover *platform = (Rover *)(args) ;
	// sockets
	udp_client_server::udp_server		*socketIn ;

	socketIn = new udp_client_server::udp_server("192.168.1.101", 60000) ;
	//socketIn = new udp_client_server::udp_server("192.168.7.2", 60000) ;

	std::cout << "RX thread reporting" << std::endl;
	std::cout << "size is " << sizeof(platform->network->ntInPage) << std::endl ;
	while(1){
		char message[ sizeof(platform->network->ntInPage) ] ;

		socketIn->recv(message, sizeof(message));

		platform->network->ntInPage.speedRefEnable = *((int*)message) ;
		platform->network->ntInPage.speedRefBalance = *((int*)message+1) ;
		platform->network->ntInPage.speedRefValue = *((int*)message+2) ;

		//std::cout << "enab: " << platform->network->ntInPage.speedRefEnable
		//		  << " ref: " << platform->network->ntInPage.speedRefValue
		//		  << " bal: " << platform->network->ntInPage.speedRefBalance
		//		  << std::endl ;

		if (platform->network->ntInPage.speedRefEnable == 1){
			int reference, balance ;
			reference = platform->network->ntInPage.speedRefValue ;
			balance = platform->network->ntInPage.speedRefBalance ;

			float rightMod, leftMod ;
			if (balance >= 50)
				rightMod = (float)(75-balance) / 25 ;
			else
				rightMod = 1 ;

			if (balance <= 50)
				leftMod  = (float)(balance - 25) / 25 ;
			else
				leftMod = 1 ;

			int leftRef, rightRef ;

			leftRef = leftMod * reference ;
			rightRef = rightMod * reference ;

			platform->drive->setSpeedRef(leftMotor, &leftRef) ;
			platform->drive->setSpeedRef(rightMotor, &rightRef) ;

			std::cout << "leftRef = " << leftRef
				      << "rightRef = " << rightRef
				      << std::endl ;

		} else {
			int leftRef = 0 ;
			int rightRef = 0 ;

			platform->drive->setSpeedRef(leftMotor, &leftRef) ;
			platform->drive->setSpeedRef(rightMotor, &rightRef) ;

			std::cout << "leftRef = " << leftRef
				      << "rightRef = " << rightRef
				      << std::endl ;
		}
	}

	return NULL ;
}


void ControlNetworkRobot::initializeNetworkPages() {



}

