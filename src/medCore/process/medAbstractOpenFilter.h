#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractOpenFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractOpenFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractOpenFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractOpenFilter(void) {}

public:
    virtual void setRadius(double radius) = 0;
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractOpenFilter*)
DTK_DECLARE_PLUGIN        (medAbstractOpenFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractOpenFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractOpenFilter, MEDCORE_EXPORT)

