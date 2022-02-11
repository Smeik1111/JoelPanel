from __future__ import print_function

import time
import serial

ser = serial.Serial(
    port='COM3',
    baudrate=115200
)

#wait until bootloader finished
time.sleep(2)

try:
    sleepTime = 0.030
    while sleepTime > 0:
        counter = 10
        test = False
        while counter> 0:
            if(test):
                ser.write(b'PREVIEW_2_ON/')
            else:
                ser.write(b'PREVIEW_4_ON/')
            test = not test
            ser.flush()
            time.sleep(sleepTime)
            out = b''
            while ser.inWaiting() > 0:
                out += ser.read(1)
            if out != b'PREVIEW_2_ON\r\n' and out != b'PREVIEW_4_ON\r\n' :
                print("Error received: %s" % out)
                break;
            counter = counter -1
        print("sucess with %s" % sleepTime)
        sleepTime = sleepTime - 0.0005
except KeyboardInterrupt:
    ser.close()
    exit(1)
ser.close()

