# RPi_GPS
 A small GPS Tracker and speed indicator for RaspberryPi

# HARDWARE:
 - Raspberry Pi (this was tested with RPi 2 only...for now)
 - WaveShare 3" LCD Screen
 - Adafruit Ultimate GPS (Initially I've used UP-501 GPS Receiver but it was an older chip...times change and so should we!)

# HISTORY:
 This project was inspired by my desire to advance in sailing. I needed something that would tell me the speed since that's a good
 indicator of performance. I didn't want to buy an off-the-shelf device because what's the fun in that!? So, this project was 
 born. 

# SUMMARY
 This software is designed for use with a Raspberry Pi (2 in my case), but should run on any Linux box*. and the aforementioned 
 hardware. It will extract and display all the relevant (to me) GPS information and output your speed in Knots with large numbers.

'*' Please use "Any Linux Box" with caution as I have only tested this software on my PI and the Laptop. Your box must have a display 
  at least as big as specified in HARDWARE section.

# THE HOW-TO's

0. To run this software you will need "ncurses" library. 
```
sudo apt-get install libncurses-dev
```
Hit TAB after ...curses to get the latest package.

1. Download the .cpp and the .h files and place them all in the directory of your choosing.
  
2. Compile by opening Terminal and doing the following:
```
cd "path_to_your_folder"
g++ *.cpp -o myTinyGPS.out -lncurses
```
NOTE: You may substitude "myTinyGPS.out" with "any_name.out" - this will be your executable.

# To run:
```
./myTinyGPS.out
```
 To quit press 'q' (I'm working on this feature being available through the GPIO / buttons on the side of the screen)
