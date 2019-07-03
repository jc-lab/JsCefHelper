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
 * @file BrowserWindowInterface.h
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#pragma once
#ifndef __JSCEFHELPER_BROWSER_BROWSERWINDOWINTERFACE_H__
#define __JSCEFHELPER_BROWSER_BROWSERWINDOWINTERFACE_H__

#include <include/cef_browser.h>
#include <include/cef_waitable_event.h>
#include <include/base/cef_lock.h>
#include <include/cef_client.h>

#include "CefAppUIInterfaceHandlerFactory.h"

class CWnd;

namespace JsCefHelper {
	namespace browser {
		class CBrowserWindow;
		class CefBrowserApp;

		class IBrowserWindow
		{
		public:
			virtual void doLoadCef() = 0;

			//virtual afx_msg LRESULT OnLoadCef(WPARAM wParam = NULL, LPARAM lParam = NULL) = 0;

			virtual CefRefPtr<CefBrowser> getCefBrowser() = 0;
			virtual CefRefPtr<CefBrowser> getCefBrowserWait(DWORD dwTimeout = INFINITE) = 0;
			virtual CefRefPtr<CefFrame> getCefFrameWait(DWORD dwTimeout = INFINITE) = 0;
			virtual void loadURL(const char* url, int len = -1) = 0;
			virtual void loadURL(const std::string& url) = 0;
			virtual bool loadCefRequest(const CefRefPtr<CefRequest> & request) = 0;
		};

		CBrowserWindow* createBrowserWindow(CefRefPtr<CefBrowserApp> browser_app, CefRefPtr<CefAppUIInterfaceHandlerFactory> appUIInterfaceHandlerFactory = NULL, CefRefPtr<CefClient> cefClientDelegate = NULL, CWnd* pParent = NULL);

	} // namespace browser
} // namespace JsCefHelper

#endif /* __JSCEFHELPER_BROWSER_BROWSERWINDOWINTERFACE_H__ */
