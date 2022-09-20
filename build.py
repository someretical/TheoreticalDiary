import os
import platform
import shutil
import sys

BUILD_DIR = "cmake-build-release"


def setup_botan():
    os.system("python3 external-libs/botan/configure.py --amalgamation --minimized-build --disable-shared "
              "--enable-modules="
              "aes,"
              "argon2,"
              "auto_rng,"
              "base64,"
              "compression,"
              "gcm,"
              "hmac,"
              "sha2_32,"
              "sha2_64,"
              "system_rng,"
              "zlib,"
              )
    shutil.move("botan_all.cpp", "src/crypto/botan_all.cpp")
    shutil.move("botan_all.h", "src/crypto/botan_all.h")

    print("Setup Botan")


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

    os.environ["DEPLOY_PLATFORM_THEMES"] = "dummy value"
    os.environ["EXTRA_QT_PLUGINS"] = "svg"
    os.system(f"{BUILD_DIR}/linuxdeploy-x86_64.AppImage --appdir AppDir --executable "
              f"{BUILD_DIR}/theoreticaldiary --desktop-file src/meta/me.someretical.TheoreticalDiary.desktop "
              f"--icon-file src/icons/application/scalable/apps/theoreticaldiary.svg --plugin qt")

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

    print(f"Packed AppImage in {BUILD_DIR}/bin")


def main():
    if "--build-botan-only" in sys.argv:
        return setup_botan()

    if "--build-botan" in sys.argv:
        setup_botan()

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
