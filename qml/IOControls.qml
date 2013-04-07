/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.1
import GuitarTuner 1.0

/*!
  \qmlclass IOControls

  \brief Contains the components for controlling the input mode and setting
         the sensitivity and volume.
*/
Item {
    /*!
      Initializes the values of the controls.
    */
    function initValues()
    {
        slider.initialized = false;
        slider.initValue();
    }

    // Default width and height
    width: isHarmattan ? 440 : 320;
    height: 50

    Connections {
        target: guitarTuner

        onIsInputChanged: {
            if (isInput) {
                slider.value = guitarTuner.sensitivity;
            }
            else {
                slider.value = guitarTuner.volume;
            }
        }
        onSensitivityChanged: slider.initValue();
        onVolumeChanged: slider.initValue();
    }

    Row {
        id: containerRow

        anchors {
            fill: parent
            leftMargin: 24
            rightMargin: 12
        }

        spacing: 12

        /*!
          An element for showing the mode and changing the mute state.
        */
        Image {
            id: modeIconImage
            smooth: true
            source: guitarTuner.isInput ? "images/sensitivity.png" :
                        guitarTuner.isMuted ? "images/volume_off.png" : "images/volume.png";

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    if (!guitarTuner.isInput) {
                        // Toggle mute
                        guitarTuner.isMuted = !guitarTuner.isMuted;
                    }
                }
            }
        }

        /*!
          Slider for adjusting the sensitivity/volume.
        */
        Slider {
            id: slider

            property bool initialized: false

            /*!
              Calculates a color based on \a value which should be a real
              number [0.0, 1.0].
            */
            function calculateColor(value) {
                var color;

                if (value == 0.0) {
                    color = "#3333ff"; // Blue
                }
                else {
                    // Update the accent color based on the value
                    var tempValue = Math.round(value * 255);
                    var redValueString = tempValue.toString(16);
                    tempValue = Math.round((value - 1.0) * -255);
                    var greenValueString = tempValue.toString(16);

                    if (redValueString.length < 2)
                        redValueString = "0" + redValueString;

                    if (greenValueString.length < 2)
                        greenValueString = "0" + greenValueString;

                    color = "#" + redValueString + greenValueString + "00";
                }

                return color;
            }

            /*!
              Initializes the slider alue based on the guitar tuner engine
              state.
            */
            function initValue()
            {
                if (initialized)
                    return;

                if (guitarTuner.isInput) {
                    value = guitarTuner.sensitivity;
                }
                else {
                    value = guitarTuner.volume;
                }

                backgroundColor = calculateColor(value);
                initialized = true;
            }

            width: containerRow.width - modeIconImage.width - 24
            minimumValue: 0.0
            maximumValue: 1.0
            stepSize: 0.05
            valueIndicatorVisible: false

            onValueChanged: {
                if (!initialized)
                    return;

                if (guitarTuner.isInput) {
                    guitarTuner.sensitivity = value;
                }
                else {
                    guitarTuner.volume = value;
                }

                // Update the accent color based on the value
                backgroundColor = calculateColor(value);
            }
        }
    }

    Component.onCompleted: initValues();
}
