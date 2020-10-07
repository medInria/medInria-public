#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>

#include <medWidgetsExport.h>

class QWidget;

class medAbstractParameter;

class medAbstractParameterPresenterPrivate;
class MEDWIDGETS_EXPORT medAbstractParameterPresenter : public QObject
{
    Q_OBJECT

public:
    medAbstractParameterPresenter(medAbstractParameter *parent = nullptr);
    virtual ~medAbstractParameterPresenter();

    virtual QWidget *buildWidget() = 0;
    virtual medAbstractParameter *parameter() const = 0;

    void setVisible(bool visibility);
    bool isVisible() const;
    void setEnable(bool enabled);
    bool isEnable() const;

public:
    static medAbstractParameterPresenter* buildFromParameter(medAbstractParameter* parameter);

protected:
    void _connectWidget(QWidget *widget);

signals:
    void _visibilityChanged(bool);
    void _isEnabledChanged(bool);

private:
    const QScopedPointer<medAbstractParameterPresenterPrivate> d;
};

