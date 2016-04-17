import musicpd
import readline
import sys
import util

def keepConnected(client):
    conn = False
    while conn == False:
        conn = True
        try:
            client.client.connect(client.host, client.port)
        except SocketError as e:
            conn = False
            time.sleep(2)

def evaluate (inp, client, l, DBlocation):
    inp = inp.split (' ')
    client.update_status ()
    status = client.status
    
    
    if len(inp) > 1 and not str(inp[1]):
        inp.pop()
    pass;

def shell (client, DBlocation):
    l = list()
    while 1:
        sys.stdout.write(': ')
        inp = input ()
        readline.get_line_buffer ()
        try:
            client.client.ping()
        except:
            keepConnected(client)
        if inp:
            l = evaluate(inp, client, l, DBlocation)
        else:
            util.current_status (client)
