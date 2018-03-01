 #include <pb_encode.h>
#include <pb_decode.h>
#include "secret_number.pb.h"

bool rcv_data = true;
int rcv_value = 0;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  Serial.println("Startup, World...");
  rcv_data = false;
}

void loop() {

  uint8_t buffer[128];
  size_t message_length;
  bool status;

  if (rcv_data) {    
    // Print out the data we got
    Serial.print("Received number was: ");
    Serial.println(rcv_value, DEC);

    // Blink the number of the value we were given
    for (int i = 0; i < rcv_value; ++i) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(75);
        digitalWrite(LED_BUILTIN, LOW);
        delay(75);
    }
    
    rcv_data = false;
  }
  // SERIAL RECEIVE

  // Wait until we receive a message length message
  if (Serial.available() > 0) {

    // First byte tells us the message length
    message_length = Serial.read();
    Serial.print("Received message size: ");
    Serial.println(message_length, DEC);

    // Read the incoming data
    Serial.readBytes(buffer, message_length);
    Serial.print("Read in (bytes): ");
    Serial.println(message_length, DEC);

    // Create pb input stream
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

    // Unpack the binary data into our message
    SecretNumber rcv_msg;
    status = pb_decode(&stream, SecretNumber_fields, &rcv_msg);
        
    // Check for errors
    if (!status)
    {
      Serial.print("Decoding failed:");
      Serial.println(PB_GET_ERROR(&stream));
    } else {
      rcv_data = true;
      rcv_value = rcv_msg.value;
    }
  }
}
