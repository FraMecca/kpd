import std.stdio;
import std.string;
import std.getopt;
import std.typecons;
import std.algorithm;
import std.getopt;
import std.ascii;
import std.conv;

import kpd_search;
import database;
import libmpdclient_extern;


void main(string[] args)
{
    // TODO config file

    bool add;
    bool previous;
    string backward;
    string forward;
    bool clear;
    bool consume;
    ulong del;
    ulong[] delRange;
    bool list;
    bool next;
    bool uris;
    string play;
    bool pause;
    bool random;
    bool repeat;
    ulong[] shuffleRange;
    bool single;
    bool stop;
    string[] searchTerms;
    string seek;
    bool shuffle;
    bool update;
    ulong[] swap;
    bool usage;

	string host = "192.168.1.131";
	short port = 6600;
    auto conn = MPDConnection(host, port); 

    void play_handler()
    {
        if(args.length == 1) {
            conn.play();
        } else {
            auto nextArg = args[1];
            conn.play(to!int(nextArg)); 
        }
    }

    try {
        auto rslt = getopt(args, "add|a",  "add to current playlist", &add,
                                 "previous|b|prev",  "play previous song", &conn.previous,
                                 "backward|B",  "seek backwards of nsec or %song", &backward,
                                 "forward|F",  "seek forward of nsec or %song", &forward,
                                 "clear|c",  "clear current playlist", &clear,
                                 "consume",  "toggle consume", &conn.consume,
                                 "del|d",  "del specified item from playlist", &del,
                                 "del-range|D",  "del specified range from playlist", &delRange,
                                 "list|l",  "display current playlist", &list,
                                 "next|n",  "play next song", &next,
                                 "display-uri|U",  "display URIs", &uris,
                                 "play|p",  "toggle play or play specified item",
                                 &play_handler,
                                 "pause|P", "pause", &conn.pause,
                                 "random|r",  "toggle random", &random,
                                 "repeat",  "toggle repeat", &repeat,
                                 "shuffleRange",  "shuffle given range", &shuffleRange,
                                 "single",  "toggle single", &single,
                                 "stop",  "toggle stop", &stop,
                                 "search|s",  "search given key(s)", &searchTerms,
                                 "shuffle|t",  "shuffle current playlist", &shuffle,
                                 "update",  "send update request to MPD", &update,
                                 "swap",  "swap specified items", &swap,
                          );
        if(rslt.helpWanted) {
            defaultGetoptPrinter("kpd is a client for MPD.", rslt.options);
            return;
        }
        auto gen = new DBParser("/home/fra/.mpd/mpd.db");
        //writeln(gen.all.filter!(a => search(a, [tuple(Tag.ALL, "tool")])));
    } catch (Exception e){
        stderr.writeln(e.msg);
    } 
}
