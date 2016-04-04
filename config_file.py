import os

def parse_file ():
    confFile = os.path.expanduser ("~") + '/.kpd.conf'
    if os.path.isfile (confFile):
        with open (confFile, 'r') as fp:
            dbLocation = fp.readline ().rstrip ('\n')
    else:
        print ('a config file is needed...\nInput mpd database location: ')
        dbLocation = input ()
        with open (confFile, 'w') as fp:
            fp.write (dbLocation)
        exit (1)

    return dbLocation
