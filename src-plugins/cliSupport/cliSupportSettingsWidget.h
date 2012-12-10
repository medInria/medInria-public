/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medSettingsWidget.h>

class cliSupportSettingsWidgetPrivate;

class cliSupportSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
public:
    explicit cliSupportSettingsWidget(QWidget *parent = 0);

    QString identifier()  const;
    QString description() const;

public slots:
    void read();

protected:
    bool validate();
    virtual bool write();

private:
  cliSupportSettingsWidgetPrivate * d;
};

