/*
 * Copyright 2019 JC-Lab
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file CefBasicClient.cpp
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */

#include "CefBasicClient.h"

#include <include/cef_request_handler.h>

namespace JsCefHelper {
	namespace browser {

		CefBasicClient::CefBasicClient(CefRefPtr<CefBrowserApp> app) : 
			app_(app)
		{
		}

		CefRefPtr<CefResourceHandler> CefBasicClient::GetResourceHandler(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request) {
			return app_->cef_resource_manager()->GetResourceHandler(browser, frame, request);
		}

		CefRequestHandler::ReturnValue CefBasicClient::OnBeforeResourceLoad(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			CefRefPtr<CefRequestCallback> callback) {
			return app_->cef_resource_manager()->OnBeforeResourceLoad(browser, frame, request, callback);
		};

	} // namespace browser
} // namespace JsCefHelper
