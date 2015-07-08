#include <medItkSubstractImageProcessPresenter.h>

#include <medItkSubstractImageProcess.h>

#include <QLabel>

class medItkSubstractImageProcessPresenterPrivate
{
public:
    medItkSubstractImageProcess* process;
};


medItkSubstractImageProcessPresenter::medItkSubstractImageProcessPresenter(QObject *parent): medAbstractSubstractimageProcessPresenter(parent),
    d(new medItkSubstractImageProcessPresenterPrivate)
{
    d->process = new medItkSubstractImageProcess(this);
}

medItkSubstractImageProcessPresenter::~medItkSubstractImageProcessPresenter()
{
    delete d;
}

medAbstractSubstractimageProcess* medItkSubstractImageProcessPresenter::process() const
{
    return d->process;
}

QWidget* medItkSubstractImageProcessPresenter::toolbox() const
{
    return new QLabel("Le truc de OUF");
}
