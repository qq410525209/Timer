#include "surf_global.h"
#include <hv/requests.h>
#include <hv/hthread.h>
#include <core/logger.h>

void CSurfGlobalAPIPlugin::CreateRequest(std::string sEndpointAlias, const GlobalAPIRequest& req, HttpResponseCallback& cb) {
	if (!m_umEndpoint.contains(sEndpointAlias)) {
		LOG::Warning("[SurfGlobalAPIPlugin::CreateRequest] Endpoint alias \'%s\' not exists!\n", sEndpointAlias.c_str());
		return;
	}

	auto& sEndpoint = m_umEndpoint[sEndpointAlias];

	switch (req.m_iMethod) {
		case HTTP_GET: {
			HttpRequestPtr http_req(new HttpRequest);
			http_req->method = HTTP_GET;

			if (!req.empty()) {
				for (auto it = req.begin(); it != req.end(); ++it) {
					auto& val = it.value();
					http_req->query_params[it.key()] = val.is_string() ? val.get<std::string>() : val.dump();
				}
			}
			http_req->url = sEndpoint;

			http_req->timeout = req.m_iTimeout;

			http_req->SetBearerTokenAuth(req.m_sCustomToken.empty() ? m_GlobalAuth.m_sToken : req.m_sCustomToken);

			http_client_send_async(http_req, cb);
			break;
		}
		case HTTP_POST: {
			HttpRequestPtr http_req(new HttpRequest);
			http_req->method = HTTP_POST;
			http_req->url = sEndpoint;
			http_req->timeout = req.m_iTimeout;

			if (!req.empty()) {
				std::string sBody = req.dump();
				http_req->SetBody(sBody);
			}

			http_req->SetBearerTokenAuth(req.m_sCustomToken.empty() ? m_GlobalAuth.m_sToken : req.m_sCustomToken);
			http_req->SetHeader("Content-Type", "application/json");

			http_client_send_async(http_req, cb);
			break;
		}
		default: {
			SDK_ASSERT(false);
			break;
		}
	}
}
