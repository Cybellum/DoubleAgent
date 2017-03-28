/* Includes ******************************************************************/
#include <Windows.h>
#include "Status.h"
#include "Memory.h"

/* Public Function Definitions ***********************************************/
void* memset(void *pvBuffer, int iValue, size_t nBufferLen)
{
	SIZE_T nBufferIndex = 0;

	/* Sets the buffer */
	for (nBufferIndex = 0; nBufferIndex < nBufferLen; nBufferIndex++)
	{
		((PBYTE)pvBuffer)[nBufferIndex] = (BYTE)iValue;
	}

	return pvBuffer;
}
