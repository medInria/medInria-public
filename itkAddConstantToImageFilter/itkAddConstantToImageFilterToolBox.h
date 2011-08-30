
#ifndef ITKADDCONSTANTTOOLBOX_H
#define ITKADDCONSTANTTOOLBOX_H

#include <medToolBoxFilteringCustom.h>

class itkAddConstantToImageFilterToolBoxPrivate;

class itkAddConstantToImageFilterToolBox : public medToolBoxFilteringCustom
{
    Q_OBJECT

public:
     itkAddConstantToImageFilterToolBox(QWidget *parent = 0);
    ~itkAddConstantToImageFilterToolBox(void);

public:
    static bool registered(void);
    dtkAbstractData* processOutput();

signals:
    void success(void);
    void failure(void);

public slots:

    void run(void);

private:
    itkAddConstantToImageFilterToolBoxPrivate *d;
};

medToolBoxFilteringCustom *createitkAddConstantToImageFilterToolBox(QWidget *parent);

#endif

