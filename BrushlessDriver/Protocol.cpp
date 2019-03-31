/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: MotorDriver.h
*/


#define MODULE_PROTOCOL

#include <arduino.h>

#include "Config.h"
#include "Utility.h"
#include "Protocol.h"
#include "Controller.h"

Protocol::Protocol()
{
  rx_index = 0;
  rx_cmd = false;     
}

void Protocol::init(Controller &_controller)
{
  controller = &_controller;
}


// Process pending bytes
void Protocol::update()
{
  while (Serial.available() > 0)
    process_rx((byte) Serial.read());
}

// Process single byte
// Does not timeout for command entry. Should be fine > will restart cmd
void Protocol::process_rx(byte c)
{
  #ifdef DEBUG
  if(c == 'a') controller->move_gimbal( 0.5,  0.0);  else   // Roll Right
  if(c == 'w') controller->move_gimbal( 0.0,  0.5);  else    // Pitch Up
  if(c == 'd') controller->move_gimbal(-0.5,  0.0);  else   // Roll Left
  if(c == 's') controller->move_gimbal( 0.0, -0.5);  else    // Pitch Down
  if(c == ' ') controller->toggle_laser(); else    // Pitch Down
  return;
  #endif

  // Look for start value if we are not inside of a packet
  if (rx_cmd == false)
  {
    // Command Start
    if (c == 0xff) rx_cmd = true; 
    
    rx_index = 0; 
  }

  rx_buff[rx_index++] = c;
      
  if (rx_index >= GIMBAL_RX_LEN)
  {
    byte checksum = get_checksum(rx_buff, GIMBAL_RX_LEN-1);

    if (checksum == rx_buff[GIMBAL_RX_LEN-1])
    {
      parse_control(rx_buff); 
      send_telemetry();
    }
    rx_index = 0;
    rx_cmd = false;
  } 
   
}



void Protocol::send_telemetry(void)
{
  uint16_t dist   = controller->get_dist_raw();
  uint16_t signal = controller->get_signal_raw();
  uint16_t temp   = controller->get_temp_raw();
 
  // -1800 to 1800
  uint16_t ang_x  = 10.0 * controller->get_angle_x();
  uint16_t ang_y  = 10.0 * controller->get_angle_y();

  unsigned char tx_buffer[GIMBAL_TX_LEN];

  tx_buffer[0] = 0xff;

  set_word(tx_buffer, 1, ang_x);
  set_word(tx_buffer, 3, ang_y);
  set_word(tx_buffer, 5, dist);
  set_word(tx_buffer, 7, signal);
  set_word(tx_buffer, 9, temp);

  tx_buffer[GIMBAL_TX_LEN-1] = get_checksum(tx_buffer, GIMBAL_TX_LEN-1);

  Serial.write(tx_buffer, GIMBAL_TX_LEN);

  // for (int i = 0; i < 6; i++)
  //Serial.print(buff[i], HEX);
  //Serial.println("");
}


// Process update command
void Protocol::parse_control(byte * rx_data)
{
  // Byte 0 is 0xff header
  int x =     (rx_data[1] << 8) + rx_data[2];   //  Bytes 1 and 2 are X angle*100
  int y =     (rx_data[3] << 8) + rx_data[4];   //  Bytes 3 and 4 are Y angle*100
  int laser = rx_data[5];                   // Byte 5 is Laser State
  int spare = rx_data[6]; 
  // Byte 7 is checksum
  
  controller->set_laser(laser);
  controller->set_target(x/100.0, y/100.0);
}
