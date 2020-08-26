#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractViewNavigator.h>

#include<medAbstractParameterL.h>
#include <medCoreLegacyExport.h>
#include <medImageViewEnum.h>

#include <QDomDocument>

class medAbstractView;
class medBoolParameterL;

class medAbstractNavigatorPrivate;
class MEDCORELEGACY_EXPORT medAbstractNavigator : public dtkAbstractViewNavigator
{
    Q_OBJECT

public:
    medAbstractNavigator(medAbstractView* parent);
    virtual ~medAbstractNavigator();

    virtual QString description() const = 0;
    virtual QString name() const;
    virtual QString version() const;

    QWidget* toolBoxWidget();
    QWidget* toolBarWidget();

    virtual QList<medAbstractParameterL*> linkableParameters() = 0;
    virtual QList<medBoolParameterL*> mouseInteractionParameters() = 0;

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);
    void fromXMLNode(QDomElement *element);

protected:
    virtual QWidget* buildToolBoxWidget() = 0;
    virtual QWidget* buildToolBarWidget() = 0;

    virtual void restoreParameters(QHash<QString,QString> parameters);

private slots:


private:
    medAbstractNavigatorPrivate *d;

/*=========================================================================
                        NOT IMPLEMENTED HERE ON PURPOSE
*=========================================================================*/
public slots:
    virtual void updateWidgets();

private:
    using dtkAbstractViewNavigator::setView;
    using dtkAbstractViewNavigator::view;

    using dtkAbstractViewNavigator::setData;
    using dtkAbstractViewNavigator::data;
    using dtkAbstractViewNavigator::output;
    using dtkAbstractViewNavigator::predraw;
    using dtkAbstractViewNavigator::draw;
    using dtkAbstractViewNavigator::postdraw;
    using dtkAbstractViewNavigator::enabled;
    using dtkAbstractViewNavigator::enable;
    using dtkAbstractViewNavigator::disable;
};
