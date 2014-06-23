/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include <medCoreExport.h>


class medDatabaseSearchPanelPrivate;

class EditCombo : public QWidget
{
    Q_OBJECT
public:
    EditCombo(QString _text, int _column);
    ~EditCombo();
signals:

    void textChanged(const QString& , int);

protected slots:
    void onTextChanged(const QString&);

private:
    QLineEdit edit;
    QLabel label;
    int column;

};

class MEDCORE_EXPORT medDatabaseSearchPanel : public medToolBox
{
    Q_OBJECT
public:
    medDatabaseSearchPanel(QWidget *parent = 0);
    ~medDatabaseSearchPanel();

    void setColumnNames(const QStringList &columns);

signals:
    void filter(const QString &text, int column);

protected slots:

    void addBox();
    void removeBox();

private:
    medDatabaseSearchPanelPrivate* d;
};


