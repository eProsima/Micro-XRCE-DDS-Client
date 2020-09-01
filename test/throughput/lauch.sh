AGENT_BUILD_PATH=/home/pgarrido/dev/eProsima/Micro-XRCE-DDS-Agent/build1.4.1
BUILD_PATH=build_bl/

PUB_BINARY=$BUILD_PATH/test/throughput/SimpleArrayPublisher/SimpleArrayPublisher
SUB_BINARY=$BUILD_PATH/test/throughput/TroughputRegister/TroughputRegister

echo "" > results.txt

for TOPIC_SIZE in 8 16 32 64 128 256 512 1024 2048 4096 8192; do 
    $AGENT_BUILD_PATH/MicroXRCEAgent udp4 --port 8888 -v0 -m ced -r $(pwd)/test/throughput/agent.refs &
    AGENT_PID=$!

    sleep 1

    $PUB_BINARY localhost 8888 $TOPIC_SIZE &
    PUB_PID=$!

    ANS=$($SUB_BINARY localhost 8888 $TOPIC_SIZE)

    echo "$TOPIC_SIZE, $ANS" >> results.txt
    kill -9 $PUB_PID
    kill -9 $AGENT_PID
done
