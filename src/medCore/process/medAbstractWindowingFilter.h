#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractWindowingFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractWindowingFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractWindowingFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractWindowingFilter(void) {}

public:
    virtual void setMinimumIntensityValue(double value) = 0;
    virtual void setMaximumIntensityValue(double value) = 0;
    virtual void setMinimumOutputIntensityValue(double value) = 0;
    virtual void setMaximumOutputIntensityValue(double value) = 0;
    
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractWindowingFilter*)
DTK_DECLARE_PLUGIN        (medAbstractWindowingFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractWindowingFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractWindowingFilter, MEDCORE_EXPORT)

