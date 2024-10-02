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
    virtual void Error( const string& value );
    virtual void Progress( const int position );

};

#endif // _LOG_H