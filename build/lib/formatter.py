import util, re 

def display_help ():
    print ('''Configure the format of song display for status and the playlist. The metadata delimiters are "%name%", "%artist%", "%album%", "%title%", "%track%", "%time%", and "%file%".\n The [] operator is used to group output such that if no metadata delimiters are found or matched between \'[\' and \']\', then none of the characters between \'[\' and \']\' are output. \nSome useful examples for format are: "%file%" and "[[%artist% - ]%title%]|[%file%]". This command also takes the following defined escape sequences:
           \\ - backslash
           \\a - alert
           \\b - backspace
           \\t - tab
           \\n - newline
           \\v - vertical tab
           \\r - carriage return''')


# artist, 
# name,
# album,
# title,
# track
# total
# time
# file


def raise_error (string):
    print ('there is no ' + string + ' entry in song metadata')
    exit (1)

def check_album (song):
    if not 'album' in song:
        raise_error ('album')
def check_name (song):
    if not 'name' in song:
        raise_error ('name')
def check_title (song):
    if not 'title' in song:
        raise_error ('title')
def check_track (song):
    if not 'track' in song:
        raise_error ('track')
def check_artist (song):
    if not ('artist') in song:
        raise_error ('artist')
options = {'%artist%': check_artist, '%album%': check_album, '%title%': check_title, '%track%': check_track}
options2 = ['%file%', '%time%', '%total%']

slashLetters = {'\\n':'\n', '\\v':'\v', '\\t':'\t', '\\r':'\r', '\\f':'\f', '\\a':'\a'}


def print_status (client, formato):

#horrendous help
    if formato == 'help':
        display_help ()
        exit ()
#end

    song = client.currentsong ()
    status = client.status ()
    time = status ['time']
    time = time.split (':')

    keys = options.keys ()
    for key in keys:
        if key in formato and not str ('[' + key + ']') in formato:
            options[key] (song) #exit if not in metadata
        if key in formato:
            try:
                tmpKey = key.replace ('%', '')
                formato = formato.replace (key, str (song[tmpKey]))
            except KeyError:
                pass


    keys = slashLetters.keys ()
    for key in keys:
        if key in formato:
            formato = formato.replace (key, slashLetters[key])
    #latest entries
    for entry in options2:
        if entry in formato:
            if entry == '%file%':
                formato = formato.replace (entry, song['file'])
            if entry == '%time%':
                elapsedTime = util.convert_time (time[0])
                formato = formato.replace (entry, elapsedTime)
            if entry == '%total%':
                totalTime = util.convert_time (time[1])
                formato = formato.replace (entry, totalTime)



    formato = formato.replace ('[', '')
    formato = formato.replace (']', '')
    print (formato)
