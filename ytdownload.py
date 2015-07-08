#!/usr/bin/python

import os
import sys

if(len(sys.argv) < 3):
    print "Usage : ./ytdownload.py <youtube link, with https://> <save location as .mp4 file>"
    print "If VLC doesn't work, edit the vlc location manually into the script"
    exit()

ytlink = sys.argv[1]
saveloc = sys.argv[2]
vlcloc = "/Applications/VLC.app/Contents/MacOS/VLC"

#os.system("/Applications/VLC.app/Contents/MacOS/VLC -I rc "+ytlink+" --sout='#transcode{vcodec=mp4v,acodec=mp4a}:std{access=file,mux=mp4,dst="+saveloc+"}' vlc://quit")
os.system(vlcloc+" -I rc "+ytlink+" --sout='#transcode{vcodec=mp4v,acodec=mp4a}:std{access=file,mux=mp4,dst="+saveloc+"}' vlc://quit")

print ('Sucessfully Downloaded')