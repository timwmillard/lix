#!/bin/sh

for n in {1..10000}
do
    curl localhost:9333 &
done
wait
