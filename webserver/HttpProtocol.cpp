#include "HttpProtocol.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstdio>
using namespace std;

HttpHeader::HttpHeader()
{
}

HttpHeader::HttpHeader(const string& hline, size_t colSpacePos)
	: field(hline.substr(0,colSpacePos)), value(hline.substr(colSpacePos+2))
{
}

HttpHeader::HttpHeader(const string& f, const string& v) : field(f), value(v)
{
}

const string& HttpHeader::getField()
{
	return field;
}

const string& HttpHeader::getValue()
{
	return value;
}

QueryParameter::QueryParameter()
{
}

QueryParameter::QueryParameter(const string& p, const string& v) : param(p), value(v)
{
}

const string& QueryParameter::getParam()
{
	return param;
}

const string& QueryParameter::getValue()
{
	return value;
}


HttpRequest::HttpRequest(char* data)
{
	string strdata(data);
	vector<string> lines;
	size_t start=0;
	size_t end = strdata.find("\r\n");
	bool hasContent = false;
	keepAlive = false;
	while (end != string::npos)
	{
		lines.push_back(strdata.substr(start,end-start));
		if (lines.back().find("Content-Length") == 0) hasContent = true;
		if (start == end && !hasContent) break;
		start = end + 2;
		end = strdata.find("\r\n",start);
	}
	parseRequestLine(lines[0]);
	for (unsigned i=1;i<lines.size();i++)
		if (lines[i].compare("") != 0)
			parseHeaderLine(lines[i]);

}

HttpRequest::~HttpRequest()
{
	for (unsigned int i=0;i<headers.size();i++)
		delete headers[i];
	for (unsigned int i=0;i<parameters.size();i++)
			delete parameters[i];
}

void HttpRequest::parseRequestLine(string& reqline)
{
	size_t sp1 = reqline.find(" ");
	size_t sp2 = reqline.find(" ",sp1+1);
	method = reqline.substr(0,sp1);
	requestURI = reqline.substr(sp1+1,sp2-sp1-1);
	parseQueryParameters();
	if (requestURI.length()!=1 && requestURI[requestURI.length()-1]=='/') requestURI.erase(requestURI.end()-1);
	httpVersion = reqline.substr(sp2);
}

void HttpRequest::parseQueryParameters()
{
	size_t qMark = requestURI.find('?');
	if (qMark != string::npos)
	{
		size_t start = qMark+1;
		size_t mid = requestURI.find('=',start);
		size_t end = requestURI.find('&',mid);
		if (end == string::npos)
		{
			parameters.push_back(new QueryParameter(requestURI.substr(start,mid-start),requestURI.substr(mid+1)));
			requestURI = requestURI.substr(0,qMark);
			return;
		}
		parameters.push_back(new QueryParameter(requestURI.substr(start,mid-start),requestURI.substr(mid+1,end-mid-1)));
		while (end != string::npos)
		{
			size_t start = end+1;
			size_t mid = requestURI.find('=',start);
			size_t end = requestURI.find('&',mid);
			if (end == string::npos)
			{
				parameters.push_back(new QueryParameter(requestURI.substr(start,mid-start),requestURI.substr(mid+1)));
				requestURI = requestURI.substr(0,qMark);
				return;
			}
			parameters.push_back(new QueryParameter(requestURI.substr(start,mid-start),requestURI.substr(mid+1,end-mid-1)));
		}
	}
}

void HttpRequest::parseHeaderLine(string& hline)
{
	headers.push_back(new HttpHeader(hline,hline.find(": ")));
	if (headers.back()->getField().compare("Connection") &&
		headers.back()->getValue().compare("Keep-Alive"))
	{
		keepAlive = true;
	}
}

const std::string& HttpRequest::getMethod()
{
	return method;
}

const std::string& HttpRequest::getRequestURI()
{
	return requestURI;
}

const std::string& HttpRequest::getHttpVersion()
{
	return httpVersion;
}

const std::vector<HttpHeader*>& HttpRequest::getHeaders()
{
	return headers;
}

const std::vector<QueryParameter*>& HttpRequest::getParams()
{
	return parameters;
}

bool HttpRequest::isKeepAlive()
{
	return keepAlive;
}

HttpResponse::HttpResponse()
	: content(NULL), contentLength(0)
{
}

HttpResponse::HttpResponse(const string& status)
	: statusLine(status), content(NULL), contentLength(0)
{
}

HttpResponse::~HttpResponse()
{
	for (unsigned i=0;i<headers.size();i++)
		delete headers[i];
}

void HttpResponse::setStatusLine(const string& stline)
{
	this->statusLine = stline;
}

void HttpResponse::addHeader(const string& field, const string& value)
{
	headers.push_back(new HttpHeader(field,value));
}

void HttpResponse::setContent(void* content, unsigned length)
{
	this->content = content;
	this->contentLength = length;
}

void HttpResponse::writeToFD(int fd)
{
	if (write(fd,statusLine.c_str(),statusLine.length()) == -1)
	{
		perror("Failed while writing status line");
		return;
	}
	if (write(fd,"\r\n",2) == -1)
	{
		perror("Failed while writing status line delimiter");
		return;
	}
	for (unsigned i=0;i<headers.size();i++)
	{
		if (write(fd,headers[i]->getField().c_str(),headers[i]->getField().length()) == -1)
		{
			perror("Failed while writing header field");
			return;
		}
		if (write(fd,": ",2) == -1)
		{
			perror("Failed while writing header field delimited");
			return;
		}
		if (write(fd,headers[i]->getValue().c_str(),headers[i]->getValue().length()) == -1)
		{
			perror("Failed while writing header value");
			return;
		}
		if (write(fd,"\r\n",2) == -1)
		{
			perror("Failed while writing header delimiter");
			return;
		}
	}
	if (write(fd,"\r\n",2) == -1)
	{
		perror("Failed while writing final header delimiter");
		return;
	}
	if (content != NULL)
	{
		if (write(fd,content,contentLength) == -1)
		{
			perror("Failed while writing content");
			return;
		}
	}
}
