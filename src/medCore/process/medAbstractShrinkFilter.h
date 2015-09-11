#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractShrinkFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractShrinkFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractShrinkFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractShrinkFilter(void) {}

public:
    virtual void setXShrink(double xShrink) = 0;
    virtual void setYShrink(double yShrink) = 0;
    virtual void setZShrink(double zShrink) = 0;
    
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractShrinkFilter*)
DTK_DECLARE_PLUGIN        (medAbstractShrinkFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractShrinkFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractShrinkFilter, MEDCORE_EXPORT)

