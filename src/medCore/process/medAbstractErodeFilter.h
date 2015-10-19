#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractErodeFilteringProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractErodeFilter : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractErodeFilter(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractErodeFilter(void) {}

public:
    virtual void setRadius(double radius) = 0;
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* filteredImage(void) const = 0;


};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractErodeFilter*)
DTK_DECLARE_PLUGIN        (medAbstractErodeFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractErodeFilter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractErodeFilter, MEDCORE_EXPORT)

