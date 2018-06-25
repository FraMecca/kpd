import std.concurrency;
import std.container;
//import std.variant;
import std.string;
import std.conv;
import std.typecons;
import taggedalgebraic;
import iopipe.textpipe;
import iopipe.zip;
import iopipe.bufpipe;
import std.io;

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
	string uri;
    string title;
}

private union Base { Song song; Playlist playlist; };
alias DBUnion = TaggedAlgebraic!Base;
//alias RT = Algebraic!(Song, Playlist);

import std.stdio : writeln;

class DBParser {
	private string database;

	this(string filename) {
		this.database = filename;
	}
	
	@property Generator!DBUnion all() {
		return new Generator!DBUnion (
		{
			File(database).refCounted.bufd.unzip.runEncoded!((input) {
				string dir;
				DBUnion current;
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
							current.get!Song.uri = dir ~ val;
							break;
						case "song_end":
							assert(current != Song.init, to!string(current) ~ " " ~ dir);
							yield(current);
							break;
                        case "playlist_begin":
							current = Playlist();
							current.get!Playlist.uri = dir ~ val;
                            current.get!Playlist.title = val;
                            break;
                        case "playlist_end":
							assert(current != Playlist.init, to!string(current) ~ " " ~ dir);
                            yield(current);
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


