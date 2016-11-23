#include "RPi_GPS.h"
using namespace std;

int main() {


	USART_GPS myGPS;


   initscr(); // initialize Ncurses
   noecho();
   nodelay(stdscr, TRUE);
   curs_set(0);

   while(getch() != 'q'){

	   	   	myGPS.GPS_get();
   			myGPS.GPSdata_showSpeed(myGPS.get_GndSpdKts(), Kts);
   			myGPS.GPSdata_w_print();
   			refresh();
   			wmove(stdscr, 0, 0);

   	    }//end while


   endwin();


return 0;
}







