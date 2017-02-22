#!/bin/bash

octave --silent --eval "pkg load signal" 
str=$?
if [ -z "$$(octave --eval "pkg load signal")" ]; then  echo success; else echo fail; fi
