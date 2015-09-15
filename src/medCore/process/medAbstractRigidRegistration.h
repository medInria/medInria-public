#pragma once

#include <dtkCore>

#include <QObject>
#include <QRunnable>
#include <QMatrix4x4>

#include <data/medAbstractImageData.h>

class medAbstractImageData;

// ///////////////////////////////////////////////////////////////////
// medAbstractRigidRegistrationingProcess process interface
// ///////////////////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractRigidRegistration : public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractRigidRegistration(QObject *parent = NULL) : QObject(parent), QRunnable() {}
    virtual ~medAbstractRigidRegistration(void) {}

public:
    virtual void setFixedImage (medAbstractImageData*  fixedImage) = 0;
    virtual void setMovingImage(medAbstractImageData* movingImage) = 0;

public: 
    virtual medAbstractImageData*   transformedImage(void) const = 0;
    virtual QMatrix4x4              transform       (void) const = 0;

};

// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////
DTK_DECLARE_OBJECT        (medAbstractRigidRegistration*)
DTK_DECLARE_PLUGIN        (medAbstractRigidRegistration, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractRigidRegistration, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractRigidRegistration, MEDCORE_EXPORT)

