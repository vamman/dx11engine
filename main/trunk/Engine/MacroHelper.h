#ifndef MACRO_HELPER_H_
#define MACRO_HELPER_H_

#define ACTION_THROUGH_ASSERT(action, messageBody)					\
{																	\
	HRESULT hr = S_OK;												\
	bool res = true;												\
	hr = (action);													\
	if(FAILED(hr))													\
	{																\
		HWND hwnd = FindWindow(L"Engine", NULL);					\
		MessageBox(hwnd, messageBody, L"ASSERT !", MB_OK);			\
		abort();													\
	}																\
}

#define ASSERT(statement, messageBody)								\
{																	\
	if (!statement)													\
	{																\
		HWND hwnd = FindWindow(L"Engine", NULL);					\
		MessageBox(hwnd, messageBody, L"ASSERT !", MB_OK);			\
		abort();													\
	}																\
}

#define V_RETURN(statement, messageHeader, messageBody)				\
{																	\
	HRESULT hr = (statement);										\
	if( FAILED(hr) )												\
	{																\
		MessageBox(hwnd, messageBody, messageHeader, MB_OK);		\
		return false;												\
	}																\
}

#define  CREATE_ORDINARY_OBJ_WITH_MAT(obj, objectNmae, objectFilePath, materialToAssign, vertexType)												\
{																																		\
	obj = ModelFactory::GetInstance()->CreateOrdinaryModel(mD3D->GetDevice(), hwnd, objectNmae, objectFilePath, vertexType);						\
	obj->SetMaterial(MaterialFactory::GetInstance()->GetMaterialByName(materialToAssign));												\
}

#define  CREATE_INSTANCED_OBJ_WITH_MAT(objectNmae, objectFilePath, materialToAssign, numberOfObjects)									\
{																																		\
	ModelObject* object = new ModelObject();																							\
	object = ModelFactory::GetInstance()->CreateInstancedModel(mD3D->GetDevice(), hwnd, objectNmae, objectFilePath, numberOfObjects);	\
	object->SetMaterial(MaterialFactory::GetInstance()->GetMaterialByName(materialToAssign));											\
}

#define SHUTDOWN_OBJ(obj)		\
{								\
	if(obj)						\
	{							\
		obj->Shutdown();		\
		delete obj;				\
		obj = 0;				\
	}							\
}

#define  WRITE_SENTENCE(D3D, numericValue, stringValue, posX, posY, colorR, colorG, colorB, sentenceNumber)		\
{																												\
	char tempString[10];																						\
	char dataString[50];																						\
	_itoa_s((int) numericValue, tempString, 10);																\
	strcpy_s(dataString, stringValue);																			\
	strcat_s(dataString, tempString);																			\
	result = m_Text->AddSentence(D3D, dataString, posX, posY, colorR, colorG, colorB, sentenceNumber);			\
	if(!result) { return false; }																				\
}

#endif