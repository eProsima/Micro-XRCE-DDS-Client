#!/bin/sh

# Memory map file.
echo $1 $(size -t ./libmicro_xrce_dds_client.a | tail -1) >> ./memory_map.txt
