#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_
#include <vector>
#include <string>

class HttpHeader
{
private:
	std::string field;
	std::string value;
public:
	HttpHeader();
	HttpHeader(std::string headerLine, size_t colSpacePos);
	HttpHeader(std::string field, std::string value);

	const std::string& getField();
	const std::string& getValue();
};

class HttpRequest
{
private:
	std::string method;
	std::string requestURI;
	std::string httpVersion;
	std::vector<HttpHeader*> headers;

	void parseRequestLine(std::string reqline);
	void parseHeaderLine(std::string hline);
public:
	HttpRequest(char* data);
	~HttpRequest();

	const std::string& getMethod();
	const std::string& getRequestURI();
	const std::string& getHttpVersion();
	const std::vector<HttpHeader*>& getHeaders();
};

class HttpResponse
{
private:
	std::string statusLine;
	std::vector<HttpHeader*> headers;
	void* content;
	unsigned contentLength;
public:
	HttpResponse(std::string statusLine);
	~HttpResponse();

	void addHeader(std::string field, std::string value);
	void setContent(void* content, unsigned length);

	void writeToFD(int fd);
};


#endif /* HTTPREQUEST_H_ */
