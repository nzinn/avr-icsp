import avr
import serial
import time



PACKET_SIZE = 132
packet = bytearray(PACKET_SIZE)

PROG_ENABLE = 0
SET_CLOCK = 8

with serial.Serial('/dev/ttyUSB0') as ser:
    time.sleep(3)

    packet[0] = PROG_ENABLE
    ret = avr.send_rec_packet(ser, packet)
    print(ret.hex(' '))
    
    packet[0] = SET_CLOCK
    ret = avr.send_rec_packet(ser, packet)
    print(ret.hex(' '))
