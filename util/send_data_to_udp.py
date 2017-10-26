#!/usr/bin/env python

import socket
import sys

if len(sys.argv) == 4:
    ip = sys.argv[1]
    port = int(sys.argv[2])
    hex_data = sys.argv[3].split('-')

    byte_data = ''
    for h in hex_data:
        int_element = int(h, 16)
        char_element = chr(int_element)
        byte_data = byte_data + char_element

    #print byte_data

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(byte_data, (ip, port))

else:
    print('program ip port hex_data')