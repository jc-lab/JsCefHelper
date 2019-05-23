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
 * @file CefClientAppBrowser.cpp
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */

#include "CefBrowserApp.h"
#include <include/base/cef_bind.h>

#if defined(OS_WIN) && OS_WIN
#include <Windows.h>
#include <tchar.h>
#include <string.h>
#endif

namespace JsCefHelper {
	namespace browser {

		CefBrowserApp::CefBrowserApp()
		{
			cef_resource_manager_ = new CefResourceManager();
			cef_inited_ = false;
		}

		CefBrowserApp::~CefBrowserApp()
		{
			if (cef_inited_) {
				::CefShutdown();
				cef_inited_ = false;
			}
		}

		bool CefBrowserApp::init(const CefMainArgs& args, const CefSettings* base_settings)
		{
			CefSettings cefSettings;
			void* windows_sandbox_info = NULL;

			if (base_settings)
				cefSettings = *base_settings;

			cefSettings.multi_threaded_message_loop = 0; //needed for MFC projects (unless we want to implement custom message-loop)
			cefSettings.pack_loading_disabled = 0;
#if defined(CEF_USE_SANDBOX) && CEF_USE_SANDBOX
			// Manage the life span of the sandbox information object. This is necessary
			// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
			CefScopedSandboxInfo scoped_sandbox;
			windows_sandbox_info = scoped_sandbox.sandbox_info();
			cefSettings.no_sandbox = 0;
#else
			cefSettings.no_sandbox = 1;
#endif
			cefSettings.remote_debugging_port = 31000;

#if defined(OS_WIN) && OS_WIN
			{
				TCHAR szFilePath[MAX_PATH];
				TCHAR* pDotPos;
				DWORD dwLen = ::GetModuleFileName(NULL, szFilePath, MAX_PATH);
				if (!dwLen)
				{
					pDotPos = _tcsrchr(szFilePath, _T('.'));
					if (pDotPos)
						*pDotPos = 0;
					_tcscat_s(szFilePath, _T("\\"));

#ifdef _UNICODE
					cef_string_from_wide(szFilePath, _tcslen(szFilePath), &cefSettings.resources_dir_path);
#else
					cef_string_from_utf8(szFilePath, strlen(szFilePath), &cefSettings.resources_dir_path);
#endif
				}
			}
#endif

			if (::CefInitialize(args, cefSettings, this, windows_sandbox_info))
			{
				cef_inited_ = true;
				return true;
			}

			return false;
		}

		bool CefBrowserApp::init(const CefMainArgs& args, const CefSettings& settings, void* windows_sandbox_info)
		{
			if (::CefInitialize(args, settings, this, windows_sandbox_info))
			{
				cef_inited_ = true;
				return true;
			}

			return false;
		}

		void CefBrowserApp::CefResourceManagerAddProvider_(CefResourceManager* manager, CefResourceManager::Provider* provider, int order, const std::string& identifier)
		{
			if (!CefCurrentlyOn(TID_IO)) {
				// Execute on the browser IO thread.
				CefPostTask(TID_IO, base::Bind(CefResourceManagerAddProvider_, manager, provider, order, identifier));
				return;
			}
			manager->AddProvider(provider, order, identifier);
		}

	} // namespace browser
} // namespace JsCefHelper
