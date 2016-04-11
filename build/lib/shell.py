import musicpd
import readline
import sys
import util


def keepConnected(client):
    conn = False
    while conn == False:
        conn = True
        try:
            client.connect('127.0.0.1', '6600')
        except SocketErrori as e:
            conn = False
            time.sleep(2)

def evaluate (inp, client, l, DBlocation):
    inp = inp.split (' ')
    status = client.status ()
    
    if len(inp) > 1 and not str(inp[1]):
        inp.pop()
    
    if inp[0] == 'p' or 'play' == (inp[0]):
        try:
            if not status['state'] == 'stop':
                if len(inp) == 1:
                    util.pause (client)
                else:
                    util.play (client, int(inp[1]))
            else:                     
                if len(inp) == 1:
                    util.play (client, 0)
                else:
                    util.play (client, int(inp[1]))
        except:
           print('mpd error: bad song index') 
    elif inp[0] == 'pause':
        util.pause (client)
    elif inp[0] == 'next' or inp[0] == 'n':
        util.next (client)
    elif inp[0] == 'previous' or inp[0] == 'ps':
        util.previous (client)
    elif inp[0] == 'stop':
        util.stop (client)
    elif inp[0] == 'pl' or inp[0] == 'playlist':
        util.print_playlist (client)
    elif inp[0] == 'update' or inp[0] == 'u':
        util.update (client)
    elif inp[0] == 'clear':
        util.clear (client)
    elif inp[0] == 'random':
        util.mpdrandom (client, inp[1])
    elif inp[0] == 'shuffle':
        util.shuffle (client)
    elif inp[0] == 'consume':
        util.consume (client, inp[1])
    elif inp[0] == 'swap':
        util.swap (client, int (inp[1]) - 1, int (inp[2]) - 1)
    elif inp[0] == 'single':
        util.single (client, inp[1])
    elif inp[0] == 'search' or inp[0] == 's':
        if '-f' in inp  or '--filter' in inp:
            l = util.mpdsearch(inp[1], inp, DBlocation, True)
        else:
            l = util.mpdsearch(inp[1], inp, DBlocation, False)
    elif inp[0] == 'a' or inp[0] == 'add':
        if l:
            for line in l:
                client.add (line)
        else:
            print('You have to search first!')
    elif inp[0] == 'q' or inp[0] == 'quit':
        quit()
    return l

def shell (client, DBlocation):
    l = list()
    while 1:
        sys.stdout.write(': ')
        inp = input ()
        readline.get_line_buffer ()
        try:
            client.ping()
        except:
            keepConnected(client)
        if inp:
            l = evaluate(inp, client, l, DBlocation)
        else:
            util.current_status (client)


