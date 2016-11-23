//============================================================================
// Name        : RPi_GPS.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "RPi_GPS.h"
using namespace std;



// Class Function Definitions
USART_GPS::USART_GPS(){

	GPSdata_clear();
	UART_SETUP();

}

void USART_GPS::GPSdata_clear(){


		for(int i = 0; i < MSG_MAX_SIZE; i++){
			rx_GPGGA[i] = ZERO;
			rx_GPGSA[i] = ZERO;
			rx_GPRMC[i] = ZERO;
			for(int j = 0; j < 4; j++){rx_GPGSV[j][i] = ZERO;}

		}

		Horiz_Dilution 	= 0.0;
		Elevation 		= 0.0;
		HeightOfGeoid 	= 0.0;
		Speed			= 0.0;
		GndSpdKts		= SPEED_INIT;
		GndSpdKph		= 0.0;
		TrackAngle		= 0.0;
		MagneticVariation = 0.0;
		MagVar_Direction  = ' ';

		date_and_time.timeHH = 0;
		date_and_time.timeMM = 0;
		date_and_time.timeSS = 0;
		date_and_time.day 	 = 0;
		date_and_time.month  = 0;
		date_and_time.year	 = 0;

		NS = '_';
		EW = '_';

		latitude.Lat_deg = latitude.Lat_min = latitude.Lat_deciMin 		= 0;
		longitude.Long_deg = longitude.Long_min = longitude.Long_deciMin 	= 0;

}

void USART_GPS::GPSdata_clear(char* data, int limit){

	for(int i = 0; i < limit; i++){
	*(data + i) = ZERO;
	}
}

void USART_GPS::UART_SETUP(){
	//-------------------------
	//----- SETUP USART 0 -----
	//-------------------------
	//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively

	//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	uart0_filestream = -1;
	uart0_filestream = open(USB_UART, O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
		if (uart0_filestream == -1)
		{
			//ERROR - CAN'T OPEN SERIAL PORT
			printf(ERR_OPEN_UART);
			//exit(1);

		}

	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)

		struct termios options;
		tcgetattr(uart0_filestream, &options);
		options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
		options.c_iflag = IGNPAR;
		options.c_oflag = 0;
		options.c_lflag = 0;
		tcflush(uart0_filestream, TCIFLUSH);
		tcsetattr(uart0_filestream, TCSANOW, &options);

}

void USART_GPS::GPS_get(){

	// for testing only
	/*const char tx_msg[] =
			  "0000*6F\r\n"
			  "$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n"
			 // "$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
			 // "$GPSGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
			 // "$GPGSV,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
			 // "$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A*7D\r\n"
			  "$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
			  "$GPGSA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
			  "$GPGSV,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
			  "$GPGSV,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
			  "$GPGSV,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
			  "$GPGSV,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
			  "$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A*7D\r\n"
			  "$GPGGA,045252.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n";
*/

	char rx_buffer[1] = "";
	char* p_rx_msg = rx_GPGGA;
	int msg_cntr = 0;
	int MSG_flag = -1;

	GPSdata_clear();

	/* No message = -1
	 * Start of string = 0
	 * GPGGA = 1
	 * GPGSA = 2
	 * GPGSV1= 3
	 * GPGSV2= 4
	 * GPGSV3= 5
	 * GPGSV4= 6
	 * GPRMC = 7
	 * GPVTG = 8
	 * End of msg = 9
	 * */

// If any characters
if(uart0_filestream != (int)-1){

	while((MSG_flag < 9)) // while message not complete
	{
	    //rx_buffer[0] = tx_msg[tx_cntr]; // for test only
	    if(read(uart0_filestream, (void*)rx_buffer, 1) > 0){

	    if(rx_buffer[0] == '$' && MSG_flag == -1){
	        MSG_flag = 0;
	        msg_cntr = 0;
	    }

	    if(MSG_flag >= 0){
	        *(p_rx_msg + msg_cntr) = rx_buffer[0];

	        if(msg_cntr == 5 && MSG_flag >= 0){

	        	if(strcmp(p_rx_msg, CONST_GPGGA) == 0){
	        		MSG_flag = 1;
	        	}else{
	        		if(MSG_flag == 0){
	        			MSG_flag = -1;
	        		    msg_cntr = -1;
	        		}
	        	}
	        	if(strcmp(p_rx_msg, CONST_GPRMC) == 0 && MSG_flag >= 2){
	        		MSG_flag = 7;
	        		GPSdata_clear(p_rx_msg, MSG_MAX_SIZE);
	        		p_rx_msg = rx_GPRMC;
	        		strcpy(p_rx_msg, CONST_GPRMC);
	        	}



	        	if(strcmp(p_rx_msg, CONST_GPVTG) == 0 && MSG_flag >= 2){
	        		MSG_flag = 8;
	        		GPSdata_clear(p_rx_msg, MSG_MAX_SIZE);
	        		p_rx_msg = rx_GPVTG;
	        		strcpy(p_rx_msg, CONST_GPVTG);
	        	}


	    }//end if

	        if(rx_buffer[0] == '\n' && MSG_flag > 0){
	        	MSG_flag++;

	        		        switch(MSG_flag){
	        		          case 1:
	        		        	  p_rx_msg = rx_GPGGA;
	        		          break;

	        		          case 2:
	        		        	  p_rx_msg = rx_GPGSA;
	        		          break;

	        		          case 3:
	        		          case 4:
	        		          case 5:
	        		          case 6:
	        		        	  p_rx_msg = rx_GPGSV[MSG_flag - 3];
	        		          break;

	        		          case 7:
	        		        	  p_rx_msg = rx_GPRMC;
	        		          break;

	        		          case 8:
	        		           	  p_rx_msg = rx_GPVTG;
	        		          break;

	        		          default:
	        		        	  //p_rx_msg =
	        		          break;

	        		        }

	        	msg_cntr = -1;
	        }//end if

	        msg_cntr++;
	    	}// end if flag > 0
	      } // end if
    	}//end while

	GPSdata_parseGPGGA();
	GPSdata_parseGPRMC();
	GPSdata_parseGPVTG();

  } // end if

}// end GPS_get()

void USART_GPS::print_raw_rx_message(){

	printw("The rx_GPGGA  is:%s"
		   "The rx_GPGSA  is:%s"
		   "The rx_GPGSV1 is:%s"
		   "The rx_GPGSV2 is:%s"
		   "The rx_GPGSV3 is:%s"
		   "The rx_GPGSV4 is:%s"
		   "The rx_GPRMC  is:%s"
		   "The rx_GPVTG  is:%s\r", rx_GPGGA, rx_GPGSA, rx_GPGSV[0], rx_GPGSV[1], rx_GPGSV[2], rx_GPGSV[3], rx_GPRMC, rx_GPVTG);

}

/*  $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47

Where:
     GGA          Global Positioning System Fix Data
     123519       Fix taken at 12:35:19 UTC
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     1            Fix quality: 0 = invalid
                               1 = GPS fix (SPS)
                               2 = DGPS fix
                               3 = PPS fix
			       4 = Real Time Kinematic
			       5 = Float RTK
                               6 = estimated (dead reckoning) (2.3 feature)
			       7 = Manual input mode
			       8 = Simulation mode
     08           Number of satellites being tracked
     0.9          Horizontal dilution of position
     545.4,M      Altitude, Meters, above mean sea level
     46.9,M       Height of geoid (mean sea level) above WGS84
                      ellipsoid
     (empty field) time in seconds since last DGPS update
     (empty field) DGPS station ID number
     *47          the checksum data, always begins with */
void USART_GPS::GPSdata_parseGPGGA(){

int Gcntr = 0;
int Lcntr = 0;
int message = 0;
char current = ' ';
char loc_Time[6];
char loc_Latitude[COORD_MSG_SIZE];
char loc_Longitude[COORD_MSG_SIZE];
char loc_Horiz_Dilution[5];
char loc_Elevation[COORD_MSG_SIZE];
char loc_fixQual;
char loc_numberOfSat[2];
char loc_GeoidHeight[7];
char dummy[COORD_MSG_SIZE];
char* p_msg;
//char t_rx_GPGGA[] = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";


GPSdata_clear(loc_Time, 6);
GPSdata_clear(loc_Latitude, COORD_MSG_SIZE);
GPSdata_clear(loc_Longitude, COORD_MSG_SIZE);
GPSdata_clear(loc_Horiz_Dilution, 5);
GPSdata_clear(loc_Elevation, COORD_MSG_SIZE);
GPSdata_clear(loc_GeoidHeight, 7);

if(rx_GPGGA[0] != '\0'){
//if(t_rx_GPGGA[0] != '\0'){

	while(current != '*'){

		switch(message){
				case 0:
					p_msg = loc_Time;
					break;
				case 1:
					p_msg = loc_Latitude;
					break;
				case 2:
					p_msg = &NS;
					break;
				case 3:
					p_msg = loc_Longitude;
					break;
				case 4:
					p_msg = &EW;
					break;
				case 5:
					p_msg = &loc_fixQual;
					break;
				case 6:
					p_msg = loc_numberOfSat;
					break;
				case 7:
					p_msg = loc_Horiz_Dilution;
					break;
				case 8:
					p_msg = loc_Elevation;
					break;
				case 9:
					p_msg = dummy;
					break;
				case 10:
					p_msg = loc_GeoidHeight;
					break;
				case 11:
					p_msg = dummy;
					break;

				default:
					p_msg = dummy;
					break;
				}

		do{
			current = rx_GPGGA[Gcntr+7];
			Gcntr++;
			if(current == ','){
						message++;
						Lcntr = 0;
			}else{
			*(p_msg+Lcntr) = current;
			Lcntr++;

			}

		}while((current != ',') && (current != '*'));//end while

	}// end while

// Convert the parsed date into numbers
	date_and_time.timeHH = charToInt(loc_Time[0])*10 + charToInt(loc_Time[1]);
	date_and_time.timeMM = charToInt(loc_Time[2])*10 + charToInt(loc_Time[3]);
	date_and_time.timeSS = charToInt(loc_Time[4])*10 + charToInt(loc_Time[5]);

	latitude.Lat_deg 		= charToInt(loc_Latitude[0]) * 10 + charToInt(loc_Latitude[1]);
	latitude.Lat_min 		= charToInt(loc_Latitude[2]) * 10 + charToInt(loc_Latitude[3]);
	latitude.Lat_deciMin 	= charToInt(loc_Latitude[5]) * 1000 + charToInt(loc_Latitude[6]) * 100 + charToInt(loc_Latitude[7]) * 10 + charToInt(loc_Latitude[8]);

	longitude.Long_deg 		= charToInt(loc_Longitude[0])*100 + charToInt(loc_Longitude[1])*10 + charToInt(loc_Longitude[2]);
	longitude.Long_min 		= charToInt(loc_Longitude[3])*10 + charToInt(loc_Longitude[4]);
	longitude.Long_deciMin 	= charToInt(loc_Longitude[6])*1000 + charToInt(loc_Longitude[7])*100 + charToInt(loc_Longitude[8])* 10 + charToInt(loc_Longitude[9]);

	Horiz_Dilution = StrToFloat(loc_Horiz_Dilution);
	Elevation = StrToFloat(loc_Elevation);
	HeightOfGeoid = StrToFloat(loc_GeoidHeight);



//printf("Geoid Height: \t%s\n", GeoidHeight);


   }// end if

}// end fnc

/*$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

Where:
     RMC          Recommended Minimum sentence C
     123519       Fix taken at 12:35:19 UTC
     A            Status A=active or V=Void.
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     022.4        Speed over the ground in knots
     084.4        Track angle in degrees True
     230394       Date - 23rd of March 1994
     003.1,W      Magnetic Variation
     *6A          The checksum data, always begins with **/
void USART_GPS::GPSdata_parseGPRMC(){

	//char t_rx_GPRMC[] = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A\r\n";

	int Gcntr = 0;
	int Lcntr = 0;
	int message = 0;
	char current = ' ';
	char loc_Time[6];
	char loc_Status = ' ';
	char loc_Latitude[COORD_MSG_SIZE];
	char loc_Longitude[COORD_MSG_SIZE];
	char loc_Speed[6];
	char loc_TrackAng[6];
	char loc_Date[6];
	char loc_MagneticVariation[6];
	char loc_MagVar_Direction = ' ';
	char disposal[COORD_MSG_SIZE];
	char* p_msg;

	GPSdata_clear(loc_Latitude, COORD_MSG_SIZE);
	GPSdata_clear(loc_Longitude, COORD_MSG_SIZE);
	GPSdata_clear(loc_Speed, 6);
	GPSdata_clear(loc_TrackAng, 6);
	GPSdata_clear(loc_Date, 6);
	GPSdata_clear(loc_MagneticVariation, 6);
	GPSdata_clear(disposal, COORD_MSG_SIZE);

	if(rx_GPRMC[0] != '\0'){

		while(current != '*'){

			switch(message){
					case 0:
						p_msg = loc_Time;
						break;
					case 1:
						p_msg = &loc_Status;
						break;
					case 2:
						p_msg = loc_Latitude;
						break;
					case 3:
						p_msg = disposal;
						break;
					case 4:
						p_msg = loc_Longitude;
						break;
					case 5:
						p_msg = disposal;
						break;
					case 6:
						p_msg = loc_Speed;
						break;
					case 7:
						p_msg = loc_TrackAng;
						break;
					case 8:
						p_msg = loc_Date;
						break;
					case 9:
						p_msg = loc_MagneticVariation;
						break;
					case 10:
						p_msg = &loc_MagVar_Direction;
						break;

					default:
						p_msg = disposal;
						break;
					}

			do{

				current = rx_GPRMC[Gcntr+7];
				Gcntr++;

				if(current == ','){
							message++;
							Lcntr = 0;
				}else{
				*(p_msg+Lcntr) = current;
				Lcntr++;

				}

			}while((current != ',') && (current != '*'));//end while

		}// end while
	}// end if


	// Convert Acquired Data to Numbers
	Speed = StrToFloat(loc_Speed);
	TrackAngle = StrToFloat(loc_TrackAng);
	MagneticVariation = StrToFloat(loc_MagneticVariation);
	MagVar_Direction = loc_MagVar_Direction;

	date_and_time.day = charToInt(loc_Date[0])*10 + charToInt(loc_Date[1]);
	date_and_time.month = charToInt(loc_Date[2])*10 + charToInt(loc_Date[3]);
	date_and_time.year = charToInt(loc_Date[4])*10 + charToInt(loc_Date[5]);



}//end fnc

/*$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48

where:
        VTG          Track made good and ground speed
        054.7,T      True track made good (degrees)
        034.4,M      Magnetic track made good
        005.5,N      Ground speed, knots
        010.2,K      Ground speed, Kilometers per hour
        *48          Checksum
*/
void USART_GPS::GPSdata_parseGPVTG(){
		int Gcntr = 0;
		int Lcntr = 0;
		int message = 0;
		char current = ' ';
		char loc_TrueTrack[6];
		char loc_MagTrack[6];
		char loc_GndSpeedKts[6];
		char loc_GndSpeedKph[6];

		char disposal[COORD_MSG_SIZE];
		char* p_msg;

		GPSdata_clear(loc_TrueTrack, 6);
		GPSdata_clear(loc_MagTrack, 6);
		GPSdata_clear(loc_GndSpeedKts, 6);
		GPSdata_clear(loc_GndSpeedKph, 6);
		GPSdata_clear(disposal, COORD_MSG_SIZE);

		if(rx_GPVTG[0] != '\0'){

			while(current != '*'){

				switch(message){
						case 0:
							p_msg = loc_TrueTrack;
							break;
						case 1:
							p_msg = disposal;
							break;
						case 2:
							p_msg = loc_MagTrack;
							break;
						case 3:
							p_msg = disposal;
							break;
						case 4:
							p_msg = loc_GndSpeedKts;
							break;
						case 5:
							p_msg = disposal;
							break;
						case 6:
							p_msg = loc_GndSpeedKph;
							break;
						default:
							p_msg = disposal;
							break;
						}

				do{

					current = rx_GPVTG[Gcntr+7];
					Gcntr++;

					if(current == ','){
								message++;
								Lcntr = 0;
					}else{
					*(p_msg+Lcntr) = current;
					Lcntr++;

					}

				}while((current != ',') && (current != '*'));//end while

			}// end while
		}// end if


		// Convert Acquired Data to Numbers
		GndSpdKts = StrToFloat(loc_GndSpeedKts);
		GndSpdKph = StrToFloat(loc_GndSpeedKph);



}//end fnc

void USART_GPS::GPSdata_display(){

	printf("Raw GPGGA data: %s\n", rx_GPGGA);
	printf("Raw GPRMC data: %s\n", rx_GPRMC);
	//printf("Date: \t\t   %c%c.%c%c.%c%c\n", Date[0], Date[1], Date[2], Date[3], Date[4], Date[5]);
	printf("Time: \t\t   %02i:%02i:%02i\n", date_and_time.timeHH, date_and_time.timeMM, date_and_time.timeSS);
	printf("Latitude:  \t %3i' %02i.%03i\", %c\n", latitude.Lat_deg, latitude.Lat_min, latitude.Lat_deciMin, NS);
	printf("Longitude: \t %3i' %02i.%03i\", %c\n", longitude.Long_deg, longitude.Long_min, longitude.Long_deciMin, EW);
	printf("H. Dilution: \t  %4.1f\n", Horiz_Dilution);
	printf("Altitude:  \t %5.1f\n", Elevation);
	printf("Height of Geoid: %5.1f\n", HeightOfGeoid);
	printf("Speed: \t\t %5.1f\n", Speed);
	printf("Track Angle: \t %5.1f\n\n\n", TrackAngle);

}

void USART_GPS::GPSdata_w_print(){

		printw("\nDate: \t\t %02i.%02i.20%02i @ ", date_and_time.day, date_and_time.month, date_and_time.year);
		printw("%02i:%02i:%02i\n", date_and_time.timeHH, date_and_time.timeMM, date_and_time.timeSS);
		printw("Latitude: \t %3i' %02i.%03i\", %c\n", latitude.Lat_deg, latitude.Lat_min, latitude.Lat_deciMin, NS);
		printw("Longitude:\t %3i' %02i.%03i\", %c\n", longitude.Long_deg, longitude.Long_min, longitude.Long_deciMin, EW);
		printw("H. Dilution:\t  %4.1f\n", Horiz_Dilution);
		printw("Altitude:\t %5.1f\n", Elevation);
		//printw("Height of Geoid: %5.1f\n", HeightOfGeoid);
		printw("Speed (Kts) :\t %5.1f\n", GndSpdKts);
		printw("Speed (Km/h):\t %5.1f\n", GndSpdKph);
		printw("Track Angle:\t %5.1f\n\n\n", TrackAngle);

}//end fnc

void USART_GPS::GPSdata_showSpeed(float speed, string* unit){

	string row;

	int hundreds= 0;
	int tens 	= 0;
	int ones 	= 0;
	int tenths 	= 0;

hundreds = (int)(speed/100);
tens 	 = (int)(speed - hundreds*100) / 10;
ones 	 = (int)(speed - hundreds*100 - tens*10);
tenths 	 = (int)floor((speed - hundreds*100 - tens*10 - ones)*10);




	for(int i = 0; i < MAX_COLUMNS; i++){
		row = (string)*(NUMBERS(hundreds)+i) + (string)*(NUMBERS(tens)+i) + (string)*(NUMBERS(ones)+i) + (string)decimal[i] + (string)*(NUMBERS(tenths)+i) + (string)*(unit +i);
		printw("%s\n", row.c_str());


	}


}

float USART_GPS::get_GndSpdKts(){return GndSpdKts;}

float USART_GPS::get_GndSpdKph(){return GndSpdKph;}

int USART_GPS::charToInt(char ch_number){ return (int)(atoi(&ch_number));}

float USART_GPS::StrToFloat(char* ch_number){return atof(ch_number);}

string* USART_GPS::NUMBERS(int number){
	string* NUMBER;

	switch( number ){
	case 0:
		NUMBER = zero;
		break;
	case 1:
		NUMBER = one;
		break;
	case 2:
		NUMBER = two;
		break;
	case 3:
		NUMBER = three;
		break;
	case 4:
		NUMBER = four;
		break;
	case 5:
		NUMBER = five;
		break;
	case 6:
		NUMBER = six;
		break;
	case 7:
		NUMBER = seven;
		break;
	case 8:
		NUMBER = eight;
		break;
	case 9:
		NUMBER = nine;
		break;
	default:
		break;


	}

return NUMBER;
}


