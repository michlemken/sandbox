//
// PocoServer.cpp
//
// Library: PocoServer
// Package: PocoServerCore
// Module:  PocoServer
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


#include "Poco/Servlet/PocoServer/PocoServer.h"
#include "Poco/Servlet/PocoServer/PocoServerManifest.h"
#include "Poco/Servlet/PocoServer/PocoHttpServletRequest.h"
#include "Poco/Servlet/PocoServer/PocoHttpServletResponse.h"
#include "Poco/Servlet/Ex/HttpServerConfig.h"
#include "Poco/Servlet/Ex/SessionManager.h"
#include "Poco/String.h"
#include "Poco/Thread.h"
#include "Poco/Mutex.h"
#include "Poco/Path.h"
#include "Poco/Logger.h"
#include "Poco/File.h"
#include "Poco/LocalDateTime.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeParser.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include "Poco/Exception.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/IPAddress.h"
#include "Poco/Net/NetworkInterface.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/NetException.h"
#include "Poco/Util/Application.h"
#include <sstream>


using Poco::Net::SocketAddress;
using Poco::Net::IPAddress;
using Poco::Net::ServerSocket;
using Poco::Net::NetworkInterface;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPClientSession;
using Poco::Net::HTMLForm;
using Poco::Net::NameValueCollection;
using Poco::Net::NetException;
using Poco::Util::Application;
using Poco::Servlet::Ex::HttpServletDispatcher;
using Poco::Servlet::Ex::HttpServerConfig;
using Poco::Servlet::Ex::SessionManager;


namespace Poco {
namespace Servlet {
namespace PocoServer {


class RedirectRequestHandler: public HTTPRequestHandler
{
public:

	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
		HTMLForm form;
		Application& app = Application::instance();
		app.logger().information("Redirect request from " + request.clientAddress().toString());
		form.load(request);
		NameValueCollection::ConstIterator it = form.find("url");
		if (form.end() != it)
		{
			app.logger().information("Redirect to " + it->second);
			response.redirect(it->second);
		}
		else
		{
			app.logger().error("No URI specified");
			response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
			response.send() 
				<< "<html><body><h1>" 
				"Redirect URI must be specified (protocol://address[:port]/redirect?url=address[?params])." 
				"</h1></body></html>";
		}
	}
};


class StatusRequestHandler: public HTTPRequestHandler
{
public:
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
		Application& app = Application::instance();
		std::string dateFmt = DateTimeFormat::SORTABLE_FORMAT;
		app.logger().information("Status request from " + request.clientAddress().toString());

		LocalDateTime now;
		std::string dt(DateTimeFormatter::format(now, dateFmt));

		response.setChunkedTransferEncoding(true);
		response.setContentType("text/html");

		std::ostream& ostr = response.send();
		ostr << "<html><head><title>Molto Server plugin control panel</title>";
		
		std::string refresh;
		HTMLForm form(request);
		try { refresh = form["refresh"]; }
		catch(NotFoundException&) {	refresh = "5"; }
						
		ostr << "<meta http-equiv=\"refresh\" content=\"" << refresh << "\">"
			<< "</head><body><p>" 
			<< "<p>Server started : " 
			<< DateTimeFormatter::format(LocalDateTime(app.startTime()), dateFmt)
			<< "</p><p>Current time : " << dt << "</p>"
			<< "</p><p>Uptime : " << DateTimeFormatter::format(app.uptime(), dateFmt)
			<< "</body></html>";
	}
};


class ServletRequestHandler: public HTTPRequestHandler
{
public:
	ServletRequestHandler(HttpServletDispatcher& dispatcher, 
		const HttpServerConfig& config,
		Logger* pLogger=0):
		_dispatcher(dispatcher),
		_config(config),
		_pLogger(pLogger)
	{
	}

	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
		try
		{
			if (_pLogger) 
			{
				_pLogger->information("Request from " + request.clientAddress().toString());
				_pLogger->information(request.getURI());
			}

			_pResponse = &response;
			PocoHttpServletRequest req(request, &_dispatcher);
			PocoHttpServletResponse res(response);
			std::string uri = request.getURI();

			if (_pLogger) _pLogger->trace(uri);
			// global static files are handled directly from here
			// everything else gets passed to the servlet engine
			if(isGlobalFile(uri))
			{
				Path p(_config.getGlobalDir()); p.makeParent();
				std::string path = p.toString();
				while (path.size() && path[path.size()-1] == Path::separator())
					path = path.substr(0, path.size()-1);

				path += uri;		
				handleFile(req, res, path);
			}
			else if (resolveAlias(uri))
			{
				handleFile(req, res, uri);
			}
			else
			{
				if (!_dispatcher.isStaticFileRequest(req.getServletPath()))	
				{
					try
					{
						assignSession(req, res);
					}
					catch(Exception& ex)
					{
						std::string err = ex.displayText();
						if (_pLogger) _pLogger->error(err);
						sendError(err, HTTPResponse::HTTP_PRECONDITION_FAILED);
						return;
					}
				}

				_dispatcher.dispatch(req, res);
			}
		}
		catch(NetException& ex)
		{
			std::string err = ex.displayText();
			if (_pLogger) _pLogger->error(err);
			return;
		}
		catch(Exception& ex)
		{
			std::string err = ex.displayText();
			if (_pLogger) _pLogger->error(err);
			sendError(err);
			return;
		}
	}

private:

	void sendError(const std::string& text, 
		HTTPResponse::HTTPStatus status = HTTPResponse::HTTP_INTERNAL_SERVER_ERROR)
	{
		if(_pLogger) _pLogger->error(text);
		_pResponse->setStatus(status);
		_pResponse->send() 
			<< "<html><body><h1>An error has occured:<br>" 
			<< text 
			<< "</h1></body></html>";
	}

	bool isModifiedSince(const std::string& path, const std::string& ifModifiedSince)
	{
		DateTime dt;
		int tzDiff;

		if (DateTimeParser::tryParse(ifModifiedSince, dt, tzDiff))
		{
			File f(path);
			if (f.exists()) return f.getLastModified() > dt.timestamp();
			else throw FileNotFoundException(path);
		}
		else throw DataFormatException(ifModifiedSince);
	}

	bool isGlobalFile(const std::string& path)
	{
		Path u(path);
		Path g(_config.getGlobalDir());
		return (g[g.depth()] == u[0]);
	}

	bool resolveFileName(std::string& path)
	{
		path =	path.substr(0, path.find_first_of('?'));
		std::string fileName;

		File file(path);
		
		if (file.exists())
		{
			if (file.isFile()) return true;

			if (file.isDirectory())
			{
				file = path + "/index.htm";
				if (file.exists()) { path = file.path(); return true; }

				file = path + "/index.html";
				if (file.exists()) { path = file.path(); return true; }
			}
		}
		
		return false;
	}

	bool resolveAlias(std::string& uri)
	{
		HttpServerConfig::AliasMap::const_iterator it = _config.aliases().begin();
		for(; it != _config.aliases().end(); ++it)
		{
			if(it->first.size() > uri.size()) continue;

			if(0 == icompare(it->first, 0, it->first.size(), uri, 0, it->first.size()))
			{ uri.replace(0, it->first.size(), it->second); return true; }
		}

		return false;
	}

	void handleFile(PocoHttpServletRequest& req,
			PocoHttpServletResponse& res,
			const std::string& path)
	{
		std::string fName = path;

		if(resolveFileName(fName))
		{
			bool modifiedSince = true;
			try
			{
				std::string modified = req.getHeader("If-Modified-Since");
				if ("" != modified)	modifiedSince = isModifiedSince(fName, modified);
			}
			catch (FileNotFoundException&)
			{
			}
			catch (DataFormatException&)
			{
			}

			if (modifiedSince)
			{
				if (_pLogger) _pLogger->debug("Resolved file: " + fName);
				_dispatcher.handleFileRequest(fName, res);
			}
			else
			{
				if (_pLogger) _pLogger->debug("Not modified: " + fName);
				_pResponse->setStatus(HTTPResponse::HTTP_NOT_MODIFIED);
				_pResponse->send();
			}
		}
		else
			sendError(req.getServletPath(), HTTPResponse::HTTP_NOT_FOUND);
	}

	void assignSession(PocoHttpServletRequest& req, PocoHttpServletResponse& res)
	{
		SessionManager* psm = const_cast<SessionManager*>(_dispatcher.getSessionManager());
		
		poco_check_ptr(psm);
		req.setSessionManager(psm);

		if(!req.isRequestedSessionIdValid()) 
			req.setSession(&psm->makeSession());
		else 
			req.setSession(&psm->session(req.getRequestedSessionId()));

		Cookie c(HttpSession::COOKIE_SESSION_ID, req.getSession()->getId());
		c.setSecure(req.isSecure());
		c.setPath(req.getContextPath());
		res.addCookie(c);
	}

	HttpServletDispatcher& _dispatcher;
	Logger* _pLogger;
	HTTPServerResponse* _pResponse;
	const HttpServerConfig& _config;
};


class ServletRequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
	ServletRequestHandlerFactory(HttpServletDispatcher& dispatcher, 
		const HttpServerConfig& config,
		Logger* pLogger=0):
		_dispatcher(dispatcher), 
		_config(config),
		_pLogger(pLogger)
	{
	}

	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
	{
		if (0 == icompare(std::string("/redirect"), request.getURI().substr(0,9)))
			return new RedirectRequestHandler;
		else if (0 == icompare(std::string("/status"), request.getURI().substr(0,7)))
			return new StatusRequestHandler;
		else if(request.getURI().size())
			return new ServletRequestHandler(_dispatcher, _config, _pLogger);
		else
			return 0;
	}
private:
	HttpServletDispatcher& _dispatcher;
	Logger* _pLogger;
	const HttpServerConfig& _config;
};


//PocoServer

PocoServer::PocoServer()
{
}


PocoServer::PocoServer(const HttpServerConfig& config, HttpServletDispatcher& dispatcher, Logger* pLogger):
HttpServer(config, dispatcher, pLogger)
{
}


PocoServer::~PocoServer() 
{
}


void PocoServer::start()
{
	Mutex::ScopedLock l(_mutex);

	if(!isRunning())
	{
		information("Getting ready to run Poco HTTP server ...");
		std::vector<HTTPServer*> servers;
		
		const HttpServerConfig* pConfig = config();
		poco_check_ptr(pConfig);
		HttpServletDispatcher* pDispatcher = dispatcher();
		poco_check_ptr(pConfig);

		std::vector<std::string>* pInterfaces = 0;
		const std::vector<std::string>& interfaces = 
			pConfig->interfaces().size() ? pConfig->interfaces() : *(pInterfaces = new std::vector<std::string>);

		if (0 == interfaces.size()) // no interfaces specified in config, listen on all
		{
			NetworkInterface::NetworkInterfaceList list = NetworkInterface::list();
			poco_assert (!list.empty());
			NetworkInterface::NetworkInterfaceList::const_iterator it = list.begin();
			for (; it != list.end(); ++it) 
				const_cast<std::vector<std::string>&>(interfaces).push_back(it->address().toString() + ":80");
		}

		std::ostringstream listenList;
		listenList << "( ";

		std::vector<std::string>::const_iterator it = interfaces.begin();
		for(; it != interfaces.end(); ++it)
		{
			try
			{
				std::string address(*it);
				if (address.find(':') == address.npos) address += ":80";
				SocketAddress sa(address);
				ServerSocket svs(sa);
				std::ostringstream os;
				os << "Server socket created for " << *it;
				debug(os.str());

				HTTPServerParams* pParams = new HTTPServerParams;
				ServletRequestHandlerFactory* pFactory = 
					new ServletRequestHandlerFactory(*pDispatcher, *pConfig, &logger());
				servers.push_back(new HTTPServer(pFactory, svs, pParams));
				poco_check_ptr(servers.back());

				debug("Server created.");
				servers.back()->start(); 
				listenList << address << " ";
				running(true);
			}
			catch(Exception& ex)
			{
				error(ex.displayText());
			}
		}

		delete pInterfaces;

		if(isRunning())
		{
			listenList << ')';
			information("Server up and running " + listenList.str());
			
			while(!isStopped()) Thread::sleep(300);
			information("Stop signal received. Shutting down.");
			
			std::vector<HTTPServer*>::iterator sit = servers.begin();
			for(; sit != servers.end(); ++sit) delete *sit;

			running(false);
		}
		else error("No servers found. Exiting.");
	}
}


} } } // namespace Poco::Servlet::PocoServer
