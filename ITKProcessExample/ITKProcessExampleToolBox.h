
#ifndef ITKPROCESSEXAMPLETOOLBOX_H
#define ITKPROCESSEXAMPLEDENOISINGTOOLBOX_H

#include <medFilteringAbstractToolBox.h>

class ITKProcessExampleToolBoxPrivate;

class ITKProcessExampleToolBox : public medFilteringAbstractToolBox
{
    Q_OBJECT

public:
     ITKProcessExampleToolBox(QWidget *parentToolBox = 0);
    ~ITKProcessExampleToolBox(void);

public:
    static bool registered(void);
    dtkAbstractData* processOutput();

signals:
    void success(void);
    void failure(void);

public slots:

    void run(void);

private:
    ITKProcessExampleToolBoxPrivate *d;
};

#endif

