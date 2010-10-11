#include "dcmtkPacsKey.h"

void dcmtkPacsKey::convert( dcmtkKey* key )
{
    elem = key->elem;
    group = key->group;
    value = QString::fromStdString(key->value);
}
