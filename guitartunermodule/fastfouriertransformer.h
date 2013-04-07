/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef FASTFOURIERTRANSFORM_H
#define FASTFOURIERTRANSFORM_H

#include <QtCore/QObject>
#include <QtCore/QList>

class FastFourierTransformer : public QObject
{
    Q_OBJECT

public:
    explicit FastFourierTransformer(QObject *parent = 0);
    ~FastFourierTransformer();

public:
    void reserve(int n);
    void calculateFFT(QList<qint16> wave);
    int getMaximumDensityIndex();
    void setCutOffForDensity(float cutoff);

private:
    float *m_waveFloat;
    float *m_workingArray;
    int *m_ifac;
    int m_last_n;
    float m_cutOffForDensitySquared;
};

#endif // FASTFOURIERTRANSFORM_H
