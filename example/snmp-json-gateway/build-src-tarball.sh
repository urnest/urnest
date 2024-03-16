#!/bin/sh

set -e

d=$(cd $(dirname "$0") && pwd)

outd=$(pwd)

set -x

cd "$d/../.."

version=$(git describe --tags --exact-match  |grep 'snmp-json-gateway-' || true)

test $(echo $version | wc -w) -lt 2
if [ -z "$version" ]
then
  version="snmp-json-gateway-unknown"
else
  if [ $(git status -s |wc -l) != 0 ]
  then
    version="snmp-json-gateway-unknown"
  fi
fi  

tar czf "$outd/$version.tar.gz" --exclude '*~' --exclude '__pycache__' Odinfile xju hcp example/snmp-json-gateway

echo "created $outd/$version.tar.gz"
