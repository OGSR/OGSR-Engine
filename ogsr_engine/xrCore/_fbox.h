#pragma once
#include "_vector3d.h"

template <class T>
class _box3
{
public:
    using TYPE = T;
    using Self = _box3<T>;
    using SelfRef = Self&;
    using SelfCRef = const Self&;
    using Tvector = _vector3<T>;
    using Tmatrix = _matrix<T>;

    union
    {
        struct
        {
            Tvector min;
            Tvector max;
        };
        struct
        {
            T x1, y1, z1;
            T x2, y2, z2;
        };
    };

    bool is_valid() const noexcept { return (x2 >= x1) && (y2 >= y1) && (z2 >= z1); }
    const T* data() const noexcept { return &min.x; }

    SelfRef set(const Tvector& _min, const Tvector& _max)
    {
        min.set(_min);
        max.set(_max);
        return *this;
    }

    SelfRef set(T x1, T y1, T z1, T x2, T y2, T z2)
    {
        min.set(x1, y1, z1);
        max.set(x2, y2, z2);
        return *this;
    }

    SelfRef set(SelfCRef b)
    {
        min.set(b.min);
        max.set(b.max);
        return *this;
    }

    SelfRef setb(const Tvector& center, const Tvector& dim)
    {
        min.sub(center, dim);
        max.add(center, dim);
        return *this;
    }

    SelfRef set_zero()
    {
        min.set(0, 0, 0);
        max.set(0, 0, 0);
        return *this;
    }

    SelfRef identity()
    {
        min.set(-0.5, -0.5, -0.5);
        max.set(0.5, 0.5, 0.5);
        return *this;
    }

    SelfRef invalidate()
    {
        min.set(type_max<T>, type_max<T>, type_max<T>);
        max.set(type_min<T>, type_min<T>, type_min<T>);
        return *this;
    }

    SelfRef shrink(T s)
    {
        min.add(s);
        max.sub(s);
        return *this;
    }

    SelfRef shrink(const Tvector& s)
    {
        min.add(s);
        max.sub(s);
        return *this;
    }

    SelfRef grow(T s)
    {
        min.sub(s);
        max.add(s);
        return *this;
    }

    SelfRef grow(const Tvector& s)
    {
        min.sub(s);
        max.add(s);
        return *this;
    }

    SelfRef add(const Tvector& p)
    {
        min.add(p);
        max.add(p);
        return *this;
    }

    SelfRef sub(const Tvector& p)
    {
        min.sub(p);
        max.sub(p);
        return *this;
    }

    SelfRef offset(const Tvector& p)
    {
        min.add(p);
        max.add(p);
        return *this;
    }

    SelfRef add(SelfCRef b, const Tvector& p)
    {
        min.add(b.min, p);
        max.add(b.max, p);
        return *this;
    }

    ICF bool contains(T x, T y, T z) const
    {
        return (x >= x1) && (x <= x2) && (y >= y1) && (y <= y2) && (z >= z1) && (z <= z2);
    }

    ICF bool contains(const Tvector& p) const { return contains(p.x, p.y, p.z); }
    ICF bool contains(SelfCRef b) const { return contains(b.min) && contains(b.max); }
    bool similar(SelfCRef b) const { return min.similar(b.min) && max.similar(b.max); }

    ICF SelfRef modify(const Tvector& p)
    {
        min.min(p);
        max.max(p);
        return *this;
    }

    ICF SelfRef modify(T x, T y, T z)
    {
        _vector3<T> tmp = {x, y, z};
        return modify(tmp);
    }

    SelfRef merge(SelfCRef b)
    {
        modify(b.min);
        modify(b.max);
        return *this;
    }

    SelfRef merge(SelfCRef b1, SelfCRef b2)
    {
        invalidate();
        merge(b1);
        merge(b2);
        return *this;
    }
    ICF SelfRef xform(SelfCRef B, const Tmatrix& m)
    {
        // The three edges transformed: you can efficiently transform an X-only vector3
        // by just getting the "X" column of the matrix
        Tvector vx, vy, vz;
        vx.mul(m.i, B.max.x - B.min.x);
        vy.mul(m.j, B.max.y - B.min.y);
        vz.mul(m.k, B.max.z - B.min.z);

        // Transform the min point
        m.transform_tiny(min, B.min);
        max.set(min);

        // Take the transformed min & axes and find _new_ extents
        // Using CPU code in the right place is faster...
        if (negative(vx.x))
            min.x += vx.x;
        else
            max.x += vx.x;
        if (negative(vx.y))
            min.y += vx.y;
        else
            max.y += vx.y;
        if (negative(vx.z))
            min.z += vx.z;
        else
            max.z += vx.z;
        if (negative(vy.x))
            min.x += vy.x;
        else
            max.x += vy.x;
        if (negative(vy.y))
            min.y += vy.y;
        else
            max.y += vy.y;
        if (negative(vy.z))
            min.z += vy.z;
        else
            max.z += vy.z;
        if (negative(vz.x))
            min.x += vz.x;
        else
            max.x += vz.x;
        if (negative(vz.y))
            min.y += vz.y;
        else
            max.y += vz.y;
        if (negative(vz.z))
            min.z += vz.z;
        else
            max.z += vz.z;
        return *this;
    }
    ICF SelfRef xform(const Tmatrix& m)
    {
        Self b;
        b.set(*this);
        return xform(b, m);
    }

    void getsize(Tvector& R) const { R.sub(max, min); }

    void getradius(Tvector& R) const
    {
        getsize(R);
        R.mul(0.5f);
    }

    T getradius() const
    {
        Tvector R;
        getradius(R);
        return R.magnitude();
    }

    T getvolume() const
    {
        Tvector sz;
        getsize(sz);
        return sz.x * sz.y * sz.z;
    }

    SelfCRef getcenter(Tvector& C) const
    {
        C.x = (min.x + max.x) * 0.5f;
        C.y = (min.y + max.y) * 0.5f;
        C.z = (min.z + max.z) * 0.5f;
        return *this;
    }

    SelfCRef get_CD(Tvector& bc, Tvector& bd) const // center + dimensions
    {
        bd.sub(max, min).mul(.5f);
        bc.add(min, bd);
        return *this;
    }

    SelfRef scale(float s) // 0.1 means make 110%, -0.1 means make 90%
    {
        Fvector bd;
        bd.sub(max, min).mul(s);
        grow(bd);
        return *this;
    }

    SelfCRef getsphere(Tvector& C, T& R) const
    {
        getcenter(C);
        R = C.distance_to(max);
        return *this;
    }

    // Detects if this box intersect other
    ICF bool intersect(SelfCRef box)
    {
        if (max.x < box.min.x)
            return false;
        if (max.y < box.min.y)
            return false;
        if (max.z < box.min.z)
            return false;
        if (min.x > box.max.x)
            return false;
        if (min.y > box.max.y)
            return false;
        if (min.z > box.max.z)
            return false;
        return true;
    }

    // Does the vector3 intersects box
    bool Pick(const Tvector& start, const Tvector& dir)
    {
        T alpha, xt, yt, zt;
        Tvector rvmin, rvmax;

        rvmin.sub(min, start);
        rvmax.sub(max, start);

        if (!fis_zero(dir.x))
        {
            alpha = rvmin.x / dir.x;
            yt = alpha * dir.y;
            if (yt >= rvmin.y && yt <= rvmax.y)
            {
                zt = alpha * dir.z;
                if (zt >= rvmin.z && zt <= rvmax.z)
                    return true;
            }
            alpha = rvmax.x / dir.x;
            yt = alpha * dir.y;
            if (yt >= rvmin.y && yt <= rvmax.y)
            {
                zt = alpha * dir.z;
                if (zt >= rvmin.z && zt <= rvmax.z)
                    return true;
            }
        }

        if (!fis_zero(dir.y))
        {
            alpha = rvmin.y / dir.y;
            xt = alpha * dir.x;
            if (xt >= rvmin.x && xt <= rvmax.x)
            {
                zt = alpha * dir.z;
                if (zt >= rvmin.z && zt <= rvmax.z)
                    return true;
            }
            alpha = rvmax.y / dir.y;
            xt = alpha * dir.x;
            if (xt >= rvmin.x && xt <= rvmax.x)
            {
                zt = alpha * dir.z;
                if (zt >= rvmin.z && zt <= rvmax.z)
                    return true;
            }
        }

        if (!fis_zero(dir.z))
        {
            alpha = rvmin.z / dir.z;
            xt = alpha * dir.x;
            if (xt >= rvmin.x && xt <= rvmax.x)
            {
                yt = alpha * dir.y;
                if (yt >= rvmin.y && yt <= rvmax.y)
                    return true;
            }
            alpha = rvmax.z / dir.z;
            xt = alpha * dir.x;
            if (xt >= rvmin.x && xt <= rvmax.x)
            {
                yt = alpha * dir.y;
                if (yt >= rvmin.y && yt <= rvmax.y)
                    return true;
            }
        }
        return false;
    };

    u32& IntRref(T& x) { return (u32&)x; }

    enum ERP_Result
    {
        rpNone = 0,
        rpOriginInside = 1,
        rpOriginOutside = 2,
        fcv_forcedword = u32(-1)
    };

    ERP_Result Pick2(const Tvector& origin, const Tvector& dir, Tvector& coord)
    {
        bool Inside = true;
        Tvector MaxT;
        MaxT.x = MaxT.y = MaxT.z = -1.0f;

        // Find candidate planes.
        {
            if (origin[0] < min[0])
            {
                coord[0] = min[0];
                Inside = false;
                if (IntRref(dir[0]))
                    MaxT[0] = (min[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
            }
            else if (origin[0] > max[0])
            {
                coord[0] = max[0];
                Inside = false;
                if (IntRref(dir[0]))
                    MaxT[0] = (max[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
            }
        }
        {
            if (origin[1] < min[1])
            {
                coord[1] = min[1];
                Inside = false;
                if (IntRref(dir[1]))
                    MaxT[1] = (min[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
            }
            else if (origin[1] > max[1])
            {
                coord[1] = max[1];
                Inside = false;
                if (IntRref(dir[1]))
                    MaxT[1] = (max[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
            }
        }
        {
            if (origin[2] < min[2])
            {
                coord[2] = min[2];
                Inside = false;
                if (IntRref(dir[2]))
                    MaxT[2] = (min[2] - origin[2]) / dir[2]; // Calculate T distances to candidate planes
            }
            else if (origin[2] > max[2])
            {
                coord[2] = max[2];
                Inside = false;
                if (IntRref(dir[2]))
                    MaxT[2] = (max[2] - origin[2]) / dir[2]; // Calculate T distances to candidate planes
            }
        }

        // Ray origin inside bounding box
        if (Inside)
        {
            coord = origin;
            return rpOriginInside;
        }

        // Get largest of the maxT's for final choice of intersection
        u32 WhichPlane = 0;
        if (MaxT[1] > MaxT[0])
            WhichPlane = 1;
        if (MaxT[2] > MaxT[WhichPlane])
            WhichPlane = 2;

        // Check final candidate actually inside box
        if (IntRref(MaxT[WhichPlane]) & 0x80000000) return rpNone;

        if (0 == WhichPlane)
        {
            // 1 & 2
            coord[1] = origin[1] + MaxT[0] * dir[1];

            if ((coord[1] < min[1]) || (coord[1] > max[1])) return rpNone;

            coord[2] = origin[2] + MaxT[0] * dir[2];

            if ((coord[2] < min[2]) || (coord[2] > max[2])) return rpNone;

            return rpOriginOutside;
        }
        if (1 == WhichPlane)
        {
            // 0 & 2
            coord[0] = origin[0] + MaxT[1] * dir[0];

            if ((coord[0] < min[0]) || (coord[0] > max[0])) return rpNone;

            coord[2] = origin[2] + MaxT[1] * dir[2];

            if ((coord[2] < min[2]) || (coord[2] > max[2])) return rpNone;

            return rpOriginOutside;
        }
        if (2 == WhichPlane)
        {
            // 0 & 1
            coord[0] = origin[0] + MaxT[2] * dir[0];

            if ((coord[0] < min[0]) || (coord[0] > max[0])) return rpNone;

            coord[1] = origin[1] + MaxT[2] * dir[1];

            if ((coord[1] < min[1]) || (coord[1] > max[1])) return rpNone;

            return rpOriginOutside;
        }
        return rpNone;
    }

    void getpoint(int index, Tvector& result) const
    {
        switch (index)
        {
        case 0: result.set(min.x, min.y, min.z); break;
        case 1: result.set(min.x, min.y, max.z); break;
        case 2: result.set(max.x, min.y, max.z); break;
        case 3: result.set(max.x, min.y, min.z); break;
        case 4: result.set(min.x, max.y, min.z); break;
        case 5: result.set(min.x, max.y, max.z); break;
        case 6: result.set(max.x, max.y, max.z); break;
        case 7: result.set(max.x, max.y, min.z); break;
        default: result.set(0, 0, 0); break;
        }
    }

    void getpoints(Tvector* result)
    {
        result[0].set(min.x, min.y, min.z);
        result[1].set(min.x, min.y, max.z);
        result[2].set(max.x, min.y, max.z);
        result[3].set(max.x, min.y, min.z);
        result[4].set(min.x, max.y, min.z);
        result[5].set(min.x, max.y, max.z);
        result[6].set(max.x, max.y, max.z);
        result[7].set(max.x, max.y, min.z);
    }

    SelfRef modify(SelfCRef src, const Tmatrix& M)
    {
        Tvector pt;
        for (int i = 0; i < 8; i++)
        {
            src.getpoint(i, pt);
            M.transform_tiny(pt);
            modify(pt);
        }
        return *this;
    }
};

using Fbox = _box3<float>;
using Fbox3 = _box3<float>;

using Dbox = _box3<double>;
using Dbox3 = _box3<double>;

template <class T>
bool _valid(const _box3<T>& c)
{
    return _valid(c.min) && _valid(c.max);
}
