import serial
import sys
from enum import Enum
import time

PACKET_SIZE = 256

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


def send_packet(ser, data):
    if len(data) != PACKET_SIZE:
        print("Incorrect data size!")
        return
    
    ser.write(START_MAGIC.to_bytes(2, byteorder='little'))
    ser.write(data)
    ser.write(END_MAGIC.to_bytes(2, byteorder='little'))


def rec_packet(ser):
    frame = ser.read(PACKET_SIZE + 4)

    start_magic = frame[1] << 8 | frame[0]
    end_magic = frame[PACKET_SIZE + 3] << 8 | frame[PACKET_SIZE + 2]

    if start_magic != START_MAGIC or end_magic != END_MAGIC:
        print("Frame magic's not equal!")
    return frame


with serial.Serial(sys.argv[1], timeout=1) as ser:
    time.sleep(3)

    while 1:
        send_packet(ser, bytearray(PACKET_SIZE))

        s = rec_packet(ser)
        
        time.sleep(1)
    


