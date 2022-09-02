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

#include "SymmetricCipherStream.h"

const int BLOCK_SIZE = 16;

SymmetricCipherStream::SymmetricCipherStream(QIODevice *baseDevice)
    : LayeredStream(baseDevice), m_cipher(new Cipher()), m_bufferPos(0), m_bufferFilling(false), m_error(false),
      m_isInitialized(false), m_dataWritten(false)
{
}

SymmetricCipherStream::~SymmetricCipherStream()
{
    close();
}

auto SymmetricCipherStream::init(Botan::Cipher_Dir direction, const QByteArray &key, const QByteArray &iv) -> bool
{
    m_isInitialized = m_cipher->init(direction, key, iv);
    if (!m_isInitialized) {
        setErrorString(m_cipher->errorString());
        return false;
    }

    return true;
}

void SymmetricCipherStream::resetInternalState()
{
    m_buffer.clear();
    m_bufferPos = 0;
    m_bufferFilling = false;
    m_error = false;
    m_dataWritten = false;
    m_cipher->reset();
}

auto SymmetricCipherStream::open(QIODevice::OpenMode mode) -> bool
{
    return m_isInitialized && LayeredStream::open(mode);
}

auto SymmetricCipherStream::reset() -> bool
{
    if (isWritable() && m_dataWritten) {
        if (!writeBlock(true)) {
            return false;
        }
    }

    resetInternalState();

    return true;
}

void SymmetricCipherStream::close()
{
    if (isWritable() && m_dataWritten) {
        writeBlock(true);
    }

    resetInternalState();

    LayeredStream::close();
}

auto SymmetricCipherStream::readData(char *data, qint64 maxSize) -> qint64
{
    Q_ASSERT(maxSize >= 0);

    if (m_error) {
        return -1;
    }

    qint64 bytesRemaining = maxSize;
    qint64 offset = 0;

    while (bytesRemaining > 0) {
        if ((m_bufferPos == m_buffer.size()) || m_bufferFilling) {
            if (!readBlock()) {
                if (m_error) {
                    return -1;
                }
                else {
                    return maxSize - bytesRemaining;
                }
            }
        }

        auto bytesToCopy = qMin(bytesRemaining, static_cast<qint64>(m_buffer.size() - m_bufferPos));

        memcpy(data + offset, m_buffer.constData() + m_bufferPos, bytesToCopy);

        offset += bytesToCopy;
        m_bufferPos += bytesToCopy;
        bytesRemaining -= bytesToCopy;
    }

    return maxSize;
}

auto SymmetricCipherStream::readBlock() -> bool
{
    QByteArray newData;

    if (m_bufferFilling) {
        newData.resize(BLOCK_SIZE - m_buffer.size());
    }
    else {
        m_buffer.clear();
        newData.resize(BLOCK_SIZE);
    }

    auto readResult = m_baseDevice->read(newData.data(), newData.size());

    if (readResult == -1) {
        m_error = true;
        setErrorString(m_baseDevice->errorString());
        return false;
    }
    else {
        m_buffer.append(newData.left(readResult));
    }

    if (m_buffer.size() != BLOCK_SIZE) {
        m_bufferFilling = true;
        return false;
    }
    else {
        m_bufferPos = 0;
        m_bufferFilling = false;

        if (m_baseDevice->atEnd()) {
            if (!m_cipher->finish(m_buffer)) {
                m_error = true;
                setErrorString(m_cipher->errorString());
                return false;
            }
        }
        else if (m_buffer.size() > 0) {
            if (!m_cipher->process(m_buffer)) {
                m_error = true;
                setErrorString(m_cipher->errorString());
                return false;
            }
        }
        return m_buffer.size() > 0;
    }
}

auto SymmetricCipherStream::writeData(const char *data, qint64 maxSize) -> qint64
{
    Q_ASSERT(maxSize >= 0);

    if (m_error) {
        return -1;
    }

    m_dataWritten = true;
    qint64 bytesRemaining = maxSize;
    qint64 offset = 0;

    while (bytesRemaining > 0) {
        int bytesToCopy = qMin(bytesRemaining, static_cast<qint64>(BLOCK_SIZE - m_buffer.size()));

        m_buffer.append(data + offset, bytesToCopy);

        offset += bytesToCopy;
        bytesRemaining -= bytesToCopy;

        if (m_buffer.size() == BLOCK_SIZE) {
            if (!writeBlock(false)) {
                if (m_error) {
                    return -1;
                }
                else {
                    return maxSize - bytesRemaining;
                }
            }
        }
    }

    return maxSize;
}

auto SymmetricCipherStream::writeBlock(bool lastBlock) -> bool
{
    Q_ASSERT(lastBlock || (m_buffer.size() == BLOCK_SIZE));

    if (lastBlock) {
        if (!m_cipher->finish(m_buffer)) {
            m_error = true;
            setErrorString(m_cipher->errorString());
            return false;
        }
    }
    else if (!m_cipher->process(m_buffer)) {
        m_error = true;
        setErrorString(m_cipher->errorString());
        return false;
    }

    if (m_baseDevice->write(m_buffer) != m_buffer.size()) {
        m_error = true;
        setErrorString(m_baseDevice->errorString());
        return false;
    }
    else {
        m_buffer.clear();
        return true;
    }
}