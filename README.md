*This is the repository for our human powered submarine Axios, that we have decided to no longer to pursue. **IT IS NOT COMPLETE, LET ALONE TESTED.***

*Our motivation for making this repository public to to share what progress we did make to help others who might be interested in developing a similar system.*

# Axios

Our attempt at a human powered submarine, titled Axios. Within it there were two electronics systems to be implemented: rebreather monitoring and navigation. These were meant to be operate entirely independantly of one another, being powered off seperate batteries.

In this repository resides the code and hardware design files prepared for the initial versions of these systems from before the team decided to cancel the project.

Both systems were based around ATmega328P microcontrollers due to the team's familiarity with them and their adaquate amount of peripherals for serving both roles.

## Rebreather System

This was meant to supervise the different gas sensors around the submarine hull to inform the pilot about the atmospheric state within Axios. It collected data from three oxygen sensors and one carbon dioxide (CO2) sensor, it then displayed the value of each on seperate seven-segment displays.

Of the two systems prepared for Axios, this was by far the more mature one. It was essentially ready for final deployment.

### Carbon Dioxide Sensing

For monitoring CO2 an MH-Z19 sensor was used which uses infrared to measure CO2 in the air. Getting data with the sensor was simple, it outputs a PWM signal at 1Hz that was trivial to monitor using the ATmega328P. The only hardware needed to interface it with the Arduino was a level shifting transistor to step the 3.3V PWM signal to 5V for the microcontroller to read.

One minor note we were unaware of is that the sensor has a small heater within it which is used to measure the CO2. This means the sensor occasionally draws a surge of current to keep it warm. This caused us to think the system was malfunctioning and drawing excessive power in our preliminary tests.

### Oxygen Sensing

This was the main focus of this board, since we were responsible for reading the raw sensor data. The oxygen sensors we were planning to use were standard oxygen sensors used for SCUBA diving. These are essentially chemical cells that when exposed to varying concentrations of oxygen in the air would vary their voltage proportionally. They had two issues the circuit needed to address:

1. The output voltage was pretty small. About 65mV for a new cell at 100% oxygen, but we would ideally be monitoring 21%, so about 20mV.
2. Although roughly linear, the output at 0% was not 0 mV, but about 15 mV.
3. Being chemical cells, they would gradually degrade and our circuit would need to be tuned for this.

The solution for this was a set of of high precision amplifiers made from operational amplifiers. These would buffer and then amplifier the sensor output to a level more easily read by the ATmega with its internal ADC resolution of about 4 mV. To adjust the circuit for the sensors as they aged we would conduct a two-point calibration (at 100% and 21% oxygen levels) adjusting potentiometers to get the display values to match these conditions.

## Navigation

Axios was designed to be controlled by three servo-actuated control fins. These needed a microcontroller to recieve input from the captian (?) and actuate them accordingly. In addition to steering the submarine, it was planned for the captian of Axios to be provided orientation data about the submarine (roll, pitch, yaw), depth information, and power from their pedals. 

This board needs significant revision before it is worthy of installation into any submarine.

### Steering

Steering was to be done using a modified game controller (so we had wires directly to the potentiometers within it) which was read as an analog signal and then following some linear algebra, servo angles would be determined and set. INtentionally a short and simple loop.

There was discussion of a feature to have the submarine maintain depth without user input, but we never reached this stage.

### Orientation Data

Orientation data was meant to be provided by the use of a 9-axis IMU attached to the board. This approach quickly revealed a few key issues:

- The boards were going to move frequently within Axios, and the IMU would need to be recalibrated for each new position. This could be remedied by off-boarding the IMU to a set location in Axios.
- The IMU only provided accelerations and magnetic field, we would need to code or find a library to perform the sensor fusion on the ATmega.
   - This would likely lead to significant usage of processor time
   - Our solution may not be able to replicate a more polished sensor even after significant work

Little was done for this as we were planning to transition to another IMU, the BNO055 which would address both of these points.

### Depth Information

Reading the depth was done using a pressure sensor that was connected to a port in the hull. The one we had acquired for Axios output a simple linear output proportional to the pressure sensed and a slight constant offset. Reading this analog signal, then dividing by the specific gravity of water would allow us to accurately estimate our depth in water.

### Pedal Power Data

This was planned to use a Raspberry Pi Zero with an ANT dongle to read available ANT data from the commercial power pedals in Axios and then broadcast it over UART to the microcontroller to display it for the rider.

This never really reached development, although it would likely be heavily based off the working ANT code prepared for [TITAN (2019)](https://github.com/hpvdt/titan_2019).

## Displays

The display system was identical across the boards. Given the amount of pins that needed to be controlled for all the displays a shift register was used for each display to control the segments as the digits were multiplexed by the microcontroller. This meant that all displays on a board could be controller by a handful of pins on the ATmega and a series of daisy-chained shift registers.

Although the two boards would operate seperately, they were designed to be roughly arranged as shown below. These displays were meant to be bright enough to pass through the back of the partial mirror used to provide the captian a view.

![Planned Axios layout](https://github.com/hpvdt/axios/blob/main/Hardware/Axios%20dash.png)

# Software Used

For the design of the hardware EAGLE was used. For the microcontroller code it was all prepared using the Arduino IDE.
