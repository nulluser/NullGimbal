/*
    Gimbal Tracker

    (C) 2019 nulluser # gmail.com

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

#define MODULE_SERIAL

#include <windows.h>
#include <stdio.h>

#include "utility.h"
#include "serial.h"

// Serial Constructor
Serial::Serial(const char *pn, unsigned int _baud)
{
    console(MODULE "Constructor\n");

    serial_port = new char [strlen(pn) + 1];
    strcpy(serial_port, pn);

    baud = _baud;

    online = false;
}

// Serial Destructor
Serial :: ~Serial(void)
{
    console(MODULE "Destructor\n");
}

// Starts the serial port
void Serial::start(void)
{
	console(MODULE "Start\n");

	online = false;

	char serial_tmp[sizeof(serial_port) + 10];

	sprintf(serial_tmp, "\\\\.\\%s", serial_port);

	h_serial = CreateFile(serial_tmp,						// Port name
			 			  GENERIC_READ | GENERIC_WRITE,		// File access
		                  0,								// Share mode
			 			  NULL,								// Security
			 			  OPEN_ALWAYS,						// Creation
			 			  0,								// Flags
						  NULL );


	// Make sure port was opened
	if (h_serial == INVALID_HANDLE_VALUE)
	{
        console(MODULE "Unable to open serial port (%s)\n", serial_port);
		return;
	}

	// Try to setup buffer sizes. Non fatal
    if(!SetupComm(h_serial, SERIAL_BUFF_SIZE, SERIAL_BUFF_SIZE))
    {
		console(MODULE "Unable to set buffers\n");
		//return;
    }

    // Get current comm state
	DCB serial_param;

	if (!GetCommState(h_serial, &serial_param))
	{
		console(MODULE "Unable to get commstate\n");
		return;
	}

	// Set Parameters
    serial_param.BaudRate = baud;
	serial_param.fParity = false;
    serial_param.fBinary  = true;
	serial_param.fOutxCtsFlow = false;
	serial_param.fOutxDsrFlow = false;
	serial_param.fDtrControl = DTR_CONTROL_DISABLE;
    serial_param.fDsrSensitivity = false;
    serial_param.fErrorChar = false;
    serial_param.fNull = false;
	serial_param.fOutX = false;
	serial_param.fInX = false;
    serial_param.fRtsControl = RTS_CONTROL_DISABLE;//	// Disable request to send control
    serial_param.Parity = NOPARITY;
    serial_param.ByteSize = 8;
	serial_param.StopBits = ONESTOPBIT;

	// Write parameters
	if (!SetCommState(h_serial, &serial_param))
	{
		console(MODULE "Unable to set commstate\n");
		return;
	}


    // Setup timouts
	COMMTIMEOUTS timeouts;

	timeouts.ReadIntervalTimeout = SERIAL_READ_TIMEOUT;

	timeouts.ReadTotalTimeoutMultiplier = SERIAL_BYTE_MULT;
	timeouts.ReadTotalTimeoutConstant = SERIAL_BYTE_TIMEOUT;
	timeouts.WriteTotalTimeoutMultiplier = SERIAL_BYTE_MULT;
	timeouts.WriteTotalTimeoutConstant = SERIAL_BYTE_TIMEOUT;

	if (!SetCommTimeouts(h_serial, &timeouts))
	{
		console(MODULE "Unable to get comm timeouts\n");
		return;
	}

    online = true;              // Open ok

    flush();

	console(MODULE "Online\n");
}


// Close the serial port
void Serial::stop(void)
{
	console(MODULE "Stop\n");

    if (h_serial)
    	CloseHandle(h_serial);

    console(MODULE "Stop Complete\n");
}

// Read into buffer
int Serial::read(unsigned char * buff, unsigned len)
{
    if (!online) return 0;

    DWORD num_read = 0;

    if (!ReadFile(h_serial, buff, len, &num_read, NULL))
    {
        console(MODULE "ReadFile error\n");
        return(0);
    }

    if (num_read != len)
    {
        console(MODULE "Read Timeout\n");
        return(0);
    }

    return num_read;
}


// Send a buffer over serial
int Serial::write(unsigned char *buff, unsigned int len)
{
    if (!online) return 0;

    DWORD num_written = 0;

    if (!WriteFile(h_serial, buff, len, &num_written, NULL))
    {
        console(MODULE "WriteFile error\n");
        return(0);
    }

    return num_written;
}



// Clear data from port for a time period
void Serial::flush(void)
{
    console(MODULE "Serial Flush\n");

    if (!online) return;

    double start = get_time();

    while(get_time() - start < FLUSH_TIME)
    {// Flush buffer
        unsigned char buff[1];

        DWORD num_read = 1;
        ReadFile(h_serial, buff, 1, &num_read, NULL);

        if (num_read > 0)
            log(MODULE "Flush %x\n", buff[0]);
    }
}


/* Validate serial packet */
#if 0
int Serial :: validate_packet(unsigned char *buff, unsigned int len)
{
  /*  int c = get_checksum(buff, len-1);

    if (c != buff[len-1])
    {
        add_line("Invalid packet checksum actual: %02x  Expected: %02x", buff[len-1], c);
        show_buffer("Buffer: ", buff, len, 1);

        return(FAIL);
    }

    return(SUCCESS);*/
    return 0;
}
#endif
/* End of validate packet */


/* Process serial packet */
#if 0
int Serial :: process_packet( unsigned char *buff, unsigned int len )
{

//    else
    {
       // engine.set_telemetry(buff);
       // engine.update_complete(0);
    }

    return(0);
}
#endif
/* End of process packet */





/* Send a packet over serial to the controller */
// send_bytes is the payload size
#if 0
int Serial :: serial_packet(unsigned char packet_type, unsigned char *send_buff, int send_bytes)
{
//    console("Send packet");

    if (send_bytes <= 0) return(0);

    unsigned char tx_packet[send_bytes+3];


    tx_packet[0] = send_bytes+3;
    tx_packet[1] = packet_type;

    memcpy(tx_packet+2, send_buff, send_bytes);

    tx_packet[send_bytes+2] = 0;//get_checksum (tx_packet, send_bytes+1);

    if (serial_send (tx_packet, send_bytes+3) == -1)
        return(1);

    return(0);
}
#endif
/* End of serial_transfer */


/* Clear comm buffers and wait for board to reset */
#if 0
int Serial :: check_flush( void )
{
    static unsigned char buff[1];


    DWORD num_read;

    if (serial_flush == 0)
        return (0);

    // First time in this flush
    // Setup flush to take 1.5 seconds
    if (flush_count == 0)
    {
        flush_count = 1500 / SERIAL_BYTE_TIMEOUT;
        console(MODULE "Start flush\n");
    }

    // See if we are still flushing
    if (--flush_count > 0)
    {
        num_read = 1;               // Prime loop

        while(num_read > 0)         // Flush buffer
            ReadFile(h_serial, buff, 1, &num_read, NULL);
    }
    else
    {
        serial_flush = 0;
        console(MODULE "End flush\n");
    }

    return (1);
}
#endif
/* End of flush */







//    const int RX_MAX = 256;


//char serial_rx[RX_MAX];
//int rx_index = 0;



/* Read and process serial data */
#if 0
int Serial ::read_packet( void )
{

    char buff[RX_MAX];


    DWORD num_read = 0;

  /*  if (ReadFile(h_serial, buff, RX_MAX-1, &num_read, NULL) != 1)
    {
        console(MODULE "ReadFile error\n");
        return(1);
    }*/

    if (num_read == 0)
    {
        //console(MODULE "Read Timeout\n");
        return(1);
    }

    unsigned int index = 0;


    while(index < num_read)
    {




    char ch = buff[index++];


    if (ch == '\n')
    {
        serial_rx[rx_index] = 0;
        rx_index = 0;

        console("Serial in: (%s)\n", serial_rx);
    } else
    {
console("rx %c\n", ch);
        serial_rx[rx_index++] = ch;

        if (rx_index > RX_MAX)
            rx_index = 0;

    }

}


//    engine.get_control(buff);

   /* unsigned char packet_type = COMM_NI_CTRL;

    if(serial_packet(packet_type, buff, CONTROL_SIZE))
    {
        add_line(MODULE "Unable to send control packet");
        return(FAIL);
    }*/

  /*  DWORD num_read;

// Get packet length
    unsigned len = 0;

    if (serial_read( buff, 1) )
    {
        add_line(MODULE "Read Timeout");
        return(FAIL);
    }

    len = buff[0];


    // read rest of packet
    if (serial_read( buff+1, len-1) == FAIL)
    {
        add_line(MODULE "Size Mismatch");

        return(FAIL);

    }


    if (validate_packet(buff, len) == FAIL)
    {
        // Force the controller to timeout and flush remaining
        //flush();

        add_line(MODULE "Unable to validate packet");
        return(1);
    }

//   show_buffer("Read", buff, len, 1);

    if (process_packet(buff+2, len - 3) == 1)
   {
        add_line(MODULE "Process Packet Fail");
        return(1);
    }
*/
    return(0);
}

#endif
/* End of read_packet */


/* Serial thread for rx processing */
#if 0
DWORD WINAPI Serial :: serial_thread( void )
{
    log(MODULE "Enter Serial Thread\n");

//return 0;


    while(serial_running)
    {
        //check_flush();

        Sleep(10);


      //  read_packet();


//        /Sleep(1);
        /*// See if we are flushing
        /if (check_flush() == 0)
        {
            // Read packet normally
            if (read_packet() == FAIL)
            {
                serial_flush = 1;
                engine.reset_control ();
            }
        }*/
    }

    log(MODULE "Leave Serial Thread\n");

}
#endif
/* End of serial thread */


/* Wrapper for the serial thread */
/*DWORD WINAPI Serial::thread_wrapper(LPVOID pvoid)
{
    Serial *s = (Serial *) pvoid;

    s->serial_thread();

	return (DWORD)1;
}*/
/* End of serial_cam_class */




