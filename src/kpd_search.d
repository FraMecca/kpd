import std.algorithm;
import std.traits;
import std.typecons;
import taggedalgebraic;
import std.string;

import database;

enum Tag {ARTIST, ALBUM, TITLE, URI, GENRE, PERFORMER,COMPOSER, TRACK, ALBUMARTIST,
    DISC, ALL, PLAYLIST};

// convert to lowercase and find
auto find = (string a, string b) => a.toLower.canFind(b);

bool search(DBUnion target, Tuple!(Tag, string)[] queries)
{
    // fails on first miss, true only if all queries satisfied
    foreach(q; queries) {
        auto tag = q[0];
        auto val = q[1].toLower;
        bool found = false;
        switch(tag) {
            default:
                assert(0);
            case Tag.PLAYLIST:
                if(!(target.kind is DBUnion.Kind.playlist &&
                        (target.get!Playlist.uri.find(val) ||
                         target.get!Playlist.title.find(val)))) return false;
                break;
            case Tag.ALL:
                if(target.kind !is DBUnion.Kind.song) return false;
                if(target.get!Song.artist.find(val) ||
                   target.get!Song.album.find(val) ||
                   target.get!Song.title.find(val) ||
                   target.get!Song.uri.find(val) ||
                   target.get!Song.genre.find(val) ||
                   target.get!Song.title.find(val) ||
                   target.get!Song.performer.find(val) ||
                   target.get!Song.composer.find(val) ||
                   target.get!Song.track.find(val) ||
                   target.get!Song.albumArtist.find(val) ||
                   target.get!Song.disc.find(val)) found = true; 
                if(!found) return false;
                break;
            static foreach(t; __traits( allMembers,Tag)) {
                static if(t != "ALBUMARTIST" && t != "ALL" && t != "PLAYLIST")
                    mixin("case Tag."~t~":" ~
                        "if(target.kind !is DBUnion.Kind.song) return false;" ~
                        "if(!target.get!Song."~t.toLower~".find(val)) return false;");
            }
        }
    }
    return true;
}
