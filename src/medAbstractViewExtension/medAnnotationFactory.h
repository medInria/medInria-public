/*
 * medAnnotationFactory.h
 *
 *  Created on: 31 mai 2011
 *      Author: jstark
 */
#ifndef MEDANNOTATIONFACTORY_H_
#define MEDANNOTATIONFACTORY_H_

#include "medAbstractViewExtensionExport.h"

#include <dtkCore/dtkAbstractFactory.h>

#include <QString>

#include <memory>

class medAnnotationFactoryPrivate;
class medAnnotationGraphicsObject;

class MEDABSTRACTVIEWEXTENSION_EXPORT medAnnotationFactory : public dtkAbstractFactory {
    Q_OBJECT;
public:
    typedef medAnnotationGraphicsObject *(*CreatorFunc)(void);

public:
    medAnnotationFactory();
    virtual ~medAnnotationFactory();

    bool registerAnnotation(const QString &annotationName, CreatorFunc func );

    std::auto_ptr<medAnnotationGraphicsObject> create( const QString &annotationName);

    static medAnnotationFactory* instance();
    static void destroy();

private:
    medAnnotationFactoryPrivate * d;
};

#endif /* MEDANNOTATIONFACTORY_H_ */
