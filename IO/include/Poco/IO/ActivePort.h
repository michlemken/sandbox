//
// ActivePort.h
//
// $Id: //poco/Main/template/class.h#8 $
//
// Library: IO
// Package: IO
// Module:  ActivePort
//
// Definition of the ActivePort class.
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


#ifndef IO_ActivePort_INCLUDED
#define IO_ActivePort_INCLUDED


#include "Poco/IO/IO.h"
#include "Poco/ActiveMethod.h"
#include "Poco/Void.h"


namespace Poco {
namespace IO {


template <class P>
class ActivePort
{
public:
	ActivePort(P& port):
		read(this, &ActivePort::readImp),
		write(this, &ActivePort::writeImp),
		_port(port)
	{
	}
	
	~ActivePort()
	{
	}
	
	Poco::ActiveMethod<std::string, Void, ActivePort> read;
	Poco::ActiveMethod<int, std::string, ActivePort> write;

protected:
	std::string readImp(const Void& v)
	{	
		std::string buffer;
		return _port.read(buffer);
	}
	
	int writeImp(const std::string& data)
	{	
		return _port.write(data);
	}

private:
	P& _port;
};


} } // namespace Poco::IO


#endif // IO_ActivePort_INCLUDED
