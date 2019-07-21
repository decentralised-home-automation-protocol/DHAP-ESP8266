#include "WString.h"

class Status
{
public:
    virtual String getStatus()
    {
        return "No status information defined.";
    }

    virtual int getMaxLeaseLength()
    {
        //Default to 10 minutes maximum lease length.
        return 100000;
    }

    virtual int getMinUpdatePeriod()
    {
        //Default to 500ms minimum update period.
        return 500;
    }
};