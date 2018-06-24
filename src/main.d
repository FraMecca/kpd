import std.stdio;
import std.string;
import std.getopt;

import database;
import libmpdclient_extern;

void main(string[] args)
{
	writeln("bela");
	string host = "localhost";
	short port = 6600;

	auto conn = MPDConnection(host, port);
	conn.play;
	conn.consume(true);
	conn.repeat(false);
	conn.next;
	conn.stop;
	auto gen = new DBParser("/home/user/.mpd/database");
	foreach (elem; gen.all) {
		final switch(elem.kind){
			case DBUnion.Kind.song:
				writeln("Song:", elem.toString);
				break;
			case DBUnion.Kind.playlist:
				writeln("Playlist:", elem.toString);
				break;
		}
	}
}
