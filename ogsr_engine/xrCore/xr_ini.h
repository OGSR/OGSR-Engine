#pragma once

class CInifile;
struct xr_token;

class XRCORE_API CInifile
{
    friend class CIniMerger;

public:
    using Item = std::pair<shared_str, shared_str>;
    struct XRCORE_API Sect
    {
        u32 Index{};
        shared_str Name{};
        shared_str ParentNames{};
        string_unordered_map<shared_str, shared_str> Data;
        xr_vector<Item> Ordered_Data;
        BOOL line_exist(LPCSTR, LPCSTR* = nullptr);
        u32 line_count();

        LPCSTR r_string(LPCSTR);
        u32 r_u32(LPCSTR);
        float r_float(LPCSTR);

        Ivector2 r_ivector2(LPCSTR);
        Fvector3 r_fvector3(LPCSTR);
    };
    using Root = string_unordered_map<shared_str, Sect*>;
    using RootItem = std::pair<shared_str, Sect*>;

    // factorisation
    static CInifile* Create(LPCSTR, BOOL = TRUE);
    static void Destroy(CInifile*);

    static IC BOOL IsBOOL(LPCSTR B) { return (xr_strcmp(B, "on") == 0 || xr_strcmp(B, "yes") == 0 || xr_strcmp(B, "true") == 0 || xr_strcmp(B, "1") == 0); }

protected:
    LPSTR fName;

    Root DATA;
    xr_vector<RootItem> Ordered_DATA;

    Sect* Current{}; // for use during load only

    void Load(IReader*, LPCSTR, BOOL, const CInifile*, bool root_level);

public:
    bool bReadOnly;
    BOOL bSaveAtEnd;
    bool bWasChanged{};

public:
    CInifile(IReader*, LPCSTR = 0);
    CInifile(LPCSTR, BOOL = TRUE, BOOL = TRUE, BOOL = TRUE);

    virtual ~CInifile();

    virtual const Root& sections() const { return DATA; }

    virtual const xr_vector<RootItem>& sections_ordered() const { return Ordered_DATA; }

    void load_file(BOOL allow_dup_sections = FALSE, const CInifile* f = NULL);

    bool save_as(LPCSTR = 0);
    std::string get_as_string();

    LPCSTR fname() { return fName; }

    virtual Sect& r_section(LPCSTR);
    virtual Sect& r_section(const shared_str&);

    BOOL line_exist(LPCSTR, LPCSTR);
    BOOL line_exist(const shared_str&, const shared_str&);

    u32 line_count(LPCSTR);
    u32 line_count(const shared_str&);

    virtual BOOL section_exist(LPCSTR);
    virtual BOOL section_exist(const shared_str&);

    CLASS_ID r_clsid(LPCSTR, LPCSTR);
    CLASS_ID r_clsid(const shared_str& S, LPCSTR L) { return r_clsid(S.c_str(), L); }

    LPCSTR r_string(LPCSTR, LPCSTR); // оставляет кавычки
    LPCSTR r_string(const shared_str& S, LPCSTR L) { return r_string(S.c_str(), L); } // оставляет кавычки

    shared_str r_string_wb(LPCSTR, LPCSTR); // убирает кавычки
    shared_str r_string_wb(const shared_str& S, LPCSTR L) { return r_string_wb(S.c_str(), L); } // убирает кавычки

    u8 r_u8(LPCSTR, LPCSTR);
    u8 r_u8(const shared_str& S, LPCSTR L) { return r_u8(S.c_str(), L); }

    u16 r_u16(LPCSTR, LPCSTR);
    u16 r_u16(const shared_str& S, LPCSTR L) { return r_u16(S.c_str(), L); }

    u32 r_u32(LPCSTR, LPCSTR);
    u32 r_u32(const shared_str& S, LPCSTR L) { return r_u32(S.c_str(), L); }

    s8 r_s8(LPCSTR, LPCSTR);
    s8 r_s8(const shared_str& S, LPCSTR L) { return r_s8(S.c_str(), L); }

    s16 r_s16(LPCSTR, LPCSTR);
    s16 r_s16(const shared_str& S, LPCSTR L) { return r_s16(S.c_str(), L); }

    s32 r_s32(LPCSTR, LPCSTR);
    s32 r_s32(const shared_str& S, LPCSTR L) { return r_s32(S.c_str(), L); }

    float r_float(LPCSTR, LPCSTR);
    float r_float(const shared_str& S, LPCSTR L) { return r_float(S.c_str(), L); }

    Fcolor r_fcolor(LPCSTR, LPCSTR);
    Fcolor r_fcolor(const shared_str& S, LPCSTR L) { return r_fcolor(S.c_str(), L); }

    u32 r_color(LPCSTR, LPCSTR);
    u32 r_color(const shared_str& S, LPCSTR L) { return r_color(S.c_str(), L); }

    Ivector2 r_ivector2(LPCSTR, LPCSTR);
    Ivector2 r_ivector2(const shared_str& S, LPCSTR L) { return r_ivector2(S.c_str(), L); }

    Ivector3 r_ivector3(LPCSTR, LPCSTR);
    Ivector3 r_ivector3(const shared_str& S, LPCSTR L) { return r_ivector3(S.c_str(), L); }

    Ivector4 r_ivector4(LPCSTR, LPCSTR);
    Ivector4 r_ivector4(const shared_str& S, LPCSTR L) { return r_ivector4(S.c_str(), L); }

    Fvector2 r_fvector2(LPCSTR, LPCSTR);
    Fvector2 r_fvector2(const shared_str& S, LPCSTR L) { return r_fvector2(S.c_str(), L); }

    Fvector3 r_fvector3(LPCSTR, LPCSTR);
    Fvector3 r_fvector3(const shared_str& S, LPCSTR L) { return r_fvector3(S.c_str(), L); }

    Fvector4 r_fvector4(LPCSTR, LPCSTR);
    Fvector4 r_fvector4(const shared_str& S, LPCSTR L) { return r_fvector4(S.c_str(), L); }

    BOOL r_bool(LPCSTR, LPCSTR);
    BOOL r_bool(const shared_str& S, LPCSTR L) { return r_bool(S.c_str(), L); }

    int r_token(LPCSTR, LPCSTR, const xr_token* token_list);

    BOOL r_line(LPCSTR, int, LPCSTR*, LPCSTR*);
    BOOL r_line(const shared_str&, int, LPCSTR*, LPCSTR*);

    void w_string(LPCSTR, LPCSTR, LPCSTR);
    void w_u8(LPCSTR, LPCSTR, u8);
    void w_u16(LPCSTR, LPCSTR, u16);
    void w_u32(LPCSTR, LPCSTR, u32);
    void w_s8(LPCSTR, LPCSTR, s8);
    void w_s16(LPCSTR, LPCSTR, s16);
    void w_s32(LPCSTR, LPCSTR, s32);
    void w_float(LPCSTR, LPCSTR, float);
    void w_fcolor(LPCSTR, LPCSTR, const Fcolor&);
    void w_color(LPCSTR, LPCSTR, u32);
    void w_ivector2(LPCSTR, LPCSTR, const Ivector2&);
    void w_ivector3(LPCSTR, LPCSTR, const Ivector3&);
    void w_ivector4(LPCSTR, LPCSTR, const Ivector4&);
    void w_fvector2(LPCSTR, LPCSTR, const Fvector2&);
    void w_fvector3(LPCSTR, LPCSTR, const Fvector3&);
    void w_fvector4(LPCSTR, LPCSTR, const Fvector4&);
    void w_bool(LPCSTR, LPCSTR, bool);

    void remove_line(LPCSTR, LPCSTR);
    void remove_section(LPCSTR);

    Sect& append_section(LPCSTR name, Sect* base = nullptr);
};

// Main configuration file
extern XRCORE_API CInifile* pSettings;
