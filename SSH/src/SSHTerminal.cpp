//
// SSHTerminal.cpp
//
// $Id: //poco/Main/SSH/src/SSHTerminal.cpp#4 $
//
// Library: SSH
// Package: SSHTerminal
// Module:  SSHTerminal
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


#include "Poco/SSH/SSHTerminal.h"
#include "Poco/SSH/SSHSession.h"
#include "Poco/SSH/SSHSessionImpl.h"
#include "Poco/SSH/SSHException.h"


namespace Poco {
namespace SSH {


SSHTerminal::SSHTerminal():
	_pSession(0),
	_channel()
{
}


SSHTerminal::SSHTerminal(SSHSession& session, const std::string& terminalEmulation, const SSHTerminal::EnvVariables& environment):
	_pSession(0),
	_channel()
{
	init(session, terminalEmulation, environment);
}


SSHTerminal::~SSHTerminal()
{
	close();
}


void SSHTerminal::init(SSHSession& session, const std::string& terminalEmulation, const SSHTerminal::EnvVariables& environment)
{
	close();
	_channel.init(session);

	EnvVariables::const_iterator it = environment.begin();
	EnvVariables::const_iterator itEnd = environment.end();
	for (; it != itEnd; ++it)
	{
		setEnvironmentVariable(it->first, it->second);
	}

	// request a terminal: 2nd param is the emulation
	char* pTermEmul = const_cast<char*>(terminalEmulation.c_str());
	if (libssh2_channel_request_pty(_channel.channel(), pTermEmul))
	{
		close();
		throw SSHException("Failed to acquire pty for SSHTerminal");
	}

	_pSession = &session;
	_pSession->duplicate();
}


void SSHTerminal::close()
{
	_channel.close();

	if (_pSession)
		_pSession->release();
	_pSession = 0;
}


void SSHTerminal::setEnvironmentVariable(const std::string& key, const std::string& value)
{
	if (!_channel.channel())
		throw SSHException("SSHTerminal not initialized");
	libssh2_channel_setenv(_channel.channel(), const_cast<char*>(key.c_str()), const_cast<char*>(value.c_str()));
}


SSHChannel SSHTerminal::openShell()
{
	// open shell
	if (!_channel.channel())
		throw SSHException("SSHTerminal not initialized");

	if (libssh2_channel_shell(_channel.channel()))
	{
		throw SSHException("Failed to open shell for SSHTerminal");
	}

	return _channel;
}


SSHChannel SSHTerminal::launch(const std::string& command)
{
	if (!_channel.channel())
		throw SSHException("SSHTerminal not initialized");
	if (libssh2_channel_exec(_channel.channel(), command.c_str()))
	{
		throw SSHException("Failed to execute command for SSHTerminal");
	}
	return _channel;
}


void SSHTerminal::setBlocking(bool val)
{
	if (!_channel.channel())
		throw SSHException("SSHTerminal not initialized");
	libssh2_channel_set_blocking(_channel.channel(), (val?1:0));
}


} } // namespace Poco::SSH