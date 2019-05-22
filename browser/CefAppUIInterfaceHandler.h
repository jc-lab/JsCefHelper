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
 * @file CefAppUIInterfaceHandler.h
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#pragma once
#ifndef __JSCEFHELPER_BROWSER_CEFAPPUIINTERFACEHANDLER_H__
#define __JSCEFHELPER_BROWSER_CEFAPPUIINTERFACEHANDLER_H__

#include <include/cef_base.h>
#include <include/cef_values.h>
#include <include/cef_thread.h>
#include <include/base/cef_lock.h>

#include <map>
#include <string>

class CefBrowser;

namespace JsCefHelper {
	namespace browser {

		class ExecuteResultHandler : public virtual CefBaseRefCounted
		{
		public:
			virtual void resolve(CefRefPtr<CefValue> retval) = 0;
			virtual void reject(CefRefPtr<CefValue> retval) = 0;
		};

		class FrontExecuteFuture : public virtual CefBaseRefCounted
		{
		public:
			virtual bool wait(int timeoutms) = 0;
			virtual std::string resultType() const = 0; // resolve/reject/undefined
			virtual CefRefPtr<CefValue> returnValue() const = 0;

			bool isResolved() const {
				return (resultType().compare("resolve")) == 0;
			}
			bool isRejected() const {
				return (resultType().compare("reject")) == 0;
			}
			bool isUndefined() const {
				return (resultType().compare("undefined")) == 0;
			}
		};

		class CBrowserWindow;
		class CefAppUIInterfaceHandler : public virtual CefBaseRefCounted {
			DISALLOW_COPY_AND_ASSIGN(CefAppUIInterfaceHandler);

			friend class CBrowserWindow;

		private:
			class InitTask;
			class ExecuteTask;
			class ExecuteResultHandlerImpl;
			class FrontExecuteFutureImpl;

			CefRefPtr<CefThread> executeThread;

			CefRefPtr<CefBrowser> browser;
			std::string uniqueKey;

			base::Lock frontExecuteCallIdLock;
			int32_t frontExecuteCallIdCount;
			std::map<int32_t, CefRefPtr<FrontExecuteFutureImpl> > frontExecuteCallCtxs;

			void attach(CefRefPtr<CefBrowser> browser, const std::string& uniqueKey);
			void detach();
			void onFrontExecuteRequest(const std::string& methodName, CefRefPtr<CefListValue> arguments, int argBeginIndex);

		public:
			CefAppUIInterfaceHandler();
			virtual ~CefAppUIInterfaceHandler();

			virtual bool initRunner() { return true; };
			virtual bool executeRunner(const std::string& methodName, CefRefPtr<CefListValue>& arguments, int argBeginIndex, bool &asyncReturn, CefRefPtr<ExecuteResultHandler> resultHandler) = 0;
			virtual bool controlExecuteRunner(const std::string& methodName, CefRefPtr<CefListValue>& arguments, int argBeginIndex, bool& asyncReturn, CefRefPtr<ExecuteResultHandler> resultHandler) = 0;

			CefRefPtr<FrontExecuteFuture> frontExecute(const std::string& method, CefRefPtr<CefListValue>& arguments);
			bool contextChanged(const std::string& type, const std::string& data);
		};

	} // namespace browser
} // namespace JsCefHelper

#endif /* __JSCEFHELPER_BROWSER_CEFAPPUIINTERFACEHANDLER_H__ */
