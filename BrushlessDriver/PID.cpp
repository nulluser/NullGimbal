/*
  Brushless Gimbal Controller 
  
  2019 nulluser@gmail.com

  File: PID.cpp
*/



#include <arduino.h>

#include "PID.h"


PID::PID()
{
  Kp = 0;
  Ki = 0;
  Kd = 0;
  Kmin = 0;
  Kmax = 0;
  sp = 0;
  pv = 0;
  mv = 0;
  iterm = 0;
  last_pv = 0;
  update_count = 0;
}

void PID::set_params(float _Kp, float _Ki, float _Kd)
{
  Kp = _Kp;
  Ki = _Ki * dt;       // Account for dt in constants
  Kd = _Kd / dt;       // Account for dt in constants
}


void PID::set_limits(float _Kmin, float _Kmax)
{
  Kmin = _Kmin;
  Kmax = _Kmax;
}

// Update Rate
void PID::set_dt(float _dt)  
{
  dt = _dt;
}


void PID::set_name(const char *_name) 
{ 
  strcpy(name, _name); 
};


// Setpoint
void PID::set_sp(float _sp)
{
  sp = _sp;
}

  
void PID::update(float _pv)
{
  pv = _pv;

  // Compute error
  float error = sp - pv;
  
  // Intergral term
  iterm += Ki * error;
  
  // Clamp sum
  if (iterm > Kmax) iterm = Kmax; else
  if (iterm < Kmin) iterm = Kmin;

  // Derivative of process variable
  float dpv = pv - last_pv;

  // Save previous process variable for D term
  last_pv = pv;   
  
  /*float max_dpv = 25;
  if (dpv < -max_dpv*dt) dpv = -max_dpv*dt;
  if (dpv >  max_dpv*dt) dpv = max_dpv*dt;*/
  
   
  // Compute PID values
  float P = Kp * error;                 // Proportional Term
  float D = Kd * dpv;   // D Term, Note sign
  
  // Compute output. Note sign on D term
  mv = P + iterm - D;

  // Clamp Output
  if (mv < Kmin) mv = Kmin;
  if (mv > Kmax) mv = Kmax;
 
  #ifdef DEBUG_PID
  
  if (update_count++ > 1000)
  {
    update_count = 0;

    char buff[100];

    Serial.print(name);

    Serial.print(" SP: ");
    dtostrf(sp,6, 2, buff);
    Serial.print(buff);

    Serial.print(" PV: ");
    dtostrf(pv,6, 2, buff);
    Serial.print(buff);

    Serial.print(" P: ");
    dtostrf(P, 10, 6, buff);
    Serial.print(buff);
    
    Serial.print(" I: ");
    dtostrf(iterm,10, 6, buff);
    Serial.print(buff);

    Serial.print(" D: ");
    dtostrf(D,10, 6, buff);
    Serial.print(buff);

    Serial.print(" MV: ");
    dtostrf(mv,5, 2, buff);
    Serial.print(buff);

    /*Serial.print(" TKP: ");
    dtostrf(Kp,10, 8, buff);
    Serial.print(buff);    
    
    Serial.print(" TKI: ");
    dtostrf(Ki/dt,10, 8, buff);
    Serial.print(buff);    
    
    Serial.print(" TKD: ");
    dtostrf(Kd*dt,10, 8, buff);
    Serial.print(buff);    

    Serial.print(" Kmin: ");
    dtostrf(Kmin,10, 8, buff);
    Serial.print(buff);  
    
    Serial.print(" Kmax: ");
    dtostrf(Kmax,10, 8, buff);
    Serial.print(buff);*/  

    Serial.println("");
  }
  #endif


}





  
