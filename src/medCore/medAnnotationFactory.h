/*
 * medAnnotationFactory.h
 *
 *  Created on: 31 mai 2011
 *      Author: jstark
 */
#ifndef MEDANNOTATIONFACTORY_H_
#define MEDANNOTATIONFACTORY_H_

#include "medCoreExport.h"

#include <dtkCore/dtkAbstractFactory.h>

#include "medAbstractAnnotationRepresentation.h"
#include <QString>

#include <memory>

class medAnnotationData;
class medAnnotationFactoryPrivate;
class medAnnotationGraphicsObject;

class MEDCORE_EXPORT medAnnotationFactory : public dtkAbstractFactory {
    Q_OBJECT;
public:
    typedef QObject * (*CreatorFunc)(void);

public:
    medAnnotationFactory();
    virtual ~medAnnotationFactory();

    bool registerAnnotation(const QString &annotationName, const QStringList & dataNames, const QStringList & viewNames, CreatorFunc func );
    bool registerAnnotation(const QString &annotationName, const QString & dataName, const QString & viewName, CreatorFunc func );

    QObject * create( const QString &annotationName);
    QObject * createAnnotationForData( const medAnnotationData * annData, const QString & viewName);

    QString annotationForData( const QString & dataName, const QString & viewName );

    static medAnnotationFactory* instance();
    static void destroy();

private:
    medAnnotationFactoryPrivate * d;
};

#endif /* MEDANNOTATIONFACTORY_H_ */
