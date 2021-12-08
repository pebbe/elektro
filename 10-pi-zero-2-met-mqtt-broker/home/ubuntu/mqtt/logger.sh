#!/bin/sh

cd `dirname $0`

while :
do
    ./logger 2>> logger.err
    sleep 30
done
