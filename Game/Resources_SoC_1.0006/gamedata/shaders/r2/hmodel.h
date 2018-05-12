#ifndef HMODEL_H
#define HMODEL_H

#include "common.h"
#include "ogse_config.h"
uniform samplerCUBE         env_s0                ;
uniform samplerCUBE         env_s1                ;
uniform samplerCUBE         sky_s0                ;
uniform samplerCUBE         sky_s1                ;
uniform float3x4                        m_v2w                ;

void        hmodel                 (out float3 hdiffuse, out float3 hspecular, float m, float h, float s, float3 _point, float3 normal)
{
        // hscale - something like diffuse reflection
        float3         nw                         = mul                 (m_v2w,normal);
        float         hscale                 = h;                                //. *        (.5h + .5h*nw.y);
#ifdef         USE_GAMMA_22
                        hscale                = (hscale*hscale);        // make it more linear
#endif

        // reflection vector
        float3        v2pointL        		= normalize        (_point);
        float3         v2point               = mul                (m_v2w,v2pointL);
        float3        vreflect        		= reflect         (v2point,nw);
        float         hspec                 	= .5h+.5h*dot        (vreflect,v2point);

        // material
          float4         light                = tex3D                (s_material, float3(hscale, hspec, m) );                // sample material

        // diffuse color
        float3         e0d               = texCUBE         (env_s0,nw).xyz;
        float3         e1d               = texCUBE         (env_s1,nw).xyz;
        float3         env_d             = env_color.xyz*lerp(e0d,e1d,env_color.w)        ;
					env_d*=env_d;		// contrast
        hdiffuse                        = env_d * light.xyz + L_ambient.rgb;

        // specular color
                        vreflect.y      = vreflect.y*2-1;                                                        // fake remapping
        float3         e0s               = texCUBE         (env_s0,vreflect).xyz;
        float3         e1s               = texCUBE         (env_s1,vreflect).xyz;
        float3         env_s             = env_color.xyz*lerp(e0s,e1s,env_color.w)        ;
					env_s*=env_s;		// contrast
        hspecular                       = env_s*light.w*s;                //*h*m*s        ;        //env_s        *light.w         * s;
}

void         hmodel_table        (out float3 hdiffuse, out float3 hspecular, float m, float h, float s, float3 _point, float3 normal)
{
        // hscale - something like diffuse reflection
        float         hscale         = h;

        // reflection vector
        float3         v2point        = normalize        (_point);
        float3        vreflect= reflect         (v2point,normal);
        float         hspec         = .5h+.5h*dot        (vreflect,v2point);

        // material
          float4         light        = tex3D                (s_material, float3(hscale, hspec, m) );                // sample material

        // diffuse color
        float3         env_d         = texCUBE         (env_s0,normal).xyz;

        // specular color
        float3         env_s          = texCUBE         (env_s0,vreflect).xyz;

        //
        hdiffuse        = env_d        *light.xyz         + L_ambient.rgb        ;
        hspecular        = env_s        *light.w         * s                ;
}

#endif
