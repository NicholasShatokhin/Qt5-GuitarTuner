/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.1

/*!
  \qmlclass Meter
  \brief A meter component.
*/
Item {
    id: meter

    /*!
      \qmlproperty Meter::backlightOn
      \brief If set to true, will set the backlight on.
    */
    property bool backlightOn: false

    property real value: 0
    property real minValue: -12
    property real maxValue: 12

    /*!
      Reacts to \a voiceDifference and moves the pointer accordingly.
    */
    function handleVoiceDifferenceChanged(voiceDifference)
    {
        value = voiceDifference;

        pointerImage.angle = (((value - minValue) / (maxValue - minValue)) *
                              (pointerImage.angleMax - pointerImage.angleMin)) +
                             pointerImage.angleMin;
    }

    Image {
        id: meterBackgroundImage
        anchors.fill: parent
        smooth: true
        source: "images/meter-background.png"
    }

    // Backlight
    Rectangle {
        anchors.fill: parent
        opacity: backlightOn ? 1.0 : 0.0;

        gradient: Gradient {
            GradientStop {
                position: 0;
                color: guitarTuner.isInput ? "#5588ff88" : "#558888ff"

                Behavior on color { ColorAnimation { duration: 250 } }
            }
            GradientStop { position: 1; color: "#22ffffff" }
        }

        Behavior on opacity { NumberAnimation { duration: 250 } }
    }

    Image {
        id:pointerShadowImage
        x: pointerImage.x - 2
        y: pointerImage.y - 2
        height: pointerImage.height
        smooth: true
        source: "images/pointer-shadow.png"

        transform: Rotation {
            origin.x: 2
            origin.y: height
            angle: -pointerImage.angle
        }
    }

    Image {
        id: pointerImage

        property real angle: 0
        property real angleMax: -40
        property real angleMin: 40

        height: parent.height * 0.92

        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            bottomMargin: 2
        }

        smooth: true
        source: "images/pointer.png"
        transformOrigin: "Bottom"
        rotation: -angle

        Behavior on angle {
            SpringAnimation {
                spring: 1.4
                damping: 0.15
            }
        }
    }
}
