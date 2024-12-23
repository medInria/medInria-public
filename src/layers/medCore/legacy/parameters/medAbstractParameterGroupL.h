#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore>
#include <medCoreExport.h>

class medAbstractView;
class medAbstractLayeredView;
class medAbstractParameterGroupLPrivate;

class MEDCORE_EXPORT medAbstractParameterGroupL : public QObject
{
    Q_OBJECT

public:
    medAbstractParameterGroupL(QString name = "", QObject *parent = nullptr, QString workspace = "");
    virtual ~medAbstractParameterGroupL();

    void setName(QString name);
    QString name() const;

    void addParameterToLink(QString parameter);
    void removeParameter(QString parameter);

    void setParametersToLink(QStringList parameters);
    QStringList parametersToLink() const;
    QStringList parametersNotToLink() const;

    void setColor(QColor color);
    QColor color() const;

    virtual void setLinkAllParameters(bool linkAll);
    bool linkAll() const;

    void setWorkspace(QString workspace);
    QString workspace() const;

protected:
    virtual void updatePool() = 0;

signals:
    void groupColorChanged(QColor oldColor, QColor newColor);

private:
    medAbstractParameterGroupLPrivate *d;
};
