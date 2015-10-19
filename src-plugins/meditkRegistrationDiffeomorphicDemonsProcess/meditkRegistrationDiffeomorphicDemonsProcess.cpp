#include "meditkRegistrationDiffeomorphicDemonsProcess.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkRegistrationDiffeomorphicDemonsProcess::meditkRegistrationDiffeomorphicDemonsProcess(void)
{
    this->setNumberOfIterations("15x10x5");
    m_updateRule=0;
    m_gradientType=0;
    m_maximumUpdateStepLength=2.0;
    m_updateFieldStandardDeviation=0.0;
    m_displacementFieldStandardDeviation=1.5;
    m_useHistogramMatching=false;
}

meditkRegistrationDiffeomorphicDemonsProcess::~meditkRegistrationDiffeomorphicDemonsProcess(void)
{

}

void meditkRegistrationDiffeomorphicDemonsProcess::setFixedImage(medAbstractImageData* image)
{
    m_fixedImage=image;
}

void meditkRegistrationDiffeomorphicDemonsProcess::setMovingImage(medAbstractImageData* image)
{
    m_movingImage=image;
}

medAbstractImageData* meditkRegistrationDiffeomorphicDemonsProcess::transformedImage(void) const
{
    return m_transformedImage;
}

/*medAbstractData* meditkRegistrationDiffeomorphicDemonsProcess::transform(void) const
{
    return m_transform;
}*/


void meditkRegistrationDiffeomorphicDemonsProcess::setDisplacementFieldStandardDeviation(double value)
{
        qDebug()<<Q_FUNC_INFO;
    m_displacementFieldStandardDeviation=value;
}

void meditkRegistrationDiffeomorphicDemonsProcess::setGradientType(int type_id)
{
        qDebug()<<Q_FUNC_INFO;
    m_gradientType=type_id;
}

void meditkRegistrationDiffeomorphicDemonsProcess::setUpdateRule(int rule_id)
{
        qDebug()<<Q_FUNC_INFO;
    m_updateRule=rule_id;
}

void meditkRegistrationDiffeomorphicDemonsProcess::setUpdateFieldStandardDeviation(double value)
{
        qDebug()<<Q_FUNC_INFO;
    m_updateFieldStandardDeviation=value;
}

void meditkRegistrationDiffeomorphicDemonsProcess::setMaximumUpdateLength(double value)
{
    qDebug()<<Q_FUNC_INFO;
    m_maximumUpdateStepLength=value;
}

void meditkRegistrationDiffeomorphicDemonsProcess::setUseHistogramMatching(bool useHisto)
{
    qDebug()<<Q_FUNC_INFO;
    m_useHistogramMatching=useHisto;
}

void meditkRegistrationDiffeomorphicDemonsProcess::setOutputFile(QString file)
{
    m_outputFile=file;
}

void meditkRegistrationDiffeomorphicDemonsProcess::setNumberOfIterations(QString text)
{
    QStringList tokens=text.split("x");
    m_iterations.clear();
    for(QString token:tokens)
    {
        bool ok=false;
        int tmp=token.toInt(&ok);
        if(ok)
            m_iterations.push_back(tmp);
        else
            qDebug()<<"wrong format";
    }
}

void meditkRegistrationDiffeomorphicDemonsProcess::run(void)
{
    process.setFixedInput(m_fixedImage);
    process.setMovingInput(m_movingImage);

    process.setDisplacementFieldStandardDeviation(m_displacementFieldStandardDeviation);
    process.setGradientType(m_gradientType);
    process.setMaximumUpdateLength(m_maximumUpdateStepLength);
    process.setNumberOfIterations(m_iterations);
    process.setUseHistogramMatching(m_useHistogramMatching);
    process.setUpdateRule(m_updateRule);
    process.setUpdateFieldStandardDeviation(m_updateFieldStandardDeviation);
    process.run();
    if(!m_outputFile.isEmpty())
    {
        QStringList list;
        list<<""<<m_outputFile;
        process.write(list);
    }
    m_transformedImage=dynamic_cast<medAbstractImageData*>(process.output());

}
