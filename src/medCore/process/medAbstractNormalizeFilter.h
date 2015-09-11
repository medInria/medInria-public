#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractNormalizeFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractNormalizeFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractNormalizeFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractNormalizeFilter(void) {}

public:
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractNormalizeFilter*)
DTK_DECLARE_PLUGIN        (medAbstractNormalizeFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractNormalizeFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractNormalizeFilter, MEDCORE_EXPORT)

