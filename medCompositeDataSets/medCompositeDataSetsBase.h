#ifndef MEDCOMPOSITEDATESETBASE_H
#define MEDCOMPOSITEDATESETBASE_H

#include <map>
#include <string>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkLog.h>

namespace MedInria {

    //  A base class for all composite data sets.

    class medCompositeDataSetsBase: public dtkAbstractData {
    public:

        virtual bool    registered()  const = 0;
        virtual QString description() const = 0;

        //  Verification that there is a handler for this specific type and version (major,minor) of the format.
        //  Returns a new instance of a reader/writer for that format.

        static medCompositeDataSetsBase* known(const std::string& type,const unsigned major,const unsigned minor) {
            for (Registery::const_iterator i=registery().begin();i!=registery().end();++i)
                if (type==i->first && i->second->has_version(major,minor))
                    return i->second->clone(major,minor);

            return 0;
        }

        static medCompositeDataSetsBase* find(const QString& desc) {
            for (Registery::const_iterator i=registery().begin();i!=registery().end();++i)
                if (i->second->description()==desc)
                    return i->second->clone(-1,-1);
            return 0;
        }

        //  Give the tag and version associated to the data set.

        virtual QString tag()     const = 0;
        virtual QString version() const = 0;

        //  Check that the plugin can handle a given version (major,mnior) of the file format.

        virtual bool has_version(const unsigned,const unsigned) const = 0;

        //  Create a new instance tuned for the revision version (major,mnior) of the file format.
        //  Use -1 as major to indicate the default version of the file format.

        virtual medCompositeDataSetsBase* clone(const int major,const int minor) const = 0;

        //  Read the description from an array.

        virtual bool read_description(const QByteArray& buf) = 0;
        virtual bool read_data() = 0;

        virtual bool write_description(QTextStream&) = 0;
        virtual bool write_data(const QString&,const dtkAbstractData*) = 0;

        static QStringList initialize() {
            QStringList& ql = known_types();
            for (Registery::const_iterator i=registery().begin();i!=registery().end();++i) {
                const QString desc = i->second->description();
                if (!(i->second->registered()))
                    dtkWarning() << "Unable to register " << desc << " type";
                ql << desc;
            }
            qDebug() << ql;
            return ql;
        }

        static QStringList handled() { return known_types(); }

    protected:

        medCompositeDataSetsBase(const char* name,const medCompositeDataSetsBase* proto) {
            registery().insert(Registery::value_type(name,proto));
        }

    private:

        typedef std::map<std::string,const medCompositeDataSetsBase*> Registery;

        static Registery& registery() {
            static Registery reg;
            return reg;
        }

        static QStringList& known_types() {
            static QStringList types;
            return types;
        }
    };
}

#endif  //  ! MEDCOMPOSITEDATESETBASE_H
