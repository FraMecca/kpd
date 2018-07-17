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
import std.file;

import libmpdclient_extern;

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
        if (!filename.exists) {
            throw new MPDException("Unable to open database.");
        }
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
					auto val = idx > 0 ? to!string(line[idx+2 .. $]) : ""; //idx + 2 to account for ": "
                    sw: switch(key) {
						default:
							break sw;
						case "begin":
							dir = val ~ "/";
							break sw;
						case "end":
							dir = "";
							break sw;
						case "song_begin":
							current = Song();
							current.get!Song.uri = dir ~ val;
							break sw;
						case "song_end":
							assert(current != Song.init, to!string(current) ~ " " ~ dir);
							yield(current);
							break sw;
                        case "playlist_begin":
							current = Playlist();
							current.get!Playlist.uri = dir ~ val;
                            current.get!Playlist.title = val;
                            break sw;
                        case "playlist_end":
							assert(current != Playlist.init, to!string(current) ~ " " ~ dir);
                            yield(current);
                            break sw;
						static foreach(tag; ["artist", "album", "title",
											 "genre", "date", "performer", "composer",
											 "track", "albumArtist", "disc"]) {
							mixin("case \"" ~ tag.capitalize ~ "\": current.get!Song." ~ tag
									~ " = val; break sw;");
						}
					}
				}
			});
		});
	}
}


