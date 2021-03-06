//
// SerialChannel_WIN32.cpp
//
// $Id: //poco/Main/IO/src/SerialChannel_POSIX.cpp#1 $
//
// Library: IO
// Package: Serial
// Module:  SerialChannel
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
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


#include "Poco/IO/SerialChannel_WIN32.h"
#include "Poco/Exception.h"
#include <windows.h>

using Poco::CreateFileException;
using Poco::IOException;


namespace Poco {
namespace IO {


SerialChannelImpl::SerialChannelImpl(const std::string& name, const SerialConfigImpl& config): 
	_name(name), _config(config)
{
	openImpl();
}


SerialChannelImpl::~SerialChannelImpl()
{
	closeImpl();
}


void SerialChannelImpl::initImpl()
{
	if (!SetCommState(_handle, &(_config.dcb()))) handleError(_name);

	if (!SetCommTimeouts(_handle, &(_config.commTimeouts()))) handleError(_name);

	DWORD bufSize = (DWORD) _config.getBufferSizeImpl();
	SetupComm(_handle, bufSize, bufSize);
}


void SerialChannelImpl::reconfigureImpl(const SerialConfigImpl& config)
{
	_config = config;
	initImpl();
}


void SerialChannelImpl::openImpl()
{
	_handle = CreateFileA(_name.c_str(), GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (INVALID_HANDLE_VALUE == _handle) handleError(_name);

	initImpl();
}


void SerialChannelImpl::closeImpl()
{
	if (!CloseHandle(_handle)) handleError(_name);
}


char SerialChannelImpl::readImpl()
{
	char readBuf = 0;
	readImpl(&readBuf, 1);
	return readBuf;
}


int SerialChannelImpl::readImpl(char* pBuffer, std::size_t length)
{
	if (0 == length) return 0;

	std::string buf;
	readImpl(buf, length);
	std::size_t len = length;
	if (buf.size() < length) len = buf.size();
	strncpy(pBuffer, buf.c_str(), len);
	return (int) len;
}


std::string& SerialChannelImpl::readImpl(std::string& buffer, std::size_t length)
{
	buffer.clear();
	int bufSize = length ? (int) length : (int) _config.getBufferSizeImpl();
	if (0 == bufSize) return buffer;
	char* pReadBuf = new char[bufSize+1];
	DWORD read = 0;
	DWORD readCount = 0;
	buffer.clear();
	do
    {
		ZeroMemory(pReadBuf, bufSize+1);
		if (!ReadFile(_handle, pReadBuf + readCount, bufSize - readCount, &read, NULL)) 
		{
			delete[] pReadBuf;
			handleError(_name);
		}
		else if (0 == read) break;

		poco_assert(read <= bufSize - readCount);
		buffer.append(pReadBuf + readCount, read);

		if (length) readCount += read;
		
		if (_config.getUseEOFImpl()) 
		{
			size_t pos = buffer.find(_config.getEOFCharImpl());
			if (pos != buffer.npos)
			{
				buffer = buffer.substr(0, pos);
				PurgeComm(_handle, PURGE_RXCLEAR);
				break;
			}
		}

		if (length && readCount >= length) break;
	}while(true);

	delete[] pReadBuf;
	return buffer;
}


int SerialChannelImpl::writeImpl(char c)
{
	return writeImpl(&c, 1);
}


int SerialChannelImpl::writeImpl(const char* pBuffer, std::size_t length)
{
	if (0 == length) return 0;

	std::string str;
	str.assign(pBuffer, length);
	
	return writeImpl(str);
}


int SerialChannelImpl::writeImpl(const std::string& data)
{
	if (0 == data.length()) return 0;

	std::string d = data;

	if (_config.getUseEOFImpl()) 
	{
		size_t pos = d.find(_config.getEOFCharImpl());
		if (pos != d.npos) d = d.substr(0, pos+1);
	}

	DWORD written = 0;
	DWORD length = static_cast<DWORD>(d.length());

	if (!WriteFile(_handle, d.data(), length, &written, NULL) || 
		((written != length) && (0 != written)))
		handleError(_name);
	else if (0 == written)
		throw IOException("Error writing to " + _name);

	return written;
}


const std::string& SerialChannelImpl::getNameImpl() const
{
	return _name;
}


std::string& SerialChannelImpl::getErrorText(std::string& buf)
{
    DWORD dwRet;
    LPTSTR pTemp = NULL;

    DWORD errCode = GetLastError();

    dwRet = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
		NULL,
		errCode,
		LANG_USER_DEFAULT,
		(LPTSTR) &pTemp,
		0,
		NULL);

    if (dwRet && pTemp)
    {
		if ((std::string(pTemp).length()-2) >= 0)
		{
			pTemp[std::string(pTemp).length()-2] = TEXT('\0');  //remove cr and newline character
			buf = pTemp;
		}

        LocalFree((HLOCAL) pTemp);
    }
    return buf;
}


void SerialChannelImpl::handleError(const std::string& name)
{
	std::string errorText;
	DWORD error = GetLastError();

	switch (error)
	{
	case ERROR_FILE_NOT_FOUND:
		throw FileNotFoundException(name, getErrorText(errorText));
	case ERROR_ACCESS_DENIED:
		throw FileAccessDeniedException(name, getErrorText(errorText));
	case ERROR_ALREADY_EXISTS:
	case ERROR_FILE_EXISTS:
		throw FileExistsException(name, getErrorText(errorText));
	case ERROR_FILE_READ_ONLY:
		throw FileReadOnlyException(name, getErrorText(errorText));
	case ERROR_CANNOT_MAKE:
	case ERROR_INVALID_NAME:
	case ERROR_FILENAME_EXCED_RANGE:
		throw CreateFileException(name, getErrorText(errorText));
	case ERROR_BROKEN_PIPE:
	case ERROR_INVALID_USER_BUFFER:
	case ERROR_INSUFFICIENT_BUFFER:
		throw IOException(name, getErrorText(errorText));
	case ERROR_NOT_ENOUGH_MEMORY:
		throw OutOfMemoryException(name, getErrorText(errorText));
	case ERROR_HANDLE_EOF: break;
	default:
		throw FileException(name, getErrorText(errorText));
	}
}


} } // namespace Poco::IO
