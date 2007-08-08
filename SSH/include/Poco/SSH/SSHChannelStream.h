//
// SSHChannelStream.h
//
// $Id: //poco/Main/SSH/include/Poco/SSH/SSHChannelStream.h#4 $
//
// Library: SSH
// Package: SSHCore
// Module:  SSHChannelStream
//
// Definition of the SSHChannelStream class.
//
// Copyright (c) 2007, Applied Informatics Software Engineering GmbH.
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


#ifndef SSH_SSHChannelStream_INCLUDED
#define SSH_SSHChannelStream_INCLUDED


#include "Poco/SSH/SSH.h"
#include "Poco/SSH/SSHChannel.h"
#include "Poco/BufferedStreamBuf.h"
#include <istream>
#include <ostream>
#include <iostream>


namespace Poco {
namespace SSH {


class SSH_API SSHChannelStreamBuf: public Poco::BufferedStreamBuf
	/// SSHChannelStreamBuf...
{
public:
	enum StreamType
	{
		ST_STD,
		ST_ERR
	};

	SSHChannelStreamBuf(const SSHChannel& channel, StreamType type, Poco::BufferedStreamBuf::openmode mode);
		/// Creates the SSHChannelStreamBuf.

	virtual ~SSHChannelStreamBuf();
		/// Destroys the SSHChannelStreamBuf.

	void close();
		/// Flushes the output stream

	int sync();
		/// Flushes the output stream

private:
	bool data();
		/// Returns true if data is waiting to be read. NOT working in libssh2-0.15!
	
protected:
	int readFromDevice(char* buffer, std::streamsize length);

	int writeToDevice(const char* buffer, std::streamsize length);

private:
	enum
	{
		STREAM_BUFFER_SIZE = 1024
	};

	SSHChannel _channel;
	StreamType _type;
};


class SSH_API SSHChannelStreamIOS: public virtual std::ios
	/// The base class for SSHChannelInputStream and SSHChannelOutputStream.
	///
	/// This class is needed to ensure the correct initialization
	/// order of the stream buffer and base classes.
{
public:
	typedef SSHChannelStreamBuf::StreamType StreamType;

	SSHChannelStreamIOS(const SSHChannel& channel, StreamType type, std::ios_base::openmode mode);
		/// Creates the SSHChannelStreamIOS.

	virtual ~SSHChannelStreamIOS();
		/// Destroys the SSHChannelStreamIOS.

	SSHChannelStreamBuf* rdbuf();
		/// Returns a pointer to the underlying streambuf.

protected:
	SSHChannelStreamBuf _buf;
};


class SSH_API SSHChannelInputStream: public SSHChannelStreamIOS, public std::istream
	/// Input stream for reading from a SSHChannel
{
public:
	SSHChannelInputStream(const SSHChannel& channel, StreamType type = SSHChannelStreamBuf::ST_STD);
		/// Creates the SSHChannelInputStream.

	virtual ~SSHChannelInputStream();
		/// Destroys the SSHChannelInputStream.
};


class SSH_API SSHChannelOutputStream: public SSHChannelStreamIOS, public std::ostream
	/// Output stream for writing to a SSHChannel
{
public:
	SSHChannelOutputStream(const SSHChannel& channel, StreamType type = SSHChannelStreamBuf::ST_STD);
		/// Creates the SSHChannelOutputStream.

	virtual ~SSHChannelOutputStream();
		/// Destroys the SSHChannelOutputStream.

	void close();
		/// Flushes the output stream
};


class SSH_API SSHChannelStream: public SSHChannelStreamIOS, public std::iostream
	/// SSHChannelStream reads and writes from/to an SSHChannel
{
public:
	SSHChannelStream(const SSHChannel& channel, StreamType type = SSHChannelStreamBuf::ST_STD);
		/// Creates the SSHChannelStream.

	virtual ~SSHChannelStream();
		/// Destroys the SSHChannelStream.

	void close();
		/// Flushes the output stream
};


} } // namespace Poco::SSH


#endif // SSH_SSHChannelStream_INCLUDED
