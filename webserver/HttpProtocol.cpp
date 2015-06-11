#include "HttpProtocol.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>
using namespace std;

HttpHeader::HttpHeader()
{
}

HttpHeader::HttpHeader(string hline, size_t colSpacePos)
	: field(hline.substr(0,colSpacePos)), value(hline.substr(colSpacePos+2))
{
}

HttpHeader::HttpHeader(string f, string v) : field(f), value(v)
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


HttpRequest::HttpRequest(char* data)
{
	string strdata(data);
	vector<string> lines;
	size_t start=0;
	size_t end = strdata.find("\r\n");
	bool hasContent = false;
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
}

void HttpRequest::parseRequestLine(string reqline)
{
	size_t sp1 = reqline.find(" ");
	size_t sp2 = reqline.find(" ",sp1+1);
	method = reqline.substr(0,sp1);
	requestURI = reqline.substr(sp1+1,sp2-sp1-1);
	httpVersion = reqline.substr(sp2);
}

void HttpRequest::parseHeaderLine(string hline)
{
	headers.push_back(new HttpHeader(hline,hline.find(": ")));
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

HttpResponse::HttpResponse(string status)
	: statusLine(status), content(NULL), contentLength(0)
{
}

HttpResponse::~HttpResponse()
{
	for (unsigned i=0;i<headers.size();i++)
		delete headers[i];
}

void HttpResponse::addHeader(string field, string value)
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
	write(fd,statusLine.c_str(),statusLine.length());
	write(fd,"\r\n",2);
	for (unsigned i=0;i<headers.size();i++)
	{
		write(fd,headers[i]->getField().c_str(),headers[i]->getField().length());
		write(fd,": ",2);
		write(fd,headers[i]->getValue().c_str(),headers[i]->getValue().length());
		write(fd,"\r\n",2);
	}
	write(fd,"\r\n",2);
	if (content != NULL)
	{
		write(fd,content,contentLength);
	}
}
