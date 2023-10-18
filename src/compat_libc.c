/*
	Copyright 2022-2023 Nick Little

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <dlfcn.h>
#include <errno.h>

__attribute__((__weak__)) int __register_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void), void *dso_handle)
{
	static void *pthread_atfork_fn = 0;
	static void *register_atfork_fn = 0;

	if (!pthread_atfork_fn)
	{
		if (!register_atfork_fn)
		{
			pthread_atfork_fn = dlsym(RTLD_DEFAULT, "pthread_atfork");
			register_atfork_fn = dlsym(RTLD_NEXT, "__register_atfork");

			if (!pthread_atfork_fn && !register_atfork_fn)
				return ENOMEM;

			return __register_atfork(prepare, parent, child, dso_handle);
		}

		return ((int (*)(void (*)(void), void (*)(void), void (*)(void), void *))register_atfork_fn)(prepare, parent, child, dso_handle);
	}

	return ((int (*)(void (*)(void), void (*)(void), void (*)(void)))pthread_atfork_fn)(prepare, parent, child);
}
