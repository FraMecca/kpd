# kpd-client
This is a rewrite for an mpd client called kpd (Kyuss Music Player, python_old branch)
It is very similar to mpc in functionality but the with a more efficent and less strict search.

kpd uses [libmpdclient](https://www.musicpd.org/libs/libmpdclient/)

## Usage
---

```
dub build -b release
```

you can run the client on your console.
At the first run you will be prompted for the configuration file (.kpdrc on your home).

Here is an example:

```
host "localhost"
port 6600
db "/home/user/.mpd/database"
```

The client has a set of commands that can be explored using `kpd --help`.

## Search
---
One of the principal feature is the search that differs from most of the other mpd clients.
The search accepts only one comma divided string and is case insensitive.
You can specify artist, album and other metadata by writing `album:key`
Example:

```
$ kpd --search kyuss,queen
Kyuss Discography/Albums/(1995) ...and the Circus Leaves Town/09 Size Queen.flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/01 Into the Void.flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/02 Fatso Forgotso.flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/03 Fatso Forgotso Phase II (Flip the Phase).flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/04 If Only Everything.flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/05 Born to Hula.flac
Kyuss Discography/EPs, Singles, & Splits/(1997) Kyuss - Queens of the Stone Age/06 Spiders and Vinegaroons.flac
```

```
$ kpd --search kyuss,queen,album:circus
Kyuss Discography/Albums/(1995) ...and the Circus Leaves Town/09 Size Queen.flac
```

```
$ kpd --search valley,artist:kyuss -f conan,water
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/01 Gardenia.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/02 Asteroid.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/03 Supa Scoopa and Mighty Scoop.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/04 100°.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/05 Space Cadet.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/06 Demon Cleaner.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/07 Odyssey.flac
Kyuss Discography/Albums/(1994) Welcome to Sky Valley/09 N.O..flac
```


### Search Methods
The search method has been rewritten in C in a simplier way: it just looks inside the gzipped default mpd database.
The search is way less strict: case-insensitive, no type field.

### Add result
The result of the search can be added to the playlist with `--add` or `-a` arguments. kpd can also be used in a pipe like this:

```
$ kpd -U -s kyuss -f queen hula | kpd -a
```
but skips some commands such as `-f` and accepts only full URIs.
