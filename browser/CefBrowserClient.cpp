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
  * @file CefBrowserClient.h
  * @author Jichan ( development@jc-lab.net )
  * @date 2019-06-02
  */

#include "CefBrowserClient.h"

#include "CefBrowserApp.h"

#include "CefAppUIInterfaceHandlerFactory.h"

namespace JsCefHelper {
	namespace browser {

		CefBrowserClient::CefBrowserClient(CefRefPtr<CefBrowserApp> browser_app, CefRefPtr<CefAppUIInterfaceHandlerFactory> appUIInterfaceHandlerFactory, CefRefPtr<CefClient> cefClientDelegate)
			: app_(browser_app)
		{
			this->appUIInterfaceHandlerFactory = appUIInterfaceHandlerFactory;
			this->cefClientDelegate = cefClientDelegate;
			this->eventBrowserIsReady = CefWaitableEvent::CreateWaitableEvent(false, false);
			this->eventFrameIsReady = CefWaitableEvent::CreateWaitableEvent(false, false);
		}

		CefBrowserClient::~CefBrowserClient() {
		}

		CefRefPtr<CefContextMenuHandler> CefBrowserClient::GetContextMenuHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetContextMenuHandler();
			return this;
		}
		CefRefPtr<CefDialogHandler> CefBrowserClient::GetDialogHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetDialogHandler();
			return NULL;
		}
		CefRefPtr<CefDisplayHandler> CefBrowserClient::GetDisplayHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetDisplayHandler();
			return NULL;
		}
		CefRefPtr<CefDownloadHandler> CefBrowserClient::GetDownloadHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetDownloadHandler();
			return NULL;
		}
		CefRefPtr<CefDragHandler> CefBrowserClient::GetDragHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetDragHandler();
			return NULL;
		}
		CefRefPtr<CefFindHandler> CefBrowserClient::GetFindHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetFindHandler();
			return NULL;
		}
		CefRefPtr<CefFocusHandler> CefBrowserClient::GetFocusHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetFocusHandler();
			return NULL;
		}
		CefRefPtr<CefJSDialogHandler> CefBrowserClient::GetJSDialogHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetJSDialogHandler();
			return NULL;
		}
		CefRefPtr<CefKeyboardHandler> CefBrowserClient::GetKeyboardHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetKeyboardHandler();
			return NULL;
		}
		CefRefPtr<CefLifeSpanHandler> CefBrowserClient::GetLifeSpanHandler() {
			return this;
		}
		CefRefPtr<CefLoadHandler> CefBrowserClient::GetLoadHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetLoadHandler();
			return NULL;
		}
		CefRefPtr<CefRenderHandler> CefBrowserClient::GetRenderHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetRenderHandler();
			return NULL;
		}
		CefRefPtr<CefRequestHandler> CefBrowserClient::GetRequestHandler() {
			if (cefClientDelegate.get())
				return cefClientDelegate->GetRequestHandler();
			return this;
		}
		bool CefBrowserClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) {
			std::string name = message->GetName();
			CefRefPtr<CefListValue> arguments = message->GetArgumentList();
			if (name == "JsCefHelperif_newctx")
			{
				if (!arguments || (arguments->GetSize() != 2))
				{
					return false;
				}
				std::string uniqueKey = arguments->GetString(0);
				std::string viewName = arguments->GetString(1);

				if (this->appUIInterfaceHandlerFactory.get())
				{
					CefRefPtr<CefAppUIInterfaceHandler> handler = this->appUIInterfaceHandlerFactory->createAppUIInterfaceHandler(viewName);
					if (handler.get())
					{
						handler->attach(browser, uniqueKey);
						m_interfaces[uniqueKey] = handler;
						return true;
					}
				}
			}
			else if (name == "JsCefHelperif_delctxs")
			{
				size_t i;
				if (!arguments || (arguments->GetSize() <= 0))
				{
					return false;
				}
				for (i = 0; i < arguments->GetSize(); i++)
				{
					std::string uniqueKey = arguments->GetString(i);
					interface_map_t::iterator ifIter = m_interfaces.find(uniqueKey);
					if (ifIter != m_interfaces.end()) {
						ifIter->second->detach();
						m_interfaces.erase(ifIter);
					}
				}
				return true;
			}
			else if (name == "JsCefHelperif_front_exec")
			{
				// uniqueKey, methodName[, arguments...]
				if (!arguments || (arguments->GetSize() < 2))
				{
					return false;
				}
				std::string uniqueKey = arguments->GetString(0);
				std::string methodName = arguments->GetString(1);
				interface_map_t::iterator ifIter = m_interfaces.find(uniqueKey);
				if (ifIter != m_interfaces.end()) {
					ifIter->second->onFrontExecuteRequest(methodName, arguments, 2);
					return true;
				}
			}
			return false;
		}

		void CefBrowserClient::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefContextMenuParams> params,
			CefRefPtr<CefMenuModel> model)
		{
			// Hide context menu
			model->Clear();
		}

		// ========== CefLifeSpanHandler ==========
		bool CefBrowserClient::OnBeforePopup(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			const CefString& target_url,
			const CefString& target_frame_name,
			CefLifeSpanHandler::WindowOpenDisposition target_disposition,
			bool user_gesture,
			const CefPopupFeatures& popupFeatures,
			CefWindowInfo& windowInfo,
			CefRefPtr<CefClient>& client,
			CefBrowserSettings& settings,
			bool* no_javascript_access) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefLifeSpanHandler> handler = cefClientDelegate->GetLifeSpanHandler();
				if (handler.get()) {
					return handler->OnBeforePopup(browser, frame, target_url, target_frame_name, target_disposition, user_gesture, popupFeatures, windowInfo, client, settings, no_javascript_access);
				}
			}
			return false;
		}
		void CefBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
		{
			this->m_browser = browser;
			m_show_lock.Acquire();
			if (!m_show_url.empty())
			{
				CefRefPtr<CefFrame> frame = m_browser->GetMainFrame();
				m_browser->StopLoad();
				if (frame.get())
				{
					frame->LoadURL(m_show_url);
					m_show_url.clear();
				}
			}
			m_show_lock.Release();
			this->eventBrowserIsReady->Signal();
			if (cefClientDelegate.get()) {
				CefRefPtr<CefLifeSpanHandler> handler = cefClientDelegate->GetLifeSpanHandler();
				if (handler.get()) {
					handler->OnAfterCreated(browser);
				}
			}
		}
		bool CefBrowserClient::DoClose(CefRefPtr<CefBrowser> browser)
		{
			m_browser = NULL;
			if (cefClientDelegate.get()) {
				CefRefPtr<CefLifeSpanHandler> handler = cefClientDelegate->GetLifeSpanHandler();
				if (handler.get()) {
					return handler->DoClose(browser);
				}
			}
			return true;
		}
		void CefBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
		{
			if (m_browser != NULL && browser->GetIdentifier() == m_browser->GetIdentifier())
			{
				m_browser = NULL;
				if (cefClientDelegate.get()) {
					CefRefPtr<CefLifeSpanHandler> handler = cefClientDelegate->GetLifeSpanHandler();
					if (handler.get()) {
						handler->OnBeforeClose(browser);
					}
				}
			}
		}
		// ========== CefRequestHandler ==========
		bool CefBrowserClient::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			bool user_gesture,
			bool is_redirect) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnBeforeBrowse(browser, frame, request, user_gesture, is_redirect);
				}
			}
			return false;
		}
		bool CefBrowserClient::OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			const CefString& target_url,
			CefRequestHandler::WindowOpenDisposition target_disposition,
			bool user_gesture) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnOpenURLFromTab(browser, frame, target_url, target_disposition, user_gesture);
				}
			}
			return false;
		}
		CefRequestHandler::ReturnValue CefBrowserClient::OnBeforeResourceLoad(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			CefRefPtr<CefRequestCallback> callback) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnBeforeResourceLoad(browser, frame, request, callback);
				}
			}
			return app_->cef_resource_manager()->OnBeforeResourceLoad(browser, frame, request, callback);
		}
		CefRefPtr<CefResourceHandler> CefBrowserClient::GetResourceHandler(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->GetResourceHandler(browser, frame, request);
				}
			}
			return app_->cef_resource_manager()->GetResourceHandler(browser, frame, request);
		}
		void CefBrowserClient::OnResourceRedirect(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			CefRefPtr<CefResponse> response,
			CefString& new_url) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					handler->OnResourceRedirect(browser, frame, request, response, new_url);
				}
			}
		}
		bool CefBrowserClient::OnResourceResponse(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			CefRefPtr<CefResponse> response) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnResourceResponse(browser, frame, request, response);
				}
			}
			return false;
		}
		CefRefPtr<CefResponseFilter> CefBrowserClient::GetResourceResponseFilter(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			CefRefPtr<CefResponse> response) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->GetResourceResponseFilter(browser, frame, request, response);
				}
			}
			return NULL;
		}
		void CefBrowserClient::OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			CefRefPtr<CefResponse> response,
			URLRequestStatus status,
			int64 received_content_length) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnResourceLoadComplete(browser, frame, request, response, status, received_content_length);
				}
			}
		}
		bool CefBrowserClient::GetAuthCredentials(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			bool isProxy,
			const CefString& host,
			int port,
			const CefString& realm,
			const CefString& scheme,
			CefRefPtr<CefAuthCallback> callback) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->GetAuthCredentials(browser, frame, isProxy, host, port, realm, scheme, callback);
				}
			}
			return false;
		}
		bool CefBrowserClient::CanGetCookies(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->CanGetCookies(browser, frame, request);
				}
			}
			return true;
		}
		bool CefBrowserClient::CanSetCookie(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			const CefCookie& cookie) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->CanSetCookie(browser, frame, request, cookie);
				}
			}
			return true;
		}
		bool CefBrowserClient::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
			const CefString& origin_url,
			int64 new_size,
			CefRefPtr<CefRequestCallback> callback) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnQuotaRequest(browser, origin_url, new_size, callback);
				}
			}
			return false;
		}
		void CefBrowserClient::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
			const CefString& url,
			bool& allow_os_execution) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnProtocolExecution(browser, url, allow_os_execution);
				}
			}
		}
		bool CefBrowserClient::OnCertificateError(CefRefPtr<CefBrowser> browser,
			cef_errorcode_t cert_error,
			const CefString& request_url,
			CefRefPtr<CefSSLInfo> ssl_info,
			CefRefPtr<CefRequestCallback> callback) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnCertificateError(browser, cert_error, request_url, ssl_info, callback);
				}
			}
			return false;
		}
		bool CefBrowserClient::OnSelectClientCertificate(
			CefRefPtr<CefBrowser> browser,
			bool isProxy,
			const CefString& host,
			int port,
			const X509CertificateList& certificates,
			CefRefPtr<CefSelectClientCertificateCallback> callback) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnSelectClientCertificate(browser, isProxy, host, port, certificates, callback);
				}
			}
			return false;
		}
		void CefBrowserClient::OnPluginCrashed(CefRefPtr<CefBrowser> browser,
			const CefString& plugin_path) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnPluginCrashed(browser, plugin_path);
				}
			}
		}
		void CefBrowserClient::OnRenderViewReady(CefRefPtr<CefBrowser> browser) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnRenderViewReady(browser);
				}
			}
		}
		void CefBrowserClient::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
			TerminationStatus status) {
			if (cefClientDelegate.get()) {
				CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
				if (handler.get()) {
					return handler->OnRenderProcessTerminated(browser, status);
				}
			}
		}

		void CefBrowserClient::loadUrl(const std::string& url)
		{
			if (m_browser.get())
			{
				CefRefPtr<CefFrame> frame = m_browser->GetMainFrame();
				if (frame.get())
				{
					frame->LoadURL(url);
					return;
				}
			}

			m_show_lock.Acquire();
			m_show_url = url;
			m_show_lock.Release();
		}

		std::string CefBrowserClient::swapLoadUrl()
		{
			std::string result;
			m_show_lock.Acquire();
			result = m_show_url;
			m_show_lock.Release();
			return result;
		}

		bool CefBrowserClient::loadCefRequest(const CefRefPtr<CefRequest>& request)
		{
			if (m_browser.get())
			{
				CefRefPtr<CefFrame> frame = m_browser->GetMainFrame();
				if (frame.get())
				{
					frame->LoadRequest(request);
					return true;
				}
			}

			return false;
		}

	}
}
