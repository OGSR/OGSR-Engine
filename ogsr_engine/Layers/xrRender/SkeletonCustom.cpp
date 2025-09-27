#include "stdafx.h"

#include "SkeletonCustom.h"
#include "SkeletonX.h"
#include "../../xr_3da/fmesh.h"
#include "../../xr_3da/Render.h"

int psSkeletonUpdate = 32;

u16 CKinematics::LL_BoneID(const char* B) const
{
    if (const auto I = bone_map_N.find(B); I != bone_map_N.end())
        return I->second;

    return BI_NONE;
}

u16 CKinematics::LL_BoneID(const shared_str& B) const
{
    if (const auto I = bone_map_N.find(B); I != bone_map_N.end())
        return I->second;

    return BI_NONE;
}

const char* CKinematics::LL_BoneName(const u16 ID) const
{
    for (const auto& bone : bone_map_N)
        if (bone.second == ID)
            return bone.first.c_str();

    return nullptr;
}

#ifdef DEBUG
void CKinematics::DebugRender(Fmatrix& XFORM)
{
    cmd_list.set_Shader(dxRenderDeviceRender::Instance().m_WireShader);

    CalculateBones();

    CBoneData::BoneDebug dbgLines;
    (*bones)[iRoot]->DebugQuery(dbgLines);

    Fvector Z;
    Z.set(0, 0, 0);
    Fvector H1;
    H1.set(0.01f, 0.01f, 0.01f);
    Fvector H2;
    H2.mul(H1, 2);
    for (u32 i = 0; i < dbgLines.size(); i += 2)
    {
        Fmatrix& M1 = bone_instances[dbgLines[i]].mTransform;
        Fmatrix& M2 = bone_instances[dbgLines[i + 1]].mTransform;

        Fvector P1, P2;
        M1.transform_tiny(P1, Z);
        M2.transform_tiny(P2, Z);
        cmd_list.dbg_DrawLINE(XFORM, P1, P2, D3DCOLOR_XRGB(0, 255, 0));

        Fmatrix M;
        M.mul_43(XFORM, M2);
        cmd_list.dbg_DrawOBB(M, H1, D3DCOLOR_XRGB(255, 255, 255));
        cmd_list.dbg_DrawOBB(M, H2, D3DCOLOR_XRGB(255, 255, 255));
    }

    for (u32 b = 0; b < bones->size(); b++)
    {
        Fobb& obb = (*bones)[b]->obb;
        Fmatrix& Mbone = bone_instances[b].mTransform;
        Fmatrix Mbox;
        obb.xform_get(Mbox);
        Fmatrix X;
        X.mul(Mbone, Mbox);
        Fmatrix W;
        W.mul(XFORM, X);
        cmd_list.dbg_DrawOBB(W, obb.m_halfsize, D3DCOLOR_XRGB(0, 0, 255));
    }
}
#endif

CKinematics::CKinematics()
{
#ifdef DEBUG
    dbg_single_use_marker = FALSE;
#endif

    m_is_original_lod = false;

    mOldWorldMartrix.identity();
    mOldWorldMartrixTmp.identity();

    dwFirstRenderFrame = static_cast<u32>(-1);
}

CKinematics::~CKinematics()
{
    IBoneInstances_Destroy();

    // wallmarks
    ClearWallmarks();

    if (m_lod)
    {
        if (CKinematics* lod_kinematics = smart_cast<CKinematics*>(m_lod))
        {
            if (lod_kinematics->m_is_original_lod)
            {
                lod_kinematics->Release();
            }
        }

        xr_delete(m_lod);
    }
}


void CKinematics::StoreVisualMatrix(const Fmatrix& world_matrix)
{
    if (dwFirstRenderFrame != Device.dwFrame)
    {
        dwFirstRenderFrame = Device.dwFrame;
        mOldWorldMartrix.set(mOldWorldMartrixTmp);
        mOldWorldMartrixTmp.set(world_matrix);

        for (u16 i = 0; i < LL_BoneCount(); ++i)
        {
            auto& Bi = LL_GetBoneInstance(i);

            Bi.mRenderTransform_old.set(Bi.mRenderTransform_tmp);
            Bi.mRenderTransform_tmp.set(Bi.mRenderTransform);
        }
    }
}

void CKinematics::IBoneInstances_Create()
{
    // VERIFY				(bones->size() < 64, "More than 64 bones is a crazy thing!");
    const u32 size = bones->size();
    bone_instances = xr_alloc<CBoneInstance>(size);
    for (u32 i = 0; i < size; i++)
        bone_instances[i].construct();
}

void CKinematics::IBoneInstances_Destroy()
{
    if (bone_instances)
    {
        xr_free(bone_instances);
        bone_instances = nullptr;
    }
}

CSkeletonX* CKinematics::LL_GetChild(const u32 idx) const
{
    IRenderVisual* V = children[idx];
    CSkeletonX* B = smart_cast<CSkeletonX*>(V);
    return B;
}

void CKinematics::Load(const char* N, IReader* data, const u32 dwFlags)
{
    // Msg				("skeleton: %s",N);
    inherited::Load(N, data, dwFlags);

    pUserData = nullptr;
    m_lod = nullptr;
    // loading lods

    IReader* LD = data->open_chunk(OGF_S_LODS);
    if (LD)
    {
        // From stream
        {
            string_path lod_name;
            LD->r_string(lod_name, sizeof(lod_name));

            m_lod = smart_cast<dxRender_Visual*>(RImplementation.model_CreateChild(lod_name, nullptr));

            if (CKinematics* lod_kinematics = smart_cast<CKinematics*>(m_lod))
            {
                lod_kinematics->m_is_original_lod = true;
            }

            VERIFY(m_lod, "Cant create LOD model for", N);
        }
        LD->close();
    }

    IReader* UD;

    if (const auto userdata_override_ini = RImplementation.Models->userdata_override_ini; 
        userdata_override_ini && userdata_override_ini->section_exist("data"))
    {
        for (const auto& pair : userdata_override_ini->r_section("data").Ordered_Data)
        {
            if (!strncmp(N, pair.first.c_str(), strlen(pair.first.c_str())))
            {
                UD = FS.r_open(fsgame::game_configs, pair.second.c_str());
                if (UD)
                {
                    Msg("Loading user_data [%s] for %s...", pair.second.c_str(), N);

                    pUserData = xr_new<CInifile>(UD, FS.get_path(fsgame::game_configs)->m_Path);
                    UD->close();
                }

                break;
            }
        }
    }

    // try to read custom user data for module from ltx file
    if (!pUserData)
    {
        string_path ini_path;
        strcpy_s(ini_path, N);
        if (strext(ini_path))
            *strext(ini_path) = 0;
        strcat_s(ini_path, ".ltx");

        UD = FS.r_open(fsgame::game_meshes, ini_path);
        if (UD)
        {
            Msg("Loading user_data [%s] for %s...", ini_path, N);

            pUserData = xr_new<CInifile>(UD, FS.get_path(fsgame::game_configs)->m_Path);
            UD->close();
        }
    }

    // read user data from model if no custom found
    if (!pUserData)
    {
        // User data
        UD = data->open_chunk(OGF_S_USERDATA);
        pUserData = UD ? xr_new<CInifile>(UD, FS.get_path(fsgame::game_configs)->m_Path) : nullptr;
        if (UD)
            UD->close();
    }

    // Globals
    bones = xr_new<vecBones>();
    bone_instances = nullptr;

    // Load bones
    xr_vector<shared_str> L_parents;

    R_ASSERT(data->find_chunk(OGF_S_BONE_NAMES));

    visimask.zero();

    u32 dwCount = data->r_u32();

    bone_map_N.reserve(dwCount);

    const auto bone_override_ini = RImplementation.Models->bone_override_ini;

    const bool has_bones = bone_override_ini && bone_override_ini->section_exist(N);

    for (; dwCount; dwCount--)
    {
        string256 buf;

        // Bone
        u16 ID = static_cast<u16>(bones->size());
        data->r_stringZ(buf, sizeof(buf));
        strlwr(buf);
        CBoneData* pBone = CreateBoneData(ID);
        pBone->name = buf;
        pBone->child_faces.resize(children.size());
        bones->push_back(pBone);

        bone_map_N.emplace(pBone->name, ID);

        // It's parent
        data->r_stringZ(buf, sizeof(buf));
        strlwr(buf);
        L_parents.emplace_back(buf);

        data->r(&pBone->obb, sizeof(Fobb));
        visimask.set(ID, true);
    }

    
    if (has_bones)
    {
        for (const auto& it : bone_override_ini->r_section(N).Ordered_Data)
        {
            shared_str bone_nm_old = it.first;
            shared_str bone_nm = it.second;

            u16 ID = LL_BoneID(bone_nm);

            if (ID != BI_NONE)
            {
                bone_map_N.emplace(bone_nm_old, ID);

                MsgDbg("Bone override model [%s] from [%s] to [%s]", N, bone_nm_old.c_str(), bone_nm.c_str());
            }
        }
    }

    // Attach bones to their parents
    iRoot = BI_NONE;
    for (u32 i = 0; i < bones->size(); i++)
    {
        shared_str P = L_parents[i];
        CBoneData* B = (*bones)[i];
        if (!P.c_str() || !P[0])
        {
            // no parent - this is root bone
            R_ASSERT(BI_NONE == iRoot);
            iRoot = static_cast<u16>(i);
            B->SetParentID(BI_NONE);
            continue;
        }
        else
        {
            const u16 ID = LL_BoneID(P);
            R_ASSERT(ID != BI_NONE);
            (*bones)[ID]->children.push_back(B);
            B->SetParentID(ID);
        }
    }
    R_ASSERT(BI_NONE != iRoot);

    // Free parents
    L_parents.clear();

    // IK data
    if (IReader* IKD = data->open_chunk(OGF_S_IKDATA))
    {
        const bool fix_cop_joints = pUserData ? READ_IF_EXISTS(pUserData, r_bool, "compat", "fix_cop_joints", false) : false;
        for (const auto& B : *bones)
        {
            const u16 vers = static_cast<u16>(IKD->r_u32());
            IKD->r_stringZ(B->game_mtl_name);
            IKD->r(&B->shape, sizeof(SBoneShape));
            B->IK_data.Import(*IKD, vers);

            IKD->r_fvector3(B->rotation);
            IKD->r_fvector3(B->position);
            B->bind_transform.setXYZi(B->rotation);
            B->bind_transform.translate_over(B->position);
            B->mass = IKD->r_float();
            IKD->r_fvector3(B->center_of_mass);

            if (fix_cop_joints)
            {
                // https://bitbucket.org/stalker/xray_re-tools/commits/209b9014129ceeb7d92375a77f60835553266bf1
                for (auto& it : B->IK_data.limits)
                {
                    const Fvector2 vec = it.limit;
                    const float tmp = vec.x;
                    it.limit.x = -vec.y;
                    it.limit.y = -tmp;
                }
            }
        }
        // calculate model to bone converting matrix
        (*bones)[LL_GetBoneRoot()]->CalculateM2B(Fidentity);
        IKD->close();
    }

    // after load process
    {
        for (u16 child_idx = 0; child_idx < static_cast<u16>(children.size()); child_idx++)
            LL_GetChild(child_idx)->AfterLoad(this, child_idx);
    }

    // unique bone faces
    {
        for (const auto& B : *bones)
        {
            for (u32 child_idx = 0; child_idx < children.size(); child_idx++)
            {
                CBoneData::FacesVec faces = B->child_faces[child_idx];
                std::sort(faces.begin(), faces.end());
                const CBoneData::FacesVecIt new_end = std::unique(faces.begin(), faces.end());
                faces.erase(new_end, faces.end());
                B->child_faces[child_idx].clear();
                B->child_faces[child_idx] = faces;
            }
        }
    }

    // reset update_callback
    Update_Callback = nullptr;
    // reset update frame
    wm_frame = static_cast<u32>(-1);

    LL_Validate();
}

IC void iBuildGroups(CBoneData* B, U16Vec& tgt, u16 id, u16& last_id)
{
    if (B->IK_data.ik_flags.is(SJointIKData::flBreakable))
        id = ++last_id;
    tgt[B->GetSelfID()] = id;
    for (const auto& bone_it : B->children)
        iBuildGroups(bone_it, tgt, id, last_id);
}

void CKinematics::LL_Validate()
{
    // check breakable
    BOOL bCheckBreakable = FALSE;
    for (u16 k = 0; k < LL_BoneCount(); k++)
    {
        if (LL_GetData(k).IK_data.ik_flags.is(SJointIKData::flBreakable) && (LL_GetData(k).IK_data.type != jtNone))
        {
            bCheckBreakable = TRUE;
            break;
        }
    }

    if (bCheckBreakable)
    {
        BOOL bValidBreakable = TRUE;

        xr_vector<xr_vector<u16>> groups;
        LL_GetBoneGroups(groups);

        xr_vector<u16> b_parts(LL_BoneCount(), BI_NONE);
        CBoneData* root = &LL_GetData(LL_GetBoneRoot());
        u16 last_id = 0;
        iBuildGroups(root, b_parts, 0, last_id);

        for (u16 g = 0; g < static_cast<u16>(groups.size()); ++g)
        {
            xr_vector<u16>& group = groups[g];
            const u16 bp_id = b_parts[group[0]];
            for (u32 b = 1; b < groups[g].size(); b++)
                if (bp_id != b_parts[groups[g][b]])
                {
                    bValidBreakable = FALSE;
                    break;
                }
        }

        if (bValidBreakable == FALSE)
        {
            for (u16 k = 0; k < LL_BoneCount(); k++)
            {
                CBoneData& BD = LL_GetData(k);
                if (BD.IK_data.ik_flags.is(SJointIKData::flBreakable))
                    BD.IK_data.ik_flags.set(SJointIKData::flBreakable, FALSE);
            }
#ifdef DEBUG
            Msg("! ERROR: Invalid breakable object: '%s'", *dbg_name);
#endif
        }
    }
}

void CKinematics::Copy(dxRender_Visual* P)
{
    inherited::Copy(P);

    const CKinematics* pFrom = smart_cast<CKinematics*>(P);
    VERIFY(pFrom);
    pUserData = pFrom->pUserData;
    bones = pFrom->bones;
    iRoot = pFrom->iRoot;
    bone_map_N = pFrom->bone_map_N;
    visimask = pFrom->visimask;

    IBoneInstances_Create();

    for (u32 i = 0; i < children.size(); i++)
        LL_GetChild(i)->SetParent(this);

    CalculateBones_Invalidate();

    m_lod = (pFrom->m_lod) ? smart_cast<dxRender_Visual*>(RImplementation.model_Duplicate(pFrom->m_lod)) : nullptr;
}

void CKinematics::CalculateBones_Invalidate()
{
    UCalc_Time = 0x0;
    UCalc_Visibox = psSkeletonUpdate;
}

void CKinematics::Spawn()
{
    inherited::Spawn();
    // bones
    for (u32 i = 0; i < bones->size(); i++)
        bone_instances[i].construct();
    Update_Callback = nullptr;
    CalculateBones_Invalidate();
    // wallmarks
    ClearWallmarks();
    Visibility_Invalidate();
    LL_SetBoneRoot(0);
}

void CKinematics::Depart()
{
    inherited::Depart();
    // wallmarks
    ClearWallmarks();

    // unmask all bones
    visimask.zero();
    if (bones)
    {
        for (u16 b = 0; b < bones->size(); b++)
            visimask.set(b, true);
    }

    // visibility
    children.insert(children.end(), children_invisible.begin(), children_invisible.end());
    children_invisible.clear();
}

void CKinematics::Release()
{
    // xr_free bones
    for (auto& B : *bones)
    {
        xr_delete(B);
    }

    // destroy shared data
    xr_delete(pUserData);
    xr_delete(bones);
    bone_map_N.clear();

    inherited::Release();
}

void CKinematics::LL_SetBoneVisible(const u16 bone_id, const BOOL val, const BOOL bRecursive)
{
    //R_ASSERT(bone_id < LL_BoneCount());
    if (bone_id >= LL_BoneCount())
        return;
    visimask.set(bone_id, !!val);
    if (!visimask.is(bone_id))
    {
        bone_instances[bone_id].mTransform.scale(0.f, 0.f, 0.f);
        if (LL_GetData(bone_id).GetParentID() < LL_BoneCount() && LL_GetData(bone_id).GetParentID() != BI_NONE)
            bone_instances[bone_id].mTransform.c = LL_GetBoneInstance(LL_GetData(bone_id).GetParentID()).mTransform.c;
    }
    else
    {
        CalculateBones_Invalidate();
    }
    bone_instances[bone_id].mRenderTransform.mul_43(bone_instances[bone_id].mTransform, (*bones)[bone_id]->m2b_transform);
    if (bRecursive)
    {
        for (const auto& C : (*bones)[bone_id]->children)
            LL_SetBoneVisible(C->GetSelfID(), val, bRecursive);
    }
    Visibility_Invalidate();
}

void CKinematics::LL_SetBonesVisible(VisMask mask)
{
    visimask.zero();
    for (u16 b = 0; b < bones->size(); b++)
    {
        if (mask.is(b))
        {
            visimask.set(b, true);
        }
        else
        {
            Fmatrix& A = bone_instances[b].mTransform;
            Fmatrix& B = bone_instances[b].mRenderTransform;
            A.scale(0.f, 0.f, 0.f);
            B.mul_43(A, (*bones)[b]->m2b_transform);
        }
    }
    CalculateBones_Invalidate();
    Visibility_Invalidate();
}

void CKinematics::Visibility_Update()
{
    ZoneScoped;

    Update_Visibility = FALSE;
    // check visible
    for (u32 c_it = 0; c_it < children.size(); c_it++)
    {
        CSkeletonX* _c = smart_cast<CSkeletonX*>(children[c_it]);
        VERIFY(_c);
        if (!_c->has_visible_bones())
        {
            // move into invisible list
            children_invisible.push_back(children[c_it]);
            swap(children[c_it], children.back());
            children.pop_back();
            Update_Visibility = TRUE;
        }
    }

    // check invisible
    for (u32 _it = 0; _it < children_invisible.size(); _it++)
    {
        CSkeletonX* _c = smart_cast<CSkeletonX*>(children_invisible[_it]);
        VERIFY(_c);
        if (_c->has_visible_bones())
        {
            // move into visible list
            children.push_back(children_invisible[_it]);
            swap(children_invisible[_it], children_invisible.back());
            children_invisible.pop_back();
            Update_Visibility = TRUE;
        }
    }
}

IC static void RecursiveBindTransform(CKinematics* K, xr_vector<Fmatrix>& matrices, const u16 bone_id, const Fmatrix& parent)
{
    const CBoneData& BD = K->LL_GetData(bone_id);
    Fmatrix& BM = matrices[bone_id];
    // Build matrix
    BM.mul_43(parent, BD.bind_transform);
    for (const auto& C : BD.children)
        RecursiveBindTransform(K, matrices, C->GetSelfID(), BM);
}

void CKinematics::LL_GetBindTransform(xr_vector<Fmatrix>& matrices)
{
    matrices.resize(LL_BoneCount());
    RecursiveBindTransform(this, matrices, iRoot, Fidentity);
}

void BuildMatrix(Fmatrix& mView, const float invsz, const Fvector norm, const Fvector& from)
{
    // build projection
    Fmatrix mScale;
    Fvector at, up, right, y;
    at.sub(from, norm);
    y.set(0, 1, 0);
    if (_abs(norm.y) > .99f)
        y.set(1, 0, 0);
    right.crossproduct(y, norm);
    up.crossproduct(norm, right);
    mView.build_camera(from, at, up);
    mScale.scale(invsz, invsz, invsz);
    mView.mulA_43(mScale);
}
void CKinematics::EnumBoneVertices(SEnumVerticesCallback& C, const u16 bone_id)
{
    for (u32 i = 0; i < children.size(); i++)
        LL_GetChild(i)->EnumBoneVertices(C, bone_id);
}

DEFINE_VECTOR(Fobb, OBBVec, OBBVecIt);

bool CKinematics::PickBone(const Fmatrix& parent_xform, IKinematics::pick_result& r, const float dist, const Fvector& start, const Fvector& dir, const u16 bone_id)
{
    Fvector S, D; // normal		= {0,0,0}
    // transform ray from world to model
    Fmatrix P;
    P.invert(parent_xform);
    P.transform_tiny(S, start);
    P.transform_dir(D, dir);
    for (u32 i = 0; i < children.size(); i++)
        if (LL_GetChild(i)->PickBone(r, dist, S, D, bone_id))
        {
            parent_xform.transform_dir(r.normal);
            parent_xform.transform_tiny(r.tri[0]);
            parent_xform.transform_tiny(r.tri[1]);
            parent_xform.transform_tiny(r.tri[2]);
            return true;
        }
    return false;
}

void CKinematics::wallmark_calculate_details::add_wallmark_internal(CKinematics* parent)
{
    ZoneScoped;

    Fvector S, D, normal = {0, 0, 0};
    // transform ray from world to model
    Fmatrix P;
    P.invert(*parent_xform);
    P.transform_tiny(S, position);
    P.transform_dir(D, direction);
    // find pick point
    float dist = flt_max;
    BOOL picked = FALSE;

    DEFINE_VECTOR(Fobb, OBBVec, OBBVecIt);
    OBBVec cache_obb;
    cache_obb.resize(parent->LL_BoneCount());
    IKinematics::pick_result r;
    r.normal = normal;
    r.dist = dist;
    for (u16 k = 0; k < parent->LL_BoneCount(); k++)
    {
        CBoneData& BD = parent->LL_GetData(k);
        if (parent->LL_GetBoneVisible(k) && !BD.shape.flags.is(SBoneShape::sfNoPickable))
        {
            Fobb& obb = cache_obb[k];
            obb.transform(BD.obb, parent->LL_GetBoneInstance(k).mTransform);
            if (CDB::TestRayOBB(S, D, obb))
                for (u32 i = 0; i < parent->children.size(); i++)
                {
                    if (parent->LL_GetChild(i)->PickBone(r, dist, S, D, k))
                    {
                        picked = TRUE;
                        dist = r.dist;
                        normal = r.normal;
                    }
                }
        }
    }
    if (!picked)
    {
        return;
    }

    // calculate contact point
    Fvector cp;
    cp.mad(S, D, dist);

    // collect collide boxes
    Fsphere test_sphere;
    test_sphere.set(cp, size);
    U16Vec test_bones;
    test_bones.reserve(parent->LL_BoneCount());
    for (u16 k = 0; k < parent->LL_BoneCount(); k++)
    {
        CBoneData& BD = parent->LL_GetData(k);
        if (parent->LL_GetBoneVisible(k) && !BD.shape.flags.is(SBoneShape::sfNoPickable))
        {
            Fobb& obb = cache_obb[k];
            if (CDB::TestSphereOBB(test_sphere, obb))
                test_bones.push_back(k);
        }
    }

    // ok. allocate wallmark
    intrusive_ptr<CSkeletonWallmark> wm = xr_new<CSkeletonWallmark>(parent, parent_xform, shader, cp, Device.fTimeGlobal);
    wm->m_LocalBounds.set(cp, size * 2.f);
    wm->XFORM()->transform_tiny(wm->m_Bounds.P, cp);
    wm->m_Bounds.R = wm->m_LocalBounds.R;

    Fvector tmp;
    tmp.invert(D);
    normal.add(tmp).normalize();

    // build UV projection matrix
    Fmatrix mView, mRot;
    BuildMatrix(mView, 1 / (0.9f * size), normal, cp);
    mRot.rotateZ(::Random.randF(deg2rad(-20.f), deg2rad(20.f)));
    mView.mulA_43(mRot);

    // fill vertices
    for (u32 i = 0; i < parent->children.size(); i++)
    {
        CSkeletonX* S = parent->LL_GetChild(i);
        for (u16& test_bone : test_bones)
            S->FillVertices(mView, *wm, normal, size, test_bone);
    }

    {
        parent->wallmarks_lock.lock();

        // find similar wm and remove
        for (u32 wm_idx = 0; wm_idx < parent->wallmarks.size(); wm_idx++)
        {
            intrusive_ptr<CSkeletonWallmark>& wm = parent->wallmarks[wm_idx];
            if (wm->Similar(shader, cp, 0.02f))
            {
                if (wm_idx < parent->wallmarks.size() - 1)
                    wm = parent->wallmarks.back();
                parent->wallmarks.pop_back();
                break;
            }
        }

        // add new
        parent->wallmarks.push_back(wm);

        parent->wallmarks_lock.unlock();
    }
}

void CKinematics::AddWallmark(Fmatrix* parent_xform, const Fvector3& start, const Fvector3& dir, const ref_shader& shader, const float size)
{
    ZoneScoped;

    wallmarks_to_add.emplace_back(parent_xform, start, dir, shader, size);

    Device.add_to_seq_parallel(fastdelegate::MakeDelegate(this, &CKinematics::AddWallmarkAsync));
}

void CKinematics::AddWallmarkAsync()
{
    for (auto& item : wallmarks_to_add)
    {
        item.add_wallmark_internal(this);
    }

    wallmarks_to_add.clear();
}

void CKinematics::CalculateWallmarks(const bool hud) // called on render
{
    ZoneScoped;

    if (wm_frame == Device.dwFrame)
        return;

    wm_frame = Device.dwFrame;

    wallmarks_lock.lock();

    if (!wallmarks.empty())
    {
        std::erase_if(wallmarks, [&](const auto& wm) {
            if ((Device.fTimeGlobal - wm->TimeStart()) / ps_r__WallmarkTTL >= 1.f)
            {
                return true; // remove wallmark
            }

                // append wm to WallmarkEngine
                if (!hud && RImplementation.ViewBase.testSphere_dirty(wm->m_Bounds.P, wm->m_Bounds.R))
                    ::RImplementation.append_SkeletonWallmark(wm);

                return false;
        });
    }

    wallmarks_lock.unlock();
}

void CKinematics::RenderWallmark(intrusive_ptr<CSkeletonWallmark> wm, FVF::LIT*& V)
{
    VERIFY(wm);
    VERIFY(V);
    VERIFY(bones, "Invalid visual. Bones already released.");
    VERIFY(bone_instances, "Invalid visual. bone_instances already deleted.");

    if ((wm == nullptr) || (nullptr == bones) || (nullptr == bone_instances))
        return;

    ZoneScoped;

    // skin vertices
    for (u32 f_idx = 0; f_idx < wm->m_Faces.size(); f_idx++)
    {
        const CSkeletonWallmark::WMFace F = wm->m_Faces[f_idx];
        const float w = (Device.fTimeGlobal - wm->TimeStart()) / ps_r__WallmarkTTL;
        for (u32 k = 0; k < 3; k++)
        {
            Fvector P;
            if (F.bone_id[k][0] == F.bone_id[k][1])
            {
                // 1-link
                Fmatrix& xform0 = LL_GetBoneInstance(F.bone_id[k][0]).mRenderTransform;
                xform0.transform_tiny(P, F.vert[k]);
            }
            else if (F.bone_id[k][1] == F.bone_id[k][2])
            {
                // 2-link
                Fvector P0, P1;
                Fmatrix& xform0 = LL_GetBoneInstance(F.bone_id[k][0]).mRenderTransform;
                Fmatrix& xform1 = LL_GetBoneInstance(F.bone_id[k][1]).mRenderTransform;
                xform0.transform_tiny(P0, F.vert[k]);
                xform1.transform_tiny(P1, F.vert[k]);
                P.lerp(P0, P1, F.weight[k][0]);
            }
            else if (F.bone_id[k][2] == F.bone_id[k][3])
            {
                // 3-link
                Fvector P0, P1, P2;
                Fmatrix& xform0 = LL_GetBoneInstance(F.bone_id[k][0]).mRenderTransform;
                Fmatrix& xform1 = LL_GetBoneInstance(F.bone_id[k][1]).mRenderTransform;
                Fmatrix& xform2 = LL_GetBoneInstance(F.bone_id[k][2]).mRenderTransform;
                xform0.transform_tiny(P0, F.vert[k]);
                xform1.transform_tiny(P1, F.vert[k]);
                xform2.transform_tiny(P2, F.vert[k]);
                const float w0 = F.weight[k][0];
                const float w1 = F.weight[k][1];
                P0.mul(w0);
                P1.mul(w1);
                P2.mul(1 - w0 - w1);
                P = P0;
                P.add(P1);
                P.add(P2);
            }
            else
            {
                // 4-link
                Fvector PB[4];
                for (int i = 0; i < 4; ++i)
                {
                    Fmatrix& xform = LL_GetBoneInstance(F.bone_id[k][i]).mRenderTransform;
                    xform.transform_tiny(PB[i], F.vert[k]);
                }

                float s = 0.f;
                for (int i = 0; i < 3; ++i)
                {
                    PB[i].mul(F.weight[k][i]);
                    s += F.weight[k][i];
                }
                PB[3].mul(1 - s);

                P = PB[0];
                for (int i = 1; i < 4; ++i)
                    P.add(PB[i]);
            }
            wm->XFORM()->transform_tiny(V->p, P);
            V->t.set(F.uv[k]);
            int aC = iFloor(w * 255.f);
            clamp(aC, 0, 255);
            V->color = color_rgba(128, 128, 128, aC);
            V++;
        }
    }
    wm->XFORM()->transform_tiny(wm->m_Bounds.P, wm->m_LocalBounds.P);
}

void CKinematics::ClearWallmarks()
{
    Device.remove_from_seq_parallel(fastdelegate::MakeDelegate(this, &CKinematics::AddWallmarkAsync));

    wallmarks_to_add.clear();
    
    wallmarks_lock.lock();

    wallmarks.clear();

    wallmarks_lock.unlock();
}

int CKinematics::LL_GetBoneGroups(xr_vector<xr_vector<u16>>& groups)
{
    groups.resize(children.size());
    for (u16 bone_idx = 0; bone_idx < static_cast<u16>(bones->size()); bone_idx++)
    {
        const CBoneData* B = (*bones)[bone_idx];
        for (u32 child_idx = 0; child_idx < children.size(); child_idx++)
        {
            if (!B->child_faces[child_idx].empty())
            {
                groups[child_idx].push_back(bone_idx);
            }
        }
    }
    return groups.size();
}

#ifdef DEBUG
CSkeletonWallmark::~CSkeletonWallmark()
{
    if (used_in_render != u32(-1))
    {
        Msg("used_in_render=%d", used_in_render);
        VERIFY(used_in_render == u32(-1));
    }
}
#endif

/************************* Add by Zander *************************************************************/
/************************* Дебаговые функции для исследования моделей. *******************************/
static Fbox& get_mesh_RC_data(const FHierrarhyVisual* pHV, const u32 id) { return pHV->children.at(id)->getVisData().box; }

Fvector3 CKinematics::RC_VisBox(const u32 id)
{
    const Fbox& t = get_mesh_RC_data(this, id);
    Fvector3 modelSize;
    t.getsize(modelSize);
    return modelSize;
}

Fvector3 CKinematics::RC_VisCenter(const u32 id)
{
    const Fbox& t = get_mesh_RC_data(this, id);
    Fvector3 modelSize;
    t.getcenter(modelSize);
    return modelSize;
}

Fvector3 CKinematics::RC_VisBorderMin(const u32 id)
{
    const Fbox& t = get_mesh_RC_data(this, id);
    Fvector3 borderModelMin;
    borderModelMin.set(t.min);
    return borderModelMin;
}

Fvector3 CKinematics::RC_VisBorderMax(const u32 id)
{
    const Fbox& t = get_mesh_RC_data(this, id);
    Fvector3 borderModelMax;
    borderModelMax.set(t.max);
    return borderModelMax;
}

void CKinematics::RC_Dump()
{
    Msg("|********** Dump children meshes **************|");
    const Fbox& mBox = getVisData().box;
    Fvector3 temp;
    mBox.getsize(temp);
    Msg("[core] : box[%3.3f,%3.3f,%3.3f]", temp.x, temp.y, temp.z);
    mBox.getcenter(temp);
    Msg("[core] : center[%3.3f,%3.3f,%3.3f]", temp.x, temp.y, temp.z);
    temp.set(mBox.min);
    Msg("[core] : min[%3.3f,%3.3f,%3.3f]", temp.x, temp.y, temp.z);
    temp.set(mBox.max);
    Msg("[core] : max[%3.3f,%3.3f,%3.3f]", temp.x, temp.y, temp.z);
    for (u32 i = 0; i < children.size(); i++)
    {
        temp.set(RC_VisBox(i));
        Msg("[child %u] : box[%3.3f,%3.3f,%3.3f]", i, temp.x, temp.y, temp.z);
        temp.set(RC_VisCenter(i));
        Msg("[child %u] : center[%3.3f,%3.3f,%3.3f]", i, temp.x, temp.y, temp.z);
        temp.set(RC_VisBorderMin(i));
        Msg("[child %u] : min[%3.3f,%3.3f,%3.3f]", i, temp.x, temp.y, temp.z);
        temp.set(RC_VisBorderMax(i));
        Msg("[child %u] : max[%3.3f,%3.3f,%3.3f]", i, temp.x, temp.y, temp.z);
        FHierrarhyVisual* HV = smart_cast<FHierrarhyVisual*>(children.at(i));
        if (HV && HV->children.size())
        {
            for (u32 j = 0; j < HV->children.size(); j++)
            {
                const Fbox& FB = get_mesh_RC_data(HV, j);
                FB.getsize(temp);
                Msg("[child %u->%u] : box[%3.3f,%3.3f,%3.3f]", i, j, temp.x, temp.y, temp.z);
                FB.getcenter(temp);
                Msg("[child %u->%u] : center[%3.3f,%3.3f,%3.3f]", i, j, temp.x, temp.y, temp.z);
                temp.set(FB.min);
                Msg("[child %u->%u] : min[%3.3f,%3.3f,%3.3f]", i, j, temp.x, temp.y, temp.z);
                temp.set(FB.max);
                Msg("[child %u->%u] : max[%3.3f,%3.3f,%3.3f]", i, j, temp.x, temp.y, temp.z);
                FHierrarhyVisual* kHV = smart_cast<FHierrarhyVisual*>(HV->children.at(j));
                if (kHV && kHV->children.size())
                {
                    for (u32 k = 0; k < kHV->children.size(); k++)
                    {
                        const Fbox& kFB = get_mesh_RC_data(kHV, k);
                        kFB.getsize(temp);
                        Msg("[child %u->%u->%u] : box[%3.3f,%3.3f,%3.3f]", i, j, k, temp.x, temp.y, temp.z);
                        kFB.getcenter(temp);
                        Msg("[child %u->%u->%u] : center[%3.3f,%3.3f,%3.3f]", i, j, k, temp.x, temp.y, temp.z);
                        temp.set(kFB.min);
                        Msg("[child %u->%u->%u] : min[%3.3f,%3.3f,%3.3f]", i, j, k, temp.x, temp.y, temp.z);
                        temp.set(kFB.max);
                        Msg("[child %u->%u->%u] : max[%3.3f,%3.3f,%3.3f]", i, j, k, temp.x, temp.y, temp.z);
                    }
                }
            }
        }
    }

    Msg("|********** End Dump children meshes **********|");

    Msg("|********** Dump children bones ***************|");
    for (u32 i = 0; i < bones->size(); i++)
    {
        const CBoneData* B = (*bones)[i];
        Msg("Bone [%u][%s]:", i, LL_BoneName(static_cast<u16>(i)));
        Msg("bind_transform[%3.3f,%3.3f,%3.3f] m2b_transform[%3.3f,%3.3f,%3.3f] center_of_mass[%3.3f,%3.3f,%3.3f]", B->bind_transform.c.x, B->bind_transform.c.y,
            B->bind_transform.c.z, B->m2b_transform.c.x, B->m2b_transform.c.y, B->m2b_transform.c.z, B->center_of_mass.x, B->center_of_mass.y, B->center_of_mass.z);
    }
    Msg("|********** End Dump children bones ***********|");
}
/************************* End add *************************************/

u16 CKinematics::LL_VisibleBoneCount() {
    u16 cnt = 0;
    for (u16 i = 0; i < LL_BoneCount(); i++)
        if (LL_GetBoneVisible(i)) cnt++;
    return cnt;
}