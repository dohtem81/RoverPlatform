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

#ifndef ENCODER_H_
#define ENCODER_H_

// includes
#include <pthread.h>
#include "BlackGPIO.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

void * monitorThread(void *);


// class declaration
// ----------------------------------------------------------------------------------

class Encoder {
public:
	Encoder(BlackLib::gpioName, BlackLib::gpioName, std::string name="");
	long getCountsRaw() { return counter ; } ;
	bool isLocked() ;
	int lock() { return pthread_mutex_lock(&this->myMutex); } ;
	int unlock() { return pthread_mutex_unlock(&this->myMutex); } ;
	BlackLib::BlackGPIO* getGPIO_A() { return aFile; };
	BlackLib::BlackGPIO* getGPIO_B() { return bFile; };
	bool getAState() { return this->aState ; } ;
	bool getBState() { return this->bState ; } ;
	void setAState(bool *state) { this->aState = *state ; } ;
	void setBState(bool *state) { this->bState = *state ; } ;
	long modCounter(long delta) { counter += delta; return counter; } ;
	void readBothPhases(bool*, bool*);

	std::string						myName;
	long 							prevDelta ;

private:
	BlackLib::BlackGPIO				*aFile ;
	BlackLib::BlackGPIO 			*bFile ;
	long							counter ;
	bool							aState ;
	bool							bState ;
	std::ifstream					*aFD ;
	std::ifstream					*bFD ;

	pthread_t						myThread ;
	pthread_mutex_t					myMutex ;

};


#endif /* ENCODER_H_ */
