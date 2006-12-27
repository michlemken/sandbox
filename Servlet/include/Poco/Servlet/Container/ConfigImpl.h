//
// ConfigImpl.h
//
//
// Library: Container
// Package: ContainerCore
// Module:  ConfigImpl
//
// Definition of the ConfigImpl class.
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


#ifndef Container_ConfigImpl_INCLUDED
#define Container_ConfigImpl_INCLUDED


#include "Poco/Servlet/Container/ContainerBase.h"
#include "Poco/Servlet/ServletConfig.h"
#include "Poco/Servlet/Filter.h"
#include "Poco/Servlet/Container/ServletContextImpl.h"
#include <string>
#include <map>


namespace Poco {
namespace Servlet {
namespace Container {


class Container_API ConfigImpl
{
 public:
	 typedef std::map<std::string, std::string> InitParamMap;

	ConfigImpl(const ServletContextImpl& context);
	~ConfigImpl();
	 
	void setInitParameter(const std::string& name, const std::string& value);
	const std::string& getInitParameter(const std::string& name) const;
	const std::string& getInitParameter(const std::string& name, const std::string& defValue) const;
	std::vector<std::string> getInitParameterNames() const;
	 
	const ServletContext& getContext() const;

	void setDisplayName(const std::string& name);
	void setName(const std::string& name);
	void setClass(const std::string& name);
	void setLibrary(const std::string& name);

	const std::string& getDisplayName() const;
	const std::string& getName() const;
	const std::string& getClass() const;
	const std::string& getLibrary() const;

 private:
	ConfigImpl();

	typedef std::map<std::string, std::string> ParameterMap;
	std::string _name;
	std::string _displayName;
	std::string _className;
	std::string _libraryName;
	ParameterMap _parameters;

	const ServletContextImpl& _servletContext;
};

class Container_API ServletConfigImpl : public ConfigImpl, public ServletConfig
{
public:
	ServletConfigImpl(const ServletContextImpl& context):
	ConfigImpl(context)
	{
	}

	const std::string& getInitParameter(const std::string& name) const
	{
		return ConfigImpl::getInitParameter(name);
	}

	std::vector<std::string> getInitParameterNames() const
	{
		return ConfigImpl::getInitParameterNames();
	}

	const ServletContext& getServletContext() const
	{
		return getContext();
	}

	const std::string& getServletName() const
	{
		return getName();
	}

private:
	 ServletConfigImpl();
};

class Container_API FilterConfigImpl : public ConfigImpl, public Poco::Servlet::FilterConfig
{
public:
	FilterConfigImpl(const ServletContextImpl& context):
	ConfigImpl(context)
	{
	}

	const std::string& getInitParameter(const std::string& name) const
	{
		return ConfigImpl::getInitParameter(name);
	}

	std::vector<std::string> getInitParameterNames() const
	{
		return ConfigImpl::getInitParameterNames();
	}

	const ServletContext& getServletContext() const
	{
		return getContext();
	}

	const std::string& getFilterName() const
	{
		return getName();
	}

 private:
	 FilterConfigImpl();
};


} } } // namespace Poco::Servlet::Container


#endif //Container_ConfigImpl_INCLUDED