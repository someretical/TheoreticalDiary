import os
import platform
import shutil

BUILD_DIR = "cmake-build-release"


def windows_build():
    pass


def osx_build():
    pass


def linux_build():
    if not os.path.isdir(BUILD_DIR):
        os.mkdir(BUILD_DIR)
        os.system(f"cmake -B{BUILD_DIR} -S.")

    os.makedirs(f"{BUILD_DIR}/bin", exist_ok=True)

    if os.path.isdir("AppDir"):
        shutil.rmtree("AppDir")

    if not os.path.isfile(f"{BUILD_DIR}/linuxdeploy-x86_64.AppImage"):
        os.system(f"wget -O {BUILD_DIR}/linuxdeploy-x86_64.AppImage "
                  "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64"
                  ".AppImage")
        os.system(f"chmod +x {BUILD_DIR}/linuxdeploy-x86_64.AppImage")

    if not os.path.isfile(f"{BUILD_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage"):
        os.system(f"wget -O {BUILD_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage "
                  "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy"
                  "-plugin-qt-x86_64.AppImage")
        os.system(f"chmod +x {BUILD_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage")

    if not os.path.isfile(f"{BUILD_DIR}/appimagetool-x86_64.AppImage"):
        os.system(f"wget -O {BUILD_DIR}/appimagetool-x86_64.AppImage https://github.com/AppImage/AppImageKit/releases"
                  f"/download/continuous/appimagetool-x86_64.AppImage")
        os.system(f"chmod +x {BUILD_DIR}/appimagetool-x86_64.AppImage")

    os.system(f"cmake --build {BUILD_DIR} --target theoreticaldiary --config Release")

    print("Compiled binary")

    os.system(f"{BUILD_DIR}/linuxdeploy-x86_64.AppImage --appdir AppDir --executable "
              f"{BUILD_DIR}/theoreticaldiary --desktop-file src/meta/me.someretical.TheoreticalDiary.desktop "
              f"--icon-file src/icons/apps/theoreticaldiary.svg --plugin qt")

    contents = ""

    with open("AppDir/AppRun", "r") as file:
        contents = file.read().replace("AppRun.wrapped", '"Theoretical Diary"')

    with open("AppDir/AppRun", "w") as file:
        file.write(contents)

    os.rename("AppDir/AppRun.wrapped", "AppDir/Theoretical Diary")

    with open("src/meta/version") as version_file:
        version = version_file.read().strip()
        destination = f"{BUILD_DIR}/bin/Theoretical_Diary-v{version}-x86_64.AppImage"
        os.system(f"{BUILD_DIR}/appimagetool-x86_64.AppImage ./AppDir {destination}")

    print("Packed AppImage")


def main():
    if platform.system() == "Windows":
        windows_build()
    elif platform.system() == "Darwin":
        osx_build()
    elif platform.system() == "Linux":
        linux_build()
    else:
        print(f"Cannot build for {platform.system()}")


if __name__ == "__main__":
    main()
