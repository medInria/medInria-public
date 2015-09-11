#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractCloseFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractCloseFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractCloseFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractCloseFilter(void) {}

public:
    virtual void setRadius(double radius) = 0;
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractCloseFilter*)
DTK_DECLARE_PLUGIN        (medAbstractCloseFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractCloseFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractCloseFilter, MEDCORE_EXPORT)

