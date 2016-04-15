import musicpd, MPDdatabase
import shell as sh
from sys import argv
import sys

class mpdclient:
    def __init__ (self, DBlocation, searchMode, host, port, pickleDB, argFilter):
        self.DBlocation = DBlocation
        self.client = musicpd.MPDClient ()
        self.client.connect (host, port)
        self.status = self.client.status ()
        self.searchMode = searchMode
        self.pickleDB = pickleDB
        if argFilter:
            self.needToFilter = argFilter
        else:
            self.needToFilter = False
    def update_status (self):
        self.status = self.client.status ()

class nostdout (object):
    pass
    def __init__ (self, std):
        self.stdout = std
    def write(self, x = None):
        pass
    def flush (self, x = None):
        pass
    def restore_std (self):
        return self.stdout


#functions
def seek_track (client, n, status, mode = 's'):
    time = status['time'].split(':')
    elapsedTime = int (time[0])
    if mode == 's':
        client.client.seekid (status['songid'], n)
    elif mode == '+':
        client.client.seekid (status['songid'], n + elapsedTime)
    elif mode == '-':
        client.client.seekid (status['songid'], elapsedTime - n)
    elif mode == '%':
        totalTime = int (time[1])
        client.client.seekid (status['songid'], totalTime * n / 100)
    else:
        raise ValueError

def seek (client, state, null):
    status = client.status
    try:
        if '%' in state:
            n = int (state.split ('%')[0])
            seek_track (client, n, status, '%')
        elif '+' in state:
            n = int (state.split ('+')[1])
            seek_track (client, n, status, '+')
        elif '-' in state:
            n = int (state.split ('-')[1])
            seek_track (client, n, status, '-')
        else:
            n = int (state)
            seek_track (client, n, status)
    except:
        print ('syntax is: seek [s][%] or [+,-][s] or [s]')

def convert_time (inTime):
    inTime = int (inTime)
    l = str (round (inTime / 60 - 0.5))
    l = l + ':'
    sec = str (inTime % 60)
    if len (sec) < 2:
        sec = '0' + sec
    l = l + sec
    if len (l) < 4:
        l = l + '0'
    return l

def playlist (client, args, null):
    i = 1
    l = ''
    for item in client.client.playlistinfo ():
        if 'artist' in item:
            l += item['artist']
        if 'title' in item:
            l += ' - ' + item['title']
        if not l:
            l = item['file']
        print (str (i) + '.', l)
        i += 1
        l = ''

def get_generic_info (status):
    genericInfo = '   '
    if status['random'] is '1':
        genericInfo += 'random: on\t'
    if status['consume'] is '1':
        if len (genericInfo) > 0:
            genericInfo += 'consume: on\t'
        else:
            genericInfo += 'consume: on\t'
    if status['single'] is '1':
        if len (genericInfo) > 0:
            genericInfo += 'single: on'
        else:
            genericInfo += 'single: on'

    return genericInfo

def current_status (client):
    status = client.client.status ()
    song = client.client.currentsong ()
    try:
        artist = song['artist']
    except:
        artist = ''
    try:
        album = song['album']
    except:
        album = ''
    try:
        title = song['title']
    except:
        title = ''
    if artist or title:
        print (artist, '-', title)
    else:
        print (song['file'])
    if album:
        print (album)
    time = status['time']
    time = time.split (':')
    elapsedTime = convert_time (time[0])
    totalTime = convert_time (time[1])
    genericInfo = get_generic_info (status) #get random, repeat, consume, single
    print ('(' + status['state'] + ')', ' #' + str (int (status['song']) + 1) + '/' + status['playlistlength'], ' ', elapsedTime + '/' + totalTime, genericInfo)
    if 'updating_db' in status:
        print ('Database Update #' + status['updating_db'])

#clean the string for a correct return in stdout
def polish_return (retlist):
    finalList = list()
    for dictDB in retlist:
        finalList.append(dictDB['directory'].strip('\n')+"/"+dictDB['fsName'].strip('\n'))
    return finalList

def mpdplay (client, n):
    if n == 0:
        client.client.play ()
    else:
        n = int (n)
        client.client.play (n-1)
    current_status (client)

def play (client, args, null):
    try:
        if args == None:
            if client.status['state'] == 'stop':
                mpdplay (client, 0)
            else:
                pause (client)
        else:
            mpdplay (client, int (args))

    except:
        print ('mpd error: bad song index')
        exit (1)

def pause (client):
    client.client.pause ()
    current_status (client)

def next (client, args, null):
    try:
        client.client.next ()
        current_status (client)
    except:
        print ('out of bounds')
        exit (2)

def previous (client, args, null):
    try:
        client.client.previous ()
        current_status (client)
    except:
        print ('out of bounds')
        exit (2)

def random (client, state, null):
        if state == 'on':
            client.client.random (1)
        elif state == 'off':
            client.client.random (0)
        else:
            print ('Toggle on or off')

def update (client, args, null):
    client.client.update ()

def add(client, args, result):
    for entry in result:
        entry = entry.rstrip ('\n')
        client.client.add (entry)

def filter (client, args, res):
    pass

def search (client, searchItem, null):
    argFilter = client.needToFilter
    DBlocation = client.DBlocation
    retlist = MPDdatabase.searchDB(searchItem, DBlocation, client.searchMode, client.pickleDB)
    if argFilter:
        retlist = MPDdatabase.filterDB(argv, retlist)
    res = polish_return (retlist)
    for entry in res:
        print(entry)
    return res

def shuffle (client, args, null):
    client.client.shuffle ()

def clear (client, args, null):
    client.client.clear ()

def swap (client, args, null):
    _swap (client.client, int (args[0]) - 1, int (args[1]) - 1)


def _swap (client, a, b):
    client.swap (a, b)

def stop (client, args, null):
    client.client.stop ()

def consume (client, state, null):
    if state == 'on':
        client.client.consume (1)
    elif state == 'off':
        client.client.consume (0)
    else:
        print ('Toggle on or off')

def single (client, state, null):
    if state == 'on':
        client.client.single (1)
    elif state == 'off':
        client.client.single (0)
    else:
        print ('Toggle on or off')

def shell (client, DBlocation):
    return (sh.shell (client, DBlocation))

def output (client, args, null):
    if sys.stdout == sys.__stdout__:
        pass
    else:
        sys.stdout = nostdout.restore_std (sys.stdout)

def no_output (client, args, null):
    sys.stdout = nostdout (sys.stdout)

def search_mode (client, args, null):
    client.searchMode = args

#end util.py
