#ifndef _BASICRESOURCE_H_
#define _BASICRESOURCE_H_

#include <string>
#include "windows.h"

#define MAX_RESOURCE_NAME 100

class BasicResource
{
	public:
		BasicResource(void);
		virtual			~BasicResource(void);
		virtual void	Shutdown() = 0;
		virtual char*	GetResourceName();
		virtual void	SetResourceName(const char* name);

	private:
		char m_ResourceName[MAX_RESOURCE_NAME];
};

#endif