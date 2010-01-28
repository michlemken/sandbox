//
// LuaTest.cpp
//
// $Id: //poco/Main/Script/Lua/testsuite/src/LuaTest.cpp#1 $
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


#include "LuaTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Script/Lua/Environment.h"
#include "Poco/Script/Lua/lua/lua.h"
#include "Poco/Script/Lua/lua/lauxlib.h"
#include "Poco/Script/Lua/lua/lualib.h"
#include "Poco/Script/EnvironmentStream.h"
#include "Poco/Script/ScriptException.h"
#include "Poco/DynamicAny.h"
#include <iostream>


using Poco::Script::Lua::Environment;
using Poco::Script::EnvironmentStream;
using Poco::Script::ScriptException;
using Poco::DynamicAny;


LuaTest::LuaTest(const std::string& name): 
	CppUnit::TestCase(name)
{
}


LuaTest::~LuaTest()
{
}


void LuaTest::testExecute()
{
	Environment e;
	e.execute("print 'hello world'");
	e.reset();

  std::vector<Poco::DynamicAny> arguments;
	arguments.push_back(1);
	arguments.push_back(2);
	
	std::string code = "return function(a, b) "
		"c = a + b;"
		"print('Lua says: ' .. a .. ' + ' .. b .. ' = ' .. c);"
		"return c;"
		"end";

  std::vector<Poco::DynamicAny> results;
	e.execute(code, &arguments, &results);
	assert(1 == results.size());
	
	std::cout << "C++ says: Lua said " 
		<< arguments[0].convert<int>() << " + " 
		<< arguments[1].convert<int>() << " = " 
		<< results[0].convert<double>() << std::endl;
	e.reset();

	try
	{
		e.execute("a piece of bad code");
		fail("must fail");
	}
	catch (ScriptException& ex)
	{ 
		std::cout << "ScriptException caught: "
			<< ex.displayText() << std::endl; 
	}
}


void LuaTest::testPersist()
{
	fail ("not implemented");
}


void LuaTest::setUp()
{
}


void LuaTest::tearDown()
{
}


CppUnit::Test* LuaTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("LuaTest");

	CppUnit_addTest(pSuite, LuaTest, testExecute);
	CppUnit_addTest(pSuite, LuaTest, testPersist);

	return pSuite;
}
