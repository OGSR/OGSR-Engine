#pragma once

#include "ParticleEffectDef.h"
#include "ParticleGroup.h"

class ECORE_API CPSLibrary
{
    string_unordered_map<shared_str, std::unique_ptr<PS::CPEDef>> m_PEDs;
    string_unordered_map<shared_str, std::unique_ptr<PS::CPGDef>> m_PGDs;

public:
    void LoadAll();
    void ExportAllAsNew();

    bool Load(const char* nm);
    bool Save(const char* nm);

    bool Load2();
    bool Save2(bool override) const;

    void DumpTextures() const;

    CPSLibrary() = default;
    ~CPSLibrary() = default;

    void OnCreate();
    void OnDestroy();

    PS::CPEDef* FindPED(const char* name);
    PS::CPGDef* FindPGD(const char* name);

    const decltype(m_PEDs)& IteratePEDs() { return m_PEDs; }
    const decltype(m_PGDs)& IteratePGDs() { return m_PGDs; }

    void Reload();
};

#define PS_VERSION 0x0001
#define PS_CHUNK_VERSION 0x0001
#define PS_CHUNK_FIRSTGEN 0x0002
#define PS_CHUNK_SECONDGEN 0x0003
#define PS_CHUNK_THIRDGEN 0x0004
