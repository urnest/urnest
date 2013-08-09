#!/bin/bash
x=$(
  ( 
    ODINMAXBUILDS=1 odin '()+cmd=mkdir x \&\& false:stdout!:redo' 
    ODINMAXBUILDS=1 odin '()+cmd=mkdir x \&\& false:stdout'
    ODINMAXBUILDS=1 odin '()+cmd=true:stdout'
  )2>&1
)
if echo "$x"|grep 'rm failed'
then
  echo "$x" &&
  exit 1
fi
