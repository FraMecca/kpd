import os


def database (entry, dbLocation, hostip, portnumber):
    return entry, hostip, portnumber
def host (entry, dbLocation, hostip, portnumber):
    return dbLocation, entry, portnumber
def port (entry, dbLocation, hostip, portnumber):
    return dbLocation, hostip, entry

options = {'db': database, 'host': host, 'port': port} 

def parse_file ():
    confFile = os.path.expanduser ("~") + '/.kpd.conf'
    if os.path.isfile (confFile):
        dbLocation = '0'
        hostip = '0'
        portnumber = '0'
        with open (confFile, 'r') as fp:
            configs = [line.rstrip ('\n') for line in fp]
        for line in configs:
            if ' = ' in line:
                entry = line.split (' = ')
            else:
                entry = line.split ('=')
            dbLocation, hostip, portnumber = options [entry[0]] (entry [1], dbLocation, hostip, portnumber)

    else:
        print ('a config file is needed...\nInput mpd database location: ')
        dbLocation = input ()
        print ('input host')
        host = input ()
        print ('input port')
        port = input ()
        with open (confFile, 'w') as fp:
            fp.write ('db = ' + dbLocation + '\n')
            fp.write ('host = ' + host + '\n')
            fp.write ('port = ' + port + '\n')
        exit (2)


    return dbLocation, hostip, portnumber
