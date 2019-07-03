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
 * @file BrowserWindow.h
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#pragma once
#ifndef __JSCEFHELPER_BROWSER_BROWSERWINDOW_H__
#define __JSCEFHELPER_BROWSER_BROWSERWINDOW_H__

#include "BrowserWindowInterface.h"

#include <include/cef_browser.h>
#include <include/cef_waitable_event.h>
#include <include/base/cef_lock.h>
#include <include/cef_client.h>

#include "CefAppUIInterfaceHandlerFactory.h"

#include "CefBrowserClient.h"

// Need #include <afxwin.h> in cpp file

namespace JsCefHelper {
	namespace browser {
		class CefBrowserApp;

		class CBrowserWindow : public CFrameWnd, public IBrowserWindow
		{
			DECLARE_DYNAMIC(CBrowserWindow)

		public:
			static const LONG WM_DOLOADCEF;
			static const LONG WM_CUSTOM;

		private:
			CStatic m_view_webview;

			CefRefPtr<CefBrowserClient> m_clientHandler;
			CefRefPtr<CefAppUIInterfaceHandlerFactory> appUIInterfaceHandlerFactory;

		public:
			CBrowserWindow(CefRefPtr<CefBrowserApp> browser_app, CefRefPtr<CefAppUIInterfaceHandlerFactory> appUIInterfaceHandlerFactory = NULL, CefRefPtr<CefClient> cefClientDelegate = NULL, CWnd* pParent = NULL);
			virtual ~CBrowserWindow();

		protected:
			DECLARE_MESSAGE_MAP()

		public:
			void doLoadCef() override
			{
				SendMessage(WM_DOLOADCEF, NULL, NULL);
			}

			virtual BOOL PreTranslateMessage(MSG* pMsg);
			virtual afx_msg void OnSize(UINT nType, int cx, int cy);
			virtual afx_msg void OnClose();

			virtual afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
			afx_msg LRESULT OnLoadCef(WPARAM wParam = NULL, LPARAM lParam = NULL);// override;

			CefRefPtr<CefBrowser> getCefBrowser() override;
			CefRefPtr<CefBrowser> getCefBrowserWait(DWORD dwTimeout = INFINITE) override;
			CefRefPtr<CefFrame> getCefFrameWait(DWORD dwTimeout = INFINITE) override;
			void loadURL(const char* url, int len = -1) override;
			void loadURL(const std::string& url) override;
			bool loadCefRequest(const CefRefPtr<CefRequest> & request) override;

			CefRefPtr<CefBrowserClient> getCefBrowserClient() const;
		};

	} // namespace browser
} // namespace JsCefHelper

#endif /* __JSCEFHELPER_BROWSER_BROWSERWINDOW_H__ */
