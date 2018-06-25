import std.stdio;
import std.range;
import std.string;
import std.getopt;
import std.typecons;
import std.algorithm;
import std.conv;
import std.exception;

import kpd_search;
import database;
import libmpdclient_extern;


void main(string[] args)
{
    // TODO config file
    try {

		auto oargs = args.idup;
    	bool add;
    	string backward;
    	string forward;
    	ulong[] delRange;
    	bool list;
    	bool uris;
    	ulong[] shuffleRange;
    	string[] searchTerms;
    	string seek;
    	ulong[] swap;

		//string host = "192.168.1.131";
		string host = "localhost";
		short port = 6600;
    	auto conn = MPDConnection(host, port); 

    	void opt_handler(string opt)
    	{
    		// split long and short option and get original occurrence
    		auto div = opt.indexOf("|");
    		assert(div > 0);
            auto lng = "--" ~ opt[0 .. div]; auto sht = "-" ~ opt[div+1 .. $];
            assert(sht.length < lng.length);
            auto idx = oargs.countUntil(sht);
            long eqIdx = -1;
            if(idx == -1) idx = oargs.countUntil(lng);
            // manage options such as --play=2
            if(idx == -1) {
            	foreach(i, param; oargs.enumerate){
            		if(param.canFind(sht~"=") || param.canFind(lng~"=")){
            			idx = i;
            			eqIdx = oargs[idx].indexOf("=");
            			break;
            		}
            	}
            }
            assert(idx > 0, to!string(idx));
            assert(eqIdx >= -1);

        	if(oargs.length == idx + 1 && eqIdx == -1) {
        		switch(opt){
        			case "play|p":
            			conn.play();
            			break;
            		default:
            			throw new GetOptException("Missing value for argument " ~ lng ~ ".");
            	}
        	} else {
				string arg1 = eqIdx != -1 ? oargs[idx][eqIdx+1 .. $] : oargs[idx+1];
				string arg2;
        		switch(opt){
        			case "play|p":
            			conn.play(to!int(arg1)); 
            			break;
        			case "del|d":
        				conn.del(to!int(arg1));
        				break;
        			case "del-range|D":
            			enforce(oargs.length >= idx+2, 
            					new GetOptException("Missing second value for argument " ~ lng ~ "."));
        				arg2 = oargs[idx+2];
        				writeln(arg1, to!int(arg2));
        				break;
            		default:
            			throw new GetOptException("Missing value for argument " ~ lng ~ ".");
            	}
        	}
    	}

        auto rslt = getopt(args, config.caseSensitive,
        						 "add|a",  "add to current playlist", &add,
                                 "previous|b|prev",  "play previous song", &conn.previous,
                                 "backward|B",  "seek backwards of nsec or %song", &backward,
                                 "forward|F",  "seek forward of nsec or %song", &forward,
								 "clear|c",  "clear current playlist", &conn.clear,
								 "consume",  "toggle consume", &conn.consume,
								 "del|d",  "del specified item from playlist", &opt_handler,
                                 "del-range|D",  "del specified range from playlist", &opt_handler,
								 "list|l",  "display current playlist", &list,
								 "next|n",  "play next song", &conn.next,
								 "display-uri|U",  "display URIs", &uris,
                                 "play|p",  "toggle play or play specified item", &opt_handler,
								 "pause|P", "pause", &conn.pause,
								 "random|r",  "toggle random", &conn.random,
								 "repeat",  "toggle repeat", &conn.repeat,
								 "single",  "toggle single", &conn.single,
								 "stop",  "toggle stop", &conn.stop,
                                 "search|s",  "search given key(s)", &searchTerms,
								 "shuffle|t",  "shuffle current playlist", &conn.shuffle,
                                 "shuffleRange",  "shuffle given range", &shuffleRange,
								 "update",  "send update request to MPD", &conn.update,
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
