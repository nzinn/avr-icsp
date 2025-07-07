import serial
import sys
from enum import Enum
import time

PACKET_SIZE = 1

START_MAGIC = 0xCEC4
END_MAGIC = 0x0F14
    
PROG_ENABLE   = 0
CHIP_ERASE    = 1
WRITE_FLASH   = 2
READ_FLASH    = 3
WRITE_EEPROM  = 4
READ_EEPROM   = 5
WRITE_FUSE    = 6
READ_FUSE     = 7


OK = 0
MALFORMED_PACKET = 1
EXEC_ERR = 2


with serial.Serial(sys.argv[1], timeout=1) as ser:
    time.sleep(5)
    print("Done sleeping")
    while 1:

        ser.write((START_MAGIC).to_bytes(2, byteorder='little'))
        
        for x in range(PACKET_SIZE):
        
            ser.write(x.to_bytes(1, byteorder='little'))

        ser.write((END_MAGIC).to_bytes(2, byteorder='little'))
        s = ser.read(PACKET_SIZE + 4)
        print(s.hex(' '))
        time.sleep(.5)
