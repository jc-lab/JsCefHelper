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
 * @file CefAppUIInterfaceV8Handler.cpp
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#include "CefAppUIInterfaceV8Handler.h"

#include <include/wrapper/cef_closure_task.h>
#include <include/base/cef_bind.h>

#include <JsCPPUtils/StringEncoding.h>

#include "../CefValueConverter.h"

#ifdef DBF_PRINTF
#undef DBF_PRINTF
#endif

#define DBF_PRINTF(format, ...) fprintf(stderr, format "\n", __VA_ARGS__)

namespace JsCefHelper {
	namespace renderer {
		CefAppUIInterfaceV8Handler::CefAppUIInterfaceV8Handler(const CefRefPtr<CefBrowser>& browser, const std::string& viewName, const std::string& uniqueKey)
		{
			m_browser = browser;
			m_viewName = viewName;
			m_uniqueKey = uniqueKey;
			DBF_PRINTF("CCommunicatorRendererHandler created\n");
		}

		CefAppUIInterfaceV8Handler::~CefAppUIInterfaceV8Handler()
		{
			DBF_PRINTF("CCommunicatorRendererHandler destroy\n");
		}

		bool CefAppUIInterfaceV8Handler::Execute(const CefString& name,
			CefRefPtr<CefV8Value> object,
			const CefV8ValueList& arguments,
			CefRefPtr<CefV8Value>& retval,
			CefString& exception)
		{
			CefRefPtr<CefProcessMessage> message;
			CefRefPtr<CefListValue> messageArgs;
			std::string utf8Name = name;
			printf("CefAppUIInterfaceV8Handler: Execute: %s\n", utf8Name.c_str());
			if (name == "init")
			{
				m_cefJsContext = CefV8Context::GetCurrentContext();
				m_cefJsFuncs["onInit"] = m_obj->GetValue("onInit");
				m_cefJsFuncs["onContextChanged"] = m_obj->GetValue("onContextChanged");
				m_cefJsFuncs["onExecuteResponse"] = m_obj->GetValue("onExecuteResponse");
				m_cefJsFuncs["onFrontExecuteRequest"] = m_obj->GetValue("onFrontExecuteRequest");
				message = CefProcessMessage::Create("JsCefHelperif_newctx");
				messageArgs = message->GetArgumentList();
				messageArgs->SetSize(2);
				messageArgs->SetString(0, m_uniqueKey);
				messageArgs->SetString(1, m_viewName);
			} else {
				message = CefProcessMessage::Create("JsCefHelperif_front_exec");
				// uniqueKey, methodName[, arguments...]
				messageArgs = message->GetArgumentList();
				messageArgs->SetSize(2 + arguments.size());
				messageArgs->SetString(0, m_uniqueKey);
				messageArgs->SetString(1, name);
				AppendCefV8ValueListToCefListValue(messageArgs, 2, arguments);
			}
			m_browser->SendProcessMessage(PID_BROWSER, message);
			return true;
		};

		CefRefPtr<CefV8Value> CefAppUIInterfaceV8Handler::createJsObject() {
			CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(NULL, NULL);
			obj->SetValue("init", CefV8Value::CreateFunction("init", this), V8_PROPERTY_ATTRIBUTE_NONE);
			obj->SetValue("executeRequest", CefV8Value::CreateFunction("executeRequest", this), V8_PROPERTY_ATTRIBUTE_NONE);
			obj->SetValue("controlExecuteRequest", CefV8Value::CreateFunction("controlExecuteRequest", this), V8_PROPERTY_ATTRIBUTE_NONE);
			obj->SetValue("frontExecuteResponse", CefV8Value::CreateFunction("frontExecuteResponse", this), V8_PROPERTY_ATTRIBUTE_NONE);
			obj->SetValue("saveContext", CefV8Value::CreateFunction("saveContext", this), V8_PROPERTY_ATTRIBUTE_NONE);
			obj->SetValue("onInit", CefV8Value::CreateFunction("onInit", this), V8_PROPERTY_ATTRIBUTE_NONE);
			obj->SetValue("onContextChanged", CefV8Value::CreateFunction("onContextChanged", this), V8_PROPERTY_ATTRIBUTE_NONE);
			obj->SetValue("onExecuteResponse", CefV8Value::CreateFunction("onExecuteResponse", this), V8_PROPERTY_ATTRIBUTE_NONE);
			obj->SetValue("onFrontExecuteRequest", CefV8Value::CreateFunction("onFrontExecuteRequest", this), V8_PROPERTY_ATTRIBUTE_NONE);
			m_obj = obj;
			return obj;
		}

		void CefAppUIInterfaceV8Handler::destoryJsObject()
		{
			m_obj = NULL;
		}

		// onInit()
		// onExecuteResponse(int callid, string restype, object data)
		// onFrontExecuteRequest(int callid, string name, objects ...args)
		// onContextChanged(string type, object data);
		void CefAppUIInterfaceV8Handler::callAppUIInterfaceImpl(const CefRefPtr<CefAppUIInterfaceV8Handler>& rendererInterface, const std::string& name, const CefRefPtr<CefListValue>& arguments, int argBeginIndex)
		{
			int i;
			bool bresult;

			if (!::CefCurrentlyOn(TID_RENDERER)) {
				// Execute on the browser IO thread.
				CefPostTask(TID_RENDERER, base::Bind(callAppUIInterfaceImpl, rendererInterface, name, arguments, argBeginIndex));
				return;
			}

			bresult = rendererInterface->m_cefJsContext->Enter();
			{
				CefV8ValueList args;
				int count = arguments->GetSize() - argBeginIndex;
				for (i = 0; i < count; i++)
				{
					args.push_back(CefValueToCefV8Value(arguments->GetValue(argBeginIndex + i)));
				}
				bresult = rendererInterface->m_cefJsFuncs[name]->ExecuteFunctionWithContext(rendererInterface->m_cefJsContext, rendererInterface->m_obj, args);
			}
			bresult = rendererInterface->m_cefJsContext->Exit();
		}
	} // namespace renderer
} // namespace JsCefHelper
