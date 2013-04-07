/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.1

Item {
    id: slider
    property real value
    property real minimumValue: 0.0
    property real maximumValue: 1.0
    property real stepSize: 0.05
    property bool valueIndicatorVisible: false
    property string backgroundColor: "#000000"

    /*!
      Calculates the knob position based on the given value.
    */
    function calculateKnobPosition(value) {
        var relativeWidth = slider.width - knob.width;

        if (relativeWidth == 0) {
            console.debug("Slider::calculateKnobPosition(): Division by zero!");
            return 0;
        }

        return value * relativeWidth;
    }

    // Default width and height
    width: sliderBackgroundImage.sourceSize.width
    height: knob.sourceSize.height

    onValueChanged: {
        var newKnobX = calculateKnobPosition(value);
        console.debug("Slider::onValueChanged: value == " + value + ", knob.x => " + newKnobX);

        if (newKnobX < 0) {
            newKnobX = 0;
        }

        knob.x = newKnobX;
    }

    // Slider background
    Rectangle {
        x: 2
        width: sliderBackgroundImage.width - 4
        height: 2
        anchors.verticalCenter: sliderBackgroundImage.verticalCenter
        color: backgroundColor
    }
    Image {
        id: sliderBackgroundImage
        width: parent.width
        anchors.verticalCenter: parent.verticalCenter
        fillMode: Image.Stretch
        smooth: true
        source: "images/slider-background.png"
    }

    // Slider knob
    Image {
        id: knob
        y: 5
        fillMode: Image.PreserveAspectFit
        smooth: true
        source: "images/slider.png"
    }

    MouseArea {
        id: sliderMouseArea

        property int margin: 20

        /*!
          Calculates and sets the new value based on the given x coordinate.
        */
        function calculateNewValue(mouseX) {
            var newValue = (mouseX - margin) / (sliderMouseArea.width - margin * 2);

            if (newValue < 0)
                newValue = 0;

            if (newValue > 1)
                newValue = 1;

            value = newValue;
        }

        anchors {
            fill: parent
            topMargin: -10
            bottomMargin: -10
        }

        onPressed: calculateNewValue(mouse.x);
        onMouseXChanged: calculateNewValue(mouse.x);
    }
}
