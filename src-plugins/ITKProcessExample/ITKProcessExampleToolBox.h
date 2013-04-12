
#pragma once
#define ITKPROCESSEXAMPLEDENOISINGTOOLBOX_H

#include <medFilteringAbstractToolBox.h>

class ITKProcessExampleToolBoxPrivate;

class ITKProcessExampleToolBox : public medFilteringAbstractToolBox
{
    Q_OBJECT

public:
     ITKProcessExampleToolBox(QWidget *parentToolBox = 0);
    ~ITKProcessExampleToolBox();

public:
    static bool registered();
    dtkAbstractData* processOutput();

signals:
    void success();
    void failure();

public slots:

    void run();

private:
    ITKProcessExampleToolBoxPrivate *d;
};



