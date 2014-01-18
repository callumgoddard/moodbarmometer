/* 
 * 'The Awesomated Mood Barometer' Arduino Firmware v2
 * 
 * To be used on an arduino connected to 'The Awesomated Mood Barometer'.
 * 
 * Developed by Callum Goddard for CruftFest 2013, part of IDMT course of the QMUL MAT Program.
 * 
 * Code has been produced using
 *   -  read ASCII example
 *   - http://learn.adafruit.com/bmp085/using-the-bmp085-api-v2
 *
 */

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

// Setup the BMP085 Altitude temperature sensor.
Adafruit_BMP085 bmp = Adafruit_BMP085(10085);

// Declare the Servos for the display dials.
Servo temperatureDialServo;
Servo pressureDialServo;
Servo humidityDialServo;

// set initial servo angles.
int temperatureDialServoAngle = 0;
int pressureDialServoAngle = 0;
int humidityDialServoAngle = 0;


// Declare the pins that correspond to the 
// red, green and blue RGB LED colours
const int redLEDPin = 3;
const int greenLEDPin = 5;
const int blueLEDPin = 6;

// Declare the pins that the servos are attached to.
const int temperatureDialServoPin = 9;
const int pressureDialServoPin = 10;
const int humidityDialServoPin = 11;


void setup(){

  // initialise serial communication.
  Serial.begin(9600); 

  // Sets up the pin modes for th RGB LED
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);

  // attaches the dial servos to it's pin and
  // resets the servo position.
  temperatureDialServo.attach(temperatureDialServoPin);
  temperatureDialServo.write(temperatureDialServoAngle);
  pressureDialServo.attach(pressureDialServoPin);
  pressureDialServo.write(pressureDialServoAngle);
  humidityDialServo.attach(humidityDialServoPin);
  humidityDialServo.write(humidityDialServoAngle);

  // if statment code taken from Adafruit example. - 
  // stops the programm if the BMP085 sensor is not connected.  
  if(!bmp.begin()){
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
}


void loop() {

  boolean internalSensorMode;
  int temperature;
  int pressure;
  int humidity;
  int sensorTemperature;
  int sensorPressure;

 
  // Get the values from the internal Pressure Temperature sensor.
   sensors_event_t event; // makes a new sensor event
   bmp.getEvent(&event);  // gets an event.
   
   if(event.pressure){
   // get current temperature and pressure values
   //  from the BMP sensor
   float bmpTemp;
   float bmpPressure;
   bmp.getTemperature(&bmpTemp);
   bmpPressure = event.pressure;
   
   // convert the sensor values to intergers.
   sensorTemperature = (int)bmpTemp;
   sensorPressure = (int)bmpPressure;
   }

  // CODE taken from read ASCII example.
  // if there's any serial available, read it:

  // checks to see if there is serial data to be read
  while (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    internalSensorMode = (boolean)Serial.parseInt(); // converts the interger to a boolean to set the barometer mode.
    // do it again:
    temperature = Serial.parseInt();
    // do it again:
    pressure = Serial.parseInt();
    // do it again:
    humidity = Serial.parseInt();
    
    // look for a new line - this is the end
    // of the message.
    if (Serial.read() == '\n') {

      if(internalSensorMode){
        // internal sensor mode has been set
        // update the temperature and pressure values
        // to the ones obtained by the BMP085 sensor.
        
        temperature = sensorTemperature;
        pressure = sensorPressure;

      }

      // update the servo position to display the
      // correct value.
      temperatureDialServo.write(temperatureToAngle(temperature));
      pressureDialServo.write(pressureToAngle(pressure));
      humidityDialServo.write(humidityToAngle(humidity));


      // update the RGB LEDs
      analogWrite(redLEDPin, temperatureToLED(temperature)); 
      analogWrite(greenLEDPin, pressureToLED(pressure));        
      analogWrite(blueLEDPin, humidityToLED(humidity));  
      
    }  
  }

}



// function to map temperatuer (in Celcius)
// to the angle needed to display the value on the temperature dial
int temperatureToAngle(int t){

  // constrains the value between the range
  // -19 and +28  which is the temperature range
  // in celsius the dial can display
  int temperature = constrain(t, -19, 39);

  // maps the temperature value to the correct servo angle
  int servoAngle = map(temperature, -19, 38, 0, 180)-5; // -5 to make the servo alignment more accurate with the physical dial display.

  return servoAngle;
}

int humidityToAngle(int h){

  // constrains the value between the range
  // 27 and 80 - which is the humidity range
  // in % the dial can display.
  int humidity = constrain(h, 36, 90);

  //maps the humidity to the correct servo angle.
  int servoAngle = map(humidity, 36, 90, 0, 177); // servo mapped between 0 and 177 to prevent servo sticking.

  return servoAngle; 

}



int pressureToAngle(int p){
  // Constrain the Pressure value
  // to the range the dial can display.
  int pressure = constrain(p, 977, 1048);

  //maps the humidity to a servo angle.
  int servoAngle = map(pressure, 977, 1048, 0, 165);

  return servoAngle;
}

// maps the temperature (in celsius) to the red LED value.
int temperatureToLED(int t) {

  // maps the temperature - inverted mapping
  // because of common annoid RGB LED.
  int temp = map((int)t, -19, 38, 255, 0);

  // value is constrained after the mapping
  // to allow for out lying temperature values
  // but still only have the LED light up
  // in the correct range.
  int LEDValue = constrain(temp, 0, 255);

  return LEDValue;

}

// maps the pressure to the green LED value.
int pressureToLED(float p){

  int pressure = constrain((int)p, 950, 1050);      // constrains pressure to a smaller range.
  int ledValue = map(pressure, 1000, 1050, 0, 255); // map pressure to the LED intensity.

  // constrain and invert because
  // common annoid RGB LEDs are being used
  ledValue = 255 - constrain(ledValue, 0, 255);

  return ledValue; 
}

// maps the humidity to the blue LED value
int humidityToLED(int h){

  int humidity = constrain(h, 0, 100);            // constrains humidity between 0 and 100%
  int ledValue = map(humidity, 0, 100, 0, 255);  // maps humidity to the LED intensity.

  // constrain and invert because
  // common annoid RGB LEDs are being used
  ledValue = 255 - constrain(ledValue, 0, 255);

  return ledValue;
}















