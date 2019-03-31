/*
	MicroDyno Interface
    (C) 2011 Jason Hunt nulluser@gmail.com

	This program is free software; you can redistribute it and/or
    	modify it under the terms of the GNU General Public License
    	as published by the Free Software Foundation; either version 2
    	of the License, or (at your option) any later version.

    	This program is distributed in the hope that it will be useful,
    	but WITHOUT ANY WARRANTY; without even the implied warranty of
    	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    	GNU General Public License for more details.

    	You should have received a copy of the GNU General Public License
    	along with this program; if not, write to the Free Software
    	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/


#ifndef SERIAL_H
#define SERIAL_H

// Private
#ifdef MODULE_SERIAL

#define MODULE "[Serial]   "

#define SERIAL_BUFF_SIZE 128        // Internal buffer size
#define SERIAL_READ_TIMEOUT 250     // Timeout
#define SERIAL_BYTE_TIMEOUT 100
#define SERIAL_BYTE_MULT    1

#define FLUSH_TIME  0.5         // Seconds for serial flush

#endif


/* Serial comm */
class Serial
{
  public:
    Serial( const char *pn, unsigned int _baud );
    ~Serial( void );

    void start( void );
    void stop( void );

    void set_port(const char *pn) { strcpy(serial_port, pn); };
    void flush(void);

    int read( unsigned char * buff, unsigned len);
    int write(unsigned char *buff, unsigned int len);

    bool get_online(void) { return online; };


  private:

    HANDLE h_serial;

    char *serial_port;                  // Filename of serial port
    unsigned int baud;                  // baud rate

    bool online;                        // True if serial is working
};

#endif

