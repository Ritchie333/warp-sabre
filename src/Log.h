#ifndef _LOG_H
#define _LOG_H

#include <string>
using namespace std;

class Log
{
public:
    Log();
    virtual ~Log();

    virtual void Add( const string& value );

};

#endif // _LOG_H