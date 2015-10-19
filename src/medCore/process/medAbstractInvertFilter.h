#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractInvertFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractInvertFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractInvertFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractInvertFilter(void) {}

public:
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractInvertFilter*)
DTK_DECLARE_PLUGIN        (medAbstractInvertFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractInvertFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractInvertFilter, MEDCORE_EXPORT)

