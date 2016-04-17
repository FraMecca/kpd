import gzip, pickle, os

def load_DB_into_memory_mpd (filename):
    fp = gzip.open (filename, mode = 'rt')
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

def check_db_status (pickleDB, DBlocation):
    if os.path.getctime (pickleDB) < os.path.getctime (DBlocation):
        return False
    else:
        return True

def load_DB_into_memory_pickle (DBlocation, pickleDB):
    if check_db_status (pickleDB, DBlocation) == False:
        print ('warning: pickle serial db is older than mpd database\nwill now update')
        with open (pickleDB, mode = 'wb') as fp:
            listDB = load_DB_into_memory_mpd (DBlocation)
            pickle.dump (listDB, fp)
    with open (pickleDB, mode = 'rb') as fp:
        listDB = pickle.load (fp)
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
                    break #does not check other keys in dict
    return retlist

def filter (flag, retlistDB, key):
    lastlist = list()
    for dictDB in retlistDB:
        if check(key, flag, dictDB):
            lastlist.append(dictDB)
    return lastlist

def searchDB (searchArg, DBlocation, searchMode, pickleDB):
    res = list()
    if searchMode == 'mpd':
        listDB = load_DB_into_memory_mpd (DBlocation)
    elif searchMode == 'pickle':
        listDB = load_DB_into_memory_pickle (DBlocation, pickleDB)
    else:
        print ('wrong search_mode')
        exit (1)
    retlist = search(searchArg, listDB)
    return retlist

def polish_argStr(argStr):
    try:
        idx = argStr.index('-f')
    except:
        idx = argStr.index('--filter')
    argStr = [ch.lower() for ch in argStr]
    retSt = argStr[idx:]
    #save just the part with filter items
    i = 0
    for entry in retSt:
        if '-' in entry[0] and (entry != '-f' and entry != '--filter'):
            break
        else:
            i += 1
    return retSt[:i]

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
