
#ifndef READ_DELIMITED_FILE_H
#define READ_DELIMITED_FILE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "TransformPoly.h"
using namespace std;

class DelimitedFileValue
{
public:
	string data;

	DelimitedFileValue();
	DelimitedFileValue(const DelimitedFileValue &a);
	virtual ~DelimitedFileValue();
	class DelimitedFileValue &operator=(const DelimitedFileValue &a);

	double GetVald();
	int GetVali();
	const char *GetVals();
	void SetVald(double in);
	void SetVals(const char *in);

	void ProcessVal(const char *buffer, int size);
};

class DelimitedFileLine
{
public:
	vector<class DelimitedFileValue> vals;

	DelimitedFileLine();
	DelimitedFileLine(const class DelimitedFile &a);
	virtual ~DelimitedFileLine();
	class DelimitedFileLine &operator=(const class DelimitedFileLine &a);

	unsigned int NumVals();
	class DelimitedFileValue &GetVal(int num);
	class DelimitedFileValue &operator[](int num);

	void ProcessLine(const char *line, string &delimiter);
};

class DelimitedFile
{
protected:
	vector<class DelimitedFileLine> lines;
	string delimiter;
public:
	DelimitedFile();
	DelimitedFile(const class DelimitedFile &a);
	virtual ~DelimitedFile();
	class DelimitedFile &operator=(const class DelimitedFile &a);

	int Open(const char *filename);
	unsigned int NumLines();
	class DelimitedFileLine &GetLine(int num);
	class DelimitedFileLine &operator[](int num);
};

class TextFile
{
public:
	TextFile();
	TextFile(const class TextFile &a);
	virtual ~TextFile();
	class TextFile &operator=(const class TextFile &a);
	int Open(const char *filename);
	void Close();
	virtual int GetLine(string &line);

protected:
	ifstream file;
};

//**********************************************************************

class DelimitedFileStreamCallback
{
public:
	DelimitedFileStreamCallback();
	virtual ~DelimitedFileStreamCallback();
	virtual void Val(int col, const char *buffer, int len);
	virtual void EndOfLine() {};
};

class DelimitedFileStream
{
public:
	string delimiter;
	ifstream file;

	DelimitedFileStream();
	DelimitedFileStream(const class DelimitedFile &a);
	virtual ~DelimitedFileStream();
	class DelimitedFileStream &operator=(const class DelimitedFileStream &a);

	int Open(const char *filename);
	void ToStart();
	int GetLine(class DelimitedFileStreamCallback &callback);
	int GetAllLines(class DelimitedFileStreamCallback &callback);
	void ProcessLine(const char *line,
					 class DelimitedFileStreamCallback &callback);
};

//*********************************************************************

int SeekInDelimitedFile(class DelimitedFile &file, int firstColValue, class DelimitedFileLine &lineOut);

#endif // READ_DELIMITED_FILE_H
