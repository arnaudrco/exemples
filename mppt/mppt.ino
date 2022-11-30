/*****************************************************************************
         Copyright  Motahhir  All Rights Reserved
*****************************************************************************/

/*****************************************************************************
   Header Files included
 *****************************************************************************/

/******************************************************************************
   PROJECT :  MPPT (P&O) implementation
   Function : P&O Arduino Code
 ******************************************************************************
 * *
    Written by  :  Saad Motahhir                       Date : 09/10/2016
 * *
    Email : saad.motahhir@usmba.ac.ma
 ******************************************************************************
   MODIFICATION LOG:
 ******************************************************************************
   Modified by :                                           Date :
   Comments :
 ******************************************************************************/
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

float sensorValue1 = 0;
float sensorValue2 = 0;
float voltageValue = 0;
float currentValue = 0;
float Power_now = 0, Power_anc = 0, voltage_anc = 0;
float delta = 3;
float pwm = 128;
void setup()
{
  pinMode(6, OUTPUT);
  lcd.begin(16, 2);
}

void loop()
{
  sensorValue1 = analogRead(A0);
  sensorValue2 = analogRead(A1);
  voltageValue = (sensorValue1 * 5.0 / 1023.0) * 5;
  currentValue = (sensorValue2 * 5.0 / 1023.0);
  lcd.setCursor(0, 0);
  Power_now = voltageValue * currentValue;

  lcd.print("Ppv=");
  lcd.print(Power_now);
  lcd.print("W");
  lcd.print(pwm);
  lcd.setCursor(0, 1);
  lcd.print("V=");
  lcd.print(voltageValue);
  lcd.print("V I=");
  lcd.print(currentValue);
  lcd.print("A");



  if (Power_now > Power_anc)
  { if (voltageValue > voltage_anc)
      pwm = pwm - delta;
    else
      pwm = pwm + delta;
  }
  else
  {
    if (voltageValue > voltage_anc)
      pwm = pwm + delta;
    else
      pwm = pwm - delta;
  }
  Power_anc = Power_now;
  voltage_anc = voltageValue;
  if (pwm < 20)
    pwm = 20;
  if (pwm > 150)
    pwm = 150;

  analogWrite(6, pwm);
}

