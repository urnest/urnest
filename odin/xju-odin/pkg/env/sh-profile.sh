#!/bin/sh -e

env=$1;  shift;

if [ -n "$env" ] ; then
  cat $env > sh-profile
else
  touch sh-profile
fi  
