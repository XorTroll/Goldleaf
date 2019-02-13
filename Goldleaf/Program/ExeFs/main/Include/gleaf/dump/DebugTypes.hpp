
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Code ported from Atmosphere's creport

*/

#pragma once
#include <switch.h>

namespace gleaf::dump
{
    struct StackFrame
    {
        u64 fp;
        u64 lr;
    };

    struct AttachProcessInfo
    {
        u64 title_id;
        u64 process_id;
        char name[0xC];
        u32 flags;
        u64 user_exception_context_address;
    };

    struct AttachThreadInfo
    {
        u64 thread_id;
        u64 tls_address;
        u64 entrypoint;
    };

    enum class DebugExceptionType : u32
    {
        UndefinedInstruction = 0,
        InstructionAbort = 1,
        DataAbort = 2,
        AlignmentFault = 3,
        DebuggerAttached = 4,
        BreakPoint = 5,
        UserBreak = 6,
        DebuggerBreak = 7,
        BadSvc = 8,
        UnknownNine = 9,
    };

    struct UndefinedInstructionInfo
    {
        u32 insn;
    };

    struct DataAbortInfo
    {
        u64 address;
    };

    struct AlignmentFaultInfo
    {
        u64 address;
    };

    struct UserBreakInfo
    {
        u64 break_reason;
        u64 address;
        u64 size;
    };

    struct BadSvcInfo
    {
        u32 id;
    };

    union SpecificExceptionInfo
    {
        UndefinedInstructionInfo undefined_instruction;
        DataAbortInfo data_abort;
        AlignmentFaultInfo alignment_fault;
        UserBreakInfo user_break;
        BadSvcInfo bad_svc;
        u64 raw;
    };

    struct ExceptionInfo
    {
        DebugExceptionType type;
        u64 address;
        SpecificExceptionInfo specific;
    };


    enum class DebugEventType : u32
    {
        AttachProcess = 0,
        AttachThread = 1,
        ExitProcess = 2,
        ExitThread = 3,
        Exception = 4
    };

    union DebugInfo
    {
        AttachProcessInfo attach_process;
        AttachThreadInfo attach_thread;
        ExceptionInfo exception;
    };

    struct DebugEventInfo
    {
        DebugEventType type;
        u32 flags;
        u64 thread_id;
        union
        {
            DebugInfo info;
            u64 _[0x40/sizeof(u64)];
        };
    };
}