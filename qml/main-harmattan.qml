/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.1
import com.nokia.meego 1.1

PageStackWindow {
    id: root
    showStatusBar: false
    showToolBar: false

    initialPage: Page {
        anchors.fill: parent
        orientationLock: PageOrientation.LockPortrait

        GuitarTunerPanel {
            anchors.fill: parent
        }
    }
}
