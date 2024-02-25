[![License](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://github.com/robert-ancell/microflash/blob/master/COPYING)

This is the source code to *Microflash* — a graphical application to
flash programs to the [micro:bit](https://microbit.org/). If you
install Microflash after downloading a .hex file you will be able to
download it to the micro:bit in one click.

![Screenshot](https://raw.githubusercontent.com/robert-ancell/microflash/master/screenshot.png)

# Installing

On an Ubuntu 18.04 LTS machine enter the following:
```
$ sudo add-apt-repository ppa:robert-ancell/microflash-stable
$ sudo apt update
$ sudo apt install microflash
```

There is also a `microflash-edge` PPA available if you want to get the
latest builds.

# Building from source

Get the source:
```
$ git clone https://github.com/robert-ancell/microflash.git
$ cd microflash
```

Build and run:
```
$ meson --prefix $PWD/_install _build/
$ ninja -C _build/
$ ./_build/src/microflash
```

# Contributing

To contribute code to Microflash make a [pull request](https://github.com/robert-ancell/microflash/pulls).
If you find issues please [report them](https://github.com/robert-ancell/microflash/issues).
Please [contribute translations](https://poeditor.com/join/project/9hrDWqHfzU).
