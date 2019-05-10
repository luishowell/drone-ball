# Persistence-of-Vision Spherical LED Display

## Introduction

Recent years have seen a steep increase in the use of drone technology in both commercial and home uses. This rapid progression has created an innovation opportunity as different creative ways to use the technology are investigated. One such area of interest is the use of drones within display technology. Indeed, Intel have already developed a platform that uses many drones in conjunction with one another to create displays at various events. 
 
This project considers that instead of using many drones to create a large display, a single drone could be used to create a 3D sphere display.  Creating such a display for a drone poses a number of challenges, most notably that a permanent display cannot be used without disrupting the aerodynamics of the drone catastrophically. Different display methodologies must, therefore, be considered to allow such a display to be realised.
 
The aim of this project was to design a 3D persistence-of-vision display around a drone to achieve a constant display. Unfortunately, a funding request for this project was unsuccessful meaning an appropriate drone could not be purchased. In spite of this, the display was designed around a chosen drone.

## Electronic Design

The electronic design of the sphere was essential to effectively create the display. The overall electronic system diagram is shown below and contains:    

* LED Strips - Lengths of RGB LEDs to light the display.
* Microcontroller - Control the operation of the display.
* Stepper Motor and Driver - Used to spin the LED strips.
* Power Supply - Provide power to the display without outside connection.
* SD Card - Storage of images to be displayed.
* Bluetooth Module - Facilitate wireless control of the display.

![system diagram](https://github.com/luishowell/drone-ball/blob/master/images/system_diagram_small.PNG?raw=true)

## Android App

An android application was developed in order to control the sphere remotely. This application communicated with the sphere through a Bluetooth connection and allowed various aspects of the display to be controlled.

![app](https://github.com/luishowell/drone-ball/blob/master/images/app_small.png?raw=true)


## Mechanical Design

While the drone display includes several parts, they may be grouped as follows:

* Outer frame (carbon fibre and 3D printed connectors)
* LED hoops (aluminium strips and disk connectors)
* Central plate (MDF with mounted components)
* Gear system (OTS gears, bearings and carbon fibre central shaft)
* Central shaft (carbon fibre and 3D printed connectors)

![mech diagram](https://github.com/luishowell/drone-ball/blob/master/images/mech_diagram.png?raw=true)

![final construction](https://github.com/luishowell/drone-ball/blob/master/images/construction_img_small.png?raw=true)


## Results

* App is used to control all aspects of operation
* Persistence achieved, with position of coloured stripes preserved 
* Stepper motor operational up to around 200rpm

![while running](https://github.com/luishowell/drone-ball/blob/master/images/running_img_small.png?raw=true)

## Current Issues

* Significant vibration of outer frame
* Three LED strips used rather than four
* Small amount of slippage in the gears

## Creators

* Luis Howell 
* Stephanie Irvine
* Eve McGlynn 
* Joe Walker