import std.concurrency;
import std.container;
import std.variant;
import std.string;
import std.conv;
import iopipe.textpipe;
import iopipe.zip;
import iopipe.bufpipe;
import std.io;

class DBParser {
	private:
		string database;
	
	public:
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

		this(string filename) {
			this.database = filename;
		}
		
		@property Generator!(Algebraic!(Song, Playlist)) getSongs() {
			return new Generator!(Algebraic!(Song, Playlist)) (
			{
				File(database).bufd.unzip.runEncoded!((input) {
					string dir;
					Song current;
					foreach(line; input.byLine()) { 
						string[] content = to!(string[])(line.split(":"));
						assert(content.length == 2, to!string(content));
						string key = content[0];
						string val = content[1];
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
								yield(current);
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


