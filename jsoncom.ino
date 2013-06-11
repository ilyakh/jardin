#include <aJSON.h>

// Convention: all 'value' fields are floats

// constants
const int REQUEST_BYTES = 256; // identifies number of bytes to reserve for request
const char TERMINATOR = '\n';
// [!] requests larger than REQUEST_BYTES will be truncated

// uses following variables to build requests
char request_buffer[ REQUEST_BYTES ];
int buffer_index = 0;

boolean complete = false;

// identifies the serial_stream as Hardware Serial for the json-library
aJsonStream serial_stream( &Serial );


typedef struct callback {
  char code;
  float value;
};

callback ph = {
  1, // code 
  (random(0,100) * 0.001)
};


/*
void composeResponse( char* t, float v, char* u ) {
    
    aJsonObject* record;
    record = aJson.createObject();
      aJson.addStringToObject( record, "type", t );
      aJson.addNumberToObject( record, "value", v );
      aJson.addStringToObject( record, "unit", u );
     
    aJson.print( record, &serial_stream );
    
    aJson.deleteItem( record );
    
    Serial.println(); //
    
}
*/


void reply( String command ) {
  Serial.print( "Executing command" );
  Serial.println( command );
}


void setup() {
   
  Serial.begin( 9600 );
  
}

void loop() {    
  
  if ( complete ) {
      
      // toggle the state of buffer
      complete = false;
      
      // extract command from the incoming request   
      aJsonObject* request = aJson.parse( request_buffer );
      aJsonObject* command = aJson.getObjectItem( request, "command" );
      
      reply( command->valuestring );
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
