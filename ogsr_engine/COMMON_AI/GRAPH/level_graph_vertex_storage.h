#pragma once

#include "stdafx.h"
#include "level_graph_space.h"
#include "../xr_3da/xrLevel.h"

namespace LevelGraph
{
class CVertexStorage
{
    CVertex* m_vertex_nodes{}; // vertex array
    size_t m_vertex_count;
    bool m_compatibility_mode{};

private:
    template <typename OldNodes>
    CVertex* convert_nodes(IReader* stream, size_t vertex_count, xrAI_Versions version)
    {
        m_compatibility_mode = true;

        auto* nodes = xr_alloc<NodeCompressed>(vertex_count + 1); // additional one, so we don't trigger access violation
        const auto* oldNodes = static_cast<OldNodes*>(stream->pointer());

        for (size_t i = 0; i < vertex_count; ++i)
        {
            nodes[i] = oldNodes[i];
        }

        // Mark end node
        // so we can spot that in debugger, if we need
        NodeCompressed& endNode = nodes[vertex_count];
        endNode.data[0] = 'A';
        endNode.data[1] = 'I';
        endNode.data[2] = version;
        endNode.data[3] = ' ';
        endNode.data[4] = 'N';
        endNode.data[5] = 'O';
        endNode.data[6] = 'D';
        endNode.data[7] = 'E';
        endNode.data[8] = ' ';
        endNode.data[9] = 'E';
        endNode.data[10] = 'N';
        endNode.data[11] = 'D';
        static_assert(sizeof(endNode.data) >= 12, "If you have changed the NodeCompressed structure, please update the code above.");

        static_assert(sizeof(CVertex) == sizeof(NodeCompressed), "If you have changed the CVertex class, please update the nodes allocation code above.");
        return static_cast<CVertex*>(nodes);
    }

public:
    CVertexStorage(IReader* stream, size_t vertex_count, xrAI_Versions version) : m_vertex_count(vertex_count)
    {
        switch (version)
        {
        case XRAI_CURRENT_VERSION:
            static_assert(XRAI_CURRENT_VERSION == XRAI_VERSION_SKYLOADER, "If you have changed the xrAI version, don't forget to add backward compatibility with older versions.");
            m_vertex_nodes = static_cast<CVertex*>(stream->pointer());
            break;

        case XRAI_VERSION_BORSHT_BIG: m_vertex_nodes = convert_nodes<NodeCompressedBORSHT_BIG>(stream, vertex_count, version); break;

        case XRAI_VERSION_BORSHT: m_vertex_nodes = convert_nodes<NodeCompressedBORSHT>(stream, vertex_count, version); break;

        case XRAI_VERSION_CS_COP: m_vertex_nodes = convert_nodes<NodeCompressedCS_COP>(stream, vertex_count, version); break;

        case XRAI_VERSION_PRIQUEL:
        case XRAI_VERSION_SOC: m_vertex_nodes = convert_nodes<NodeCompressedSOC>(stream, vertex_count, version); break;

        default: FATAL("Unsupported level graph version."); NODEFAULT;
        } // switch (version)
    }

    ~CVertexStorage()
    {
        if (m_compatibility_mode)
        {
            xr_free(m_vertex_nodes);
        }
    }

    [[nodiscard]] CVertex* begin() { return m_vertex_nodes; }
    [[nodiscard]] CVertex* end() { return m_vertex_nodes + m_vertex_count; }
};
} // namespace LevelGraph