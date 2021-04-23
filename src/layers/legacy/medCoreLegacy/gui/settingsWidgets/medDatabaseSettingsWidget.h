#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include <medCoreLegacyExport.h>

class medDatabaseSettingsWidgetPrivate;

class MEDCORELEGACY_EXPORT medDatabaseSettingsWidget : public QDialog
{
    Q_OBJECT
    
public:
    medDatabaseSettingsWidget(QWidget *parent = nullptr);

public slots:
    void read();

private slots:
    void selectDbDirectory();

protected:
    void write();

private:
    QLineEdit* dbPath;
    QPushButton* btChooseDir;
};
