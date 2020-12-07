#!/bin/sh

HOST="pi@192.168.100.80"

scp ./mpd-uart.py ${HOST}:/home/pi/mpd-uart.py
ssh ${HOST} sudo systemctl restart mpd-uart
