/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#include "voicegenerator.h"

#include <QtCore/QDebug>
#include <QtCore/qmath.h>
#include <QtCore/qendian.h>

#include "constants.h"

const int BufferSizeMilliseconds(100);


/*!
  \class VoiceGenerator
  \brief Generates sound according to the set frequency and amplitude.
*/


/*!
  Constructor.
*/
VoiceGenerator::VoiceGenerator(const QAudioFormat &format,
                               qreal frequency,
                               qreal amplitude,
                               QObject *parent)
    : QIODevice(parent),
      m_format(format),
      m_position(0),
      m_maxPosition(0),
      m_amplitude(amplitude),
      m_frequency(FrequencyE)
{
    Q_ASSERT(m_format.sampleSize() % 8 == 0);

    int sampleBytes = m_format.channels() * (m_format.sampleSize() / 8);

    // + 1 to round up, just to be sure that all samples fit.
    qint64 samplesInBuffer = m_format.sampleRate()
                             * BufferSizeMilliseconds / 1000 + 1;
    qint64 length = samplesInBuffer * sampleBytes;
    m_buffer.resize(length);
    setFrequency(frequency);
}


/*!
  Destructor.
*/
VoiceGenerator::~VoiceGenerator()
{
    stop();
}


/*!
  Sets the frequency to \a frequency.
*/
void VoiceGenerator::setFrequency(qreal frequency)
{
    Q_ASSERT(frequency != 0);
    Q_ASSERT(1 / frequency < BufferSizeMilliseconds);
    m_frequency = frequency;
    refreshData();
    qDebug() << "VoiceGenerator::setFrequency(): Frequency set to" << m_frequency;
}


/*!
  Returns the current frequency.
*/
qreal VoiceGenerator::frequency()
{
    return m_frequency;
}


/*!
  Called by the QIODevice. Puts \a maxlen amount of voice samples into
  \a data array. Returns the amount of data read.
*/
qint64 VoiceGenerator::readData(char *data, qint64 maxlen)
{
    qint64 total(0);
    qint64 chunk(0);

    while (total < maxlen) {
        if (maxlen - total >= m_maxPosition - m_position) {
            // the needed buffer is longer than the currently
            // available buffer from m_position to the m_maxPosition
            chunk = m_maxPosition - m_position;
            memcpy(data, m_buffer.constData() + m_position, chunk);
            m_position = 0;
        }
        else {
            // we can copy the needed data directly, and the loop will end
            chunk = maxlen - total;
            memcpy(data, m_buffer.constData() + m_position, chunk);
            m_position = (m_position + chunk) % m_maxPosition;
        }

        data += chunk;
        total += chunk;
    }

    return total;
}


/*!
  Empty implementation for writeData, since no data is provided
  for the VoiceGenerator class.
*/
qint64 VoiceGenerator::writeData(const char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);

    return 0;
}


/*!
  Returns the number of bytes available.
*/
qint64 VoiceGenerator::bytesAvailable() const
{
    return m_maxPosition + QIODevice::bytesAvailable();
}


/*!
  Sets the amplitude for the voice to \a amplitude.
*/
void VoiceGenerator::setAmplitude(qreal amplitude)
{
    Q_ASSERT(amplitude >= 0);
    qDebug() << "VoiceGenerator::setAmplitude():" << amplitude;
    m_amplitude = amplitude;
    refreshData();
}


/*!
  Opens the parent QIODevice.
*/
void VoiceGenerator::start()
{
    open(QIODevice::ReadOnly);
}


/*!
  Closes the parent QIODevice. Resets the m_position to zero.
*/
void VoiceGenerator::stop()
{
    close();
    m_position = 0;
}


/*!
  Stores \a realValue into bytes pointed by \a ptr as an int value.
  Align-safe.
*/
void VoiceGenerator::setValue(uchar *ptr, qreal realValue)
{
    if (m_format.sampleSize() == 8)
    {
        quint8 value(0);

        if (m_format.sampleType() == QAudioFormat::UnSignedInt) {
            value = static_cast<quint8>(
                        qRound((1.0 + realValue) / 2
                               * M_MAX_AMPLITUDE_8BIT_UNSIGNED));
        }
        else if (m_format.sampleType() == QAudioFormat::SignedInt) {
            value = static_cast<qint8>(
                        qRound(realValue
                               * M_MAX_AMPLITUDE_8BIT_SIGNED));
        }

        *reinterpret_cast<quint8*>(ptr) = value;
    }
    else if (m_format.sampleSize() == 16) {
        quint16 value(0);

        if (m_format.sampleType() == QAudioFormat::UnSignedInt) {
            value = static_cast<quint16>(
                        qRound((1.0 + realValue) / 2
                               * M_MAX_AMPLITUDE_16BIT_UNSIGNED));
        }
        else if (m_format.sampleType() == QAudioFormat::SignedInt) {
            value = static_cast<qint16>(
                        qRound(realValue
                               * M_MAX_AMPLITUDE_16BIT_SIGNED));
        }

        if (m_format.byteOrder() == QAudioFormat::LittleEndian) {
            qToLittleEndian<qint16>(value, ptr);
        }
        else {
            qToBigEndian<qint16>(value, ptr);
        }
    }
}


/*!
  Generates voice data corresponding a sine voice with target frequency.
  The number of data generated is calculated and stored to m_maxPosition.
*/
void VoiceGenerator::refreshData()
{
    if (m_frequency == 0) {
        // Let's not divide by zero.
        return;
    }

    const int channelBytes = m_format.sampleSize() / 8;
    const int sampleSize = m_format.channels() * channelBytes;
    const qint64 voiceOscillationsInBuffer = BufferSizeMilliseconds
                                         * m_frequency / 1000;
    const qint64 voiceSamplesInBuffer = voiceOscillationsInBuffer
                                   * m_format.sampleRate() / m_frequency;
    m_maxPosition = voiceSamplesInBuffer * sampleSize;
    qint64 dataGenerationLength = m_buffer.size();


    Q_ASSERT(m_maxPosition % (sampleSize) == 0);
    Q_ASSERT(dataGenerationLength <= m_buffer.size());

    uchar *ptr = reinterpret_cast<uchar *>(m_buffer.data());
    int sampleIndex = 0;

    while (dataGenerationLength > 0) {
        qreal realValue = 0;

        if (sampleIndex < voiceSamplesInBuffer) {
            realValue =
                m_amplitude * qSin(2.0f * M_PI * m_frequency
                * qreal(sampleIndex % m_format.sampleRate())
                / m_format.sampleRate());
        }

        for (int i=0; i<m_format.channels(); ++i) {
            setValue(ptr, realValue);
            ptr += channelBytes;
            dataGenerationLength -= channelBytes;
        }

        ++sampleIndex;
    }
}
