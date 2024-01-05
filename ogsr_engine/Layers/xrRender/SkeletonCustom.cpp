#include "stdafx.h"

#include "SkeletonCustom.h"
#include "SkeletonX.h"
#include "../../xr_3da/fmesh.h"
#include "../../xr_3da/Render.h"
#include "../../COMMON_AI/smart_cast.h"

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

const char* CKinematics::LL_BoneName_dbg(const u16 ID) const
{
    for (const auto& bone : bone_map_N)
        if (bone.second == ID)
            return bone.first.c_str();

    return nullptr;
}

#ifdef DEBUG
void CKinematics::DebugRender(Fmatrix& XFORM)
{
    RCache.set_Shader(dxRenderDeviceRender::Instance().m_WireShader);

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
        RCache.dbg_DrawLINE(XFORM, P1, P2, D3DCOLOR_XRGB(0, 255, 0));

        Fmatrix M;
        M.mul_43(XFORM, M2);
        RCache.dbg_DrawOBB(M, H1, D3DCOLOR_XRGB(255, 255, 255));
        RCache.dbg_DrawOBB(M, H2, D3DCOLOR_XRGB(255, 255, 255));
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
        RCache.dbg_DrawOBB(W, obb.m_halfsize, D3DCOLOR_XRGB(0, 0, 255));
    }
}
#endif

CKinematics::CKinematics()
{
#ifdef DEBUG
    dbg_single_use_marker = FALSE;
#endif

    m_is_original_lod = false;
}

CKinematics::~CKinematics()
{
    IBoneInstances_Destroy();
    // wallmarks
    ClearWallmarks();

    if (m_lod)
    {
        if (CKinematics* lod_kinematics = dynamic_cast<CKinematics*>(m_lod))
        {
            if (lod_kinematics->m_is_original_lod)
            {
                lod_kinematics->Release();
            }
        }

        xr_delete(m_lod);
    }
}

void CKinematics::IBoneInstances_Create()
{
    // VERIFY2				(bones->size() < 64, "More than 64 bones is a crazy thing!");
    u32 size = bones->size();
    bone_instances = xr_alloc<CBoneInstance>(size);
    for (u32 i = 0; i < size; i++)
        bone_instances[i].construct();
}

void CKinematics::IBoneInstances_Destroy()
{
    if (bone_instances)
    {
        xr_free(bone_instances);
        bone_instances = NULL;
    }
}

CSkeletonX* CKinematics::LL_GetChild(u32 idx)
{
    IRenderVisual* V = children[idx];
    CSkeletonX* B = dynamic_cast<CSkeletonX*>(V);
    return B;
}

void CKinematics::Load(const char* N, IReader* data, u32 dwFlags)
{
    // Msg				("skeleton: %s",N);
    inherited::Load(N, data, dwFlags);

    pUserData = NULL;
    m_lod = NULL;
    // loading lods

    IReader* LD = data->open_chunk(OGF_S_LODS);
    if (LD)
    {
        // From stream
        {
            string_path lod_name;
            LD->r_string(lod_name, sizeof(lod_name));
            //.         strconcat		(sizeof(name_load),name_load, short_name, ":lod:", lod_name.c_str());
            m_lod = (dxRender_Visual*)::Render->model_CreateChild(lod_name, NULL);

            if (CKinematics* lod_kinematics = dynamic_cast<CKinematics*>(m_lod))
            {
                lod_kinematics->m_is_original_lod = true;
            }

            VERIFY3(m_lod, "Cant create LOD model for", N);
            //.			VERIFY2			(m_lod->Type==MT_HIERRARHY || m_lod->Type==MT_PROGRESSIVE || m_lod->Type==MT_NORMAL,lod_name.c_str());
            /*
                        strconcat		(name_load, short_name, ":lod:1");
                        m_lod 			= ::Render->model_CreateChild(name_load,LD);
                        VERIFY			(m_lod->Type==MT_SKELETON_GEOMDEF_PM || m_lod->Type==MT_SKELETON_GEOMDEF_ST);
            */
        }
        LD->close();
    }

    string_path ini_path;
    strcpy_s(ini_path, N);
    if (strext(ini_path))
        *strext(ini_path) = 0;
    strcat_s(ini_path, ".ltx");

    // try to read custom user data for module from ltx file
    IReader* UD = FS.r_open("$game_meshes$", ini_path);
    pUserData = UD ? xr_new<CInifile>(UD, FS.get_path("$game_config$")->m_Path) : nullptr;
    if (UD)
        UD->close();

    // read user data from model if no custom found
    if (!pUserData)
    {
        // User data
        UD = data->open_chunk(OGF_S_USERDATA);
        pUserData = UD ? xr_new<CInifile>(UD, FS.get_path("$game_config$")->m_Path) : nullptr;
        if (UD)
            UD->close();
    }

    // Globals
    bones = xr_new<vecBones>();
    bone_instances = NULL;

    // Load bones
#pragma todo("container is created in stack!")
    xr_vector<shared_str> L_parents;

    R_ASSERT(data->find_chunk(OGF_S_BONE_NAMES));

    visimask.zero();

    u32 dwCount = data->r_u32();
    // Msg				("!!! %d bones",dwCount);
    // if (dwCount >= 64)	Msg			("!!! More than 64 bones is a crazy thing! (%d), %s",dwCount,N);
    // VERIFY3			(dwCount <= 64, "More than 64 bones is a crazy thing!",N);

    bone_map_N.reserve(dwCount);

    for (; dwCount; dwCount--)
    {
        string256 buf;

        // Bone
        u16 ID = u16(bones->size());
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
        L_parents.push_back(buf);

        data->r(&pBone->obb, sizeof(Fobb));
        visimask.set(ID, true);
    }

    // Attach bones to their parents
    iRoot = BI_NONE;
    for (u32 i = 0; i < bones->size(); i++)
    {
        shared_str P = L_parents[i];
        CBoneData* B = (*bones)[i];
        if (!P || !P[0])
        {
            // no parent - this is root bone
            R_ASSERT(BI_NONE == iRoot);
            iRoot = u16(i);
            B->SetParentID(BI_NONE);
            continue;
        }
        else
        {
            u16 ID = LL_BoneID(P);
            R_ASSERT(ID != BI_NONE);
            (*bones)[ID]->children.push_back(B);
            B->SetParentID(ID);
        }
    }
    R_ASSERT(BI_NONE != iRoot);

    // Free parents
    L_parents.clear();

    // IK data
    IReader* IKD = data->open_chunk(OGF_S_IKDATA);
    if (IKD)
    {
        bool fix_cop_joints = pUserData ? READ_IF_EXISTS(pUserData, r_bool, "compat", "fix_cop_joints", false) : false;
        for (u32 i = 0; i < bones->size(); i++)
        {
            CBoneData* B = (*bones)[i];
            u16 vers = (u16)IKD->r_u32();
            IKD->r_stringZ(B->game_mtl_name);
            IKD->r(&B->shape, sizeof(SBoneShape));
            B->IK_data.Import(*IKD, vers);
            Fvector vXYZ, vT;
            IKD->r_fvector3(vXYZ);
            IKD->r_fvector3(vT);
            B->bind_transform.setXYZi(vXYZ);
            B->bind_transform.translate_over(vT);
            B->mass = IKD->r_float();
            IKD->r_fvector3(B->center_of_mass);

            if (fix_cop_joints)
            {
                // https://bitbucket.org/stalker/xray_re-tools/commits/209b9014129ceeb7d92375a77f60835553266bf1
                for (auto& it : B->IK_data.limits)
                {
                    Fvector2 vec = it.limit;
                    float tmp = vec.x;
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
        for (u16 child_idx = 0; child_idx < (u16)children.size(); child_idx++)
            LL_GetChild(child_idx)->AfterLoad(this, child_idx);
    }

    // unique bone faces
    {
        for (u32 bone_idx = 0; bone_idx < bones->size(); bone_idx++)
        {
            CBoneData* B = (*bones)[bone_idx];
            for (u32 child_idx = 0; child_idx < children.size(); child_idx++)
            {
                CBoneData::FacesVec faces = B->child_faces[child_idx];
                std::sort(faces.begin(), faces.end());
                CBoneData::FacesVecIt new_end = std::unique(faces.begin(), faces.end());
                faces.erase(new_end, faces.end());
                B->child_faces[child_idx].clear();
                B->child_faces[child_idx] = faces;
            }
        }
    }

    // reset update_callback
    Update_Callback = NULL;
    // reset update frame
    wm_frame = u32(-1);

    LL_Validate();
}

IC void iBuildGroups(CBoneData* B, U16Vec& tgt, u16 id, u16& last_id)
{
    if (B->IK_data.ik_flags.is(SJointIKData::flBreakable))
        id = ++last_id;
    tgt[B->GetSelfID()] = id;
    for (xr_vector<CBoneData*>::iterator bone_it = B->children.begin(); bone_it != B->children.end(); bone_it++)
        iBuildGroups(*bone_it, tgt, id, last_id);
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

#pragma todo("container is created in stack!")
        xr_vector<xr_vector<u16>> groups;
        LL_GetBoneGroups(groups);

#pragma todo("container is created in stack!")
        xr_vector<u16> b_parts(LL_BoneCount(), BI_NONE);
        CBoneData* root = &LL_GetData(LL_GetBoneRoot());
        u16 last_id = 0;
        iBuildGroups(root, b_parts, 0, last_id);

        for (u16 g = 0; g < (u16)groups.size(); ++g)
        {
            xr_vector<u16>& group = groups[g];
            u16 bp_id = b_parts[group[0]];
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

    CKinematics* pFrom = dynamic_cast<CKinematics*>(P);
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

    m_lod = (pFrom->m_lod) ? (dxRender_Visual*)::Render->model_Duplicate(pFrom->m_lod) : 0;
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
    Update_Callback = NULL;
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
    for (u32 i = 0; i < bones->size(); i++)
    {
        CBoneData*& B = (*bones)[i];
        xr_delete(B);
    }

    // destroy shared data
    xr_delete(pUserData);
    xr_delete(bones);
    bone_map_N.clear();

    inherited::Release();
}

void CKinematics::LL_SetBoneVisible(u16 bone_id, BOOL val, BOOL bRecursive)
{
    R_ASSERT(bone_id < LL_BoneCount());
    visimask.set(bone_id, !!val);
    if (!visimask.is(bone_id))
    {
        bone_instances[bone_id].mTransform.scale(0.f, 0.f, 0.f);
    }
    else
    {
        CalculateBones_Invalidate();
    }
    bone_instances[bone_id].mRenderTransform.mul_43(bone_instances[bone_id].mTransform, (*bones)[bone_id]->m2b_transform);
    if (bRecursive)
    {
        for (xr_vector<CBoneData*>::iterator C = (*bones)[bone_id]->children.begin(); C != (*bones)[bone_id]->children.end(); C++)
            LL_SetBoneVisible((*C)->GetSelfID(), val, bRecursive);
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
    Update_Visibility = FALSE;
    // check visible
    for (u32 c_it = 0; c_it < children.size(); c_it++)
    {
        CSkeletonX* _c = dynamic_cast<CSkeletonX*>(children[c_it]);
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
        CSkeletonX* _c = dynamic_cast<CSkeletonX*>(children_invisible[_it]);
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

IC static void RecursiveBindTransform(CKinematics* K, xr_vector<Fmatrix>& matrices, u16 bone_id, const Fmatrix& parent)
{
    CBoneData& BD = K->LL_GetData(bone_id);
    Fmatrix& BM = matrices[bone_id];
    // Build matrix
    BM.mul_43(parent, BD.bind_transform);
    for (xr_vector<CBoneData*>::iterator C = BD.children.begin(); C != BD.children.end(); C++)
        RecursiveBindTransform(K, matrices, (*C)->GetSelfID(), BM);
}

void CKinematics::LL_GetBindTransform(xr_vector<Fmatrix>& matrices)
{
    matrices.resize(LL_BoneCount());
    RecursiveBindTransform(this, matrices, iRoot, Fidentity);
}

void BuildMatrix(Fmatrix& mView, float invsz, const Fvector norm, const Fvector& from)
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
void CKinematics::EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id)
{
    for (u32 i = 0; i < children.size(); i++)
        LL_GetChild(i)->EnumBoneVertices(C, bone_id);
}
#include "cl_intersect.h"

DEFINE_VECTOR(Fobb, OBBVec, OBBVecIt);

bool CKinematics::PickBone(const Fmatrix& parent_xform, IKinematics::pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id)
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

void CKinematics::AddWallmark(const Fmatrix* parent_xform, const Fvector3& start, const Fvector3& dir, ref_shader shader, float size)
{
    Fvector S, D, normal = {0, 0, 0};
    // transform ray from world to model
    Fmatrix P;
    P.invert(*parent_xform);
    P.transform_tiny(S, start);
    P.transform_dir(D, dir);
    // find pick point
    float dist = flt_max;
    BOOL picked = FALSE;

    DEFINE_VECTOR(Fobb, OBBVec, OBBVecIt);
    OBBVec cache_obb;
    cache_obb.resize(LL_BoneCount());
    IKinematics::pick_result r;
    r.normal = normal;
    r.dist = dist;
    for (u16 k = 0; k < LL_BoneCount(); k++)
    {
        CBoneData& BD = LL_GetData(k);
        if (LL_GetBoneVisible(k) && !BD.shape.flags.is(SBoneShape::sfNoPickable))
        {
            Fobb& obb = cache_obb[k];
            obb.transform(BD.obb, LL_GetBoneInstance(k).mTransform);
            if (CDB::TestRayOBB(S, D, obb))
                for (u32 i = 0; i < children.size(); i++)
                {
                    if (LL_GetChild(i)->PickBone(r, dist, S, D, k))
                    {
                        picked = TRUE;
                        dist = r.dist;
                        normal = r.normal;
                        // dynamics set wallmarks bug fix
                    }
                }
        }
    }
    if (!picked)
        return;

    // calculate contact point
    Fvector cp;
    cp.mad(S, D, dist);

    // collect collide boxes
    Fsphere test_sphere;
    test_sphere.set(cp, size);
    U16Vec test_bones;
    test_bones.reserve(LL_BoneCount());
    for (u16 k = 0; k < LL_BoneCount(); k++)
    {
        CBoneData& BD = LL_GetData(k);
        if (LL_GetBoneVisible(k) && !BD.shape.flags.is(SBoneShape::sfNoPickable))
        {
            Fobb& obb = cache_obb[k];
            if (CDB::TestSphereOBB(test_sphere, obb))
                test_bones.push_back(k);
        }
    }

    // find similar wm
    for (u32 wm_idx = 0; wm_idx < wallmarks.size(); wm_idx++)
    {
        intrusive_ptr<CSkeletonWallmark>& wm = wallmarks[wm_idx];
        if (wm->Similar(shader, cp, 0.02f))
        {
            if (wm_idx < wallmarks.size() - 1)
                wm = wallmarks.back();
            wallmarks.pop_back();
            break;
        }
    }

    // ok. allocate wallmark
    intrusive_ptr<CSkeletonWallmark> wm = xr_new<CSkeletonWallmark>(this, parent_xform, shader, cp, RDEVICE.fTimeGlobal);
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
    for (u32 i = 0; i < children.size(); i++)
    {
        CSkeletonX* S = LL_GetChild(i);
        for (U16It b_it = test_bones.begin(); b_it != test_bones.end(); b_it++)
            S->FillVertices(mView, *wm, normal, size, *b_it);
    }

    wallmarks.push_back(wm);
}

void CKinematics::CalculateWallmarks()
{
    if (!wallmarks.empty() && (wm_frame != RDEVICE.dwFrame))
    {
        wm_frame = RDEVICE.dwFrame;
        bool need_remove = false;
        for (SkeletonWMVecIt it = wallmarks.begin(); it != wallmarks.end(); it++)
        {
            intrusive_ptr<CSkeletonWallmark>& wm = *it;
            float w = (RDEVICE.fTimeGlobal - wm->TimeStart()) / ps_r__WallmarkTTL;
            if (w < 1.f)
            {
                // append wm to WallmarkEngine
                if (::Render->ViewBase.testSphere_dirty(wm->m_Bounds.P, wm->m_Bounds.R))
                    //::Render->add_SkeletonWallmark	(wm);
                    ::RImplementation.add_SkeletonWallmark(wm);
            }
            else
            {
                // remove wallmark
                need_remove = true;
            }
        }
        if (need_remove)
        {
            auto new_end = std::remove_if(wallmarks.begin(), wallmarks.end(), [](const auto& x) { return x == 0; });
            wallmarks.erase(new_end, wallmarks.end());
        }
    }
}

void CKinematics::RenderWallmark(intrusive_ptr<CSkeletonWallmark> wm, FVF::LIT*& V)
{
    VERIFY(wm);
    VERIFY(V);
    VERIFY2(bones, "Invalid visual. Bones already released.");
    VERIFY2(bone_instances, "Invalid visual. bone_instances already deleted.");

    if ((wm == 0) || (0 == bones) || (0 == bone_instances))
        return;

    // skin vertices
    for (u32 f_idx = 0; f_idx < wm->m_Faces.size(); f_idx++)
    {
        CSkeletonWallmark::WMFace F = wm->m_Faces[f_idx];
        float w = (RDEVICE.fTimeGlobal - wm->TimeStart()) / ps_r__WallmarkTTL;
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
                float w0 = F.weight[k][0];
                float w1 = F.weight[k][1];
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
    //	for (SkeletonWMVecIt it=wallmarks.begin(); it!=wallmarks.end(); it++)
    //		xr_delete	(*it);
    wallmarks.clear();
}

int CKinematics::LL_GetBoneGroups(xr_vector<xr_vector<u16>>& groups)
{
    groups.resize(children.size());
    for (u16 bone_idx = 0; bone_idx < (u16)bones->size(); bone_idx++)
    {
        CBoneData* B = (*bones)[bone_idx];
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
static Fbox& get_mesh_RC_data(FHierrarhyVisual* pHV, const u32 id) { return pHV->children.at(id)->getVisData().box; }

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
        FHierrarhyVisual* HV = dynamic_cast<FHierrarhyVisual*>(children.at(i));
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
                FHierrarhyVisual* kHV = dynamic_cast<FHierrarhyVisual*>(HV->children.at(j));
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
        CBoneData* B = (*bones)[i];
        Msg("Bone [%u][%s]:", i, LL_BoneName_dbg(u16(i)));
        Msg("bind_transform[%3.3f,%3.3f,%3.3f] m2b_transform[%3.3f,%3.3f,%3.3f] center_of_mass[%3.3f,%3.3f,%3.3f]", B->bind_transform.c.x, B->bind_transform.c.y,
            B->bind_transform.c.z, B->m2b_transform.c.x, B->m2b_transform.c.y, B->m2b_transform.c.z, B->center_of_mass.x, B->center_of_mass.y, B->center_of_mass.z);
    }
    Msg("|********** End Dump children bones ***********|");
}
/************************* End add *************************************/
