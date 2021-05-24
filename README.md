# FLFTRender
A small code of libfreetype2 for FLTK RGB image.

![screenshot v0.2.0.1](https://user-images.githubusercontent.com/6948225/80074172-c0cb9980-8583-11ea-9b0a-a9d7bbc2dd22.png)

## Requires

* FLTK-1.3.(5) or latest ( tested on 1.4.0.6 )
* [libfreetype2](https://www.freetype.org/download.html)

## libfreetype2 configure

* MinGW-W64 user should configure libfreetype2 with this:
```
$ ./configure --with-harfbuzz=no
```

## Additional required libraries

* MinGW-W64 may need these libraries must be linked together.
    1. graphite2
    2. rpcrt4
    2. bz2
    3. z
    4. (optional)png
