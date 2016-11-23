//============================================================================
// Name        : RPi_GPS.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


//============================================================================
#ifndef _PRi_GPS_h_
#define _RPi_GPS_h_

//#include <stdio>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <math.h>
#include <curses.h>
#include <stdlib.h>
#include "Numbers.h"

using namespace std;

//#include "TinyGPS++.h"

#define DEFAULT_UART "/dev/ttyAMA0" // This is the RPi internal
#define USB_UART "/dev/ttyUSB0" // USB UART adapter initializes to this
#define SPEED_INIT 367.8
#define ERR_OPEN_UART "Error - Unable to open UART. Ensure your device is connected and not in use by another application\n"

#define CONST_GPGGA "$GPGGA"
#define CONST_GPGSA "$GPGSA"
#define CONST_GPGSV "$GPGSV"
#define CONST_GPRMC "$GPRMC"
#define CONST_GPVTG "$GPVTG"

#define MSG_MAX_SIZE 100
#define COORD_MSG_SIZE 10
#define ZERO '\0'


class USART_GPS{

public:
      void print_raw_rx_message();
      void GPSdata_clear();
      void GPSdata_clear(char* data, int limit);
      void GPS_get();
      void GPSdata_display();
      void GPSdata_showSpeed(float speed, string* unit);
      void GPSdata_w_print();

      float get_GndSpdKts();
      float get_GndSpdKph();



      USART_GPS();
    //  USART_GPS(char* UART_Filename, int baud); // for future use

private:

      void UART_SETUP();
      void GPSdata_parseGPGGA();
      void GPSdata_parseGPRMC();
      void GPSdata_parseGPVTG();
      string* NUMBERS(int number);

      int charToInt(char ch_number);
      float StrToFloat(char* ch_height);

      int uart0_filestream;
      //char rx_message[400]; // Unparsed message received from GPS module
      char rx_GPGGA[MSG_MAX_SIZE];
      char rx_GPGSA[MSG_MAX_SIZE];
      char rx_GPGSV[4][MSG_MAX_SIZE];
      char rx_GPRMC[MSG_MAX_SIZE];
      char rx_GPVTG[MSG_MAX_SIZE];


      struct Latitude{
    	  int Lat_deg;
    	  int Lat_min;
    	  int Lat_deciMin;
      }latitude;   // Current Latitude

      struct Longitude{
    	  int Long_deg;
    	  int Long_min;
    	  int Long_deciMin;
      }longitude;  // Current Longitude

      struct DateAndTime{
    	  int timeHH;
    	  int timeMM;
    	  int timeSS;

    	  int day;
    	  int month;
    	  int year;

      }date_and_time;

      float Horiz_Dilution;
      float Elevation;  // Current Elevation above mean sea level
      float HeightOfGeoid;
      float Speed;
      float GndSpdKts;
      float GndSpdKph;
      float TrackAngle;
      float MagneticVariation;
      char  MagVar_Direction;



      char NS;
      char EW;
};


#endif
