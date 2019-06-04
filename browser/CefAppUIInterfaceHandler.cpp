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
 * @file CefAppUIInterfaceHandler.cpp
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#include "CefAppUIInterfaceHandler.h"

#include <include/cef_browser.h>
#include <include/cef_thread.h>
#include <include/cef_waitable_event.h>

#include "CefBrowserApp.h"

namespace JsCefHelper {
	namespace browser {
		class CefAppUIInterfaceHandler::ExecuteResultHandlerImpl : public ExecuteResultHandler
		{
			IMPLEMENT_REFCOUNTING(ExecuteResultHandlerImpl);
		public:
			bool returned;
			CefRefPtr<CefBrowser> browser;
			CefRefPtr<CefProcessMessage> message;

			ExecuteResultHandlerImpl(CefRefPtr<CefBrowser> &browser, const std::string& lookupKey, int callid)
			{
				CefRefPtr<CefListValue> msgArgs;
				this->returned = false;
				this->browser = browser;
				this->message = CefProcessMessage::Create("JsCefHelperif_app_call");
				msgArgs = this->message->GetArgumentList();
				msgArgs->SetSize(5);
				msgArgs->SetString(0, lookupKey);
				msgArgs->SetString(1, "onExecuteResponse");
				msgArgs->SetInt(2, callid);
			}
			void resolve(CefRefPtr<CefValue> retval) override
			{
				CefRefPtr<CefListValue> msgArgs = message->GetArgumentList();
				if (this->returned)
					return;
				this->returned = true;
				msgArgs->SetString(3, "resolve");
				msgArgs->SetValue(4, retval);
				this->browser->SendProcessMessage(PID_RENDERER, this->message);

			}
			void reject(CefRefPtr<CefValue> retval) override
			{
				CefRefPtr<CefListValue> msgArgs = this->message->GetArgumentList();
				if (this->returned)
					return;
				this->returned = true;
				msgArgs->SetString(3, "reject");
				msgArgs->SetValue(4, retval);
				this->browser->SendProcessMessage(PID_RENDERER, this->message);
			}
		};
		class CefAppUIInterfaceHandler::FrontExecuteFutureImpl : public FrontExecuteFuture
		{
			IMPLEMENT_REFCOUNTING(FrontExecuteFutureImpl);
		private:
			CefRefPtr<CefWaitableEvent> doneEvent;
		public:
			int32_t m_callid;
			std::string m_resultType;
			CefRefPtr<CefValue> m_retval;

			FrontExecuteFutureImpl()
			{
				this->doneEvent = CefWaitableEvent::CreateWaitableEvent(false, false);
			}
			bool wait(int timeoutms) override
			{
				if (timeoutms < 0)
					this->doneEvent->Wait();
				else
					return this->doneEvent->TimedWait(timeoutms);
				return true;
			}
			std::string resultType() const override
			{
				return this->m_resultType;
			}
			CefRefPtr<CefValue> returnValue() const override
			{
				return this->m_retval;
			}
			void done(const std::string& type, CefRefPtr<CefValue> retval)
			{
				m_resultType = type;
				m_retval = retval;
				this->doneEvent->Signal();
			}
		};
		class CefAppUIInterfaceHandler::InitTask : public CefTask
		{
			IMPLEMENT_REFCOUNTING(InitTask);
		private:
			CefRefPtr<CefAppUIInterfaceHandler> handler;
			CefRefPtr<CefBrowser> browser;
			std::string lookupKey;
		public:
			InitTask(CefRefPtr<CefAppUIInterfaceHandler> handler, CefRefPtr<CefBrowser> browser, const std::string& lookupKey)
			{
				this->handler = handler;
				this->browser = browser;
				this->lookupKey = lookupKey;
			}

			void Execute() override {
				bool result = this->handler->initRunner();
				CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("JsCefHelperif_app_call");
				CefRefPtr<CefListValue> msgArgs = message->GetArgumentList();
				msgArgs->SetSize(2);
				msgArgs->SetString(0, lookupKey);
				msgArgs->SetString(1, "onInit");
				this->browser->SendProcessMessage(PID_RENDERER, message);
			}
		};
		class CefAppUIInterfaceHandler::ExecuteTask : public CefTask
		{
			IMPLEMENT_REFCOUNTING(ExecuteTask);
		private:
			CefRefPtr<CefAppUIInterfaceHandler> handler;
			CefRefPtr<CefBrowser> browser;
			std::string lookupKey;
			int type;
			CefRefPtr<CefListValue> arguments;
			int argBeginIndex;
		public:
			ExecuteTask(CefRefPtr<CefAppUIInterfaceHandler> handler, CefRefPtr<CefBrowser> browser, const std::string& lookupKey, int type, CefRefPtr<CefListValue> &arguments, int argBeginIndex)
			{
				this->handler = handler;
				this->browser = browser;
				this->lookupKey = lookupKey;
				this->type = type;
				this->arguments = arguments;
				this->argBeginIndex = argBeginIndex;
			}

			void Execute() override {
				bool result;
				bool asyncReturn = false;

				CefRefPtr<ExecuteResultHandlerImpl> resultHandler = new ExecuteResultHandlerImpl(this->browser, this->lookupKey, arguments->GetInt(argBeginIndex + 0));

				if (this->type == 0)
				{
					result = this->handler->controlExecuteRunner(arguments->GetString(argBeginIndex + 1), arguments, argBeginIndex + 2, asyncReturn, resultHandler);
				} else {
					result = this->handler->executeRunner(arguments->GetString(argBeginIndex + 1), arguments, argBeginIndex + 2, asyncReturn, resultHandler);
				}

				if (!result)
				{
					resultHandler->reject(CefValue::Create());
				}else if(!asyncReturn)
				{
					resultHandler->resolve(CefValue::Create());
				}
			}
		};

		CefAppUIInterfaceHandler::CefAppUIInterfaceHandler()
		{
			frontExecuteCallIdCount = 0;
		}

		CefAppUIInterfaceHandler::~CefAppUIInterfaceHandler()
		{
		}

		void CefAppUIInterfaceHandler::attach(CefRefPtr<CefBrowser> browser, const std::string& uniqueKey)
		{
			this->executeThread = CefThread::CreateThread("JsAppUIInterface-ExecuteThread");
			this->browser = browser;
			this->uniqueKey = uniqueKey;
			this->executeThread->GetTaskRunner()->PostTask(new InitTask(this, this->browser, this->uniqueKey));
		}
		void CefAppUIInterfaceHandler::detach()
		{
			this->browser = NULL;
			if (this->executeThread.get()) {
				this->executeThread->Stop();
				this->executeThread = NULL;
			}
		}
		void CefAppUIInterfaceHandler::onFrontExecuteRequest(const std::string& methodName, CefRefPtr<CefListValue> arguments, int argBeginIndex)
		{
			if (methodName == "controlExecuteRequest")
			{
				CefRefPtr<CefListValue> argumentsClone = arguments->Copy();
				this->executeThread->GetTaskRunner()->PostTask(new ExecuteTask(this, this->browser, this->uniqueKey, 0, argumentsClone, argBeginIndex));
			}
			else if (methodName == "executeRequest")
			{
				CefRefPtr<CefListValue> argumentsClone = arguments->Copy();
				this->executeThread->GetTaskRunner()->PostTask(new ExecuteTask(this, this->browser, this->uniqueKey, 1, argumentsClone, argBeginIndex));
			}
			else if (methodName == "frontExecuteResponse")
			{
				int32_t callid = arguments->GetInt(argBeginIndex + 0);
				CefRefPtr<FrontExecuteFutureImpl> future;
				frontExecuteCallIdLock.Acquire();
				std::map<int32_t, CefRefPtr<FrontExecuteFutureImpl> >::iterator iterCallCtx = frontExecuteCallCtxs.find(callid);
				if (iterCallCtx != frontExecuteCallCtxs.end())
				{
					future = iterCallCtx->second;
					frontExecuteCallCtxs.erase(iterCallCtx);
				}
				frontExecuteCallIdLock.Release();
				if (future.get())
				{
					future->done(arguments->GetString(argBeginIndex + 1), arguments->GetValue(argBeginIndex + 2));
				}
			}
		}
		CefRefPtr<FrontExecuteFuture> CefAppUIInterfaceHandler::frontExecute(const std::string& method, CefRefPtr<CefListValue>& arguments)
		{
			CefRefPtr<FrontExecuteFutureImpl> future = new FrontExecuteFutureImpl();
			CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("JsCefHelperif_app_call");
			CefRefPtr<CefListValue> msgArgs = message->GetArgumentList();
			int i;
			frontExecuteCallIdLock.Acquire();
			future->m_callid = ++frontExecuteCallIdCount;
			frontExecuteCallCtxs[future->m_callid] = future;
			frontExecuteCallIdLock.Release();
			msgArgs->SetSize(4 + (arguments.get() ? arguments->GetSize() : 0));
			msgArgs->SetString(0, this->uniqueKey);
			msgArgs->SetString(1, "onFrontExecuteRequest");
			msgArgs->SetInt(2, future->m_callid);
			msgArgs->SetString(3, method);
			if (arguments.get()) {
				for (i = 0; i < arguments->GetSize(); i++)
				{
					msgArgs->SetValue(4 + i, arguments->GetValue(i));
				}
			}
			this->browser->SendProcessMessage(PID_RENDERER, message);
			return future;
		}
		bool CefAppUIInterfaceHandler::contextChanged(const std::string& type, const std::string& data)
		{
			CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("JsCefHelperif_app_call");
			CefRefPtr<CefListValue> msgArgs = message->GetArgumentList();
			int i;
			msgArgs->SetSize(4);
			msgArgs->SetString(0, this->uniqueKey);
			msgArgs->SetString(1, "onContextChanged");
			msgArgs->SetString(2, type);
			msgArgs->SetString(3, data);
			return this->browser->SendProcessMessage(PID_RENDERER, message);
		}
	} // namespace browser
} // namespace JsCefHelper
