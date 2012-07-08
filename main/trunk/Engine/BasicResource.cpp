#include "BasicResource.h"

BasicResource::BasicResource(void)
{
}


BasicResource::~BasicResource(void)
{
}

void BasicResource::SetResourceName(const char* name)
{
	strcpy_s(m_ResourceName, MAX_RESOURCE_NAME, name);
}

char* BasicResource::GetResourceName()
{
	return m_ResourceName;
}