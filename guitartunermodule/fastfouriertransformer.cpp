/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#include "fastfouriertransformer.h"

#include <math.h>

#define STIN  inline
#define __STATIC

#include "fftpack.c"

// called by __ogg_fdrffti
__STATIC void drfti1(int n, float *wa, int *ifac);
void __ogg_fdrffti(int n, float *wsave, int *ifac);
void __ogg_fdcosqi(int n, float *wsave, int *ifac);
// called by drftf1
STIN void dradf2(int ido,int l1,float *cc,float *ch,float *wa1);
// called by drftf1
STIN void dradf4(int ido,int l1,float *cc,float *ch,float *wa1,
        float *wa2,float *wa3);
// called by drftf1
STIN void dradfg(int ido,int ip,int l1,int idl1,float *cc,float *c1,
        float *c2,float *ch,float *ch2,float *wa);
// called by drftf1
STIN void drftf1(int n,float *c,float *ch,float *wa,int *ifac);
void __ogg_fdrfftf(int n,float *r,float *wsave,int *ifac);
STIN void dcsqf1(int n,float *x,float *w,float *xh,int *ifac);
void __ogg_fdcosqf(int n,float *x,float *wsave,int *ifac);
STIN void dradb2(int ido,int l1,float *cc,float *ch,float *wa1);
STIN void dradb3(int ido,int l1,float *cc,float *ch,float *wa1,
                          float *wa2);
STIN void dradb4(int ido,int l1,float *cc,float *ch,float *wa1,
        float *wa2,float *wa3);
STIN void dradbg(int ido,int ip,int l1,int idl1,float *cc,float *c1,
            float *c2,float *ch,float *ch2,float *wa);
STIN void drftb1(int n, float *c, float *ch, float *wa, int *ifac);
void __ogg_fdrfftb(int n, float *r, float *wsave, int *ifac);
STIN void dcsqb1(int n,float *x,float *w,float *xh,int *ifac);
void __ogg_fdcosqb(int n,float *x,float *wsave,int *ifac);


/*!
  Constructor.
*/
FastFourierTransformer::FastFourierTransformer(QObject *parent)
    : QObject(parent),
      m_waveFloat(0),
      m_workingArray(0),
      m_ifac(0),
      m_last_n(-1),
      m_cutOffForDensitySquared(0)
{
}


/*!
  Destructor.
*/
FastFourierTransformer::~FastFourierTransformer()
{
    if (m_waveFloat != 0) {
        delete [] m_waveFloat;
    }

    if (m_workingArray != 0) {
       delete [] m_workingArray;
    }

    if (m_ifac != 0) {
        delete [] m_ifac;
    }
}


/*!
  Prepares the arrays to be of length \a n.
*/
void FastFourierTransformer::reserve(int n)
{
    Q_ASSERT(n > 0);

    if (m_waveFloat != 0) {
        delete [] m_waveFloat;
    }

    if (m_workingArray != 0) {
       delete [] m_workingArray;
    }

    if (m_ifac != 0) {
        delete [] m_ifac;
    }

    m_workingArray = new float[2 * n + 15];
    m_waveFloat = new float[n];
    m_ifac = new int[n];
    __ogg_fdrffti(n, m_workingArray, m_ifac);
    m_last_n = n;
}


/*!
  Calculates the Fast Fourier Transformation (FFT).
*/
void FastFourierTransformer::calculateFFT(QList<qint16> wave)
{
    const int n = wave.size();

    if (m_last_n != n) {
        reserve(n);
    }

    for (int i = 0; i < n; i++) {
        m_waveFloat[i] = (float) wave.at(i);
    }

    __ogg_fdrfftf(n, m_waveFloat, m_workingArray, m_ifac);
}


/*!
  Returns the index which corresponds to the maximum density of the FFT.
*/
int FastFourierTransformer::getMaximumDensityIndex()
{
    const int halfN = m_last_n / 2;
    float maxDensity = 0;
    int maxDensityIndex = 0;
    float densitySquared = 0.f;

    for (int k = 1; k < halfN; k++) {
        // Here, we calculate the frequency k/N.
        // k=1, the wave oscillation time is N, and the frequency
        //      is 1/sample.
        // k=2, the wave oscillation time is N/2, and the frequency
        //      is 2/sample.
        // k=3, the wave oscillation time is N/3, and the frequency
        //      is 3/sample.
        // Note, that the documentation is for Fortran, so indexes in the
        // documentation does not match.
        // The sine and cosine coefficients are obtained thus as follows:
        const float cosCoefficient = qAbs(m_waveFloat[2 * k - 1]);
        const float sinCoefficient = qAbs(m_waveFloat[2 * k]);

        densitySquared =
            sinCoefficient * sinCoefficient + cosCoefficient * cosCoefficient;

        if (densitySquared > maxDensity) {
            maxDensity = densitySquared;
            maxDensityIndex = k;
        }
    }

    if (m_cutOffForDensitySquared < maxDensity) {
        return maxDensityIndex;
    }

    return -1;
}


/*!
  Sets the cutoff density.
*/
void FastFourierTransformer::setCutOffForDensity(float cutoff)
{
    m_cutOffForDensitySquared = cutoff * cutoff;
}
