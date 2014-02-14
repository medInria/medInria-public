/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"
#include <medSettingsWidget.h>
#include <QWidget>

class medSystemSettingsWidgetPrivate;

class MEDGUI_EXPORT medSystemSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
public:
    medSystemSettingsWidget(QWidget *parent = 0);

    QString identifier()  const;
    QString description() const;

signals:

public slots:
    void read();

protected:
    /**
     * @brief Performs the validation of each control inside the widget.
     *
     * @param void
     * @return true is the validation is successful, false otherwise.
    */
    bool validate();

    virtual bool write();

private:
  medSystemSettingsWidgetPrivate *d;

  /**
   * @brief Validates a line of text containing paths.
   * For the text to be valid, it has to match any of the
   * following three regular expressions: <path>[:<path>]*
   * or [<path>:]+ or just <path>, where <path> are valid paths in the system.
   * Note that colons shall be used to separate paths, which is the Unix standard,
   * not like semicolons, a Windows standard.
   *
   * @param The string containing the paths
   * @return true is the paths are valid, false otherwise
  */
  bool validatePaths(QString paths);
};

MEDGUI_EXPORT medSettingsWidget* createSystemSettingsWidget(QWidget *parent);

