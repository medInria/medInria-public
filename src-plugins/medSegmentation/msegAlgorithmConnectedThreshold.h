/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "msegAlgorithmGeneric.h"

#include <QVector3D>

class medAbstractData;
class dtkAbstractProcessFactory;

namespace mseg {

    template < typename TPixel, unsigned int N > class AlgorithmConnectedThresholdPrivate;

/** Connected Region Growing segmentation algorithm 
 * Maintains a list of seed points and a high / low threshold.*/
class AlgorithmConnectedThreshold : public AlgorithmGeneric {
    Q_OBJECT;
public:
    AlgorithmConnectedThreshold();
    ~AlgorithmConnectedThreshold();

    //! Override dtkAbstractObject
    QString description() const { return s_description(); }
    QString identifier() const { return s_identifier(); }

    //! Override AlgorithmGeneric
    QString localizedName() MED_OVERRIDE;

    QList<QVector3D> seedPoints() const { return m_seedPoints; }
    void addSeedPoint(const QVector3D & val) { 
        if (!m_seedPoints.contains(val))
            m_seedPoints.push_back( val ); 
    }

    //! Get / set the high and low threshold.
    double lowThreshold() const { return m_lowThreshold; }
    void setLowThreshold(double val) { m_lowThreshold = val; }
    double highThreshold() const { return m_highThreshold; }
    void setHighThreshold(double val) { m_highThreshold = val; }

    void run();

    //! Creator, using new.
    static dtkAbstractProcess * create();

    /** Get name to use for this when registering with a factory.*/
    static QString s_identifier();
    static QString s_description();

    //! Get a human readable name for this widget.
    /** \param trObj : Provide an object for the tr() function. If NULL qApp will be used. */
    static QString s_localizedName(const QObject * trObj);

private:
    template < typename TPixel, unsigned int N >
        friend class AlgorithmConnectedThresholdPrivate;

    QList<QVector3D> m_seedPoints;

    double m_lowThreshold;
    double m_highThreshold;
};

} // namespace mseg


