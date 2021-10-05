# theoretical-diary

# About

This app is still under development...

Check back later?

## Building

### Windows

The prebuilt binaries are available in artifact.zip (check the result of the latest successful Github actions)

To build from source:

1. Download and install the latest version of QT Creator 5.x.x
2. Clone the repository into a directory.
3. Build the release version in Qt Creator.
6. Copy the produced executable file into another folder.
7. Run `windeployqt ./<name of exe file>` in the new folder
8. You may have to copy `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, and `libwinpthread-1.dll` from your Qt installation into the new folder. This depends on whether or not `g++` has been added to path but the Github actions I set up uses a Windows environment *with* `g++` added to path and `windeployqt` *still* doesn't automatically add them.

There is a good chance you will run into a gazillion different issues while building on Windows so GLHF :^)

### Linux

Only instructions for Ubuntu will be provided. If you're not using Ubuntu, you should be able to figure out how to install the dependencies anyway.

Disregarding any system dependencies, clone the repository into a directory and run the following commands:

```
$ sudo apt install qt5-default
$ sudo apt install libqt5networkauth5
```

1. Run `qmake -config release -spec win32-g++` in the directory.
2. Run `make` in the directory.
3. Run `make clean` in the directory.

You should now have the executable file in the directory.
