#pragma once
#include <medAbstractProcess.h>
#include <medAbstractData.h>
#include <reformatPluginExport.h>

class resampleProcessPrivate;
class REFORMATPLUGIN_EXPORT resampleProcess : public medAbstractProcess
{
    Q_OBJECT
public:
    resampleProcess(void);
    virtual ~resampleProcess(void);
    virtual QString description(void) const;
    static bool registered(void);

    public slots:
        //! Input data to the plugin is set through here
        void setInput(medAbstractData *data);
        void setInput(medAbstractData *,int);
        //! Parameters are set through here, channel allows to handle multiple parameters
        void setParameter(double data, int channel);
        //! Method to actually start the filter
        int update(void);
        //! The output will be available through here
        medAbstractData *output(void);
private:
    template <class ImageType> void resample(const char * str);
    resampleProcessPrivate *d;
};
dtkAbstractProcess *createResampleProcess();
