//
// SSHTest.cpp
//
// $Id: //poco/Main/SSH/testsuite/src/SSHTest.cpp#2 $
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


#include "SSHTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/AutoPtr.h"
#include "Poco/SSH/SSHChannel.h"
#include "Poco/SSH/SSHChannelStream.h"
#include "Poco/SSH/SSHSession.h"
#include "Poco/SSH/SSHTerminal.h"
#include "Poco/SSH/DummyValidator.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/SSH/SCPStream.h"
#include "Poco/File.h"
#include "Poco/Path.h"
#include "Poco/StreamCopier.h"
#include <fstream>
#include <sstream>


using namespace Poco::SSH;

#define SERVER "cis"
#define USER "test"
#define PWD "test"


SSHTest::SSHTest(const std::string& name): CppUnit::TestCase(name)
{
}


SSHTest::~SSHTest()
{
}


void SSHTest::testSessionCreate()
{
	DummyValidator val;
	Poco::Net::SocketAddress addr(SERVER, 22);
	Poco::AutoPtr<SSHSession> ptrSes = new SSHSession(val, addr, USER, PWD);
}


void SSHTest::testSCPWriteStream()
{
	DummyValidator val;
	Poco::Net::SocketAddress addr(SERVER, 22);
	Poco::AutoPtr<SSHSession> ptrSes = new SSHSession(val, addr, USER, PWD);
	std::string data("testdata");
	
	Poco::Path remFile("/tmp/dummy.file");
	SCPOutputStream out(*ptrSes, data.size(), remFile);
	std::istringstream istr(data);
	
	Poco::StreamCopier::copyStream(istr, out);
	out << std::flush;
	SCPInputStream in(*ptrSes, remFile);
	std::ostringstream ostr;
	
	Poco::StreamCopier::copyStream(in, ostr);
	std::string result = ostr.str();
	poco_assert (result == data);
}


void SSHTest::testSCPRead()
{
	// make sure that the file is written
	testSCPWriteStream();

	DummyValidator val;
	Poco::Net::SocketAddress addr(SERVER, 22);
	Poco::AutoPtr<SSHSession> ptrSes = new SSHSession(val, addr, USER, PWD);
	Poco::Path remFile("/tmp/dummy.file");
	SCPInputStream in(*ptrSes, remFile);
	std::ostringstream out;
	
	Poco::StreamCopier::copyStream(in, out);
	std::string result = out.str();
	poco_assert (result == "testdata");
}


void SSHTest::testShell()
{
	DummyValidator val;
	Poco::Net::SocketAddress addr(SERVER, 22);
	Poco::AutoPtr<SSHSession> ptrSes = new SSHSession(val, addr, USER, PWD);
	SSHTerminal term(*ptrSes);
	SSHChannel channel = term.openShell();
	SSHChannelOutputStream out(channel);
	SSHChannelInputStream in(channel);
	std::string txt;

	out << "ls -al\n";
	out << std::flush;
	
	std::getline(in, txt);
	poco_assert (!txt.empty());
}


void SSHTest::testChannel()
{
	DummyValidator val;
	Poco::Net::SocketAddress addr(SERVER, 22);
	Poco::AutoPtr<SSHSession> ptrSes = new SSHSession(val, addr, USER, PWD);
	{
		SSHChannel channel(*ptrSes);
	}
}


void SSHTest::setUp()
{
}


void SSHTest::tearDown()
{
}


CppUnit::Test* SSHTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("SSHTest");

	CppUnit_addTest(pSuite, SSHTest, testSessionCreate);
	CppUnit_addTest(pSuite, SSHTest, testSCPWriteStream);
	CppUnit_addTest(pSuite, SSHTest, testSCPRead);
	CppUnit_addTest(pSuite, SSHTest, testShell);
	CppUnit_addTest(pSuite, SSHTest, testChannel);

	return pSuite;
}
