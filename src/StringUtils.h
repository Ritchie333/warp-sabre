
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <sstream>
#include <vector>
using namespace std;

string PackBeforeString(string baseString, string pack, int minLength);
string IntToString(int input);
string RemoveTrailingSpaces(string str);
string RemoveLeadingSpaces(string str);
string RemoveTrailingWhitespace(string str);
string RemoveLeadingWhitespace(string str);
string RemoveWhitespace(string str);
string FilenameRemoveFrameNumAndExtension(string filename);
string FilenameChangeNumber(string filename, int num);
string GetFileExtension(string filename);
string RemoveFileExtension(const char *filename);
string RemoveFilePath(const char *filename);
string GetFilePath(const char *filename);
int GetFileAsString(const char *filename, string &dataOut);
vector<string> SplitString(string in, string token);

string ReplaceAllInstances(string input, string findStr, string replaceString);
string EscapeCommas(string input);
string UnescapeCommas(string input);

int dirExists(const char *path);
int fileExists(const char *path);

template <class T>
string ToString(T input)
{
	ostringstream out;
	out << input;
	return out.str();
}

const string version( const char* pname );

#define XSTR(x)	STR(x)
#define STR(x) #x

#define DEFINE_VERSION(po)	\
	if( po.HasArg( "version" )) { cout << version( argv[ 0 ] ) << endl;	exit(0); }

#define DEFINE_HELP(po, desc) \
	if (po.HasArg("help")) { cout << desc.str() << endl; exit(0); }

#endif // STRING_UTILS_H
