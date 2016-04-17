# kpd-client
This is a mpd client written entirely in python.
It offers a shell mode from which you can run commands within the client using a readline library.
It is very similar to mpc in functionality but the with a more efficent and less strict search.

It requires python-musicpd module to run, install it with pip.

`pip3 install python-musicpd`

## Usage
---
After installing the client (with superuser permission)

`python3 setup.py install` 

you can run the client on your console.
At the first run you will be prompted for the configuration file (.kpd.conf on your home).

Here is an example:

```
db = /home/user/.mpd/database
host = localhost
port = 6600
searchmode = pickle
pickle_DB_location = /home/user/.mpd/kpd_db
```

The client has a set of commands that can be explored using `kpd --help`:

```
usage: kpd [-h] [-p [PLAY]] [-n] [-ps] [--stop] [--random [RANDOM]] [-u] [-a]
           [-s [SEARCH]] [-f FILTER [FILTER ...]] [--clear] [-pl]
           [--seek [SEEK]] [--shuffle] [--consume [CONSUME]]
           [--single [SINGLE]] [--swap SWAP SWAP] [--shell]
           [--format FORMAT [FORMAT ...]] [--output] [--no-output]
           [--search-mode [SEARCH_MODE]]

optional arguments:
  -h, --help            show this help message and exit
  -p [PLAY], --play [PLAY]
                        toggle play, input number to play song in playlist
  -n, --next            play next track
  -ps, --previous, --prev
                        play previous track
  --stop                stop playback
  --random [RANDOM]     random [on,off]
  -u, --update          update mpd database
  -a, --add             add songs to playlist, useful to receive piped input
                        from shell
  -s [SEARCH], --search [SEARCH]
                        search a string in the database, casi insensitive
  -f FILTER [FILTER ...], --filter FILTER [FILTER ...]
                        filter search result, can use 'artist', 'album',
                        'title', grep like functionality
  --clear               clear playlist
  -pl, --playlist       show playlist
  --seek [SEEK]         seek current track: works by seconds or by percentage
  --shuffle             shuffle playlist
  --consume [CONSUME]   consume mode [on,off]
  --single [SINGLE]     single mode [on,off]
  --swap SWAP SWAP      swap two tracks in the playlist
  --shell, -sh          invoke shell
  --format FORMAT [FORMAT ...]
                        change display format, write 'kpd --format help' for
                        usage
  --output              print output on screen, default enabled
  --no-output           do not print output on screen
  --search-mode [SEARCH_MODE]
                        switch search mode: options are 'mpd' (mpd database
                        lookup) or 'pickle' (pickle lib serial database,
                        faster)
```

The arguments are evaluated in the exact order they are written in the console.

## Search
---
One of the principal feature is the search.
The search accepts only one string and is case insensitive.
The search can be narrowed using the _filter_ option.
Example:

```
$ kpd --search kyuss -f queen
Kyuss Discography/Albums/(1995) ...and the Circus Leaves Town/09 Size Queen.flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/01 Into the Void.flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/02 Fatso Forgotso.flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/03 Fatso Forgotso Phase II (Flip the Phase).flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/04 If Only Everything.flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/05 Born to Hula.flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/06 Spiders and Vinegaroons.flac
```

```
$ kpd --search kyuss -f queen album circus
Kyuss Discography/Albums/(1995) ...and the Circus Leaves Town/09 Size Queen.flac
```

```
$ kpd --search valley -f artist kyuss
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/01 Gardenia.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/02 Asteroid.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/03 Supa Scoopa and Mighty Scoop.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/04 100°.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/05 Space Cadet.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/06 Demon Cleaner.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/07 Odyssey.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/08 Conan Troutman.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/09 N.O..flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/10 Whitewater.flac
```

The filter flag is case insensitive and accepts _artist, album, title_ option parameters.
Actually the filter flag has grep like functionality so it can filter the search on any parameters, symbols or file extensions included.

### Search Methods
In the configuration file the are some parameters that specifies the search method that the client will use.

```
searchmode = pickle
pickle_DB_location = /home/user/.mpd/kpd_db
```

With these parameters the client will use the [pickle](https://docs.python.org/3.4/library/pickle.html) python library to store the database in serial format.
If `searchmode = mpd` is specified in the config file kpd will search using the zipped mpd database that is stored in the mpd directory.
The pickle method is way faster and strongly suggested, however a secondary database will be written to disk.
