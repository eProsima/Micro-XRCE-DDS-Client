#!/bin/sh

# Create build directory.
if [ -d build ]; then
    rm -rf build
fi
mkdir build && cd build;

# Configure global cmake.
cmake -DTHIRDPARTY=ON ..;

# Compile profiles.
PROFILES="complete_profile core_profile xml_profile ref_profile tcp_profile udp_profile serial_profile"
for P in $PROFILES
do
    cmake -DCLIENT_CONFIG="${PWD}"/../test/memory/memory_map/$P.config ..;
    make;
    ../test/memory/memory_map/memory_map.sh $P
done

# Launch memory map analysis.
python3 ../test/memory/memory_map/memory_map_analysis.py ./memory_map.txt

# Launch stack analysis.
cmake -DTHIRDPARTY=ON -DMEMORY_PERFORMANCE=ON -DCLIENT_CONFIG="${PWD}"/../test/memory/consumption/performance_profile.config ..
make
../test/memory/consumption/stack_usage.sh
