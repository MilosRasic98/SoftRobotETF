# SoftRobotETF
This repository covers the development of hardware and software of an open-source soft robotics platform developed at the School of Electrical Engineering, University of Belgrade in Serbia.

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Testing/Pictures/IMG_20230920_233352621~2.jpg" width="500">

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Testing/Pictures/IMG_20230920_231954353~2.jpg" width="500">

The idea behind this project is to have a silicone continuum (pink cylinder shown in the picture above) that has internally routed tendons that are controlled with actuators that have closed loop control using load cells to measure the tendon force. In this way, rather than controlling the position of the tendons, forces on each of the tendons are controlled to actuate the robot.

This GitHub Repository contains all of the necessary files for replicating and futher modifying this soft robot. The whole setup was designed with modularity in mind so that different parts can be tested easily. Most of the parts picked for this setup are commonly available like standard sized machine screws and 20x20 aluminium profiles, while the rest of the parts were designed with 3D printing in mind. All of printed parts that you will see in the pictures here, have been printed in PLA/PLA+.

This GitHub Repository currently contains these things:
1. Actuators - DIY version which can be soldered on a perfboard as well as a PCB designed for the V2 version, all of the necessary files are available to replicate
2. Main Assembly - This includes the aluminium frame, mold for casting the silicone robot as well as the 3D printed parts that connect the silicone to the rest of the robot
3. Test Stand - A small setup used for testing the actuator performance before installing the actuators on the robot
4. Testing - Pictures and videos of testing the robot

Besides this GitHub Repository, there is a Google Sheet document that contains the complete BOM for the actuator V2 PCB as well as a lot od additional data, like the MCU pinout, API commands, Errata and this document will be further updated as the project progresses further. The link to the Google Sheet document can be found here: https://docs.google.com/spreadsheets/d/1j4l9SDbZnH_yYO-BC3xJZ4RkYJjM1L0_M2_zAHDFm0k/edit?gid=212558158#gid=212558158

## Actuators

Both versions of the actuator are based around the same motor: https://www.pololu.com/product/1442
The actuators main difference is in the electronics that controls them, the V1 is hand soldered and one MCU controls 2 motors, while the V2 has custom designed PCB that only controls one motor. To get the force feedback control, load cells are used to measure the tendon tension, instead of using something like the HX711 which can do a maximum sample rate of 80Hz, an instrumentation amplifier is used to get the sampling rate above 1kHz. Both versions of the actuator have been tested with the INA125P instrumentaion amplifier, while the V2 also has a smaller INA826 instrumentation amplifier which is uses (the INA125P was put as backup on the PCB). This requires a test procedure to calibrate the actuators, and whole procedure can be found in the main Google Sheet.

For the purposes of calibrating and testing the actuators, a test stand has been designed, that can be seen in the picture below.

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Test%20Stand/Pictures/IMG_20231217_222425632~2.jpg" width="400">

This test stand consists of a load cell connected to the HX711 amplifier (24bit sigma-delta amplifier usually used with load cells) that is used for getting the absolute values, while the setup is calibrated. The V2 actuator has a connector on the PCB for this calibration test stand. After calibrating and checking the linearity of the readings, the actuator controller can be tuned and tested with reference waveforms for force. While the tuning and algorithms aren't final yet, some of the results can be seen in the 3 pictures below.

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Actuators/Actuator%20V2/Pictures/Simple.png" width="700">

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Actuators/Actuator%20V2/Pictures/Sawtooth.png" width="700">

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Actuators/Actuator%20V2/Pictures/Sine.png" width="700">

A picture of the newest V2 actuator can be seen below.

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Actuators/Actuator%20V2/Pictures/IMG_20231217_222207981~2.jpg" width="700">

## Frame

The frame has been designed out of aluminium 20x20 extrusions since they are available everywhere and easy to work with. The angle at which the robot is in its starting position can be changed from vertical to horizontal for different testing scenarios. That can be seen in the pictures below.

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Testing/Pictures/Outlook-Position0D.png" width="500">

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Testing/Pictures/Outlook-Position45.png" width="500">

## Silicone

The silicone used in this setup is Mold Max 30: https://www.smooth-on.com/products/mold-max-30/

To cast the robot a custom mold was used (all of the CAD files for it can be found in this repository), one thing to note with the silicone casting process is the importance of using 1mm thick rods that leave the channels for threading the tendons through later on. Below are the picture of the mold as well as of the cast silicone.

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Main%20Assembly/Silicone%20Mold/Pictures/SiliconeMold.png" width="400">

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Main%20Assembly/Silicone%20Mold/Pictures/IMG-20230909-WA0010.jpeg" width="400">

## Mounting the Silicone

To mount the silicone to the rest of the robot, there are 2 essential parts, one is the universal plate which has anchors to attach to the silicone, and the other one is the robot mount plate which sits on the aluminium frame. This follows the principle of keeping everything modular with this setup, so that different continuum shapes can easily be tested on this setup, just by using the same kind of universal plate during the casting process. Pictures of the universal plate and robot mount can be seen below.

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Main%20Assembly/Silicone%20Mounting/Pictures/UniversalPlate.png" width="400">

<img src="https://github.com/MilosRasic98/SoftRobotETF/blob/main/Main%20Assembly/Silicone%20Mounting/Pictures/RobotMountingPlate.png" width="400">

All of the codes, CAD files as well as Gerber files for ordering the PCBs can be found in the folders in this repository.
