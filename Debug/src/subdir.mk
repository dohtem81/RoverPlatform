################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/strus/workspace/BlackLib/v2_0/BlackADC.cpp \
/home/strus/workspace/BlackLib/v2_0/BlackCore.cpp \
/home/strus/workspace/BlackLib/v2_0/BlackGPIO.cpp \
/home/strus/workspace/BlackLib/v2_0/BlackPWM.cpp \
../src/ControlNetworkRobot.cpp \
../src/Encoder.cpp \
../src/MotorDriverMC33926.cpp \
../src/Rover.cpp \
../src/RoverPlatform.cpp \
../src/SpeedControl.cpp \
/home/strus/workspace/BlackLib/v2_0/exampleAndTiming/Timing.cpp \
/home/strus/workspace/Inotify/inotify-cxx.cpp \
../src/udp_client_server.cpp 

OBJS += \
./src/BlackADC.o \
./src/BlackCore.o \
./src/BlackGPIO.o \
./src/BlackPWM.o \
./src/ControlNetworkRobot.o \
./src/Encoder.o \
./src/MotorDriverMC33926.o \
./src/Rover.o \
./src/RoverPlatform.o \
./src/SpeedControl.o \
./src/Timing.o \
./src/inotify-cxx.o \
./src/udp_client_server.o 

CPP_DEPS += \
./src/BlackADC.d \
./src/BlackCore.d \
./src/BlackGPIO.d \
./src/BlackPWM.d \
./src/ControlNetworkRobot.d \
./src/Encoder.d \
./src/MotorDriverMC33926.d \
./src/Rover.d \
./src/RoverPlatform.d \
./src/SpeedControl.d \
./src/Timing.d \
./src/inotify-cxx.d \
./src/udp_client_server.d 


# Each subdirectory must supply rules for building sources it contributes
src/BlackADC.o: /home/strus/workspace/BlackLib/v2_0/BlackADC.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/arm-linux-gnueabihf/include/c++/4.8.1 -I"/home/strus/workspace/BlackLib/v2_0" -I"/home/strus/workspace/Inotify" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/BlackCore.o: /home/strus/workspace/BlackLib/v2_0/BlackCore.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/arm-linux-gnueabihf/include/c++/4.8.1 -I"/home/strus/workspace/BlackLib/v2_0" -I"/home/strus/workspace/Inotify" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/BlackGPIO.o: /home/strus/workspace/BlackLib/v2_0/BlackGPIO.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/arm-linux-gnueabihf/include/c++/4.8.1 -I"/home/strus/workspace/BlackLib/v2_0" -I"/home/strus/workspace/Inotify" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/BlackPWM.o: /home/strus/workspace/BlackLib/v2_0/BlackPWM.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/arm-linux-gnueabihf/include/c++/4.8.1 -I"/home/strus/workspace/BlackLib/v2_0" -I"/home/strus/workspace/Inotify" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/arm-linux-gnueabihf/include/c++/4.8.1 -I"/home/strus/workspace/BlackLib/v2_0" -I"/home/strus/workspace/Inotify" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Timing.o: /home/strus/workspace/BlackLib/v2_0/exampleAndTiming/Timing.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/arm-linux-gnueabihf/include/c++/4.8.1 -I"/home/strus/workspace/BlackLib/v2_0" -I"/home/strus/workspace/Inotify" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/inotify-cxx.o: /home/strus/workspace/Inotify/inotify-cxx.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/arm-linux-gnueabihf/include/c++/4.8.1 -I"/home/strus/workspace/BlackLib/v2_0" -I"/home/strus/workspace/Inotify" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


