import pegged.grammar;
import sumtype;

import std.string;
import std.exception: enforce;

@safe pure
uint n_values_on_argument(const ParseTree node)
in{
    assert(node.name == "Cli.Argument", "Not an argument node");
}do{
    uint nchildren = 0;

    @safe pure void rec(const ParseTree childnode){
        if(childnode.name == "Cli.Value") // they can't have Cli.Value as children
            nchildren++;
        else
            foreach(c; childnode.children)
                rec(c);
    }

    foreach(c; node.children)
        rec(c);

    return nchildren;
}

@safe pure
void get_single_argument(T)(const ParseTree node, ref uint nchildren, ref T fst)
in{
    assert(node.name == "Cli.Argument", "Not an argument node");
}do{
    import std.conv: to;
    T value;
    @safe pure void rec(const ParseTree childnode){
        if(childnode.name == "Cli.Value"){ // they can't have Cli.Value as children
            nchildren++;
            fst = to!T(childnode.matches[0]);
        }
        else
            foreach(c; childnode.children)
                rec(c);
    }

    foreach(c; node.children)
        rec(c);
}

@safe pure
void get_double_argument(T, E)(const ParseTree node, ref uint nchildren, ref T fst, ref E snd)
in{
    assert(node.name == "Cli.Argument", "Not an argument node");
}do{
    import std.conv: to;
    T value;
    @safe pure void rec(const ParseTree childnode){
        if(childnode.name == "Cli.Value"){ // they can't have Cli.Value as children
            nchildren++;
            if(nchildren == 1)
                fst = to!T(childnode.matches[0]);
            else if(nchildren == 2)
                snd = to!E(childnode.matches[0]);
        }
        else
            foreach(c; childnode.children)
                rec(c);
    }

    foreach(c; node.children)
        rec(c);
}

enum sanityCheck = `
    import std.conv:to;
    assert(node.name == "Cli.Argument", "Not an argument node");
    assert(node.matches.length >= 2, "Can't match on sid or lid");
    string _e = "Wrong match on " ~sid~"|"~lid~" from "~node.matches.to!string;
    assert(node.matches[1] == lid || node.matches[1] == sid, _e);
`;
string noArgumentCode(const string structname){
    return `
    static CliArgument make(const ParseTree node)
    {` ~ sanityCheck ~ `
    if(n_values_on_argument(node) != 0)
        return CliArgument(InvalidArgument("Too many arguments for option '"~lid~"'."));
    else
        return CliArgument(`~structname~`());
    }
}
    `;
}
string singleArgumentCode(string op)(const string structname, const string fst, const string value){
    string valstring = fst~` fst = ` ~value~`;`;
    static if(op == "single")
        enum check =  ` if(n != 1)return CliArgument(InvalidArgument("Option '"~lid~"' requires exactly one argument."));`;
    else {
        static assert(op == "optional");
        enum check =  ` if(n != 0 && n != 1)return CliArgument(InvalidArgument("Option '"~lid~"' requires exactly zero or one argument."));`;
    }
    return `
    static CliArgument make(const ParseTree node)
    {` ~ sanityCheck ~ `
        uint n = 0; `~ valstring ~`
        get_single_argument!`~fst~`(node, n, fst);`~
            check ~ `
    else
        return CliArgument(`~structname~`(fst));
    }
}
    `;
};
string doubleArgumentCode(const string structname, const string fst, const string snd){
    return `
    static CliArgument make(const ParseTree node)
    {` ~ sanityCheck ~ `
        uint n = 0;
        `~fst~ ` fst; `~snd~` snd;
        get_double_argument!(`~fst~","~snd~`)(node, n, fst, snd);
    if(n != 2)
        return CliArgument(InvalidArgument("Option '"~lid~"' requires exactly two arguments."));
    else
        return CliArgument(`~structname~`(fst, snd));
    }
}
    `;
};

ArgCode noArgument(const string structname, immutable string lid, const string sid){
    // static assert(lid != "", "Long argument needed."); // TODO cannot be read at CT
    auto mx = `struct `~structname~`{ static string sid ="`~sid~`"; static string lid ="`~lid~`";`~ 
        noArgumentCode(structname);
    return ArgCode(structname, lid, sid, mx);
}
ArgCode singleArgument(const string structname, immutable string lid, const string sid, string type){
    // static assert(lid != "", "Long argument needed."); // TODO cannot be read at CT
    auto mx = `struct `~structname~`{ static string sid ="`~sid~`"; static string lid ="`~lid~`";`~
        type~` value; ` ~
        singleArgumentCode!"single"(structname, type, type~".init");
    return ArgCode(structname, lid, sid, mx);
}
ArgCode optionalArgument(T)(const string structname, immutable string lid, const string sid, string type, T value){
    import std.conv: to;
    // static assert(lid != "", "Long argument needed."); // TODO cannot be read at CT
    auto mx = `struct `~structname~`{ static string sid ="`~sid~`"; static string lid ="`~lid~`";`~
        type~` value ; ` ~
        singleArgumentCode!"optional"(structname, type, to!string(value));
    return ArgCode(structname, lid, sid, mx);
}

ArgCode doubleArgument(const string structname, immutable string lid, const string sid, string fst, string snd){
    // static assert(lid != "", "Long argument needed."); // TODO cannot be read at CT
    string mx = `struct `~structname~`{ static string sid ="`~sid~`"; static string lid ="`~lid~`";`~
        fst~` fst; ` ~
        snd~` snd; ` ~
        doubleArgumentCode(structname, fst, snd);
    return ArgCode(structname, lid, sid, mx);
}



/////// More readable code

struct ArgCode{
    string structtype;
    string lid;
    string sid;
    string mixinResult;
}

mixin(grammar(`Cli:
    Cli <- Expression*

    Expression <- (Argument|Values|Stdin) space* Expression*

    Argument <- ('-' Sid|'--' Lid) space* Values*

    Sid <- [A-z]
    Lid <- identifier

    Values <- (Value space* Values|Value)

    Value <- (identifier|[0-9]+)

    Stdin <- '-'
`));

string parseCliGen(ArgCode[] args ...)
{
    string structs;
    foreach(arg; args){
        structs ~= arg.mixinResult;
    }

    string types = "InvalidArgument, ";
    foreach(arg; args){
        types ~= arg.structtype ~ ", ";
    }
    string sumtp = `alias CliArgument = SumType!(`~types~`);`;

    string parse = `CliArgument[] parseCli(const string[] argv){
                        immutable args = argv.join(' ');

                        ParseTree pt = Cli(args);
                        enforce(pt.successful, "TODO better msg");
                        typeof(return) res;

                        void recur(ParseTree node){
                            if(node.name == "Cli.Argument"){
                                switch(node.matches[1]){
                                default:
                                   assert(false);
    `;
    foreach(arg; args){
        parse ~= `case "`~arg.lid~`":
    `;
    if(arg.sid != "")
        parse ~= `case "`~arg.sid~`":
    `;
        parse ~= `res ~= `~arg.structtype ~`.make(node); break;
    `;
    }
    parse ~= `}
                            } else {
                                foreach(n; node.children){
                                    recur(n);
                                }
                            }
                        }
                        foreach(node; pt.children){
                            recur(node);
                        }
            return res;}
    `;

    string res = structs ~ sumtp ~ parse;
    return res;
}

struct InvalidArgument{
    string why;
}

enum opts = parseCliGen(noArgument("Quiet", "quiet", "q"),
                        optionalArgument("Play", "play", "p", "int", 5),
                        singleArgument("Consume", "consume", "", "string"),
                        doubleArgument("Swap", "swap", "", "int", "int"));
mixin(opts);
void main()
{
    import std;

    auto argv =  "dub-run --quiet --play --swap 2 2 --swap 9 8 2 --consume on --play 2".split(' ');
    auto res = parseCli(argv[1..$]);
    foreach(got, expected; zip(res, variantArray(Quiet(), Play(5), Swap(2, 2),
                                                 InvalidArgument("Option 'swap' requires exactly two arguments."),
                                                 Consume("on"), Play(2)))){

        assert(got == expected, got.toString~":"~expected.toString);
    }

}
