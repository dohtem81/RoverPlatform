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

#ifndef ROVERSIMULATOR_H_
#define ROVERSIMULATOR_H_

#include <string>

const std::string 		MainThreadTiming		= "MAIN_THREAD_TIMING" ;

// threads definitions
const int				PRINTER_THREAD			= 0 ;
const int				SIMULATOR_THREAD		= 1 ;

const int				NUMBER_OF_THREADS		= 2 ;

// this section is about filenames that are being used to
// write simulated outputs
const std::string		GPIO_FOLDER				= "/home/strus/workspace/RoverSimulator/SIMMULATION/" ;
const std::string		LTFENCODER_A			= GPIO_FOLDER + "LTFENCODER_A" ;
const std::string		LTFENCODER_B			= GPIO_FOLDER + "LTFENCODER_B" ;
const std::string		LTRENCODER_A			= GPIO_FOLDER + "LTRENCODER_A" ;
const std::string		LTRENCODER_B			= GPIO_FOLDER + "LTRENCODER_B" ;
const std::string		RTFENCODER_A			= GPIO_FOLDER + "RTFENCODER_A" ;
const std::string		RTFENCODER_B			= GPIO_FOLDER + "RTFENCODER_B" ;
const std::string		RTRENCODER_A			= GPIO_FOLDER + "RTRENCODER_A" ;
const std::string		RTRENCODER_B			= GPIO_FOLDER + "RTRENCODER_B" ;

const int				PT_LTFENCODER_A			= 0 ;
const int				PT_LTFENCODER_B			= 1 ;
const int				PT_LTRENCODER_A			= 2 ;
const int				PT_LTRENCODER_B			= 3 ;
const int				PT_RTFENCODER_A			= 4 ;
const int				PT_RTFENCODER_B			= 5 ;
const int				PT_RTRENCODER_A			= 6 ;
const int				PT_RTRENCODER_B			= 7 ;

const int				ENCODERFILESCOUNT		= 8 ;


#endif // ROVERSIMULATOR_H_
