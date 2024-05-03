/*
 * MAX31865_lib.c
 * Written by: Abderrahmane Taha
*/

#include "MAX31865_lib.h"

/*-------------- Characteristics of the PT100 sensor and the reference resistor connected to MAX31865 ------------*/
#define MAX31865_PT100_R0 (double)100.0 // Resistance of the PT100 sensor at 0°C
#define MAX31865_Rref (double)428.5 // Resistance of the reference resistor connected to MAX31865
/*-------------- Characteristics of the PT100 sensor and the reference resistor connected to MAX31865 ------------*/

/*----------- Coefficients from GOST 6651-2009 for PT100 sensor (Platinum RTD and CE, 0.00385°C^-1) ---------*/
#define MAX31865_A (double)0.0039083
#define MAX31865_B (double)0.0000005775
/*----------- Coefficients from GOST 6651-2009 for PT100 sensor (Platinum RTD and CE, 0.00385°C^-1) ---------*/

/*----------------------------------------- Global variables ---------------------------------------------*/
double MAX31865_PT100_R = 0.0; // Global variable defining the resistance of the PT100 sensor
double MAX31865_PT100_T = 0.0; // Global variable defining the temperature of the PT100 sensor
bool MAX31865_Sensor_Error = 0; // Global variable defining the fault status of the PT100 sensor
/*----------------------------------------- Global variables ---------------------------------------------*/

/*------------------------------------------- For SPI operation -----------------------------------------------*/
#define cs_set() CS_GPIO_Port-> BSRR = (uint32_t) CS_Pin << 16u; // Pull CS to ground
#define cs_reset() CS_GPIO_Port ->BSRR = CS_Pin; // Pull CS to 3.3V
extern SPI_HandleTypeDef hspi1;
// P.S. Maximum SPI speed is 5 MHz.
// Also note that Clock Polarity (CLPOL) = Low and Clock Phase (CPHA) = 2 Edge.
/*------------------------------------------- For SPI operation -----------------------------------------------*/

/*======================= INITIALIZATION OF THE MAX31865 MODULE =========================*/
void MAX31865_Init(uint8_t num_wires) {
	/// Function to initialize the MAX31865 module
	/// Since there's no need to display the complete module setup, we'll make
	/// a small simplification for the end user
	/// all the user can configure is to choose the type of connection
	/// 2, 3, or 4 wires
	/// \param num_wires - type of sensor connection: 2, 3, or 4 wires
	uint8_t MAX31865_Reinitialization_cnt = 0;
	MAX31865_Sensor_Error = 0;
	uint8_t MAX31865_Configuration_register_write[] = { 0x80, 0x00 };
	if (num_wires == 2 || num_wires == 4) {
		MAX31865_Configuration_register_write[1] = 0xC3; //0xC3
	} else if (num_wires == 3) {
		MAX31865_Configuration_register_write[1] = 0xD3; //0xD3
	}
	cs_set();
	HAL_SPI_Transmit(&hspi1, MAX31865_Configuration_register_write, 2, 100);
	cs_reset();
	// To reach the sensor after power-up, since initialization may not succeed at first, let's introduce a loop.
	while (MAX31865_Configuration_info() != 0xD1 && MAX31865_Configuration_info() != 0xC1) {
		MAX31865_Reinitialization_cnt++;

		cs_set();
		HAL_SPI_Transmit(&hspi1, MAX31865_Configuration_register_write, 2, 100);
		cs_reset();

		if (MAX31865_Reinitialization_cnt == 100) {
			//printf("Initialization MAX31865 != OK\r\n");
			break;
		}

	}

}
/*======================= INITIALIZATION OF THE MAX31865 MODULE =========================*/

/*==================== INFORMATION ABOUT THE CONFIGURATION OF THE MAX31865 MODULE ===================*/
uint8_t MAX31865_Configuration_info(void) {
	/// Function to get information about the configuration of the MAX31865 module
	/// Returns the configuration value.
	/// Don't be surprised if you send 0xC3 during initialization and get 0xC1
	/// (see MAX31865 datasheet p.14 "The fault status clear bit D1, self-clears to 0.")
	uint8_t read_data = 0x00;
	uint8_t MAX31865_Configuration = 0x00;
	cs_set();
	HAL_SPI_Transmit(&hspi1, &read_data, 1, 100);
	HAL_SPI_Receive(&hspi1, &MAX31865_Configuration, 1, 100);
	cs_reset();
	return MAX31865_Configuration;
}
/*==================== INFORMATION ABOUT THE CONFIGURATION OF THE MAX31865 MODULE ===================*/

/*=================== MAIN FUNCTION FOR WORKING WITH THE MAX31865 MODULE ==================*/
double MAX31865_Get_Temperature(void) {
	/// Main function for working with the MAX31865 module
	/// Accesses the initial address of the module memory register and reads 7 bytes from it.
	/// The function also includes self-diagnosis of the module, which will report if there's something wrong with the sensor.

	double data; // variable for calculations

	struct rx_data_MAX31865 {
		uint16_t RTD_Resistance_Registers; // Resistance registers
		uint16_t High_Fault_Threshold; // High fault threshold
		uint16_t Low_Fault_Threshold; // Low fault threshold
		uint8_t Fault_Status; // Fault status
	};

	struct rx_data_MAX31865 MAX31865_receieve_data;

	uint8_t MAX31865_start_address_of_the_poll = 0x01; // Address of the register from which to start reading data
	uint8_t MAX31865_rx_buffer[7]; // buffer to store incoming data
	cs_set();
	HAL_SPI_Transmit(&hspi1, &MAX31865_start_address_of_the_poll, 1, 100);
	HAL_SPI_Receive(&hspi1, MAX31865_rx_buffer, 7, 100);
	cs_reset();
	MAX31865_receieve_data.RTD_Resistance_Registers = ((MAX31865_rx_buffer[0] << 8) | MAX31865_rx_buffer[1]) >> 1; // Resistance register data
	MAX31865_receieve_data.High_Fault_Threshold = ((MAX31865_rx_buffer[2] << 8) | MAX31865_rx_buffer[3]) >> 1; // High fault threshold data
	MAX31865_receieve_data.Low_Fault_Threshold = (MAX31865_rx_buffer[4] << 8) | MAX31865_rx_buffer[5]; // Low fault threshold data
	MAX31865_receieve_data.Fault_Status = MAX31865_rx_buffer[6]; // Fault status
	if (MAX31865_receieve_data.Fault_Status > 0x00) {

		/*-------------- Your actions to react to the sensor error here ---------------*/
		MAX31865_Sensor_Error = 1;
		//printf("Sensor Error!\r\n");

		/*---- Automatic error reset ----*/
		MAX31865_Init(3);
		MAX31865_Sensor_Error = 0;
		/*---- Automatic error reset ----*/

		// This is how you can reset the error by reinitializing the sensor.
		// Error reset, as desired. Usually, errors are not automatically reset, but the operator is notified to acknowledge the error.
		// Until the operator arrives, the setup remains in error, and all control nodes should be disabled.
		/*-------------- Your actions to react to the sensor error here ---------------*/

	}

	//printf("RTD Resistance Registers = %X\r\n", MAX31865_receieve_data.RTD_Resistance_Registers);
	//printf("High Fault Threshold = %X\r\n", MAX31865_receieve_data.High_Fault_Threshold);
	//printf("Low Fault Threshold = %X\r\n", MAX31865_receieve_data.Low_Fault_Threshold);
	//printf("Fault status = %X\r\n", MAX31865_receieve_data.Fault_Status = MAX31865_rx_buffer[6]);
	data = ((double) MAX31865_receieve_data.RTD_Resistance_Registers * MAX31865_Rref ) / (double) 32768.0; // Replace 4000 by 400 for PT100
	//printf("Rrtd = %lf\n", data);
	return MAX31865_Get_Temperature_math(data);
}
/*=================== MAIN FUNCTION FOR WORKING WITH THE MAX31865 MODULE ==================*/

/*========================================= CONVERSION OF RESISTANCE TO TEMPERATURE ACCORDING TO GOST 6651-2009 =========================================*/
double MAX31865_Get_Temperature_math(double PT100_Resistance) {
	if (PT100_Resistance >= (double) 100.0) {
		double MAX31865_math_Discriminant = (double) 0.00001527480889 - ((double) -0.00000231 * (1 - (PT100_Resistance / MAX31865_PT100_R0 )));
		MAX31865_PT100_T = ((double) -0.0039083 + sqrt(MAX31865_math_Discriminant)) / (double) -0.000001155;
	} else {
		MAX31865_PT100_T = (double) 0.000000000270 * pow(PT100_Resistance, 5) - (double) 0.000000066245 * pow(PT100_Resistance, 4) - (double) 0.000000184636 * pow(PT100_Resistance, 3)
				+ (double) 0.002320232987 * pow(PT100_Resistance, 2) + (double) 2.229927824035 * PT100_Resistance - (double) 242.090854986215;
	}
	return MAX31865_PT100_T;
}
/*========================================= CONVERSION OF RESISTANCE TO TEMPERATURE ACCORDING TO GOST 6651-2009 =========================================*/
