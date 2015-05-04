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

#ifndef SPEEDCONTROL_H_
#define SPEEDCONTROL_H_

#include <vector>
#include "Encoder.h"
#include "MotorDriverMC33926.h"

class SpeedControl {
public:
	SpeedControl(DualMC33926 *, std::vector<Encoder>*);
	virtual ~SpeedControl();

private:
	bool calibrateFunction();
	DualMC33926 				*drive ;
	std::vector<Encoder>    	*encoderList ;
};

#endif /* SPEEDCONTROL_H_ */
