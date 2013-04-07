/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtCore/qglobal.h>

#define M_MAX_AMPLITUDE_8BIT_SIGNED 127
#define M_MAX_AMPLITUDE_8BIT_UNSIGNED 255
#define M_MAX_AMPLITUDE_16BIT_SIGNED 32767
#define M_MAX_AMPLITUDE_16BIT_UNSIGNED 65535

#define M_TWELTH_ROOT_OF_2 1.05946309435930000000

// M_SAMPLE_COUNT_MULTIPLIER is 2/(M_TWELTH_ROOT_OF_2 - 1)
#define M_SAMPLE_COUNT_MULTIPLIER 33.63430749021150000000

const int DataFrequencyHzOutput(44100);
const int DataFrequencyHzInput(48000);

const int MaxInputValue(50);
const int MinInputValue(-50);

// Frequencies of different strings
const qreal FrequencyE(82.407);
const qreal FrequencyA(110.00);
const qreal FrequencyD(146.83);
const qreal FrequencyG(196.00);
const qreal FrequencyB(246.94);
const qreal Frequencye(329.63);

#endif // CONSTANTS_H
