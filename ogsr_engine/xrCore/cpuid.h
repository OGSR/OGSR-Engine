#pragma once

#include <bitset>
#include <winternl.h>

struct XRCORE_API _processor_info final
{
    _processor_info();

    string32 vendor; // vendor name
    string64 brand; // Name of model eg. Intel_Pentium_Pro
    int family; // family of the processor, eg. Intel_Pentium_Pro is family 6 processor
    int model; // model of processor, eg. Intel_Pentium_Pro is model 1 of family 6 processor
    int stepping; // Processor revision number
    unsigned int threadCount; // number of available threads, both physical and logical
    unsigned int coresCount; // number of physical cores

    void clearFeatures() { m_f1_ECX = m_f1_EDX = m_f7_EBX = m_f7_ECX = m_f81_ECX = m_f81_EDX = 0; }
    bool hasMMX() const { return m_f1_EDX[23]; }
    bool has3DNOWExt() const { return m_f81_EDX[30]; }
    bool has3DNOW() const { return m_f81_EDX[31]; }
    bool hasSSE() const { return m_f1_EDX[25]; }
    bool hasSSE2() const { return m_f1_EDX[26]; }
    bool hasSSE3() const { return m_f1_ECX[0]; }
    bool hasMWAIT() const { return m_f1_ECX[3]; }
    bool hasSSSE3() const { return m_f1_ECX[9]; }
    bool hasSSE41() const { return m_f1_ECX[19]; }
    bool hasSSE42() const { return m_f1_ECX[20]; }
    bool hasSSE4a() const { return m_f81_ECX[6]; }
    bool hasAVX() const { return m_f1_ECX[28]; }
    bool hasAVX2() const { return m_f7_EBX[5]; }

    bool getCPULoad(double& val);
    void MTCPULoad();
    DWORD m_dwNumberOfProcessors;
    std::unique_ptr<float[]> fUsage;

private:
    std::bitset<32> m_f1_ECX;
    std::bitset<32> m_f1_EDX;
    std::bitset<32> m_f7_EBX;
    std::bitset<32> m_f7_ECX;
    std::bitset<32> m_f81_ECX;
    std::bitset<32> m_f81_EDX;

    DWORD m_dwCount = 0;
    FILETIME prevSysIdle{}, prevSysKernel{}, prevSysUser{};
    std::unique_ptr<LARGE_INTEGER[]> m_idleTime;
    std::unique_ptr<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION[]> perfomanceInfo;
};
