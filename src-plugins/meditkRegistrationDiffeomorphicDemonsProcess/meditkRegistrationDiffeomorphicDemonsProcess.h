#pragma once

#include <dtkCore>
#include <medCore.h>

#include "itkProcessRegistrationDiffeomorphicDemons.h"
#include "meditkRegistrationDiffeomorphicDemonsProcessToolbox.h"

#include <medAbstractRegistrationProcess.h>


class meditkRegistrationDiffeomorphicDemonsProcess : public medAbstractRegistrationProcess
{
    Q_OBJECT

public:
     meditkRegistrationDiffeomorphicDemonsProcess(void);
    ~meditkRegistrationDiffeomorphicDemonsProcess(void);
    
    virtual void setFixedImage(medAbstractImageData* image);
    virtual void setMovingImage(medAbstractImageData* image);


public: 
    virtual medAbstractImageData* transformedImage(void) const;
    //virtual medAbstractImageData* transform(void) const;

    //virtual QTransform transform(void) const = 0;

public slots:
      void setDisplacementFieldStandardDeviation(double value);
      void setGradientType(int );
      void setUpdateRule(int rule_id);
      void setUpdateFieldStandardDeviation(double value);
      void setMaximumUpdateLength(double value);
      void setUseHistogramMatching(bool useHisto);
      void setNumberOfIterations(QString text);
      void setOutputFile(QString file);

public:
    void run();

private:
    itkProcessRegistrationDiffeomorphicDemons process;
    medAbstractImageData* m_movingImage;
    medAbstractImageData* m_fixedImage;

    medAbstractImageData* m_transformedImage;
    medAbstractData* m_transform;

    std::vector<unsigned int> m_iterations;
    unsigned char m_updateRule;
    unsigned char m_gradientType;
    double m_maximumUpdateStepLength;
    double m_updateFieldStandardDeviation;
    double m_displacementFieldStandardDeviation;
    bool m_useHistogramMatching;
    QString m_outputFile;

};


inline medAbstractRegistrationProcess *meditkRegistrationDiffeomorphicDemonsProcessCreator(void)
{
    return new meditkRegistrationDiffeomorphicDemonsProcess();
}
