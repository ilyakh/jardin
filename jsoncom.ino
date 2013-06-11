#include <aJSON.h>

// [+] Convention: all 'value' fields are floats;
// [+] Add event emulation functions (alters the values stored in the status object);



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

aJsonObject* request;
String command;

typedef struct Callback {
  String name;
  int code;
  
  float (*f) ();
};

Callback callbacks[2];





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

float random_reading() {
  return 0.001 * random( 1, 100 ); 
}

void reply( String command ) {
  Serial.print( "Executing command" );
  Serial.println( command );
  
  // [-] debug
  
  int iterations = sizeof( callbacks ) / sizeof(Callback);
  float result = 0.0;
  Serial.println( iterations );
  
  for ( int i = 0; i < iterations; i++ ) {
    if ( callbacks[i].name == command ) {
       Serial.print( "Running operation #" );
       Serial.print( callbacks[i].code );
       
       result = callbacks[i].f();
       
       Serial.println( "Callback result: " );       
       Serial.println( result );
       
       Serial.println();
    }
  }
}


void setup() {
  
  Callback n;
   n.name = "pH";
   n.code = 1;
   n.f = random_reading;
  
  callbacks[0] = n;
  
  Callback m;
   m.name = "EC";
   m.code = 2;
   m.f = random_reading;   
  
  callbacks[1] = m;
  
  
  Serial.begin( 9600 );
  
}

void loop() {    
  
  if ( complete ) {
      
      // toggle the state of buffer
      complete = false; 
      
      // extract command from the incoming request   
      request = aJson.parse( request_buffer );
      command = aJson.getObjectItem( request, "command" )->valuestring;
      
      reply( command );

      aJson.deleteItem( request );      
      // aJson.deleteItem( command );
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
