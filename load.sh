#!/bin/sh

for n in {1..1000}
do
    curl localhost:9333 &
done
wait
