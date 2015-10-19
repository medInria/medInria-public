#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractAddFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractAddFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractAddFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractAddFilter(void) {}

public:
    virtual void setValue(double value) = 0;
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractAddFilter*)
DTK_DECLARE_PLUGIN        (medAbstractAddFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractAddFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractAddFilter, MEDCORE_EXPORT)

