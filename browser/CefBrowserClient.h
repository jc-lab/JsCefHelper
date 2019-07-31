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

#include <include/cef_client.h>
#include <include/cef_waitable_event.h>

#include "CefAppUIInterfaceHandler.h"

namespace JsCefHelper {
	namespace browser {
		class CefBrowserApp;
		class CefAppUIInterfaceHandlerFactory;

		class CefBrowserClient : public CefClient, public CefLifeSpanHandler, public CefRequestHandler, public CefContextMenuHandler, public CefLoadHandler {
		private:
			IMPLEMENT_REFCOUNTING(CefBrowserClient);

		private:
			CefRefPtr<CefBrowserApp> app_;
			CefRefPtr<CefAppUIInterfaceHandlerFactory> appUIInterfaceHandlerFactory;
			CefRefPtr<CefClient> cefClientDelegate;
			base::Lock m_show_lock;
			std::string m_show_url;

			typedef std::map<std::string, CefRefPtr<CefAppUIInterfaceHandler> > interface_map_t;
			interface_map_t m_interfaces;

		public:
			CefRefPtr<CefWaitableEvent> eventBrowserIsReady;
			CefRefPtr<CefWaitableEvent> eventFrameIsReady;

			CefRefPtr<CefBrowser> m_browser;

			CefBrowserClient(CefRefPtr<CefBrowserApp> browser_app, CefRefPtr<CefAppUIInterfaceHandlerFactory> appUIInterfaceHandlerFactory, CefRefPtr<CefClient> cefClientDelegate);
			~CefBrowserClient();

			CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE;
			CefRefPtr<CefDialogHandler> GetDialogHandler() OVERRIDE;
			CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE;
			CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE;
			CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE;
			CefRefPtr<CefFindHandler> GetFindHandler() OVERRIDE;
			CefRefPtr<CefFocusHandler> GetFocusHandler() OVERRIDE;
			CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() OVERRIDE;
			CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE;
			CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler();
			CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
			CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE;
			CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE;
			bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
				CefProcessId source_process,
				CefRefPtr<CefProcessMessage> message) OVERRIDE;

			virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefContextMenuParams> params,
				CefRefPtr<CefMenuModel> model);

			// ========== CefLifeSpanHandler ==========
			virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				const CefString& target_url,
				const CefString& target_frame_name,
				CefLifeSpanHandler::WindowOpenDisposition target_disposition,
				bool user_gesture,
				const CefPopupFeatures& popupFeatures,
				CefWindowInfo& windowInfo,
				CefRefPtr<CefClient>& client,
				CefBrowserSettings& settings,
				bool* no_javascript_access) OVERRIDE;
			void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
			bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
			void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
			// ========== CefRequestHandler ==========
			virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				bool user_gesture,
				bool is_redirect) OVERRIDE;
			virtual bool OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				const CefString& target_url,
				CefRequestHandler::WindowOpenDisposition target_disposition,
				bool user_gesture) OVERRIDE;
			virtual ReturnValue OnBeforeResourceLoad(
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				CefRefPtr<CefRequestCallback> callback) OVERRIDE;
			virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request) OVERRIDE;
			virtual void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				CefRefPtr<CefResponse> response,
				CefString& new_url) OVERRIDE;
			virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				CefRefPtr<CefResponse> response) OVERRIDE;
			virtual CefRefPtr<CefResponseFilter> GetResourceResponseFilter(
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				CefRefPtr<CefResponse> response) OVERRIDE;
			virtual void OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				CefRefPtr<CefResponse> response,
				URLRequestStatus status,
				int64 received_content_length) OVERRIDE;
			virtual bool GetAuthCredentials(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				bool isProxy,
				const CefString& host,
				int port,
				const CefString& realm,
				const CefString& scheme,
				CefRefPtr<CefAuthCallback> callback) OVERRIDE;
			virtual bool CanGetCookies(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request) OVERRIDE;
			virtual bool CanSetCookie(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				const CefCookie& cookie) OVERRIDE;
			virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
				const CefString& origin_url,
				int64 new_size,
				CefRefPtr<CefRequestCallback> callback) OVERRIDE;
			virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
				const CefString& url,
				bool& allow_os_execution) OVERRIDE;
			virtual bool OnCertificateError(CefRefPtr<CefBrowser> browser,
				cef_errorcode_t cert_error,
				const CefString& request_url,
				CefRefPtr<CefSSLInfo> ssl_info,
				CefRefPtr<CefRequestCallback> callback) OVERRIDE;
			virtual bool OnSelectClientCertificate(
				CefRefPtr<CefBrowser> browser,
				bool isProxy,
				const CefString& host,
				int port,
				const X509CertificateList& certificates,
				CefRefPtr<CefSelectClientCertificateCallback> callback) OVERRIDE;
			virtual void OnPluginCrashed(CefRefPtr<CefBrowser> browser,
				const CefString& plugin_path) OVERRIDE;
			virtual void OnRenderViewReady(CefRefPtr<CefBrowser> browser) OVERRIDE;
			virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
				TerminationStatus status) OVERRIDE;
			// ========== CefLoadHandler ==========
			void OnLoadError(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				ErrorCode errorCode,
				const CefString& errorText,
				const CefString& failedUrl);

			void loadUrl(const std::string& url);
			std::string swapLoadUrl();
			bool loadCefRequest(const CefRefPtr<CefRequest>& request);
		};
	}
}
