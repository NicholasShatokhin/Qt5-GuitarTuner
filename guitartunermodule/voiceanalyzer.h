/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef VOICEANALYZER_H
#define VOICEANALYZER_H

#include <QtCore/QIODevice>
#include <QtCore/QVariant>
#include <QtMultimediaKit/QAudioFormat>

class FastFourierTransformer;


class VoiceAnalyzer : public QIODevice
{
    Q_OBJECT

public:
    explicit VoiceAnalyzer(const QAudioFormat &format, QObject *parent = 0);

public:
    void start(qreal frequency);
    void stop();
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 maxlen);
    qreal frequency();
    int getMaximumVoiceDifference();
    int getMaximumPrecisionPerNote();

public slots:
    void setFrequency(qreal frequency);
    void setCutOffPercentage(qreal cutoff);

private:
    qint16 getValueInt16(const uchar *ptr);
    void analyzeVoice();

signals:
    void voiceDifferenceChanged(qreal frequency);
    void correctFrequency();
    void lowVoice();

private:
    FastFourierTransformer *m_fftHelper; // Owned
    const QAudioFormat m_format;
    QList<qint16> m_samples;
    int m_totalSampleCount;
    int m_maximumVoiceDifference;
    int m_stepSize;
    qreal m_frequency;
    qint64 m_position;
};


#endif // VOICEANALYZER_H
