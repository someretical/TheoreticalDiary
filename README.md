# Theoretical Diary

## About

A digital diary with Google Drive backups. This app is currently feature complete. New non-essential features may be added over time.

## Images

![](images/readme/main_menu.png)

![](images/readme/editor.png)

![](images/readme/entry_list.png)

![](images/readme/statistics.png)

![](images/readme/pixels.png)

![](images/readme/settings.png)

## Installation

### Windows

Check the result of the latest successful Github actions for [`windows.yml`](https://github.com/someretical/TheoreticalDiary/actions/workflows/windows.yml). The only platform it is guaranteed to work on is Windows 10 x64.

To fix scaling issues on Windows, follow these instructions:

1. Download and extract the ZIP file.
2. Right click the executable file and select `Properties`.
3. Click on the `Compatability` tab and then click on `Change high DPI settings`.
4. Then tick the override checkbox and make sure the dropdown option is set to `Application`.
5. Click `OK` and then click `Apply` to save the changes.

![](images/readme/windows_compat_1.png)

![](images/readme/windows_compat_2.png)

### Linux

Check the result of the latest successful Github actions for [`linux.yml`](https://github.com/someretical/TheoreticalDiary/actions/workflows/linux.yml). Due to package version requirements, the app image will only work from Ubuntu 20.04 onwards.

### MacOS

Check the result of the latest successful Github actions for [`osx.yml`](https://github.com/someretical/TheoreticalDiary/actions/workflows/osx.yml). It should work for version 10.15 and up.
## Building

Check the steps followed in all the workflow files.