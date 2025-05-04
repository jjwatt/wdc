# wdc

warp-dir in C. A simple directory bookmarking program using nob.h as the build system. The main program uses nob.h as a "library," too, taking advantage of its Dynamic Arrays and stuff like String_Builder.

## Usage

```
    "Usage: %s [OPTIONS]
        Options:
            -h, --help          Show this help message and exit
            -l, --list          List bookmarks
            -a, --add [name]    Add current directory with name
```

`wdc` is meant to be used with finders like `fzy`[^1]. e.g.,

```console
goto() {
    cd "$(wdc --list | fzy | cut -d'|' -f2)" || return
}
```

[^1]: https://github.com/jhawthorn/fzy

