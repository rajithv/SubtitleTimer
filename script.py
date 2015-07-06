#!/usr/bin/python

import sys
import math

def format_timestamp(seconds):
    mins = int(math.floor(float(seconds))) // 60
    seconds = float(seconds) - float(mins*60);
    hours = mins // 60;
    mins = mins % 60;
    seconds_string = "{:2.3f}".format(seconds).replace('.',',')
    
    return str(hours)+":"+str(mins)+":"+seconds_string

prefix = ""

#num = argv[0]

if( len(sys.argv) != 4):
    print len(sys.argv) - 1, "arguments given"
    print "Usage: <times file> <text file> <output srt file>"
    exit()

time_file = sys.argv[1]
text_file = sys.argv[2]
output_file = sys.argv[3]

times = open(time_file)
de = open(text_file)

de_srt = open(output_file, "w")

segmentedTimes = times.read().split('\n-\n')

list_of_timestamps = []

for time in segmentedTimes:
    print "NEW SET OF TIMES"
    timestamps = time.split('\n')
    
    for i in xrange( 0, len(timestamps)-1):
        print timestamps[i], timestamps[i+1]
        new_timestamp = (timestamps[i], timestamps[i+1])
        list_of_timestamps.append(new_timestamp)

lines = de.read().split('\n')

for i in xrange(0, len(lines)-1):
    de_srt.write(str(i+1) + "\n")
    de_srt.write(format_timestamp(list_of_timestamps[i][0]) + " --> " + format_timestamp(list_of_timestamps[i][1]) + "\n")
    de_srt.write(lines[i]+ "\n\n")
    