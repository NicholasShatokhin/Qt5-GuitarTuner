/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.1

/*!
  \qmlclass StringSelector

  \brief A UI component containing the buttons for selecting a string or an
         an auto mode.
*/
Item {
    id: stringSelector

    property int buttonSize: 40

    width: (buttonSize + 6) * 7 + (20 - 6)
    height: buttonSize

    Row {
        anchors {
            fill: parent
            leftMargin: 10
            rightMargin: 10
        }

        spacing: 6

        Repeater {
            model: StringsModel {}

            delegate: Image {
                width: stringSelector.buttonSize
                height: width
                smooth: true

                source: {
                    if (guitarTuner.autoModeEnabled && guitarTuner.isInput) {
                        if (note == "Auto") {
                            // Auto mode is on
                            return onSource;
                        }

                        return offSource;
                    }

                    if (guitarTuner.string == stringNote) {
                        return onSource;
                    }

                    return offSource;
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        console.debug("StringSelector: " + note + " selected.");

                        if (note == "Auto") {
                            // Set the input mode on if necessary and turn the
                            // auto mode on
                            if (!guitarTuner.isInput) {
                                guitarTuner.isInput = true;
                            }

                            guitarTuner.autoModeEnabled = true;
                        }
                        else {
                            // Disable the auto mode if necessary and set the
                            // selected string for the tuner engine

                            if (guitarTuner.isInput && guitarTuner.autoModeEnabled) {
                                guitarTuner.autoModeEnabled = false;
                            }

                            guitarTuner.string = stringNote;
                        }
                    }
                }
            }
        }
    }
}
