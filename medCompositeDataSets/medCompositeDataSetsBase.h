#ifndef MEDCOMPOSITEDATESETBASE_H
#define MEDCOMPOSITEDATESETBASE_H

#include <map>
#include <string>

namespace MedInria {

    //  A base class for all composite data sets.

    class medCompositeDataSetsBase {
    public:

        //  Verification that there is a handler for this specific type and version of the format.
        //  Returns a new instance of a reader/writer for that format.

        static medCompositeDataSetsBase* known(const std::string& type,const unsigned version) {
            for (Registery::const_iterator i=reg.begin();i!=reg.end();++i)
                if (type==i->first && i->second->has_version(version)) {
                    return i->second->clone(version);
            }

            return 0;
        }

        //  Check that the plugin can handle a given version ofthe file format.

        virtual bool has_version(const unsigned) const = 0;

        //  Create a new instance tuned for the revision version ofthe file format.

        virtual medCompositeDataSetsBase* clone(const unsigned version) const = 0;

    protected:

        medCompositeDataSetsBase(const std::string& name,const medCompositeDataSetsBase* proto) {
            reg.insert(Registery::value_type(name,proto));
        }

    private:

        typedef std::map<std::string,const medCompositeDataSetsBase*> Registery;

        static Registery reg;
    };
}

#endif  //  ! MEDCOMPOSITEDATESETBASE_H
