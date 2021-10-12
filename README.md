# theoretical-diary

## About

This app is still under development...

Check back later?

## Building

### Windows

The prebuilt binaries are available in `theoretical-diary-x64` (check the result of the latest successful Github actions for `windows.yml`)

If you want to build by yourself, GLHF. I'm not bothered to go through all the instructions since it's so complicated setting everything up.

The only thing that I will mention is that you should strongly consider using MSVC to build on Windows.

### Linux

The prebuilt binary is available as `theoretical-diary-linux` (check the result of the latest successful Github actions for `linux.yml`). You will still need to install the runtime packages, just not the `-dev` ones.

Only instructions for Ubuntu will be provided. If you're not using Ubuntu, you should be able to figure out how to install the dependencies anyway.

Disregarding any system dependencies, clone the repository into a directory and run the following commands:

```
$ sudo apt install qt5-default
$ sudo apt install libqt5networkauth5
$ sudo apt install libqt5networkauth5-dev
$ sudo apt install nlohmann-json3-dev

$ sudo sh build-cryptopp.sh
$ qmake CONFIG+=release
$ make -j8
$ make clean
```

You should now have the executable file in the directory.
