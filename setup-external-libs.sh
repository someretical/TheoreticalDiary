git submodule update --init --recursive &&

cd external-libs/qt-secret && qmake -r DEFINE+=WITHOUT_GUI && make -j8 && cd ../.. &&

mv external-libs/qt-secret/src/build/release external-libs/qt-secret/src/build/debug &&

cd external-libs/qt-secret/src/mini-gmp && qmake && make -j8 && cd ../../../.. &&

mv external-libs/qt-secret/src/mini-gmp/src/build/release external-libs/qt-secret/src/mini-gmp/src/build/debug
