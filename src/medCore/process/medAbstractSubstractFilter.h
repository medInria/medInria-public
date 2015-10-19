#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractSubstractFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractSubstractFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractSubstractFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractSubstractFilter(void) {}

public:
    virtual void setValue(double value) = 0;
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractSubstractFilter*)
DTK_DECLARE_PLUGIN        (medAbstractSubstractFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractSubstractFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractSubstractFilter, MEDCORE_EXPORT)

