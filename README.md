# Temperature Controller PID Project

## Overview

This project focuses on developing a temperature controller system using a PID (Proportional-Integral-Derivative) algorithm. The system utilizes an STM32 microcontroller (µC) to maintain a consistent temperature within an oven. Key components include a temperature sensor, a TFT LCD display, and the STM32F407VGT6 development board. The project also incorporates CubeIDE for pin mapping, NVIM for programming efficiency, and employs C and Rust programming languages.

## Components

- **STM32F407VGT6 Board**: This development board serves as the central processing unit for the temperature control system.
- **Temperature Sensor**: Utilizes PT100 resistance temperature detectors (RTD) to accurately measure the temperature inside the oven.
- **LCD Display Module**: A 3.5-inch TFT LCD display module with SPI interface (320x480) and touch screen capability (14 pins) is employed to visualize temperature readings and system status.

- **MAX31865 RTD-to-Digital Converter**: Converts the analog signals from the PT100 sensor into digital signals readable by the microcontroller.

## Functionality

1. **Temperature Sensing and Display**: The temperature sensor detects the temperature within the oven, and the STM32 µC processes this data. The temperature readings are then displayed on the LCD screen, providing real-time feedback to the user.

2. **PID Temperature Control**: The STM32 microcontroller implements a PID controller algorithm to regulate the oven temperature. It continuously adjusts the heating elements to maintain the temperature around the desired setpoint.

3. **User Interaction**: The system allows users to set the desired temperature via the LCD touchscreen interface. The microcontroller translates these user inputs into control signals sent to the oven to adjust its temperature accordingly.

4. **Alarm/Timer Functionality**: There is a provision for implementing alarm and timer functionalities, enhancing the usability of the system. Users can set alarms for specific temperature thresholds or utilize a timer for cooking processes.

## Development Tools

- **CubeIDE**: Used for pin mapping and configuring the STM32 microcontroller, facilitating efficient development and integration of hardware components.
- **NVIM**: Preferred text editor for programming tasks, providing enhanced efficiency and customization options for code development.

## Libraries

- **Custom ILI9431 and Touch Support Libraries**: Rewritten libraries specifically tailored for the project's LCD display module, ensuring seamless integration and optimized performance.

## Programming Languages

- **C and Rust**: The project utilizes both C and Rust programming languages for implementing the firmware and system logic, leveraging their respective strengths for efficient and reliable code execution.
