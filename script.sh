#!/bin/sh
uptime | grep -o '[0-9]\+\.[0-9]\+*' > iolog.txt
ifstat -A -T 1 1 >> iolog.txt
iostat >> iolog.txt