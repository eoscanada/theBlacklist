#!/bin/bash
# change it to your local ip
HOST=192.168.1.68

NAME=theblacklist

docker ps | grep $NAME-eos-dev
if [ $? -ne 0 ]; then
    echo "Run eos dev env "
    docker run --name $NAME-eos-dev -dit --rm -v  `(pwd)`:/$NAME lemonswang/eos-dev
fi

docker exec $NAME-eos-dev eosiocpp -o /$NAME/$NAME.wast /$NAME/$NAME.cpp
echo ".................................build complete!"

docker cp ../$NAME nodeosd:/
