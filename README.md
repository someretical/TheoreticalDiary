# theoretical-diary

# About

This app is still under development...

Check back later?

## Building

### Windows

The prebuilt binaries are available in win_64_release.zip (check the result of the latest successful Github actions)

If you want to build by yourself, GLHF. I'm not bothered to go through all the instructions since it's so complicated setting everything up.

### Linux

Only instructions for Ubuntu will be provided. If you're not using Ubuntu, you should be able to figure out how to install the dependencies anyway.

Disregarding any system dependencies, clone the repository into a directory and run the following commands:

```
$ sudo apt install qt5-default
$ sudo apt install libqt5networkauth5
$ sudo apt install libqt5networkauth5-dev
$ sudo apt install nlohmann-json3-dev

$ sudo sh build-cryptopp.sh
$ qmake -config release -spec win32-g++
$ make
$ make clean
```

You should now have the executable file in the directory.
