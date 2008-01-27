//
// FlowPanel.h
//
// $Id: //poco/Main/WebWidgets/include/Poco/WebWidgets/FlowPanel.h#2 $
//
// Library: WebWidgets
// Package: WebGUI
// Module:  FlowPanel
//
// Definition of the FlowPanel class.
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


#ifndef WebWidgets_FlowPanel_INCLUDED
#define WebWidgets_FlowPanel_INCLUDED


#include "Poco/WebWidgets/WebWidgets.h"
#include "Poco/WebWidgets/IndexedPanel.h"
#include "Poco/AutoPtr.h"
#include <vector>


namespace Poco {
namespace WebWidgets {


class WebWidgets_API FlowPanel: public IndexedPanel
	/// A FlowPanel stores widgets next to each other as long the page offers enough place
	///    <flow>
	///         children
	///    </flow>

{
public:
	static const std::string FACTORY_ID;
	typedef std::vector<Poco::AutoPtr<Widget> > Children;

	FlowPanel(const std::vector<Poco::AutoPtr<Widget> >& children = std::vector<Poco::AutoPtr<Widget> >());
		/// Creates the FlowPanel.

	FlowPanel(const Poco::XML::Element* pElem);
		/// Creates the FlowPanel from the XML element.

	virtual ~FlowPanel();
		/// Destroys the FlowPanel.

	void accept(Visitor& v);

	Poco::AutoPtr<Widget> clone() const;
};


} } // namespace Poco::WebWidgets


#endif // WebWidgets_FlowPanel_INCLUDED