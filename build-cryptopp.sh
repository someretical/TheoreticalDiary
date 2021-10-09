git submodule update --init --recursive &&

# Build crypto++
cd external-libs/cryptopp && 
make dynamic cryptest.exe && 
make libcryptopp.a libcryptopp.so cryptest.exe && 
sudo make install PREFIX=/usr/local &&
cd ../..
