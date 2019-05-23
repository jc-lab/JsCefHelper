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
 * @file CefInternalResourceProvider.cpp
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */

#include "CefInternalResourceProvider.h"

namespace JsCefHelper {
	namespace browser {

		CefInternalResourceProvider::CefInternalResourceProvider(const std::string& url_prefix) : 
			url_prefix_(url_prefix)
		{
		}

		bool CefInternalResourceProvider::OnRequest(scoped_refptr<CefResourceManager::Request> request)
		{
			CEF_REQUIRE_IO_THREAD();

			CefRefPtr<CefResourceHandler> handler = NULL;

			const std::string& url = request->url();
			if (url.find(url_prefix_) != 0L) {
				return handler;
			}
			const std::string& relative_path = url.substr(url_prefix_.length());
			if (!relative_path.empty()) {
				handler = ReadResource(relative_path);
				if (handler.get())
				{
					request->Continue(handler);
				}
			}

			return handler;
		}
		
		void CefInternalResourceProvider::OnRequestCanceled(scoped_refptr<CefResourceManager::Request> request)
		{
		}

		std::string CefInternalResourceProvider::ResolveMimeFromUrl(const std::string& url)
		{
			std::basic_string<char> mimetype = "text/plain";
			size_t url_extpos = url.rfind('.');
			if (url_extpos != url.npos)
			{
				std::basic_string<char> strExtName = url.substr(url_extpos + 1);
				if ((_stricmp(strExtName.c_str(), "htm") == 0) || (_stricmp(strExtName.c_str(), "html") == 0))
				{
					mimetype = "text/html";
				}
				else if ((_stricmp(strExtName.c_str(), "bin") == 0))
				{
					mimetype = "application/octet-stream";
				}
				else if ((_stricmp(strExtName.c_str(), "css") == 0))
				{
					mimetype = "text/css";
				}
				else if ((_stricmp(strExtName.c_str(), "js") == 0))
				{
					mimetype = "application/js";
				}
				else if ((_stricmp(strExtName.c_str(), "gif") == 0))
				{
					mimetype = "image/gif";
				}
				else if ((_stricmp(strExtName.c_str(), "ico") == 0))
				{
					mimetype = "image/x-icon";
				}
				else if ((_stricmp(strExtName.c_str(), "jpg") == 0) || (_stricmp(strExtName.c_str(), "jpeg") == 0))
				{
					mimetype = "image/jpeg";
				}
				else if ((_stricmp(strExtName.c_str(), "png") == 0))
				{
					mimetype = "image/png";
				}
				else if ((_stricmp(strExtName.c_str(), "webp") == 0))
				{
					mimetype = "image/webp";
				}
				else if ((_stricmp(strExtName.c_str(), "weba") == 0))
				{
					mimetype = "audio/webm";
				}
				else if ((_stricmp(strExtName.c_str(), "webm") == 0))
				{
					mimetype = "video/webm";
				}
				else if ((_stricmp(strExtName.c_str(), "mpeg") == 0))
				{
					mimetype = "video/mpeg";
				}
				else if ((_stricmp(strExtName.c_str(), "tif") == 0) || (_stricmp(strExtName.c_str(), "tiff") == 0))
				{
					mimetype = "image/tiff";
				}
				else if ((_stricmp(strExtName.c_str(), "ttf") == 0))
				{
					mimetype = "application/x-font-ttf";
				}
				else if ((_stricmp(strExtName.c_str(), "woff") == 0))
				{
					mimetype = "application/x-font-woff";
				}
				else if ((_stricmp(strExtName.c_str(), "woff2") == 0))
				{
					mimetype = "application/x-font-woff2";
				}
				else if ((_stricmp(strExtName.c_str(), "xml") == 0))
				{
					mimetype = "application/xml";
				}
				else if ((_stricmp(strExtName.c_str(), "json") == 0))
				{
					mimetype = "application/json";
				}
				else {
					mimetype = "text/plain";
				}
			}
			return mimetype;
		}
		
		int CefInternalResourceProvider::urldecode(const char* source, char* dest, int destbufsize)
		{
			int num = 0, i, hexv, index = 0;
			int retval = 0;
			while (*source && index < destbufsize)
			{
				if (*source == '%')
				{
					num = 0;
					retval = 0;
					for (i = 0; i < 2; i++)
					{
						*source++;
						if (*(source) < ':')
						{
							num = *(source)-48;
						}
						else if (*(source) > '@' && *(source) < '[')
						{
							num = (*(source)-'A') + 10;
						}
						else
						{
							num = (*(source)-'a') + 10;
						}

						if ((16 * (1 - i)))
							num = (num * 16);
						retval += num;
					}
					dest[index] = retval;
					index++;
				}
				else
				{
					dest[index] = *source;
					index++;
				}
				*source++;
			}
			return index;
		}

	} // namespace browser
} // namespace JsCefHelper
