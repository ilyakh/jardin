import org.json.*;

import processing.serial.*;
import java.util.regex.Pattern;

Serial serialPort;
String messageBuffer = "";

void setup() {
  // reduce the framerate
  frameRate( 10 );
  
  println( Serial.list() );
  
  serialPort = new Serial( this, Serial.list()[0], 9600 );
  serialPort.clear();
  
  serialPort.bufferUntil( 10 ); 
  
  
}

boolean requested = false;

void requestEC() {
  
  if ( !requested ) {
    JSON requestObject = JSON.createObject();
    requestObject.setString( "command", "EC" );
    
    String ecRequest = requestObject.toString();
    char[] ecMessage = ecRequest.toCharArray();
    
    for ( int i = 0; i < ecMessage.length; i++ ) {
       serialPort.write( (int) ecMessage[i] ); 
    }
    
    
    serialPort.write( '\n' );
    
  }
  
}


void draw() {
    
  

  if ( messageBuffer != "" ) {
    
    try {
      
      JSON responseObject = JSON.parse( messageBuffer );
      println( responseObject );
      
      println( "Current value of the reading: " );
      println( responseObject.getFloat( "value" ) );
      
    } catch ( RuntimeException e ) {
      
      println( "Unable to parse response" );
      println( messageBuffer );
       
    }
    
    messageBuffer = "";
  }
  
}

void serialEvent( Serial p ) {
  messageBuffer = p.readString();
}

void mousePressed() {
  requestEC();
}
