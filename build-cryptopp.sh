git submodule update --init --recursive &&

# Build crypto++
cd external-libs/cryptopp && 
make -j8 dynamic cryptest.exe && 
make -j8 libcryptopp.a libcryptopp.so cryptest.exe && 
sudo make install PREFIX=/usr/local &&
cd ../..
