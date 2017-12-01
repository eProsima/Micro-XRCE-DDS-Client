# Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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