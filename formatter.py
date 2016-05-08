import util, re 

def display_help ():
    print ('''Configure the format of song display for status and the playlist. The metadata delimiters are "%name%", "%artist%", "%album%", "%title%", "%track%", "%time%", and "%file%".\n The [] operator is used to group output such that if no metadata delimiters are found or matched between \'[\' and \']\', then none of the characters between \'[\' and \']\' are output. \nSome useful examples for format are: "%file%" and "[%artist% - ]%title%". This command also takes the following defined escape sequences:
           \\ - backslash
           \\a - alert
           \\b - backspace
           \\t - tab
           \\n - newline
           \\v - vertical tab
           \\r - carriage return
Note that in case the song hasn't the correspective metadata attribute, the filename will be printed''')

options = ['%artist%', '%album%', '%title%', '%file%']
options2 = ['%time%', '%total%', '%tracks%', '%track%', '%status%']

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

    for key in options:
        if key in formato:
            try:
                tmpKey = key.replace ('%', '')
                keyParentesi = ''.join (['[', key, ']'])
                try:
                    formato = formato.replace (keyParentesi, str (song[tmpKey]))
                except KeyError:
                    formato = formato.replace (key, str (song[tmpKey]))
                    print (formato)
            except KeyError:
                if not keyParentesi in formato:
                    formato = formato.replace (key, song['file'])
                else:
                    formato = formato.replace (keyParentesi, '')

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
            if entry == '%tracks%':
                formato = formato.replace (entry, status['playlistlength'])
            if entry == '%track%':
                formato = formato.replace (entry, str (int (status['song']) + 1))
            if entry == '%status%':
                formato = formato.replace (entry, status['state'])

    if formato == '':
        formato = song['file']
    print (formato)
