
#ifndef _msegAlgorithmConnectedThreshold_h_
#define _msegAlgorithmConnectedThreshold_h_

#include "msegAlgorithmGeneric.h"

#include <QVector3D>

class dtkAbstractData;
class dtkAbstractProcessFactory;

namespace mseg {

    template < typename TPixel, unsigned int N > class AlgorithmConnectedThresholdPrivate;

/** Connected Region Growing segmentation algorithm */
class AlgorithmConnectedThreshold : public AlgorithmGeneric {
    Q_OBJECT;
public:
    AlgorithmConnectedThreshold();
    ~AlgorithmConnectedThreshold();
// Override AlgorithmGeneric
    QString localizedName() MED_OVERRIDE;

    QList<QVector3D> seedPoints() const { return m_seedPoints; }
    void addSeedPoint(const QVector3D & val) { 
        if (!m_seedPoints.contains(val))
            m_seedPoints.push_back( val ); 
    }

    double lowThreshold() const { return m_lowThreshold; }
    void setLowThreshold(double val) { m_lowThreshold = val; }
    double highThreshold() const { return m_highThreshold; }
    void setHighThreshold(double val) { m_highThreshold = val; }

    void run();

    static dtkAbstractProcess * create();

    /** Get name to use for this when registering with a factory.*/
    static QString s_identifier();

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

#endif // _msegAlgorithmConnectedThreshold_h_
