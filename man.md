kpd(1) - client for Music Player Daemon (MPD)
========================================

## SYNOPSIS

`kpd` <options> [<arguments>]

## DESCRIPTION 

`kpd` is a client for MPD, the Music Player Daemon.
kpd connects to a MPD istance ad controls it according to commands issued.
If no command is given, the current status is printed.

## OPTIONS

`-f,--format`

	Configure the format of song display for status and the playlist. The metadata delimiters are "%name%", "%artist%", "%album%", "%title%", "%track%", "%time%", and "%file%". The [] 
	operator is used to group output such that if no metadata delimiters are found or matched between '[' and ']', then none of the characters between '[' and ']' are output. '&' and '|' 
	are logical operators for and and or. '#' is used to escape characters. Some useful examples for format are: "%file%" and "[[%artist% - ]%title%]|[%file%]". This command also takes 
	the following defined escape sequences:
    	\\ - backslash
    	\a - alert
    	\b - backspace
    	\t - tab
    	\n - newline
    	\v - vertical tab
    	\f - form-feed
    	\r - carriage return 

	
`-v, --verbose` <on|off>

	Enable or disable output on screen

`--host`

	The host to connect to. If not given `kpd` will use the value set on `settings.h`

`--port`

	The port to connect to. If not given `kpd` will use the value set on `settings.h`

`-a, --add`

	Add a song from the search results to the playlist. Can also read input from pipes.

`--clear`

	Empties the playlist

`--consume` <on|off|true|false|0|1>

	Toggle consume mode if state is not specified

`delete`
	Aspettando Galla

`-n, --next`

	Starts playing next song

`-P, --pause`

	Pauses playing

`-p, --play` <position>

	Starts playing the song on playlist specified by the number. If no position is specified, toggles play

`-b, --prev, --previous`

	Starts playing previous song on playlist

`--random` <on|off|true|false|0|1>

	Toggle random mode if state is not specified

`--seek`
	Aspettando Eug

`--single` <on|off|true|false|0|1>

	Toggle single mode if state is not specified

`--shuffle`

	Shuffles all songs on the playlist

`--swap`
	Aspettando Gab

`--update`

	Scans music directory for updates

## RETURN VALUES

## BUGS

Report bugs to me@francescomecca.eu


## AUTHOR

Eugenio Corso, Gabriele Corso, Francesco Galla, Francesco Mecca
