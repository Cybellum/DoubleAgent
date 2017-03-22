#pragma once

/* Includes ******************************************************************/
#include <Windows.h>
#include "Status.h"

/* Function Declarations *****************************************************/
/*
 * Receives a file path and returns its directory path
 * The directory path must be freed via HeapFree
 */
DOUBLEAGENT_STATUS PATH_GetDirectory(IN PCWSTR pcwszFilePath, OUT PWSTR *ppwszDirPath);
/*
 * Receives two file paths and combines them into a single valid path
 * The combined path must be freed via HeapFree
 */
DOUBLEAGENT_STATUS PATH_Combine(IN PCWSTR pcwszPath1, IN PCWSTR pcwszPath2, OUT PWSTR *ppwszCombined);
