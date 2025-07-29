import serial
import sys

from math import ceil
from enum import Enum
import time
from intelhex import IntelHex


HEADER_LENGTH = 4
PAGE_NUM_BYTES = 128


PACKET_SIZE = HEADER_LENGTH + PAGE_NUM_BYTES

START_MAGIC = 0xCEC4
END_MAGIC = 0x0F14

PROG_ENABLE = 0
CHIP_ERASE = 1
WRITE_FLASH = 2
READ_FLASH_PAGE = 3
WRITE_EEPROM = 4
READ_EEPROM = 5
WRITE_FUSE = 6
READ_FUSE = 7


OK = 0
MALFORMED_PACKET = 1
EXEC_ERR = 2


PACKET_STATUS_POS = 0


PRINT_WIDTH = 16


def send_packet(ser, data):
    if len(data) != PACKET_SIZE:
        print(f'Incorrect data size: {len(data)}')
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
        return None
    return frame[2:(PACKET_SIZE + 2)]

        

def send_rec_packet(ser, data):
    send_packet(ser, data)
    return rec_packet(ser)


def write_file_to_flash(ser, filename):
    hex_file = IntelHex(filename)
    file_num_bytes = len(hex_file)

    programmed_bytes = 0
    
    print(f'File segments: {hex_file.segments()}')
    addr = 0
    while True:
        packet = bytearray()

        packet.append(WRITE_FLASH)

        packet.extend(int(addr).to_bytes(2, "little"))

        # Padding
        packet.append(0)

        packet_num_bytes = PAGE_NUM_BYTES
        for offset in range(PAGE_NUM_BYTES):
            if (addr * 2 + offset) < file_num_bytes:
                programmed_bytes += 1

                packet.append(hex_file[int(addr * 2) + offset])
            else:
                packet_num_bytes = offset
                packet.extend(bytes(PAGE_NUM_BYTES - offset))
                break

        if packet_num_bytes > 0:
            print(packet.hex(' '))
            print()
            send_rec_packet(ser, packet)
        else:
            break
        addr += PAGE_NUM_BYTES / 2
    print(f'Final bytes: {programmed_bytes}\n')



def verify_program(ser, filename):

    hex_file = IntelHex(filename)
    file_num_bytes = len(hex_file)


    flash_contents = bytearray()
    for i in range(ceil(file_num_bytes / PAGE_NUM_BYTES)):
        packet = bytearray()
        packet.append(READ_FLASH_PAGE)
        packet.extend(int(i * PAGE_NUM_BYTES / 2).to_bytes(2, 'little'))
        packet.extend(bytes(1 + PAGE_NUM_BYTES))
        rec = send_rec_packet(ser, packet)
        print(rec.hex(' '))
        print()
        if rec:
            flash_contents.extend(rec[HEADER_LENGTH:])
        else:
            print('Error retrieving flash contents.')
            exit()

    
    for i in range(file_num_bytes):
        if hex_file[i] != flash_contents[i]:

            hex_file.dump()
            print(f'Flash and file not equal! \n\n {hex_file[i]} != {flash_contents[i]} at address {i} \n\n{flash_contents.hex(" ")}') 
            exit()
        

        
def main():
    if (len(sys.argv) != 3):
        print("Improper command usage: use 'command hex_file port_file'")
        exit()

    with serial.Serial(sys.argv[2]) as ser:
        # Wait for bootloader to init
        time.sleep(3)

        packet = bytearray(PACKET_SIZE)

        print('Enabling programming...')
        packet[0] = PROG_ENABLE
        rec = send_rec_packet(ser, packet)

        if rec[PACKET_STATUS_POS] != OK:
            print("Couldn't enable programming!")
            print(rec.hex(' '))
            return


        print("Erasing chip...")
        
        packet[0] = CHIP_ERASE
        send_rec_packet(ser, packet)
        print('Chip erased.')

        print('Writing flash...')
        write_file_to_flash(ser, sys.argv[1])

        print('Verifying flash...')
        verify_program(ser, sys.argv[1])
        print('finished')

        
if __name__ == '__main__':
    main()
