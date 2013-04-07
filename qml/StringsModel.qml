/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.1
import GuitarTuner 1.0

/*!
  \qmlclass StringsModel

  \brief The list model containing the data for all the strings plus an auto
         button.
*/
ListModel {
    ListElement {
        note: "E"
        offSource: "images/tuner_e.png"
        onSource: "images/tuner_e_on.png"
        bigSource: "images/big_e.png"
        glowSource: "images/glowing_e.png"
        stringNote: GuitarTuner.StringE
    }
    ListElement {
        note: "A"
        offSource: "images/tuner_a.png"
        onSource: "images/tuner_a_on.png"
        bigSource: "images/big_a.png"
        glowSource: "images/glowing_a.png"
        stringNote: GuitarTuner.StringA
    }
    ListElement {
        note: "D"
        offSource: "images/tuner_d.png"
        onSource: "images/tuner_d_on.png"
        bigSource: "images/big_d.png"
        glowSource: "images/glowing_d.png"
        stringNote: GuitarTuner.StringD
    }
    ListElement {
        note: "G"
        offSource: "images/tuner_g.png"
        onSource: "images/tuner_g_on.png"
        bigSource: "images/big_g.png"
        glowSource: "images/glowing_g.png"
        stringNote: GuitarTuner.StringG
    }
    ListElement {
        note: "B"
        offSource: "images/tuner_b.png"
        onSource: "images/tuner_b_on.png"
        bigSource: "images/big_b.png"
        glowSource: "images/glowing_b.png"
        stringNote: GuitarTuner.StringB
    }
    ListElement {
        note: "e"
        offSource: "images/tuner_e.png"
        onSource: "images/tuner_e_on.png"
        bigSource: "images/big_e.png"
        glowSource: "images/glowing_e.png"
        stringNote: GuitarTuner.Stringe
    }
    ListElement {
        note: "Auto"
        offSource: "images/tuner_auto.png"
        onSource: "images/tuner_auto_on.png"
        stringNote: -1
    }
}
