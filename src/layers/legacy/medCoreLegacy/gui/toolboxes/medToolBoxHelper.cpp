/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <medToolBoxHelper.h>

#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

medToolBoxHelper::medToolBoxHelper(QWidget *parent, QList<QString> toolboxDocStrings)
    : QDialog(parent)
{
    setWindowTitle(tr("Help"));
    setMinimumHeight(400);
    setMinimumWidth(400);

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    // Toolbox documentation core
    QTextEdit *documentationWidget = new QTextEdit();
    documentationWidget->setText(toolboxDocStrings.at(0));
    documentationWidget->setTextInteractionFlags(Qt::TextBrowserInteraction);
    documentationWidget->setAlignment(Qt::AlignTop);
    layout->addWidget(documentationWidget);

    // Toolbox authors list
    if (!toolboxDocStrings.at(1).isEmpty())
    {
        QLabel *toolboxAuthors = new QLabel(toolboxDocStrings.at(1));
        layout->addWidget(toolboxAuthors);
    }

    // Toolbox version number
    if (!toolboxDocStrings.at(2).isEmpty())
    {
        QLabel *toolboxVersion = new QLabel("Version: "+toolboxDocStrings.at(2));
        layout->addWidget(toolboxVersion);
    }
}
