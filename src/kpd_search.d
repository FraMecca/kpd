import std.algorithm;
import std.traits;
import std.typecons;
import taggedalgebraic;
import std.string;

import std.conv;
import std.stdio; // debug

import database;

enum Tag {ARTIST, ALBUM, TITLE, URI, GENRE, PERFORMER,COMPOSER, TRACK, ALBUMARTIST,
    DISC, ALL, PLAYLIST};

// convert to lowercase and find
auto find = (string a, string b) => a.toLower.canFind(b.toLower);

bool search(DBUnion target, Tuple!(Tag, string)[] queries)
{
    if(queries.length == 0) return false; // if search is not specified
    // fails on first miss, true only if all queries satisfied
    foreach(q; queries) {
        auto tag = q[0];
        auto val = q[1];
        bool found = false;
        sw: switch(tag) {
            default:
                assert(0);
            case Tag.PLAYLIST:
                if(!(target.kind is DBUnion.Kind.playlist &&
                        (target.get!Playlist.uri.find(val) ||
                         target.get!Playlist.title.find(val)))) return false;
                break sw;
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
                break sw;
            static foreach(t; __traits( allMembers,Tag)) {
                static if(t != "ALBUMARTIST" && t != "ALL" && t != "PLAYLIST")
                    mixin("case Tag."~t~":" ~
                        "if(target.kind !is DBUnion.Kind.song) return false;" ~
                        "if(!target.get!Song."~t.toLower~".find(val)) return false; break sw;");
            }
        }
    }
    return true;
}

Tuple!(Tag, string)[] parseQueries(string[] terms) 
{
    Tuple!(Tag,string)[] ret;
    foreach(t; terms) {
        auto idx = t.indexOf(":");
        if (idx > -1 && t[idx-1] != '\\') {
            auto tagSt = t[0..idx];
            auto qst = t[idx+1..$];
            ret ~= tuple(to!Tag(tagSt.toUpper), qst);
        } else {
            ret ~= tuple(Tag.ALL, t);
        }
    }
    return ret;
}
