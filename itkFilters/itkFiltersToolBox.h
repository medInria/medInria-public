#ifndef ITKFILTERSTOOLBOX_H
#define ITKFILTERSTOOLBOX_H

#include <medFilteringAbstractToolBox.h>

class itkFiltersToolBoxPrivate;

class itkFiltersToolBox : public medFilteringAbstractToolBox
{
    Q_OBJECT

public:
    itkFiltersToolBox(QWidget *parentToolBox = 0);
    ~itkFiltersToolBox(void);

public:
    static bool registered(void);
    dtkAbstractData* processOutput();

signals:
    void success(void);
    void failure(void);

public slots:

    void onFiltersActivated(int index);

    void clear(void);
    void update(dtkAbstractView *view);

    void run(void);

private:
    itkFiltersToolBoxPrivate *d;
};

medToolBox *createitkFiltersToolBox(QWidget *parent);

#endif // ITKFILTERSTOOLBOX_H
