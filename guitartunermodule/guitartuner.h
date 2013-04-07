/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef GUITARTUNER_H
#define GUITARTUNER_H

#include <QtCore/QVariant>
#include <QtMultimediaKit/qaudio.h>
#include <QtMultimediaKit/QAudioFormat>
#include <QtQuick/QQuickItem>

// Forward declarations
class QAudioInput;
class QAudioOutput;
class VoiceAnalyzer;
class VoiceGenerator;


class GuitarTuner : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(bool isInput READ isInput WRITE setIsInput NOTIFY isInputChanged)
    Q_PROPERTY(bool isMuted READ isMuted WRITE setIsMuted NOTIFY isMutedChanged)
    Q_PROPERTY(bool autoModeEnabled READ autoModeEnabled WRITE setAutoModeEnabled NOTIFY autoModeEnabledChanged)
    Q_PROPERTY(qreal sensitivity READ sensitivity WRITE setSensitivity NOTIFY sensitivityChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(int string READ string WRITE setString NOTIFY stringChanged)
    Q_ENUMS(String)

public: // Data types

    enum String {
        StringE = 0,
        StringA,
        StringD,
        StringG,
        StringB,
        Stringe
    };

public:
    explicit GuitarTuner(QQuickItem *parent = 0);
    ~GuitarTuner();

public:
    Q_INVOKABLE QVariant settings() const;

public slots:
    void setOutputState(QAudio::State state);
    void restoreSettings(QVariant settings);
    void suspend();

private: // Property setters and getters
    bool isInput() const;
    void setIsInput(bool isInput);
    bool isMuted() const;
    void setIsMuted(bool isMuted);
    bool autoModeEnabled() const;
    void setAutoModeEnabled(bool autoModeEnabled);
    qreal sensitivity() const;
    void setSensitivity(qreal sensitivity);
    qreal volume() const;
    void setVolume(qreal volume);
    int string() const;
    void setString(int string);

private:
    void initAudioInput();
    void initAudioOutput();
    qreal stringToFrequency(String string) const;

private slots:
    void autoDetectTargetFrequency(qreal voiceDifference);

signals: // Property signals
    void isInputChanged(bool isInput);
    void isMutedChanged(bool isMuted);
    void autoModeEnabledChanged(bool autoModeEnabled);
    void sensitivityChanged(qreal sensitivity);
    void volumeChanged(qreal volume);
    void stringChanged(int string);

signals:
    void outputStateChanged(QAudio::State state);
    void lowVoice();
    void correctFrequency();
    void voiceDifferenceChanged(qreal voiceDifference);
    void autoDetectedStringChanged(int string);
    void settingsRestored(bool wasSuccessful);

private: // Data
    VoiceAnalyzer *m_voiceAnalyzer; // Owned
    VoiceGenerator *m_voiceGenerator; // Owned
    QAudioInput *m_audioInput; // Owned
    QAudioOutput *m_audioOutput; // Owned
    QAudioFormat m_formatInput;
    QAudioFormat m_formatOutput;
    bool m_isInput;
    bool m_isMuted;
    bool m_autoModeEnabled;
    qreal m_sensitivity;
    qreal m_volume;
    String m_string;
    String m_autoDetectedString;

    Q_DISABLE_COPY(GuitarTuner)
};

//qRegisterMetaType<String>("String");

#endif // GUITARTUNER_H
