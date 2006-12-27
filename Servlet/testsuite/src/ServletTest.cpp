//
// ServletTest.cpp
//
// Library: TestSuite
// Package: Servlets
// Module:  ServletTest
//
// Copyright (c) 2006, Aleksandar Fabijanic and Contributors.
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


#include "ServletTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Servlet/ServletConfig.h"
#include "Poco/Servlet/GenericServlet.h"
#include "Poco/Servlet/HttpServlet.h"
#include "Poco/Servlet/NoBodyResponse.h"
#include "Poco/Servlet/ServletRequest.h"
#include "Poco/Servlet/ServletInputStream.h"
#include "Poco/Servlet/ServletOutputStream.h"
#include "Poco/Servlet/Cookie.h"
#include "Poco/Servlet/ServletException.h"
#include "Poco/Servlet/UnavailableException.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Latin1Encoding.h"
#include "Poco/UTF8Encoding.h"
#include "Poco/Exception.h"
#include <algorithm>
#include <iostream>
#include <sstream>


using Poco::StringTokenizer;
using Poco::Latin1Encoding;
using Poco::UTF8Encoding;
using Poco::InvalidArgumentException;
using Poco::ExistsException;
using Poco::NotFoundException;
using Poco::Servlet::ServletOutputStream;
using Poco::Servlet::PrintWriter;
using Poco::Servlet::BufferedReader;
using Poco::Servlet::Cookie;
using Poco::Servlet::ServletInputStream;
using Poco::Servlet::ServletException;
using Poco::Servlet::UnavailableException;


ServletTest::ServletTest(const std::string& name): CppUnit::TestCase(name)
{
}


ServletTest::~ServletTest()
{
}

void ServletTest::testCookie()
{
	Cookie c("my cookie", "value");

	assert(c.getName() == "my cookie");
	assert(c.getValue() == "value");

	c.setComment("my comment");
	assert(c.getComment() == "my comment");

	c.setDomain("my.domain.com");
	assert(c.getDomain() == "my.domain.com");

	c.setMaxAge(1000);
	assert(c.getMaxAge() == 1000);

	c.setMaxAge(0);
	assert(c.getMaxAge() == 0);

	c.setMaxAge(-1);
	assert(c.getMaxAge() == -1);

	c.setName("your Cookie");
	assert(c.getName() == "your Cookie");

	c.setPath("my/path");
	assert(c.getPath() == "my/path");

	c.setSecure(true);
	assert(c.getSecure());

	c.setSecure(false);
	assert(!c.getSecure());

	c.setValue("new Value");
	assert(c.getValue() == "new Value");

	c.setVersion(1);
	assert(c.getVersion() == 1);
}


void ServletTest::testServletInputStream()
{
	std::string input("1234567890\n0987654321\nabcdefghijklmnopqrstuvwxyz\n");
	StringTokenizer st(input, "\n");
	assert(3 == st.count());
	std::istringstream istr(input);
	ServletInputStream sis(istr);

	int i = 0;
	int p = 0;

	for(;;) 
	{
		char c[100] = "";
		int len = st[i].length()+1;
		sis.readLine(c, p, len);
		assert(st[i] == std::string(c));
		p += len;
		if(++i >= st.count()) break;
	}

	char c[1] = "";
	sis.readLine(c, 1000, 1000);
	assert(c[0] == '\0');
}


void ServletTest::testServletOutputStream()
{
	ServletOutputStream sos(std::cout);
	std::string str("string");

	sos.print("integer:");
	sos.println(1);
	sos << 1 << std::endl;
	float f = 2.3f;
	sos.print("float:");
	sos.println(f);
	sos << f << std::endl;
	double d = 3.4;
	sos.print("double:");
	sos.println(d);
	sos << d << std::endl;

	bool b = false;
	sos.print("bool false:");
	sos.println(b);
	sos << b << std::endl;
	b = true;
	sos.print("bool true:");
	sos.println(b);
	sos << b << std::endl;

	sos.print("string:");
	sos.println(str);
	sos << str << std::endl;

	sos.print("char:");
	sos.println("char");
	sos << "char" << std::endl;

	std::ostringstream os;
	ServletOutputStream sos1(os);
	sos1.print("1234567890");
	sos1.close();
	assert (os.str() == "1234567890");
}


void ServletTest::testBufferedReader()
{
	std::string text("1234567890");
	std::istringstream istr(text);
	ServletInputStream sis(istr);
	BufferedReader br(sis);
	char c[11] = "";
	br.readLine(c, 0, sizeof(c));
	assert(text == std::string(c));

	const unsigned char latin1Chars[] = {'g', 252, 'n', 't', 'e', 'r', 0};
	const unsigned char utf8Chars[]   = {'g', 195, 188, 'n', 't', 'e', 'r', 0};
	std::string latin1Text((const char*) latin1Chars);
	std::string utf8Text((const char*) utf8Chars);
	std::istringstream istr1(latin1Text);
	ServletInputStream sis1(istr1);
	Latin1Encoding latin1Encoding;
	UTF8Encoding utf8Encoding;
	BufferedReader br1(sis1, &latin1Encoding, &utf8Encoding);
	
	char c1[sizeof(utf8Chars)] = "";
	br1.readLine(c1, 0, sizeof(c1));

	assert(utf8Text == std::string(c1));
}


void ServletTest::testPrintWriter()
{
	std::string str("string");
	PrintWriter w(std::cout);

	w.print("integer:");
	w.println(1);
	float f = 2.3f;
	w.print("float:");
	w.println(f);
	double d = 3.4;
	w.print("double:");
	w.println(d);

	bool b = false;
	w.print("bool false:");
	w.println(b);
	b = true;
	w.print("bool true:");
	w.println(b);

	w.print("string:");
	w.println(str);
	w.print("char:");
	w.println("char");

	Latin1Encoding latin1Encoding;
	UTF8Encoding utf8Encoding;
	const unsigned char latin1Chars[] = {'g', 252, 'n', 't', 'e', 'r', 0};
	const unsigned char utf8Chars[]   = {'g', 195, 188, 'n', 't', 'e', 'r', 0};
	std::string latin1Text((const char*) latin1Chars);
	std::string utf8Text((const char*) utf8Chars);
	w.print("latin1: ");
	w.println((const char*) latin1Chars);
	PrintWriter w1(std::cout, &latin1Encoding, &utf8Encoding);
	w1.print("UTF-8: ");
	w1.println((const char*) latin1Chars);

	std::ostringstream os;
	PrintWriter w2(os, &latin1Encoding, &utf8Encoding);
	w2.print((const char*) latin1Chars);
	w2.close();
	assert (os.str() == utf8Text);
}


void ServletTest::testServletException()
{
	try
	{
		throw ServletException("reason");
	}
	catch(ServletException& ex)
	{
		assert(ex.getRootCause() == "reason");
	}
}


void ServletTest::testUnavailableException()
{
	try
	{
		throw UnavailableException("reason");
	}
	catch(UnavailableException& ex)
	{
		assert(ex.getRootCause() == "reason");
		assert(ex.isPermanent());
	}

	try
	{
		throw UnavailableException("reason", 10);
	}
	catch(UnavailableException& ex)
	{
		assert(ex.getRootCause() == "reason");
		assert(!ex.isPermanent());
		assert(ex.getUnavailableSeconds() == 10);
	}
}


void ServletTest::setUp()
{
}


void ServletTest::tearDown()
{
}


CppUnit::Test* ServletTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("ServletTest");

	CppUnit_addTest(pSuite, ServletTest, testCookie);
	CppUnit_addTest(pSuite, ServletTest, testServletInputStream);
	CppUnit_addTest(pSuite, ServletTest, testServletOutputStream);
	CppUnit_addTest(pSuite, ServletTest, testBufferedReader);
	CppUnit_addTest(pSuite, ServletTest, testPrintWriter);
	CppUnit_addTest(pSuite, ServletTest, testServletException);
	CppUnit_addTest(pSuite, ServletTest, testUnavailableException);

	return pSuite;
}