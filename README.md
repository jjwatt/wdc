# wdc

warp-dir in C. A simple directory bookmarking program using nob.h as the build system. The main program uses nob.h as a "library," too, taking advantage of its Dynamic Arrays and stuff like String_Builder.

There's only one point of configuration. You can set WDC_BOOKMARK_FILE to be where bookmarks are recorded and read from. If you don't set it, it uses the default of `~/.wdc`.

## Usage

```
    "Usage: wdc [OPTIONS]
        Options:
            -h, --help          Show this help message and exit
            -l, --list          List bookmarks
            -a, --add [name]    Add current directory with name
            -p, --pop           Pop the last item off the list and return the dir
```

`wdc` is meant to be used with finders like `fzy`[^1]. e.g.,

```console
goto() {
    cd "$(wdc --list | fzy | cut -d'|' -f2)" || return
}
```

[^1]: https://github.com/jhawthorn/fzy

