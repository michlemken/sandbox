//
// HttpServletResponse.cpp
//
//
// Library: Servlet
// Package: Http
// Module:  HttpServletResponse
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
// The code in this file is derived from Apache Tomcat project
// Copyright (c) 2004 The Apache Software Foundation.
// http://www.apache.org/licenses/LICENSE-2.0
// 


#include "Poco/Servlet/HttpServletResponse.h"


namespace Poco {
namespace Servlet {


const int HttpServletResponse::SC_CONTINUE                        = 100;
const int HttpServletResponse::SC_SWITCHING_PROTOCOLS             = 101;
const int HttpServletResponse::SC_OK                              = 200;
const int HttpServletResponse::SC_CREATED                         = 201;
const int HttpServletResponse::SC_ACCEPTED                        = 202;
const int HttpServletResponse::SC_NON_AUTHORITATIVE_INFORMATION   = 203;
const int HttpServletResponse::SC_NO_CONTENT                      = 204;
const int HttpServletResponse::SC_RESET_CONTENT                   = 205;
const int HttpServletResponse::SC_PARTIAL_CONTENT                 = 206;
const int HttpServletResponse::SC_MULTIPLE_CHOICES                = 300;
const int HttpServletResponse::SC_MOVED_PERMANENTLY               = 301;
const int HttpServletResponse::SC_MOVED_TEMPORARILY               = 302;
const int HttpServletResponse::SC_FOUND                           = 302;
const int HttpServletResponse::SC_SEE_OTHER                       = 303;
const int HttpServletResponse::SC_NOT_MODIFIED                    = 304;
const int HttpServletResponse::SC_USE_PROXY                       = 305;
const int HttpServletResponse::SC_TEMPORARY_REDIRECT              = 307;
const int HttpServletResponse::SC_BAD_REQUEST                     = 400;
const int HttpServletResponse::SC_UNAUTHORIZED                    = 401;
const int HttpServletResponse::SC_PAYMENT_REQUIRED                = 402;
const int HttpServletResponse::SC_FORBIDDEN                       = 403;
const int HttpServletResponse::SC_NOT_FOUND                       = 404;
const int HttpServletResponse::SC_METHOD_NOT_ALLOWED              = 405;
const int HttpServletResponse::SC_NOT_ACCEPTABLE                  = 406;
const int HttpServletResponse::SC_PROXY_AUTHENTICATION_REQUIRED   = 407;
const int HttpServletResponse::SC_REQUEST_TIMEOUT                 = 408;
const int HttpServletResponse::SC_CONFLICT                        = 409;
const int HttpServletResponse::SC_GONE                            = 410;
const int HttpServletResponse::SC_LENGTH_REQUIRED                 = 411;
const int HttpServletResponse::SC_PRECONDITION_FAILED             = 412;
const int HttpServletResponse::SC_REQUEST_ENTITY_TOO_LARGE        = 413;
const int HttpServletResponse::SC_REQUEST_URI_TOO_LONG            = 414;
const int HttpServletResponse::SC_UNSUPPORTED_MEDIA_TYPE          = 415;
const int HttpServletResponse::SC_REQUESTED_RANGE_NOT_SATISFIABLE = 416;
const int HttpServletResponse::SC_EXPECTATION_FAILED              = 417;
const int HttpServletResponse::SC_INTERNAL_SERVER_ERROR           = 500;
const int HttpServletResponse::SC_NOT_IMPLEMENTED                 = 501;
const int HttpServletResponse::SC_BAD_GATEWAY                     = 502;
const int HttpServletResponse::SC_SERVICE_UNAVAILABLE             = 503;
const int HttpServletResponse::SC_GATEWAY_TIMEOUT                 = 504;
const int HttpServletResponse::SC_HTTP_VERSION_NOT_SUPPORTED      = 505;


HttpServletResponse::HttpServletResponse()
{
}


HttpServletResponse::~HttpServletResponse()
{
}


std::string HttpServletResponse::encodeUrl(const std::string& url)
{ 
	return encodeURL(url); 
}


std::string HttpServletResponse::encodeRedirectUrl(const std::string& url)
{ 
	return encodeRedirectURL(url); 
}


} } // namespace Poco::Servlet