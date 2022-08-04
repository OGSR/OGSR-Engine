#include "stdafx.h"
#include "PhraseDialog_script.h"
#include "PhraseDialog.h"

using namespace luabind;

void CPhraseScript::AddPrecondition(LPCSTR str) { m_Preconditions.push_back(str); }
void CPhraseScript::AddAction(LPCSTR str) { m_ScriptActions.push_back(str); }
void CPhraseScript::AddHasInfo(LPCSTR str) { m_HasInfo.push_back(str); }
void CPhraseScript::AddDontHasInfo(LPCSTR str) { m_DontHasInfo.push_back(str); }
void CPhraseScript::AddGiveInfo(LPCSTR str) { m_GiveInfo.push_back(str); }
void CPhraseScript::AddDisableInfo(LPCSTR str) { m_DisableInfo.push_back(str); }

#pragma optimize("s", on)
void CPhraseDialogExporter::script_register(lua_State* L)
{
    module(L)[class_<CPhrase>("CPhrase").def("GetPhraseScript", &CPhrase::GetPhraseScript).def("GetText", &CPhrase::GetText).def("SetText", &CPhrase::SetText),

              class_<CPhraseDialog>("CPhraseDialog")
                  .def("AddPhrase", &CPhraseDialog::AddPhrase_script)
                  .def("SetPriority", &CPhraseDialog::SetPriority)
                  .def("SetCaption", &CPhraseDialog::SetCaption)
                  .def("SetForceReload", &CPhraseDialog::SetForceReload)
                  .def("GetPhrase", &CPhraseDialog::GetPhrase)
                  .def("GetPhraseScript", &CPhraseDialog::GetPhraseScript),

              class_<CPhraseScript>("CPhraseScript")
                  .def("AddPrecondition", &CPhraseScript::AddPrecondition)
                  .def("AddAction", &CPhraseScript::AddAction)
                  .def("AddHasInfo", &CPhraseScript::AddHasInfo)
                  .def("AddDontHasInfo", &CPhraseScript::AddDontHasInfo)
                  .def("AddGiveInfo", &CPhraseScript::AddGiveInfo)
                  .def("AddDisableInfo", &CPhraseScript::AddDisableInfo)
                  .def("SetScriptText", &CPhraseScript::SetScriptText)];
}
