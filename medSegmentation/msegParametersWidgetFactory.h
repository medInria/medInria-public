/*
 * msegParametersWidgetFactory.h
 *
 *  Created on: 31 mai 2011 14:55:00
 *      Author: jstark
 */

#ifndef MSEGPARAMETERSWIDGETFACTORY_H_
#define MSEGPARAMETERSWIDGETFACTORY_H_

#include <dtkCore/dtkAbstractDataFactory.h>

namespace mseg {
class AlgorithmParametersWidget;
class Controller;

class ParametersWidgetFactory : dtkAbstractFactory {
public:
    Q_OBJECT;

public:
    typedef AlgorithmParametersWidget *(*CreatorFunc)(Controller *, QWidget *);

public:
    static ParametersWidgetFactory *instance(void);

    /** Register a new widget. 
     ** @return true if successful. false if type already exists.*/
    bool registerWidget(const QString &type, const QString &localizedName, CreatorFunc func);

    QList<QString> widgets(void);

    QString localizedName( const QString &type ) const;

    AlgorithmParametersWidget *create(QString type, Controller *controller, QWidget *parent=0);

signals:
    void widgetAdded( const QString &type );

protected:
    ParametersWidgetFactory();
    virtual ~ParametersWidgetFactory();

    static ParametersWidgetFactory *s_instance;
    class ParametersWidgetFactoryPrivate;
    ParametersWidgetFactoryPrivate *d;
};

}

#endif /* MSEGPARAMETERSWIDGETFACTORY_H_ */

