#pragma once
#include <vector>
#include <string>
#include "Request.h"
#include "Reply.h"

namespace file_server {

	class RequestHandler
	{
	public:
		//bool HandleRequest(const std::string&, std::vector<char>&);
		bool HandleRequest(Request& req,Reply& rep);

	private:
		enum {
			head_length = 4
		};
	};

}



