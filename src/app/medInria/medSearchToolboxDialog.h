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
#include <QTreeWidget>

class QLineEdit;
class QPushButton;

class medSearchToolboxDialog : public QDialog
{
    Q_OBJECT

public:
    medSearchToolboxDialog(QWidget *parent, QHash<QString, QStringList> toolboxDataHash);
    QStringList getFindText();

protected slots:
    /**
     * @brief searchTextChanged is called when the user write something in the search bar.
     * The according items are hidden and shown in the tree.
     */
    void searchTextChanged(const QString& text);

    /**
     * @brief searchItemDoubleClicked is called when the user double-click on an item.
     * The according workspace is opened.
     */
    void searchItemDoubleClicked(QTreeWidgetItem* item, int column);

private:
    QPushButton *findButton;
    QLineEdit *lineEdit;
    QStringList findText;
    QTreeWidget *tree;
    QHash<QString, QStringList> toolboxDataHash;
};

