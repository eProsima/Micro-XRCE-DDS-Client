#!/bin/sh

# Memory map file.
echo $1 $(size -t ./libmicrortps_client.a | tail -1) >> ./memory_map.txt
