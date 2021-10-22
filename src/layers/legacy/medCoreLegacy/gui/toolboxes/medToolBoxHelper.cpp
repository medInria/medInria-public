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

medToolBoxHelper::medToolBoxHelper(QList<QString> toolboxDocStrings): 
    QDialog()
{
    setWindowTitle(tr("Help"));
    setMinimumHeight(500);
    setMinimumWidth(400);

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    // Toolbox documentation core
    QTextEdit *documentationWidget = new QTextEdit();
    documentationWidget->setText(toolboxDocStrings.at(0));
    documentationWidget->setTextInteractionFlags(Qt::TextBrowserInteraction);
    documentationWidget->setAlignment(Qt::AlignTop);
    layout->addWidget(documentationWidget);

    setInformationLine(layout, QString("Author(s): "), toolboxDocStrings.at(1));
    setInformationLine(layout, QString("Contact: "),   toolboxDocStrings.at(2));
    setInformationLine(layout, QString("Version: "),   toolboxDocStrings.at(3));
}

void medToolBoxHelper::setInformationLine(QVBoxLayout *layout, QString description, QString info)
{
    if (!info.isEmpty())
    {
        QLabel *informationLine = new QLabel(description + info);
        informationLine->setTextInteractionFlags(Qt::TextSelectableByMouse);
        layout->addWidget(informationLine);
    }
}
