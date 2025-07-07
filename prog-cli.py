import serial
import sys
from enum import Enum
import time

PACKET_SIZE = 256

class PacketMagic(Enum):
    START_LSB = 0xC4
    START_MSB = 0xCE
    END_LSB = 0x14
    END_MSB = 0x0F
    
class PacketInstType(Enum):
    PROG_ENABLE   = 0
    CHIP_ERASE    = 1
    WRITE_FLASH   = 2
    READ_FLASH    = 3
    WRITE_EEPROM  = 4
    READ_EEPROM   = 5
    WRITE_FUSE    = 6
    READ_FUSE     = 7


class PacketErrorStatus(Enum):
    OK = 0
    MALFORMED_PACKET = 1
    EXEC_ERR = 2


with serial.Serial(sys.argv[1]) as ser:
    while 1:
        ser.write(PacketMagic.START_LSB)
        ser.write(PacketMagic.START_MSB)

        ser.write(PacketInstType.PROG_ENABLE)

        for x in range(255):
            ser.write(0)

        ser.write(PacketMagic.END_LSB)
        ser.write(PacketMagic.END_MSB)
        time.sleep(1)
