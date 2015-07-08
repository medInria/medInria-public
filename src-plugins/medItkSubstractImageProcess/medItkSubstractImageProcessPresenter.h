#pragma once

#include <medAbstractArithmeticOperationProcess.h>

class medItkSubstractImageProcessPresenterPrivate;

class medItkSubstractImageProcessPresenter: public medAbstractSubstractimageProcessPresenter
{
public:
    medItkSubstractImageProcessPresenter(QObject *parent = NULL);
    virtual ~medItkSubstractImageProcessPresenter();

    virtual medAbstractSubstractimageProcess* process() const;
    virtual QWidget* toolbox() const;

    virtual QString name() const {return "yolo";}
    virtual QString description() const {return "yolo";}
    virtual QString version() const {return "yolo";}

private:
    medItkSubstractImageProcessPresenterPrivate *d;
};

// ///////////////////////////////////////////////////////////////////

inline medAbstractSubstractimageProcessPresenter* medItkSubstractImageProcessPresenterCreator(void)
{
    return new medItkSubstractImageProcessPresenter();
}
