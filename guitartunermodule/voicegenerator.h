/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef VOICEGENERATOR_H
#define VOICEGENERATOR_H

#include <QtCore/QByteArray>
#include <QtCore/QIODevice>
#include <QtMultimediaKit/QAudioFormat>


class VoiceGenerator : public QIODevice
{
    Q_OBJECT

public:
    explicit VoiceGenerator(const QAudioFormat &format,
                            qreal frequency,
                            qreal amplitude,
                            QObject *parent = 0);
    ~VoiceGenerator();

public:
    void setFrequency(qreal frequency);
    qreal frequency();
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 maxlen);
    qint64 bytesAvailable() const;

public slots:
    void setAmplitude(qreal amplitude);
    void start();
    void stop();

private:
    void setValue(uchar *ptr, qreal realValue);
    void refreshData();

private:  // Data
    const QAudioFormat m_format;
    QByteArray m_buffer; // Buffer to store the data
    qint64 m_position; // Current position in buffer
    qint64 m_maxPosition; // Max position depends on the sample rate of
                          // format and the frequency of voice
    qreal m_amplitude;
    qreal m_frequency;
};


#endif // VOICEGENERATOR_H
