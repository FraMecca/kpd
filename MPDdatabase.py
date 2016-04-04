import gzip

def load_DB_into_memory (filename):
    fp = gzip.open (filename, mode = 'rt')
#    fp = open ('/home/user/.mpd/database_unzipped', mode = 'rt')
    listDB = list ()
    line = fp.readline()
    songdir = list()
    while not 'begin' in line:
        line = fp.readline()

    while line:
        
        entry = line.split (': ')
        if 'begin' == entry[0]:
            songdir.append(entry[1])
        if 'song_begin' in entry[0]:
            dictMpd = {'directory': '' , 'fsName': '' , 'time': '', 'album': '', 'date': '', 'title': '', 'track': '', 'artist': ''}
            dictMpd['fsName'] = entry[1]
            if songdir:
                dictMpd['directory'] = songdir[-1]
            else:
                dictMpd['directory'] = ''
        elif 'end' == entry[0]:
            songdir.pop()
        elif 'Artist' == entry[0]:
            dictMpd['artist'] = entry[1]
        elif 'Time' == entry[0]:
            dictMpd['time'] = float (entry[1])
        elif 'Album' == entry[0]:
            dictMpd['album'] = entry[1]
        elif 'Title' == entry[0]:
            dictMpd['title'] = entry[1]
        elif 'Track' == entry[0]:
            dictMpd['track'] = entry[1]
        elif 'Date' == entry[0]:
            dictMpd['date'] = entry[1]
        elif 'song_end' in entry[0]:
            listDB.append (dictMpd)

        line = fp.readline ()

    fp.close ()
    return listDB

def check (key, flag, dictDB):
    
    for entry in dictDB.values():
        if type(entry) == str:
            if flag == 'any':
                if key.lower() in entry.lower():
                    return True
            elif key.lower() in dictDB[flag].lower():
                return True
    return False

def search (key, listDB):
    retlist = list()
    for dictDB in listDB:
        for entry in dictDB.values():
            if not type(entry) == float:   
                if key.lower() in entry.lower():
                    retlist.append(dictDB)
                    break
    return retlist

def filter (flag, retlistDB, key):
    lastlist = list()
    for dictDB in retlistDB:
        if check(key, flag, dictDB):  
            lastlist.append(dictDB)
    return lastlist

def searchDB (searchArg, DBlocation):
    res = list()
    listDB = load_DB_into_memory(DBlocation)
    retlist = search(searchArg, listDB)
    return retlist

def polish_argStr(argStr):
    
    try:
        idx = argStr.index('-f')
    except:
        idx = argStr.index('--filter')
    argStr = [entry.lower() for entry in argStr]
    return argStr[idx:]
    
    
def filterDB (argStr, searchlist):
    
    retlist = searchlist
    dictKey = ('artist', 'album', 'title')
    argStr = polish_argStr(argStr)
    for item in argStr:
        entry = argStr.pop()
        if argStr[-1] in dictKey:
            flag = argStr.pop()
        else:
            flag = 'any'
        retlist = filter(flag, retlist, entry)

    return retlist
