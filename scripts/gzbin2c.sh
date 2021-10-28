#!/bin/sh
set -e

convert2c() {
    zcat | od -An -tx1 -v | awk '{for(i=1;i<=NF;i++){printf "0x%s,",$i}}'
}

case "$#" in
    0) convert2c ;;
    1) cat "$1" | convert2c ;;
    *) exit 1 ;;
esac
