//
// SSHSession.h
//
// $Id: //poco/Main/SSH/include/Poco/SSH/SSHSession.h#1 $
//
// Library: SSH
// Package: SSHCore
// Module:  SSHSession
//
// Definition of the SSHSession class.
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


#ifndef SSH_SSHSession_INCLUDED
#define SSH_SSHSession_INCLUDED


#include "Poco/SSH/SSH.h"
#include "Poco/RefCountedObject.h"


namespace Poco {
namespace Net {
	class SocketAddress;
	class StreamSocket;
}
}


namespace Poco {
namespace SSH {


class SSHHostValidator;
class SSHSessionImpl;


class SSH_API SSHSession: public Poco::RefCountedObject
	/// An SSHSession connects to the remote peer and validates the peer. Used by all other operations.
	/// Always store a SSHSession inside a Poco::AutoPtr.
{
public:
	enum
	{
		DEFAULT_PORT = 22
	};

	SSHSession(SSHHostValidator& validator, const std::string& server, const std::string& user, const std::string& pwd);
		/// Creates the SSHSession to the server and the default ssh port.

	SSHSession(SSHHostValidator& validator, const std::string& server, Poco::UInt16 port, const std::string& user, const std::string& pwd);
		/// Creates the SSHSession to the server and the given port.

	SSHSession(SSHHostValidator& validator, const Poco::Net::SocketAddress& addr, const std::string& user, const std::string& pwd);
		/// Creates the SSHSession.

	SSHSession(SSHHostValidator& validator, const Poco::Net::StreamSocket& sock, const std::string& user, const std::string& pwd);
		/// Creates the SSHSession.

	SSHSessionImpl* impl() const;
		/// Returns the impl object

protected:
	virtual ~SSHSession();
		/// Destroys the SSHSession.

private:
	SSHSession();
	SSHSession(const SSHSession&);
	SSHSession& operator=(const SSHSession&);

	SSHSessionImpl* _pImpl;
};


inline SSHSessionImpl* SSHSession::impl() const
{
	return _pImpl;
}


} } // namespace Poco::SSH


#endif // SSH_SSHSession_INCLUDED
