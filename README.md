# FLFTRender
A small code of libfreetype2 for FLTK RGB image.

![screenshot v0.1.3.x](https://user-images.githubusercontent.com/6948225/79111706-00da9180-7db8-11ea-98ce-cef8a231c402.png)

## Requires

* FLTK-1.3.(5)
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
