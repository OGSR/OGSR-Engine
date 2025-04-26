#pragma once

class ECORE_API R_hemi
{
public:
    R_constant* c_pos_faces;
    R_constant* c_neg_faces;
    R_constant* c_material;
    R_constant* c_hotness;
    R_constant* c_scale;

public:
    explicit R_hemi(CBackend& cmd_list_in);

    void unmap();
    void set_c_pos_faces(R_constant* C) { c_pos_faces = C; }
    void set_c_neg_faces(R_constant* C) { c_neg_faces = C; }
    void set_c_material(R_constant* C) { c_material = C; }
    void set_c_hotness(R_constant* C) { c_hotness = C; }
    void set_c_scale(R_constant* C) { c_scale = C; }

    void set_pos_faces(float posx, float posy, float posz) const;
    void set_neg_faces(float negx, float negy, float negz);
    void set_material(float x, float y, float z, float w);
    void set_hotness(float x, float y, float z, float w);
    void set_scale(float x, float y, float z, float w);

    CBackend& cmd_list;
};
