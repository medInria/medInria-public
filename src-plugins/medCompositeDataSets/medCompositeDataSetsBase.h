/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <map>
#include <string>

#include <dtkCore/dtkAbstractData.h>
#include <dtkLog/dtkLog.h>

namespace MedInria {

    //  A base class for all composite data sets.

    class medCompositeDataSetsBase: public dtkAbstractData {
    public:

        virtual bool    registered()  const = 0;
        virtual QString identifier()  const = 0;
        virtual QString description() const = 0;

        //  Verification that there is a handler for this specific type and version (major,minor) of the format.
        //  Returns a new instance of a reader/writer for that format.

        static medCompositeDataSetsBase* known(const std::string& type,const unsigned major,const unsigned minor) {
            for (Registery::const_iterator i=registery().begin();i!=registery().end();++i)
                if (type==i->first && i->second->has_version(major,minor))
                    return i->second->clone(major,minor);
            return 0;
        }

        static medCompositeDataSetsBase* find(dtkAbstractData* data) {
            const QString& desc = data->identifier();
            for (Registery::const_iterator i=registery().begin();i!=registery().end();++i)
                if (i->second->identifier()==desc)
                    return dynamic_cast<medCompositeDataSetsBase*>(data);
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
        virtual bool read_data(const QString&) = 0;

        virtual bool write_description(QTextStream&) = 0;
        virtual bool write_data(const QString&) = 0;

        virtual QImage& thumbnail() const = 0;

        static QStringList initialize() {
            QStringList& ql = known_types();
            for (Registery::const_iterator i=registery().begin();i!=registery().end();++i) {
                const QString desc = i->second->identifier();
                if (!(i->second->registered())) {
                    dtkWarn() << "Unable to register " << desc << " type";
                }
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


