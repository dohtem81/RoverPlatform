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

#include <string>
#include <pthread.h>
#include <iostream>
#include <ctime>
#include <vector>
#include "BlackGPIO.h"
#include "Encoder.h"
#include "inotify-cxx.h"


void * monitorThread(void *args){

	std::vector<Encoder> *encoderList ;

	bool currAVal, currBVal, changedA, changedB;
	long delta ;

	// set it real time, round-robin
	pthread_attr_t attr;
	struct sched_param sch_params;

    pthread_attr_getschedparam(&attr, &sch_params);
    sch_params.sched_priority = 99 ; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch_params);

	// convert args to class pointer
	encoderList = (std::vector<Encoder> *)args ;

	// we'll lock mutex, read both files and compare to previous states
	// if anything changes update counter
	std::cout << "encoder monitor thread starting loop for " << encoderList->size() << " encoders" << std::endl ;
	while (true) {
		for (std::vector<Encoder>::iterator buffer = encoderList->begin(); buffer != encoderList->end(); buffer++) {


			currAVal = buffer->getGPIO_A()->isHigh();
			currBVal = buffer->getGPIO_B()->isHigh();

			//buffer->readBothPhases(&currAVal, &currBVal);

			// now we compare previous values with current ones
			changedA = currAVal != buffer->getAState() ;
			changedB = currBVal != buffer->getBState() ;

			if (changedA && changedB) {
				//std::cout << buffer->myName << " encoder error; both inputs changed" << std::endl ;
				if (buffer->prevDelta > 0)
					delta = 2 ;
				else
					delta = -2 ;
			}
			else {
				if (changedA) {
					// forward
					if ( (currAVal && !currBVal) || (!currAVal && currBVal) )
						delta = 1;

					// reverse
					if ( (currAVal && currBVal) || (!currAVal && !currBVal) )
						delta = -1;
				}

				if (changedB) {
					// forward
					if ( (currAVal && currBVal) || (!currAVal && !currBVal) )
						delta = 1;

					// reverse
					if ( (currAVal && !currBVal) || (!currAVal && currBVal) )
						delta = -1;
				}
			}
				// modify raw counts
				if (delta != 0){
					buffer->lock();
					buffer->modCounter(delta);
					buffer->unlock();
					buffer->prevDelta = delta ;
				}
			//}

			// clean up
			buffer->setAState(&currAVal);
			buffer->setBState(&currBVal);
			currAVal = false ;
			currBVal = false ;
			changedA = false ;
			changedB = false ;
			delta = 0 ;
		}

		// sleep for 0.5ms
		struct timespec tim;
		tim.tv_sec = 0;
		tim.tv_nsec = 2500000;
		nanosleep(&tim, NULL);
	}


	return NULL;
}


// class constructor
// ----------------------------------------------------------------------------------------------------
Encoder::Encoder(BlackLib::gpioName aGPIOName, BlackLib::gpioName bGPIOName, std::string name){
	// set up name
	if (name.length() > 0)
		this->myName = name ;
	else {
		this->myName = "MD_MC33926#" ;// + std::to_string (MotorDriverMC33926::instanceCounter++);
	}

	// create pointers to both files
	aFile = new BlackLib::BlackGPIO(aGPIOName, BlackLib::input, BlackLib::FastMode) ;
	bFile = new BlackLib::BlackGPIO(bGPIOName, BlackLib::input, BlackLib::FastMode) ;

	// create mutex
	int status ;
	status = pthread_mutex_init(&this->myMutex, NULL);
	if (status != 0) {
		std::cout << "error initializing " << this->myName << " mutex; aborting" << std::endl;
		exit(0);
	}

	// initialize values
	this->counter = 0 ;
	this->aState = aFile->isHigh();
	this->bState = bFile->isHigh();

	// open files
	aFD = new std::ifstream ;
	bFD = new std::ifstream ;
	aFD->open(aFile->getValueFilePath().c_str(),std::ios::in);
	bFD->open(bFile->getValueFilePath().c_str(),std::ios::in);

}


void Encoder::readBothPhases(bool* aPhase, bool* bPhase){

    *this->aFD >> *aPhase ;
    *this->bFD >> *bPhase ;

    return ;
}




