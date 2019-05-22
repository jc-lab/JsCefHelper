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
 * @file BrowserWindow.cpp
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#include <afx.h>
#include <WinSock2.h>
#include "afxwin.h"
#include "afxcmn.h"

#include <map>

#include <include/cef_client.h>
#include <include/cef_v8.h>

#include "BrowserWindow.h"
#include "CefBrowserApp.h"
#include "CefAppUIInterfaceHandler.h"

namespace JsCefHelper {
	namespace browser {
		IMPLEMENT_DYNAMIC(CBrowserWindow, CFrameWnd)

		BEGIN_MESSAGE_MAP(CBrowserWindow, CFrameWnd)
			ON_WM_CREATE()
			ON_WM_CLOSE()
			ON_WM_DESTROY()
			ON_WM_SHOWWINDOW()
			ON_WM_SYSCOMMAND()
			ON_WM_SIZE()
			ON_MESSAGE(WM_DOLOADCEF, OnLoadCef)
			ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
		END_MESSAGE_MAP()

		const LONG CBrowserWindow::WM_DOLOADCEF = WM_USER + 1;
		const LONG CBrowserWindow::WM_CUSTOM = WM_USER + 2;

		class CBrowserWindow::CefBrowserClient : public CefClient, public CefLifeSpanHandler, public CefRequestHandler, public CefContextMenuHandler {
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

			CefBrowserClient(CefRefPtr<CefBrowserApp> browser_app, CefRefPtr<CefAppUIInterfaceHandlerFactory> appUIInterfaceHandlerFactory, CefRefPtr<CefClient> cefClientDelegate) : 
				app_(browser_app)
			{
				this->appUIInterfaceHandlerFactory = appUIInterfaceHandlerFactory;
				this->cefClientDelegate = cefClientDelegate;
				this->eventBrowserIsReady = CefWaitableEvent::CreateWaitableEvent(false, false);
				this->eventFrameIsReady = CefWaitableEvent::CreateWaitableEvent(false, false);
			}

			~CefBrowserClient() {
			}

			CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetContextMenuHandler();
				return this;
			}
			CefRefPtr<CefDialogHandler> GetDialogHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetDialogHandler();
				return NULL;
			}
			CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetDisplayHandler();
				return NULL;
			}
			CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetDownloadHandler();
				return NULL;
			}
			CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetDragHandler();
				return NULL;
			}
			CefRefPtr<CefFindHandler> GetFindHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetFindHandler();
				return NULL;
			}
			CefRefPtr<CefFocusHandler> GetFocusHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetFocusHandler();
				return NULL;
			}
			CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetJSDialogHandler();
				return NULL;
			}
			CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetKeyboardHandler();
				return NULL;
			}
			CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() {
				return this;
			}
			CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetLoadHandler();
				return NULL;
			}
			CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetRenderHandler();
				return NULL;
			}
			CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE {
				if (cefClientDelegate.get())
					return cefClientDelegate->GetRequestHandler();
				return this;
			}
			bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
				CefProcessId source_process,
				CefRefPtr<CefProcessMessage> message) OVERRIDE {
				std::string name = message->GetName();
				CefRefPtr<CefListValue> arguments = message->GetArgumentList();
				if (name == "jscefappuiif_newctx")
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
				}else if (name == "jscefappuiif_delctxs")
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
				}else if (name == "jscefappuiif_front_exec")
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

			virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefContextMenuParams> params,
				CefRefPtr<CefMenuModel> model)
			{
				// Hide context menu
				model->Clear();
			}

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
				bool* no_javascript_access) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefLifeSpanHandler> handler = cefClientDelegate->GetLifeSpanHandler();
					if (handler.get()) {
						return handler->OnBeforePopup(browser, frame, target_url, target_frame_name, target_disposition, user_gesture, popupFeatures, windowInfo, client, settings, no_javascript_access);
					}
				}
				return false;
			}
			void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE
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
			bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE
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
			void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE
			{
				m_browser = NULL;
				if (cefClientDelegate.get()) {
					CefRefPtr<CefLifeSpanHandler> handler = cefClientDelegate->GetLifeSpanHandler();
					if (handler.get()) {
						handler->OnBeforeClose(browser);
					}
				}
			}
			// ========== CefRequestHandler ==========
			virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				bool user_gesture,
				bool is_redirect) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnBeforeBrowse(browser, frame, request, user_gesture, is_redirect);
					}
				}
				return false;
			}
			virtual bool OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				const CefString& target_url,
				CefRequestHandler::WindowOpenDisposition target_disposition,
				bool user_gesture) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnOpenURLFromTab(browser, frame, target_url, target_disposition, user_gesture);
					}
				}
				return false;
			}
			virtual ReturnValue OnBeforeResourceLoad(
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				CefRefPtr<CefRequestCallback> callback) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnBeforeResourceLoad(browser, frame, request, callback);
					}
				}
				return app_->cef_resource_manager()->OnBeforeResourceLoad(browser, frame, request, callback);
			}
			virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->GetResourceHandler(browser, frame, request);
					}
				}
				return app_->cef_resource_manager()->GetResourceHandler(browser, frame, request);
			}
			virtual void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				CefRefPtr<CefResponse> response,
				CefString& new_url) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						handler->OnResourceRedirect(browser, frame, request, response, new_url);
					}
				}
			}
			virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				CefRefPtr<CefResponse> response) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnResourceResponse(browser, frame, request, response);
					}
				}
				return false;
			}
			virtual CefRefPtr<CefResponseFilter> GetResourceResponseFilter(
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				CefRefPtr<CefResponse> response) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->GetResourceResponseFilter(browser, frame, request, response);
					}
				}
				return NULL;
			}
			virtual void OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				CefRefPtr<CefResponse> response,
				URLRequestStatus status,
				int64 received_content_length) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnResourceLoadComplete(browser, frame, request, response, status, received_content_length);
					}
				}
			}
			virtual bool GetAuthCredentials(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				bool isProxy,
				const CefString& host,
				int port,
				const CefString& realm,
				const CefString& scheme,
				CefRefPtr<CefAuthCallback> callback) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->GetAuthCredentials(browser, frame, isProxy, host, port, realm, scheme, callback);
					}
				}
				return false;
			}
			virtual bool CanGetCookies(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->CanGetCookies(browser, frame, request);
					}
				}
				return true;
			}
			virtual bool CanSetCookie(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				const CefCookie& cookie) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->CanSetCookie(browser, frame, request, cookie);
					}
				}
				return true;
			}
			virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
				const CefString& origin_url,
				int64 new_size,
				CefRefPtr<CefRequestCallback> callback) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnQuotaRequest(browser, origin_url, new_size, callback);
					}
				}
				return false;
			}
			virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
				const CefString& url,
				bool& allow_os_execution) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnProtocolExecution(browser, url, allow_os_execution);
					}
				}
			}
			virtual bool OnCertificateError(CefRefPtr<CefBrowser> browser,
				cef_errorcode_t cert_error,
				const CefString& request_url,
				CefRefPtr<CefSSLInfo> ssl_info,
				CefRefPtr<CefRequestCallback> callback) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnCertificateError(browser, cert_error, request_url, ssl_info, callback);
					}
				}
				return false;
			}
			virtual bool OnSelectClientCertificate(
				CefRefPtr<CefBrowser> browser,
				bool isProxy,
				const CefString& host,
				int port,
				const X509CertificateList& certificates,
				CefRefPtr<CefSelectClientCertificateCallback> callback) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnSelectClientCertificate(browser, isProxy, host, port, certificates, callback);
					}
				}
				return false;
			}
			virtual void OnPluginCrashed(CefRefPtr<CefBrowser> browser,
				const CefString& plugin_path) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnPluginCrashed(browser, plugin_path);
					}
				}
			}
			virtual void OnRenderViewReady(CefRefPtr<CefBrowser> browser) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnRenderViewReady(browser);
					}
				}
			}
			virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
				TerminationStatus status) OVERRIDE {
				if (cefClientDelegate.get()) {
					CefRefPtr<CefRequestHandler> handler = cefClientDelegate->GetRequestHandler();
					if (handler.get()) {
						return handler->OnRenderProcessTerminated(browser, status);
					}
				}
			}

			void loadUrl(const std::string& url)
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
			
			std::string swapLoadUrl()
			{
				std::string result;
				m_show_lock.Acquire();
				result = m_show_url;
				m_show_lock.Release();
				return result;
			}

			bool loadCefRequest(const CefRefPtr<CefRequest>& request)
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
		};
		
		CBrowserWindow::CBrowserWindow(CefRefPtr<CefBrowserApp> browser_app, CefRefPtr<CefAppUIInterfaceHandlerFactory> appUIInterfaceHandlerFactory, CefRefPtr<CefClient> cefClientDelegate, CWnd* pParent) : CFrameWnd()
		{
			m_clientHandler = new CefBrowserClient(browser_app, appUIInterfaceHandlerFactory, cefClientDelegate);
		}

		CBrowserWindow::~CBrowserWindow()
		{
			if (m_clientHandler->m_browser != NULL)
			{
				m_clientHandler->m_browser->StopLoad();
				m_clientHandler->m_browser->GetHost()->CloseBrowser(true);
			}
		}

		LRESULT CBrowserWindow::OnLoadCef(WPARAM wParam, LPARAM lParam)
		{
			bool res;
			RECT rectDesktop = { 0 };
			RECT rectScreenSize;

			CefBrowserSettings cefBrowserSettings;
			CefWindowInfo cefWindowinfo;

			GetClientRect(&rectDesktop);

			rectScreenSize.left = 0;
			rectScreenSize.top = 0;
			rectScreenSize.right = rectDesktop.right - rectDesktop.left;
			rectScreenSize.bottom = rectDesktop.bottom - rectDesktop.top;

			m_view_webview.Create(NULL, WS_CHILD | WS_VISIBLE, rectScreenSize, this);
			m_view_webview.GetClientRect(&rectScreenSize);

			cefWindowinfo.SetAsChild(m_view_webview.GetSafeHwnd(), rectScreenSize);
			res = CefBrowserHost::CreateBrowser(cefWindowinfo, m_clientHandler, m_clientHandler->swapLoadUrl(), cefBrowserSettings, NULL);
			return 0;
		}

		LRESULT CBrowserWindow::OnDisplayChange(WPARAM wParam, LPARAM lParam)
		{
			return 0;
		}

		BOOL CBrowserWindow::PreTranslateMessage(MSG * pMsg)
		{
			if (pMsg->message == WM_KEYDOWN)
			{
				if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
				{
					return TRUE;                // Do not process further
				}
			}

			return __super::PreTranslateMessage(pMsg);
		}

		afx_msg void CBrowserWindow::OnSize(UINT nType, int cx, int cy)
		{
			RECT rect = { 0 };
			__super::OnSize(nType, cx, cy);
			GetClientRect(&rect);
			if(m_view_webview.GetSafeHwnd())
				m_view_webview.SetWindowPos(&wndTop, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
			if (m_clientHandler->m_browser != nullptr)
			{
				HWND hwnd = m_clientHandler->m_browser->GetHost()->GetWindowHandle();
				::SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
			}
		}

		afx_msg void CBrowserWindow::OnClose()
		{
			if (m_clientHandler->m_browser.get()) {
				m_clientHandler->m_browser->GetHost()->CloseBrowser(true);
			}
			__super::OnClose();
		}


		CefRefPtr<CefBrowser> CBrowserWindow::getCefBrowser()
		{
			return m_clientHandler->m_browser;
		}

		CefRefPtr<CefBrowser> CBrowserWindow::getCefBrowserWait(DWORD dwTimeout)
		{
			if (dwTimeout != INFINITE) {
				if (!m_clientHandler->eventBrowserIsReady->TimedWait(dwTimeout))
					return NULL;
			} else {
				m_clientHandler->eventBrowserIsReady->Wait();
			}
			return m_clientHandler->m_browser;
		}

		CefRefPtr<CefFrame> CBrowserWindow::getCefFrameWait(DWORD dwTimeout)
		{
			if (dwTimeout != INFINITE) {
				if (!m_clientHandler->eventBrowserIsReady->TimedWait(dwTimeout))
					return NULL;
				if (!m_clientHandler->eventFrameIsReady->TimedWait(dwTimeout))
					return NULL;
			} else {
				m_clientHandler->eventBrowserIsReady->TimedWait(dwTimeout);
				m_clientHandler->eventFrameIsReady->Wait();
			}
			return m_clientHandler->m_browser->GetMainFrame();
		}

		void CBrowserWindow::loadURL(const std::string& url)
		{
			m_clientHandler->loadUrl(url);
		}

		void CBrowserWindow::loadURL(const char* url, int len)
		{
			if (len < 0)
				len = strlen(url);
			m_clientHandler->loadUrl(std::string(url, len));
		}

		bool CBrowserWindow::loadCefRequest(const CefRefPtr<CefRequest> & request)
		{
			return m_clientHandler->loadCefRequest(request);
		}

		CBrowserWindow* createBrowserWindow(CefRefPtr<CefBrowserApp> browser_app, CefRefPtr<CefAppUIInterfaceHandlerFactory> appUIInterfaceHandlerFactory, CefRefPtr<CefClient> cefClientDelegate, CWnd* pParent)
		{
			return new CBrowserWindow(browser_app, appUIInterfaceHandlerFactory, cefClientDelegate, pParent);
		}
	} // namespace browser
} // namespace JsCefHelper
