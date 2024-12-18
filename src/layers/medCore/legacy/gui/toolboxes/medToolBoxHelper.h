#pragma once
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

#include <medCoreExport.h>

#include <QDialog>
#include <QVBoxLayout>

/**
 * @brief This widget displays a window with rich text documentation.
 * Inputs are a list of QString: the first is the main text documentation in html.
 * The second is the joined list of authors. The third is the plugin version. 
 */
class MEDCORE_EXPORT medToolBoxHelper : public QDialog
{
    Q_OBJECT

public:
     medToolBoxHelper(QList<QString> toolboxDocStrings);

private:
    void setInformationLine(QVBoxLayout *layout, QString description, QString info);
};
