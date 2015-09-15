#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>
#include <QTransform>
#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractRegistrationProcessingProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractRegistrationProcess : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractRegistrationProcess(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractRegistrationProcess(void) {}

public:

    /*virtual void setUpdateRule(unsigned char updateRule) =0;
    virtual void setGradientType(unsigned char gradientType)=0;
    virtual void setMaximumUpdateLength(float maximumUpdateStepLength)=0;
    virtual void setUpdateFieldStandardDeviation(float updateFieldStandardDeviation)=0;
    virtual void setDisplacementFieldStandardDeviation(float displacementFieldStandardDeviation)=0;
    virtual void setUseHistogramMatching(bool useHistogramMatching)=0;
    virtual void setNumberOfIterations(std::vector<unsigned int> iterations)=0;*/
    
    virtual void setFixedImage (medAbstractImageData* image) = 0;
    virtual void setMovingImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* image(void) const = 0;
    virtual medAbstractImageData* transform(void) const = 0;

};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractRegistrationProcess*)
DTK_DECLARE_PLUGIN        (medAbstractRegistrationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractRegistrationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractRegistrationProcess, MEDCORE_EXPORT)

