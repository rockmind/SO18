#!/bin/bash
./zaklad 1 &
p1=$!
echo ${p1}
sleep 10
kill -9 ${p1}
