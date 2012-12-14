################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../jni/hellocpp/main.cpp 

OBJS += \
./jni/hellocpp/main.o 

CPP_DEPS += \
./jni/hellocpp/main.d 


# Each subdirectory must supply rules for building sources it contributes
jni/hellocpp/%.o: ../jni/hellocpp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/software/android-ndk-r8b/platforms/android-14/arch-arm/usr/include -I/projects/sdks/cocos2d-x/cocos2d-2.0-x-2.0.2/cocos2dx/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


