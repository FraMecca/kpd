import musicpd
import MPDdatabase
import config_file

def seek_track (client, n, status, mode = 's'):
    time = status['time'].split(':')
    elapsedTime = int (time[0])
    if mode == 's':
        client.seekid (status['songid'], n)
    elif mode == '+':
        client.seekid (status['songid'], n + elapsedTime)
    elif mode == '-':
        client.seekid (status['songid'], elapsedTime - n)
    elif mode == '%':
        totalTime = int (time[1])
        client.seekid (status['songid'], totalTime * n / 100)
    else:
        raise ValueError

def seek (client, state, status):
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

def print_playlist (client):
    i = 1
    l = ''
    for item in client.playlistinfo ():
        if 'artist' in item:
            l += item['artist']
        if 'title' in item:
            l += ' - ' + item['title']
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
    status = client.status ()
    song = client.currentsong ()
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

def play (client, n):
    if n is 0:
        client.play ()
    else:
        client.play (n-1)
    current_status (client)

def pause (client):
    client.pause ()
    current_status (client)

def next (client):
    client.next ()
    current_status (client)

def previous (client):
    client.previous ()
    current_status (client)

def mpdrandom (client, state):
        
        if state == 'on':
            client.random (1)
        elif state == 'off':
            client.random (0)
        else:
            print ('Toggle on or off')

def update (client):
    client.update ()

def mpdsearch (searchItem, argv, DBlocation, argFilter):
    retlist = MPDdatabase.searchDB(searchItem, DBlocation)
    if argFilter:
        retlist = MPDdatabase.filterDB(argv, retlist)
    res = polish_return (retlist)
    for entry in res:
        print(entry)
    return res


def shuffle (client):
    client.shuffle ()

def clear (client):
    client.clear ()

def swap (client, a, b):
    client.swap (a, b)

def stop (client):
    client.stop ()

def consume (client, state):
    if state == 'on':
        client.consume (1)
    elif state == 'off':
        client.consume (0)
    else:
        print ('Toggle on or off')

def single (client, state):
    if state == 'on':
        client.single (1)
    elif state == 'off':
        client.single (0)
    else:
        print ('Toggle on or off') 
