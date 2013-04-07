/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.1
import GuitarTuner 1.0

/*!
  \qmlclass GuitarTunerPanel
  \brief The main container for the guitar tuner UI.
*/
Item {
    id: guitarTunerPanel

    // Listen to signals from the guitar tuner engine
    Connections {
        target: guitarTuner

        onIsInputChanged: {
            meter.checkOutputBacklight();

            if (guitarTuner.autoModeEnabled && !isInput) {
                stringIndicator.stringNote = guitarTuner.string;
            }
        }

        onIsMutedChanged: meter.checkOutputBacklight();
        onVolumeChanged: meter.checkOutputBacklight();

        onStringChanged: {
            console.debug("GuitarTunerPanel::onStringChanged: " + string);
            stringIndicator.stringNote = string;
        }

        onCorrectFrequency: {
            // Turn the backlight of the meter on and make the string indicator
            // glow.
            meter.backlightOn = true;
            stringIndicator.turnGlowingOn();
        }
        onVoiceDifferenceChanged: {
            // Forward the voice difference value to the meter and turn the
            // backlight on to indicate that the analyzer is active.
            meter.handleVoiceDifferenceChanged(voiceDifference);
            meter.backlightOn = true;
        }
        onAutoDetectedStringChanged: {
            console.debug("GuitarTunerPanel::onAutoDetectedStringChanged(): " + string);
            stringIndicator.stringNote = string;
        }
        onLowVoice: {
            meter.backlightOn = false;
        }

        onSettingsRestored: ioControls.initValues();
    }

    /*!
      The guitar tuner application engine.
    */
    GuitarTuner {
        id: guitarTuner
    }

    /*!
      Background image.
    */
    Image {
        id: backgroundImage
        anchors.fill: parent
        smooth: true
        source: "images/guitartuner_skin.png"
        z: 1
    }

    /*!
      Exit button. Not visible in Harmattan.
    */
    Image {
        id: exitButton

        anchors {
            top: parent.top
            right: parent.right
            margins: 20
        }

        z: 2
        source: "images/power.png"
        visible: !isHarmattan

        MouseArea {
            anchors.fill: parent
            onClicked: Qt.quit();
        }
    }

    /*!
      Meter reacting to changes detected by the audio analyzer.
    */
    Meter {
        id: meter

        /*!
          Turns of the backlight if the generator is producing sound.
          Otherwise, turns the backlight off.
        */
        function checkOutputBacklight() {
            if (!guitarTuner.isInput) {
                if (guitarTuner.isMuted || guitarTuner.volume == 0) {
                    backlightOn = false;
                }
                else {
                    backlightOn = true;
                }
            }
            else {
                backlightOn = false;
            }
        }

        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: isHarmattan ? parent.height * 0.15 : parent.height * 0.17;
        }

        width: parent.width * 0.9
        height: parent.height * 0.35
    }

    /*!
      Shows the letter of the selected/detected note on the center of the
      panel.
    */
    StringIndicator {
        id: stringIndicator

        /*!
          Turns the glowing on and starts the timer to turn it back off.
        */
        function turnGlowingOn() {
            if (!glowing) {
                glowing = true;
            }

            turnGlowingOffTimer.restart();
        }

        z: 2

        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: isHarmattan ? parent.height * 0.44 : parent.height * 0.42;
        }

        Timer {
            id: turnGlowingOffTimer
            interval: 1000
            repeat: false
            running: false
            onTriggered: stringIndicator.glowing = false;
        }
    }

    /*!
      A button for choosing the input/output mode.
    */
    Image {
        anchors {
            top: parent.top
            left: parent.left
            topMargin: parent.height * 0.56
            leftMargin: 30
        }

        z: 2
        smooth: true
        source: guitarTuner.isInput ? "images/voicemode_off.png" : "images/voicemode_on.png";

        MouseArea {
            anchors.fill: parent
            onPressed: parent.scale = 0.95;
            onReleased: parent.scale = 1 / 0.95;
            onClicked: guitarTuner.isInput = !guitarTuner.isInput;
        }
    }

    /*!
      Buttons for selecting the string whose frequency to generate/analyze.
    */
    StringSelector {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: parent.height * 0.72
            leftMargin: 10
            rightMargin: 10
        }

        z: 2
        buttonSize: isHarmattan ? parent.width * 0.12 : parent.width * 0.115;
    }

    /*!
      Controls for adjusting the microphone sensitivity/voice generator volume
      and a button for muting.
    */
    IOControls {
        id: ioControls

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: parent.height * 0.87
            leftMargin: 10
            rightMargin: 10
        }

        z: 2
    }
}
