//
// JSONConfiguration.h
//
// $Id: //poco/Main/Web/include/Poco/Web/JSONConfiguration.h#2 $
//
// Library: Web
// Package: Configuration
// Module:  JSONConfiguration
//
// Definition of the JSONConfiguration class.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
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


#ifndef Web_JSONConfiguration_INCLUDED
#define Web_JSONConfiguration_INCLUDED


#include "Poco/Web/Web.h"
#include "Poco/Web/JSONHandler.h"


namespace Poco {
namespace Web {


class Web_API JSONConfiguration
{
public:
	typedef void* ContextPtr;
	static const int UNLIMITED_DEPTH = -1;

	JSONConfiguration(int depth = UNLIMITED_DEPTH,
		bool allowComments = true);
		/// Creates JSONConfiguration.

	~JSONConfiguration();
		/// Destroys JSONConfiguration.

	JSONConfiguration(const JSONConfiguration&);
		/// Copy constructor.

	JSONConfiguration& operator = (const JSONConfiguration&);
		/// Assignment operator.

	int parseDepth() const;
		/// Returns the value of the parse depth parameter.

	bool allowComments() const;
		/// Returns the value of the allow comments parameter.

private:
	void swap(JSONConfiguration& other);

    int      _depth;
    int      _allowComments;
};


//
// inlines
//

inline int JSONConfiguration::parseDepth() const
{
	return _depth;
}


inline bool JSONConfiguration::allowComments() const
{
	return 0 != _allowComments;
}


} } // namespace Poco::Web


#endif // Web_JSONConfiguration_INCLUDED
