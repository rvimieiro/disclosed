################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bitarray.c \
../condtable.c \
../database.c \
../disclosed.c \
../main.c 

OBJS += \
./bitarray.o \
./condtable.o \
./database.o \
./disclosed.o \
./main.o 

C_DEPS += \
./bitarray.d \
./condtable.d \
./database.d \
./disclosed.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -msse4.2 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


