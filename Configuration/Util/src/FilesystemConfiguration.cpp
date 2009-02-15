//
// FilesystemConfiguration.cpp
//
// $Id: //poco/svn/Util/src/FilesystemConfiguration.cpp#1 $
//
// Library: Util
// Package: Configuration
// Module:  FilesystemConfiguration
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Util/FilesystemConfiguration.h"
#include "Poco/File.h"
#include "Poco/Path.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Exception.h"
#include "Poco/FileStream.h"


using Poco::Path;
using Poco::File;
using Poco::DirectoryIterator;
using Poco::StringTokenizer;
using Poco::NotFoundException;


namespace Poco {
namespace Util {


FilesystemConfiguration::FilesystemConfiguration(const std::string& path):
	_path(path)
{
	_path.makeDirectory();
}


FilesystemConfiguration::~FilesystemConfiguration()
{
}


void FilesystemConfiguration::clear()
{
	File regDir(_path);
	regDir.remove(true);
}


bool FilesystemConfiguration::getRaw(const std::string& key, std::string& value) const
{
	Path p(keyToPath(key));
	p.setFileName("data");
	File f(p);
	if (f.exists())
	{
		value.reserve((std::string::size_type) f.getSize());
		Poco::FileInputStream istr(p.toString());
		int c = istr.get();
		while (c != std::char_traits<char>::eof())
		{
			value += (char) c;
			c = istr.get();
		}
		return true;
	}
	else return false;
}


void FilesystemConfiguration::setRaw(const std::string& key, const std::string& value)
{
	Path p(keyToPath(key));
	File dir(p);
	dir.createDirectories();
	p.setFileName("data");
	Poco::FileOutputStream ostr(p.toString());
	ostr.write(value.data(), (std::streamsize) value.length());
}


void FilesystemConfiguration::enumerate(const std::string& key, Keys& range) const
{
	DirectoryIterator it(keyToPath(key));
	DirectoryIterator end;
	while (it != end)
	{
		 if (it->isDirectory())
			range.push_back(it.name());
		++it;
	}
}


bool FilesystemConfiguration::removePropertyImpl(const std::string& key)
{
	Path p(keyToPath(key));
	p.setFileName("data");
	File f(p);
	if (f.exists())
	{
		f.remove();
		return true;
	}
	else
		return false;
}


Path FilesystemConfiguration::keyToPath(const std::string& key) const
{
	Path result(_path);
	StringTokenizer tokenizer(key, ".", StringTokenizer::TOK_IGNORE_EMPTY | StringTokenizer::TOK_TRIM);
	for (StringTokenizer::Iterator it = tokenizer.begin(); it != tokenizer.end(); ++it)
	{
		result.pushDirectory(*it);
	}
	return result;
}


} } // namespace Poco::Util
