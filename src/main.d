import std.stdio;
import std.string;
import std.getopt;

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
	writeln(conn.status);
}
