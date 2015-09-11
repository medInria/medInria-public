#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractMultiplyFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractMultiplyFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractMultiplyFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractMultiplyFilter(void) {}

public:
    virtual void setValue(double value) = 0;
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractMultiplyFilter*)
DTK_DECLARE_PLUGIN        (medAbstractMultiplyFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractMultiplyFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractMultiplyFilter, MEDCORE_EXPORT)

