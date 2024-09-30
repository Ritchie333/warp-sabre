#include "Log.h"

#include <iostream>

Log::Log()
{

}
    
Log::~Log()
{

}

void Log::Add( const string& value )
{
    cout << value << endl;
}

void Log::Progress( const int /*position*/ )
{

}