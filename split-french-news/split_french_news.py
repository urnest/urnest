#!/usr/bin/python

import sys
import mob
import string
import os.path

def extractAudio(inputFileName):
    try:
        outputFileName=os.path.splitext(inputFileName)[0]+'.mp3'
        mob.doCommand(
            'rm -f %(outputFileName)s' % vars())
        mob.doCommand(
            'ffmpeg -i %(inputFileName)s -ab 128 -ar 48000 -ac 2 %(outputFileName)s' % vars())
        return outputFileName
    except:
        raise mob.inContext(
            'extract audio from file %(inputFileName)s' %vars(),
            sys.exc_info())
    pass

def extractAudioSegment(inputFileName, begin, end):
    try:
        outputFileName='%s-%010u.mp3' % (os.path.splitext(inputFileName)[0], begin)
        duration=end - begin
        mob.doCommand(
            'ffmpeg -i %(inputFileName)s -ab 128 -ss %(begin)s -t %(duration)s %(outputFileName)s' % vars())
        return outputFileName
    except:
        raise mob.inContext(
            'extract segment of audio [%(begin)s:%(end)s] from file %(inputFileName)s' % vars(),
            sys.exc_info())
    pass

def getDuration(inputFileName):
    try:
        x=mob.doCommand('midentify %(inputFileName)s' % vars())
        y=[ z for z in x.splitlines() if z.startswith('ID_LENGTH=') ]
        assert len(y) == 1, str(y)
        duration=int(float(y[0][10:]))
        return duration
    except:
        raise mob.inContext(
            'get duration of audio file %(inputFileName)s' % vars(),
            sys.exc_info())
    pass

def convert(inputFileName):
    try:
        audioFileName = extractAudio(inputFileName)
        duration = getDuration(audioFileName)
        begin = 0
        outputFileNames = []
        while begin < duration:
            end=min(duration, begin+60)
            outputFileNames.append(extractAudioSegment(audioFileName, begin, end))
            begin = end
            pass
        return outputFileNames
    except:
        raise mob.inContext(
            'convert video file %(inputFileName)s into audio only 30 second segments' % vars(),
            sys.exc_info())
    pass

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print 'usage: split_french_news.py xxx.mpg'
        sys.exit(1)
        pass
    print string.join(convert(sys.argv[1]), ' ')
    pass
