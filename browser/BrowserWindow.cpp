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

		afx_msg LRESULT CBrowserWindow::OnLoadCef(WPARAM wParam, LPARAM lParam)
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

		CefRefPtr<CefBrowserClient> CBrowserWindow::getCefBrowserClient() const
		{
			return m_clientHandler;
		}

		CBrowserWindow* createBrowserWindow(CefRefPtr<CefBrowserApp> browser_app, CefRefPtr<CefAppUIInterfaceHandlerFactory> appUIInterfaceHandlerFactory, CefRefPtr<CefClient> cefClientDelegate, CWnd* pParent)
		{
			return new CBrowserWindow(browser_app, appUIInterfaceHandlerFactory, cefClientDelegate, pParent);
		}
	} // namespace browser
} // namespace JsCefHelper
