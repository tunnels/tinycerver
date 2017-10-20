// 100, 200, 201, 202, 400, 401, 403, 404, 500, 503 ?

char *getStatusGivenCode(int http_response_code)
{
	char* status_code  = 
		"HTTP/1.1 500 Internal Server Error\r\n"
		"Content-type: text/html\r\n"
		"\r\n"
		"<html>\r\n"
		"	<body>\r\n"
		"		<h1>Internal Server Error</h1>\r\n"
		"		<p>A generic error message, given when an unexpected condition was encountered and no more specific message is suitable.</p>\r\n"
		"	</body>\r\n"
		"</html>\r\n";

	if (http_response_code == 500) {
		return status_code;
	}

	if (http_response_code == 200) {
		status_code = 
		"HTTP/1.1 200 OK\r\n"
		"Content-type: text/html\r\n"
		"\r\n";
	}

	if (http_response_code == 400) {
		status_code = 
		"HTTP/1.1 400 Bad Request\r\n"
		"Content-type: text/html\r\n"
		"\r\n"
		"<html>\r\n"
		"	<body>\r\n"
		"		<h1>Bad Request</h1>\r\n"
		"		<p>Your request cannot be understood by the server.</p>\r\n"
		"	</body>\r\n"
		"</html>\r\n";
	}

	if (http_response_code == 404) {
		status_code = 
		"HTTP/1.1 404 Not Found\r\n"
		"Content-type: text/html\r\n"
		"\r\n"
		"<html>\r\n"
		"	<body>\r\n"
		"		<h1>Not Found</h1>\r\n"
		"		<p>The requested URL was not found on this server.</p>\r\n"
		"	</body>\r\n"
		"</html>\r\n";
	}

	return status_code;
}
