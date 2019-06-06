import std.json;

import database;

JSONValue jsonEncode(const DBUnion src)
{
    JSONValue ret = JSONValue(["":""]);

    enum songTags = ["uri", "artist", "album", "title",
                            "genre", "date", "performer", "composer",
                     "track", "albumArtist", "disc"];
    enum playlistTags = ["uri", "title"];
    
    if(src.kind == DBUnion.Kind.playlist){
        ret.object["type"] = JSONValue("playlist");
        static foreach(tag; playlistTags) {
            mixin("ret.object[\"" ~ tag ~ "\"] = JSONValue(src." ~ tag ~ ");");
        }
    }
    else{
         ret.object["type"] = JSONValue("song");
        static foreach(tag; songTags) {
            mixin("ret.object[\"" ~ tag ~ "\"] = JSONValue(src." ~ tag ~ ");");
        }
    }

    ret.object.remove(""); // TODO initialize as object instead of this
    return ret;
}
