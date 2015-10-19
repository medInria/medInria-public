#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractMedianFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractMedianFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractMedianFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractMedianFilter(void) {}

public:
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractMedianFilter*)
DTK_DECLARE_PLUGIN        (medAbstractMedianFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractMedianFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractMedianFilter, MEDCORE_EXPORT)

