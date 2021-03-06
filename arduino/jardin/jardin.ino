#include <aJSON.h>

// [+] Convention: all 'value' fields are floats; true/false = 1.0, 0.0
// [+] Add event simulation functions for testing (alters the values stored in the status object);

// identifies number of bytes to reserve for request 
// [!] requests larger than REQUEST_BYTES will be truncated
const int   REQUEST_BYTES = 256; // [!] keep low, impacts memory om Duemilanove
const char  TERMINATOR = '\n'; // [!] avoid pretty formated json
const int   BAUD_RATE = 9600;

// uses following variables to build requests
char request_buffer[ REQUEST_BYTES ];
int  buffer_index = 0;

// state of the request 
// (toggle to true, if the terminal character is appended to the request_buffer)
boolean complete = false;

// identifies the serial_stream as Hardware Serial for the json-library
aJsonStream  serial_stream( &Serial );

// stores the incoming request and its details
aJsonObject* request;
String       command;


typedef struct Target {
  String command; // key
  int code; // numerical key
  float (*callback) (); // function that handles the reading request
};


/**
 *  Current targets:
 *  # sensor readings
 *  0: pH-level of the liquid (requires: liquid tank temperature)
 *  1: electric conductivity of the liquid
 *  2: relative humidity of the upper chamber
 *  3: liquid tank temperature
 *  4: upper chamber temperature
 *  5: liquid level
 * 99: power on actuator test
 *  
 *  Additional targets:
 *  +0:  level of nutrient
 *  +1:  level of pH+ solution
 *  +2:  level of pH- solution
 *  +3:  light pollution level of the liquid tank
 *  
 *  Calibration targets:
 *  0:  calibrate pH at 4.0
 *  1:  calibrate pH at 7.0
 *  2:  calibrate pH at 10.0
 *  3:  calibrate EC at 3000us
 *  4:  calibrate EC at 200us
 *  
 *  Test simulation targets:
 *  0:  simulate pH at level x, during next t seconds
 *  1:  simulate EC at x ppm, during next t seconds
 *  2:  simulate liquid level at x inches, during next t seconds
 *  3:  simulate relative humidity at x %, during next t seconds
 *  4:  simulate liquid tank temperature at x deg. C, during next t seconds
 *  5:  simulate upper chamber temperature at x deg. C, during next t seconds
 *
 */
Target targets[2];



void setup() {

  // Communication and handshake
  
  Serial.begin( BAUD_RATE );
  
  Serial.println( "Hello" );
  
  // Defines sensor targets
  // The callback procedure handles the request and returns a value 
  // (or, sendes a report directly into the Serial stream)
  
  Target acidity;
   acidity.command = "pH";
   acidity.code = 1;
   acidity.callback = random_reading;
  
  targets[0] = acidity;
  
  Target conductivity;
   conductivity.command = "EC";
   conductivity.code = 2;
   conductivity.callback = random_reading;   
  
  targets[1] = conductivity;
  
}




void composeResponse( char* t, float v, char* u ) {
    
    aJsonObject* record;
    record = aJson.createObject();
      aJson.addStringToObject( record, "type", t );
      aJson.addNumberToObject( record, "value", v );
      aJson.addStringToObject( record, "unit", u );
     
    aJson.print( record, &serial_stream );
    
    aJson.deleteItem( record );
    
    Serial.println();
    
}


float random_reading() {
  return 0.001 * random( 1, 100 ); 
}

void reply( String command ) {
  
  composeResponse( "pH", random_reading(), "ppm" );
  
  
  
  /*
  
  Serial.print( "Executing command: " );
  Serial.println( command );
  
  
  // report (debug, temporary)
  int iterations = sizeof( targets ) / sizeof( Target );
  float result = 0.0;
  
  
  // [/] probably fetch by enum instead?
  for ( int i = 0; i < iterations; i++ ) {
    if ( command == targets[i].command ) {
       Serial.print( "Running operation #");
       Serial.print( targets[i].code );
       Serial.print( "   " );
       
       result = targets[i].callback();
       
       Serial.print( "Reading result: ");       
       Serial.print( result, DEC );
       
       Serial.println();
    }
  }
  
  */
}

void empty_buffer() {
  // [?] will this interfere 
  for ( int i = 0; i < sizeof( request_buffer ) / sizeof( char ); i++ ) {
     request_buffer[i] = '\0'; 
  }
}




void loop() {    
  
  if ( complete ) { // process request
      
      // debug
      
      Serial.println( request_buffer );
      
      // toggle the state of buffer
      complete = false; 
      
      // extract command from the incoming request   
      request = aJson.parse( request_buffer );
      command = aJson.getObjectItem( request, "command" )->valuestring;
      
      reply( command );

      aJson.deleteItem( request );      
      // aJson.deleteItem( command );
      
      empty_buffer(); // empty request
    }
  
}


void serialEvent() {
  while ( Serial.available() ) {
    char in = (char) Serial.read();
    
    request_buffer[buffer_index++] = in;
    
    if ( in == TERMINATOR ) {
      buffer_index = 0;
      complete = true;
    }
  }
}
