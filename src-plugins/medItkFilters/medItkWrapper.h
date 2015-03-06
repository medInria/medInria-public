#pragma once

#include <QString>
class medItkFiltersProcessBase;

class medItkWrapper
{
public:
    medItkWrapper(medItkFiltersProcessBase* aBaseProcess) : d(aBaseProcess) { }
    virtual int update(QString) = 0;
    virtual ~medItkWrapper() { }

    medItkFiltersProcessBase* d;
    QString resultString;
};

template <template <typename, typename> class Prototype2>
class PrototypeInternal2
{
public:
     template <class T>
     int update(medItkFiltersProcessBase*)
     {
        // Overload with specialisation of this class
         return 0;
     }

    QString resultString;
};

template <template <typename, typename, typename> class Prototype3>
class PrototypeInternal3
{
public:
     template <class T>
     int update(medItkFiltersProcessBase*)
     {
        // Overload with specialisation of this class
         return 0;
     }

    QString resultString;
};

template <template <typename, typename> class Prototype2>
class TemplateWrapper2 : public medItkWrapper
{
public:
    TemplateWrapper2(medItkFiltersProcessBase* aBaseProcess) : medItkWrapper(aBaseProcess) { }
    int update(QString id)
    {
        qDebug() << "calling " << id;
        int res = 0;
        PrototypeInternal2<Prototype2> FilterProcess;
        if ( id == "medItkChar3ImageData" )
        {
            res = FilterProcess.template update<char>(d);
        }
        else if ( id == "medItkUChar3ImageData" )
        {
            res = FilterProcess.template update<unsigned char>(d);
        }
        else if ( id == "medItkShort3ImageData" )
        {
            res = FilterProcess.template update<short>(d);
        }
        else if ( id == "medItkUShort3ImageData" )
        {
            res = FilterProcess.template update<unsigned short>(d);
        }
        else if ( id == "medItkInt3ImageData" )
        {
            res = FilterProcess.template update<int>(d);
        }
        else if ( id == "medItkUInt3ImageData" )
        {
            res = FilterProcess.template update<unsigned int>(d);
        }
        else if ( id == "medItkLong3ImageData" )
        {
            res = FilterProcess.template update<long>(d);
        }
        else if ( id== "medItkULong3ImageData" )
        {
            res = FilterProcess.template update<unsigned long>(d);
        }
        else if ( id == "medItkFloat3ImageData" )
        {
            res = FilterProcess.template update<float>(d);
        }
        else if ( id == "medItkDouble3ImageData" )
        {
            res = FilterProcess.template update<double>(d);
        }
        else
        {
            FilterProcess.resultString = "Error : pixel type not yet implemented (" + id + ")";
            res = EXIT_FAILURE;
        }

        resultString = FilterProcess.resultString;
        return res;

    }

};

template <template <typename, typename, typename> class Prototype3>
class TemplateWrapper3 : public medItkWrapper
{
public:
    TemplateWrapper3(medItkFiltersProcessBase* aBaseProcess) : medItkWrapper(aBaseProcess) { }
    int update(QString id)
    {
        qDebug() << "calling " << id;
        int res = 0;
        PrototypeInternal3<Prototype3> FilterProcess;
        if ( id == "medItkChar3ImageData" )
        {
            res = FilterProcess.template update<char>(d);
        }
        else if ( id == "medItkUChar3ImageData" )
        {
            res = FilterProcess.template update<unsigned char>(d);
        }
        else if ( id == "medItkShort3ImageData" )
        {
            res = FilterProcess.template update<short>(d);
        }
        else if ( id == "medItkUShort3ImageData" )
        {
            res = FilterProcess.template update<unsigned short>(d);
        }
        else if ( id == "medItkInt3ImageData" )
        {
            res = FilterProcess.template update<int>(d);
        }
        else if ( id == "medItkUInt3ImageData" )
        {
            res = FilterProcess.template update<unsigned int>(d);
        }
        else if ( id == "medItkLong3ImageData" )
        {
            res = FilterProcess.template update<long>(d);
        }
        else if ( id== "medItkULong3ImageData" )
        {
            res = FilterProcess.template update<unsigned long>(d);
        }
        else if ( id == "medItkFloat3ImageData" )
        {
            res = FilterProcess.template update<float>(d);
        }
        else if ( id == "medItkDouble3ImageData" )
        {
            res = FilterProcess.template update<double>(d);
        }
        else
        {
            FilterProcess.resultString = "Error : pixel type not yet implemented (" + id + ")";
            res = EXIT_FAILURE;
        }

        resultString = FilterProcess.resultString;
        return res;

    }
};
