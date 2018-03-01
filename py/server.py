#!/usr/local/bin/python3

import serial
from secret_number_pb2 import SecretNumber
from flask import Flask, request

# Create a serial connection, which is retained while the server is live
ser = serial.Serial('/dev/cu.usbserial-AM01XX8R', 9600, timeout=1)

app = Flask(__name__)

startup_check = False

# Create a message and sen it to the Arduino Nano
def send_message(secret_number):
    msg = SecretNumber()
    msg.value = secret_number

    # Serialize the contents for us to send to Arduino
    data = SecretNumber.SerializeToString(msg)

    # Send arduino the length of the message so it knows what's up
    # TODO (cg): Figure out why this is necessary. Seems weird.
    ser.write([len(data)])

    # Send the actual message data to Arduino
    count = ser.write(data)
    print("Sent {} bytes".format(count))

    # Read in the response from the arduino and return it
    output = []
    rcv_data = ser.readline()
    while (rcv_data):
        output.append(rcv_data)
        print("R: {}".format(rcv_data))
        rcv_data = ser.readline()

    return output

@app.route("/")
def index():
    global startup_check

    # We need to wait for a message from the Arduino to let us know
    # it's ready to receive messages. This should only need to happen once
    if not startup_check:
        print("Waiting for startup signal...")

        ser.readline()

        print("Startup signal recieved!")
        startup_check = True

    # Retrieve the secret value from the browser's GET argument list
    secret_number = int(request.args.get('value'))

    # Send the message to the arduino and return the output to the browser
    output = send_message(secret_number)
    output_str = [str(out) for out in output]

    return "<br>".join(output_str)

if __name__ == "__main__":
    # Star the application in debug mode and with no local address bindings
    # so we can see the results outside of this machine
    app.run(debug=True, host='0.0.0.0')
