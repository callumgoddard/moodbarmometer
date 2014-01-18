/*
 * 'The Awesomated Mood Barometer' Weather XML fetcher v2
 *
 *  Fetches and Parses Temperature, Pressure and Humidiy from Yahoo Weather - http://developer.yahoo.com/yui/examples/connection/weather.html
 *
 *  Developed by Callum Goddard for CruftFest 2013, part of IDMT course of the QMUL MAT Program.
 * 
 *  Instructions:
 *   1) Have Awesomated Mood Barometer connected to computer
 *   2) Run this sketch
 *   3) Use Keyboard Left and Right Arrow keys to switch between Cities
 * 
 *  Note: If a null pointer exception or error occurs when assigning temperature, pressure or humidity it is Yahoo weather.
*   The xml data is not availible from Yahoo Weather.
 */

import processing.serial.*;

Serial port; // makes the serial port.

// varibales for XML weather retrival.
XML root;
XML condition;
XML atmosphere;

int temperature; 
int humidity;
int pressure;
String weatherCondition;

// City codes for yahoo weather
// storred in arrays with the city name and code
// so the city name can be displayed.
String[] beijing = {
  "Beijing", "CHXX0008"
};
String[] london = {
  "London", "UKXX0085"
};
String[] moscow = {
  "Moscow", "RSXX0063"
};
String[] munich = {
  "Munich", "GMXX0087"
};
String[] paris = {
  "Paris", "FRXX0076"
};
String[] riyadh = {
  "Riyadh", "SAXX0017"
};
String[] tokyo = {
  "Tokyo", "JAXX0085"
};
String[] currentLocation = {
  "Current Location", "UKXX0085" // value for current location of the awesomated mood barometer - which is london as the only awesomate mood barometer is currently located in london.
}; 

int cityNumber = 0; // varable to keep track of the city the weather is being displayed from.
String[] city = currentLocation; // variable to track the city data, defaulted to be current location.

int mode = 1;     // used to change the mode of the awesomated mood barometer
// 0 = use internal sensors for temperature and pressure
// 1 = use information sent via serial.

String displayText; // for displaying the data as text.

void setup() {
  size(800, 200);

  // Prints a list of serial devices
  // so the arduino can be selected
  println(Serial.list());

  // Opens the port to the arduino
  port = new Serial(this, Serial.list()[0], 9600);

  // initialises the barometer with default settings.
  getWeatherData(city);
  sendSerialData();
}


void draw() {
  // display text information
  background(125);
  textSize(32);
  fill(50);
  text(displayText, 30, 30);

  sendSerialData(); // sends serial data.
}

// Use arrow keys to cycle through the different cities
void keyPressed() {
  if (key == CODED) {
    if (keyCode == LEFT)
      switchCity(-1);
    getWeatherData(city); // get Weather Data
    sendSerialData();    // send weather data to serial port.
    if (keyCode == RIGHT)
      switchCity(+1);
    getWeatherData(city); // get Weather Data
    sendSerialData();    // send weather data to serial port.
  }
}

void switchCity(int s) {

  // increment the city number, but constrained to numbers 0-7.
  cityNumber = (cityNumber + s ) % 8;

  // if the cityNumber is -1
  // set it to be = to 7.
  if (cityNumber == -1)
    cityNumber = 7;

  // matches the city number to the city
  // retrives the weather data and sets the barometer mode.
  switch(cityNumber) {
  case 0: 
    // current Location of the awesomated mood barometer
    city = currentLocation;
    mode = 1;  // set the barometer to read its internal sensors
    break;
  case 1:
    // Show weather for London
    city = london;
    mode = 0; // turn off internal sensor reading in the barometer
    break;
  case 2:
    // Show weather for Paris
    city = paris;
    mode = 0; // turn off internal sensor reading in the barometer
    break;
  case 3:
    // Show weather for Munich
    city = munich;
    mode = 0; // turn off internal sensor reading in the barometer
    break;
  case 4:
    // Show weather for Moscow
    city = moscow;
    mode = 0; // turn off internal sensor reading in the barometer
    break;
  case 5:
    // Show weather for Riyadh
    city = riyadh;
    mode = 0; // turn off internal sensor reading in the barometer
    break;
  case 6:
    // Show weather for Beijing
    city = beijing;
    mode = 0; // turn off internal sensor reading in the barometer
    break;
  case 7:
    // Show weather for Tokyo
    city = tokyo;
    mode = 0; // turn off internal sensor reading in the barometer
    break;
  }
}


// Gets weather data from Yahoo Weather and Parses the XML file 
// gets the relevant pieces of info and updates the variables.
void getWeatherData(String[] cityCode) {

  //String cityCode = c;

  // Code developed with help from Processing Data Tutorial - http://processing.org/tutorials/data/
  // variable names and call methods used.
  root        = loadXML("http://xml.weather.yahoo.com/forecastrss?p=" + cityCode[1]);     // retrieves the weather info from yahoo             
  condition   = root.getChild("channel").getChild("item").getChild("yweather:condition");  // extracts the condition information
  atmosphere  = root.getChild("channel").getChild("yweather:atmosphere");  // gets the atmosphere information

  
  //need to add a check for when yahoo fails to give data.
  
  
  temperature = (condition.getInt("temp") - 32) * 5/9;            // gets the temperature in Farenheit (F) and Converts to Celcius (C)
  pressure    = (int)(atmosphere.getFloat("pressure") * 33.89);   // gets the pressure value in Inches of Mercury (inHg) and converts to hectopascals (hPa)
  humidity    = atmosphere.getInt("humidity");                    // gets the humidity value (%)

  weatherCondition = condition.getString("text"); // gets the current weather condition

  // Displays the weather conditions taken from Yahoo weather.
  // both in the GUI and in terminal
  displayText = "It is currently " + weatherCondition +" in " + cityCode[0] + "\n " + "Temperature = " + temperature +" C \n"+ " Atmospheric pressure = "+ pressure + " hPa \n" + " Humdity = " + humidity +"%\n";
  println("It is currently " + weatherCondition +" in " + cityCode[0] + "\n " + "Temperature = " + temperature +" C \n"+ " Atmospheric pressure = "+ pressure + " hPa \n" + " Humdity = " + humidity +"%\n");
}

void sendSerialData() {
  // formats the data into a string to write to the serial
  String data = new String(mode +"," + temperature + "," + pressure +"," + humidity + "\n");
  port.write(data);
}


