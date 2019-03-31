/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: Protocol.h
*/

#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef MODULE_PROTOCOL
#endif 

#define GIMBAL_RX_LEN 8   // Data length from PC to gimbal
#define GIMBAL_TX_LEN 12

class Controller;

class Protocol 
{
 public:

  Protocol();

  void init(Controller &_controller);
  void update();
     
 private:

  void process_rx(byte c);
  void parse_control(byte * cmd);
  void send_telemetry(void);
 
  Controller *controller;

  unsigned char rx_buff[GIMBAL_RX_LEN];
  int rx_index;
  bool rx_cmd;        // receiving command

  
};

#endif
