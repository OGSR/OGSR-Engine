////////////////////////////////////////////////////////////////////////////////
// Generates forwarding headers using the information
// in the header.lst & vendor.lst files
//
// To create a header list from an existing directory of headers
// dir *.h /b > headers.lst
//
// Copyright Shannon Barber 2002.
//
// Permission to use, copy, modify, distribute and sell this software for any 
// purpose is hereby granted without fee, provided that the above copyright 
// notice appear in all copies and that both that copyright notice and this 
// permission notice appear in supporting documentation. It is provided "as is" 
// without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>

using namespace std;

struct Compiler
	{
	Compiler() {}
	Compiler(const string& VersionTest, const string& SubDir) : 
	 version_test(VersionTest), subdir(SubDir)
		{}
	string version_test;
	string subdir;
	};
	
int main(int argc, char* argv[])
	{
	if(argc != 3)
		{
		cout <<"Usage: <Header List> <Version/Vendor List>"<<endl;
		return -1;
		}

	typedef vector<Compiler> cv_type;
	
	cv_type vendors;
	char buf[1024];
	string check;
	string subdir;
	fstream vendor_file;

	vendor_file.open(argv[2], ios::in);
	if(!vendor_file.is_open())
		{
		cout << "Unable to open vendor file: " << argv[2] << endl;
		return -2;
		}
	else
		{
		vendors.reserve(10);
		while(!vendor_file.eof())
			{
			vendor_file.getline(buf, 1024, '\n');
			check = buf;
			vendor_file.getline(buf, 1024, '\n');
			subdir = buf;
			vendor_file.getline(buf, 1024, '\n');
			if(!(check.empty() || subdir.empty()))
				vendors.push_back(Compiler(check, subdir));
			else
				{
				cout << "Error parsing vendors, check:" << check << "\tsubdir:" << subdir << endl;
				}
			}
		}
	
	fstream header_list(argv[1]);
	string header;
	
	if(!header_list.is_open())
		{
		cout << "Invalid header list file, " << argv[1] << endl;
		return -3;
		}
	while(!header_list.eof())
		{
		header_list >> header;
		cout << header << endl;
		fstream header_file(header.c_str(), ios::out);
		if(!header_file.is_open())
			{
			cout << "Unable to open header file for output: " << header << endl;
			}
		else
			{
			string start(string("// Generated header: ")+header+'\n');
			string line(start.size(),'/');

			header_file << line << '\n';
			header_file << start;
			header_file << "// Forwards to the appropriate code\n";
			header_file << "// that works on the detected compiler\n";
			time_t rawtime;
			time(&rawtime);
			header_file << "// Generated on " << ctime(&rawtime);
			header_file << line << "\n\n";
						
			cv_type::iterator it=vendors.begin(), itEnd = vendors.end();
			
			header_file << "#ifdef LOKI_USE_REFERENCE\n";
			header_file << "#\tinclude \"Reference/" << header << "\"\n";
			header_file << "#else\n";

			header_file << "#\tif " << it->version_test << endl;
			header_file << "#\t\tinclude \"" << it->subdir << "/" << header << "\"\n";
			++it;
			for(; it!=itEnd; ++it)
				{
				header_file << "#\telif " << it->version_test << endl;
				header_file << "#\t\tinclude \"" << it->subdir;
				header_file << "/" << header << "\"\n";
				}
			header_file << "#\telse\n";
			header_file << "#\t\tinclude \"Reference/" << header << "\"\n";
			header_file << "#\tendif\n";
			header_file << "#endif\n";
			}
		}
	return 0;
	}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// September 16, 2002: Changed it to only test for compilers where ports exist,
// else use the Reference version. It used to give an error if used on a
// compiler that wasn't tested for. Also removed "./".
// It still needs to test for Intel and Metrowerks, as these define _MSC_VER,
// too. T.S.
////////////////////////////////////////////////////////////////////////////////
