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
#include <QComboBox>
#include <QHeaderView>
#include <QPushButton>
#include <QTextEdit>

#include <medSourcesLoader.h>
#include <medSourceSettingsDragAreaWidget.h>
#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medSourcesSettings : public QWidget
{
    Q_OBJECT

public:
    explicit medSourcesSettings(medSourcesLoader * pSourceLoader, QWidget *parent = nullptr);

protected:
    void createNewSource();
    void updateSelectedSourceDescription(int currentIndex);
    void updateWhenASourceIsSelected(medAbstractSource * pSource);
    void updateWhenDefaultIsPushed();
    void updateWhenConnectIsPushed();

private:
    medSourcesLoader * sourceLoader;
    medAbstractSource * selectedSource;
    medSourceSettingsDragAreaWidget * dragDropAreaWidget;
    QTextEdit * textEdit;
    QStringList sourceTypeList;
    QStringList sourceNameList;
    QStringList sourceDescriptionList;
    QPushButton * connectButton;
    QComboBox * listCombobox;
};
