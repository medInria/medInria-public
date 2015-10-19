#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>
#include <QTransform>

#include "../medCoreExport.h"

class medAbstractData;
class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractRegistrationProcessingProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractRegistrationProcess : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractRegistrationProcess(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractRegistrationProcess(void) {}

public:    
    virtual void setFixedImage (medAbstractImageData* image) = 0;
    virtual void setMovingImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* transformedImage(void) const = 0;

};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractRegistrationProcess*)
DTK_DECLARE_PLUGIN        (medAbstractRegistrationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractRegistrationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractRegistrationProcess, MEDCORE_EXPORT)
DTK_DECLARE_WIDGET_FACTORY(medAbstractRegistrationProcess, MEDCORE_EXPORT)

