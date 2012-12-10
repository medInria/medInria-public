#ifndef cliSupportTOOLBOX_H
#define cliSupportTOOLBOX_H

#include "medToolBox.h"

class dtkAbstractView;
class dtkAbstractData;
class cliSupportToolBoxPrivate;
class medDataIndex;
class medWorkspace;

class cliSupportToolBox : public medToolBox
{
    Q_OBJECT
public:
     cliSupportToolBox(QWidget *parent, medWorkspace * workspace = 0);
    ~cliSupportToolBox();


    virtual void setData(dtkAbstractData *data);

     static QString name();
     static QString identifier();
     static QString description();

protected:
     void init();

public slots:
     virtual void update (dtkAbstractView *view);

protected slots:

     virtual void clear (void);

     void moduleSelected(int index);
     void runCurrentModule();

     void moduleFinished();
     void progressTextChanged(const QString & progressText);
     void progressValueChanged(int progressValue);

private:

    cliSupportToolBoxPrivate *d;

};


#endif
