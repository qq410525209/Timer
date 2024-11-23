#pragma once

#include <hv/HttpMessage.h>
#include <sdk/common.h>

using IPv4Response = std::function<void(const std::string& ip)>;

namespace HTTP {
	void Get(const std::string& url, HttpResponseCallback cb = 0);
	void Get(const std::string& url, const json& params, HttpResponseCallback cb = 0);

	void Post(const std::string& url, HttpResponseCallback cb = 0);
	void Post(const std::string& url, const json& body, HttpResponseCallback cb = 0);

	void GetPublicIP(IPv4Response callback);
} // namespace HTTP
