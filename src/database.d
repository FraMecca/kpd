import std.concurrency;
import std.container;
import std.variant;
import std.string;
import std.conv;
import std.typecons;
import std.algorithm.mutation : move;
import taggedalgebraic;
import iopipe.textpipe;
import iopipe.zip;
import iopipe.bufpipe;
import std.io;

struct Song {
	string artist;	
	string album;
	string title;
	@property string uri;
	string genre;
	string date;
	string performer;
	string composer;
	string track;
	string albumArtist;
	string disc;
	~this(){}
}

struct Playlist {
	string name;
}

import std.stdio : writeln;

class DBParser {
	private string database;

	this(string filename) {
		this.database = filename;
	}

	union Base { Song s; Playlist p; };
	alias RT = TaggedAlgebraic!Base;
	
	@property Generator!RT all() {
		return new Generator!RT (
		{
			File(database).refCounted.bufd.unzip.runEncoded!((input) {
				string dir;
				RT current;
				foreach(line; input.byLineRange) { 
					auto idx = line.indexOf(":");
					string key = idx > 0 ? to!string(line[0 .. idx]) : to!string(line);
					string val = idx > 0 ? to!string(line[idx+2 .. $]) : ""; //idx + 2 to account for ": "
					switch(key) {
						default:
							break;
						case "begin":
							dir = val ~ "/";
							break;
						case "end":
							dir = "";
							break;
						case "song_begin":
							current = Song();
							current.uri = dir ~ val;
							break;
						case "song_end":
							yield(move(current));
							break;
						static foreach(tag; ["artist", "album", "title", 
											 "genre", "date", "performer", "composer",
											 "track", "albumArtist", "disc"]) { 
							mixin("case \"" ~ tag.capitalize ~ "\": current.get!Song." ~ tag
									~ " = val;");
						}
					}
				}
			});
		});
	}
}


