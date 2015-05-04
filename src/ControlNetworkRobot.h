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

#ifndef CONTROLNETWORKROBOT_H_
#define CONTROLNETWORKROBOT_H_

#include "udp_client_server.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
#include "RoverPlatform.hpp"
#include "MotorDriverMC33926.h"
#include "Encoder.h"
#include "BlackADC.h"
#include <vector>
#include "MotorDriverMC33926.h"
#include "Rover.h"

	/*
	 *
	 * sensor related
	 * -----------------
	 * ENCODER RIGHT FRONT: - A input				INT
	 * 						- B input				INT
	 * 						- counter				INT
	 * ENCODER RIGHT BACK:  - A input				INT
	 * 						- B input				INT
	 * 						- counter				INT
	 * ENCODER LEFT FRONT:  - A input				INT
	 * 						- B input				INT
	 * 						- counter				INT
	 * ENCODER LEFT BACK:   - A input				INT
	 * 						- B input				INT
	 * 						- counter				INT
	 * MOTOR RIGHT:         - speed reference		FLOAT
	 *                      - current				FLOAT
	 *                      - speed feedback		FLOAT
	 *                      - status flag			INT
	 *                      - in1					INT
	 *                      - in2					INT
	 * MOTOR LEFT:          - speed reference		FLOAT
	 *                      - current				FLOAT
	 *                      - speed feedback		FLOAT
	 *                      - status flag			INT
	 *                      - in1					INT
	 *                      - in2					INT
	 * OTHER:               - drive enabled			INT
	 *                      - Vbatt voltage			FLOAT
	 *                      - Control voltage		FLOAT
	 *
	 */
struct _NetworkOutgoing {

	int 			erf_aIn ;
	int 			erf_bIn ;
	int 			erf_cnt ;
	int 			erb_aIn ;
	int 			erb_bIn ;
	int 			erb_cnt ;
	int 			elf_aIn ;
	int 			elf_bIn ;
	int 			elf_cnt ;
	int 			elb_aIn ;
	int 			elb_bIn ;
	int 			elb_cnt ;
	float           mtr_spdref ;
	float           mtr_curfbk ;
	float           mtr_spdfbk ;
	int             mtr_status ;
	int             mtr_in1 ;
	int             mtr_in2 ;
	float           mtl_spdref ;
	float           mtl_curfbk ;
	float           mtl_spdfbk ;
	int             mtl_status ;
	int             mtl_in1 ;
	int             mtl_in2 ;
	int             drive_enabled ;
	float           vbatt_volt ;
	float           ctrl_volt ;
	long 			erf_rawCounts ;
	long 			erb_rawCounts ;
	long 			elf_rawCounts ;
	long 			elb_rawCounts ;


} typedef NetworkOutgoing;


/*
 *
 * speed ref related
 * -----------------
 * speed ref enable     int
 * speed ref balance    int
 * speed ref value      int
 *
 */
struct _NetworkIncoming {

int     speedRefEnable ;
int     speedRefBalance ;
int     speedRefValue ;

} typedef NetworkIncoming;


// let compiler know about Rover class
class Rover ;





class ControlNetworkRobot {
public:
	ControlNetworkRobot(Rover*);
	virtual ~ControlNetworkRobot();

private:
	static void * commReceiveThread(void *);
	static void * commTransmitThread(void *);

	void initializeNetworkPages();

	// network page
	NetworkOutgoing ntOutPage ;
	NetworkIncoming ntInPage ;

	// threads
	pthread_t receiveThreadPtr ;
	pthread_t transmitThreadPtr ;
};

#endif /* CONTROLNETWORKROBOT_H_ */
