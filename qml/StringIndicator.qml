/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.1
import GuitarTuner 1.0

/*!
  \qmlclass StringIndicator

  \brief A component showing the selected string and indicating if the correct
         frequency has been detected.
*/
Item {
    id: stringIndicator
    width: 100
    height: 100

    /*!
      \qmlproperty stringNote
      \brief Defines which note to show.
    */
    property int stringNote: GuitarTuner.StringE

    /*!
      \qmlproperty glowing
      \brief If set true, the image will be glowing.
    */
    property bool glowing: false

    StringsModel {
        id: stringsModel
    }

    Image {
        anchors.centerIn: parent
        smooth: true

        source: {
            for (var i = 0; i < stringsModel.count; i++) {
                var item = stringsModel.get(i);

                if (item.stringNote == stringIndicator.stringNote) {
                    if (stringIndicator.glowing) {
                        return item.glowSource;
                    }

                    return item.bigSource;
                }
            }

            return "";
        }
    }
}
