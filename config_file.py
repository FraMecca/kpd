import os

values = {'db': '', 'host':'', 'port':'', 'searchmode':'', 'pickle_DB_location':''}

def parse_file ():
    confFile = os.path.expanduser ("~") + '/.kpd.conf'
    if os.path.isfile (confFile):
        pickle_DB_location = '0'
        with open (confFile, 'r') as fp:
            configs = [line.rstrip ('\n') for line in fp]
        for line in configs:
            if ' = ' in line:
                entry = line.split (' = ')
            else:
                entry = line.split ('=')
            values[entry[0]] = entry [1]

    else:
        print ('a config file is needed...\nInput mpd database location: ')
        dbLocation = input ()
        print ('\ninput host')
        host = input ()
        print ('\ninput port')
        port = input ()
        print ('\ninput search mode (mpd, pickle)')
        searchMode = input ()
        print ('\ninput pickle_DB_location, if any')
        pickle_DB_location = input ()
        with open (confFile, 'w') as fp:
            fp.write ('db = ' + dbLocation + '\n')
            fp.write ('host = ' + host + '\n')
            fp.write ('port = ' + port + '\n')
            fp.write ('searchmode = ' + searchMode + '\n')
            if pickle_DB_location:
                fp.write ('pickle_DB_location = ' + pickle_DB_location + '\n')
        exit (2)

    return values['db'], values['searchmode'], values['host'], values['port'], values['pickle_DB_location']
