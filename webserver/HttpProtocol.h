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
	HttpHeader(const std::string& headerLine, size_t colSpacePos);
	HttpHeader(const std::string& field, const std::string& value);

	const std::string& getField();
	const std::string& getValue();
};

class QueryParameter
{
private:
	std::string param;
	std::string value;
public:
	QueryParameter();
	QueryParameter(const std::string& param, const std::string& value);

	const std::string& getParam();
	const std::string& getValue();
};

class HttpRequest
{
private:
	std::string method;
	std::string requestURI;
	std::string httpVersion;
	std::vector<HttpHeader*> headers;
	std::vector<QueryParameter*> parameters;
	bool keepAlive;

	void parseRequestLine(std::string& reqline);
	void parseQueryParameters();
	void parseHeaderLine(std::string& hline);
public:
	HttpRequest(char* data);
	~HttpRequest();

	const std::string& getMethod();
	const std::string& getRequestURI();
	const std::string& getHttpVersion();
	const std::vector<HttpHeader*>& getHeaders();
	const std::vector<QueryParameter*>& getParams();
	bool isKeepAlive();
};

class HttpResponse
{
private:
	std::string statusLine;
	std::vector<HttpHeader*> headers;
	void* content;
	unsigned contentLength;
public:
	HttpResponse();
	HttpResponse(const std::string& statusLine);
	~HttpResponse();

	void setStatusLine(const std::string& stline);
	void addHeader(const std::string& field, const std::string& value);
	void setContent(void* content, unsigned length);

	void writeToFD(int fd);
};


#endif /* HTTPREQUEST_H_ */
