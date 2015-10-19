#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractGaussianFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractGaussianFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractGaussianFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractGaussianFilter(void) {}

public:
    virtual void setSigma(double sigma) = 0;
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractGaussianFilter*)
DTK_DECLARE_PLUGIN        (medAbstractGaussianFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractGaussianFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractGaussianFilter, MEDCORE_EXPORT)

