This is the source code to *Microflash* — a graphical application to
flash programs to the [micro:bit](https://microbit.org/). If you
install Microflash after downloading a .hex file you will be able to
download it to the micro:bit in one click.

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
