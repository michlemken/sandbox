//
// SSHSessionImpl.h
//
// $Id: //poco/Main/SSH/include/Poco/SSH/SSHSessionImpl.h#2 $
//
// Library: SSH
// Package: SSHCore
// Module:  SSHSessionImpl
//
// Definition of the SSHSessionImpl class.
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


#ifndef SSH_SSHSessionImpl_INCLUDED
#define SSH_SSHSessionImpl_INCLUDED


#include "Poco/SSH/SSH.h"
#include "Poco/Net/StreamSocket.h"
#include "libssh2.h"
#include "libssh2_config.h"


namespace Poco {
namespace Net {
	class SocketAddress;
}
}


namespace Poco {
namespace SSH {


class SSHHostValidator;


class SSH_API SSHSessionImpl
	/// Implementation class used by SSHSession. Never use this class directly!
{
public:
	SSHSessionImpl(SSHHostValidator& validator, const Poco::Net::SocketAddress& addr, const std::string& user, const std::string& pwd);
		/// Creates the SSHSessionImpl.

	SSHSessionImpl(SSHHostValidator& validator, const Poco::Net::StreamSocket& sock, const std::string& user, const std::string& pwd);
		/// Creates the SSHSessionImpl.

	virtual ~SSHSessionImpl();
		/// Destroys the SSHSessionImpl.

	LIBSSH2_SESSION* session() const;
		/// Returns the session pointer

	const Poco::Net::StreamSocket& socket() const;
		/// Returns the underlying StreamSocket

private:
	void setBlocking(bool val);
		/// Sets the session to blocking/unblocking

	bool isBlocking() const;
		/// Checks if the session is blocking or not

	void establish(SSHHostValidator& validator, const std::string& user, const std::string& pwd);
		/// connects to the host and establishes the SSH session

	void close();
		/// Release resources, closes the session

private:
	SSHSessionImpl();
	SSHSessionImpl(const SSHSessionImpl&);
	SSHSessionImpl& operator=(const SSHSessionImpl&);

private:
	Poco::Net::StreamSocket _socket;
	LIBSSH2_SESSION*        _pSession;
};


inline LIBSSH2_SESSION* SSHSessionImpl::session() const
{
	return _pSession;
}


inline const Poco::Net::StreamSocket& SSHSessionImpl::socket() const
{
	return _socket;
}


} } // namespace Poco::SSH


#endif // SSH_SSHSessionImpl_INCLUDED
