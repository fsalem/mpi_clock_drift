#!/bin/bash

PROVIDER=gni
TL=msg

if [ "x$ALPS_APP_PE" = "x0" ] ; then
    ./a.out -s $FIRST -e $TL -p $PROVIDER
else
    ./a.out -s $SECOND -e $TL -p $PROVIDER $FIRST
fi