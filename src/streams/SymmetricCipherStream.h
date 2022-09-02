/*
 *  Copyright (C) 2010 Felix Geyer <debfx@fobos.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KEEPASSX_SYMMETRICCIPHERSTREAM_H
#define KEEPASSX_SYMMETRICCIPHERSTREAM_H

#include <QByteArray>
#include <QScopedPointer>

#include "crypto/Cipher.h"
#include "crypto/botan_all.h"
#include "streams/LayeredStream.h"

class SymmetricCipherStream : public LayeredStream {
    Q_OBJECT

public:
    explicit SymmetricCipherStream(QIODevice *baseDevice);
    ~SymmetricCipherStream() override;
    auto init(Botan::Cipher_Dir direction, const QByteArray &key, const QByteArray &iv) -> bool;
    auto open(QIODevice::OpenMode mode) -> bool override;
    auto reset() -> bool override;
    void close() override;

protected:
    auto readData(char *data, qint64 maxSize) -> qint64 override;
    auto writeData(const char *data, qint64 maxSize) -> qint64 override;

private:
    void resetInternalState();
    auto readBlock() -> bool;
    auto writeBlock(bool lastBlock) -> bool;

    const QScopedPointer<Cipher> m_cipher;
    QByteArray m_buffer;
    bool m_error;
    int m_bufferPos;
    bool m_bufferFilling;
    bool m_isInitialized;
    bool m_dataWritten;
};

#endif // KEEPASSX_SYMMETRICCIPHERSTREAM_H
