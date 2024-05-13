#include "stdafx.h"

#include "level_graph.h"
#include "ai_space.h"

using namespace luabind;

const CLevelGraph* get_level_graph() { return (&ai().level_graph()); }

CLevelGraph* get_level_graph_for_level(LPCSTR level)
{
    string_path fn;
    strconcat(sizeof(fn), fn, level, "\\", LEVEL_GRAPH_NAME);

    string_path fName;
    FS.update_path(fName, "$game_levels$", fn);
    return xr_new<CLevelGraph>(fName);
}

u32 vertex_count(const CLevelGraph* self) { return self->header().vertex_count(); }

u32 vertex_id_script1(const CLevelGraph* self, const Fvector& vec) { return self->vertex_id(vec); }
u32 vertex_id_script2(const CLevelGraph* self, u32 node, const Fvector& vec) { return self->vertex_id(node, vec); }

Fvector vertex_position_script(const CLevelGraph* self, u32 level_vertex_id) { return (self->vertex_position(level_vertex_id)); }

void CLevelGraph::script_register(lua_State* L)
{
    module(L)[def("level_graph", &get_level_graph), def("level_graph", &get_level_graph_for_level, adopt<result>()),

              class_<CLevelGraph>("CLevelGraph")
                  .def("vertex_position", &vertex_position_script)
                  .def("vertex_count", &vertex_count)
                  .def("vertex_id", &vertex_id_script1)
                  .def("vertex_id", &vertex_id_script2)
                  .def("nearest_vertex_id", &CLevelGraph::nearest_vertex_id)
                  .def("valid_vertex_id", &CLevelGraph::valid_vertex_id)
                  .def("is_accessible_vertex_id", &CLevelGraph::is_accessible)];
}