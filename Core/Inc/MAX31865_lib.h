/*
 * MAX31865_lib.h
 * Written by: Abderrahmane Taha
*/


#ifndef INC_MAX31865_LIB_H_
#define INC_MAX31865_LIB_H_

#include <main.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

void MAX31865_Init(uint8_t num_wires);
uint8_t MAX31865_Configuration_info(void);
double MAX31865_Get_Temperature(void);
double MAX31865_Get_Temperature_math(double PT100_Resistance);

#endif /* INC_MAX31865_LIB_H_ */
