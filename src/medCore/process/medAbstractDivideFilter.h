#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractDivideFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractDivideFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractDivideFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractDivideFilter(void) {}

public:
    virtual void setValue(double value) = 0;
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractDivideFilter*)
DTK_DECLARE_PLUGIN        (medAbstractDivideFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractDivideFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractDivideFilter, MEDCORE_EXPORT)

