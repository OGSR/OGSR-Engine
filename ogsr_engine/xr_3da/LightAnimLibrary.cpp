//---------------------------------------------------------------------------
#include "stdafx.h"

#include "LightAnimLibrary.h"
//---------------------------------------------------------------------------
#define LANIM_VERSION 0x0001
//---------------------------------------------------------------------------
#define CHUNK_VERSION 0x0000
#define CHUNK_ITEM_LIST 0x0001
//---------------------------------------------------------------------------
#define CHUNK_ITEM_COMMON 0x0001
#define CHUNK_ITEM_KEYS 0x0002
//---------------------------------------------------------------------------

ELightAnimLibrary LALib;

CLAItem::CLAItem()
{
    def_fFPS = fFPS = 15.f;
    iFrameCount = 1;
}

void CLAItem::InitDefault() { Keys[0] = 0x00000000; }

void CLAItem::Load(IReader& F)
{
    R_ASSERT(F.find_chunk(CHUNK_ITEM_COMMON));
    F.r_stringZ(cName);
    def_fFPS = fFPS = F.r_float();
    iFrameCount = F.r_u32();

    R_ASSERT(F.find_chunk(CHUNK_ITEM_KEYS));

    const int key_cnt = F.r_u32();
    for (int i = 0; i < key_cnt; i++)
    {
        int key = F.r_u32();
        Keys[key] = F.r_u32();
    }
}

void CLAItem::MoveKey(int from, int to)
{
    R_ASSERT(from <= iFrameCount);
    R_ASSERT(to <= iFrameCount);
    KeyPairIt it = Keys.find(from);
    if (it != Keys.end())
    {
        Keys[to] = it->second;
        Keys.erase(it);
    }
}

void CLAItem::Resize(int new_len)
{
    VERIFY((new_len >= 1));
    if (new_len != iFrameCount)
    {
        if (new_len > iFrameCount)
        {
            int old_len = iFrameCount;
            iFrameCount = new_len;
            MoveKey(old_len, new_len);
        }
        else
        {
            KeyPairIt I = Keys.upper_bound(new_len - 1);
            if (I != Keys.end())
                Keys.erase(I, Keys.end());
            iFrameCount = new_len;
        }
    }
}

u32 CLAItem::InterpolateRGB(int frame)
{
    R_ASSERT(frame <= iFrameCount);

    KeyPairIt A = Keys.find(frame);
    KeyPairIt B;
    if (A != Keys.end())
    { // ключ - возвращаем цвет ключа
        return A->second;
    }
    else
    { // не ключ
        B = Keys.upper_bound(frame); // ищем следующий ключ
        if (B == Keys.end())
        { // если его нет вернем цвет последнего ключа
            --B;
            return B->second;
        }
        A = B; // иначе в A занесем предыдущий ключ
        --A;
    }

    R_ASSERT(Keys.size() > 1);
    // интерполируем цвет
    Fcolor c, c0, c1;
    float a0 = (float)A->first;
    float a1 = (float)B->first;
    c0.set(A->second);
    c1.set(B->second);
    float t = float(frame - a0) / float(a1 - a0);
    c.lerp(c0, c1, t);
    return c.get();
}

u32 CLAItem::InterpolateBGR(int frame)
{
    u32 c = InterpolateRGB(frame);
    return color_rgba(color_get_B(c), color_get_G(c), color_get_R(c), color_get_A(c));
}

u32 CLAItem::CalculateRGB(float T, int& frame)
{
    frame = iFloor(fmodf(T, float(iFrameCount) / fFPS) * fFPS);
    return InterpolateRGB(frame);
}

u32 CLAItem::CalculateBGR(float T, int& frame)
{
    frame = iFloor(fmodf(T, float(iFrameCount) / fFPS) * fFPS);
    return InterpolateBGR(frame);
}

//------------------------------------------------------------------------------
// Library
//------------------------------------------------------------------------------
ELightAnimLibrary::ELightAnimLibrary() {}

ELightAnimLibrary::~ELightAnimLibrary() {}

void ELightAnimLibrary::OnCreate() { Load(); }

void ELightAnimLibrary::OnDestroy() { Unload(); }

void ELightAnimLibrary::Unload()
{
    for (auto& Item : Items)
        xr_delete(Item);
    Items.clear();
}

void ELightAnimLibrary::DbgDumpInfo() const
{
    LAItemVec tmp{Items};

    std::sort(tmp.begin(), tmp.end(), [](const CLAItem* a, const CLAItem* b) { return xr_strcmp(a->cName, b->cName) < 0; });

    for (auto& Item : tmp)
    {
        Msg("~ ELightAnimLibrary Item [%s] FPS: %.2f, FrameCount: %d, KeyCount: %d", Item->cName.c_str(), Item->fFPS, Item->iFrameCount, Item->Keys.size());
    }
}

void ELightAnimLibrary::Load()
{
    FS_FileSet flist;
    FS.file_list(flist, fsgame::game_data, FS_ListFiles | FS_RootOnly, "*lanims*.xr");
    Msg("[%s] count of *lanims*.xr files: [%u]", __FUNCTION__, flist.size());

    for (const auto& file : flist)
    {
        string_path fn;
        FS.update_path(fn, fsgame::game_data, file.name.c_str());

        if (IReader* fs = FS.r_open(fn))
        {
            u16 version = 0;
            if (fs->find_chunk(CHUNK_VERSION))
            {
                version = fs->r_u16();
            }

            if (IReader* obj = fs->open_chunk(CHUNK_ITEM_LIST))
            {
                IReader* o = obj->open_chunk(0);

                for (int count = 1; o; count++)
                {
                    CLAItem* I = xr_new<CLAItem>();
                    I->Load(*o);

                    if (version == 0)
                    {
                        for (auto& Key : I->Keys)
                            Key.second = subst_alpha(bgr2rgb(Key.second), color_get_A(Key.second));
                    }

                    Items.push_back(I);

                    o->close();

                    o = obj->open_chunk(count); // open next chunk
                }

                obj->close();
            }

            FS.r_close(fs);
        }
    }
}

LAItemIt ELightAnimLibrary::FindItemI(LPCSTR name)
{
    if (name && name[0])
        for (LAItemIt it = Items.begin(); it != Items.end(); ++it)
            if (0 == xr_strcmp((*it)->cName, name))
                return it;
    return Items.end();
}

CLAItem* ELightAnimLibrary::FindItem(LPCSTR name)
{
    LAItemIt it = FindItemI(name);
    return (it != Items.end()) ? *it : nullptr;
}