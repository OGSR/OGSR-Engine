#ifndef _F_SPHERE_H_
#define _F_SPHERE_H_

template <class T>
struct _sphere
{
    _vector3<T> P;
    T R;

public:
    IC void set(const _vector3<T>& _P, T _R)
    {
        P.set(_P);
        R = _R;
    }
    IC void set(const _sphere<T>& S)
    {
        P.set(S.P);
        R = S.R;
    }
    IC void identity()
    {
        P.set(0, 0, 0);
        R = 1;
    }

    enum ERP_Result
    {
        rpNone = 0,
        rpOriginInside = 1,
        rpOriginOutside = 2,
        fcv_forcedword = u32(-1)
    };
    // Ray-sphere intersection
    ICF ERP_Result intersect(const _vector3<T>& start, const _vector3<T>& dir, T& dist) const
    {
        T t;
        ERP_Result result = intersect_ray(start, dir, t);
        if (result == rpOriginInside || (result == rpOriginOutside && t <= dist))
        {
            if (t < dist)
                dist = t;
            return result;
        }
        return rpNone;
    }

    ICF BOOL intersect(const _vector3<T>& S, const _vector3<T>& D) const
    {
        T t;
        return intersect_ray(S, D, t) != rpNone;
    }
    ICF BOOL intersect(const _sphere<T>& S) const
    {
        T SumR = R + S.R;
        return P.distance_to_sqr(S.P) < SumR * SumR;
    }
    IC BOOL contains(const _vector3<T>& PT) const { return P.distance_to_sqr(PT) <= (R * R + EPS_S); }

    // returns true if this wholly contains the argument sphere
    IC BOOL contains(const _sphere<T>& S) const
    {
        // can't contain a sphere that's bigger than me !
        const T RDiff = R - S.R;
        if (RDiff < 0)
            return false;

        return (P.distance_to_sqr(S.P) <= RDiff * RDiff);
    }

    // return's volume of sphere
    IC T volume() const { return T(PI_MUL_4 / 3) * (R * R * R); }

      // https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code
    ICF ERP_Result intersect_ray(const _vector3<T>& start, const _vector3<T>& dir, T& t) const
    {
        _vector3<T> m;
        m.sub(start, P);
        T b = m.dotproduct(dir);
        T c = m.dotproduct(m) - R * R;

        // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0).
        if (c > 0.0f && b > 0.0f)
            return rpNone;
        T discr = b * b - c;

        // A negative discriminant corresponds to ray missing sphere.
        if (discr < 0.0f)
            return rpNone;

        // Ray now found to intersect sphere, compute smallest t value of
        // intersection.
        t = -b - _sqrt(discr);

        // If t is negative, ray started inside sphere so clamp t to zero.
        if (t < 0.0f)
        {
            t = -b + _sqrt(discr);
            if (t < 0.0f)
                t = 0.0f;
            return rpOriginInside;
        }

        return rpOriginOutside;
    }
};

typedef _sphere<float> Fsphere;
typedef _sphere<double> Dsphere;

template <class T>
BOOL _valid(const _sphere<T>& s)
{
    return _valid(s.P) && _valid(s.R);
}

void XRCORE_API Fsphere_compute(Fsphere& dest, const Fvector* verts, int count);

#endif