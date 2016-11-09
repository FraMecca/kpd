kpd(1) - client for Music Player Daemon (MPD)
========================================

## SYNOPSIS

`kpd` <options> [<arguments>] <options> [<arguments>] ... ...

## DESCRIPTION 

`kpd` is a client for MPD, the Music Player Daemon.
kpd connects to a MPD istance ad controls it according to commands issued.
If no command is given, the current status is printed.

## OPTIONS

`-V, --verbose` <on|off>

	Enable or disable output on screen

`--host`

	The host to connect to. If not given `kpd` will use the value set on `settings.h`

`--port`

	The port to connect to. If not given `kpd` will use the value set on `settings.h`

`-a, --add`

	Add a song from the search results to the playlist. Can also read input from pipes. Doesn't accept arguments.

`--clear`

	Empties the playlist

`--consume` <on|off|true|false|0|1>

	Toggle consume mode if state is not specified

`delete`
	Aspettando Galla

`delete range`
	Aspettando Galla

`-h, --help`

		Show help

`-l, --list`

		Prints entire playlist

`-m, --move`
	COMPLETa

`-n, --next`

		Starts playing next song

`-P, --pause`

	Pauses playing

`-p, --play` <position>

	Starts playing the song on playlist specified by the number. If no position is specified, toggles play

`-b, --prev, --previous`

	Starts playing previous song on playlist
`-N, --full-names` <on|off|true|false|0|1>
	
	Print song uri instead title, artist and album

	

`--random` <on|off|true|false|0|1>

	Toggle random mode if state is not specified

`--repeat` <on|off|true|false|0|1>

	Toggle repeat mode if state is not specified

`--stop`

	Stops playing

`-S, --seek`
	Aspettando Eug

`--single` <on|off|true|false|0|1>

	Toggle single mode if state is not specified

`--shuffle`

	Shuffles all songs on the playlist

`--shuffle-range`
	COMPLETA

`--swap`
	Aspettando Gab

`--update`

	Scans music directory for updates

`-V, --verbose` <on|off|true|false|0|1>

	Disable (redirect to /dev/null) or enable output. The state must be specified

	
	

## RETURN VALUES

`0`
	Success

`64`
	Error in command line. Remember that arguments that aren't recognized are ignored

`69`
	Can't open connection to MPD. Check if MPD is running or host is reachable 

## BUGS

Report bugs to me@francescomecca.eu


## AUTHOR

Eugenio Corso, Gabriele Corso, Francesco Galla, Francesco Mecca
