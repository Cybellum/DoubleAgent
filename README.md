# DoubleAgent
DoubleAgent is a new Zero-Day technique for injecting code and maintaining persistence on a machine (i.e. auto-run).

DoubleAgent can exploit:

* Every Windows version (Windows XP to Windows 10)
* Every Windows architecture (x86 and x64)
* Every Windows user (SYSTEM/Admin/etc.)
* Every target process, including privileged processes (OS/Antivirus/etc.)

DoubleAgent exploits a 15 years old legitimate feature of Windows and therefore cannot be patched.

## Code Injection

DoubleAgent gives the attacker the ability to inject any DLL into any process. The code injection occurs extremely early during the victim’s process boot, giving the attacker full control over the process and no way for the process to protect itself.
The code injection technique is so unique that it’s not detected or blocked by any antivirus.

## Persistency

DoubleAgent can continue injecting code even after reboot making it a perfect persistence technique to “survive” reboots/updates/reinstalls/patches/etc.
Once the attacker decides to inject a DLL into a process, they are forcefully bounded forever. Even if the victim would completely uninstall and reinstall its program, the attacker’s DLL would still be injected every time the process executes.

## Attack Vectors

* Attacking Antivirus & Next Generation Antivirus – Taking full control of any antivirus by injecting code into it while bypassing all of its self-protection mechanism. The attack has been verified and works on all the major antiviruses including but not limited to: Avast, AVG, Avira, Bitdefender, Comodo, ESET, F-Secure, Kaspersky, Malwarebytes, McAfee, Norton, Panda, Quick Heal and Trend Micro.
 For more details, checkout our [Taking Full Control Over Your Antivirus](http://cybellum.com/doubleagent-taking-full-control-antivirus/) article.
 
* Installing Persistent Malware – Installing malware that can “survive” reboots and are automatically executed once the operating system boots.

* Hijacking Permissions – Hijacking the permissions of an existing trusted process to perform malicious operations in disguise of the trusted process. e.g. Exfiltrating data, C&C communication, lateral movement, stealing and encrypting sensitive data.

* Altering Process Behavior – Modifying the behavior of the process. e.g. Installing backdoors, weakening encryption algorithms, etc.

* Attacking Other Users/Sessions – Injecting code to processes of other users/sessions (SYSTEM/Admin/etc.).

## Technical Deep Dive
For more details, checkout our [technical](http://cybellum.com/doubleagentzero-day-code-injection-and-persistence-technique/) article.

## Installation
1. Clone/Download the DoubleAgent source code.
2. Build the [main solution](https://github.com/Cybellum/DoubleAgent) twice, once in x86 and once in x64.
   This step is crucial as it creates both x86 and x64 versions of DoubleAgentDll.dll which is required in order to perform a successful installation.
3. Copy the entire bin folder to the target machine.
4. Execute the installer:
   ```
   Usage:  DoubleAgent.exe install\uninstall\repair process_name
   
   e.g.    DoubleAgent.exe install cmd.exe
   ```
   Note that the 32bit installer (DoubleAgent_x86.exe) can be used both on Windows x86 and Windows x64.
   But the 64bit installer (DoubleAgent_x64.exe) can be used only on Windows x64.
5. The next time the target process loads [DoubleAgentDll.dll](https://github.com/Cybellum/DoubleAgent/tree/master/DoubleAgentDll) would be injected into it.

## Authors
Cybellum Technologies LTD (http://cybellum.com/)
