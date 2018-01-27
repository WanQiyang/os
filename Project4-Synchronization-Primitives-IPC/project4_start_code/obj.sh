#!/bin/bash
echo $1
mipsel-linux-objdump -D -S $1 > in.txt
