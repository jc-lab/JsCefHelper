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
 * @file CefRendererApp.h
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#ifndef __JSCEFHELPER_RENDERER_CEFRENDERERAPP_H__
#define __JSCEFHELPER_RENDERER_CEFRENDERERAPP_H__
#pragma once

#include <include/cef_base.h>
#include <include/base/cef_lock.h>
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_context_menu_handler.h>
#include <include/cef_process_message.h>

#include <map>
#include <list>

#include "CefAppUIInterfaceV8Handler.h"

namespace JsCefHelper {
	namespace renderer {

		class CefAppUIInterfaceV8Handler;

		class CefRendererApp : public CefApp, public CefRenderProcessHandler {
		public:
			typedef CefRendererApp super;
		
		private:
			class CefBrowserExternalV8Handler;

			typedef std::map<std::string, CefRefPtr<CefAppUIInterfaceV8Handler> > interface_view_map_t;
			typedef std::map<void*, interface_view_map_t > interface_context_map_t;
			typedef std::map<std::string, CefRefPtr<CefAppUIInterfaceV8Handler> > interface_lookup_map_t;
			interface_context_map_t m_interfaces;
			std::map<std::string, CefRefPtr<CefAppUIInterfaceV8Handler> > m_interfaceLookup;

		public:
			std::string m_createAppUIInterfaceFunctionName;

		public:
			CefRendererApp(const std::string& createAppUIInterfaceFunctionName);
			virtual ~CefRendererApp();
			void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
			void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;

			virtual void Lock() = 0;
			virtual void Unlock() = 0;

			// CefApp methods.
			CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {
				return this;
			}

			virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) OVERRIDE;

			virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) OVERRIDE;

			virtual bool OnProcessMessageReceived(
				CefRefPtr<CefBrowser> browser,
				CefProcessId source_process,
				CefRefPtr<CefProcessMessage> message) OVERRIDE;

		private:
			CefRefPtr<CefAppUIInterfaceV8Handler> createInterface(CefRefPtr<CefBrowser> browser, void* contextId, const std::string& viewName);
		};

		class CefRendererAppImpl : public CefRendererApp {
			IMPLEMENT_REFCOUNTING(CefRendererAppImpl);
			IMPLEMENT_LOCKING(CefRendererAppImpl);
			DISALLOW_COPY_AND_ASSIGN(CefRendererAppImpl);

		public:
			CefRendererAppImpl(const std::string& createAppUIInterfaceFunctionName = "createJsAppUIInterface") :
				CefRendererApp(createAppUIInterfaceFunctionName)
			{}
			virtual ~CefRendererAppImpl() {}
		};

	} // namespace renderer
} // namespace JsCefHelper

#endif /* __CEFCLIENNT_CEFRENDERERAPP_H__ */
