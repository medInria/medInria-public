#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractDilateFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractDilateFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractDilateFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractDilateFilter(void) {}

public:
    virtual void setRadius(double radius) = 0;
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractDilateFilter*)
DTK_DECLARE_PLUGIN        (medAbstractDilateFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractDilateFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractDilateFilter, MEDCORE_EXPORT)

