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
 * @file CefRendererApp.cpp
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#include "CefRendererApp.h"

#include <include/wrapper/cef_closure_task.h>
#include <include/base/cef_bind.h>

#include "../sha256.h"

#include <stdio.h>

#if !defined(_MSC_VER)
#define sprintf_s sprintf
#endif

#ifdef DBF_PRINTF
#undef DBF_PRINTF
#endif

#ifdef _DEBUG
#define DBF_PRINTF(format, ...) if(stderr) { fprintf(stderr, format "\n", __VA_ARGS__); }
#else
#define DBF_PRINTF(format, ...)
#endif

namespace JsCefHelper {
	namespace renderer {
		class CefRendererApp::CefBrowserExternalV8Handler : public CefV8Handler {
		private:
			CefRendererApp *app;
			CefRefPtr<CefBrowser> browser;
			void *contextId;
			std::string createAppUIInterfaceFunctionName;

			IMPLEMENT_REFCOUNTING(CefBrowserExternalV8Handler);
			DISALLOW_COPY_AND_ASSIGN(CefBrowserExternalV8Handler);

		public:
			CefBrowserExternalV8Handler(CefRendererApp *app, CefRefPtr<CefBrowser> browser, void *contextId, const std::string& createAppUIInterfaceFunctionName) {
				this->app = app;
				this->browser = browser;
				this->contextId = contextId;
				this->createAppUIInterfaceFunctionName = createAppUIInterfaceFunctionName;
			}

			bool Execute(const CefString& name,
				CefRefPtr<CefV8Value> object,
				const CefV8ValueList& arguments,
				CefRefPtr<CefV8Value>& retval,
				CefString& exception) OVERRIDE
			{
				if (name == createAppUIInterfaceFunctionName)
				{
					if (arguments.size() < 1) {
						exception = "Need view name (first argument is not set)";
						return false;
					}
					CefRefPtr<CefV8Value> arg0 = arguments.at(0);
					if (!arg0->IsString()) {
						exception = "Need view name (first argument is not string)";
						return false;
					}
					CefRefPtr<CefAppUIInterfaceV8Handler> interfaceHandler = this->app->createInterface(this->browser, this->contextId, arg0->GetStringValue());
					if (!interfaceHandler) {
						exception = "Already created interface";
					} else {
						retval = interfaceHandler->createJsObject();
					}
					return true;
				}
				return false;
			}
		};

		CefRendererApp::CefRendererApp(const std::string& createAppUIInterfaceFunctionName)
		{
			m_createAppUIInterfaceFunctionName = createAppUIInterfaceFunctionName;
		}

		CefRendererApp::~CefRendererApp()
		{
		}

		void CefRendererApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
		{
			DBF_PRINTF("OnBrowserCreated[%p]", browser.get());
		}

		void CefRendererApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
		{
			DBF_PRINTF("OnBrowserDestroyed[%p]", browser.get());
		}

		void CefRendererApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context)
		{
			CefRefPtr<CefV8Value> global = context->GetGlobal();
			CefRefPtr<CefV8Value> external = CefV8Value::CreateObject(NULL, NULL);
			CefRefPtr<CefBrowserExternalV8Handler> funcCreateAppUIInterfaceHandler = new CefBrowserExternalV8Handler(this, browser, (void*)frame->GetIdentifier(), m_createAppUIInterfaceFunctionName);
			CefRefPtr<CefV8Value> funcCreateAppUIInterface = CefV8Value::CreateFunction(m_createAppUIInterfaceFunctionName, funcCreateAppUIInterfaceHandler);
			external->SetValue(m_createAppUIInterfaceFunctionName, funcCreateAppUIInterface, V8_PROPERTY_ATTRIBUTE_NONE);
			global->SetValue("external", external, V8_PROPERTY_ATTRIBUTE_NONE);

			DBF_PRINTF("OnContextCreated[%p/%p/%lld]", context.get(), frame.get(), frame->GetIdentifier());
		}
		void CefRendererApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context)
		{
			DBF_PRINTF("OnContextReleased[%p/%p/%lld]", context.get(), frame.get(), frame->GetIdentifier());
			Lock();
			interface_context_map_t::iterator contextIter = m_interfaces.find((void*)frame->GetIdentifier());
			if (contextIter != m_interfaces.end()) {
				CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("jscefappuiif_delctxs");
				CefRefPtr<CefListValue> messageArgs = message->GetArgumentList();
				size_t index = 0;
				messageArgs->SetSize(contextIter->second.size());
				for (interface_view_map_t::iterator ifIter = contextIter->second.begin(); ifIter != contextIter->second.end(); ifIter++)
				{
					messageArgs->SetString(index++, ifIter->second->getUniqueKey());
				}
				browser->SendProcessMessage(PID_BROWSER, message);
				m_interfaces.erase(contextIter);
			}
			Unlock();
		}

		/**
		 * message arguments
		 * 
		 * callJsCefAppUIInterface
		 * - lookupKey
		 * - [arguments]
		 */
		bool CefRendererApp::OnProcessMessageReceived(
			CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message)
		{
			std::string name = message->GetName();
			CefRefPtr<CefListValue> arguments = message->GetArgumentList();
			if (name == "jscefappuiif_app_call")
			{
				if (arguments->GetSize() >= 2)
				{
					std::string lookupKey = arguments->GetString(0);
					std::string methodName = arguments->GetString(1);
					Lock();
					interface_lookup_map_t::iterator iterInterface = m_interfaceLookup.find(lookupKey);
					if (iterInterface != m_interfaceLookup.end())
					{
						iterInterface->second->callAppUIInterfaceImpl(iterInterface->second, methodName, arguments, 2);
					}
					else {
						DBF_PRINTF("OnProcessMessageReceived[%p]: callJsCefAppUIInterface: AppUIInterface[%s] destoyed", browser.get(), lookupKey.c_str());
					}
					Unlock();
				}
				return true;
			}
			return false;
		}

		CefRefPtr<CefAppUIInterfaceV8Handler> CefRendererApp::createInterface(CefRefPtr<CefBrowser> browser, void *contextId, const std::string& viewName)
		{
			CefRefPtr<CefAppUIInterfaceV8Handler> interfaceHandler;
			SHA256_CTX keyHashCtx;
			unsigned char keyHash[32] = {0};
			const char* HexStrings = "01234567890ABCDEF";
			char keybuf[128];
			int i;
			sha256_init(&keyHashCtx);
			sha256_update(&keyHashCtx, (const uint8_t*)& contextId, sizeof(contextId));
			sha256_update(&keyHashCtx, (const uint8_t*)viewName.c_str(), viewName.length());
			sha256_final(&keyHashCtx, keyHash);

			for (i = 0; i < 32; i++) {
				keybuf[i * 2 + 0] = HexStrings[(keyHash[i] >> 4) & 0xf];
				keybuf[i * 2 + 1] = HexStrings[(keyHash[i] >> 0) & 0xf];
			}
			keybuf[64] = 0;

			this->Lock();

			interface_context_map_t::iterator contextIter = m_interfaces.find(contextId);
			if (contextIter != m_interfaces.end())
			{
				interface_view_map_t::iterator viewIter = contextIter->second.find(viewName);
				if (viewIter != contextIter->second.end())
				{
					interfaceHandler = viewIter->second;
				}
			}

			if (!interfaceHandler)
			{
				// If not exists
				interfaceHandler = new CefAppUIInterfaceV8Handler(browser, viewName, keybuf);
				m_interfaces[contextId][viewName] = interfaceHandler;
				m_interfaceLookup[interfaceHandler->getUniqueKey()] = interfaceHandler;
			} else {
				// If exists DO NOT create AGAIN
				interfaceHandler = NULL;
			}

			this->Unlock();

			return interfaceHandler;
		}
	} // namespace renderer
} // namespace JsCefHelper
