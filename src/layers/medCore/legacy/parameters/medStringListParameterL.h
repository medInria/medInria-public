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

#include <QIcon>

#include <medAbstractParameterL.h>

class QComboBox;
class QWidget;
class QStringList;

class medStringListParameterLPrivate;
class MEDCORE_EXPORT medStringListParameterL: public medAbstractStringParameterL
{
    Q_OBJECT

public:
    static QIcon createIconFromColor(const QString &colorName);

    medStringListParameterL(QString name = "Unknown string list parameter", QObject* parent = nullptr);
    virtual ~medStringListParameterL();

    void addItem(QString item, QIcon icon = QIcon());
    void addItems(QStringList items);
    void removeItem(QString item);
    void clear();

    QStringList items() const;
    QComboBox* getComboBox();

    virtual QWidget* getWidget();

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);
    static QString fromString(QString value);

protected:
    virtual void updateInternWigets();

private slots:
    void removeInternComboBox();

private:
    medStringListParameterLPrivate* d;
};
