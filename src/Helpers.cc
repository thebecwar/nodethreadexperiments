#include "Helpers.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

typedef DWORD pid_t;

#elif defined(MACOS)

#else

#include <strings.h>
#define stricmp strcasecmp

#include <dirent.h>

#endif

#include <vector>
#include <string>
#include <sstream>
#include <uv.h>

using namespace v8;

#define BAD_IDEA_MACRO(x) if (stricmp(*val, "#x") == 0) {\
        return x;\
    }

int GetSignal(Local<String>& str)
{
    String::Utf8Value val(str);

#ifdef SIGHUP
    BAD_IDEA_MACRO(SIGHUP)
#endif

#ifdef SIGHUP
        BAD_IDEA_MACRO(SIGHUP);
#endif

#ifdef SIGINT
    BAD_IDEA_MACRO(SIGINT);
#endif

#ifdef SIGQUIT
    BAD_IDEA_MACRO(SIGQUIT);
#endif

#ifdef SIGILL
    BAD_IDEA_MACRO(SIGILL);
#endif

#ifdef SIGTRAP
    BAD_IDEA_MACRO(SIGTRAP);
#endif

#ifdef SIGABRT
    BAD_IDEA_MACRO(SIGABRT);
#endif

#ifdef SIGIOT
# if SIGABRT != SIGIOT
    BAD_IDEA_MACRO(SIGIOT);
# endif
#endif

#ifdef SIGBUS
    BAD_IDEA_MACRO(SIGBUS);
#endif

#ifdef SIGFPE
    BAD_IDEA_MACRO(SIGFPE);
#endif

#ifdef SIGKILL
    BAD_IDEA_MACRO(SIGKILL);
#endif

#ifdef SIGUSR1
    BAD_IDEA_MACRO(SIGUSR1);
#endif

#ifdef SIGSEGV
    BAD_IDEA_MACRO(SIGSEGV);
#endif

#ifdef SIGUSR2
    BAD_IDEA_MACRO(SIGUSR2);
#endif

#ifdef SIGPIPE
    BAD_IDEA_MACRO(SIGPIPE);
#endif

#ifdef SIGALRM
    BAD_IDEA_MACRO(SIGALRM);
#endif

    BAD_IDEA_MACRO(SIGTERM);

#ifdef SIGCHLD
    BAD_IDEA_MACRO(SIGCHLD);
#endif

#ifdef SIGSTKFLT
    BAD_IDEA_MACRO(SIGSTKFLT);
#endif


#ifdef SIGCONT
    BAD_IDEA_MACRO(SIGCONT);
#endif

#ifdef SIGSTOP
    BAD_IDEA_MACRO(SIGSTOP);
#endif

#ifdef SIGTSTP
    BAD_IDEA_MACRO(SIGTSTP);
#endif

#ifdef SIGBREAK
    BAD_IDEA_MACRO(SIGBREAK);
#endif

#ifdef SIGTTIN
    BAD_IDEA_MACRO(SIGTTIN);
#endif

#ifdef SIGTTOU
    BAD_IDEA_MACRO(SIGTTOU);
#endif

#ifdef SIGURG
    BAD_IDEA_MACRO(SIGURG);
#endif

#ifdef SIGXCPU
    BAD_IDEA_MACRO(SIGXCPU);
#endif

#ifdef SIGXFSZ
    BAD_IDEA_MACRO(SIGXFSZ);
#endif

#ifdef SIGVTALRM
    BAD_IDEA_MACRO(SIGVTALRM);
#endif

#ifdef SIGPROF
    BAD_IDEA_MACRO(SIGPROF);
#endif

#ifdef SIGWINCH
    BAD_IDEA_MACRO(SIGWINCH);
#endif

#ifdef SIGIO
    BAD_IDEA_MACRO(SIGIO);
#endif

#ifdef SIGPOLL
# if SIGPOLL != SIGIO
    BAD_IDEA_MACRO(SIGPOLL);
# endif
#endif

#ifdef SIGLOST
# if SIGLOST != SIGABRT
    BAD_IDEA_MACRO(SIGLOST);
# endif
#endif

#ifdef SIGPWR
# if SIGPWR != SIGLOST
    BAD_IDEA_MACRO(SIGPWR);
# endif
#endif

#ifdef SIGINFO
# if !defined(SIGPWR) || SIGINFO != SIGPWR
    BAD_IDEA_MACRO(SIGINFO);
# endif
#endif

#ifdef SIGSYS
    BAD_IDEA_MACRO(SIGSYS);
#endif

#ifdef SIGTERM
    return SIGTERM;
#else
    return 0;
#endif
}

void GetChildProcessIds(const FunctionCallbackInfo<Value>& args)
{
    Isolate* isolate = args.GetIsolate();

    std::vector<pid_t> children;
    pid_t currentProc;

    if (args.Holder().IsEmpty())
    {
        if (args.Length() > 0)
        {
            currentProc = (pid_t)args[0]->IntegerValue();
        }
        else
        {
            args.GetReturnValue().Set(Array::New(isolate, 0));
            return;
        }
    }
    else
    {
        Local<String> pidKey = String::NewFromUtf8(isolate, "pid");
        Local<Value> pid = args.Holder()->Get(pidKey);
        if (pid.IsEmpty())
        {
            args.GetReturnValue().Set(Array::New(isolate, 0));
            return;
        }
        currentProc = static_cast<pid_t>(pid->IntegerValue());
    }

#ifdef WIN32
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    BOOL ok = Process32First(hSnap, &pe);

    if (ok)
    {
        do
        {
            if (pe.th32ParentProcessID == currentProc)
            {
                children.emplace_back(pe.th32ProcessID);
            }
        } while (Process32Next(hSnap, &pe));
    }

    CloseHandle(hSnap);

#else
    // Posix todo
    DIR* proc = opendir("/proc");
    
    dirent* ent = readdir(proc);
    do
    {
        pid_t pid;
        sscanf(ent->d_name, "%d", &pid);
        children.emplace_back(pid);

    } while ((ent = readdir(proc)) != nullptr);



#endif

    Local<Array> result = Array::New(isolate, children.size());
    for (int i = 0; i < children.size(); i++)
    {
        result->Set(i, Integer::NewFromUnsigned(isolate, children[i]));
    }

    args.GetReturnValue().Set(result);
}
void KillChildProcs(const FunctionCallbackInfo<Value>& args)
{
    Isolate* isolate = args.GetIsolate();

    GetChildProcessIds(args);

    int signal = 0;
    if (args.Length() > 0)
    {
        Local<String> sig = args[0]->ToString();
        signal = GetSignal(sig);
    }
    else
    {
        signal = SIGTERM;
    }

    Local<Array> children = Local<Array>::Cast(args.GetReturnValue().Get());
    args.GetReturnValue().SetUndefined();

    Local<Array> result = Array::New(isolate, children->Length());
    if (children->Length() > 0)
    {
        for (uint32_t i = 0; i < children->Length(); i++)
        {
            pid_t pid = static_cast<pid_t>(children->Get(i)->IntegerValue());
            int err = uv_kill(pid, signal);
            result->Set(i, Boolean::New(isolate, (err == 0)));
        }
    }
    args.GetReturnValue().Set(result);
}
void GetProcessInfo(const FunctionCallbackInfo<Value>& args)
{
    // Parameters:
    //  pid : number OR executableName: string
    //          The process id or name of the executable we are trying to find.
    //  allInfo : boolean
    //          Query for additional information beyond the basic information.
    // If they pass undefined, or null, or if the first parameter is a boolean, the function returns an array of all processes.
    // Returns Object:
    // pid - Process ID
    // executableName - Name of the running executable
    // nThreads - Count of threads in the process.
    // threads - thread objects
    //      threadid - Thread ID
    //      priority - Thread priority
    // modules - Modules in the function
    // heaps - Process Heap info

    Isolate* isolate = args.GetIsolate();

    pid_t pid = 0;
    std::string procName;
    bool searchPid = false;
    bool enumAll = false;
    bool allInfo = false;

    if (args.Length() == 2)
    {
        if (!args[0]->IsNumber() && !args[0]->IsNumberObject() &&
            !args[0]->IsString() && !args[0]->IsStringObject() &&
            !args[0]->IsNullOrUndefined())
        {
            isolate->ThrowException(Exception::SyntaxError(String::NewFromUtf8(isolate, "argument 0 -> string or number")));
            return;
        }
        if (!args[1]->IsBoolean() && !args[1]->IsBooleanObject())
        {
            isolate->ThrowException(Exception::SyntaxError(String::NewFromUtf8(isolate, "argument 1 -> boolean")));
            return;
        }

        if (args[0]->IsString() || args[0]->IsStringObject())
        {
            String::Utf8Value val(args[0]->ToString());
            procName = std::string(*val);
        }
        else if (args[0]->IsNumber() || args[0]->IsNumberObject())
        {
            pid = static_cast<pid_t>(args[0]->IntegerValue());
            searchPid = true;
        }
        else if (args[0]->IsNullOrUndefined())
        {
            enumAll = true;
        }
        
        allInfo = args[1]->BooleanValue();
    }
    else if (args.Length() == 1)
    {
        if (args[0]->IsBoolean() || args[0]->IsBooleanObject())
        {
            enumAll = true;
            allInfo = args[0]->BooleanValue();
        }
        else if (args[0]->IsNumber() || args[0]->IsNumberObject())
        {
            pid = static_cast<pid_t>(args[0]->IntegerValue());
            searchPid = true;
        }
        else if (args[0]->IsString() || args[0]->IsStringObject())
        {
            String::Utf8Value val(args[0]->ToString());
            procName = std::string(*val);
        }
        else
        {
            isolate->ThrowException(Exception::SyntaxError(String::NewFromUtf8(isolate, "If one argument is provided it must be a pid, process name, or boolean")));
            return;
        }

    }
    else
    {
        enumAll = true;
    }

    Local<Value> result;
    if (enumAll || !searchPid)
    {
        result = Array::New(isolate);
    }
    else
    {
        result = Object::New(isolate);
    }

    // Object Keys -
    Local<String> pidKey = String::NewFromUtf8(isolate, "pid");
    Local<String> ppidKey = String::NewFromUtf8(isolate, "ppid");
    Local<String> exeNameKey = String::NewFromUtf8(isolate, "executableName");
    Local<String> nThreadsKey = String::NewFromUtf8(isolate, "nThreads");
    Local<String> threadsKey = String::NewFromUtf8(isolate, "threads");
    Local<String> threadIdKey = String::NewFromUtf8(isolate, "threadId");
    Local<String> threadPriorityKey = String::NewFromUtf8(isolate, "priority");
    Local<String> modulesKey = String::NewFromUtf8(isolate, "modules");
    Local<String> moduleBaseAddrKey = String::NewFromUtf8(isolate, "baseAddress");
    Local<String> moduleSizeKey = String::NewFromUtf8(isolate, "size");
    Local<String> moduleHandleKey = String::NewFromUtf8(isolate, "handle");
    Local<String> moduleNameKey = String::NewFromUtf8(isolate, "name");
    Local<String> modulePathKey = String::NewFromUtf8(isolate, "path");

    Local<String> heaps = String::NewFromUtf8(isolate, "heaps");


    // Platform specific enumeration

#ifdef WIN32
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        BOOL ok = Process32First(hSnap, &pe);
        if (ok)
        {
            bool found = false;
            Local<Object> current;
            do
            {
                if (enumAll || !searchPid)
                {
                    current = Object::New(isolate);
                }
                else
                {
                    current = Local<Object>::Cast(result);
                }

                if (enumAll || 
                    (searchPid && pid == pe.th32ProcessID) ||
                    (!searchPid && stricmp(procName.c_str(), pe.szExeFile) == 0))
                {
                    found = true;

                    current->Set(pidKey, Integer::NewFromUnsigned(isolate, pe.th32ProcessID));
                    current->Set(ppidKey, Integer::New(isolate, pe.th32ParentProcessID));
                    current->Set(exeNameKey, String::NewFromUtf8(isolate, pe.szExeFile));
                    current->Set(nThreadsKey, Integer::New(isolate, pe.cntThreads));
                    current->Set(threadsKey, Array::New(isolate, 0));
                    current->Set(modulesKey, Array::New(isolate, 0));

                    if (allInfo)
                    {
                        HANDLE hDetailSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pe.th32ProcessID);
                        if (hDetailSnap != INVALID_HANDLE_VALUE)
                        {
                            THREADENTRY32 threadEntry;
                            threadEntry.dwSize = sizeof(THREADENTRY32);
                            ok = Thread32First(hDetailSnap, &threadEntry);
                            if (ok)
                            {
                                Local<Array> threads = Local<Array>::Cast(current->Get(threadsKey));
                                do
                                {
                                    if (threadEntry.th32OwnerProcessID == pe.th32ProcessID)
                                    {
                                        Local<Object> thread = Object::New(isolate);
                                        thread->Set(threadIdKey, Integer::New(isolate, threadEntry.th32ThreadID));
                                        thread->Set(threadPriorityKey, Integer::New(isolate, threadEntry.tpBasePri));
                                        threads->Set(threads->Length(), thread);
                                    }
                                } while (Thread32Next(hDetailSnap, &threadEntry));
                            }
                            CloseHandle(hDetailSnap);
                        }
                        hDetailSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe.th32ProcessID);
                        if (hDetailSnap != INVALID_HANDLE_VALUE)
                        {
                            MODULEENTRY32 moduleEntry;
                            moduleEntry.dwSize = sizeof(MODULEENTRY32);
                            ok = Module32First(hDetailSnap, &moduleEntry);
                            if (ok)
                            {
                                Local<Array> modules = Local<Array>::Cast(current->Get(modulesKey));
                                do
                                {
                                    if (moduleEntry.th32ProcessID == pe.th32ProcessID)
                                    {
                                        Local<Object> module = Object::New(isolate);
                                        std::stringstream ss;

                                        ss << "0x" << std::hex << (void*)moduleEntry.modBaseAddr;
                                        module->Set(moduleBaseAddrKey, String::NewFromUtf8(isolate, ss.str().c_str()));
                                        ss.str("");

                                        ss << "0x" << std::hex << (void*)moduleEntry.hModule;
                                        module->Set(moduleHandleKey, String::NewFromUtf8(isolate, ss.str().c_str()));
                                        ss.str("");

                                        module->Set(moduleSizeKey, Integer::NewFromUnsigned(isolate, moduleEntry.modBaseSize));
                                        module->Set(moduleNameKey, String::NewFromUtf8(isolate, moduleEntry.szModule));
                                        module->Set(modulePathKey, String::NewFromUtf8(isolate, moduleEntry.szExePath));

                                        modules->Set(modules->Length(), module);
                                    }
                                } while (Module32Next(hDetailSnap, &moduleEntry));
                            }
                            CloseHandle(hDetailSnap);
                        }

                    }

                }

                if (enumAll)
                {
                    Local<Array> arr = Local<Array>::Cast(result);
                    arr->Set(arr->Length(), current);
                }
                else if (!searchPid)
                {
                    if (found)
                    {
                        Local<Array> arr = Local<Array>::Cast(result);
                        arr->Set(arr->Length(), current);
                        found = false;
                    }
                }
                else
                {
                    if (found)
                        break;
                }

            } while (Process32Next(hSnap, &pe));

        }

        CloseHandle(hSnap);
    }


#else

#endif


    args.GetReturnValue().Set(result);
}


Persistent<Function> getChildFunc;
Persistent<Function> killChildProcsFunc;
Persistent<Function> getProcInfoFunc;

void Helpers_Init(v8::Local<v8::Object> exports)
{
    Isolate* isolate = exports->GetIsolate();
    Local<Object> globals = isolate->GetCurrentContext()->Global();

    Local<String> moduleKey = String::NewFromUtf8(isolate, "child_process");
    Local<String> typeKey = String::NewFromUtf8(isolate, "ChildProcess");
    Local<String> protoKey = String::NewFromUtf8(isolate, "prototype");

    Local<Object> module = globals->Get(moduleKey)->ToObject();
    Local<Object> type = module->Get(typeKey)->ToObject();

    if (type->IsFunction())
    {
        Local<String> getChildFuncName = String::NewFromUtf8(isolate, "getChildPids");
        Local<String> killChildProcsFuncName = String::NewFromUtf8(isolate, "killChildProcesses");
        Local<String> getProcInfoFuncName = String::NewFromUtf8(isolate, "getProcessInfo");

        Local<Object> prototype = type->Get(protoKey)->ToObject();

        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, GetChildProcessIds);
        tpl->SetClassName(getChildFuncName);
        getChildFunc.Reset(isolate, tpl->GetFunction());
        prototype->Set(getChildFuncName, tpl->GetFunction());

        tpl = FunctionTemplate::New(isolate, KillChildProcs);
        tpl->SetClassName(killChildProcsFuncName);
        killChildProcsFunc.Reset(isolate, tpl->GetFunction());
        prototype->Set(killChildProcsFuncName, tpl->GetFunction());

        tpl = FunctionTemplate::New(isolate, GetProcessInfo);
        tpl->SetClassName(getProcInfoFuncName);
        getProcInfoFunc.Reset(isolate, tpl->GetFunction());
        module->Set(getProcInfoFuncName, tpl->GetFunction()); // Add to the child_process module
    }

}

