#define  _WIN32_DCOM
#include <iostream>
#include <vector>
#include <comdef.h>
#include <Windows.h>
#include <Wbemidl.h>
#include <strsafe.h>
#include <tchar.h>
#include <string>
#include <iomanip>
#include <atlstr.h>
using namespace std;

#pragma comment(lib, "wbemuuid.lib")

SERVICE_STATUS ServiceStatus = { 0 };

SERVICE_STATUS_HANDLE hServiceStatusHandle = NULL;
int flag = 0;
HANDLE hServiceEvent = NULL;
void Mointer();
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpArgv);
void WINAPI ServiceControlHandler(DWORD dwControl);
void ServiceReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
void ServiceInit(DWORD dwArgc, LPTSTR* lpArgv);
void ServiceInstall(LPWSTR arg);
void ServiceDelete(LPWSTR arg);
void ServiceStart(LPWSTR arg);
void ServiceStop(LPWSTR arg);
DWORD WINAPI  processthread(LPVOID);

struct Details
{
	BSTR Process_Name;
	string PID;
	DWORD Process_Id;
	DWORD Vbytes;
};

DWORD main() {
	string Input;
	LPWSTR SERVICENAME = LPWSTR(TEXT("Myservice"));
	
Looping:
	cout << "1.Install \n2.Uninstall \n3.Start \n4.Stop \n5.Exit " << endl;
	cout << "Enter your input : ";
	getline(cin >> ws, Input);

	BOOL bStServiceCtrlDispatcher = FALSE;
	if (Input == "Install" || Input == "1" || Input == "install") {
		ServiceInstall(SERVICENAME);
	}
	else if (Input == "Uninstall" || Input == "2" || Input == "uninstall") {
		ServiceDelete(SERVICENAME);
	}
	else if (Input == "Start" || Input == "3" || Input == "start") {
		ServiceStart(SERVICENAME);
	}
	else if (Input == "Stop" || Input == "4" || Input == "stop") {
		ServiceStop(SERVICENAME);
	}
	else if (Input == "Exit" || Input == "5" || Input == "exit") {
		exit(0);
	}
	else {
		SERVICE_TABLE_ENTRY DispatchTable[] = {
		 {(LPWSTR)SERVICENAME,(LPSERVICE_MAIN_FUNCTION)ServiceMain},
		 { NULL, NULL }
		};

		bStServiceCtrlDispatcher = StartServiceCtrlDispatcher(
			DispatchTable);
		if (FALSE == bStServiceCtrlDispatcher)
		{
			cout << "StartServiceCtrlDispatcher Failed" << endl;
			cout << "Error Code - " << GetLastError() << endl;
		}
		else
		{
			cout << "StartServiceCtrlDispatcher Success" << endl;
		}
	}

	cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
	goto Looping;

}

void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpArgv)
{
	try {
		cout << "ServiceMain Start" << endl;
		BOOL bServiceStatus = FALSE;
		hServiceStatusHandle = RegisterServiceCtrlHandler(
			*lpArgv,
			ServiceControlHandler);
		if (NULL == hServiceStatusHandle)
		{
			cout << " RegisterServiceCtrlHandler Failed " << endl;
			cout << "Error Code - " << GetLastError() << endl;
		}
		else
		{
			cout << "RegisterServiceCtrlHandler Success" << endl;
		}
		ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		ServiceStatus.dwServiceSpecificExitCode = 0;
		ServiceReportStatus(
			SERVICE_START_PENDING,
			NO_ERROR,
			3000);
		bServiceStatus = SetServiceStatus(
			hServiceStatusHandle,
			&ServiceStatus);
		if (FALSE == bServiceStatus)
		{
			cout << "Service Status Initial Setup FAILED = " <<
				GetLastError() << endl;
		}
		else
		{
			cout << "Service Status initial Setup SUCCESS" << endl;
		}
		ServiceInit(dwArgc, lpArgv);
		cout << "ServiceMain End" << endl;
	}
	catch (...) {
		printf("Error code(%d)", GetLastError());
	}

}

void WINAPI ServiceControlHandler(DWORD dwControl)
{

	try {
		cout << "ServiceControlHandler" << endl;
		switch (dwControl)
		{
		case SERVICE_CONTROL_STOP:
		{

			ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
			cout << "Service stopped" << endl;
			break;
		}
		default:
			break;
		}
		cout << "ServiceControlHandler" << endl;


	}

	catch (...) {
		printf("Error code(%d)", GetLastError());
	}



}

void ServiceInit(DWORD dwArgc, LPTSTR* lpArgv)
{

	try {
		cout << "ServiceInit Start" << endl;

		hServiceEvent = CreateEvent(
			NULL,
			TRUE,
			FALSE,
			NULL);
		if (NULL == hServiceEvent)
		{

			ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
		}
		else
		{

			ServiceReportStatus(SERVICE_RUNNING, NO_ERROR, 0);
		}

		while (1)
		{

			HANDLE hThread = CreateThread(
				NULL,
				0,
				processthread,
				NULL,
				0,
				NULL);
			WaitForSingleObject(hThread, INFINITE);
			ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
			return;
		}
		cout << "ServiceInit End" << endl;
	}
	catch (...) {
		printf("Error code(%d)", GetLastError());
	}
}

void FileWrite(Details details, DWORD flag , HANDLE file)
{
	DWORD written = 0;
	
}

DWORD Monitor(vector<Details> &ovector,vector<Details> &pvector)
{
	HRESULT hres;
	IWbemLocator* pLoc = 0;
	IWbemServices* pSvc = 0;
	IEnumWbemClassObject* pEnumerator = NULL;
	IWbemClassObject* pclsObj;
	ULONG uReturn = 0;
	VARIANT vtProp;
	VARIANT vtPID;
	string PID;
	DWORD index =0;
	HANDLE file;
	DWORD written = 0;
	static DWORD count ;
	Details details;

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		cout << "Failed to initialize COM library. "
			<< "Error code = 0x"
			<< hex << hres << endl;
		return 1;
	}

	hres = CoInitializeSecurity(
		NULL,
		-1,               
		NULL,    
		NULL,    
		RPC_C_AUTHN_LEVEL_DEFAULT,    
		RPC_C_IMP_LEVEL_IMPERSONATE,  
		NULL,
		EOAC_NONE,        
		NULL              
	);


	if (FAILED(hres))
	{
		cout << "Failed to initialize security. "
			<< "Error code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return 1;
	}

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&pLoc);

	if (FAILED(hres))
	{
		cout << "Failed to create IWbemLocator object. "
			<< "Error code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return 1;       
	}

	hres = pLoc->ConnectServer(

		_bstr_t(L"ROOT\\CIMV2"), 
		NULL,                    
		NULL,                    
		0,                       
		NULL,                                     
		0,                              
		0,                       
		&pSvc                    
	);

	if (FAILED(hres))
	{
		cout << "Could not connect. Error code = 0x"
			<< hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return 1;                
	}

	cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

	hres = CoSetProxyBlanket(

		pSvc,                         
		RPC_C_AUTHN_WINNT,            
		RPC_C_AUTHZ_NONE,             
		NULL,                         
		RPC_C_AUTHN_LEVEL_CALL,       
		RPC_C_IMP_LEVEL_IMPERSONATE,  
		NULL,                          
		EOAC_NONE                     
	);

	if (FAILED(hres))
	{
		cout << "Could not set proxy blanket. Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;               
	}

	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_Process"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		cout << "Query for processes failed. "
			<< "Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;               
	}

	else
	{
		file = CreateFile(
			TEXT("My_checking.txt"),
			FILE_APPEND_DATA,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (file == INVALID_HANDLE_VALUE) {

			printf("File not created (%d)", GetLastError());
		}

		while (pEnumerator)
		{
			hres = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);

			if (0 == uReturn)
			{
				break;
			}
			hres = pclsObj->Get(L"Name",0,&vtProp,0,0);
			hres = pclsObj->Get(L"ProcessId",0,&vtPID,0,0);
			PID = to_string(vtPID.intVal);

			details.PID = PID;
			details.Process_Id = vtPID.intVal;
			details.Process_Name = vtProp.bstrVal;
			pvector.push_back(details);

			if (count == 0) {
				ovector.push_back(details);
			}
			else {
				loop:
				if (ovector[index].Process_Id != pvector[index].Process_Id)
				{
					if (ovector[index].Process_Id > pvector[index].Process_Id)
					{
						//delete ovector
						WriteFile(
							file,
							details.PID.c_str(),
							details.PID.length(),
							&written,
							NULL);
						WriteFile(
							file,
							" - ",
							strlen(" - "),
							&written,
							NULL);
						WriteFile(
							file,
							details.Process_Name,
							SysStringLen(details.Process_Name),
							&written,
							NULL);
						WriteFile(
							file,
							" - Started",
							strlen(" - Started"),
							&written,
							NULL);
						WriteFile(
							file,
							"\n",
							strlen("\n"),
							&written,
							NULL);
						ovector.erase(ovector.begin() + index);
					}
					else {
						//insert pvector to ovector
						WriteFile(
							file,
							details.PID.c_str(),
							details.PID.length(),
							&written,
							NULL);
						WriteFile(
							file,
							" - ",
							strlen(" - "),
							&written,
							NULL);
						WriteFile(
							file,
							details.Process_Name,
							SysStringLen(details.Process_Name),
							&written,
							NULL);
						WriteFile(
							file,
							" - Stopped",
							strlen(" - Stopped"),
							&written,
							NULL);
						WriteFile(
							file,
							"\n",
							strlen("\n"),
							&written,
							NULL);
					pvector.insert(pvector.begin() + index ,details );
						//file write//started
					}
					goto loop;
				}
			}
			index++;
			VariantClear(&vtProp);
			VariantClear(&vtPID);

			pclsObj->Release();
			pclsObj = NULL;
		}
	}
	pvector.clear();
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	count++;
	CoUninitialize();
	return 0;
}

DWORD WINAPI  processthread(LPVOID lpParam){
	try {
		vector<Details> ovector;
		vector<Details> pvector;
		while (WaitForSingleObject(hServiceEvent, 0) != WAIT_OBJECT_0)
		{
			Monitor(ovector,pvector);
			Sleep(10000);
		}
		return ERROR_SUCCESS;
	}
	catch (...) {
		printf("Error code(%d)", GetLastError());
	}
}

void ServiceReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	try {

		cout << "ServiceReportStatus Start" << endl;

		static DWORD dwCheckPoint = 1;
		BOOL bSetServiceStatus = FALSE;

		ServiceStatus.dwCurrentState = dwCurrentState;
		ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
		ServiceStatus.dwWaitHint = dwWaitHint;

		if (dwCurrentState == SERVICE_START_PENDING)
		{
			ServiceStatus.dwControlsAccepted = 0;
		}
		else
		{
			ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		}

		if ((dwCurrentState == SERVICE_RUNNING) ||
			(dwCurrentState == SERVICE_STOPPED))
		{
			ServiceStatus.dwCheckPoint = 0;
		}
		else
		{
			ServiceStatus.dwCheckPoint = dwCheckPoint++;
		}

		bSetServiceStatus = SetServiceStatus(
			hServiceStatusHandle,
			&ServiceStatus);
		if (FALSE == bSetServiceStatus)
		{
			cout << "Service Status FAILED " << endl;
			cout << "Error No - " << GetLastError() << endl;
		}
		else
		{
			cout << "Service Status SUCCESS" << endl;
		}
		cout << "ServiceReportStatus End" << endl;
	}
	catch (...) {
		printf("Error code(%d)", GetLastError());
	}
}

void ServiceInstall(LPWSTR arg)
{
	// cout << "ServiceInstall Start" << endl;
	try {
		SC_HANDLE hScOpenSCManager = NULL,
			hScCreateService = NULL;
		DWORD dwGetModuleFileName = 0;
		TCHAR szPath[MAX_PATH];

		dwGetModuleFileName = GetModuleFileName(
			NULL,
			szPath,
			MAX_PATH);
		if (0 == dwGetModuleFileName)
		{
			cout << "Service Installation Failed " << endl;
			cout << "Error No = " << GetLastError() << endl;
		}
		else
		{
			cout << "Successfully install the File\n" << endl;
		}


		hScOpenSCManager = OpenSCManager(
			NULL,
			NULL,
			SC_MANAGER_ALL_ACCESS);
		if (NULL == hScOpenSCManager)
		{
			cout << "OpenSCManager Failed " << endl;
			cout << "Error No - " << GetLastError() << endl;
		}
		else
		{
			cout << "OpenSCManager Success" << endl;
		}


		hScCreateService = CreateService(
			hScOpenSCManager,
			arg,
			arg,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_NORMAL,
			szPath,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);
		if (NULL == hScCreateService)
		{
			cout << "CreateService Failed " << endl;
			cout << "Error No - " << GetLastError() << endl;
			CloseServiceHandle(hScOpenSCManager);
		}
		else
		{
			cout << "CreateService Success" << endl;
		}

		CloseServiceHandle(hScCreateService);
		CloseServiceHandle(hScOpenSCManager);
	}
	catch (...) {
		printf("Error code(%d)", GetLastError());
	}
}

void ServiceDelete(LPWSTR arg)
{
	SC_HANDLE hScOpenSCManager = NULL;
	SC_HANDLE hScOpenService = NULL;
	BOOL bDeleteService = FALSE;

	hScOpenSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);
	if (NULL == hScOpenSCManager)
	{


		cout << "OpenSCManager Failed " << endl;
		cout << "Error No - " << GetLastError() << endl;
	}
	else
	{
		cout << "OpenSCManager Success" << endl;
	}

	hScOpenService = OpenService(
		hScOpenSCManager,
		arg,
		SERVICE_ALL_ACCESS);
	if (NULL == hScOpenService)
	{
		cout << "OpenService Failed " << endl;
		cout << "Error No- " << GetLastError() << endl;
	}
	else
	{
		cout << "OpenService Success " << endl;
	}

	bDeleteService = DeleteService(hScOpenService);
	if (FALSE == bDeleteService)
	{
		cout << "Delete Service Failed " << endl;
		cout << "Error No- " << GetLastError() << endl;
	}
	else
	{
		cout << "Delete Service Success" << endl;
	}


	CloseServiceHandle(hScOpenService);
	CloseServiceHandle(hScOpenSCManager);
	cout << "ServiceDelete End" << endl;
}

void ServiceStart(LPWSTR arg)
{
	cout << "Inside ServiceStart function" << endl;

	BOOL bStartService = FALSE,
		bQueryServiceStatus = FALSE;
	SC_HANDLE hOpenSCManager = NULL,
		hOpenService = NULL;

	DWORD dwBytesNeeded;
	SERVICE_STATUS_PROCESS SvcStatusProcess;

	hOpenSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);
	if (NULL == hOpenSCManager)
	{
		cout << "hOpenSCManager Failed " << endl;
		cout << "Error No = " << GetLastError() << endl;
	}
	else
	{
		cout << "hOpenSCManager Success" << endl;
	}

	hOpenService = OpenService(
		hOpenSCManager,
		arg,
		SC_MANAGER_ALL_ACCESS);
	if (NULL == hOpenService)
	{
		cout << "OpenService Failed " << endl;
		cout << "Error No = " << GetLastError() << endl;
		CloseServiceHandle(hOpenSCManager);
	}
	else
	{
		cout << "OpenService Success" << endl;
	}

	bQueryServiceStatus = QueryServiceStatusEx(
		hOpenService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&SvcStatusProcess,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded);

	if (FALSE == bQueryServiceStatus)
	{
		cout << "QueryService Failed " << endl;
		cout << "Error No = " << GetLastError() << endl;
	}
	else
	{
		cout << "QueryService Success" << endl;
	}

	if ((SvcStatusProcess.dwCurrentState != SERVICE_STOPPED) &&
		(SvcStatusProcess.dwCurrentState != SERVICE_STOP_PENDING))
	{
		cout << " service is already running" << endl;
	}
	else
	{
		cout << "Service is Already Stopped" << endl;
	}

	while (SvcStatusProcess.dwCurrentState == SERVICE_STOP_PENDING)
	{

		bQueryServiceStatus = QueryServiceStatusEx(
			hOpenService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&SvcStatusProcess,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded);

		if (FALSE == bQueryServiceStatus)
		{
			cout << "QueryService Failed" << endl;
			cout << "Error No = " << GetLastError() << endl;
			CloseServiceHandle(hOpenService);
			CloseServiceHandle(hOpenSCManager);
		}
		else
		{
			cout << "QueryService Success" << endl;
		}

	}
	bStartService = StartService(
		hOpenService,
		NULL,
		NULL);
	if (FALSE == bStartService)
	{
		cout << "StartService Failed " << endl;
		cout << "Error No = " << GetLastError() << endl;
		CloseServiceHandle(hOpenService);
		CloseServiceHandle(hOpenSCManager);
	}
	else
	{
		cout << "StartService Success" << endl;
	}

	bQueryServiceStatus = QueryServiceStatusEx(
		hOpenService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&SvcStatusProcess,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded);

	if (FALSE == bQueryServiceStatus)
	{
		cout << "QueryService Failed " << endl;
		cout << "Error No = " << GetLastError() << endl;
		CloseServiceHandle(hOpenService);
		CloseServiceHandle(hOpenSCManager);
	}
	else
	{
		cout << "QueryService Success" << endl;
	}


	if (SvcStatusProcess.dwCurrentState == SERVICE_RUNNING)
	{
		cout << "Service Started Running..." << endl;
	}
	else
	{
		cout << "Service Running Failed" << endl;
		cout << "Error No = " << GetLastError() << endl;
		CloseServiceHandle(hOpenService);
		CloseServiceHandle(hOpenSCManager);
	}

	CloseServiceHandle(hOpenService);
	CloseServiceHandle(hOpenSCManager);
	cout << "ServiceStart end" << endl;
}

void ServiceStop(LPWSTR arg)
{
	cout << "Inside Service Stop" << endl;
	SERVICE_STATUS_PROCESS SvcStatusProcess;
	SC_HANDLE hScOpenSCManager = NULL,
		hScOpenService = NULL;
	BOOL bQueryServiceStatus = TRUE,
		bControlService = TRUE;
	DWORD dwBytesNeeded;
	hScOpenSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);
	if (NULL == hScOpenSCManager)
	{
		cout << "OpenSCManager Failed " << endl;
		cout << "Error No = " << GetLastError() << endl;
	}
	else
	{
		cout << "OpenSCManager Success" << endl;
	}
	hScOpenService = OpenService(
		hScOpenSCManager,
		arg,
		SC_MANAGER_ALL_ACCESS);
	if (NULL == hScOpenService)
	{
		cout << "OpenService Failed " << endl;
		cout << "Error No = " << GetLastError() << endl;
		CloseServiceHandle(hScOpenSCManager);
	}
	else
	{
		cout << "OpenService Success" << endl;
	}

	bQueryServiceStatus = QueryServiceStatusEx(
		hScOpenService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&SvcStatusProcess,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded);
	if (FALSE == bQueryServiceStatus)
	{
		cout << "QueryService Failed " << endl;
		cout << "Error No = " << GetLastError() << endl;
		CloseServiceHandle(hScOpenService);
		CloseServiceHandle(hScOpenSCManager);
	}
	else
	{
		cout << "QueryService Success" << endl;
	}

	bControlService = ControlService(
		hScOpenService,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&SvcStatusProcess);
	if (TRUE == bControlService)
	{
		cout << "Control Service Success" << endl;
	}
	else
	{
		cout << "Control Service Failed " << endl;
		cout << "Error No = " << GetLastError() << endl;
		CloseServiceHandle(hScOpenService);
		CloseServiceHandle(hScOpenSCManager);
	}


	while (SvcStatusProcess.dwCurrentState != SERVICE_STOPPED)
	{

		bQueryServiceStatus = QueryServiceStatusEx(
			hScOpenService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&SvcStatusProcess,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded);

		if (TRUE == bQueryServiceStatus)
		{
			cout << "QueryService Failed" << endl;
			cout << "Error No = " << GetLastError() << endl;
			CloseServiceHandle(hScOpenService);
			CloseServiceHandle(hScOpenSCManager);
		}
		else
		{
			cout << "QueryService Success" << endl;
		}

		if (SvcStatusProcess.dwCurrentState == SERVICE_STOPPED)
		{
			cout << "Service Stopped Successfully" << endl;
			break;
		}
		else
		{
			cout << "Service Stopped Failed" << endl;
			cout << "Error No = " << GetLastError() << endl;
			CloseServiceHandle(hScOpenService);
			CloseServiceHandle(hScOpenSCManager);
		}
	}
	CloseServiceHandle(hScOpenService);
	CloseServiceHandle(hScOpenSCManager);
	cout << "Service Stop" << endl;
}
