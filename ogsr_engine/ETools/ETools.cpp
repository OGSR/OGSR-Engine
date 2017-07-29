#include	"stdafx.h"
#include	"ETools.h"
#include	"xrXRC.h"
#include "Shobjidl.h"

#pragma warning(disable:4267)

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  fdwReason, 
					  LPVOID lpReserved
					  )
{ 
	switch(fdwReason) {
		case DLL_PROCESS_ATTACH:
			Core._initialize("XRayEditorTools",0,FALSE);
			//FPU::m64r	();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			Core._destroy();
			break;
	}
	return TRUE;
}

namespace ETOOLS{
	ETOOLS_API bool  __stdcall TestRayTriA(const Fvector& C, const Fvector& D, Fvector** p, float& u, float& v, float& range, bool bCull)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float det,inv_det;
		// find vectors for two edges sharing vert0
		edge1.sub(*p[1], *p[0]);
		edge2.sub(*p[2], *p[0]);
		// begin calculating determinant - also used to calculate U parameter
		pvec.crossproduct(D, edge2);
		// if determinant is near zero, ray lies in plane of triangle
		det = edge1.dotproduct(pvec);
		if (bCull){						// define TEST_CULL if culling is desired
			if (det < EPS)  return false;
			tvec.sub(C, *p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec);			// calculate U parameter and test bounds
			if (u < 0.0 || u > det) return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec);			// calculate V parameter and test bounds
			if (v < 0.0 || u + v > det) return false;
			range = edge2.dotproduct(qvec);		// calculate t, scale parameters, ray intersects triangle
			inv_det = 1.0f / det;
			range *= inv_det;
			u *= inv_det;
			v *= inv_det;
		}else{											// the non-culling branch
			if (det > -EPS && det < EPS) return false;
			inv_det = 1.0f / det;
			tvec.sub(C, *p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec)*inv_det;	// calculate U parameter and test bounds
			if (u < 0.0f || u > 1.0f)    return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec)*inv_det;	// calculate V parameter and test bounds
			if (v < 0.0f || u + v > 1.0f) return false;
			range = edge2.dotproduct(qvec)*inv_det;// calculate t, ray intersects triangle
		}
		return true;
	}
	//-- Ray-Triangle : 1st level of indirection --------------------------------
	ETOOLS_API bool  __stdcall TestRayTriB(const Fvector& C, const Fvector& D, Fvector* p, float& u, float& v, float& range, bool bCull)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float det,inv_det;
		// find vectors for two edges sharing vert0
		edge1.sub(p[1], p[0]);
		edge2.sub(p[2], p[0]);
		// begin calculating determinant - also used to calculate U parameter
		pvec.crossproduct(D, edge2);
		// if determinant is near zero, ray lies in plane of triangle
		det = edge1.dotproduct(pvec);
		if (bCull){						// define TEST_CULL if culling is desired
			if (det < EPS)  return false;
			tvec.sub(C, p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec);			// calculate U parameter and test bounds
			if (u < 0.0f || u > det) return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec);			// calculate V parameter and test bounds
			if (v < 0.0f || u + v > det) return false;
			range = edge2.dotproduct(qvec);		// calculate t, scale parameters, ray intersects triangle
			inv_det = 1.0f / det;
			range *= inv_det;
			u *= inv_det;
			v *= inv_det;
		}else{											// the non-culling branch
			if (det > -EPS && det < EPS) return false;
			inv_det = 1.0f / det;
			tvec.sub(C, p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec)*inv_det;	// calculate U parameter and test bounds
			if (u < 0.0f || u > 1.0f)    return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec)*inv_det;	// calculate V parameter and test bounds
			if (v < 0.0f || u + v > 1.0f) return false;
			range = edge2.dotproduct(qvec)*inv_det;// calculate t, ray intersects triangle
		}
		return true;
	}
	//-- Ray-Triangle(always return range) : 1st level of indirection --------------------------------
	ETOOLS_API bool  __stdcall TestRayTri2(const Fvector& C, const Fvector& D, Fvector* p, float& range)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float det,inv_det,u,v;

		// find vectors for two edges sharing vert0
		edge1.sub(p[1], p[0]);
		edge2.sub(p[2], p[0]);
		// begin calculating determinant - also used to calculate U parameter
		pvec.crossproduct(D, edge2);
		// if determinant is near zero, ray lies in plane of triangle
		det = edge1.dotproduct(pvec);

		if (_abs(det) < EPS_S)		{ range=-1; return false; }
		inv_det = 1.0f / det;
		tvec.sub(C, p[0]);					// calculate distance from vert0 to ray origin
		u = tvec.dotproduct(pvec)*inv_det;	// calculate U parameter and test bounds
		qvec.crossproduct(tvec, edge1);		// prepare to test V parameter
		range = edge2.dotproduct(qvec)*inv_det;// calculate t, ray intersects plane
		if (u < 0.0f || u > 1.0f)		return false;
		v = D.dotproduct(qvec)*inv_det;		// calculate V parameter and test bounds
		if (v < 0.0f || u + v > 1.0f) return false;
		return true;
	}

	ETOOLS_API CDB::Collector*			 __stdcall create_collector		()
	{
		return							xr_new<CDB::Collector>	();
	}
	ETOOLS_API void						 __stdcall destroy_collector		(CDB::Collector*& M)
	{
		xr_delete						(M);
	}
#ifdef _WIN64
	ETOOLS_API void						 __stdcall collector_add_face_d	(CDB::Collector* CL, const Fvector& v0, const Fvector& v1, const Fvector& v2, u64 dummy)
	{
		CL->add_face_D					(v0,v1,v2,dummy);
	}
	ETOOLS_API void						 __stdcall collector_add_face_pd	(CDB::Collector* CL, const Fvector& v0, const Fvector& v1, const Fvector& v2, u64 dummy, float eps)
	{
		CL->add_face_packed_D			(v0,v1,v2,dummy,eps);
	}
	ETOOLS_API void						 __stdcall collectorp_add_face_d	(CDB::CollectorPacked* CL, const Fvector& v0, const Fvector& v1, const Fvector& v2, u64 dummy)
	{
		CL->add_face_D					(v0,v1,v2,dummy);
	}
#else
	ETOOLS_API void						 __stdcall collector_add_face_d	(CDB::Collector* CL, const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy)
	{
		CL->add_face_D					(v0,v1,v2,dummy);
	}
	ETOOLS_API void						 __stdcall collector_add_face_pd	(CDB::Collector* CL, const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy, float eps)
	{
		CL->add_face_packed_D			(v0,v1,v2,dummy,eps);
	}
	ETOOLS_API void						 __stdcall collectorp_add_face_d	(CDB::CollectorPacked* CL, const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy)
	{
		CL->add_face_D					(v0,v1,v2,dummy);
	}
#endif

	ETOOLS_API CDB::CollectorPacked*	 __stdcall create_collectorp		(const Fbox &bb, int apx_vertices, int apx_faces)
	{
		return							xr_new<CDB::CollectorPacked>	(bb, apx_vertices, apx_faces);
	}
	ETOOLS_API void						 __stdcall destroy_collectorp		(CDB::CollectorPacked*& M)
	{
		xr_delete						(M);
	}

	ETOOLS_API CDB::COLLIDER* __stdcall get_collider	(){return XRC.collider();}
	ETOOLS_API CDB::MODEL*	 __stdcall create_model_clp(CDB::CollectorPacked* CL)
	{
		return				create_model(CL->getV(), CL->getVS(), CL->getT(), CL->getTS());
	}
	ETOOLS_API CDB::MODEL*	 __stdcall create_model_cl	(CDB::Collector* CL)
	{
		return				create_model(CL->getV(), CL->getVS(), CL->getT(), CL->getTS());
	}
	ETOOLS_API CDB::MODEL*	 __stdcall create_model	(Fvector* V, int Vcnt, CDB::TRI* T, int Tcnt)
	{
		CDB::MODEL* M		= xr_new<CDB::MODEL> ();
		M->build			(V, Vcnt, T, Tcnt);
		return M;
	}
	ETOOLS_API void			 __stdcall destroy_model	(CDB::MODEL*& M)
	{
		xr_delete			(M);
	}
	ETOOLS_API CDB::RESULT*	 __stdcall r_begin	()	{	return XRC.r_begin();		};
	ETOOLS_API CDB::RESULT*	 __stdcall r_end	()	{	return XRC.r_end();			};
	ETOOLS_API int	 __stdcall r_count			()	{	return XRC.r_count();		};
	ETOOLS_API void  __stdcall ray_options	(u32 flags)
	{
		XRC.ray_options(flags);
	}
	ETOOLS_API void	 __stdcall ray_query	(const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range)
	{
		XRC.ray_query(m_def,r_start,r_dir,r_range);
	}
	ETOOLS_API void	 __stdcall ray_query_m	(const Fmatrix& inv_parent, const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range)
	{
		XRC.ray_query(inv_parent,m_def,r_start,r_dir,r_range);
	}
	ETOOLS_API void  __stdcall box_options	(u32 flags)
	{
		XRC.box_options(flags);
	}
	ETOOLS_API void	 __stdcall box_query	(const CDB::MODEL *m_def, const Fvector& b_center, const Fvector& b_dim)
	{
		XRC.box_query(m_def, b_center, b_dim);
	}
	ETOOLS_API void	 __stdcall box_query_m	(const Fmatrix& inv_parent, const CDB::MODEL *m_def, const Fbox& src)
	{
		XRC.box_query(inv_parent, m_def, src);
	}
	ETOOLS_API bool	__stdcall	GetOpenNameImpl(string_path& buffer, FS_Path& P, bool bMulti, LPCSTR offset, int start_flt_ext)
	{
		// а теперь все надо перевести в юникод
		// 1. дефолтное расширение
		WCHAR wDefExt[32];
		MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			P.m_DefExt, -1, wDefExt, 32);

		// 2. начальная папка
		string_path id;
		GetCurrentDirectory(520, id);
		strcat(id, "\\");
		strcat(id, (offset&&offset[0]) ? offset : P.m_Path);

		WCHAR wInitial[520];
		MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			id/*(offset&&offset[0]) ? offset : P.m_Path*/, -1, wInitial, 520
		);

		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
		bool bRes = false;
		if (SUCCEEDED(hr))
		{
			pFileOpen->SetDefaultExtension(wDefExt);
			pFileOpen->SetTitle(L"Open a File");
			pFileOpen->SetOptions(FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST | FOS_NOREADONLYRETURN | FOS_NOCHANGEDIR | (bMulti ? FOS_ALLOWMULTISELECT : 0));

			IShellItem *inFolder;
			hr = SHCreateItemFromParsingName(wInitial, NULL, IID_PPV_ARGS(&inFolder));

			if (SUCCEEDED(hr))
				pFileOpen->SetDefaultFolder(inFolder);

			//  фильтр			
			WCHAR wInfo[64];
			MultiByteToWideChar(
				CP_ACP,
				MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
				P.m_FilterCaption ? P.m_FilterCaption : "", -1, wInfo, 64);

			if (P.m_DefExt)
			{
				COMDLG_FILTERSPEC flt[2];
				flt[0].pszName = P.m_FilterCaption ? wInfo : L"";
				flt[0].pszSpec = wDefExt;
				flt[1] = { L"All files", L"*.*" };
				pFileOpen->SetFileTypes(2, flt);
			}
			else
			{
				COMDLG_FILTERSPEC flt[1];
				flt[0] = { L"All files", L"*.*" };
				pFileOpen->SetFileTypes(1, flt);
			}

			// Show the Open dialog box.
			hr = pFileOpen->Show(GetForegroundWindow());

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				bRes = true;
				if (bMulti)
				{
					IShellItemArray *pItemArray;
					hr = pFileOpen->GetResults(&pItemArray);
					if (SUCCEEDED(hr))
					{
						DWORD cSelItems;
						// Get the number of selected files.
						hr = pItemArray->GetCount(&cSelItems);
						for (DWORD j = 0; j < cSelItems; j++)
						{
							string_path buf;
							IShellItem *pItem;
							hr = pItemArray->GetItemAt(j, &pItem);

							PWSTR pszFilePath;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

							WideCharToMultiByte(
								CP_ACP, NULL,
								pszFilePath, -1, buf, 520, NULL, NULL);

							strcat(buffer, buf);
							if (j != cSelItems)
								strcat(buffer, ",");

							pItem->Release();
						}

					}
				}
				else
				{
					IShellItem *pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

						WideCharToMultiByte(
							CP_ACP, NULL,
							pszFilePath, -1, buffer, 520, NULL, NULL);

						pItem->Release();
					}
				}
				strlwr(buffer);
			}
			else
			{
				switch (hr)
				{
				case ERROR_CANCELLED:	Log("The user closed the window by cancelling the operation."); break;
				}
			}
			pFileOpen->Release();
		}
		return bRes;
	}
	ETOOLS_API bool	__stdcall	GetSaveNameImpl(string_path& buffer, FS_Path& P, LPCSTR offset, int start_flt_ext)
	{
		// а теперь все надо перевести в юникод
		// 1. дефолтное расширение
		WCHAR wDefExt[32];
		MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			P.m_DefExt, -1, wDefExt, 32);

		// 2. фильтр
		COMDLG_FILTERSPEC *flt;
		int flt_num = 1;
		WCHAR wInfo[64];
		MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			P.m_FilterCaption ? P.m_FilterCaption : "", -1, wInfo, 64);

		if (P.m_DefExt)
		{
			flt = new COMDLG_FILTERSPEC[2];
			flt[0].pszName = P.m_FilterCaption ? wInfo : L"";
			flt[0].pszSpec = wDefExt;
			flt[1] = { L"All files", L"*.*" };
			flt_num = 2;
		}
		else
		{
			flt = new COMDLG_FILTERSPEC[1];
			flt[0] = { L"All files", L"*.*" };
		}

		// 3. начальная папка
		string_path id;
		GetCurrentDirectory(520, id);
		strcat(id, "\\");
		strcat(id, (offset&&offset[0]) ? offset : P.m_Path);

		WCHAR wInitial[520];
		MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			id/*(offset&&offset[0]) ? offset : P.m_Path*/, -1, wInitial, 520
		);

		IFileSaveDialog *pFileSave;

		// Create the FileOpenDialog object.
		HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
		bool bRes = false;
		if (SUCCEEDED(hr))
		{
			pFileSave->SetDefaultExtension(wDefExt);
			pFileSave->SetTitle(L"Save a File");
			pFileSave->SetOptions(FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST | FOS_NOREADONLYRETURN | FOS_NOCHANGEDIR | 0);

			IShellItem *inFolder;
			hr = SHCreateItemFromParsingName(wInitial, NULL, IID_PPV_ARGS(&inFolder));

			if (SUCCEEDED(hr))
				pFileSave->SetDefaultFolder(inFolder);

			pFileSave->SetFileTypes(flt_num, flt);

			// Show the Open dialog box.
			hr = pFileSave->Show(GetForegroundWindow());

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				bRes = true;

				IShellItem *pItem;
				hr = pFileSave->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					WideCharToMultiByte(
						CP_ACP,
						NULL,
						pszFilePath, -1, buffer, 520, NULL, NULL);

					pItem->Release();
				}
				strlwr(buffer);
			}
			else
			{
				switch (hr)
				{
				case ERROR_CANCELLED:	Log("The user closed the window by cancelling the operation."); break;
				}
			}
			pFileSave->Release();
		}

		delete[] flt;
		return bRes;
	}
}

