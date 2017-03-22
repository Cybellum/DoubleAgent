#pragma once

/* Includes ******************************************************************/
#include <Windows.h>
#include "Status.h"

/* Function Declarations *****************************************************/
/*
 * Installs an application verifier for the process
 */
DOUBLEAGENT_STATUS VERIFIER_Install(IN PCWSTR pcwszProcessName, IN PCWSTR pcwszVrfDllName, IN PCWSTR pcwszVrfDllPathX86, IN PCWSTR pcwszVrfDllPathX64);
/*
 * In some cases (application crash, exception, etc.) the install\uninstall functions may accidentally leave the machine in an undefined state
 * Repairs the machine to its original state
 */
DOUBLEAGENT_STATUS VERIFIER_Repair(VOID);
/*
 * Uninstalls the application verifier from the process
 */
VOID VERIFIER_Uninstall(IN PCWSTR pcwszProcessName, IN PCWSTR pcwszVrfDllName);
