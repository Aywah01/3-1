// original source code: https://github.com/tronixlabsAU/HC06
#include <SoftwareSerial.h>

SoftwareSerial BT(5, 6); 
// creates a "virtual" serial port/UART
// connect BT module TX to D10
// connect BT module RX to D11
// connect BT Vcc to 5V, GND to GND

#define SW 2 // define pin 2 as control pin
// by Robojax for Robojax.com, Jan 01, 2018
void setup()  
{
    
  // set digital pin to control as an output
  pinMode(SW, OUTPUT);

  // set the data rate for the SoftwareSerial port
  BT.begin(9600);

  // Send test message to other device
  BT.println("Hello from Arduino");
}

char a; // stores incoming character from other device

void loop() 
{
  if (BT.available())
  // if text arrived in from BT serial...
  {
    a=(BT.read());
    if (a=='1')
    {
      digitalWrite(SW, LOW);// Turn LIGH ON
      BT.println("Light on");
    }
    if (a=='2')
    {
      digitalWrite(SW, HIGH);// Turn LIGHT OFF
      BT.println("Light off");
    }
    if (a=='?')
    {
      BT.println("Send '1' to turn LIGHT on");
      BT.println("Send '2' to turn LIGHT OFF");
    }   
    
  }
}
