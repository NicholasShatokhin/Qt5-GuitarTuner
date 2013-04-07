/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#include "guitartuner.h"

#include <QtCore/QDebug>
#include <QtCore/qmath.h>
#include <QtMultimediaKit/QAudioDeviceInfo>
#include <QtMultimediaKit/QAudioInput>
#include <QtMultimediaKit/QAudioOutput>

#include "constants.h"
#include "voiceanalyzer.h"
#include "voicegenerator.h"

// Constants
const qreal StringInterval(2.5f);
const QString IsInputKey("isInput");
const QString IsMutedKey("isMuted");
const QString AutoModeEnabledKey("autoModeEnabled");
const QString SensitivityKey("sensitivity");
const QString VolumeKey("volume");
const QString StringKey("string");


/*!
  \class GuitarTuner
  \brief Implements the main interface of the plugin.
*/


/*!
  Constructor.
*/
GuitarTuner::GuitarTuner(QQuickItem *parent)
    : QQuickItem(parent),
      m_voiceAnalyzer(0),
      m_voiceGenerator(0),
      m_audioInput(0),
      m_audioOutput(0),
      m_isInput(true),
      m_isMuted(false),
      m_autoModeEnabled(false),
      m_sensitivity(0.5f),
      m_volume(0.5f),
      m_string(StringE),
      m_autoDetectedString(StringE)
{
    // Initialize audio output and input.
    initAudioOutput();
    initAudioInput();

    // Connect the signals of the voice analyzer.
    connect(m_voiceAnalyzer, SIGNAL(lowVoice()), this, SIGNAL(lowVoice()));
    connect(m_voiceAnalyzer, SIGNAL(correctFrequency()), this, SIGNAL(correctFrequency()));
    connect(m_voiceAnalyzer, SIGNAL(voiceDifferenceChanged(qreal)),
            this, SIGNAL(voiceDifferenceChanged(qreal)));
    setIsInput(true);
}


/*!
  Destructor.
*/
GuitarTuner::~GuitarTuner()
{
    // No implementation required.
}


/*!
  Constructs and returns a variant map containing the current settings of this
  engine.
*/
QVariant GuitarTuner::settings() const
{
    QVariantMap retval;
    retval.insert(IsInputKey, m_isInput);
    retval.insert(IsMutedKey, m_isMuted);
    retval.insert(AutoModeEnabledKey, m_autoModeEnabled);
    retval.insert(SensitivityKey, m_sensitivity);
    retval.insert(VolumeKey, m_volume);
    retval.insert(StringKey, QVariant::fromValue((int)m_string));
    qDebug() << "GuitarTuner::settings():" << retval;
    return QVariant::fromValue(retval);
}


/*!
  Suspends the audio output, if \a state is ActiveState and the voice is muted.
*/
void GuitarTuner::setOutputState(QAudio::State state)
{
    if (QAudio::ActiveState == state && m_isMuted) {
        // If the voice is muted, suspend the voice.
        m_audioOutput->suspend();
    }
}


/*!
  Restores and applies \a settings. \a settings should have been received from
  GuitarTuner::settings(). The boolean parameter of the signal is set to true
  if the restoration was successful.
*/
void GuitarTuner::restoreSettings(QVariant settings)
{
    if (!settings.isValid() || !settings.canConvert<QVariantMap>()) {
        qDebug() << "GuitarTuner::restoreSettings(): Invalid argument, skipping...";
        emit settingsRestored(false);
        return;
    }

    QVariantMap map = settings.toMap();

    if (map.isEmpty()) {
        qDebug() << "GuitarTuner::restoreSettings(): Empty map received, skipping...";
        emit settingsRestored(false);
        return;
    }

    qDebug() << "GuitarTuner::restoreSettings():" << map;
    setString((String)map.value(StringKey).toInt()); // This needs to be set first!
    setIsInput(map.value(IsInputKey).toBool());
    setIsMuted(map.value(IsMutedKey).toBool());
    setAutoModeEnabled(map.value(AutoModeEnabledKey).toBool());
    setSensitivity(map.value(SensitivityKey).toReal());
    setVolume(map.value(VolumeKey).toReal());
    emit settingsRestored(true);
}


/*!
  Stops all the ongoing activities of this engine. Restart the activities by
  applying the settings or by calling GuitarTuner::setIsInput().
*/
void GuitarTuner::suspend()
{
    qDebug() << "GuitarTuner::suspend()";

    if (m_isInput) {
        // Stop audio input and audio analyzer.
        m_audioInput->stop();
        m_voiceAnalyzer->stop();
    }
    else {
        // Stop audio output and audio generator.
        m_audioOutput->stop();
        m_voiceGenerator->stop();
    }
}


/*!
  Returns true if the tuner is in the input mode, false otherwise.
*/
bool GuitarTuner::isInput() const
{
    return m_isInput;
}


/*!
  Sets the input mode to \a isInput.
*/
void GuitarTuner::setIsInput(bool isInput)
{
    qDebug() << "GuitarTuner::setIsInput():" << isInput;

    // Stop audio input/output depending on the previous state.
    suspend();

    m_isInput = isInput;

    if (m_isInput) {
        // Start the audio analyzer and then the audio input.
        m_voiceAnalyzer->start(stringToFrequency(m_string));
        m_audioInput->start(m_voiceAnalyzer);
    }
    else {
        // Set up the audio output.

        // If the current frequency of voice generator
        // is not the same as the target frequency selected in the UI,
        // update voice generator's frequency.
        if (m_voiceGenerator->frequency() != stringToFrequency(m_string)) {
            m_voiceGenerator->setFrequency(stringToFrequency(m_string));
        }

        // Start the voice generator and then the audio output.
        m_voiceGenerator->start();

        if (!m_isMuted) {
            m_audioOutput->start(m_voiceGenerator);
        }
    }

    emit isInputChanged(m_isInput);
}


/*!
  Returns true if the tuner is muted, false otherwise.
*/
bool GuitarTuner::isMuted() const
{
    return m_isMuted;
}


/*!
  Sets the mute to \a muted.
*/
void GuitarTuner::setIsMuted(bool isMuted)
{
    if (m_isMuted == isMuted) {
        return;
    }

    m_isMuted = isMuted;

    if (m_isMuted) {
        m_audioOutput->suspend();
    }
    else {
        if (m_audioOutput->state() == QAudio::SuspendedState) {
            m_audioOutput->resume();
        }
        else {
            m_audioOutput->start(m_voiceGenerator);
        }
    }

    emit isMutedChanged(m_isMuted);
}


/*!
  Returns true if the auto mode is enabled, false otherwise.
*/
bool GuitarTuner::autoModeEnabled() const
{
    return m_autoModeEnabled;
}


/*!
  Enables auto mode if \a autoModeEnabled is true. Note that auto mode only has
  effect if the input mode is on.
*/
void GuitarTuner::setAutoModeEnabled(bool autoModeEnabled)
{
    if (m_autoModeEnabled != autoModeEnabled) {
        m_autoModeEnabled = autoModeEnabled;

        if (m_autoModeEnabled) {
            m_autoDetectedString = m_string;

            // Start listening analyzer for automatic detection
            connect(m_voiceAnalyzer, SIGNAL(voiceDifferenceChanged(qreal)),
                    this, SLOT(autoDetectTargetFrequency(qreal)));
        }
        else {
            // Restore the original setting because that might have changed
            // during the automatic detection
            setString(m_string);

            // Disconnect the signal-slot connection
            disconnect(m_voiceAnalyzer, SIGNAL(voiceDifferenceChanged(qreal)),
                       this, SLOT(autoDetectTargetFrequency(qreal)));
        }

        emit autoModeEnabledChanged(m_autoModeEnabled);
    }
}


/*!
  Returns the set sensitivity level of the microphone.
*/
qreal GuitarTuner::sensitivity() const
{
    return m_sensitivity;
}


/*!
  Sets the sensitivity level to \a sensitivity.
*/
void GuitarTuner::setSensitivity(qreal sensitivity)
{
    if (sensitivity < 0 || sensitivity > 1.0) {
        return;
    }

    m_sensitivity = sensitivity;
    qreal temp = m_sensitivity;
    temp = temp / 2 + 0.5; // No point of being below 0.5
    temp = 1 - temp;
    m_voiceAnalyzer->setCutOffPercentage(temp);
    emit sensitivityChanged(m_sensitivity);
}


/*!
  Returns the set volume level.
*/
qreal GuitarTuner::volume() const
{
    return m_volume;
}


/*!
  Sets the volume level to \a volume.
*/
void GuitarTuner::setVolume(qreal volume)
{
    if (m_volume == volume
            || volume < 0 || volume > 1.0)
    {
        return;
    }

    m_volume = volume;
    m_voiceGenerator->setAmplitude(m_volume);
    emit volumeChanged(m_volume);
}

/*!
  Returns the current string whose frequency is being analyzed of generated.
*/
int GuitarTuner::string() const
{
    return (int)m_string;
}


/*!
  Sets the string, whose frequency is being analyzed or generated, to
  \a string.
*/
void GuitarTuner::setString(int string)
{
    Q_ASSERT(string >= 0 && string < 6);
    m_string = (String)string;

    if (m_isInput) {
        // Stop the audio input and voice analyzer.
        m_audioInput->stop();
        m_voiceAnalyzer->stop();

        // Start the voice analyzer with new frequency and audio input.
        m_voiceAnalyzer->start(stringToFrequency(m_string));
        m_audioInput->start(m_voiceAnalyzer);
    }
    else {
        // Stop the audio output and voice generator.
        m_audioOutput->stop();
        m_voiceGenerator->stop();

        // Set the voice generator's frequency to the target frequency.
        m_voiceGenerator->setFrequency(stringToFrequency(m_string));

        // Start the voice generator and audio output.
        m_voiceGenerator->start();

        if (!m_isMuted) {
            m_audioOutput->start(m_voiceGenerator);
        }
    }

    emit stringChanged((int)m_string);
}


/*!
  Initializes the audio input.
*/
void GuitarTuner::initAudioInput()
{
    // Set up the input format.
    m_formatInput.setFrequency(DataFrequencyHzInput);
    m_formatInput.setCodec("audio/pcm");
    m_formatInput.setSampleSize(16);
    m_formatInput.setChannels(1);
    m_formatInput.setByteOrder(QAudioFormat::LittleEndian);
    m_formatInput.setSampleType(QAudioFormat::SignedInt);

    // Obtain a default input device, and if the format is not
    // supported, find the nearest format available.
    QAudioDeviceInfo inputDeviceInfo(QAudioDeviceInfo::defaultInputDevice());

    if (!inputDeviceInfo.isFormatSupported(m_formatInput)) {
        m_formatInput = inputDeviceInfo.nearestFormat(m_formatInput);
    }

    // Create new QAudioInput and VoiceAnalyzer instances, and store them in
    // m_audioInput and m_voiceAnalyzer, respectively. Remember to set the cut-off
    // percentage for voice analyzer.
    m_audioInput = new QAudioInput(inputDeviceInfo, m_formatInput, this);
    m_voiceAnalyzer = new VoiceAnalyzer(m_formatInput, this);
    setSensitivity(m_sensitivity);
}


/*!
  Initializes the audio output.
*/
void GuitarTuner::initAudioOutput()
{
    // Set up the output format.
    m_formatOutput.setFrequency(DataFrequencyHzOutput);
    m_formatOutput.setCodec("audio/pcm");
    m_formatOutput.setSampleSize(16);
    m_formatOutput.setChannels(1);
    m_formatOutput.setByteOrder(QAudioFormat::LittleEndian);
    m_formatOutput.setSampleType(QAudioFormat::SignedInt);

    // Obtain a default output device, and if the format is not supported,
    // find the nearest format available.
    QAudioDeviceInfo outputDeviceInfo(QAudioDeviceInfo::defaultOutputDevice());

    if (!outputDeviceInfo.isFormatSupported(m_formatOutput)) {
        m_formatOutput = outputDeviceInfo.nearestFormat(m_formatOutput);
    }

    // Create new QAudioOutput and VoiceGenerator instances, and store
    // them in m_audioOutput and m_voiceGenerator, respectively.
    m_audioOutput = new QAudioOutput(outputDeviceInfo,
                                     m_formatOutput, this);
    m_voiceGenerator = new VoiceGenerator(m_formatOutput,
                                          stringToFrequency(m_string),
                                          m_volume,
                                          this);

    // Connect m_audioOutput stateChanged signal to outputStateChanged.
    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)),
            SIGNAL(outputStateChanged(QAudio::State)));
}


/*!
  Maps \a string to correct frequency and returns the frequency value.
*/
qreal GuitarTuner::stringToFrequency(String string) const
{
    switch (string) {
    case StringE: return FrequencyE;
    case StringA: return FrequencyA;
    case StringD: return FrequencyD;
    case StringG: return FrequencyG;
    case StringB: return FrequencyB;
    case Stringe: return Frequencye;
    }

    return 1;
}


/*!
  Detects the nearest target frequency (string) based on \a voiceDifference and
  emits GuitarTuner::autoDetectedStringChanged() signal.
*/
void GuitarTuner::autoDetectTargetFrequency(qreal voiceDifference)
{
    qreal tempDifference = abs(voiceDifference);
    qreal tempIndex = (qreal)m_autoDetectedString;
    qreal tempInterval(StringInterval);

    if ((qRound(tempIndex) - (voiceDifference < 0)) == (int)Stringe) {
        tempInterval = 5000;
    }

    while (!(voiceDifference < 0 && tempIndex == 0)
           && tempDifference >= tempInterval)
    {
        tempDifference -= tempInterval;
        tempIndex += voiceDifference / abs(voiceDifference);

        if ((qRound(tempIndex) - (voiceDifference < 0)) == (int)Stringe) {
            tempInterval = 5000;
        }
        else {
            tempInterval = StringInterval;
        }
    }

    int tempString = qRound(tempIndex);

    if (tempString != (int)m_autoDetectedString
            && tempString >= (int)StringE && tempString <= (int)Stringe)
    {
        m_autoDetectedString = (String)tempString;
        qDebug() << "GuitarTuner::autoDetectTargetFrequency(): Detected string with index" << m_autoDetectedString;
        m_voiceAnalyzer->setFrequency(stringToFrequency(m_autoDetectedString));
        emit autoDetectedStringChanged(m_autoDetectedString);
    }
}


QML_DECLARE_TYPE(GuitarTuner)
