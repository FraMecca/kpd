import std.concurrency;
import std.container;
import std.variant;
import std.string;
import std.conv;
import iopipe.textpipe;
import iopipe.zip;
import iopipe.bufpipe;
import std.io;
import std.typecons;

struct Song {
	string artist;	
	string album;
	string title;
	string uri;
	string genre;
	string date;
	string performer;
	string composer;
	string track;
	string albumArtist;
	string disc;
}

struct Playlist {
	string name;
}

class DBParser {
	private string database;

	this(string filename) {
		this.database = filename;
	}
	
	@property Generator!(Algebraic!(Song, Playlist)) all() {
		return new Generator!(Algebraic!(Song, Playlist)) (
		{
			File(database).refCounted.bufd.unzip.runEncoded!((input) {
				string dir;
				Song current;
				foreach(line; input.byLineRange) { 
					auto idx = line.indexOf(":");
					string key = idx > 0 ? to!string(line[0 .. idx]) : to!string(line);
					string val = idx > 0 ? to!string(line[idx .. $]) : ""; // not used if ""
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
							current.uri	= dir ~ val;
							break;
						case "song_end":
							yield(Algebraic!(Song, Playlist)(current));
							current = Song(); // reset song struct
							break;
						static foreach(tag; ["artist", "album", "title", 
											 "genre", "date", "performer", "composer",
											 "track", "albumArtist", "disc"]) { 
							mixin("case \"" ~ tag.capitalize ~ "\": current." ~ tag
									~ " = val;");
						}
					}
				}
			});
		});
	}
}


