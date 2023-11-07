// ImGui Win32 + DirectX11 binding

// In this binding, ImTextureID is used to store a 'ID3D11ShaderResourceView*' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include "stdafx.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"

// #include <addons/ImGuizmo/ImGuizmo.h> // TODO ImGui

#define DIRECTINPUT_VERSION 0x0800

// DirectX
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dinput.h>

// Data
// static INT64                    g_Time = 0;
// static INT64                    g_TicksPerSecond = 0;

static HWND g_hWnd = 0;
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static ID3D11Buffer* g_pVB = NULL;
static ID3D11Buffer* g_pIB = NULL;
static ID3D10Blob* g_pVertexShaderBlob = NULL;
static ID3D11VertexShader* g_pVertexShader = NULL;
static ID3D11InputLayout* g_pInputLayout = NULL;
static ID3D11Buffer* g_pVertexConstantBuffer = NULL;
static ID3D10Blob* g_pPixelShaderBlob = NULL;
static ID3D11PixelShader* g_pPixelShader = NULL;
static ID3D11SamplerState* g_pFontSampler = NULL;
static ID3D11ShaderResourceView* g_pFontTextureView = NULL;
static ID3D11RasterizerState* g_pRasterizerState = NULL;
static ID3D11BlendState* g_pBlendState = NULL;
static ID3D11DepthStencilState* g_pDepthStencilState = NULL;

static int g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

// need for imgui
static INT64 g_Time = 0;
static INT64 g_TicksPerSecond = 0;

struct VERTEX_CONSTANT_BUFFER
{
    float mvp[4][4];
};

// File: 'C:\Windows\Fonts\Ariblk.ttf' (167592 bytes)
// Exported using binary_to_compressed_c.cpp
constexpr char Ariblk_compressed_data_base85[] =
    "7])#######]ra1Y'/###I),##g?hhLjKI##e%1S:r.'6*Z1?5/F+8>,S[;99>-?\?$u@pl8WKdp/(RdL<.BC;X<euH2/>t(3SGxF>Rg`&$gf08IAgHB6^JJf==l0KL4sbf(2H:;$TZn42"
    "?svgU9.>>#`IqA6aNV=BLHHt$ZXt7I^Yqr$a0XGH`911Er>'DNVci21xKk7D*wQaCd:m1Kf'-p/Ya(*HV`R%#w,OJ-i$Y)5+-0%Jtgre/1;r:Q#a'8Im>K0FoKg._;w4<1E3WlAq.d<B"
    "pXW_/__NnL5s6A4AJr9.')8<d$4acW%db#5<_[FHGxl5Ax7YY#&`+/()%2eGc)-xFV$eS%o1NP&k3JuBcGE^-FGB`NMnQS%#I[^IVcNT%<l6mL^u5;-w)A>BrdK5=6pVuL/^w.:qmnUC"
    "/LFM.-;kHrjgV'O@C=GHL26PJK'=2LbIt?-1)Q-GOOdbRo<$1,,t42qgISq)s.tCl_EOK8odPirFOm##pw@)XXuoEM8s]-'U%h:H;L90Likaw'W`):28=5F%%^[SubD/2']*CfuXt%nN"
    "`KViLQ*As$?@Ke$3t6;65#YV$5e+/(ZO]o@wa&&=&.%v#?qh#$pxIGM@<EW%P5mD3%koiL?EED#CIOe#Dl1Z#>l(Z#*GlY#un_Y#6$<SII,oJMOqPl;rrFS7A+Hu#0j'D<tYR5fwHrgu"
    "M4GCiM=Q5/4VGb.i@ZY#5rJ,Mu'^fLML/s6N5NH$Xr`[$Wfx]$c]=Y%8cgB%1&vj#`?%)$3`ErLIAjC$v=@#M:B4h#V'l-$Q]?F%*.@>#Il5&>q*]`XT,YD4g$C2:HkhP0bLfS8uMk]>"
    "$&H;@0q(m9?.`-6uxRGEGcUPK?QrdETfBJ:fp%]t#heERl%2PS.i@5Bj4U2U0lUoRDX?pS2x&5AbMooSCU&&YMOT;.;@,F%sde>PBMH_&cso-$N'Xw0c8:F%Hd)m9YgE/2miSSS3XHZ$"
    "+)P:v%lB#$vU'^#T_9v-CDrGNYRu[tQNE/2r=d5Krp$cr/=6kbg#Z-?Ftn-$-fNkb1J(F.6sC_&Y.ZSJxGmDOmU%JUCwL_&.Gq##0pP_&.5Pk4v`=>PJYs^fUc-F%kW6R*HTG_&pv-kX"
    ")KC_&1dC_&oZ+vGt<Ak=G,E>5MV1F%]KCwK%2,Rs$$,F%rLW9V44dwgePSSS^vNwK7%MRW&@v9VslRfLN(h(M%b-]OTZRA-o99`O/P$/$>mdw'64LAG*_#t71*El])T.DNLSg;IudPY5"
    ")@v;%e@<jL$]MXMBI_*#8?>uuxvre.K785#K)MP-H6T;-.QP8.==M(vfAbVIs)^C>^sfi0B0bf11t=D3NCqXu%=QV-=DVH*:i#;Qq=#29s[0N:TfBJ:DNxoJ.$'QK5Ijx=Sn5s.8IFmB"
    "'nCA+0c`SAB5#sIr6EpJ&()##e_`s-XCCpLvs&nL4nTA$E)V*Mwv>U$3fY1M/GE>5E(v-$7EBmKJX*,2*2'Z$WQ)B#orjrL#4DhLHRrhL:pIiL>dn*NVoNuLRg$9MFo6mL0^4C.0lUoR"
    "jXm;-k/D9.AkTxX<Sw],V>:&5YY6#6u%2s@xPbA>Y)>F%2&6PS4r&.$p4Dp.`:OYYHdK_&:AJ&#>R12_5:=2C]^*j1fYooS#kR>m)-S:v&]F&#q=+m9OccG3HF^v-kxko.K=MJVJ&,^>"
    "N[7F%;e$Z$vrWVRLn@5KS21T.,bH(#V6T;-45T;-GtfN-oglS.q=85#Bl*J-O'l?-f+NM-1%6pLofipL(^uKN14`T.p2>uuZF)e.#1U'#cx+G-x4;T.1iL7#gZ>T%9^x.:,%Jm/W>*)#"
    "-GD,#=dEB-+.P[MOIH##4I$?$vK)uL4dgvLc8NmN*kU:#[fG<-Dll-MFYW0$^f#tL;ihAOF:LkLknHlL5.X[$3cGlL7VTvLrmdPN8u.;%)%Rm82q[H%1g]cF+q+d2u)EJhwo62K>Mnvc"
    "khq##;Oqo[`2,F%xD9w^s4IG%rUx9)pJbJhTJh`aainoeatb-6^#Ofh-6x-$.Q28]=E.&G7n48f0.ip7:P1KaKKUX%9@Mp7[?iG%Lvw2233DhLj/,x#o>?_#IC%+MqGgnL?4DhL_0nlL"
    "xqJfL83+,814T;.,r12UHAl;--sH6%xBQk+`GW&H*Wx0#4?:/#'U:a%0rgo7hZo>I4@X%$'[Cu7;pNx'7u:<I+)P:vf=$v#u:v8.h`u7IH,Uk+Ytr##t&1R3u15kOmdQcMK9T9VGppw0"
    "^N]._JWxr$.Uwr$^#,F%5?k-$5Aq-$ttrr$s'>cMfdF?-C?EY.N<E%v]`BK-_6T;-Ahj3M5UMpLe&&YM47+)#jZWR%_4$sIY&p;-jgG<-la/,MQ7LkLB1J8%T%AnLX6S8%4V4oL)MwA-"
    "XOxF/8uUC$kRp99B.GDOE5K;I8nnq.rxu7IZlqEI;pXf:Aq_#$bG'x7EVO&>Sqk+MiWUsLO:qL9N&3.->d.#Yxqs9)UdZ>5G[g%ug(Q2(Z:Z<I8Uv.:f/LM'KUw7I>_Y2:Z#2F@%'GWf"
    "ChPhL?^Jd%:ccLMBo&-#3)m<-n'N'%g3u9)x>JsZUTj-$`jiPfrbX/iuk4mAt?p&$7`j>5,vw22Ws56#0$2=/+FUr$cvclL,`p;M#:#gLBB'^FgX)<-3P-U&cZB5B()G/2g,^p03IE>l"
    "641aF5.BgLxtT^+sB3<$]2I#Y`/'g2+Yi8AZ)]m8b.*Z6r;Lkb5s`lo^?T/(S9jG;r_s9)U:Qw9o3<&FqYMAX?:t-$?;7PJSI^]++ebw'(pp-$:g=J:e:C;IFY:5Ki6$j:qqIK)fS^V$"
    "QR/R3U5QP&#-BVHAE&%H8gBVHR`CVH]BSS%O#a%O5C#.6YJ/pe'cX]u8O^-6VsaGDsm;>PoAfw9eIAD3ZZ)AO$x:uul@h2U^]W]+f90R3C'%Z$fFVwBnq?<I/qn4So8e%O7ec1%8CZv$"
    "7&x+Mx[Z##jg)X&>)&crniIKD4%RlSOhfr?\?'o;-rM#<-(6T;-vps$5`;^*#vmNp#jc3T#-Oo1va]SX#Ha4m#ohJWQ7-1loN-ZF%8m,xMT%T,MT#//Mdl-xPc&<e2[F,sZ<1,vYU/+,i"
    "J$hoeb$M9i:F^Po/5l-$GGZF%6QquZEPuoJcPb;/BP:PSIw2M^D:+,):kfu>DMk(EH44>5Vhku5f%q-$TG+87l+x%+]0FG27shf('O28.omrc<;J[A,Kq]PB7&0hL^(@f$D*iC$*L(_$"
    "Z#j[$Qr4^$7N#<-wej/1J'Fa$*(G^$gwh_$-8fT%Euix=%P6D<j-9A4.V:G;B23;6p,eo7#l.>>uliu5(uix=&-.79g%M]=1oao@diPlSkuPlSb#@^P_tCM9M6Z?%n5gNMua3,#Xv:$#"
    "Ser,#VLd;#kIwA-A6T;-x5T;-]mk0MpOOK2LPUV$?$Dg$]p3a$3q(K$W7T;-&*m<-EN.U.A`Gj#NN.U.FDBY$Pt:T.q-`,%L>DiLNY$mS?q$Z$;F5F%PxOk+4g/58L+'&+2GBYP9ba(6"
    "^sW:ve9[Y#dtU=ujdrr-p&5`a3_br?S-RS.5lhG3]4RA-J#)t-eh`:M`lT;-2jk0MdjP8.D-<uuo01F%lQ4F%41UDN?^^vLp&0,MgsjfLsd)qL%edEM%B1sL(JwUM==,G-Na./M]lk0M"
    "8fL:#f_CH->bCH-@5T;-g8'G.(m2H)q*mgLGtlSN)=mtMp4Eg$dSCx$;,o`F>@9.$C^J=#noP8.L5t=$+r#Z$rR1x'+u#7v'ST;-tJmANnslrd%)C.$)I2w$o+*l$sN51M,=#gLik:5T"
    "lb0W.Nt-ktV#Y-Z7qgr?OM4Z$/Q[r7E2$?$EcFoL^M`PMELYGM@]i5M6)Wu0K$4A=e9[Y#FD3GM/tcP92ILk=Lj$:;8sdr?Oo0pSP9^w'5sL_&B*Iv$-S'DMMQ%(#><S48d7hR'ErAA+"
    "r]bY#gW<SRuG%&+W-<kFZI35&U-bE[gkimLD^YO-1jUT0M7%H%c#.<$ZHRv7Qv7R37VhERDOI<-=F_1MGq+G-6SgoL0I5O-B?FV./pMD%)?9C-0n6a.kNp:%gA;=-SYlS.Nw-@%xS3N0"
    "AH:;$[gc/$vi)k:[eC_&nu'q'9[`=-wfG<-;5T;-+N=KM=@c87x.r1$:ww+;RJXI%+jPlS/VT;-56E0U>x)/Ub&u0$,($;#V]Qp/Uhd,$IuK+$;O#<-6I5s-wkYxOO2AY/DaOlSr]x.:"
    "=(jq0%Vaj$;5F^#J'L-MjX#Z,kUx>eJ&3mN-pF1#i^J=#+n[j%<&),2[1?8JAA-S'P/:5&%Bu(389Y`33%R`<-r?s$rx8;-:%,)NR'N?#1okA#O<%'OkeKgLFVD@-HW4PS&M2%P_II`-"
    "?n9E#[jL.VFj)4')6?^?3_YgL3E,*8/$..6CS>$C#oM/)MJfk=aiG>#Xq82'3q-.M[6,1U*F8>-RQ@'OXx]@#^692'[w2[pe<dgOAQwq2`kn@#nnH>#eqOJ(MF7@#[hR3kwX@X1:bXP&"
    "v<nx,=*0/1;gNd**+w>#%><j1JsoP'kAx21bSxu,4,9;-EGNfL0t//11Y/GMM44V.&A#v,[9fu-r[hKM)O#@.B5V11a0:[0OO9v-I0*j1dcBW/:UeQ/VgZ)*l9;%,TwUC#^@F31WG$C#"
    "jhH>#x7])*Q=pgLHH'#,/?ku-hKfQaaZd`*ZN:h.V-=A#wAmD-R_4,/EpZi1k>3hYR1<5&%3K@P?nN'OGS(]MMjx>-J29;-2(^,MVPBcN],G?-==cG-UUit-:lSfLCKH>#ire+MmGT%#"
    ":nAQ&B`:;$8=n8%BF%w#<b_$',%-aNLn`;$/R)W%/1ei0Dkjp%>X39%GhNT%-Vj-$,6g*%RfU;$.BSb.'wJe$V1[8%<lVq2Z4@W$B_<9%bFe--Wl`q2)l<1>*P4gLqGqhM?@%q.BeWp%"
    "N?;b7`[ns$;nZv$j[=gL?xKQ&PZ[;%<0&T&vMw61Jw/Q&5c:v#?XWp%tQ<nL$lDv#K:)bR-cX,MMqI/M9Rl)NW9&<$@Ies$*pia-8>?X1]UsP&Kwjp%S)':)oNd>$l[K-mat8Q&DG@2."
    "ge'dMjaXp%dIxfLliJGM=L4T%,W)a-*?xB/ix9hPJ>g05Ybc>$r`];%`Bl-$jJ];%jJXO4Abd--'t8I$H+`n*^oRC/1oOgL@S49%v>f<%8Fns$Gnj5&[O#qrabNT%kbFgLAbhp7$I2W%"
    "eZ3L#kMAv$S]6wgW47W$_CRvIXK+9%i^7<%H$96&1@Vt-nkXgL6x`;$r0lgLJ)LS-2xc<-vT#%.lhOgLedbp%h85F%oNpQ&N?,3'VSh[-Dh#I67XJ6aRdR['.]4gL)tbT%Fq8Q&/@$Z$"
    "4Pt>$k`1C&G[)T&-c:5/>.iv#m*(hLO/vhL3kDvYdOh1MX9$n&<dUh.BtAQ&,DNfLv=JW$g73K8x;vV%-IvV%E>3t_eXRW$@7%<$'4m;%?\?tglLkZ<%=:.<$@[Np%=:ggLbWXp%U&Os%"
    "oo?['Otn*7)45b@spuqVMK[w'-FvV%v34I-U(+O4wEn0#sb%1,1:H&O(t2C8)M;s%Ta@v$qUc?KabWp%gR=gL8A,gLKYPgLaK/<$>7.<$@Ons$FXU).mtXgLl-4r.Bks5&$@Lk+JL]'J"
    "O;Qt1h$BQ&Gb<9%(lg58TKZ5hQJ=4%8%$Z$,C2<%6uh;$4.GDNNoh&&)YkGMen`;$@:Is-gO4gL.f=T%J3gm&o+JfLl[4T%EO.w#`r]E[PcCv#Y:MW-dMC_&G:/FIk'96&BkWT%K$tp%"
    "8F`t-ptbgLTtMv#eUF,M+nFT%<:.<$?k8m&Z;XW%<I*T%uMa9Do?#C/d*96&9ZToL*&-*O/:#gLcgk5&J6TQ&?KJp&pr?['_Les$l_=gLE=qm&>CwS%<(Oh#n[<p%NK,n&ALRs$7=es$"
    "c7]fLmL>gLo%8s$]6Is%uYjU-KiFP8X3%O=_O39%?F3p%6i/gLs2$9/Q?K6&iO=,MigOT%:%%W$>*wV%RoBW%owkgLNIfs$CSK-QgU:W%`K3L#%ej-$48*=(ed+t-_:ofL7v)o&`>tf-"
    ":3?tL@hL?%gnWB-9l/,MVGpV-^ZNL#]XW5&@YrmM:bl,M?N%NNnAhQ&AREp%x?X$1:=<p%De*t$IXI<$4?uM0Fea5&J'tp%neFgL&_YgLP,:w-d@ofLMU=T%[4@k==#c;-9T(1.lqbgL"
    "d4pfL)B1q%Jtsp%D?Yj'ukFgLtaP[.K*tT%n-0_-]P^t:m$'q%RdlN'O006&rntGMp_YgL:ebp%k5p*%_wSm&.k@p&&F*nLU[FT%Cdpdm,fbgL6F5gLRecgLwFCH/=C3T%tnbgL1UiX-"
    "R;x/30q2Z)1#u05e[w8%NNpQ&;phh.BUR<$7-a#M@ui;$eI$XL&+Z;%3i/I$5ACQ'Ft86&uh,Z$(C2W%/N=_/9kZ;%LLCgL/L>gLdg'6&Nlg8&te-MMFOP,M9HPGM1+lo$9t)W%?I,hL"
    "OG5gLqFD=-v%9;-RVu%MD0&ZM/GB[%A8^8&?Z4R*FH3W%t;EtCIg&t%AXNT%mhkGM'MM=->nR:.@RET%t#&<%jU=gL)Y?T-=Y]F-7p>pLa[OT%5,+u1k0^m&KtaT%DUes$u)a,MTvZ:&"
    "0VmVQ*2Vm&U1Is$TldvIY<xS%:Xh>$#Vs/%a&mancgWT&XVE**LcaWq*w<6'=%.W$JC-9KYn`;$O_$W%S?jt%G_eW$L<gm&M3KQ&gjj-$m696&:x$W$0?Jb._LRW$;DQt1-lkgL1Oos$"
    "1]kZg_X*9%LTiiL?2RA-h3^SPk`8;/Gw&q%qwbgLH<qQ&Q;U<.G''q%hsNT&E=.<$dg0h$ihW5&R^P3'E_w8%MWev$@I*p%+L;s%*I)<%M'06&Sk_Z$C-^Q&fTn0#F]69.AXNT%COJe-"
    "Q))F.odP3'2+N*IZ7x34U(%W$0hr8&oeFgLZ#(U%Q^[;%P9w^oa[ws$IwY*7,e7T&/7Zm8erD<%<Ies$'rX,MF3D&0IbET%::IW$''9OM=TGgL]-r)Nu;0W$ZF<p%ihFgLNJieM8GV9."
    "NwWT%Sr=tU^oL;$K<#n&OW5n&@PPd$d_eW$rwbgL0'(q%K<pm&[qt?0C+Zm8v8d;%,9BoLvM>gLdj49%I0gm&H_R<$h[ir&L006&?LEp%YH9q%K096&J''q%P9^Q&iftS&;W+u%M<TQ&"
    "Wsc3'TZlj'GhNT%@U>hL9bbp%C)`1.mwkgLuuH3'Jtap%Jnjp%>=.<$EHP3'kk_s%kqkgLwFCH/AUWp%wwkgL$UY,M(KTT8$?vv(l:Q6'H6#3'MnWp%ST3F.C:%W$2pfs%>1r;$JMGh5"
    "HkTWA0CIQ/Sm:0(J0^Q&wt>Z$&I-L,Txh;$tV8_J+MxfLb#C6&Csg3Oewjp%uEN9`WvwV%6v4b@2Fd&O'2)iL6lYs-ntk,MAC>,MeCMX->Ah[0%>#HMa_YgLSih@-?fxF-N&c?-AXQx/"
    "Hw/6&Dhap%*D>8.BO[W$do#7*A$.Z$g/s2/It&6&lU9tS3g,pLXcXT%2<4U&LB,3'N'kT%Gh39%RobgL[aafLMgK;-&29;-.o(9.97Rs$?Est(`GPW-x3cn*W:e8%8ZcO-ag_tLg/1q%"
    "bP;S&>bQ]/NZlj'@4Rs$+<<Z$+7seH&>xfLe#LQ&5Laf1RZP3'J?T6&VQpQ&uwXgLjcHn&D7r;$jhX,M*':6&kkOgL1-?j'``T9&Dns5&M0tp%Qa./MFNQ3'<ZToLY5MMNl[8;/L9T6&"
    "w3ugLxYd3'@$]:.L<K6&nGT6'JORW$I'96&'X'%1MZG3'X^Yj';x_;$6.Dp&tGn61FU[W$JWGn&PB^Q&#5pr-w*cgLSu-q7SJf9&G3KQ&E'K6&qnXgLn^Xv-(C(hL;i0Q&7@;=-:NYO-"
    "0:7Q/SKgQ&Q^lj'<f<kFdga5'Lm'<.IkN9%jal-$7-&9&IqaT%4fcW%@[39%.(c,MH@1U%N9'q%d=xfL#kWnLp_YgLdF$iMR-8W$+Oi8&<ZvTM,e@p&NH%fMM'Ls.Wp(O'GAO0P%gt9&"
    "?njT%SH#3'P*Bm&*ohT&N$=t$WgY3'1w@T&SacN'[qt?0]O[W$v7cs%Fks5&0eVs%kU4gLA#$xLgTZ3'JP.J2TNg6&FIRs$aM`k'QQ5n&II1d-Ee)qin4q5'v$YgLl@@n&_G7L(Z^,n&"
    "T396&H'96&Q<^Q&PKpQ&'5pr-q0(hLr1Ek'P306&O*96&DNtgl0%(hL8RGgL,b)O'10+q&COwS%Yscj')Uis%J-KQ&-LQY)7#u05n-kp%Y/2O'k_=gL:_)k'fL:$^lYf;%=O5hLr8UM9"
    "<[i8&mFYB]m6O9%_O^Tr$bj-$d<Mv$D'2.M%MfW$S6a5'`)V]$q-%9&q$lgL&q^`$JK<s%9^*Z$Hf9'osGOb@eO?tL.cFgLk/_Q&w%9;-0dgW$&Un0#F&IE5E>`P-Ms@n$Z8b8&VgIJM"
    "NNfW$EX2T&fdNh#e<pm&>3Ns%'@h0,dksp%Kwsp%F;i^oJI)39Atr8&u6:HM>Nhm&Q^lj'VK^6&M'XT%u)a,M6Nw&OK'CPNE'ZhL0thDN$RGgLY9Ss$x$h[%E4ZQ8v0-Z$@n.Y-#gq;]"
    "*RGgL_8:6&QHpm&[k;$#r0iW%W#2O'aF+gLX,/W$wXS,&&>xfL3tLdM+_?3'NB>3'WjP3'%J<X(o#D0(*Cdv$@kaT%O#BT&J0^Q&ON.Z$`fH=.F_<9%P%6T&.nm--P,Tp&)q)'OQB=KM"
    "Af>hL@wbT%V^P3'A[rX-r?bs8(L>gLsg>)/VEB6&fFBpToD>X$X&M0(S99U%>UsP&DO7w#p%uS&B%u>$#Hn<CEwL?%6S]F-1);Q&h$Bm&FL7<$n5g*%$XBb.vDi0([wCv$80Ap&;NYO-"
    "?6+;.T^>n&23O2(,+7U&LE,3'TaYN'NwN9%&CV8&=**=(h?gm&-);Q&ui]#,PlpHQq`G3(I_ws$>=7W$u.uS&fvRn3JJl^$W68W$nnkgL_#pcMLDDmMlDqQ&XHK6&Sr=tUc.rV$Tm:k'"
    "W,2O'WSg9;qZG3'OE5n&tD,F%'Xn0#4g(C&WDBt%I-^Q&K*'q%lPc'&e3B6&6?oT&J0BQ&Z&Dk'?X2:1Ya5R&HLRs$e`.1(=%KgLVK6n&$+lgLRWd3'<wZw')ncN'eV%1(aAI-)O6B6&"
    "Pduj'^>X8&q2i*%d$'6&1Gp;-Q68V1RTp6&K'9Q&<(.W$>CNeH<OC-Mc>L6&FnAQ&+:1hLVZLtLt%N$+PE8p&v*84(Qd10(W?KQ&GnNT%JRIs$5r#I?)M=,M?:XJ--e)'%iY7t_o?X9%"
    "`O^Tr]X*t$C?*<%C'TQ&-k.t%JhNp%QN5n&76JT&L6K6&**HD-)$?D-#.^h8fVV8&)8Kv$`%[x-s$cgLB-vhL4Y,<-G:cG-IExY-XG0X:aqjp%x/rw0Ftm--KaNX(ldcN'[Q`q2hQPN'"
    "=.?s%[lb?KrN53'OB#n&J'06&Bn@u-mX+gLsak5&2'*I-bUNp%ZQT6&qqOgL(/L6&wK#@.QT5n&eo>q&ZvPn&UZlj'-nL?%@<0EV#d>T%.Nc'&ZL3T%,@L`$o#cvI^^'Q&FL0I$u0.kt"
    "f?pm&]_)q&Z,iK(%8JfL&7LQ&S34X$h[FgLx-8W$9+7<-QA:@-ccUt1VmuN'NQ#R&[d>n&x*cgLkrv3'HI@W$93/ZM1k9Q&e`0T&dX2B6b)CQ&REB6&QWiiLITdN't3bL>T(1I?xl'L5"
    "hNGN'5@R2(D#(N(,.GR*jZcN'if>)/XHB6&*tL?%qSct$]8rK(WK^q%A_&Q&EU@w#p%uS&E:LZ$Vg5n&a6`;%+q*Q'-,>nE.O9T9js+t$N00q%MW>3'>D6X1QJ(N(WuK-Qc0K6&9-sS&"
    "7ga;%9QO6'UKgQ&WmYn&OC]7MIevj'Q0tT%N'Z*7f<gQ&,;#s-0($q7R'&t%CkE9%44l,M?I0g-6wL?%(RcGMlQ^s$dT1.$,f'dMWW+t$faUtLcG?n&L.m'AS>GQ'],m3'lLw:2_d#R&"
    "B*9q%Ygcj'U?g2'+x-q&R6b9%])2O'2*]p&X&M0(1b2<%P6KQ&9lb_.InNT%1lls-pnOgL-_YgL]_vj'T?Xt$HBX3(YmYN'Hq8Q&Tt)qi74lgLUK?n&8`-'6^&vN'VW5n&Z)2k'iiIL("
    "eSnH)P6B6&Rm:0(61DT&TNgQ&&CMs%x)i?BOTAb.mvUK(8*WW%DFRs$rl(C&N<xM(5uql/LhwW$X)Z7&1Plo.L096&+B^aYaNHHMJa$7&ZJRh(P''q%YNs5'oL4gLh4L6&kL+gLeN:tL"
    "b+`dM[2p$0WsuN']Q>N'2>R.%f$bT%,<Jb.S?sp&I9K6&DTd9MZ,S3OnK^Q&&[hHMd/]o9ZmJ6CDq.Y-gLdE5k[$2M$3Hj']UF'S#Pb-+6=)=-:`JcMXQvj'=ZP3tg0Km&vwqZ^+Zo8&"
    ",4>R*2xbgLqs4t$PE^6&EIRs$l0*Q'&%EM-JQ+?-u(XvLAH?n&CLvS&arP6'^/v3'Ym:0(iIC1&xHn0#@8:<-jB<G;O(89&ALes$2GV(ZGoh&&KnBwK<wZv$fO+gLYG@I-dJ#D/`GR-)"
    "*;JfLVS63'YEFX$IE>3',7$Z$I.Lv$-ocW-Z%+wp'qY3'WHp2'cG%1(a5Mk'4(.m/c))O'C'p2'Mp'<.Q?p2'grg5'E(Nq2n<^m&V5%h(v*u,M%Ya>.bDi0(VA@_/CLmX(+?Gb%pml3'"
    "+UChL)8/1(ODiI2X#m3'HQu/(Ut3t$TZP3'/0ZX2W;`0(gD.-)A7.W$Td(k'h0'C-Me-a.T;ik'hDtJ-$5pr-*F(hLB(2hLJNwi0PK>n&P?>j'YmcN'U)OW%YY*.)k>0,.r-cgL;klgL"
    "9WY,M1w(hL9cmN'^sG3'(=O-Q9:lgL,01U%P'Z*79h?['nW#R&tHpe?ekj5&kXT/)pVnuLT9NO'GXw8%k[bGM[1'C-:`/,MU>T;-&Q^s$iTc/([=4I$9@1-MNDDmM5sm3'dvGn&t6lgL"
    "Tr]<$Rs10(cJRh(w'(hLdpvN'Zdl/(,R1hLkcvN'*UChLXuiQ0M9g2'UZG3'q$lgLhGH3'6@Rm/RE^6&`>iK(WP.J2dYR-)T9b9%fOgF*b,M0(xNp$..e_hLQIUa5b22O'`>.h(gl<e)"
    "qc%l'e5d3'SN53''@lgLkoV0(2cl8.Y5@-)DVvk(b#Zn&]pG3'QK5n&HZ_,)RX_D.]Seh(4K'n'Lqa5&i`7-))[7T&>?bd;vV$-b`ES>-&To(5W?^Q&fv#7&RHpQ&UNtt$H6#3'S9'U%"
    "^8bJ-aPF0Mu[#tMs.a0(dpTq%eX^TraqW9%:-Ws%jkbgLbFWN9he_$'5&%@'rjPn&[>.L(Mn5WfadO9%_onNk77ugLC3)iL4Y,<-I:cG-JE4v-63+L:4nDe-JMC9ru7(T&CSM6M>'2hL"
    "YlbkLp`D0(?@vS&K5YQ'RN>n&$(YgLm:hQ&.7>R*g0b9%A6w;%x9(hLsEx8%Q$P*RN[;39/.0X:p&M0(gLAR0dcw-)]d,n&PVIW?#<4m'MEQ-H8=;s%,R7p&,Im;%4JV(ZG*@q7f9Hv$"
    "E*f:.B_<9%I/Yq&XmY3'])v3'&xt2(qM?*MSg=.)gX=gL]Jx8%=Au%&*P4gL5'MHM;3*O'^^>N'jinh(gM%1(5(.m/h;Mk'G9>N'O#CW.UQ>N'XAL[-H.Nq2qN,3'[PeH)#4(-M&`j>."
    "ii[h(VA@_/CLmX(i4QQ'`GrK(67p6*#<dn&@5@T.]5;O'KdUg(Z0X9%YpuN'b`3e)]VIh(ncnd)EIRs$XvL0(i0'C-Ne-a.YS@1(iDtJ-%5pr-0X:hLa05L:&2%O=q^c3'TQc/(^)2k'"
    "#O'F./Jte)`?7W%*]s9)>tfQa+<5F%'Q%1(^&)O',_#`-OiT-Q)ZR-)]TTq%R*Z*70<0j(*$Xm'(hZaN`h+t$iqHT.XupHQs%`g)S-X9%3cAGMj[W3MXG$iM$i4_$e95N'CjvTM06kM("
    "'x2j(;N]'AWc_j(kl71(m;IuLW(g<$W8rg(jlEI)^)i0(j&4(2b,Vg(iS`k'f;Dk'-_LhLP,:w-re4gLZ^mN'u-ugLkYmN'5:/bInfvj'^>MO'aa5N'dS%l'g;Vk'ZrJ'+o;)O'fDi0("
    "j.tE*eM`k'2eChL$o;b2jY@L()fTF*mx[1(WTpQ&f7[A-lYKg-[EvE7mZ>3'8vNr0qoe-)`;;4'VWGN'`C:<%i,Z3'IC(d-&*=L##3M0(_@G$,ssn0#@4Pn*MIH68;j9+&%Xkh)`V*e)"
    "g#dN'SNgQ&W*=T%8P_m'Oq39%L[lHmCI1O+`^sP'ag^B]%'_q%i_^Trd-'U%CENs%evJe$t^5n&T<Tm&)U_-MHsN30`,vN'b8mn&Qe0L5@:U9VA<lNbn0F@TFSKe?YNs5''=ugLgF5gL"
    "Qi$A-8if(Nw,f/MQI)=-Rs@n$BjFQ'a#vN'.W`A.XZPj'4'Ap&F(c(.&L:hLkSZ3'@_IU.TWY3'Qg9s.R>wd)4is9)^LfERW/Y$09@(hLY^=O1`;V0(lo@1(hVnH)2tL?%>gE'OK'CPN"
    "7)0_Owc>T%1Wc'&aq&6&4atV$8NkM(_AGW-*Ccn*b$BQ&DePhL.Lo/0cJik'2[-k(@t2>0h7Bb*Qkjp%$&`$'H7WIH,Y=gLr1Ah(<Laf1m+OI)a]71(trRL(3[:hL$.Y.)Ww39%w<(-M"
    "5)j0(P-$I6v]Na*f@M3(X#;0(iPMk'Va./MTcOI)CZToLY5MMN6XPgLq:]H)qIBF*Kc;d*6:GR*mr`K(Y0t5&k.]1(Wgl3'c>`0(gGr,)%u5c*`v,n&MZPn&wbofL@ZQt.VYsa*^CdER"
    "$On0#+/4.)F6kJ)5dhj0[)Vk'[vh,)gP%L(^`g9&hU5c*ti71($I(hL%,j0(pnk,M59MhL=4/L(@)%hcx,D0(iu*I)`X)bRrpPR&,O:hL2Lr9.`&v3'84%q&PTg6&Kb:-MF3D&0j2;k'"
    "M?Kq%rHBC-`Zl[$dT1.$0x9dM_2:U%laUtLHBieM9cwm/w_pF*b2%-))7=_0Sdq,)gV;o&<*`hLw)2hLV3PI)kLBF*q.OI)jin-)H+TgL=)N0(Z;Mk'ZZpQ&:PUm'%+YgL6-jK(ec@1("
    "Be1O+)#oL()kH22cOu?,$g%1(q%+.)s[,C+rU2j(^>Mt:0Tr0(u1=.)6FmC+#]'/)#Vhp&jiI1(w;Nq-tVI0Yr#)k'AVGl2%M'+*if7l'_&;0(J_ET%O_NeHLB7.Mo@jk'XgC0(Aw$iL"
    "gxV0(`;Vk'^I9HV:E%IM`x&P'qkP(+dvl3'`8%h(W'96&^Dk<:iQYj'ZT[I-_*YsL$?[:M%lFI)tVZR&xjnguhETq%Hd8T&q-(hLo$*O']^>N'iinh(f%Q2(=dkm'g22O'dIH*RSn2R8"
    "(1O2(GxbNb<w;bNr>2=-?m>5M/;8L(0h_-M,vRd)]UF'SBq$IMdYV6MI.v<-f.@A-=DNfL@7;X-8VuNFs/Dk'hU/;/jlnh(,[ChLu@f-)P,2i1^#;K(]mu/(dAV0(pX=gL^X81(dD`0("
    "c;Vk'f`?=.krnh([cc2(l];BQw$:F*koR1(SH't-)6a'OCIw6SFx1a%3^c'&c-KQ&=_Ns-2:QQ8$O)W%I3+V.J-0q%+#9;-i`LX1fM.L(xhY_+p[no%FS0hP/N-V%ZA@h(/RvV%Y2PW-"
    "n]C_&AhUhLl<YI)mMmR&w_p'+$P0+*'6k-$;]te)U-BQ&@TJb.v)HR&lI<#.1$@iLF#en&dM@L(oGVk'0f?e61VB+*EZToLY5MMNl[8;/m%4i(AE7iL7Iq+*,0(V%)2'+*S3^Q&#&AL("
    "`V@h(kf[h(r%te)(AAP'c?'U%[AiK(_,0X:U/ks%jXTf)%g3vL]C4**=D>nEN)@-d?&?k(f%4I)b]wh(5hUhLuDZp/6=6G*kPRH)?@;=-BNYO-/Y>W-a):-m2ul$,ML[j(huN**8AIb."
    "'jwd)se-mL2Lr9.jPrk':F[Q'UpPn&VwL-MF3D&0qMi0(QQp6&tQ^_-$0FL#dT1.$3.CdMbGhq%naUtLHBieMIH*$5*4m(+jVnd),4q+*X&[d)q.8P'wSHr%4TIu/xUB+*sq5(+?77Q/"
    "u@ke),rg+*]u1n'cM[h(gJ`k'BD/$2d,Zn&iGDO'p=Tb*@N7iLDb6C+$JXe)`)rg(>.1J)8'`hLi%lI)A$ihLf:_+*w/B9rMiTF*<-s`+4O.x,n`@L(oRp'+miIL(s(fh(+q*Q'$l%O="
    "HxKf)$Y5C+e2dn&X3kp%=s*JM8#$_+>uql/fm^U%xnt2(9>58.fJi0(2>c5_Lb6<-$b?M2oDi0(.AJl'fJ%1(kVdr%40cg.i;vn&o]wuLwr$*..UChLd4DhL)4:+*'#3o&&qngul^,7&"
    "M55q&ZPn-)iPr0(0AE/%d6)@'1A0F*o4'F*sG0g-IIGj`.$pN-Fm:l-_9o?T/qn0#BMTe?Ufq/)q1OI)bRhs%i++I)mAg2(OML[0IP1<-IU>hL4QrhL<hYhL9i0F*F$f2(5@Pn*AV6hG"
    "(me-)=(2t-5kChL5l4I)euw-)f1k**?IjEI'ZU-*liRL(mr*I)U13U2.GBF*1J[w'2,k**wh>C+)GFi([S-F<,frS&0N,g)8WG,*P9rNr3'2hLe&:6&%R6ENOoh&&9U$IMt&:6&%FChL"
    ".9MhLZ;ph(uFX.)IpA.*A-96&v*;@,[6^Q&*/`$'FMsP_bs96&:aBj('$o0#<Y^b*r:^b*,46c*'`0f)C%OB,rJdR&X>ik'<%.W-U6$I6*J>?,i[I0)cSI-)v4fh(Ya./M]I6c*GZToL"
    "(]v.'AC1O+.>b.)ETIiL;hdc*h[bK2tRtI)]r8'+p#-7&mr*.)50ZX2tw,c*41m?,QtET%n49F*wQ<nL1nXT%&o0f)eZwS&5v:d*AJ>nEw^KJ+v:/-)j7Xe)frN.)9q_hL8H-n&10AA,"
    "cZ3T&5ls9)C-gQa.E5F%^UhWh42=.)s,67&hJik'(T4K0)YBF*ju*e)%9k-$D;Hg)6<*bNgELq%.m?*3f.qHQ$fs&,fs,7&6cAGM[1'C-A`/,MVGpV-jmNL#%saa*[=4I$I0r-MNDDmM"
    "@%Y.)/M4i('7,C,rjtt$mOT'+/@`%,+?EU-o&4(2w1ka*-o9f)(Ste)BN@iLsIOE*wxaHZ-veh(gMI-)9aKj(-@lgL:jX**sOk.);IPgL3AUf)/tdM2pH3:.5M=.)+ogF*.bI=-+4emL"
    "'9Ev5(%6G*?TtY-7+_F*-2+.)ko*.)%8OI)$cBf)*hF1)u_0/)-Xr%,#5x-)#,FI)Sjtgl5U[],M'/Q'5?=L#5Gt**u#KR/)6o0#(dIL(H$w68;j9+&0^`C,uwC[,*,f-)fJVk'm#qm&"
    "&RLhLN0U6&eI$XLBeChLl=f-)ZjjZp8l,c*.>a4'vq^TropPR&'ak5'6NSb.@>te)eo[L(w(oh(of@L(xIb.)&7;`+HF#hLD2K;--#9;-%#9;-m<$p-oj^9VRZ[w'F)1j(?rQ/)7$,I-"
    "@w6iL^&b;Mk,l?-]WiiL?H7eM_`.iLcL_lL0@$(+poRL(U68V1kl<.)v(4I)ofIL(=O>R*w2H7&DQs8&7'rhL#t96&#HUd*q.=.)b=@g)ro0X:0Ygb*n_=O1vL'+*1C-,*+xu$,N?F]@"
    "E;0(OGbE7S*v#T%6gc'&kZYN'E^Nb$o#cvIko.-)CN8T&v*YgL>WGI)sCte)#rPC+Pqu`4Qt]Q&i:te)upgm&.q_hLD[_p7ik%T&2ocW-mC+wpBc0f)%&xd)7bi`+4Cm(+=(.m/7+$G*"
    "YDwd)Sp'<.oiwd)oe@g)WONq2.mnH)#@`[,7n_-M%Ya>.6bM`+VA@_/CLmX(0NGb%3`Tf)Mj[iL@<&a++N/=(Dxg+*b7#_+x>ZR&u@te)70ZX2&O;`+>[r<-U0kp%vX,(+wQ<nL1'(q%"
    ".@$G*fd<p&9A@a+JT@iLJx?YN$MeR&x74i(t$6(+#Pte)`dP3'[2oiLMNXE*B@;=-ENYO-1Y>W-i5:-mEheX-ocnd)uhG_+8AIb.1PK'+ue-mL2Lr9.v14i(<X<3(`GMO'gBi-MCnG)/"
    "*2+.)igNh#*:8g)Dc8GMWF^$.*=UdMgldR&saUtL#@h+*P:m'AmH:?-%Tvn&s^9tUt?0Q&1eex,69SA,&35g)wrVb+)xl(+9I)`+.ogF*<>)C&,P^+*5hOG/xCke)1h_hL#oke)?7N*I"
    "5#]1(1RM@,w+te)8*]=-thvq&=YVT/57v_+.oBf)9Xd,*&UtD+9^Ke$FlK+*;X;D+Y8U;.>0s)+rl71(gOR,*HZ11.B*`hL)4,I).P'f)Atr@,*(_J)rx3e)X?^Q&`wNeH[/x.M$=(J)"
    "j]Ee)YaniLqnbe)uFkI)uE$EW$pk%6'&+.)@1li(r+=.)x=sS&`V*e)tirO'i+Fe)^2L,*Z/Ug)w[B+*1o.h)2[M`+8`gB+3,l00fiwd)h#?n&d'Ha4pVvR&&%Z_+%P0+*xLBF*(]te)"
    ",%1/)^bxB/na;gL*IOE*2&9;-B`JcMB(m<->Pj+M/^7.M6r,$,]UF'Sx/rw0FwUhL8O5hL>D;=-CDNfLlen6/)c'f)8q_hL,'*S&_iDI*xOkI)%rPC+*f'f)1CYgL2God)#GXe)%uXgL"
    "wf7iL1(ue)wCkI)0L#@.+.6G*mt3L)+fT+*D*8],-:?,*PGYw&;TX1#ZeKj#J,(<-<(MT.Y)Dk'=C)=-WfLa%>n?IMwA-n&+`&:)/Gke)$E(h)1=ZC+Ag^%#(<pN(83fA,X;.h(>ulW-"
    "vDO1#2uXgL&1hb*CLaf12RM`+x$q+*;FQc*HE%iL4NjD+la9q%0nU-MA.C+*Z?$I62(`W-lwE-*l+X**+fbI)[a./M%3:tNQE=KMAf>hLT`fL(4C)D+Re7u-L<.IM1xKb*jC-eQJ[Q,*"
    "kl*i(;wUhLq8LB+Eg'Z-x18L([SI1(07G,M4ul[.#_iD+rDtJ-*5pr-c#Ko8%t`-*shgb*oLg+**cT+*/;_6'>#(Z-(pM4':rs9)F6gQa0K5F%E[;D+*iT+*;$v[.s(+i(/$U)MDoSl'"
    "#AtI)xI9f)FP;d*CEM?%oY$7&:c]?5l4qHQ+FP?-pDvn&ZagQ&kY2X:6Bk-$dT1.$Kq&T.],mR&saUtLHBieM6G%q.?EF#-acO0PXB/a+ecJB+,(lM(UvRiL'NihLeGN`+/*]x,7XM`+"
    "0=mC+P+TgLH(u**pR'f)oY`k'JkAL)s+X**%Yg'+'(hf)R'2O+>Ovc*3tdM2&9,o/D1U+*7O)D+<N'V.#%0$.Pg@iLANIc*8n`%,UMUZ-Lgp7/(JbI))Or[,']0f)GK%iLlWfL(-(&O="
    "W$E)+7_R=-tr.l'dZ^Q&L]'gMOhF#-?YRE+k>DK(EK]X-2W,K)P?jn8;H*d*1p[[-,bwt-7]t**orIL(v>?3'BR]d*j)-R&$ZwuLeF`hLHgt**5dG,*m7NE+GZoa+@Z@iLhLF**TcIJ2"
    "`Jnd)%MkI)siDo&(1v$,J$'k0$Y^b*+oB+*/.:/)@5dG*CA:<-(,TV-+*x0#3'o0#Le(4+eO?tL2]gb*m/S$9HdAp&-:B_-tR%O=NqLk+Ba*XCa;x<(HS;d*/3o0#qGJn3MNIiLwcbe)"
    "dY9p&Ivv20(l,c*wF'+*3:HG*q]Do&t];o&&Yt.)fm,n&>B/a+&]9f)&`^b*x%1X::CD%,q_=O1-1?(+?-N)+6O.x,A3M?%HMK(O*ijG)]+q*u%Ag;-4G5s-MVi?>iJ%a+_AGW-6[cn*"
    "p)`K(<LVX-fPman;o#c*ATTr.WfJfLjB_b*7`a8&mOpb*1eV8&j1IQ/l[5c*5h.A,D.Ob+3ffP'BTk:.F<fx,6Kk-$VHAA,e^PN'4OCHML%L'+nr8'+9li$'-pR1(:t`)+G<n--$kP#-"
    "@hbL>#tu?0@E4m'=#$0)B3f],Z(c(.UHihL3Y%*+2_@],e#Dk'CO$G*uq,(+/7Z(+9R@A,Jt$0)#*67&oR9F*h/0X:hxgp&-B4B,uDtJ-,>58.HHxx,E<5b>1oNo&0uTf)+nr@,1:?c*"
    "i2Dk'k`FjLS)-_+RbPhLtW)k'G+FB,>Um(+UAOjLDHc.)3R;)+EH=#-17v?,we-mL2Lr9..oTf)?t80)hrI1(xd..MF3D&09lB+*aDM4'-kWnLlw&4MVGpV-osNL#4%2w,vckZpEv?#-"
    ";N]'Ah?1$-LmxA,)4))+27O*+gL0/)D*SX-6@$qrgE&njHffP'LgX#-B0s`+CWtu-[qt?00dMo&/%qF*:hU-M8sH(+).vC+*`'/)BKFu-[0x:.J)_r.v$WR'Ok0O1E$Sx,>b))+IH8*+"
    "*$6^,BjKe$J9%#.RV;50c5PB,L6*)+*i#(+<bvC+9w[x,64/J.669r.L+G=.HnvG*;RH(+'f#G*m3*q.fX_D.<s,<.BuNB,t`.-)T81r.6vu,*,+ZC+=7G-&`J%a+A16v.6BK7/D:-(+"
    "#GF.)*d)k'u*2`+rAQn&*x>c*=Grd*-u,(+3@Z(+pRJB,XS_$-I&fiLlnK'+(.&J.gosa*VxdG*LLCgLc_.iL9Hf=-4hR=-'QK,.IaweM2QrhL(klgL8&xIM&l@iL5b`=-bG3s9I0;d*"
    "PHe---oqW-lk)bR3'(a+aIE)+aJkX(F9+>-3j(d*BN>-*6n`D+,rpb*l@8b+57Uf)2.-c*Hmw.M@t)`+'CmC+([%],Im]5'n&Wm.6@)`+6]@O:Q5Dd*Q#xIM_E&],@Z9V.J$Ed*6.(J)"
    "fpO<-?89;-)hS`<WsJe$5?k-$t;rK([&a2%0xC[,NgFW-:ecn*tDI-)=LVX-kVmanQO;`+QMVP0DXKM'qt1%,ECgp&w0D%,4'Ap&>>TV-wc(C&JWkY-GIK_,@Fl2(QJdo/U)C;.=Tk-$"
    "c>(Z-n,D0(QjJb.9]+2(vI<#.O8tjLWI(/)1F;D+C:-G*0c-I6YGq;.UDI_/CLmX(4ZGb%H9f&,hibjLSoZ<.^DiI2AWxA,uW<U.<>8P'9er%,=0ZX2A5_;.bGZO0fm,n&:*=Y-LFSrL"
    "bAxd)e)]iLj(2hL7Gq;.?Ho0#Lj-i#P1)[,)v;o&F*jD+@tr%,+95O0i]q;.2IMD+Jr@&,AlqM(KO^Z-L9/&,ai'kL'l_f)?6]&,T8$<.?$SX-$f-mL2Lr9.8CQG*A0pg)rIFi(20J.M"
    "F3D&0GLZ(+joIl'/kWnLlw&4M<^`S@TGt2()_[9.[=4I$]/o.MNDDmMWm]&,[NjD+HsIiLhERV%6KbU.Y`%60PWwiL.bf],G*o9.h]4jL5Tf],b`XjLWlM60+uG$,;e`%,27%[^A[;D+"
    "X6<^/4R2%,]oip/b<((,Z#G',-pi0($eP70Mv9Z-@Mo<.ZD:;.ZcDp/Up=^,PmaV/e[X/2w1@[-^&K&,5C2%,KQS],Ip^V.53o0#`/Ml0K8do/VTAE+I9a%,3F;D+xsoN0kRC).c4(0M"
    "2NjD++o1%,xYtjL)*W%,8eMD+cI:;-2'<J3C=Z(+ddNH*5Fv(+;UuQ'sec$,#x`/)aO-eQF'8A,hKRI-Eej/1=ni%,QAq;.Y3ot-?2#8.6k(hL,<01(/n``+;h.],4URx,FNSA,<kDD+"
    "EjBV.BT0V.JYI['0In;-5#9;-+,TV-8<x0#:<o0#Rw(4+j@3U2SuN9`IH4#-K,Gm'#81X:4xIe-qp$=1Wvw<(+BLdODJ=jLM$PsL>pBV.Z86d*^68V16Ur`+Eni@,;Lm(+Qq>R*?`+m'"
    "3Yol'S,+jL-jDk',G]v-PpeiLJ//],1A1X:Md9;.x_=O1EN+>-_i_?-T,Zo/s@a2i,V,<-C89;-0-P]=9>`D+8Tt2(li71(SL9v-_AGW-?ncn*%sW**Lmch1>$J],B0S],a==j1n%KfL"
    "u;&],QqP6'*[@],4*S5'Y8ls-7H[iLg>+jL2cCZ-Kthj(^+&m0b`Z8/C^k-$nx?W.vJ.h(An_HMVtiw,*oLw,E1j$':iBJ)S>L$-QKn--'0;Z-uRd)OO0]jLh_,hLdwZG*_Yds.d(c(."
    "k&AiL9:t#-K2Z8/tV[h(_g8&,3-Sx,FK4#-Sp>W.h>9I*7jV4',[D%,q50X:v_dm'DoDX.%EtJ-0>58.erv8/9170<4F,m'F*jD+B;hV.HQo],e>Qp/)r&:/FnIX-WbPhL$0Ah(HF#W."
    "(:-5.k4LkLDsZG*YAOjL-1<9/INXU.pUN#.N,o.M<?jD+@wpe?2A0b*&2,o1*WnuL,-@[-&ji0(8X_HMO;7/;guRn34B9j(2VYS.ANt#-@G#W-XqYn*X>)+.gFxi1g&JiLP8=_0)4*U."
    "Os&I*%;ljL0/fiL(L39/UISm0bo29/U>$W.pFj9/g$B*+:$4#-@k;)+2<D].D08&,Z6<^/>-Ax,l_O31rPE&-i`h$-8A]h(5a@P1YVQW.]VdS/h(R8/gC]m0CXEnLKt*X.-H&f3x7[s."
    "fd4#->6+#-XdX>-UG_Z-QHU]4OVq$-ekb31Wjxx,VN4#-,]fl'Jk#12YD)E+lRC0M7sf&,4C@x,.&CkL-KS],DEfA,t<_)WHPbJM7Am',fE?L2HXvc*A<X:./a[h(;BJ&,0u1[,O#]iL"
    "3^]],D0Sx,KW4#-w<_W.x9#;/[fOjLdwpj(<*4#-#W`k'IQ+#-HpBv-668a+TQ]A,E$a`+R#c^,Y4SQ0SvRiL$#:h$JvL&,,,TV->Ex0#8MUR*OZ4#-WOo&,Pp'v-_E4v-D8kjLt-N`+"
    "O5Lv-tAxjLHwRiL+.v<-d+oQ-ODNfL<PAr/Qjx],B$/&,,nv.3WG$w-HWoA,J#$s.Rmx],I-2hLH$<[,G?A],&S>_+oMY^,X,]iL]NJ&,MQ;a-;SbHZup(C,Tst>-x[Jm0UJU$-Us8?&"
    "8bEU.H=P8/d+iC$?pIiLtF]h(PrV(Z_l>T8p5=m'W*f:.o+xL(RRt>-W2xiLARW50Fve_>l#7L(:a'Z-MSS-)K,xiLI$7q7oKtM(9x(t-glXjLA]CZ-V?SX-stFR0nCsT/J(.m/suZW."
    "#@RX-]p'<.D$SX-%'2Z-p0Oq2M$8x,S@=/2Xve.M$M<#.+x2*OQB=KM4D)=-<+`p.[+<p/j6q6*8cQ98a6+++K@,Q'S'Q-3@h@X-JgF#-4uVO(#[H-3e)>++$x$X-5`aW%W'a-6+8K'+"
    "#a=jLm:MhLE[oQ0M^o0#A&ij0CHb#-C-tQ/YDhV.b@@K)ZQ;.34?0,.Q8fiLO8xiLEJr-MI]XjLbF$W.lx%hcMH4Y-,Vj^okx$[-Hn2-*jal-$Sn5W.^]BcNwcJl'sbf[>1SqHQ<5ug2"
    "@+ui(w.AL(kY2X:Bgk-$dT1.$Cw6eM#@uM(,bUtL>]6w-L.m'Ap;AQ0x6KU/)<IuLmpa8&IV)P0t*$L2>?EU-''4(2`;HO0sIwW.kl?W.uFCkL>&LR/=+uM(Pd=#-OK7.MIClY-Hm^;."
    "G3s)+j_+j1Nj&UD:Oh,3Nwq/)'uv]6q1&Q0eY_Z-u_Wx-p@ap/IPLt:xP_v-w[Aq/R;TF4)[YV/H$<)+a%ew-X>qV.30wE7J6Sx,]VGl2,7PN1_r-%-H9oX-vV7L(-L=eHVrE+3Tuql/"
    "H:PQ'j*ba,RPlo.IKf],a1Fp^P0NT.gb0,3_qx(3[UKG3n/cu-A'ND+L+5I)DjwiL`SfL(x8bJ-65d]3ME/a+V2:Z-Qv'Z-/dO?6b1fQ0F(82)DK'Z-*vIL(R):v-SMZs.=Z+B,`)Y#-"
    "X6K01[A(Z-cl1C,U5h;.>,9;-GDNfL>AXq.MPj+M3LXJMNA_v-Ver=-MHl0.gib/ML&m012B?B,RSds.or:[0`p*.-Hr[A,t@5m0M/fBJPaQ?-_,:v-HNo],.s3X.Z25B,UpO>-UMhZ-"
    "Z$:$-HT9Z-B6F#-slI/2?u&,3k2c>-V)Cv-^I=/2E5<Z-p+(KMr+3p/iq'K2v(7@-[v8a++VM<-I89;-L89;--t0>>Ol<#-;pp/)#MO.)Z6M50_AGW-G'dn*/]5C+]7pw5T2qV.X>$W."
    "&hME4gBdf(1BBV.hprn':^^V.6<4m'IbRW-*#D_&dI]Q0PE@U/b#Oh)wQ-H3'.PN1Opk-$+@pQ0.&=e)U5d#[-KbU.(B.W.u<Oq2TK+Y-_$hG3`2x.M$M<#.4=N*OgTpp8H'H,*UFSa+"
    "wn=n0f[rX-P?Y<U`j.U/D`X**m99Y.CBF^,%UV6We(oQ0-VDc+IJ]l'=^FY-x80X:]%`j(X61S0+EtJ-25pr-+G(kLxYi69v5)X.OlvS/I>vW.n`XjLRFC+*1J#G4bBRg)VCt9)TagQa"
    "8d5F%x3>R0elZW.(Ml%2NT+B,q:<X.xU8Q0Uv%iLn&$S/QLr9.X#c^,Ggha+3n2d*Z)5/MF3D&0l/uY-(%U/)3kWnLlw&4MVGpV-&0OL#S>;L1[=4I$o(c/MNDDmMg=<X.-;%[-Sr=tU"
    "8pig(vaMe3(KnL25fxV.3k@O1hC4R0-.PN1q:WT/b%*C&_Y;t.FBFU.[D$W.?I%[^d5CZ-UfQW.R/CV.0-NI3V^BL4-%9Y.J+ue)TOm,4qq4R0th4N1-MGjL=[mH3Zc&gLW&dR0K7n]6"
    ">U5n0*m?W.R>?W.r%39/neAU/A%LY.fnAY.)E&f3pu-W.o]Zs.=O:j(aAFc4v5gD4Xuql/Q[1n'wj#C-VPlo.RsX>-f4os]^QS:.+T]+4YgiA-))(D5`^+B,XSDP0BPB+*PP_Z-Bk*:."
    "U]Qs.kQ,_J#n4kOdM-W.wH1O1+vV11/G`o%CF6K)RBNd*=&Qs.VsF#-P08=-kRSq/bims.ZV?<.c,u>-u(R8/P7sJs;Dq;.<#9;-.#9;-('=J-B#9;-B;pr-m`FjLO2(Z-h+Np/eE4v-"
    "QlTkLRm,H.nCaT/osQs.hM+jL+.v<-xNXg$Ijo0#rrx<(nO&:/X5L;.u9c05L%@<.iYH8/[)u>-p$c/2ai;p/Plm8/Q7S21E[_/)n]xF4hoDp/Y2l#-wRqkLhSQs.Y8at1m%n<.2dfF4"
    "rxq;.NQaoA?f3dtsPaU-5bS`<>sJe$?^k-$5o5(+?+kX(,.dvI-9FU.LJ-K)@<r-MVlhv-duZW.o_st.oga91m(uK0s8]^5@YgF*Mi`$'jOhsJtO+.)?+g;.#$Ps1*nHL2dqSU/:RPR0"
    "-AujLb>RP1LiS1(;F1/)lqNH*Bc+.)q[v1M?vt:.j^;a4gD4/M%Ya>.33IL2VA@_/hcFx',mNT/rBlj1%r;kF'(xw-fkGT/l$_K22,;&0/P@<.NcmS/ZD]v2vX0O1:7JD,Rf42(D,:;."
    "&<0X:$.&0)cpQP1.EtJ-45pr-4cCkL-`^h;EWAqD[AZ<.U&)L1pO/Q0oiOjL2J_`5<?0,.`c4jLCPfb--j5F%+kUO1qLsT/xG[i.X)($-=98N//7PN1]58iLsM2P0SLr9.cM_?-I#IB,"
    ":3a)+hAG/MF3D&0x]q;.0IHg)4kWnLlw&4M@,xlAd12H*Ki@I2Wo,K2%D(0MNDDmMmkSU/:lw<.Sr=tU=28-),H+(55;Tf38+uS/60=L2swKO1:ehK2'roQ0i1*C&ndU?-VSVT/^DC?-"
    "RMJ?2VaAa+eM($-r<dd3tft1)&A@c42_lj1MTpR/ivMS0*[1O1(=YR0QjS$91Z3M2w/B9rGEe-3e*Sa5Vu;U8nuZW.ojDE4m:3t.*GljL4jGB,&+tp/;^@P12*AB5ZpJE+>V+.)2BbiM"
    "+-cc4RZWN1Jev?,KD9^5C@,<.d(j50Dfx('rB.Q0<nRlLO:r;.I%H,4ZVH8/vD=_5&ogF*lF/Q0=B4B,IZ4L#i7jp/d%WT/<mXK0$[-L29YHF+YjoguBemg)B*LF+DP`p/bMUv-XNxt-"
    "v-YR0+3WU/d+<t.pc6<.+Ya50h1*t.C=P8/Q`a;./#9;-4dBK-E#9;-^-+V.mR8q/b3x:.lLW9/7$,I-d+qkL'3Lv-BYkP0eH+XC?Ko0#KaNX(wdH-3A=P8/b0D>.r3G70b]?s.je4n0"
    "c<Ug2nRfQ0`cd8/hw5R*e?nK)ZBeK)h(R[-@iOI)>/422q(ujL`'sP0Io1X:kubjLOQwW.kht/2+?/'5<O#;/rx($-+Ba,Ms&gO&`=%3#:KF1#F;Hb%94v_+B(O=(uPZjD1ZKR/MSHg)"
    "DH%.MibeW.pUsT/&F>r/:i753f[B+*7l2U8Q:)D+X%a$'UeseHM#D11,e?-3^Laf1FMl(5%?@P1X&=M2B%VkLr_cg3`Rl.)L<[,*o6KE+TLC+*-Fs2MIo;50,2L<7xub/M$M<#.UK^+O"
    "O0]jLZ=m<-E7rp.7s&F467<kF=KHV/G^$6/%+uJ2aeQ_+=POQ1_iMt.(7cN13Iub3ljPW8xCMv-A;6[-dEZgLO`/v.]2#T8HLeERP2U&5*P?nEe.90<OAk-*'fst.wPEe3+Rl31KbM`+"
    "ppXmLu/+J2ebPhL4Vv_+_Pcd3C$MO1H9kmL^*[w-/%IlLQ@u(5,Lhc3$VN#.uR:0MVRst.ZBqe?N?=:.u9Z]86WnuLP3BN2N[6c*SEIIMj[W3MXG$iMx?h+*N6wM1R-eB]8#Bb4sH%fM"
    "PBHp/a$9?7$iBc3F7=_0RPsb360_#._?olL>.:kLIRl(5>@[=7QVl(5=Wae3q+TgLsk=N1j?Pn0iMl#-`((70qF]M1*C_,3/E.p0<33O+R/'N2NHwL2+_Y*=jQVO1M&0f3W-GO:H/Xi2"
    "SwIlLsk0X5[]GG4F2JQ:eqrh3t+7w-=dwP12hqK2c`J0Y)Aa50DOu31kF%]6;Z@T0tLSM19]B+*R9PeHJaO2MTHY70clwM1ltWmLGh=31#:PR0h@WURigZh2SjOmL1.4r/SZUT9-><X."
    "#@6d3^RM`+o<570^2)l0wK(O1*@Y31%r4N1.U1O11<Ee3sZO+53_IlL6l.M1n9Gf%@7h+*+R:k1*wdh2ew]U/;OGR0'csT/;<751D%nx6'O1k1Ws41#DZo0#2$o0#(uh0MLC_kLI7hKM"
    "gKhk1iEk7/+tHL23x_kL=:hF*LC^$.qrXjLm=UkLxBUk1S^o0#JZEX(j.r]5GtH21?>0E3-NfB5(rAU/&Lqg2tktn08L102&]&U/A25_5-X$h2pd_02q2WE4bsjH*<0GW87b-h2io8T/"
    ">_3mL=1Ck1Y8at12wh41_9#<8Bk>n0hs3W@M4#d3Z5:&5R/QuSLikjL.k)D+d7W(Zq-8V8wuP,*f*f:.8h;d*#cvl1.$IL2?A:@6Tp0RL8GY(+r2<.39tN@,uhhkLBu3+3;#M)+@rus-"
    "Fn[lLeca.3DR(,3oBB$7fI.a5](.m/ooGG4O+',3jp'<.,.(,36lU,3;'Pq2#x4j1ARSn91M:0M$M<#.g#$gNQE=KM4D)=-F+`p.M1M@6?'r6*P`]51k88:/4#uL)@Qd2;$`X+343%l1"
    "<h0++wDR2;[^.=/PP3+3i,A8'>Lg6<#Dw/;T_eER86o0#RkI&6q]6v62o>gLDxpC4r,4h)Mdnl1bPBs0t../;7_O#7,bHl1a8B,3N6(^,e1[^5Z2b.3[#_nLDlJ:/>U*mL_KW#7@Q&^5"
    "'VN#.-.i0MbN1S0fQqe?Y,qR/>8_9;;WnuL^>%I4_BN`+_aeIMj[W3MXG$iM(n))+SdS,3,DxTM^uvY6xH%fM_lpD4&W[q98kV]5*Qh$7bF1]5LAj</u,YmLCLhkLUWE#7Ujlo9jnE#7"
    "Uo:`5ww/01,$Q70)kVI32OG70ZMJ?2*c*=.:[,r/Lx`U8O-T-+<K*p9iRM@6qC',3Z6Be4^I``5ZuuG4%iE#7raB$7gUD9rr?XY7Sq$5;BP1&?E3%l1HXA79FawL2Pe.lLHRK:/Ycte3"
    "ud4b5htXO:.ow[-a'[(+Y%%4M_sZ<.:p>L#A8g'5KUROCtlp0#+q_O1?js*4)?:a4i]CnLe)2O12YJ:9.q6-3PE-2;ABf],Ep/F4_=8:/<.rkLk#gG2;p&f3?K*.3c'F#7q),Z6@G`o%"
    "bGub+1r3x-ig`e37bu31,Jj11Rl:d4CdjI3;Q3M2HwUO1_)=e3S_8r8Ua/g2I#9;-6,TV-h,#1#R/p0#fTm3+5sJF4%[m3+1W/F4NJGc4V:sD+K:2X:u?c31$]DE4HsjI3;rLkL9XP-M"
    "quFc4sJp0#UpmBAk#2O1M&oL26hhk1/];G+Vx:d4>an12CM_[6O)oL2,=Cc3)oFf2@E7h2^Gi11'21j2@Qeh2<6Il1YL#@.T.;d441[h2B20j2$mEn:]2oh2u)]Z?Pu6#6al2v6Qo8v-"
    "N0*j1Dr*B,NgxA,n=nY6_AGW-aZdn*N#?S/f7pw5CmJF4I&TF4*poc=c2ux+hg%b4eW9L*t5Jb4>/dG*g`%i#TdIiLi:q[6kLaf1w/D99L[rd48_OB6i?olL82Vx7-keG*(i:0M.vFc4"
    "9N%I6AbK`=7Lsa4-9Na4e(1J3xa./MbEC99uZToLY5MMNM8xiLmh.n9.D9K<1#,a=4LHR*Rnxr8aZ'Z-(81J31&Kb4^2R6WGL]v79:;V07qjH*#9[H3>H0X:i0p&,N8Vx7@EtJ-=5pr-"
    "spOmLxhjmLJNwi0;Z]J3;04r7a:V@6oaLZ-bk?f=U?0,.5OrkLfh?lLRC=/M.E:T9jC%A6N4'hc?d&b4gZp79t6sd4Ekcv.+QIs.'i+=(Bc(@6Z8xY/o6Q-3[.Ql2LgL*RaFC/3mC',3"
    "-D?U;k%Sq;dT1.$[5FfMD(i(,GbUtLq$IE5L.m'A8B;69=A*x7DSLd4PF*b4f&@T0-X,k:6h%qr/K'njbaUB-=/A:9&_OB6*pNj;o)VB-V#Bb49Pp+461t9/k3AI3@?Wa4UYLw6Zq.I4"
    "^SKiLS*UC6,x3B6TXe4CN.E)5,h^v77:.#@$hk^6'HlmL'++B61mVp9u`Pg=a-^sA[80J3_#Ii;]o5,4cHflLQ?;S0qq)D5:8w[7+?a+=Akgu.qds%,mnn4MgYAU/I/?L#SL7x6b8V(G"
    "+/q0#9vj.3[_N:8:d0+&_Mjb=4w2;6e>JC>-pg'57nYR0IGQY-.G$A6)ap&,E,,G4FUt(M?k]'5YiPc4^bWu7Wk_<9T)lmLtVT?-=gFc4gQA&,VouC5UFn]642'j2kfgf3Pm3i2hXiH4"
    "uljN;QfcC5Aq0h$0=rkL8vRiLgX$a5XYQ>6Q#YD4o1)E4^4;D5nx>H3Z+ZG4?Zf--tiCM:FL`g2UOw]6If;[0Dol(5]DNfL]C_kLP/K;-c#9;-e`KW0Y'>o9Q#FM2@.ikLwdj?6bokM2"
    "YJK+4YCDH4d5tI3CmFG499&J3V&Bf363DF=%dBf3[i(D5g#@.<X<fD4hv%v2QmEi2fnn=7@G3]7--wI<IiA5Dp8Y,Ouq4<SE($HNNOqkL:sX>-ouKGN0'YW87VI&,p*f:.S;1_,.qKD5"
    "[ORA6n&%m:DV6pUQL3#-@Fw]6fS;V.:qwlLAd7iLpFgV-A/Hb%`G76:l9u98uxa[.B:;&?MSXj;3ml-$8x]6:ss9V.6aKb.:<$Z.AJ<#._D;pL9-V80O`C)5#Q9J30c-I6-@hk;VA@_/"
    "CLmX(HAHb%WWg_6<SeoLZnCm;%EiI2uD6@7+=0H<jFo%-dUw]6[0ZX2vh9k;g:;X@_6*D+d3u4:+R<nLX9WD+8Ae<8(=uV.kT2g;S7@nE0%B-dkU&[6T<';8KL/_6iHOmLklds.^O3^@"
    "&=1W.34u9)nVhQaG;6F%)`=7:x<O^6EH[i.R/0j2Z98N/E^M5:@7mjLD,H98jLr9.do>K3Zhmj1%qcr/-xS1MF3D&0/2d(5jXN]-@kWnLlw&4MVGpV-BWOL#DOof;[=4I$R#P2MNDDmM"
    "J6^_6W-T*5Sr=tUfZaw,@LEC?Op]J=NGvY6S6Rh;*B37:WSXj;7$-<8M:+C&OtJ$7DjfY6hOI]6e'&[^U7D)5_6F^6[7M[6^<b(?j>;F6V><a6L`@W.D?U(@:;47:>&+j;V1>j;Uk['?"
    "$5*C6AccD>llInC?n@t9^A2_6[iL[65<Cv70&t/<`#Y)5%MZu@4f:&?QN1G51_S]6U`C)530wr@HY_D.59T)?nN63:>1Ki1q&9Y@WJLa4fbWu7`7G-&5r+T8?L4pL&uGc4p4MH>X.`[6"
    ",IsZ@]lds.s<';8+9<22nw*^6QMX6NWbE#7o[%a53G9k;UeQU0=mpguaMOv6Qtw]6v^.#6lOV)5SKCB-vA@i;w0XY7jt/V8&FSA65Q>J4s<0Z7v%-W-p2p0#;?o0#KgX2M_ZWmLW0nlL"
    ".oQA6gK4mL(8l(5'_gV8*F4v-7Z,oLPdxF4$*$T/&U9Z7mXiD58qv<(Ns8U)kxp0#a;nBAxOGc4)U]&6ax1)5WTLB-2ZVX8l0]a5q#%m:+X]&6TGlV7QpS>6mbi@6(.ZD4cs1C6F=:'f"
    "KLr)5jL#@.0jVX8?\?1@6pZ'C6^5QA@:_SA6.fWU89GPb%k7Z1#pFZ1#qOm1#?U*<-a(MT.kk=n0UC)=-3gLa%%FWMMK)(Z-`I':)j[7A6x-x]6B;Jm:*F)+#dhK,3go)b?q)&'5kulW-"
    "axP1#uMT88:Z.j:xLaf1U'Af=$A8Y8qIl7::Z1nLWgR3<R;-',5Co[-%TWT/A5)Z-p.J6Mlx0[6.[N;A`$f1M$M<#.P79iNO3]jLZ=m<-R7rp.>7_g<p?=kF7SDc5G^$6/#I#o9SV[o/"
    "H3)[8P]Ud4)epV8:0H,<8XU9CtC>.3$m]j2CmW-M4ul[.$-S3<LEtJ-D5pr-F>rnLH?n:9H/rU8hMM5:_W_X8:B)X8MF'q/#6V9C)X-T/>Cu9)QS*Z-R:7Q/].>7:Y5%3D]J7T8wr/c="
    "d/T^>4SC71_?[*7N?g?7x^_2:esBfNI,r_,mKS?Z,YrHQmC>.3F1SA6t3T>.BkWnLlw&4M<^`S@lm:T/QN$a=q(8I$^S(3MNDDmMYD7Y8r>i_6_;YmLB963)p=KC>qAI;Bk?EU-Yc<47"
    "DN[a=lkl7:Z]]6:eQa2<Kw(-<X3_^-(F=^6O+`0M4,pV8ophp9pU;L3^N=Z@9E'UDl(W6C&)0X/+eO&Jf.Zd=QDaX8sT[=9dU?h<LxMt:/FOY@2LveF_A-o;$cbX8phN:8NVsp9HCiE>"
    "e#Y)5<kbPCM3BWAkcN&7Jv$W8jeW^6A5cMCPY_D.M`[?At//-<Otu+39Y@5C^(3?6%q(p9TW:5#0H>5CgJVS%abRA61)Eg4ohn&6)=6b-38?98srb61PwjU80<ds8-9ZW8%H299+q,W8"
    "ZHAf=oa[*=UG`o%0qYZ.poOQ1E$l89vn%a5i>T#6Ef9S:3E2t8&'6[79@4B6WmZS9,*hZ7`78s7X#9;-?,TV-6d#1#efp0#dZ9Z7.Qr5:G9n3+b,M5:?5+R:`6h;.k'3X:LO.&6/*D2:"
    "73Hp8mT+mL9XP-M@FD=-s,TV-pn,wp6'kY7E?m89lt[A6#l&N:;<>b52k0Z73vDx7>L=^6n3P<8bCN^6-4b#7^qnoBZ3(?749)99@kN;AQj:s7tv%v2(+f&6@Go2;&W0n;dw8WAu&pgu"
    "sW;,<v)o`=d+iC$a6wlLGx29/0c6HN?#%Y8Bl&Z-&+f:.n_sX..B.793&]6:Kq*_?dk%##&2>>#$ck&#:%vu#,.CB#>,>>#oU]U))+U^#'5YY#dkVS%a(###5.Is$Ew/6&UjlN'f]Rh("
    "vO9+*0CvC+@6]],P)Cv-ar)9/qefQ0+XLk1;K3.3K>pF4[1V`5l$=#7&n#<86a`T9FSFn:VF-1<g9jI=w,Pc>1v6&@Ais>AD:D-#^<rTCn/XnD(#?1F8l%JGH_bcHXQH&JiD/?K#8lWL"
    "3+RqMCt84OSguLPdY[fQtLB)S.@)BTM_`ZUN&LtV_o27XoboOY)UUiZ9H<,]I;#E^Y.`^_jwEw`$k,:bqebRcDPOldTC6/fe6sGgL'Yah/s?$jec&=kOXcUlVtrM0lGb;dg)[0#l[&*#"
    "#/nK#d<lu#`Ano#%'Q7$24^.$Wj.1$[HR<$,*+E$6U&j,0[/U-AZF>-V5:?-r.[@-/fWA-;@KB-Q%%H-2/aL-c`SM-)')W-96oX-XAUZ-G>Sa-O]f..p6Ms-3*ft-AZXu-Z`6w-0qP$."
    "/)uB.&*o9.97io.&*+q.<Ztq.L5hr.i@Nt.3L5v.Z<K'/%n>(/8ZM)/gF1,/0F[-/D-b./_pp///Y]M/Aqu;/h1C?/9V$@/7*Yuu>VR2.g+PwL^3?u#1mT%Mc5O'Md:RtLY<./$%[C;$"
    "5<rC$4Osi,*U&U-80]=-X)`1.5Cj.NHjUG-Vh(P-/o(P-NC`1.dfcoNE/;hLDVOu-X/`1.9;fQMi>98S57rhNlE2)/reOrNT6%&M_Mm0/HpSfN-:-OC[W/Y#]1`:m70Uc)AOKcM3eOf("
    "lfqbVHa1G)DHPf(3Bo[k/*8%kq>o@as&l^o<LD^4VLgLSu;>%NH,=M&';'##uAj[aGuB.T@5n+:NkU(2.oW@E&==V,35[L]o<ULSe4*uOTEttX[d)'#unbUGFv'@'dI8bks//dkf4P.k"
    "'NJ-kk)j,k_Z2,kF[^*k*]3)k>t.%k#RM$ki^KEM_t]cj*Z-vj=d,8#].qmjOVRkj+gfhj_[Tej<I9ejj^C7#AW4?-AXjfLu'^fLiZjfL<7)p.UR?AFGqco*<%t7Mq2txFH?;0$K$v+#"
    "&6Vc#@If>-x`,t$X,Ah$u8-_#oW+Z-3r;(&$'BQAtQD4#/Yjh#VX#3#=N<1#=lj1#kv%5#-0`$#[35##:F^2#`5s<#H_,3#Z9>##I)a<#m&/5#d;&=#hL;4#E7I8#D?S5#>$S-#n*]-#"
    "T1f-#b=R8#kIe8#kUw8#t7o-#sb39#OE]5#&xV<#60k9##+b9#xX)<#EaW1#LH4.#QaX.#Sgb.#3nk.#Zst.#_/:/#c5C/#gG_/#1Oh/#mSq/#rl?0#X%Y6#D#R0#x([0#<`2<##<1R#"
    "v$2G#0Su>#3Vl##w+GY#$1DG#'4;,#%2>>#8lC?#15G>#,IiG#0U%H#4b7H#>(`?#B4r?#7e.-#mcoX#<$]H#@0oH#D<+I#HH=I#LTOI#PabI#TmtI#Wpk.#L@9U#F@.@#]/CJ#a;UJ#"
    "VG7@#XFhJ#ZSI@#P_[@#sw,;#wmn@#aR$K#cx*A#]-=A#of?K#srQK#w(eK#`04&#1Z4Q#eEbA#%5wK#iQtA#m^0B#(8n0#rlo=#D?dV#qjBB#uvTB#-MEL#1YWL#5fjL#9r&M##-hB#"
    "'9$C#=(9M#A4KM#D7B2#d@-C#-K?C#::Q;#H>K2#3t#;#1UK:#7np:#2hg:#6pbm#O/3)#<vO6#1-T*#8j=6#l5n0#o]aL#no>3#g9g*#WrM<#7_V4#U5g*#VSYb#KPGF#meub#e=Cn#"
    "2AnY@mLl7:Y29f3?\?RL2/Ll31uX/q/efHW.Tsb>-D*&&,47?c*$DXI)jPr0(Y^5n&IkNT%9xh;$)/,##o%Hs*4<p8+<sFY-A7mS1HS`D+HDsW[N+pgq]/if(>I1N($k4?-h,PZ-9Ztx'"
    "S[tx'N:=x'J5i5#Qp_?.2-De?F_DM9#;.,)D.9U[2fb$/5f@[[Fc=OOcdM3EPPA_[DX=wA_5aU/fer<-:JXE2l2h']m,H6&2_BQC1``'#msXgL0_YgLTSHt.7fMM9D6Yb%7@K99gF;##"
    "<=k31mAaW[i)03MfGI_['NdpL_lGp85In3+N$V4+kqN%bv1&>-bO6iMqp.S[Div'&U-_/%MbqGM?IB?-4(k8B*8&J-LgNUO#]f$#saIq&qVW<B6pe2$7$n31R0-p.RFJoLYtFsLs3GQD"
    "sH<p.i1DI$pac2$l&QND`]`w'J?_JD622X-B6h34Nb.kXED5B?RX,r+pU,F%O_6^(t&,nLGFM<J[j?AO`]FF-gp1EO[aUV[NlcfO31[V[^m^]?qXtL1?5c@9TwbwA[%a69)4m31CtBO1"
    "j;xiL8o%;%uS*3E51^59pF:kXn?QF-#*sQMU''VM@q;q/9Lf59_N6dOu`*q/7D=6CQhX<BLm`qLEZ;Z[Bu%+%JL[%@^Rdl.eA(w8D%)IMcQW2M9[7p.$^5g7B5'a-Q#c8.]7gY8,ql59"
    "],qd+Ikv`N;.x2N<.v<-puYW-wmT*@339nL]9#N-WUg5#tC6jL;xd1M?DZ-5IjK-5nhY-?&#MJ1CZXC?n$CX[+0o9.o54=#XREB.W7s315L)=-@f@U.TFldrU*MY63C+tALS^?.uFB%B"
    "sMx2vIAw>-=e2(]`<K1Gl#6eHJaQ][A,2i15O(V[Ur]][RmII#9M1kLLW.p.SiNA7uh4X?)9sc<5:1'#$]:Z[OsZgL5GjC?*SEjLkMP0N-fipLPq&D?XP;Z[*]h`?.]^)>*jG<-5MYdM"
    "=j@iLJAv2vM*woLJZC]$=gkC?<n:$pG.P_&[?d[--5kB.bb+h74>n31rnR3=<I^01lg(h+EPi22ZiIm#@ir=-P)8Y-*sw3+ebG4+#ZQqAl$l31.+,##>/BU[121i#,#b**ttWe50tIOT"
    "LC4nM,il>)24v]+#u,=Ml4k]+f-ST%T,d<.@&QY[jBl31],WEOGRdof2sLk0u95B?\?[g]?kGtT[dKJJ1g7'A.sS_D?Wn'W[kY'D?Rl.a+`_fZ$`C9N0,F>F#2GGQ1XF;##;m-B#D3?g7"
    "Z6W%@E_f$#vZ=1>1SV>#a0>?8*WBjLv/ES_.gES747(^#S/BU)_nX>-*I9X:G*p92TV):2^j0h7('dTMKFPGM?Ii9.@V2##eq]b.3@1B#Ab=n#AZO2(LMwr[XYx'.=s[fL;OlcMwZsMM"
    "b2(@-2@;=-9I;=-[G;=-w/TV-FFR?pJN^W$`e_v%)uA2'u8A#Mx6kvLsl2R#-c:?#9<]?#S`>r[VBnQWF(l2_`#i,&aQ+ZMhv-tLT.6^?(me&#YXToLO<n&/4ENY]=r*O4COAqD_V#1#"
    "VBkW[DMZP8R+YfL[5#A@%Al2(-=oXdr/@3p>@FV.q%Im#Eu+9)v#Ga,BoY@,C;XuH5SN5*6>B(+@+2WFI9muP.WPf#kZ3h%HDBA,,taF*2-6P*D&)uKnLrhF,qg4*B&;uK.R_R)QFp/1"
    "fECnD$QOiCLjLxPZDBnDt'OR)gR't<o0`g[#H5k$dAAJCGKEM+rS6r-a6T;-]6T;-Z6T;-V6T;-T6T;-R6T;-P6T;-N6T;-L6T;-J6T;-H6T;-.^rS%iv'je>6tieb3mcelh+T#k2+5J"
    "#+OYZL^T`XY3-;[,der[8dBn'[>*VN1vNYL1#buL1t@<_0-9Q)bso-$MbET(^go-$HO<T(YZo-$d:[[0Mo$[0/#p]%<O]M*QBo-$>IfM*M6o-$L0o-$xt4<LRxe-$,Ubw,H6T;-O2I20"
    ".8pR?P1iT(*xm-$3#vA@2D;pLNWjc>.&doL:%1a>dZ`BBZkroAQdJ,3%<^;-]]rS%x*Ti)FXue)_PNb3:B/i)*5.r%J9uG2*[`h)=f;11`(4n0<q1n'M3xT.GZg;.,(>m'fJ+;-dG+;-"
    "/G85'&,s4'sfV4'jJ;4'`&Zn&Va>n&ME#n&D*^m&;eAm&2I&m&,;487vo.kKJ^t<$7e<D<pCQD<-:_x5>=,[51AN>5-&nx4#1iL3T98S[wVt+;(im>):s?^4G4)W3ZO5h%XZd;3S(^,&"
    ".6nx[cNPf?AJaW[hs#R#,l*N$>JcfLPd<R8?v+8_4?Rh,L;(V[55FS_P]r.L-.]+NaAp`?\?4(gIvjQ][fxX][e%DT.*@N,#I>58.GO=GHG9vj#UY+Q>R&ojDNS%hMJhji0R37m0CdPW-"
    ",S=15ej.m0Ov8:)70*2>::9$HECOh#`#&iLECRm/(*Ls*OpLe=63([-K=;'oD@k=8Q7'&+HR@f$q0/#G(l'58$Dac)M=Q5/@9d,EaOnRnr+TV-@-fe$+%(^uv7Q;#;X^-6bJX*v:.O9#"
    "u`o9vlhYs-$Z(EMkUU;-[J=0M:oDfU_J2,).F=YcfY0C/'aF&>puu'&..(B#Lu`uL<Dc1#rDluuR%u2vv-85#6Ywm/pP`0v$I+2$j2k(Oldr/#.d8;/?.>>#6warLKCMJ#rYwm/qh@H#"
    "?qvC#fgG<-f6T;-@:eM0^krO#Xo,V#3GKsL0p4T#fgsjL6'Fng,O[+`KY);?:IR`<>e8]bbLH;$G&PlSl)()3THh`*-%6P]A?kf1[aMP8a[,`slF#3_Wj3Yu@l,M^X[PQ^F1=ip#AL;$"
    "58Uw0Ct//1U#bf:OF[>dPUX.q;'<s%pDvA,8hTq)b?`iB$q-MgjU*j'(3EYPiCw=cd2Dip=@&9AONNi^[exdd3iFcMnAvq9+)OP8uKZ%t9[oxb(Ph;$d)42^c)oxt+P];6Dv72gTGrlo"
    "=g:#c@UJpIKbxGV[J(t$JC.?GHhFpn/+XE30=`#uLBUNKjH2_+T[7$GbmJq.<_@3^WL^6AMIL_+_iRb346g6SC/Lk^lE)4Bi]<Xd]=Stm.*B7/m),Fjxg04TaH*iC]InI;U<PFa+.P4K"
    "Kv(;%=QY7S_hu7Aa5XlBWkU1q>10Yma1B]cM2#)OXxpxYLUjA#btno]hFS]lu@#K)(mT885mQ>H@ZncWI<olgT$^#$cq5d3h4:pApi:#Qw>vf`.?slp7ewA,@L=g;fo;sRc4,QK(ipSo"
    "j/,98];7?He`^gM2M0Bc-Lx#Qa#A6KWi:b+P'bW[4&vKD#';R09]nQ^fXe0Di13U]DQ>LD>U#:SY7S[QB'.(>sL$+X9a9L#s:c>3hou_3w(1c3MoGg3#x,o3(W8>4EpV54UIt:4hLSQ4"
    "q'-s40+.p4:Ob#5'KqG5)*:G50@2L5;c<Q5B#5V5NH?[5G<4mL=R:n>4v>MM?p;S^O>N;/GGx%H8ATfa*TYA6J0rrSKpH#.'j&g<.?bSK@+p%vs(C2VN%JJsTjY5CmBlct#C(jiBJ=B$"
    "LBU)PY+>m_`M^>nddON)oKcs8*(b8gsb@pp-jaj;36+<JDsVs]MU0QhB8]E#HZ832ZcFdk^38q'kwJ?7o7OKEu`49T&6p&d3,B0<@v^TKGKCBZP3`gjXUQw%]lhH4jleND+f;0WT?igj"
    "`'Wt&x]qM#[/V6:K%j=:#Q,O:Gf:);#5fm:=2ds:d(-%;od//;@k?5;_1A:;Pa3>;mA@a;JTPK;QkHP;X+AU;`A9Z;gW1`;oq)e;u.xi;&Epn;)CCs;.`Dx;wO..<^GT;<cWC@<ik2E<"
    "C,^N<77Su<s.=f<dFgr<S'cv<Jl,@=ZMN6=DeF;=L.Z[=FaCD=Iw;I=Mk>w=H#r`=O9je=VObj=(=t'>rPl,>#hd1>RKh8>u^+?>%r#D>0J@I>1@4R>6Y>s>;ah[><eDa>A+Ff>t6-h>"
    "e<;t>$Lxu>?=@*?[I',?ARb3?ODL<?#R;A?\?BQH?\?WSf?9BUW?[cdl?_EF7@gR#w?niq%@teSN@VR1H@P%sa@a%wp@MCT$AjnA7A7]W>A%/PgAGC<YAfguoAj(u$B$:<:BGHYOB^omTB"
    "g/fYBqZ5%CqQImBNlHxB*3v%C`T(1Cl%ABC_dJRCLrdXC61[aC)@smCiPNBD-TYWDO/rPD+w1XDK,oYDP<^_DrP`&EQ6_wD`I4CE2s<3E%gY@E%fVIE)[b$F[qHaEQ+M=FR;[.FE*+ZF"
    "AG[mFqwGhFcxJ$G9$R*G>4A/G3=Q5GQ>UDGw&cKG_*$tG^^=gG#DR6HOv%NHE=F?H?.mhH(d1XHX$U[H>wc-Iiel%I3Lu8I3EqDIkgOKIr'HPI&MRUI2m]ZI6q9`Ig^X,J;HpKJ>OB8J"
    "Q4r=JfuSCJ3G<JJPmXkJ4oNRJu/sUJcX[YJY>a^JUB=cJT4TgJVGU1K`Z;qJg-OvJm1,%K#T6*K'q7/K7LT4KWRe:Kn[u@K05<FKPMqhKfG]RKC>&ZK&THaK_RGdKBZb,Ld-k;M+-=CM"
    "c.NbMD$2IMCw/OM-HBWMqm&kMbf^@NV,)2N.:l<Na9>DN)utON68FcN`)<+O62x/O9<^4Of:mXOLCFKOO@oROVq6UO-KTWO[+&ZO'@(xOFBI^OgM0`O1YmaO%a66PJJ7+P.X4WPJ,iEP"
    "SoNZP%u^dP,DnuPKYl-Qq@IBQ>J0DQ7leJQvrq]Qr8NjQsFe#RB5%+RIW?QR?>e>RhDsJREjV^ReVD#S+WJ,SAn@7S/Y*CSh<VQSF(%+TA)uwS+Tb4T)oc9T3;n>T6wrBTsOeFT'v+hT"
    "4?$QT8I`UTAljZTK2l`TRHdeT`twjTh7#pTv`6uT'w.$UnIn'Uvi+HUow30Ui=55Uegt8Uc-v=USV_AUK`mMU<hRRU4/*VU$XiYUubN_UwoF)V)EQiU2I.nUsn?#V_/8(VkTB-Vl[(2V"
    "trv6VeE`:V[c6>V'U3cVPcfKVY/qPV[elTVX+nYVcMx_Vos,eV#7.jV*M&oVDJ$uV=[>xVtCUAW;W;+W[cx,WxdR7Wb*T<WL?VYW5K2FW0=IJWUXuPW?#9%Xmr#iWP)fvWXB^%X]OL*X"
    "dfD/Xo8X4Xp`>AXk7bOXHDHQX4e1qXOK1iXTGXsXuF+%YLe4-YiD01Yw7-UY*N/sYa$e^YdA@nY`M'pY*YdqYJeJsYQ,K%Z=BI3Z1_H>ZRuGIZsOvmZ]%0dZ8,gwZ0.C&[TE<([#^5*["
    "Gu.,[l6(.[Mjv:[?q`N[#;bS[,ZlX[=?E_[JbOd[H(Qi[>?ul[H_vq[V4=w[bVG&]T0:*]ASp-]Ls-N]LnD7]U4F<]dccA]o/nF])e=L]t@0P]&Z1U]53NZ]4c@_]#$eb]vQ+h]f('1^"
    "eHJo]]L't]wSa(^*qb-^:L)3^8#r6^3-W;^9IX@^@`PE^OG<g^XToO^dpDV^*%+[^l/<_^J'Zi^%a_m^S$4w^0;,&_=mZF_tS/@_wj'E_m>kW_Q`R$`9g`3`+%###R3n0#OFH##+>uu#"
    "*8###BctM(Fn@X-D5+V-$FCo7$df=#6W9@Mj^_sL((X[$=I*X%:kT`3/Dvu#;YC;$9E?,&E@<p%-uZS%fVjp%$r__%H]Fe#19<I3],%12Q',)#lOw20wgRL2l^fw#$kJe$>W)pJ+5&cu"
    "$,u?#('$c$8HfPu#'?3ucoc/vVc)%$nQQ&M.3)'#`mE>#5iC?#3h]e%)oMg*wCU?#>R#.)X*MQAB?4w#,v16&eD###2G#i79.gC4gk,tLJS,,2f9[1244;t@<e1g%W<a%vq*.W$hsn%#"
    "hlLY7/.c@#1VuI%@tU;$iI^J)i>U>$m.BB+as57&x1XI)u3)@,lrw1(qxsA+n8Cu$*o>c*S-j$#lfw1(PU`Z#Xg57&e[TJ)CksT%Oh/m&bI;Z#TNBu$K&e`*;F[<$O[/m&QjD_Xhf3.)"
    "^5wD*T/6Y$m)1Y$;0Z('WW9#$T*'6$7CN5&;xh;$]S8](CdEA+;4pS]R=Q1MgMn51.Sxea9v$9.&PKF*k)Qn&`Tgm&cAM8&7W9E<H3g6&@nq;&Zl/Qf;x_Z#Xx%9.58jB4`#nD*U*G/("
    "TTZ`*II>n*v%^t@eFH12B)c?6)mxGGt$'@60kRA6ux^t@'o4G6Eq/%'`QGHNTrJ6KXB0u1:Jp;-Dqaf18Yfo@(LV_46uT/)SV51M.Bx9.:NAX-(64:.7f0H2kVxb4oWfhrC^qE#X`AF:"
    "fJ1-Ob$(tu#TO:m6deau5X;M#AT0.j+9AY#H2odum,.m/b5+uu[IMO#h`rS%v:i?u&5>##4W;[$b2L'##9q'#5)_V_%O4onW+?C+/TwL(h(d',1bp-%qeRs$([=A#W^.9,O&v2vVG=A,"
    "uDWv5dR?(+Nj$>H;O*P^(JJx$JmG,2Dv16&XcIC#-&8N0[U;)/'3Q/(_qN%bCwKQ&Wkj@#EYe.29I.L(m*,e<cJV$$vhr)1p)=mLB)C0MIRGgLmUdX7dk2iM3o(9.ZVeN'AOGb%,E6R&"
    "8X<R<xW9tUDB8Q(rdb*I*,+m'ZWW/MZPod)O0v2(j[3=./+fw'-9HN'v:Jm0C4.F%1=w0#v@pC4.&#1#UPk*&asu/&,?Ab.kcrN'^u:T.#oDB4.Z2E-ts@_5t5&:*>`B0MGg8/-%;^t@"
    "QR<E2tG]02YoDpLra26psW)228WTA6@[.kLn@Sh(vQYt]3k8g1Fb.87n>vC3p<ae*auHhMSd^0KLwT.2Z:Y@5HRYBGY[5h1b%F3KI0/(#*.aV$RqUN(jU'f)NCI8%jm?l13?w,*:pLh1"
    "p:gF4-a0i)ZeIs$iL0+*e5xF4/]NT/@ik=PLCoC#MIC@Bt5d)R<Q6=:2FPv(q^KSY#aq6R<DH(W^(R=7HajX8%:1*6hm#]u(+6#J'_BHJ0UHw`(%$KPs#ZE?5k4-5aL`B]<x.tB;x1j)"
    "NMHt7shG0)/X6I<JAP##CUZK%D-O9#74i$#t>$(#-R.c7'<cY#2UD##=>>##_i%O'KF2##xJLV]W#ZU]n@2v#>2G>#8.7##eul`<[IVS%T;7I2ri8#Q6*Kv-xxlW7Okk70TOMf#+n_U&"
    "g`aP&LZ:N$+#1f#IkS^3Nd[GMsFSh(rZ4N3X/,H330O>7u;>5p_942M8SPHN_wZHNrNWL2&U;8.pC#G4_R(f)-<Tv-S*@K)a^D.3O29f3aGUv-@D#O,F[uS7xXYDgN_pr^X#=Ehkjuj?"
    "*KSE+tfHo/ta;.3qKoS%kq<W_XeB*K%xn3+`C*.-)&kigLvTJU#hVWgeV'*CK=.05xFPrJvD@W.-ST7Dn&BN[nv,t^N^w<(D(j#K3ki&5k>g1T1xjVdwDoo.8lNY5ud^v.9%YfCk[>g)"
    "]9nB5(lRC#:[8Q&jUfi'YW5>$]H_j'0E9@#@#jIq[)I3'+jtr-:B-?$NH$h2S`Z40R=TA#YeEa,_MQk,&bmc*^ii.L;9D22^)0c3[b&m(Cc8a3/6H124A4l(Z6wGGnt+@5dQjl(=%rP/"
    "@w-87%xD8pE#uM(1Pct6WgSm2r.CJ#;Cn8%o/_:%Gc*w$t)t>-<;gF4)jOjL*'Nb$x&e)*R`@A4>U^:/.m@d)n=+gLQLH<*;_#;/-Vor6.j;6T.FOR6WR.4EMj$`jn1sUa<XU)rZnR6C"
    "N<=,-`)ob>7pN/<P6*SKV:$uL9KaB?Qpvq(X+=&>)FK21$^OjkVEpu:'v<=NOx?G>*evD#'I/U1cU?k2mtv#6cr0bG2R&##'?%)$`4]p$8CP##N>d=$-P:v#Tu_n<%77Z#iMka$.quf$"
    "dA]t@6LAD5piI12dQSX-`dK#$PHFd7VUb^$RAeFin7Yw9ZVxeuI,Km$YtC$#JN_oS1=%Z#`Vk*$4k:(&PWO&#RHW5]7:@W$UbO?#d(XGujb+Du3/i>78Yfo@bm)`#UD@1(^UmY#J69@#"
    "ZHTh5Zv(]b?UoiguIxKgFlv_uUL@@#*]FY#A+>>#*HJF-;E?=0#,.w#VA6`agxMwKGMb(sxvYq)Xnn&#oH5u-S08<$`++I)ox+Z,mMZ7&dL;;$hqN%bkULF*`X@?#eJ7h((kLK(/L$K)"
    ":rl>#rbuc*ldp6&sa.],XL<5&k?e],kPm.LT'x+2lEI12hx^t@X/JO(Sf+p7b'-k(f[Zs0piI12_](p.AZNP(q0q^f7t]s$#P,G4+BshL-L_F*9&;9/_Iw8%<bOU%@8WT/Ii^F*7@t=c"
    ",,`,)@bM-*dhl^08A`JDVU5U.]C#R_tbLk13?)eOSuZX$vwbm4EW*E*dZcQ-)&2X.lF`L,q(?FNmkT[-xXukgJt?(>)8Do%-?^+`C3juubg9D3#pL3&1]lY#8Kd,&0P-k&;IRs$6tFm#"
    "QGw6*)<C?6j]@122*MfL43MT.dQSX-Aejf1$/nK#=+egu_=:nuU%QTM1cfM`$/[iKK2QlJc_7,2n<85&Y*Jd+T]J#,Bcng(=CRs$A1VZ#t@pm&$HEQ&k:vi0OKLW5q3H12X_,87FT-@5"
    "ZliM)8bCE40T<pJ-uiv_Shr`$3JgiucE(X=l7aU:m>gP1s]-2K2_%v#,<%<&/_%Z#jtf&$tE'CG*0_Bu3arZ-KR5Fu4d4mu1o+O/L6YY#oS]vL.6=V79RF2$kg/.$00?tL&JbcM)GUpM"
    "sOq%/_tVlSsW-XC3]3on=ukig*)#N1@4npKXR-##Hs;&+@/K>#Bw)#0lZ4N330O>7)7h5'5T7pu[&qig0qfBJGw:(v?.]fLx/5n(;Bo(<^,Rj'At-%'.Xmfh:7nw#gssT'Q=[S#D+4#5"
    ">@o4T2%/4TB8K(O0nq;&uYN5&kFLf#589;-hOvi0EG]02It.eZ5mv2A8Yfo@k'21(?oq_,hU'f)<)U)Nd.<9/qeb**J>#G4]v5<.K9;G#pEcXR`C,k)='ssumH^c:)m>SR)C]>@%6iTC"
    "e7QV#_=VR:k'PiWEt0fuAl&2=;e&luCC>P`/rsYdG+,###@uu#A.O9##mDP_qacIqK^2k'i`Y>#$lI-%6P&C4;FE5&Njc7&2cu>#%&A]kVx[fLV7I12W_,872[)22(<ku@/Rlf%3ctM("
    "GN1K(u-l%$0`[9C$-V2n8W7WtrtP^;Qnx(<XGh1#n/,E4$#gxtpE$E,Hnf6#oAwn742aB4^/1Y$_AZ3';*)w(8(nN#7T+$MxptjLC-60D[-Nd$^uWA+[PR)*+c68%gfH:77tAx#3Gd_F"
    "uX*]'Q9OA#A,T02+n_]$)Yrk(.JV#A&U<8p^xO12i:Y@53[PZ-],cq.Xbv&%dHav6K]B.*ig'u$gvLg%'2hxtD03LhbS#BC.^rqB_plk*4YYAi`siaf,.Z`#@2:87K)R+7OI&wJp[N@D"
    "V>96NDGiu8%vEj.Xq@p$F%b//:v/b__@u^]3`u^]7%>gLtjo21<^Aa+nqN%b/R&C4&$DY7BpMB4DIF^-6b3e-Q.>j-]PHo-2B/a+&dB=ICBL6T^F,c*XgD<)YTu2vE4q<:a<u2v8i.k'"
    "Od,F%gdQgL%XZJ&b6*U.%ES]%/#1.*>X28.%0QG.5p=^,JomF%icJC4.po(O:TNp')A,j')VLf#Y%L>#aJM.2MnUw%#:%-1&KU^3'+bd(P4_t@w'$@5W.>n0MHu9)[V>8pr25C5H0=r."
    "$N@k(Pp(C&8*RL2uNro.&;[@-Vg'u$nbK+*BR3N0?]d8/V?w,*57*u7M9d;%&C]T%Xt5]k(&MC#O5+v,KLTnD@AY-==ZdPdu7ZB#1;[3<Qkt'@4)-u-Bouk<f@qD$M716cAi=WTgM@M1"
    "4VY^o@]&9OWhI^Iw*Zl*^E)gCT9=(?OODM0Sq7j0[P:5&S2:SIRdH3'nI6v#>P1v#)C^B'LHT6&Kfsl&Imhg(Fr?8%=@%<$4Z'$G`;gZ#QqN%bf3S],uN&<Q>u]/q[JV8&f5T@5'4Ka#"
    "*:biKX34,2X%dI38H]s0aD0V6Hm(C&0/i>7TW.@5g:aBup$9g15^H<(J9JC#.0;hL+vcP34t@X-Ix(v#M$O]#'T79_x'mjuZG7@#:pO.ubh$oudaYF##YQ(#heJ&vq07W$UGX&#BY0:/"
    "NevY#d@jD%R7Us-6iBF*WqLG)VZNV748$D,j-q]k:=w8%:-h^'0)Q3#Jau2v2?$q&UkYS7%VE`#JQ_t%FdK^FIVOV-Li0s-KF^:)vCJ/jA:bF3OkG,k$AV'#8^MDubBD,ME_,@5dukl("
    "U1+kb</UX-=DwA4j@A(#R:M;$C]R_#AgqT%GYQp.;&rRu,1^jMTfsf(#?O7eq/9,@C[G$(MLMfFd=nc4x3c@XHCiF`?AG]us07iuMB^+FP@FnBfOL9M]Ag_4W/5##S)2o%+6jigh6el/"
    "nqNp9Fv$8+?#CQSChAQ&5mNS7p@*D#'aj/Mv7M9/HTC<-6su:%^@u'+;dnPfZXg%O%#G)O8H3T''5gM'xRLf#IdK40N@]V%B9W6p&k@0<d]d)MA_$-1tpcn0f4)=-bU2M/Bc6<./x^DN"
    "AaSs$9vL^#f(PV--I]@#*,a+&4C^ip>q[-;sY[r/*3]ZC,Z;'OU#Rqu_#')<s=GJeUI%YIRA:.GCTTG$b5FV9]+i);*5GDic-(BS(8ri'$'4>Gb2YXul8[&JKPM0#*^$8@(AP##Ev52'"
    "lA?Z.xCQ3#q'gRJoi#[#M=(x2Qj1k'KPkuP9kxi'NIE2';S]BfR`^t@16rAuvUt,#<[[.:i)?v$=R#30`@9j(pZQ;@&keWD9jUM)6%m9f2YD[$WX&<P?9;'a4%(,DwHpc);o'wI-,-&#"
    ",dZ(##`,t$0fgG-kixi'R5v2vU(66&Ax@T%&-&O(%K?5THQ8S[+1u'lWf]G3pniL3EFZS3aQpU/p_L(+Hb/9%;S^F*<Qv2vcp7lL<eGj+vpxi'f=cjLW=7Q0wVckk+JqZ#H=i*O@#Km("
    "/f(g(3`Lf#Z-7S$VniM0oZCc+_&><-C<,D%tEUh('@]98AiVs@.$En0O5-E2IDFJ#w^v)4M2'J3qcWI)1sC-*qYVO';]d8/gcYD4<:I<-*ClH&+5GJ:D0U]Z[w=5/J<CS0%5cVCN8X&o"
    "j02K#LCgU9p@Gn:Y3kS;NR*p;M@cG)]3r+G7;*,lsDX]7sEHnG@?hn2oV=XLvknO;sL(O;sn-S73D#$B$pl/#m'p&vQ49P8>.o:JrafB/@/_mSH?u/('4>Y%UI?>#SsXb^Vq1gLnxJj2"
    "Y1i:8/pI*+KH5:%bZA>,9XegW:TN-*B[PI8,1+Q'gL/B-K2v2AtBO;pvUt,#'mm;8tZ_s-:)Um2LXv,*+2pb4G$Ts$Hjq%4L/'J3'6@L(wsAv-hKAG;EqDe-kA>lClJ#I;rV[r/+3]ZC"
    "e_jZOV2nU#)KsO]vK2/eD+jiIP5lhF@6OJ#;8FV9^7.E;YGhhhe6C^Sw.A+r$7d:Qm(@v#mAwAJJPM0#BJ(pJ,ZZp.'k''#-Mc##SAZx#gA@>#90VVZDF.s$eAw8%xq__%DVFe#x_f<]"
    "#^DQ7J'J*[Dn@X-]RZJ:a(JT]D)T5/+'<tu.%Q%.di[cMQ:N$#L%V,NfjQN'OWVVZYZ,3'Bnf2'1drA.Y2V0(24@m'85x>/esLW5>[#m:QsAuC)=WW-N]$@7A+GG-_:tv%3lET.XA6G$"
    "NBg*#uSZ^7elh;$1X%Z#?PuY#57Rs$=5LG)&;cY#GrfW65)no[.YUV$9f_V$1%@s$)h7-#xWa`<U*+,2AQPh#>;1m((u#&+&v,f*^l^6pd9$@56#[HNqC0u1ns(]$'3QJ(4'3D#:`K:Q"
    "9(V(<JB@J#'#UB#B(mju</`*Mu^fK#3<wO$F6>##6-sV7()U0Gd31N$Vb9mAM`Yv#cTb,238o&#MxO127?TT.;T<dt<BeM0Pt=qX4bDuuhIwqL$T'(MfAU*#::^*#5mc5/o0g*#ls53>"
    "uY6(#=(HX?O1pQANNuN'1'9=$Px/,0N&v2vkv#+*bCRm/).jB4t6-j(?Urt-^LQ1Mj-3K*s:e(#pbfP'A#Y,MH#XfMRRBJV=8</29$vC32gGO((Wio06;EDu0'@12AAgKYB3fu@Q:ZV-"
    "ND#G4[h2^%IFQV-fOM$@Cc'b$$9L#$9#6=1P$Hx7mK+<'_tM@#&l)+jPL@aQ4K)SI$IZ(E#4)6BmJPB8-7N;AKu1U#xZP&f/0cl<&QvLTvVs[t/Qdip;*mr6,L@f_l*E%v0frN'&eu2v"
    "&R'I)HN,F%pETqLObi<'AJAC4j>=%((64U.ETcS9<%^l:?D7m:`X@x68Y[LQ>HCY[*B^`5WxD?$eJe`*X#6u$;.Xh))&)]6G5qe*@fe4]ZpMt6OpU&$HmSC#T]3S&:hM;]kvm`4$$o]4"
    "5xR..Gh$gLVdH12r8c5/dNZh(p$-GM#.<E22u%B7GpDO+@4Oh#dlSL2`G]s0O*bi0Gcdtf;qUE4I$b$#o_)Z#j(PV-9T/J3:+r;$9vL^#nsFU.2tGJ(F4[s$a%NT/4f&gLvraV3e7#`4"
    "f[K+*&NOl(+87<.To:Z#-t[H$_,F_5e(hLgU2rRe8bbI#(A-v#ZrGo:mItXXk0L`fU#LQrApI]O,/QVCV)>R8n&Ina0(F-F?u2M0M=aY7imLc5vj1995r74omf<I)d8U%W`Z')3Qx8cf"
    "h&7`Nxxrg4^m3U#E7+Y#]Tq=GL1/nuq.B*5aUKFkwWlxO'beIu*+aj2ZfrmL`3cU8Um*##[O`S%#$Fs6?\?=8%.LPuY9W&@%[DK?#wKMG#jpLC&6%78%>.D?#57RW$gFoJLH6'u$;F<p%"
    "VNa-$A7;v#>Bu2vn$55(%w8<$nCL>#6rCZ#AFi;$9L(&O.>P>#/hWD<fP>/$bU1v#Vn)L5AKd#%3uUv#;4j%&NN3p%QZ4N3P&^t@`j)22.w[@5QjlA6mbRA6I<f;-ctG1%U2[)3gh'P6"
    "gVq(c8Yfo@8.OZ-$%TfLt^qV&Ct[@uB`d(3<^$MprB7P/WhFwq&nSTtGWZ_#x#j<#i,0PSa=VYlKV.,)eqho.jkj5:M-oX'j(N)%#a$M)^vUG):^Jd<V&v#$idYv.6>b]F5VOg17#5B,"
    ")J$v#LDjh(tn4^&n6G4KuaC5K%sT^F+6VlK0&1FE(]`BuRwV6puw?i9tgVs@R]x2Af:Kk$)m#l9#9Hv$%;#s-_3&,;E:^;.w3JT%2(p`<dOuI4*De8EqC*>TF08X#q<mh=9<'Mn%d-BI"
    "GXiag2fc&7_:E6PM`8]-puQPJ:Wi5:e@8V9_h%##0p[)v^rOq$k/4&#bn3eNikvZ#C7%@#?GLXQe<FG20W]Y#&A6YQt'T3Kjfm1K`vN]Fp]5'&[6E4<&H/q`wVfl8YhbZ.b&$N'QhRF%"
    ".meD*jkIN:Yj>g)1:rZ^*?wfC1(2?Z-lTsuVfv+MGekc)XP7bu;FT7/n#WQ=TNitL%vQ]ITdTiO?4SJ(o44DN4ZuY8+uPrJEHkK^tKgHQCC[3#eWVo$@tC$#N^fW71dK2#)1XA'dqv>#"
    ")N[&+T[&j'*R`4'S]n$:OA,h2e=K,3w:Xe)pNPm:1-;hLxenF4+87<.+TY0dn.nKE_8P)JGAv=c3cQ#9,?rf_N##crM.oXur<R?$XDxO7^vfSZ$),##.mUH-tiOH6mZ9]7pwO/(ja[VQ"
    "<Ze@%iK>##Lw1T7[;,<$=auS.;U<T%vgG93/jvg(3l-s$_sJ^F[1AjKt*u,MuZ/V69^aI33o)O)sjvGG(O[s9+=aP(bb&sRAKXo%$F1S7]qiI1TZY:d6%kpu:Z*?Mf&73#MJr*M+sv##"
    "w`3:SPfHN%;_Wp%Y<ws%Bcu>#3kTwKbwsp%+vf?K0hbd(=T;%K`CsM0terGunmZCuQkYlL2?A4vN+kTuE>N#0p$]5%/,O9#x@Vo%LvG&#IIZ##XjDB4b=L>#&5OI)4Xs<$@Td,)`'`D+"
    "0g..)>GP8.]jQA+,ki8&o*ss-ZeMiMKjH12`iYW-$Ix<(nj`i2TLb?K,5U8.I&.&4,9+G4`[C+*fn0B>m3<aE*)TF4``;8IUhOouj,7xtV=PmcjoHFr1G?^up^[<Cjq3ZVUE3d#g>qBu"
    "RcC?]ZvpFq+QMs9hn+8<*OR8l@x]*4Y*,##mZqr$K?l6#MJuX7F7*mF?Xjp%5Juu#K*4=$h<k2'E/T>#u./c&.5N)#ptnA6?ucau>=AZ$)(#pS0DEip$(BY5nlr:66OD:vh.#i9$>kR["
    "1`O_.#7YY#.dU_%n.-5/fMmf$O@5##=UR[#6E_>#Xr%jKKpCv#<^<n0H)HW>u?d0)uc+&Y$.^u5$_)i9/T%<.xV,)a5NC2)Dfp2)Sm:0(#T%%#fm3h#U(Cf#:-,-2Deh0MWbS^3BTRL2"
    "kL4I)FxrG<`Jo/1a^D.34>>`sC7Tcui&U_rh:Ro[/TAE+A84_$fWButLLggIRN<8B#B)@G[H2u$1S(REpg3$#T&voTPd&Z#+1Ro#:i.v5NJpY#;uhV$9xCv#3:n8%KXIs$?:%w#(tg>#"
    "]+8jK;s$s$AR*p%q6&t-ImKk9Co^N(d9$@50kRA6@Jvk(&^Hm#kFw,=%$)q%/Y8C46h/g1,lB%t*L8C4kG5b+eF#gLh$PT%<.%W$;I*9%5YG-4jdWZ#YZ&I$HF`;$ZfT>#;%9g1W>@2K"
    "?ku8.Krvb%=^oX%p;H#c3//(#<7ho@jK/@#uHNh#/.rJWCm%r#ib`s#wcNpujP$;6c1>+i%2TFiBd:8@^rP3#w-@;SFP+Z,=-]V$?PTm(gj*8RE[[8%E;pY#^/7W$-UNs-Yu;NA-#Q[-"
    "Bo?N#UMlqtWuoptAL]vLnPL2#Y1]fLPh<$#rqGt$H<^U%RF*p%aWRF4p<a=lU^Gb%,sxu,w2,,2McO;-,x*D#:&NS]+)rjt6hA^634b%$8[G#YG'Es6@tjp%8;LG)*AcY#7J>##6M(v#"
    "F3gm&GF*9%]u[fL;):e$Q1Cf#rdK3'03vT3nSjl(5:$@5-jlA6mtnA6A=Y@5I(jQ/Z9bi0-G9t@=WnAG(P+.)@1fs-WKkA#>Y-j#FS1s#Ww8iu`KxluBPcf$cV/.$:8lB$N81=MSe'S-"
    "9YCx$+9q4J@;u;-Ni'B%QxWT@FAP%M0`Ys-jDxlLbS*c#sqFm#CRG##Kal/(O$&<$2N6a0rk-H)=Iw8%_=ffL3+ag(1GP>#a%&/L&K_KcfN@$9vj=5p`1Ud:#U6HFG`Y)%1[/s@SDFm0"
    "=;0eZ1<9I$<4G]XOWZuYQ0n_jTaD*[Mv`6$=.P'$wFK:#-sZo@^_vG;_l62'-=?M9&;3c%8OEp%S.#j%F#G5S-PhJ_5de`$CgOI#'DH/$)r[D&#39*%a:Lf#t2@A+:`Vp.>W<8pO]*CJ"
    "+0d%Ah(PV-epq0(o5Tv-hui,<^IHa3i?^M#Ns'J#la1f_wVkF`ZLY:X75DnX:jEl#jI=I#)Bhfukw=uu2g8O#q=[XbiX+Zam25f%38(##X&pf_@B(,)Un.H+(*cQ&'sK2#Q5`k'i(O=$"
    "[Cpq%vk>7&jqw5/:i>8pu-MkLi=SX-?DXI)'I1N(6%OT%xs-JqZZ?(s_sRtu?oXrFBcAsKO0KHo)EIXuVth?@>.'6MS=tx@_]eGMg_fc)p+v7IcEb;$,1TsIWxD*,[D^-#tRE>POdc^#"
    "RG)9/Nt.<$QM`8&:e/=$5odYQ1@gU)1iUV$tvN:.OJ?8/pe`;$NN511r8jY,(jJ-<a=Rm0MxO12/Y;k(6a9D57`cu@J-#=(9]?j(lEjK:.jG,*n47T._r;9/)1?&%+d]+41ZPT.4t`a4"
    "?/[W%'BJP>%F7TKN-m8S$*Z8bD;TM#NmtI#@+mA45O&1YY^&70[cdB6adbhhK1V@,E&fH-8lBT#ZZXI#+HqfuA1m-/9_A@aimRZb8_6IWMYB6(bH5T%c.5^$Lg[%#M`A`7>H.1(1U(LL"
    "jfRL(h`e-)/V$YQdOBb*`8Sx*WQS%#2P-v#XQcf(#J5/(lU#(+,N;L(Boh5/jobS.6Glo.dPde%D9W6pc9$@5QJd>6T&GO/,CO;p@fIV8<ZtQ'xjVJ(FQEI37xNfO)F5gLbZ_F*+FI5/"
    "bPq;.UND.3j7bj$&-Q2Uvx<WkT;8d5n/u)+nxBquiYLX.%;4HaWC$C5r#ol$w4L7PTo+>.-fXi(f?m:?V`ZJ+IOO2$H4Sg:<p.+#lRYi94E5pI(/-/1U$e)5L[CK.ab,K)G6Q&>H^CZ$"
    "54EM._u%T&gsmD*]w*i*d.v@#-.d_+&$I=$elNS7xjlF#==P>#PW]e]WCQ1MV]]s&p=*D#:5gVQ5n07/ci-n7AP&C4Ai9:/BZ[s&QmH4K,Ya2Kc*KaFA-uV7XfW_475JS7-Ve(#$VI<("
    "$G1f#IkS^3Al$-1q;u5p3>9F*r4,87][7:.V?:J57.g*%Z:Y@57//(#lSmq.K$dl$'Y<9/g(<9/<L9Y.n'E/:rX=gLljb=$gCXI)q#t>-q.t7[%9tmLRS>.3/XH*L/KD8%$tw@#fv5Y>"
    "Q%LN<-1_VIOXH>#s+0on,ajhu[-'v+#qPA@Hw1O1nx9iF)H15HVB?lBjL==.aT*V7p>lJPYlA;-$),##WPUV$t@l6#2kMV7wswVQ=B:g(2YL;$i5G/Cxak&#1M0V6ZJ<H2cv@du9,E`u"
    "j9lmukf0N$^1^Z.RuR5%<%<?7T&>X7(P6?#5)P3))l%d%II[w#,ERA+Rtxf(/qIl'fmt5/30O>7'/Ws92%aw$YCh8.Gq[P/[TAX-Ql(e$U;u`<=$p_Z,-rWEtfEP+C#?xMD+ke$I@SvL"
    "cjGg2Aw:1?4<1)ELEqPN%#GwKL2###f`DwK[qZ##B1MU^hS9Z#=7Rs$^B9u$6+@s$,S1Z#YZ&I$DF`;$1JP>#V4vr$eodX.2iCv#)$667CCrV$0]u>#0KA>#h8v3#l7>##D:Uf##o-o#"
    "9#6f;K)O^G+&+22+tn1M%nQA69:WQLi2QJ(d9$@5P$(6/h/UX-GKkA#%BX(j`#/29%nN=lLllf_$TEv,eN%##$)>>#Y.SfLfcW?#;X95&jQi,4)`E`#Ak/m&Ii)d%e*KQ&Okes$<.%W$"
    "@1)?#BtA6&4flY#X2P`<%)###5t%<Ql*-ENp.;hLwuQlLjB^t@t7Cw.u$&f.m=Y@51[b,%-O@<-&xA]$qn*i]$c'^#*E28]_;tn00*RL2XaTP&h)x2)2IYu%9@nM0G9I%'](oP'MKZgL"
    "9c$E5gS4Z..wIT.jgS<$/L7D$OWtE$C7'v5eM0,;Ds-o#n-]fLd2B/L9DM@'GL#gLllMv#HJ^caThk,2_1GD47xh;$tHAT%MU<T%c5Wg1x0IH)@RNp%4JlY#$7IH)<=WP&2AcY#c5B2#"
    "C`###?$9g19jH<(P8Tg1Ma3#PAXP-Mu,RL2,G9t@e,GJ#i[FpLSt=n0_JT/)ZOQJ:1>ED#QVMV#Q.uN#6>:nug)Y:vl?u^$Ves5$:`rS%6.P'$w*o-#'5>##l',^$>h1$#(s*9/u=2sH"
    ">kAQ&8FW5&uB-L,s5p8%B$6bW6'oZ#9IaP&iBruPKqN%b`@$c*i1[8%4wT;Q2Vu>#gvmo%cXF?#(S'T74hi;QMg4<$pfvI%AxO12&_g,&)Qk39@/aqgDkY@5CEKs@E7158IxmW_.rRC#"
    "o(gxtJ?(>5po0;$5Hx[k$jj@tu$Yr6vtSu5[>]+iJ@Uxt**#O-:;3b0*5W_7fMuY#O;FD8FHp5:o.8H)3FET%4%R8%FA'<%j1ofL#QP,M<:a;$a&,b%lC%aa=Y&QALM0j(&5vj(0/i>7"
    "w,`P:hK2W%4n2bP7voc$2JZN(XlP>#r]ajL;AbWuPqclLPc/@#ft.PuT3[6M1EZ&#ju(g$<[u##uEZr/k4T&O8oLZ#C<rB8Z38N04=*9%q/+WQf^RVQFOiv#iBiuP9R&Q&/m[S7?Xjp%"
    "R<3aE^8<I3^-*kL,RYd$fU+Q:869jansx5)x1YV-=UN/1C'Txt@T^It8.LBu(jl%0LXikuse)%$THTc.kZte#YHYl41eDV7wf,3#9S@H%Q.Rs$8wJ,2ZO1@6_w=g16*x9.DNAX-^.L6r"
    "1VovLdPwqtdm$8#tV&]tF?1#5C#bi9qiD>59TL7#rUiX-,*Y-?JDqf_T*hr6wMNa3w`x:QC</Z#`5]t@Y),##M8xL$1Vx(5>doW7>Juu#3Y:;$)25##84Rs$Urg>#Uk0S-5CQl.p',)#"
    "Z*e+7%TRL2hbYV-LZ0^#Qee@#raW6b4bRh$twsT#P<hhL#2U#vrLC;$@8x-#E]:k^*M(v#+J1v#[Z#,2jHSm2;i5g)m8*a*_u'wCI$d&#TnLxL[lw&M*BbV7FqF/(./Qt1Q/###t7RcV"
    "0muC3)YX*Ii?6K1864:.%m#jWg'OpuQ15##ZwIp$vOW)#.pm(#jX95&l(i&-,EJi)kiG>#DFe'&oB%##-i2H.5;LW7>7o:/XhvE32G#i7GpMB4qCc+*%/.(#a,0#,<oJC4h^,n&NkE:."
    "TE?O'*)ce#SiK>#+Sp%5)8<vf3x.CuRWio0@w-87S6/kLbR0N(6@'*)EdfB5Yr+@5Z)RL2alxF4O/'J3CcK+*t(4I)+dF2CMIgv-?D,c4vm#50-87<.Sfu>#_Q3L##@dnAm(qDN8HGlJ"
    "iD5+5FQBTkF,L2)1Y=R*-;bL<NE0RcW=Hkm/BO/<7ZMB#CS;vJ@kov7fjq_#L+5bHdl`50-6Q=;Cn-F>=C:k1otjW[f*81<fGP^,9'F)+=`XG*ZrrP02qAWAr5YY#4(8onKSlf_SiSY,"
    "qeBW&+G2W'_f;s%(Sq._ot=T06>8N0kIb**Qvrx+9[a**Bq*i(.MtDNuaH.Mhj@dDJw.@#Ad'j(:,T02[mRlLSls,#hfe)M]J%^$dG9t@dQSX-)/'J37v3I)q;Tv-W?ng)`/gZ&&@`K#"
    "K,QEA'vl%fS4qY8KCEL#gxcR:+.nd=K:lmuQal`ub6C7[wrct1JjGk#cCe&J#nm]JonUf:&qY)*0U6;-GNa)5Sn;P(>/`S(SYB>#;?jh)gepb*0m%@#B2SY,iGH:7OB297)L$.M$Vf+M"
    "[EVcD#;2C&]9&L=k&pG3W4X_-]2*22*fd;-/Njm$-AC8.UD.&4PM#G4r.;9pXKB3*mqb%$[`2I-HA[PY<5c,5Hu#@I9Ic>#AU9fLJbfRC#^hj<^ux?1EpWB7]7.qW*-K--K[HW3'<CGR"
    ",Yh+#n_u.:?QaciIDMJ(Rtl+DXxSA$2W1v#AJAC4x<a=lM+JT%#:4x#Q:i$#@pQ0(F,j)*L<XT%0Lu:&2>jB4,#jIq<*mN')0@L(u@XA#)]?.MOe(@gd&BD5;J?t7H9[C#;RTx$v;Tv-"
    "'rJh(ae_F*)ptWoI^eF4.QvFM%%miEO`:xb^(G&$1*6tA1/OOD$_)i9wqHj1]FPW#/OvU7_T8=uDZh)I%-nAJkqWm8+Y4s6[75;--94K*Itjp%.HgA#1ML>#8K`v%m^OrRZG5/_CZg&-"
    "r_A]$jv4%5otC)+fY1C&u/x0#[Yke#/@DX-dEBD&Nr+@5od8E>DHpNG,D5C5`7JC#2cK+*KD,G4j%r;$;,`^#X@QV-+ZWe)cc6<.YFn8%bZ61gHbVVA2;[8/gSncu06g&rYW&E#*#dS%"
    "q5Dw&%#od5WnUrZoeta,,;xJV*X,v#WGg`3%vchg8TLn;_KA>#3S###XnligDiw4AS,Wk'Vf'Z),3`v%g-]qR<rH8%)5Yj'V/Mg(:ulY#iCkcM>biQj)O2c*[]>W--pGF%L)WB4X9>n0"
    "gn+DuH<@12esU1.$K4mLT7Fn-X%[9K9WnO(@Phh0kj@+4vcMD3K+5gLH]R%%s.XS7fo@cDiQVf;nuO_eSVBb4g.i;$suWTu'#5Y#[k_5U)%/+s=K<-*oh:J:TNW`j4$NS.3?%AOO;Ob-"
    "wspD=)%R=-E@8Y.R^Fl7>*Tg1:G-o/gmY3'PW]e][LQ1Mg4=-&/5R3'9cWt(h;r80+0Nl(ZOco/AxGtRd0gi'Rt3i,f9;h)4kv_#uu9-M7pa/M*)qo8JZFAuvCEpL70<E2$ab.1/r3'o"
    "EIEU8B2aq0):lI)wss>-?D,c4rN8C6AYld$TLXt(jQD.3FBM?%UU$,$VxZK#m3m9P*u5t#3:Qw,Dm0)?h?i/Fv^N8qJ6R_9V$%k<<SH4<NWT5/t_q3C<oVea(qwM1Bq;8AL.lB-osAqi"
    "v85#><+50HDUFGHf$'##J_-0#aB;o$FB%%#3+]V$&0)-&uRq._'DLT+A`>W-2p>F%r:xr%l'7##3E_/(@a--)&a((&Vobe#$(9:)^/lE?r):'#s9k>7A6ho0ojIji%((f)TK3R8IxVm/"
    "'wIK##H84GE]%GDWB9[@Kjof_Q&no:q2c4A%G1h=&?hu#%/5##T<^;-6.t;2-Mc##P5Hx#*/###R8Id)PYiU.BR-@5@'()&kv=PSh?w-$f9lmufDACjmYuIUCdE8./Fjj(#&<W/P&qdm"
    "e$9m&r9^e?O:=E&k#dn&9I.q&60ReMdQeh(uXYm0kn+DuLaW,Mx>Sh(DUpwN93)u$@f)T/]^D.3qi1M#$LOI#u*(0Qca4j2'I7lL31>/K/5GuW+Snl/4$@.#Ma+W-3RuQNc?3$#AuS,$"
    ";iAC42$D($NZa>#uu4/($3oY#hhJa#d$kp#@'W>#XW`k#-5,#>awB^#'vi4>WZMW%jG>+%1B'b%3oOu%cG*p%6cL;$Dt+j'01n8%Pt*T%g[bcMcb0dMfx4/(@m,9.vFgQ&=hxM05c+F3"
    "P#Q;pK5e)M_d+)#p'O>7sk:F*FC&f$*B,h:ls^>$76&=%*[uY%l./(#mCIfGwC#W-g:+][W)_#$-wcSJKjKG2GPwFMAg(A4IUf1BBdp1'f4g=Pqo@YY)V(EMWcA(88m[a+tS###P:PI#"
    ":`3/%1/uL#(qgb$ujS*#MqH0#b;gI$1Xt&#If`Y#jMOW*94n/_059?)<ru8.sbA]$>fo/1RaHN'iuhV$2WUSSC=`$#,?Tr./Y<=(/x<=(pjUS.u8bW<sa7mpAd(9/Ho-M07pRW5o$:Y5"
    "iuk6s;#2))WYAg%.gZEP0C[20&ctM(sf6<.0Z1(&ONXs6rNCN=UngaRqnoKHLi@cDCEL5:@5d_%7bsQ9>gKF$C'SK=5K'7;+;<fCC+o?.4ZxIu<^,U9U-'.El(Ui8Vr@3kSb7G;?vr>#"
    "+SMZIgsV2/j<a=l1.eTiRgo;-N=&8)Bxoo0uPt)G2'Sm2c,tfMN_->'riV%[SWs)G&[R5/aOl`daX$>'$66onGiK-Q^6E$#Y>cX7apr)Nd:j%$)tQD*2(.L(B'bI)%r,;%vu9-MNbPfC"
    "Toh8.:]EQ7qx;m/6@QV-0V'f)6P5W-k@x8^d'BP:/(KD*5Z3=MjKsXl/faQb2c),?_wx.>XuA/(cl;P_3#7tuf-RO#rl<7NSw/4O%/5##g26c$l%M]0Q'+&#cSfa3rrXJ:r8CW-k<a=l"
    "AQ@H)#'a]+Ve`?#?7/H%@['B'_v(8&3TZk]/i@Ks04Bl'->30(7mFf*/XM8&MSR[#-pWY,Q$4(&WQme2(16g)O2'J37j@1(;]d8/Fsajj8m]K#6rI^?Nvtrusf%h6)$?r9)rU0<DBFS4"
    "W<gB.RVLBqXIN4oQbar$oZFBJCXJw9GZb]uE;W'MN5DhLMNDpLx?N$>*l1/:@RE/2`NX4(io/#,_xW**FQY$,*m[@#&Y=gL%Jv/(St<O(hH31#XSbe#<Mgv,@Gc)%a&BD5eD;40b[Rs$"
    "&0ZJ(ZN(kb6i^F*nHMEQ$1#B$HP$F65&cnuNGHQEf&fF#p;'7D+oO<B$:7euwKnR7'fe@#6<B5&]Z>mB3(e>O%i_xKi?'##NwQ0#m]9%MtPa$#,x@;$F[@aY&>feaKW)0(cD5##2;6QS"
    "G0gi'ef@S#IVaL%3l(?#-Qm0(UJr0(:,T02/c-/3F3fu@6Bk>7k),W-iGqAZrKMH*<GUv-VM[L(Bj$2K+fr%@*%.U03ub`ud;QvZwq#`ukW`v@Al4wLWse$v.x+wL`#s'#KERS.-W%*'"
    "E,d2_->fv%30o5'*(u,MK.12qR2/L(?,8T&iv)a*O,t5(nJ+V71jb++R&=T'oH]uQH.1wg^FtS]/`*D#33=m':4r5/xe;B4V+ofL%l?a5[Pk@$gf?(+tiq^#k6Em/LS+bN<kaw#Krbp/"
    "LYPj)3-Z`-x80I$K2jB4Eo9c*]RXK2;m@GMAU.JMO^./MD2=T'G3VB#rE4;-KV-4(a7DW-'qA`?X6>n0A=Y@5;ou,#YltH3cb%Y-q+3(&q/JO(ITbi0]LKfLv$EE%/_#V/KlR_#.T(T."
    "ls'Y$7Q-i$LOPF*q/l]#XeRs$k6?H3,RGu?ghx<.+_e=h6@=shqCw,J-N?w'Uj:999J$-N97^hFja[@kif19/<H'E+J(#M(7]S;S&H2<;g`9@/k='g2@rOI)5b6uSFMI>Dv->>#DW5on"
    ";*of_:j,>>SSJ8#P&`c)WN0q%5D=x#<dvc)F-CG)@u:v#eItcMw37%)p`uf(2G#i75%pC4=iTkL5G^t@OWB6q_2*22<lNg1cHNP(RgSm2CZ)F3@6;hL%DE.3Aih8.rZv)4;Js20IkdfC"
    "[#mH)GN6c,h]3KQEJ[g$HUXeuqf3%9uvf[upS&4VNNqJ#7J&RWxs%,vD$QR$]@%%#c]v]#;&aB4w%ZQST)ic)NLr?#%tcc)CxdP&xR^U%E+vl$oX/5:8-Tg2v-H)4$&Z/>hECW-4Su(j"
    "T(<`ZaTeR#>-JQ:quuJZ.T.Gg+5cI_3;6cQs_DeHI)###^i:eHS60PSfof'$$DFciPSq:mGE9Y$J=*p%':V/:%%jx=>rJGNah7<-u[re&qOIs$B]1Z#50`C$0]u>#)9WuP1(d52mm4/("
    "=w'/L1hR;$v(^t@Rt&T.+&+22jbPi.DB`8:)FZV-HdWFf?uPjuP,MK$Spnd#u5/CM`N?##>;suLvV<$##-]V$LRBH&)Sq._6:Zm(:?a@$(CM/:ee2<$u@]d)sHu2v//ZrL[<Js$1LP>Y"
    "I#1<-_'HP&ua6(vLoNo=v2I2LELEM*j51U%XJwd)g:e5/]fKB%XLG>>B]W/2dF#ns.LP?--0`mAg[D<$6?C?-_q)C.LY<n0,W<n0D?E^%0jkV-^r8.ZM[:.lVPEhu`F];i6r1K$W2=e#"
    "xu]G2&BhiKMV'##/f1$#wj1C$P@Rm/(/RN)qam.h4Ks>&g_n8%PBkX$N+2e$i+v>#>_aT%tpA&#x8v3#WRmt%(sD?#=fY##$X+aam%p.=eYd=p,&+22&#P12PMkLs]=a$>]o#0)Y@H>#"
    "#FYf%h?XA#ww<S#&cGN#;FR&cR0#F#$f'^u$j9^uR('U#%8ol#JCSf.'0%D$iJdx1GYij7%ow<$=Z=v#?SRw>2B=AYHc.T%qlP>#hTKJ(HhFJ(XZKQ&)t(S&<2>h#;_*=$6Xfi'YmYN'"
    "2G#i7*jDB4Aot#,`<A%#>R7w#]74,MI*V/)1l]O$'Y98[>0lJ(glES7NFb#-2[ZuPm0ep&,GuY#Q;P2(ohFrf`;tY#-5Uf#UNuY#&,V#A[v+f*A/'O(F*3q-'xdQs@Vt,#fR%-1;>2>7"
    "?;1m(J'kNCoxwP'Q^wi$UjKb%#lST%g%U/)_n%s$v;Tv-A)WF3s?L]$%22Oefc2S@P1QniW-R>?P/F)-3`Jr6w%,rQMerj*Iv&88$*,##LG:;$?k''#vGH^7eX.K:Tsg#$wE78>nUtxY"
    "e/$N0[Ccv,vGBpR7j5^=VnCuRIXiv#3chV$Iike#Fns5&i=vi08%i0M95w*0Xlv2ALx]4A0?fJ&AlUX-_vV2%/uQciEHL>5Q^Rxt#qR5/CXgquokLZ#q3#54;'`($W+O9#Mgv^7C=/C#"
    "SBKQ&KH9I$V'u2v>AEhLh%Sp&xAPv-&a,3#XWis)_Ku2v=CVq%&/ac)HKF$0*IP0Ld,]iL*oTa'a$5/2^7/CuVKW6p>K.p0GCf;7].`$'9X>n0[7JC#=oOJ(vBo8%`@4gLt'?Z,eL0+*"
    "VqXt(*[v)4OD^+4v1NT/(>#G4u[]w'4U@lLaeHM3C)p<7#2DZ#><o&,llXm1bU[WK)#2b#>%B4;<]WB#R`QT8semigG.:c4,AU'<e0j%,PvKCSt8[]u'lEUWC:_F*(iB_#evanB?;hF*"
    "iwc?-n-ZQs@C*ku5jRh$67>##-w*$#F6O]#?-a>#;Z4N3&/i>7pjMhL:5F0v1[sss++>>#8'`($T+O9#t[.H>W_D^43,oH#K3Z7$[H.%#r5m.`9<fZ#<@W5&-X4Q#AmHg%IW1N$Cv)H&"
    "55G>#>l^t@'gHT6.xO12B-=W6-1/(#E2SJ(u;Tv-`GUv-R/xF4S,m]#lU7AkVS[b5v8pTT(us'B@tEQ?KUGSex_[&7foHO#%3D'-a;L-QWX6##o?l>%XKX&#H,>>#e/KM#XXF.#5NBw#"
    "DgqK(T,q#$_nZ5/doS+4u4T;-gpp%NCi,`.SiSY,a00YcL`u9'6H&)O[6B#$EJ%p(>cBn-;pd]>0U;,NvieP')6tY-s*r<-iGQ3'%@RH)X,rg(:ClKLllRL(YM5/_>c$?7vkA]$<,=A#"
    "vvX=$`RE;R`)dn&d]Rh(]M<&+68vBX,.<$7u8b.'RD%1(xkY;-,MPJ(Js_K(#U^ca)xC)+/uZIDO5)H*qLQa;wHs?Sm.#K3Jn_j8I`:5KoM*H2^1QA6gh'P68Yfo@PEFJ#IvZ<%2vdC#"
    ">Je;%2,LrAvZ-TI>PuY#*WXxBc#W21rB7P/mLwG;'']P/@Y#R0F[5n04CX)S]Hfu7]Wx=C3+%i9t]tf_B2RA6G.RA6?G6Vm=%?uu.j@duspOq$qMX&#J,>>#`h#V#A%###+*F?#90-u%"
    "mwur-T)X,4/=x$vdSp2#C@HC%/'l?-(MxN1B:TM#7/HU7=OJM'4dTw0-rZ5/['.lLRv[>/nZqr$:ZEjL&V?LMNehq#Sxv9/Ji:;$BKo_O)v8)veA@@%Y2(@-'9(@-OeTt7a&FB,o@Hp."
    "1$jr71PkKPqeW7nH7aq2:0_r2P/,##p?&@#h,&#,O&/N:w<b/(Mi3p83,WAP>0l^o;p*_#C9PD#=`&v5dR&4Q=El,O=V-&6#nS]cFk;F@#eOX1[j<T'eLsD#DM*CAgY<CA-'_(#J9S_A"
    "N_4r24`wq2ZouY#u8+mL-S()N5*(p$cbci_,%YfiPqpP90N_[-gq<Sa?:pkN.EVXOpML)51r^j:9UT)*oTOq2xK)58UXt]>wc,t$fKZ:&o3VS.8vcT%%36onmMjig9HD2:=9n,MUK/I."
    "k2p;.[jT^#S%19QDuY=&nR2aORr[(#DOYX1G`o_A6699.s1HP/NfoS/18DDXna/K<El;T/)#F#$v*3K</&c1#S$.YMb/F'MeZZ##N1h)MW?j+V0]qr$GYE<q8Hn@BH&###Uqa:2[NO^6"
    "L=-##>cm=120rxu,21[-4aoQaRE?JiUmW8&*JY8&>b86/x%,##[BC,MX1l?-4cwe-cUsH?(osH?x7OcM>^B6>B-2'o$K9w^88'58[5UB#Z^8%#twur-F`h<-5nSN-h9u-(,wx%#oOK>,"
    "(`S,X3<AA,P0%em@?U,XrVcA#&H)CA=k=W8GK-'vM-LHM]+$u7hieA,FgME-)mAD8T7f5'SqO?pPM,@pq=dl/hM_[-CMdRaRl7onu2d?pm5[@pcB$)*%Ujq.YQpZ1ix/B#=@<7SGNd&N"
    "m336.lW6iL(a;E-=jd&NO]2xLtWZeNa*^fL@e7iL,Mu(N.@pm%H),##smTF$dAl6#CdoW7@YU;$@QT5&_,dr%2l$s$h<)^k^&5WKc@(5%R7JC#n*LT%&'lA#0vRH#x_jhuR1TM#T:lmu"
    "&q3tt)MfP#M/5##8*mj#4w)'M'_K%#M/?x#Xk%l')F,##=s3t$lDc0L0_sw#TbJZ$0;EDuJ5e)Mg<(u$8^1-OT(/W?j93`#opD)IcsVDIA7eS0EjiS0C]LM^,_o`X+4%x's9sKGeVrau"
    "i:tX$CSk&#TT5i7?CO>-R2[d)7.KT&T?kp%PqNI),MAC4_`&+*Lf)p//TG&#_>Qv$#oc_+7&'#,g)<W)Tqws$t%*9.9_f)&)7b)#e2E>5n%v[#.Ui%,pn'R'5W%9,5-Vk+n='PfwH91M"
    "imVcD2*#C#dN5h1p=vi0wmg5MGJaXI%X<C5d`9D5sw-87QxO12,x,@5-G)s@L8u3+iX2MMCYPs-lgZ?>bT03(f9(#cMYk*%cOq(c5=aV$<=Rs$=;.$$e88d#)1P.)jxA+*ms'Y$>P^n1"
    "dVV,)_5D0c3T7+*>vOYGO6:Eu]eIXL.'8Uaj^*v#g_tWL[O'=1Mj=oR-^2DbxqL[Im%XE'.7HM,Mr#nB]B5c%9S;C#n'YP&&sc]4oB*72Ux'mB&U]m2?6^@uvJgj*rj`uP$b+,#@%C+v"
    "b,`_$/xv(#$rT9(w].5'cEJ@#xsRt-4uQj#1cYV-g>,=7*Fg_#L,wtRn9Kq%VF*p%P'j4'9oB:%D1u=$T2nA.+7?/(>uQ8%oc2T/S.=J2;+K[#xgbZ-,4HO(vZ2e-?f//LGHp]#:^:N*"
    "HpM+3I-W6p4K9N(d9$@51xo*%k;+/#aK9s$Bx*Q/P4_t@Lq0E6St0I$`vQL2&0fX-=Fn8%+g^I**)TF4$cRs$ax;9/@Dn;%q19f3+EsI3=[p)=5Vp5:/7Al#L=$_uL:Kf)=6RH3aQbd4"
    "MJ,xB=QZ#JhMmNLlZ+q#`O`Z$ASoE_?b4P+cY(/;9Y1U'dYYW>63/8'-IV[,Q2Q://4P9@p]:aSp4WaMZ`o6KFY?X];.eX9vFw59%/5##e9@`$Y+O9#R1g*#Q=fK)LNep06Xl3'2Qvf("
    "+o=@$xMHC#h;P>#KWd25MtsA-2,U;.d:4>-2G#i7f/aB4@o1I$dhvQ8'@Z>%79bTfTxn+MuY<rLU8l/M=($29QZ$@#Kl,eDM/Ih5bTU;%I1me<#5<@Qiw:.30.&=1WxPe#IkS^3&CO;p"
    "RvH[$jgx4p0V)s@&Rs8.oW>n02v`N0/l*)#7//(#K1s]F-.)H*uMcVHU@f.*ig'u$GO=j13?w,**)TF4/Y<9/u89o8gx_E4KjE.3VL%s$XFJn3Aedh,>(.,`P*IR8NbS),l?F8%AB,(?"
    "3ef'nL>NFKX[&@#`p3Z8fDTB51c+i?B[d*=)x?)Qm%SJuT$e$,At,X1:rsILjsrB5';.w-eQfsAw7,K?[;lxHQ-69FRrda?$eLt89lA60Q9Q:F:J%+5dXh(QAkf49K95M9E[7=-q;E90"
    "$Ke/V.-El]NwVG;O3OP&0N@W&8P>##:6W;QaKX,28/&(#N@aq8HFq#$G:iOO<h1p.DeokbS7H_&RI?>#inY(E:tnf__'EM05@kp%96*p[C7wS%M9wS[;3RS[e)L-2c3Np$qilA6X$sP("
    "YHDA.4t@X-/av5/rZv)4`OBx$$FaFi<Q'I9/M]J#7)uf_xSq0p$@k0pXt]Zft-L'M41&,v$b*X$'G-(#O:kZ#FZ9*<w]S]$ci-n7+jDB4I<W%,TXNg$@dVs)V?gU%OqHa3biAC4Fpg7/"
    "LX+aafb9dM[o320Q[YU&J/#f&2j=C/IRIV[vb'j($e(g((;le#`q]c2*h?&%hkf)M=ou2A#e=E,X(-)%Vr+@5oW>n0[?ki0rj4l95$Hg)<3rhLv$YI)@f)T/?:s:8&KP8/i==0lhJ@T%"
    "o'nSDOuJEibj0kLDu)VX'kqv7vCh;.pv,9<`O'd62Zw'$N7?lI6(G=og[xq/k+kUBl/e?-ToFk1IIY@RVnCK2[4cP1-6]it'J:;$;O`@k3JkigMDW]+nZ'_5TgDW&1DSH)H?9I$jdu2v"
    "<ke$':g,F%P5:nL.*DC$(?:^#NMa=2eH/GXksvBX64,h2n'#M$_<hK2Lv^>-8ac30BwFD<ZnZmpLAI60k(H2#q05##rLYv2LgYLQ+IRZ#PBO;pkr/V6,G2s@G>%q.nt+@5B:Ib%]i:I$"
    "0Z*@5UXhi222Ys0uU'B#`vB8IqRr.3uY?a3iYY&#*-:D3xSXV-?DXI)^7^F*WZ%>.`GUv-r*h/)I5^+4oPq;.'1pou4M=&Mnk4T#-dq-$d`*jReIU:v$UZcD8:_cDxlo%F'PaeGTT0v-"
    "dwvpH`d)0(lePD?eMMuAfxIh5?e1T#wh1T#uUlou@P9U2#Jl`#ISEL=2B=I)djGSnkDAYuI7$U5bX+)#b).K<sqVu7u>E^4oPAC47n4$QQ]<n0u$NP/)][P/f^Q('n-Z$,Q$V0,>Up(4"
    "jZr`F3q&Z-O2R,)UbtQ0,HFA,4't8.O]DW.790t-BZ.N<ib45pB:Ib%1Y2s@g+ms-r[lo7-NQs.:@`/:V+mG*x@jf%6)TF4Z^*&5b8.K/23d@_@MHjQ1ge%)*201Eq4ja6)=P*,D)nt2"
    "1UC<p-upgT]2lL<YXef6+4&##KdK^#YuC_$:Oc##[jRc7qJVa<K^hg(OQCK(go3I)6(I8%@'v##u(NjKF$^Q&=;ZZ&P,`;$qCMa<DJ:<$_G%L(Rvhg(Elrs'IM/E'(%Jb<Q5^6]%+hJ)"
    "9'`G)BfCZ#e%v`<5635]<q:I35lA12q3H12TD;N3YfHT(6a9D5UE$)QrtQA6j]@12Z>58.A=Y@5d&?m/4o0N(H[U/)u'JT%xp^#$),i#$iv9io6gUMKs2'DIK&FIT[i[o$2CuRRTNcnu"
    "x-P6u5INDu=-.m/s=wCW&$2>Y[Sx'(K,,##u_ik#[EqT%pKkA#>I>F#W'Jqb&%-fh$&5uu.TIVZ'4%v#E,W]+%;`v#$Luu#W*:gL)?5,2tSlk-A3@']VI+%$)-Q6W8<1^#$jJ=u*eE8%"
    "shSAXjSf=#,x^#>ux4B$*bh/#B.#a7kHf>#AwAQ&?cY>#bvY3'6R/m&;'+[#SU%[#/chV$C`e,)SHT6&Ur*-)N<G3'Ork;-MFi#GDoAQAK?P3'[50@6h6s52-pCp.R',)#LM9N2RRHhM"
    "9D-t$`:mlL3LO=(#UoI4/9SX-&olV-4I5gLw-gfL/EsP(T]gPBD?qS#k48,)-oNE#poNJe,Tdou<AF)8s:dVH$j]Xu1N#lu1uOu#lZj)Y%2Puu4e68%EAl6#Xnqx2@VeR(_>i0(G$cJ("
    "/SpQ&20^`n4eHN%FP,?#X?KW-NjY]o$;I7&ZIY3'I[<X$HUnS%%r,R&1E^22[Eho0=EI12#jlA6%xD8pO4_t@R4g=pP[)22S80I$9v[;63K]n$+C1hjrqlV-erut4$YiSIAg5)3pa02K"
    "Elc(<j;&&$RM'#5qFXT%:KcYu[80Mu&@E)#IN'#v->#f$'[I%#jd0'#4kn'$,d;k'LWZ`*tGi0()>YY#'EF#-$paA+U$fw#l2EA+3]5j'Jm2-)*M(v#,w2d*'Gkm'cfS>,1$r5KsY'^F"
    "%nu4K6PH_F>=Rm0/iuO(Ln)gL5r+87t7Ph#A0N<%,7^A$;,QA6<Uu'&8gcb4n.B+*N.M;$ax;9/Zk_a4fdb;-bhhp$_xG>#kh/T%R=P#[/9m@6:HjWshYaD?=HgJInXbiBlVA7;6]uF&"
    "-)Zl&xsmrVb5ntV</C'N,6sL^][088x/x[_F[UcDk:f1BLEr-9m8GYu8L)hng>uu#c=s.LUowY5_XLS.<:gr6H?-n&>VTDX$#ZLQ?Xjp%kPIk'_@Gu)mvZ`*m:9DXb.Kx(^l^6p/l*)#"
    "7qeG3NHN/25>Mt-v-MkLs=<m$::[s_iS&J3KU%],Cn+w$s08W$'oV?gR0.G4.k$UTgWPJ[A3D<;S9lQC@^Kt9(qGH42P:I=^0uLD6^K'@[%(32M7b%$p^+7`96j<E=mLSU)4eK?e'Sv."
    "ce6QDm)_u:a$kk)I5YY#MM'##?a6&X=wfi'0lo51o9KP8sbdo'DrYZ&S*pe&E-'q%NP]&(F]-k&C$Ft$K(=K(w=_t@G;EDu<9I12,*.Dutu^t@/'+78Ocnfl%$rlK=]OP8$#[.LaB>fL"
    "$1*(a#2HVHshwXc$7(/C-sM4fmT5RB6YU]kml#s$KQ]uGTs=r75b:'#;$=8%8C[s$]=7F%_il>#5xqv#(nP+#vk1X/+klF#2<P>#_4g<U*r)g*(SpgLAgad(sM6aP]OH>#O3^q%5&nE0"
    "1QZFM95f48pe;F%FtPtLF'2U#%JBquaxisup8Vcu5+P:vjJaE[]peE[#56SR+KPOWZ8L'###5>#0sw`$IT@%#0M*Y9;$8Z#GgH)*VXRS%f8`g(`2`0(+AP>#qL[.5T0F4#M`###P<_c)"
    "P53H&3mgb%;9@d)ZhNI)+2,##MN0:%$YWP&V*B2'H%M$#)SUV$STj]F1f_;$n[ojKQ=>nA+oeaar)L-2#gU@6mx<.3E*JY@+E'N(l$@mUr'nA6)ME_&`WeJ3ihd*.3n2jNk8t]F.#L^#"
    "Z1QA6f:+e>[Cfo@nO+.)&V5g)3E9N(VRG)46*E`#b:Url<E1`WR(/S@$J3S@+=s=>VVp-$Glmf_$9;fqV=61JNDJX#hrK2k=?Na3c5-G$*m''#iIIZ7X&(@#SK@(+g/5N)RujO'82hPS"
    ")@2`+IHc3'->XB+EsS>#F(U(+^%%X$Ri#:AJnq6pUWio0wRTL2'M4I)a^D.3NL%IQR+r8%@f)T/M*AX-Swfj,<ILO;<(X+Mt)je3xl:Z-`%U5/SkZY0OMuf#Y50:>W']A,6@Zr-%/5##"
    "fkr8$?+H3#q8q'#BKOH*``q;.@Ail0`0@A,+`<n0'h[=-/Eq:%w=Lf#u+%q/tT-4(LB]^HSOqkLo/_Z$Ci-=7=9I12WIQlLD[,@5j7JC#ev@n$/Vx+M+q29//la20?_#V/?;Rv$')Le$"
    "Vo:Z#>^C0c@P0G4Qe,e3tpQjPfnINN1J;XJDd<jW/%%v9Y..I#'Em=9vWn(HdhgTC4R#*R7Q8nuqTt'EYJaQ=QZBj?:7B$Z8j;^u*Fx`6X)kPXZK_ib$),##N9@`$3-O9#JRYa75bP3'"
    "0PhG)DeIw#OjY3'3oMk#>(mY#D0gM'@njT%HhnW$<Bhc)hVVO']MIP'7PuY#GIeS%Kg$H)S1Z##?d/@6A&)hl5WKN(`C:F*(psWqMKNP(l%BgL0,JO8,&9D7a&RL2&%xC#86V/)nM>n%"
    ",AH>#9D,G4W,)Y$LlPr#&FtE$vNC3('fv=,6E@xOGOud=>]Io/Eej#$0U_)/)TYFIG_Q7RqOOQDN4kj.RB[%%=8qj%ZkZi9K^ic)<Z6,)j%Iv$G]%W%=<O>-;9#3-BomCFs97w^fTZg%"
    "$WOEE^8<I3],%12okNj0G<,Dum'O>7k3M2267AP(_g`I3m>sI3'K+P(oPq;.]BF:.AHS_OM+?GiS`'C?jeNJ.k?=_?T%A>.i3xB?ZB^01GxU3tqvNw.?(h%%(`>W-S2PF%xu=']/uI(#"
    "=i0F$Uf1$#[DQ]#wx#3#U&[d)2*[S[KeUGDC$bT%G/0q%*,/f.Iacn0[]sfLOD3mLGfQL20KJ@9MUmO(]SiD3Vg#ru^:ktXlCs7uUU_5MTv''#xuF:v(#&=%C9q'#gUT1(-2)t.aq3t$"
    "CX_j(..mr-HQDCXc79u?W,gG3>K-i$Lr+@5>''0)u$D.3Yg`2%-ZlK)v7XI)A35N',ojf%E:G.3?_#V/]7nq%H/8x.RQ-V/BooL($Cep.YAwH)hGFe3=isp/C8k**_Vbl]Ou9`RNTrs/"
    "o^7K)J.@0DwfWl8X9me@#+Gq8Q43Y7.FB-<b<[Y#/q4SnMQrig,$oW_]Cj]7o0eU7=sf.*lc]DXY6xw#Y']T]2(@8%b3ap+8tF/(S^9D<EC/^##i?W*5k&9]GL<5&:2V^3MF<E2C)0V6"
    "2oYh<A#OP(.L9Q/SL;8.(Ro8%pO)W-j55L#w=PjLe84Q/twC.3w(]L(VX7s$Qc?g)b>$emw?Z%tD)Yg1^8Y:KdhF@ZG5qB5u*o&k]1CiF;eP9';^6R*1`q<73wI:-F:Lb*3<?/4S[iw,"
    "*V]21g_(V0fa8guKAGD#;i^+*V_0K20JsdmHIo7nM2qdmw5<A+J.Qj)Umlf(k&N9`$&ZR]ED#7]G,96]2Yvt$BxO12NKbB]na9F<4R#<.tmVa4,NjGM#OSQ*WD0HZA=GuMJo4.1^/`l:"
    "3EI2;Bx8Z]Rpt7XjXAQ>HHXW@d@X#A$),##eZqr$[ug:#NM&`7cqJA#DrJJ+:JlJ,&6f&,*g:9/E]/.)oqlj(4x6?#>R-:7gU%x#nrw-)#QcN0K)9##HXst.&J.p._8l#-]XvZ$<(*T."
    "hl^t@ftHa$*(1'#(xD8p]7*<>_TJ6a+hd#/h+39/Ua%ugk2g[#B/0i)<P>c4HRI1MYfb`<S;Wt(;P.`#j_WQC^.nK#/CC>5`Q4S#2i4iKVP^k;LY7im]=rX@#14?f@>x+uHw)D#JeoL^"
    "%jBxbmr/DrnxT_mBU+uu,j6CosZ+0rc7>##X3+Q%-Xa)#GPj)#j[jg7)Pn<.c+MR/Gv16&tc6<.;w]>-BdPT%]V[;ShlwV6YpMB49UV4(Ds=q&>It>1F6u2vI9[w0Cxa&']wE_&;b%lL"
    "pCNk:MTB+*=uSC4QQh&$MxeJ2i'ahaj)XL<0wZ,MH=v1M3#OcDKcG_/+-%7*S1M%Ii^V6pSFmO29i^t@H)b@5&U<8p3D=3p0;?w&L.Th(/0pAF&n7M+GjL(G6g1T/dh_58;9cD4S)B;-"
    "P]sfL7=Rv$4o%@':jE.3H.^C4:&d,*,8)%G;T)w$<=[s$#Y^58b(pM^xEq=%E$Wl&`QJ23t9*>PKknFiCE-g=%r6FeMZwt7N$6/(>o0d#[go+>uX9K,u,#Ucu-pB?ov0XT(8_S/Cg:W?"
    "3Hht'1`&n**+O=8$I;,-KU;)BSPBb4TEqW2DWN2<oa><JGfWo&;cC[,-CT;8;GEl:=J>@5&nRYSWJPAL,D2IG$Ajv#/K)k1SB280k5>##gi[h$cK*1#`Kb&#G[jg7:E`v%J'GD#Lvu2v"
    "Z`HgL>+GS7+hq7'+v0#?^R#B=-F^p&W2*&+(g)O'1I82'9kR$#SLM$#duTS.<m(>$/X&n/A[oi'QJE1(&6#o/EN>(+*MC;$4Q/E+8]'R'OhR<$ZSB0Mq/&dD1@bt(0]FqC4Tx4plFA01"
    "nD>8pdrSh(i[1F*fXW/1h>Xn0P.l/cB0G7cYO,d$$<:2DB8Jt@h%JD*lR'f)%tAv-j`Gd$1W8f38/^I*j6V,DNf_$'1.j^6RkQG$$g:?9:CCnuhB&)6AWdtKb3JM9Ur$G?D.48,JRIOC"
    "$GP_-H4DRB<pOGMoEl4h]jwUdX3gWRvbH8_u$%v5Q/MEkGl.kTkFxTKD`rjT?PC&vl-gauoJ`K$Y+V$#1XMS$`h)##6F2##:5+87f[KfL7w7guC3KkL]HU*#+G:;$fKd(NHnCdMv2C'#"
    "N$eK#*<wO$@BP##6H9w#2'Lj$N`-^#ZEO,2H(?<#MxO12dQSX-9@3/1>p8au7RU;$_^NT#)O^Y%Vo#P]54@d;T`MiKh5v9%/i_;$8c$H)6rUv#wGtSR>be[#/J1v#'`eGV_DC_&RJP>#"
    "6iLv#bu9a#M^:,).i_v#(DlY#1u?s$6%7s$'^aI3aw,@5FEk-$TTjl(N^Q.2aK4mL(x0F*p6Tg1[s>c%ZF/T%ko;l0.@9$c7S]o@YwH>#x+;B#5*qaum/uZcu>-C#[#=hs:[$S@mL'rd"
    "[^(g%5nt@=8]kigu@lCjTE,x#ZQJ[#hcRL(I]X;-Xvhk'LrcY#<?.(+NO_v%f*I>#uA_>$'v%I)j`wH):#jIqRNLQ&-<Ih(pb%T%CpMB48k.A,YN^q%I3YJ(1+Ih(>LWrR3^gpR`;mj'"
    "6@wo%iFM0_?6Od+9B>P(Oa[s&JmWD+pwkM(/XML)Ee7<$wt'hL-%MZ)Ox?F%NJ&C4c9>n0Ox^t@M0Im03M(kL_v]p(4U<8p2Uet@>rHs-)7t?>^6Im0ld#LMZie`*rcb05_Z)hL%?^f1"
    "d2DD3R*8s$=mMT%Q$5:.4:[x6Lf^I*<GUv-dSk(jTwf[`v(7eRu<;O3$=T0Ue]2`19GQQF$DOw@upj-*A^x[k]vUTM2/qS.*Ia%kr:lT>#cY`u#2F[?%/5##13(V$q]C7#apB'#aUp-`"
    "df$^#g=>v,j[l0&w/v2v3Avm&Ekj;-s>EZ5Q#@d)^K-w)YUq;*+k/Q,3ZZ)0x:%g[tp<2C&^K?-SGW**deuv,E:*T.QwS^3QDgV-:@Jb%NMPLM5LZs0Z./(#h5t.C.uK/)Qa:?-.vDQ("
    "bH1nuF7D#.>^J$8cp][uG8Rx%f@QMC;]CP/AG:;$6RnqLnKR##IiA(#3r$W$2U`Y#95YY#3fCv#wthY#goRfLjB3v#@0lf(/i$W$O7FPA(8G>#A5T>#Ba^qBoF:D5v_,87BO4gK'>vV%"
    "EeV,M?v^f1rP(Z-Shkrd;=7;H:f1YG0;^DE&PnucW;-_JVo/-#O+Q7$cCP##bn*[7C_UZ%(%^Q&9(Rs$5YG##]R?C#0k)`#>Qws$@Tw8%O3$sadTk,2$hxkLMj/V6]LG12LD0V6g^nlL"
    "0+Gi(X3I]m<F#7Kq3N%$hVXV-xhiO#HbMW#S#@ou=-'#5Q^K?dw8,;m/&)D3o=4Pf)ik&#Gp^oIID7GsH*Zm9(J%Z>GLJI%'E'##HmXlSZ/HJ(J.HP/7ncuY.X2<%9vY>>WFj:/+Yqr$"
    "b@jPC:t_U8YM=DE,B@W&AKuN'S#U^#0)<$8U;(@-tD(@-*SC[-NJ?X1kL3Y1g;=,Mf/cV$<LXm/i^na+P&U^#>K=[6mUYi9'&)aam?_]+v[RlJ$dq%#B9p=$E=*ZQ<aO1WLisg(C%nl/"
    "#/AjKC6B6&3l-s$))L^F,+bT)lelX9BMYH3qgIO(qb+DuCxIX$ULq,M8OFDud(Q1W7RTx$LK1T%fS8_4*^B.*h5Qv$iL0+**)TF4>U^:/Ed+:$^)QV#%*O]#'K1Y7TY14tNv*/R?f`?;"
    ",VTwIEwY,6Jwr+4n<fJ&<sw*AQl:(gtc?MKV(1a?-@E3c_,MF>vLDXV'6>##N3+Q%.RW)#mj9'#&kRc7lQTq%@3.G*I:h3(=1%<$9ML>#@g`v%v>LsR%BmtR]A,/_0Ybs&qU&A$Pl401"
    "NpYC+ci-n7=sKx6v(7$$]$112Di5,2OlW<%.K_s-2kd(#;n/0Mqc^k9Os]G3<Dh;-DG8v$i=>,M9h>1%/=3O)SN6h2OBWJM+])u$x@h8.eYCu7I_CE4W)H&#b(7N:ev#Z$C91T%n87<."
    "o_oYc2u=NBm&W[.DL4auo8Z9Z:mbduSfKkdcApFZovEkHdmBD3G;[T@.<:X'6bdf=v*m;9=VQl]a>Pe3Dd6-I/Y587xte0UMC0$7+Bnou4^5,I^eXf>N8^iXA<G/#pDJX#36rZ#W6>##"
    "&;q4SakrW$bLKlia^S<$xNMs#dwMcMai?mS7>Oa*$>eW%k)>>#Xr@1$7.O9#GG:^q+Pc##sc2<#<&>X7Y%vv&kug''a[1w,uIME<?x:$#bnNT#',,##:Ml>#,=Z,4;6]t@B,V#ALCkB-"
    "r,kB-;'.b5i_s,VK?,R&B$7W&7lsP&`lElf&2>>#]7SfL'b*nLHH-b#DI%+MFKo9&Lr9Q(2U:VZ;fl>#=CRs$fFFcMHqN%b/X8C4mj26pB3@12X_,870@'*)hiR)3186J*j,$E3ktd(<"
    "KLf)H)^Z&MJUHR_588T4pMOG@X67WZ'vI#/A1VZ#?M>F@-@,F@c4QLM'FuC3l<T)6'M4I)8bCE4Ol)Q9-uiv_Shr`$w'OpucBl<=+,F'#PwQ0#XE<l$&iaj05f1$#-QFWZ`5[S%fl.X7"
    ":jDB4iD(2Qk%bS7DsA&$9NHg%<]AmA94[s$fnKdMh#1-2xC0V6gl^t@&x`8.2d>E26al2Vr*)W-1dt-$);0U#xgteufN3dth+(5M.q:_$YeCG@8(pcu[05##U>uu#)802#1Yu##prM'%"
    "@U39%6YqG):7.<$=Lns$*cDO#Abwo%3f68%@O3T%4PhG):7rv#Akap%WSMj9<[e[#5MP>#?UWp%3uUv#-Y1v#@+*p%MV';QNd/v#3xqV$0Y:;$-Pc>#s=r`aJ/W&+ut_J3P&^t@`j)22"
    "&m*)#`i1@6Y'TGNT>Pve+(b2K5k'=(bZsR8KFEX(%sRL2RZ'B#F[VZ#CRf(WD/oeu_YTL5$,vx=u'mYP<.k:6bGDVdoQ#Nu&Cx-#(V_V$cx)5oHX-s-*9&##e<;YPe0c6D+Iqb*NvT^#"
    "a;tEEu4KON:9d>%^JL@-`aU@'J/,##I(aq2p./=B^e?>#b[xRe6bqigOI)20GbM4BbECB#5k:I3C;rdmW)gV'1Zb,ib7Bn$m1%+Mn6df(1)0W-0NHsQs9:m7oGu/_q$s?#.J[S%,0Bn/"
    "W2[-)3EL0.h=O.)Ipq,)Tfc3)FK(,)rtCD++BId+ci-n7=2aB4coa]+C9p=$t9as$3uqr$WWdv%^QXA#dhw1_`gw2)tLE`#8sq>$wfE.)SWlJ(W/>/_CZg&-,.^x$7Y>;-pB9U#e:>##"
    "4Y,##>ocY#@mRe)*FlgLvoKa'M82ejM`8D(UR<lA^cv75obO_&N1xB##CR`(A.Th(J1sB#>ru,#77Af;.ej'&(^MDusAX'#^/:e;nt+@5e-bn/oB-DuMMD0ATe_&47OA8%k^D.3?ch8."
    "8K><.>-w)4mfh*%mi-j:NN8^Ft0;W-VQ4x#]]eLge3*_`88X.USsk5^q8pY#s1,rLGkq6c^m`PbQuht0r'rY#$Q]6PpXgI#NU;'(:xvN^75oduF'[`^=+](#;K?D*P`OYPfV^V$BMt5("
    "3uh;$+`_;$-R`>#1'Cj$]LG12f'q&##g#++X_W>6LN=W-kjmH=W=Q:vc.h9;OAl9;W[W(a%%)##$Rj)#:6CG)IE2,j=hHqDD8pu,k)h6#,xgj]/qQo'ci-n7>pMB48LZc*>9o/l7sK(,"
    "ZXLw,Q6S[#XqN%b[u9b*(%J[#a-Ix,vhWV7Pi?`,mlAC4>>CJD0]:;$jD@W$ZPFe#kkj8.):>n0GL3O(I^*22D]*)#ptnA6ldB<-)WIm-vRTBf?<e3B^n]G3/Z-N0@GUv-a=YV-BH`hL"
    "k2G5&tLJuPD<*D=7rbweOcC88ea7H#Qnr:MrKR5/3A8N0L&7I5wc<<6v(+?Kb*)rr:oT:r0.C9rq'3wp,rbwp*iXwpW-MO+rVq-m$p6.mHt/lkroU>7UsAT(;>2>7d>n;iUKQDt'&1GD"
    "NOd&4`1%Z7`,9q[Jqr$#B#tp[PQpQ&at[<$:iu>#Bjli'rfT?#CZ4N3+D0V6RJ1@6#i,87j.a$'TZ&g2;ZI?>Z_X*ZL$U,&vm$H2U'bTuA60qu&^YF#^8xH#9%pO4`MI[#T(&5#?253^"
    "+R%Z#tSU6p9_2B<a2ipJ$Kb(<7w?cDvcnQ9,'R%tn<85&&2,iCj*PqA8t%<Q1hE<$RBO;px,,-X#ZDQ78UvU*0Hgiu#;98%J)>>#lDb*$gIbv/UFe_q+Pc##31N+&o*vr)?S,j'V9@:7"
    "K`Q=$XY]I&`TE5&KDwh(uf09pic6(#*&>uu<kUV$Wh1$#Uqn%#(WH(#X=#+#%'Mb*%l2o]0.d_+NmW5(P&=c4ci-n7,S,<-NuY<-L+Z<-2uY<-,uY<-DBLV%n/w,&d'*9+wgBd2_'VM:"
    "3v8V8mA/d+A,r,/ovTq%DWE`+PW]e]a_mLMo:Sh(fCsV7Q6:V%PfXi(4pGo/(rc(.;lDp/%cBA#L4(T7_VtC#BH``3sOuj5W?(OTj)N]F6+Rs$NH',2&%_t@^$2F3<^Kj(m.a5MhsGf("
    "aE2LjZpmH3&U<8p=)P5(v5#5pb:a;7(An'&Hm+1#4@+9.')'J3j$8W-%aN_$T<6H345u4SODC5SM55rN0=T7O[B1w30W/f3S:4`#2)T=cnO59OUqu-MO$,Sn.4&1YD:Lw-+U(0M/$nd3"
    "Qb^%`)?h&`5%Dp<8eo-$r,(:m*K`%`*B$B`=D6:)&3R@RugYR(uZKL;^K<Ji.jLQ/J:%W$ahEX$[7np.iQUV$D<?X1sF$##Y/oFi4IPZ%jXM$#];mf(dVDe;GQCAP5oU5'i7$.F0QvD-"
    "Ku@e%]$1F@Yaw8'5Xfi'6a`&.'X7k9P]bxu9LSW-]A0LPH$WLP)TY?G&%P&Y_4Vul[#=J-]Nl0.8;ci;FZkA#p]Zw$WBL^,(GuY#DIYX1(-(Y1-]F,M4UE^OX:x(#j/oP%kiWX1./MX1"
    "6Za;%)MV>Ph,elLnQdLOHFl,O]Fx8%-D^;%cQC@@6k9a#H1@@@/D_i#5v-I/^tq%,lq<^G[p0V_Q;(@-ZaGc%(-(Y1k7IX1R2aG@G?\?F@=kxX1e@%>QVCuj$`&rE@h.'B#</44OgpV]+"
    "uRqo.O6cj$;'VX18'VX1XC#B#I4IX1xIP4OvC74OWNpdm=`9(#Yda1#Jjg>$L5>##9Eb$#sem&6^+)M#'nTF$v=B:#cT`S$Fesl$EIY##q7Jv$xn%P#B?rB8$xWt$1YY##2xHs$-V(Z#"
    "ZvVB41rq;$=EI12(a.T%i_,87uV)E6%TRL2[<08)L;2T/nRfS#A@JOJc/fYc8*5BT,CFW-fw.l#LAl6#xQXq]=+=)W_qN%b8[gm&<NZ7&1^ww#W:6xnWNcdV(F<E2&*3871op?BOLG12"
    "Ob`G3fiUW-ooN@](Hf.*lhF`#RQoxF8l*R<*EcA#3xF2=R&vF/u7mf_:Jd@JvROj17SVXKI+w&#6tbf(;X*2T$@qZg$X:3$wN(Z#/x%(#]uXL280fX-hH*LuxsqtiXkPg1$%8l#R-O9#"
    "X%iY7@h[>#<aM8$Dd?)*QZPYc.1t)Wte8e$:'Y/(4u-W$]@ofLSs=dV:0s>M[lH12]*c3=bKT#A,/qR';]d8/nAJIMZr:&Xs^6e@.eO<Z#*]'?Q2igAx2V0QBx_kLxgrN$t_Fq$AhIB0"
    ".]QPSl;BPS%%Iq;nc)9.e;vQ#(Qjl$%%b9#8MtE$QV:$%Eg1$#H9/W7JZM%(F)Fu)K9,T.Xc,v()[DP%AkS^3kfeu+1rYxB]Tx=C0.rI69[32Mw_ePJZKo=C=LxlBk7[>6G:sd%T3t@="
    "(S7onm2:D3*8)v#Q5)o&UZlj'`Q4x#I=r$#A(rr$+4#RE,-%12[jFs-;(rkLI[#@5.n`i2=.4S9XPvv$pT)*46FkwAu;Kv-gF3K=%rirSmvYWD_]L/3g_9.I.gfO2^*%SJR>5B,ep1'o"
    "N05@$.dki$O7>##5@YR_F_9oe;6gC4T2@oeV>/L(dSuu#;S1v#.S:;$5o_;$6Np9%-/I8%^/jB4,,,##$7FI%0+/Cu4BBG.2ZZ=pna2(#VaI12t^RL28T[0)jW'B#_LgkhA=$ciwtvC#"
    "R,GZa*ID(j$A>(jv?<sS$WacDLNgi'Jxp2)::ns$EIRs$?[wo%8_jp%j<lfLx>IO(YZt,G_pQL2SJAd)k6qo-AaI&.&w8SYH:(32m@`]IS.KG5t&7lT@Kcb?1)(x&.Gq2)*2>>#7;P>#"
    "oFTP8j7F2_3imd854pQahIQlL=RM#$--/J3<aX=lD@%BJv08P#u7D3#*I'9.(SUV$U?l6#ss^]7qCI<$?qSQ&2PYY#9Yu>#HU3p%FTG3'3Jb#$cRXgLcbP8RE:V;$F30U%V+gQ&,w#tL"
    "r/Cq%TQPD%>0V02sqF)#K[7Q(i:Y@53u-20dQSX-p&GX$.K4L#hS2K(c:x=c$bx[b^II4JhHqf_(_QsuX8Qo@slOi^#kq[GQiIfLGHduYi#XT&?**H4Eo,eQ_@7<$64E5&dZ*T&[saF."
    "I*BQ&igeW%1c68%#fFgL?D&s$dD39%%pAUDNXuJCJ5*($Hou)%v.uBOr?Ti)a`u[0qi(M#xCShuxH7uLlaxZ?xr]XlVl>,c+*P:vR&qRn/wFd2(s6A4+i?8%W02X&`)<I3qP:B#a&c[&"
    "e?pRn:nbYcIxl]>jt]A,R$k0)pJ,##]RCb*57QO(95ZT.JPUV$`OvD-rPo>/@]>>#5RM^Ospu&#H0]NM,j@mMWI&VmeL^X1)Z[&#5EG&omNe##Eggw#-Juu#MMA^+0YY##H*@8%ut_J3"
    "xxc8.2[)22n,Hp.Fax9.F8w0#]'56u1Q:7uBN2[%d)###t#tf_Soou,?n-K3mrEe)Fqle)<,iE<4lZw#k(4i($]ZD*PoAR/Tcse)/K9*%g:Lf#0bukK`kU<-f6Y/1t3M22'7bd(>8t,#"
    "QD^3%r8MwKA5Q4B3)_N(ZUfC#J#G:.(ekZ'gC]HFZ&#dIaWT^TNm`wj3cQ#9-?rf_MvxbrS@oXuk$i^#>XA8.2G]qu;%qd[<qOfLLY?'vH_OgL]i:)-&=KfLZ^hH%ExpDE3F-^OUuOZ,"
    "L/u^,o:[8%,lQS%4Q4#-j:TY,>hWp%ghYc*XL)20XYe9.;oT:%B@7<$l-.&,lXpu,_LgJ)]?gK1g8J;-Pu)L)5.^O0H=lP(j=Y@5[ZnHDt]%j2eKT#A4vw:/R'R,*:>`:%qi@1(5D-W."
    "p:gF4Kjg8.3dV,)g``T%bJWDufMNE#RFF=e(O`RXs5lRnNM_xNON8H6TO]r8dc?IKe;7OMetwCHsC*^71d2p/Z5C(Y&/*ouWqakQQqV+Rn;_n&%BrLEXQ)8NQ@GJHf],h%Yt)s-`-%##"
    "v*Vf:66UB#912$#YZtl&2FBB=3i_lgaoCW-k7f>eu_OI)_+.g)=SeT%H8(##X&pf_DZ_c)c^b&-#MqY#pDXWQXL<5&]M@L(oLBu$UChc)%^@**sLvi0tSYA6lG=<#xx^t@3@hM9Px#Z$"
    "d8O$)f5RS#(3J5`gL1xtqxW<kZH2s#NJ<t8105##g26c$r*uH6Oww%#+S2##]Br($60u2vx5Ct%5oXA+3uLk'_DNZ5=Ob&'faEiCjXZZ#:JTZ,S[]J(R=-eMQkPfCtX->ms8`=%Jc_<7"
    "8u9t@,041)G+'C4BnV7+]mP)PNMLsu6Yx_'J_>buQQG`u=,X1:h^<D#5p)v5mQpiK7qFJ(:mk2$HAcY#7l^t@:tKT%/pkP8`EpiK5hD>#[/Bf_F<W##kd7VQq/_s$=rU;$3ohv#GeNp%"
    "1oUZ#-VuY#BQ6g%J-FT%6rUV$>g5Q&Nh/Q&Wx[fL$+5#%5(Vv##q[>##'o>#4n$8[k%bS7$]a%$[vMB4:7rv#:_jP&PW]e]&F:V&sRbF35_cn0h=cI3f^skLsP,hL7icUAkoaj1QRo['"
    "'#+9g`rrs%>xG>#S7QP/4V]iBIOtu5%s6S#d)%Ycx:tV#(9El$>&IS#-wcw'E/kV#>_AI$XxclLj3`i#.emc$U[u##T5Hx#@T'9%/8P>#^LFgL$BSs$p[$%M9V^t@/vIO(t72mLDpgj("
    "nV3Y5ATQJ(b[Rs$xO>c4?dN=uj>/7_cs3Tun,$l-QwHa1.Z8e?E48VmoH9e?Z[v.:`O*S]F4jd%u#vN'L1LZ)/Ch]Oaun.,3-0#,K=?W*vnXlA<-&v#H]f&(c'+,24/Im/5Z*@5sw-87"
    "n*m<-4qQl$)axF4ZiUP/+87<.$l2Q/>p,qT'F5gLdW;]-:]5:v1;sqFf3h02GL)*s'r1'Y,eD`0UQ$E#orIP@?VG[.S2?8/O+fb<R,_NM:i%&6BTfKY:En1gffeKYW$D`NY[nT._:2mL"
    "HTFM0<:Z(+?WrJVwUrS]p&;8.2G#i7?uSC4lxtjL78K@#)[ST]G]C0_&0)-&H?HgLh-Ng*.hq7'D&Y#-urn,&dC,B(-LM>#m$[s*fwL`$kRr;-Wjvs%B5+@5t6O;pP[)22I60I$)+We$"
    "S7JC#uhTO.abL+*[p3^%_OIs-68ZL:Ih1T/F$1P:4XHpI_'JJ1E.<4:51Wg@8tEr7mADqBQ9f*63;=[5XCV)5+8J]>/YA60Kt'5oc=L'5n_%.4S'WD+cmUg4CD5[?'N&b4SL,2(UV:[-"
    "3]oL(l`1JUh`vYcxM45/%]h$#]=vF`ud%##tp1fq(uutu4'Ym#5S+87^jB@0WU*A0^K+@0#>OGM-p'Q]oZ?.Ms,l?-raPlM5MCk/OM2,N91S>6dY4)#kuP3#^*dn$Mme%#$=hX$76u2v"
    "W:mm&;',F%P]i5:%7>Y%EpZd)'FWs$Wd)&+hh#C+6nT>(*Iw<QL-(,)e5p2)1&elf^D*.)NKM8*_xwh(n#_q%E->/(w'7L(:r8q)AL9auf8@12db-/37f<>5G@&(#w`>8p.]Ms@ch_4A"
    "6TBj(r@A[')^9D5^<Sm2U;2]-Yh)VAKP)<%qH@X-AWA&$pj305_bZMEe$%e<J-wFD(=BY5B5oFDH,?$[Yj@/&a6(v>5+rf_='<>#,&*5AD'#E37t)s-ML$##]JkCj]C['&K%t8&.ORP/"
    "L(<6/l0%D$[qv&MgEq*#KwcA#L4]V%(cr_,,2IA4PV=gLM7t4#EUvC%23(@--Po31;7ND#3/HU76f68%^K^<(4)v@=,:0F@F9Fs-Uaqo*@g5Q/s9/5/6@mk(2F)^%+>tQNi_9,M-T?a$"
    "B$I>,>3qs7]d(t.I]XbN)2^)v2D%bM%l0KMY<CM#DSSn$nPOV-,=Eq._oULl$0qRnH5NqM=b]X1/6?e?aj<T't,,B#tes8&vre`*TBndMVD_kLdlDE-e%EE-&l_j<spL_AdVBr7oQCAl"
    "q$5_ANB@W&I]oS/p9c^Sgi?(#n-#'vRx(g$Lg[%#T.#a7C3;CXl;T@#vFBb*Dau2v3G3v%H?<@,jvOw59sFZ#/i@[']<8W$Q>/**RNtT%3hq7'2>jB4p5(^#nXX.)sJM;$d`be#OUR6/"
    "/Ya/%wn+DuV&+22spcn0x.i>716ho0L24Y5.R-@5%v>)%]#G:.9hhxRSF3]-d$H+<=?42_=S&8U2^iOfH:RiTFh21=x/oT:%KpM9i.+TUBl&c&CtKLHA`N+&T,Y:vb-#B$0oV4#m[&*#"
    "j`jD>l`j/:P,$_#Ut$W$Vb($#t#h]72j8'+hW8w#:r6s$At.?#1135&;C%w#Cd?)*Rn;v#F<T6&Z=Yv,:=w8%D2j)'a%@aaid'-2UrjY$BPgk(YMcu@piI12k>pn$u]9D53x`p$EsnO("
    "NIHg)>+p=%Mj@cDVBK*C7_i5$NNRoe_U=+JPh5[j%;cY#hxEs6_^+rp,n_U#jK4>Bbh#0)^F?p.0'?'$$eXW83n_U#U,FcMEr$E5X;U^#WFDY&6sj48Qdof_`D+&lc+R,tRR6##WX[s$"
    "CuPQ8lc?N:Lbb>5itVw0:F%##bxGxkSK4x02S(m(>D*#5fw*6/pQUV$r$HlLpo9KM]^UAlJJPYG^a^#.U]W1Luc7M+S:gU+2sP/(=U=c<g[*>$[&5&#MjG)+xB/T./rx14wh6i0Z*f5%"
    "+Al6#jq@3MDHu%=x[wD32aw8'D6OA#U<7h,#Vuo.B)X,4a?tq2D90T.kq<on^X3B-giSN-ix9S-&wN+.`-s#NWN-##_0'6&4Fv&OE&0nL_^IIMHA,kMmk'AXSTr^#2$;N(ltcV-4Q+ft"
    "iZK%N*7@V#c+f+1-0O?#Cer$#Wd92'QiXgL,(S)*id.s-xDdD-<nd2&hot/MtJ]('1XtRnCk(qr'Kk$#fIIZ7l<pm&-i_,)mo*I)r@wW(gUCf#j11$&RG*p%%'dg(vJ0FEv6]F<p4Ys0"
    "/@%7ptW&-1**387aK4mL]r,E5DpMG)xvs>-.-@K)I5^+4xbYV-RXfGVen:?V7$@ouv>7S[L'SF#`UEpu'3[o#Pd,F_cXpi']$LED:2.OtFB#`/[[^%OH1KPfYx*H;-Mc##Ske%#i)^s&"
    "(jDB4gPrK(9P=A#H0=]#V`<i(oG)O']gNL#fWJ@#-nmeMp92`+G+ID<2_]]#T;u',P^)-,6<$4,>CFp@)nI^$`4d;-^Z?a0/[)22q&MW5GMxZ?o_jHZk9[hCs(5Ras`*pu#XpMu)S<j0"
    "=)7mT+R^h6+eMM2CMWm/853)*$b5)*(_aY_GJRP/:-I*$6kcR$H)P:ve=(##>w)N9w2#,2xI*j$^&&-=QL-##7MG>#>S^p7BLP)O5mOf_D8XG<LtP&#Q?Ne*^W<*N]Bk/4180Z$=7AB."
    "g7/MpK?>j0gX$?#ZY0'%2r$s$Qt-dMaYb?/mjsf_#@,GMQqt%=-GVq2V9%<&ms0,V.Jc>#ikV'PO&h,N=/.r#Cv(,#i[Ok1$DOS79c1v#T_L*4pd.lLbAdD-ro`A.Ws<##UaI?$:f68%"
    "i6Z&Ob+@3#lQ-IM9bZhM/=HuuP%r8.?l:;$nM_6/MH:;$^qv&MV+D1NxSW,#T0of$t?A%#W59o8_Q5F@+u7X1^MFAl@a732h`D0(9:i_Q0QvD-EoV&.=fmiN<nR.MOCCp&^K8w^fn<-m"
    "HD,$#mJo]]feEA+ci-n7xiDB44$%WZU=71_-)_#73mFf*NvEQ+cv,R&0U_dMlp@HD)6IeM`)AX)2M..2pQS_F5$gX$Ecc##tF^U%<l487q78Gag1N`klbuGGkn+Dunl^t@UQQV8NM:6p"
    "-c_<7X^cn088JC#vJ6Q/xbYV-XrUv-rs,T%6*]F4_R(f)hUa:VBOn*$sC5J$m`6FQ0'=cD/71&O.YeN$oo9h8B8TXB)#'i9FvvjHaTU;$8?:(h?+P:vDF<Y$*:(s9Fs.V#=9C/#5NBw#"
    "nE[5/E/8>,o@Hp.eoS+4,YjfLoQ-'v#[wW$*R]R0w,>>#`eg:#M7b(NbQR*N^KE#PqO[3#o?l>%4LX&#2vdX.JlMW#gX(]$@8MB#M=<Q&SDCmK?oM]bIDNv-+3TX1'Ue#MROIU7B'(,)"
    "hX8e?TtE9B/QvD-bmV&.3Th28EpB^#N=C0M^sGT8%J5F@ew<X1^MFAlliVF@9b#F@<-mF@2(e;-[DOZ%fHEVQ0:o20-%x+2I-9r.1[u8Tx@x^$Z[t:91;CM#&]tZ%LFCb*]=[bO,W%_u"
    "epuf$X4(@-i#O);T16O(nD,##=1.W$Ti1$#2:@XJ01x_uQ-1PMKD`lMLM1`jY(kw0Q37k(iE:v--5(02dWI;QBj:.?fOdrQLu?p.<3PN'EFdpK=I=%v?_5n$Y`''#8+-D#v8C7%XFmU7"
    "T<gj*$?Wl*n_*a*Jt/'-x5Pg$n`HSq-'7r#0fTW04E'[#]k:)+r3Vd*GIw<-M6J'.H[wqLQAMPM7vqU#9>Xf%5ogb*tRCb*NA(<-wGJ'.-V7+MxDUW77,.GVtx_]#&6A&,r$V0,[OYc*"
    "g_wT.H]U+4TcYg**XJt-%)M*M$'$X7FTV:3cV&F*sBG;-'iwb<q$f=$7Vc@P/[U#A_N4*>#pbP95PrY6gr[APR`DW]aZ5x'o=$##G7ligPb*s@LtP&#L;J/03Rg''<-,n&DTje*MdadM"
    "k$,gN-ohU#@bcD%Ycmt1H3'=-[1&'.+HhM8=NZX1*gHT%Y2Puua@iK$QJn8#-Mc##U9Os$2G#i7')aB42]:Z#5c:Z#Oi1@6S#kte'6O#3Y.G)40-k-$TV>^tVd?.MKwvvC/?GqL9[n1K"
    "t>gS%q3Vw9a<`v%.S1v#&)###b8I12Q',)#kCVi2%vK8.`ttVdA@kig$FDVQG^t7I:;aoeaJX<%IoTm(6uCZ#h9u419%r;$_QX,2v&fk()GA.%qMtcM0E4jLa-`oe)M4)F(uTk=+0J##"
    "OH:;$?@l6#3f1$#E@%%#XhLA$Akxi'<IET%X]o](UnI[#::%W$i8KI$6<n8'Ae*9%T<^Q&%_X6&jW%LC.gRRA5n8x#6v16&c[Zs0HlAAu#jiKcZZ4N33CU'#WEGDutu^t@_B42MQwL#$"
    "YR)v#CusY-4lj-$VRV,>ogCG#-$&$Nsh,v,$ew4S(=[e$g]=Iu'NDO#SeWvM_8uEt%3odu%:o-#kI>M9Q/N87ZxMJ(gbe&:NP10_#oEZ.vFe(#CI7<$7S>N'i-)>PIAqW.*h`>PY-.n6"
    ">B@W&Ld%O(6)XGug2XUGa)387bu`Bu.`1@66u<tUZoEb39M]L(3xd?%`YQ$$[$-0B.iVW#,;dfXV2Son8iBB]H8s1g9998N#)>>#WepJ'd@l6#TDpkL/'2'#%J9sLlfd##>4g(MRH?90"
    "Wc//L)B?_#e,R)0M1Qj)/WMs/4aG$0OL-##=wq*4fhZs0U#Ut@]G:;$<h;i^L>oig=Q?D*t#BJ1^Jx5(/3Nl]?RFp.wlw-)/<_HmlUIb%PW2f)Xi5N'G-8$#>ka9%h-JT]+Les*1XeY#"
    "@>Y>#MYS^+b9>^-LjcV-9kWp+4%Mf#CiwN':360LLBP3'vi_g1qOvi0MxO12MplE(K6ST%gNZh(^_,87wC$7p^9*K34Xm<6*`+12I;jn0_$9g1K@9$c1oH>#Nc$s$(=lM(6ndG*xF<m/"
    ")2pb4,R(f)H7Oj$9em1)J29f3<iXI)N4;$uLU>,;.p:/`[L%H#0Vs?6#Je^7&?We,=47IfA/t-f''7d4IQN]$0Q75%L%)tG';*L#iP,_>TG)IAl.DXX(qDXX`MaF`tToRc=4&##-S(vu"
    ":tYn$WFX&#N,>>#<B1[K6jE.3+T#a452Qa4`.^h(;dg,.HxlLMWJ,b#?v9D,Pnd_+'M5Q;pLZ4(U)To8Ft7BV)5mL^<hPp/A.4]u=sNrAl2B4VVOnA5X_)D+_@3B#?)HwFX32oZ*Bkp:"
    "h1?hGFUG+#&&c2$u`kgLE*DI3gh5g)?-X:.1moF4Y>&.=&h_+tS_03<xXr2$+T,YCZS/V?CX@>#e2Yx=KHsi^b_Xm/pS/J36?B7/:o0N(v1ZN#Kx]$?h;2UCx)gA+cO]qk0u50tGYm`G"
    "$Y7`%.M0f)4IuD4'G3]-4j`,)bFSqk0x>0tGMZ`G8^Oeu@(p?\?B:KlFXp].*:$nO(Q^Ie64:dsMb;vo1W;K*[FCJ[blJBekIm4R33RFfUfegDF5b/bIdLVv#n&H6jMCD6jnPvan,OHH3"
    "MoMD^nx89KtBH?9m,Ra^^qcW-$8D_&T*7#,H_.uu>$cIZMGfR1O/+,MKugXuc^@pS724,D#-3L>>puX$VUwNOv@q8.5_J$T5?Z&4`<>)M(XduY<#=luWCw]-YA:C&O`hV$+-DnEgP)7n"
    "mm4Flmd:]<ZTnA+ov=Fli2KlS<eJL>9M'eZU#3Z-KG&H@J8cY#f`$rS@bV's2'=;09d&##C,guubY_$'bpV-MjU5:Ck:(:v3M+fCc7mh/>RUV$<P+.#+V+E<b$Vt6VOr_,kNw9.C=S_#"
    "#F/[#+uwEFII=a=Z@%EQ##q(F.cbSG,?[j*R9IPjb8U&8M47qu;rsp#]mbYWw%;*ff@uu#.41AObS?Sn-Lm;%(4jEI]_G)4,o1p.NfQ(3*.@.$5vH^JTc@sqrT^Y#a1(##s4KS[a5/(#"
    "4n@X-$ctM(g;/5u0J#a**1q#$q[i8$@0(H/hXhn#3nMuLo:?>#IB%_#Sg;%$6OQC#D&,-O=F5gL^C=;.`n?X-Cq:B#q?nS#*3C<.8>4rdq*l8])lY8.G9PqVN#ac)'*Tv-[B'Y.G_]Q#"
    "098i^-%GY#YxECN_cBM^j2:_Af(BYY/gCGVfOeG*'VRK32E=u-c6.Z5>U^:/6kZ?SqY$CR$afND-TR3Dk_xKQdwM;MdD$7C/XHqCIR9qLm6C<fTI?`j_ero%AJt5(<LEp%*6*T%@_NT%"
    "0XB+&SF122S&fk(40_BuDpai2KC6g)lk2Q/-:I8fOITJF`<(5JKlJ=KE=:c=VkWp.L%H5/bK&_S>BO(j>rvx=,B@W&_nZ5/@BY3'Rv9B#[[KfL*u&0%VOKAX*x]@@(hUU#U#+GMi/bP("
    "1@S#6KUDBO-8^)v`.1R%53(@-Ms7^.4$6V#$3s60u^[5/k,KZ-H(*Y1o@Vs-l040N_UOnMtFRP/]M$=.5v.-)&S^J;^,&djw03;HD#wY-D#[#-4b.;QO4O'MSdV&.=(w2NFTX/M=K$<."
    "b.w9/iQUV$f^.+MsuBKMUcUU#-4ZX$-xB-Mjg)O'`HcF@xRd;-vD^*=WX&,j-*gN-(vR#/m8#<.YC*Y1+>g;%#nV&.7:h*8n:kM(7_sJ)>#wD-Qf;a-;dHwBL.HU7hRKN(uF)20S#wD-"
    "tZvD-&K=-87E^#_UrWF@js'TR`'B)vKU]=%m$V$?TLNJi&b[w^Q*Aw^'mTs7Nfql2<rU%M3*m[$tXwx=Og+W-BWAw^#c%`,63WdM?_xvMSLt'M1(gN-GKvD-1grB'?qBw^VjVcj[$&d3"
    "V$&d31gw]u^A&]$YDG0.,QxnL?H6N^&Seq/W@2e$E)>>#xrJv$]:1d/33rC$%p@eaeHj;-O0Z`-pEE(]7D(##01h58()#n0+i?8%Sg<##B;l;-8n+K.H%VV$3A;6/MH:;$uH)mLhK$+#"
    "]2SC#JPh/#-0O?#gmBB#SKA%#Y]$=.7wbf(QHl/:&Al6#MXI%#2pjg(#^T2#W6gm&_qN%bI-16&tw*mF3r-W$Ib7w#?Or$#uCjiKOg2&+Ix-p%x_/jKU^g:%@,T02+TYA6dOBk$jI&(#"
    "ptnA6P4_t@Ah#@5-9Wk+S7JC#@]=$%*EsI3Z0fX-RmfG3^b%T%j1u8NlVUi'lk'7N2]Afqn7(Lu@Hn<$x_bm#&l0N$bbhuL0H*YPQHC>#7`###7H.eZi&^%#sp*)*&0)-&1kj0_WNw2)"
    "/<Nl([+)Z-p8P?#xlCb<j[bgL1>>N-Zr6F%#:Lt%(&.n&6#UPSWRM$#o]6I2X#%1(W)SC#orn%#[9wH)#m__&^,Zr%YYP)1nmS@6RwV6p<L`t@*?h;7V*UBfXG9t@;,cs0<e6h2=Srwe"
    "jc:Q6K+'C4_R(f)px](EYp=cu]'AI#k?PYu:/@k[F8mL%jmoHA8;Q=u^J>4vO/_'#+f?S%ZZj/MY-A8%=KMS7ipfW-tU5QUN`=%tERXrd=R<X1/X*##Y54ciB$(,),Clf([F-##JYQx0"
    "k<fnLLqZ3%xcD&.)L*s8O<d;%M2(@-7DxY/5Y###kBPAXW^2,jVeJe?'uAe?91S>60H=;n`xre?PD@e?vYLX1)%Be?7X2#PCGp?.TfoigVZ;Gj>mK/)-TAs%bRTx0M),##ipfa#v.=$M"
    "<)'p90_ZV)qfuf(@@@[#Pm5R&QNgq%V2dn&C34W$EJmc%Hv6H),Mu,)oLvi0.)XGugmuC3o@,878c+F31_4T.8Yfo@m`6.%6k1N()aZv$]WD.335oQGX0Y@0[1cF^:eg*==N%RKjP)e$"
    "aa<w7c9k%#1K)uLaM4RM?+B%#@H=-*U[%%#PoO4Os.Ih)'-uU%1;BUDZ*)KCjt2&cm$LRNAxw;72[)22La55O/2tM(vE.IGW-P@0W3T#_&1cv-$IZ(Ee-)?$N:8CMJnfS[dGX:v-[S7n"
    "+hUs6#-BJ1Av@n$lm,Qh&b[5/T1g+*^=hl%$q[Y#f,#A=aA)vc>H_c)Hnf6#`g-n7UjDB4QecY#vZ_v%i9R##LsUk'8i)RSTbrV$0]:Z#$;.3'nRw<$,S:;$YY%l'pT9j'<0rhLOw'f)"
    "3h*H3KNSkL4laI3`2en0dlOP(*`,87tv3>7%xfw'_esdmD=;8.2cg/)N5lq>OhH_#*F7^J$ZE;-c+ZlSuxnF6TK$#Y#k'v,^d]CF/<2'o5=:uY>+rf_3F2j1`l/B#-Cx49H,fp&geQp."
    "7xMY$l:9uY(=r'/<+%(/E-R+QJnG+#J_-0#%-VC$Sf1$#9`eSfKJ&i%D6B6&YF0DNBh.%#?qap%8$+w#NC%w#;:iv#:0u2vb4/O(vH<v#E*96&9Y>>#Qh/ba3@aP&-?eiLi'E?#ln<h>"
    "`esT%h?Fm&ON$l$r3H12lEI12Io:D5QjlA6K`u-$wZ)22[=Y@5_#Xn0$@9$cK:oe*[Ve2KI0/(#U-Ia-`E1K(%P`S%nKkA#U@&A4STZ(EDa,)3G]W(NeR)ju8at]u$f7du[*>>#e:@`$"
    "]C[8?l.GDk5l['ok'oW$),m9%T3t@=>3s'/Ch%##_cg@kFt_>$+#[F.1wx@=9a8F.pY'F.-gg@kNi,(/vg9U%wH)^%J0%D$Fe8a#bQa.b*$%V#4xkv-c^AO9f6Ba<h/KlSLM,Q/l6CA+"
    "/Fpg%+vMB4%#jIqI-16&Zp99Rm$;D+9AZ/$YM[D*`D***'dN#,^a6&+%tCD*n06G*@,T028Spo0E5+@5&/i>7&9<Z5^R<E20*RL2]m-H3eb`$#Kw60)[8+l(g$u]>FHd&4V&)E#sWbM'"
    "=TA%k4wfG4nmMdup8'jufUbi$Ige.uro$SuriX`<K(:J#s5FNH+^Tb$M-SBo@r(^$mIN)#kC@Z7E64?#_v53'O/V`+>Sdj#tL9b*:O=I*.)k>,XWQA+ICQ%,pk8qVaE:VV:hVP(3P&UV"
    "ak5^#7G?L-/sN4s#evO(1Isi0XB?sXdT9qMAjpS8<uF7:p3VG#IJF/c&1W.U3)X(E$v[T&^NP.qhd,Ju4%h>Zq?JW-H]Zxe*kqq#q4*j$oZ^Y$i/8>,gs;m$Z(rs-__1xLt/e+#+Pc##"
    "5^'d%M?7eM?S^W&t4d;-/a0W0KGAX#L/HU7(0*/M2I8a+8>-#/-n_U#J,FcM]m5pL^Do>/lC[%%N-=$MXodLMZ.@r#Sr;U/<U;##J4Gi(7?p5/K(`($V?)mLC8MlMSm4SRGw1B#iCS1("
    "?%BZ>&SP8/WgTs-e(ArM(uCD3OF-##gjk6/ZnTZ>lIR]cp?n]>TpMKi,;###,B@W&Bm;hL/3<i22T*'mM7-##$%Ug1&;qf_+hn(<Jq=p%%p86&t;XWQMNLg(+S:;$]sfE%Ds82'ME,3'"
    "<Ze@%CTbcM9HIN%fG]t@8gU@6Dt&9(mx<.3l@,87P2L;A,b/g2=bsJQ&W,<.DNAX-C,G8R-ibtav;La^d0P5A_/1rm@b,lf^H&s$S';GDR#kF.a-SA=x$,_,S;SB(*mFsQHEY3Db.ST&"
    "4/PZ,8cQj#CYdmShlW**bTgm&hA*.)cb5(+xLpm&lT,3%0/$=1%&T'+:OtG2Zsa/2=sCcH.[[?TA`1@64mU9VnC0V6U5+@5:VC0Mf/RL2><Tv-/aS+49,B+4NoL)4S#8;3NT/i)0M0+*"
    "nwk%$+(B@TfALvLLZbA#-J-l[[c?H3DuT>?j=l(k6bn&c_OOeutH0o-gHp%#:5w/10e''#],>>#[Rgq#hcd.%Ge[5/jm<',_nwb%p6/<-Y=C[---HX1[t[s-/3;hLX33H*?&>[&seW7n"
    ">YBAtYd2,jtWC?%g4'#,<6P)+K#wD--b0W0OaP+#w-HU7uQ6.Mx.L+*8>g;%'nV&..+XCNG1x.MS(F;RPLpvNRFk'MVRvD-<T;a-9*ZF@bw?>#R@5P8P`lf_6k;J:o9<)+DCmY#>dtWQ"
    ",Jbe)pT3#-g%Z7]MgP7&BLiX-`=*m&.krS&GA[W%vZ)22jo<.3#jlA6/_bs>^>tM(4w-u%**HJ(%M4I)AJ.i$hb`$#>6vl$YiZLGROaF.5N=xk9vl^J7<[xbB.Q._O;oN%2Y<v'#$/##"
    "e8)fL%2XFI96fOSgLFuYsrv5Tt1u^]ud`fuu%r;$j?O&#X%J[-_Et]#n@N8&;&aB4H@<RSex/B+nVMk'Z#SiL.e4E*U1&E+$D6eMv&sKCnvV6pYVqA5^[G(N%9N>716ho07V)0:pK1a4"
    "_Pv'&VEv6a-eU&4WR+>BKH]I)VkYN]X#&KQc1<PSEaat_(JXR:1xkpc+5cI_58FM#xYBW$q6w0#TlQS%lk8a#[oYlLggd##I#Y]:gw50)OF-##7%HM9Vg4_-QR?##l%)g$e4(@-+>'I<"
    "vatA#[XjY1I)###F/.F@2=SX1?D)>NJhMm$ArYi9t75F@n=C,M,mR2NLS.(#Tlc;-;T;a-[1wX1]),##H?uu#b+O9#<3&W77,,##s0R?#/`U0G8Ut2'h:vi0ed,T%a&MW5;6RL2uY5<."
    "7I@X-FWJX-NMOw5OQxgui`jc2x?>UtQ*ntuB>3T#$^R4;TmDvY_])mBA5JCPJ`0Gi+9%<&SwMv#'U1N(-1TfL'4F6fg@ramC#wip7*-p8DW7T/G5429+CBDPc70#,K=2K*jlr&$B>u6/"
    "wI^.=YDq#$F>K)5N-f;-B+9$.;F.lL?PQ:v-F)##8A$E<H(B>QBiO#$moaj$i6'Y$cN?D*:v(2'x)6onubpig>kIJVN02^#8Vj^//B0w#[W$$$W4#T/,TtK&7q:oe;e0F@+]t<9ts70#"
    "vM8m0OD_w-w^eq7&`-N:)<Gm&pSgfVKf5&>V_)g28k/K):>.F<si?(#.lq;$6Gsd$67>##rdMT7eC-W';f0reddml$5vb#^>Hnf_$5qi0%)5uu-K.;ZRef34[FNP&pA_#.VY3Y7rLE`#"
    ";L[W$u->^$-MY>#'5YY#[c8U)[uhV$m]aI3P:=w?)t%t@]`/@6I]@12Z;AB.Jc.Z$w[0^#b%L[/(lp[t,N1V?):d;%YUh`.SAJ[tEi0l%]QLn#.?.a3nf53#@D5R]8ruY#==m(^@iuY#"
    "&HNm$avi2_<w>fqHMR3XiB>6#*JlY#c8I122wJs-2]i<H6x^#$upMBMVuZ##B1hb$Zf0'#wr$O=%%ge?bFRw#uSQ1pW(D0(0ZD&.PlRfL29Qj'$aSm0vr+##l9M#$MK_w-lj-#OxQ@U7"
    "8l_v#BRr;$nWxB&<cnRnf7pig&Zu(3FbgoL=VK>,D&sx+YR^2`i7Y,4eC$)b[gg>65U+M>ZcY>#DE,3'+k8j-vA1L>(@D*K%KnA#g,:w-rflP9FK*L>I#5>#.wTJ%-?UH%@m>3#)_;V7"
    "8AP>#?)OjLWT02'<*)m$@+e;-8n5a&q%LS.50'O0kQUV$sSLn#&EEjL.;-##l)m<-M)]r32w`V7#muJ$@qNX$K7Lf#X@;s$d-[6%u.i>7t9JC#.q3L#q3x@uH]fA$TSgetVt?]->.M'S"
    "^1dlL[b?##7:5a7doIA4ee:[9+Kk;$fF$v#:>uu#KLJ>claR;Q;UI@#^.s[$76u2v)UFs%lTNv#2%@s$uxkf(4VWL#,A>##1]l>#DLQc$EnhV$QZ4N3Z*ek(gE:L;Jr'BujOVD58C&f$"
    "OM>s-VQF.MR#;@K?]wj5oWlA#W1.s$tjQo#IGvg'm]tf_P%$Z#Dv#5JNBT9rE=g+M#^Y&#>GSVMjHWMMD%73#lQ-IM)5V'#3jN(#p$]5%H,O9#BF.%#on*[7)+<mLcMoqR*U487j+gF*"
    "'mK2#v=D,%v.Al'l:bx#HBp0MDqp3KfvEaER]c;-AAAv$x]MDuO4_t@r6/kLdUG&#f(PV-^R>^=m4^+407lM(g1tM(E$)G#thjhu:3)kutUaL#_t4xS3=eouIu+uu*euf$nHqfuKr/iu"
    ";WVO#00oH#6tChbP8Kiu5'oH#ZO7%#wn]xX>h;AFafO21H``]YZ^*j1Es%B#4-XKlJ%nf_+Hcu>E=<Z#Q0e0#(rs9)+H.H&fr6s$,5>>#=O7@#1]Lv#-S:;$Xn-o#F6M0aL=re$rilA6"
    "mtnA6+[2WJ`t*u1l>;@KkZ[[-T)O0PiG3^4RJO1^h+I`330(Po$Sqwk8[V:v`.(##@S^-QMUg+M#n(*8MJ,h2&>uu#G;'##CiV9V^l68%3s_r?97vS7haB%QrqNhskO4#Lq7'T7:l*%v"
    "vSF6%$Q(@'s%D?#qAN5&+&4=$?'#3'KSlV$?wfm&pLvi0$QN12xLO&#)v_E%C>[BGp^Zs0S-+T.$n@X-N;jV6]i6u>q-W3u5M%9_$GBJh8jvu5Sg820-rCG;6XKB[&&kig^Nt(3m<$B#"
    "GR$_]&-mQW0)LS.eQ48R1YUV$a$R0#GxBf#__.T%H-Yw$#*387i3t&3XZi?B/-k-$[D`$>x&-Z$nIu[$_-ZVR&_H&Gj5?e?Wghe?9]*rLh[3$#qxV#$CI@<$rnQv#ol#N#J7iv#8k6_#"
    "#a12'CMP;$bIx+MsB.p0-E3@5Ml*)#rfYlLtk+M(LqNMMdAH>#EB(9.Kp,Z$d'$C#'v_C$b.$C#%&Mc#OLjD#u_5N#lm-ouE+;,#''J/23?D>Q2v,a4D3l<%j*pw'D?'_SCuUf:4v*?,"
    "JJmBJu;0%#Vw:b7c`(0*>*oT&FH#n&7Dcj)wl0</9:w8%(_P4KhZwg#Ew*X$xn,r%MqN%bnqSF4P07m0R`>W-QJ?F%_<3<-Ni@v$4/j:%W7pC4lIVV;7&.m0auUI3+D0V6Y6W6p**387"
    "sFZDm:$x*%rM-Duk9JC#M9=X$^b/Y.Ihl)4m/xF4/]NT/A[Rs$g^X=$At>Nu>qIHu'0mN#qw5&JTgwTCkS2g:TX2WuL$Nf[(@b]#RJ9ktN/CnFW6CAu;i:H,E:U3W#)>>#,Yqr$SA?#6"
    "qRBB#vp%##oN4U%ke[%#N3J0)`/H/$w17W$ZX%w#pn]jK3M<5&Br^(+urvW(((B/LJOQc$%nhV$g3(Y7LnGD4(7G^$)=*l$dA]t@fKKP3uU)_fIQ7$ggnH7cv-MkLm*tJ/f.tM(li?/;"
    "7oP,*h6^K1LA^+4D.5W6:MtO]8Yl-$u:<R#c4qYAO@7B@V5n4$OgMsuM,wK#N7MG#]c<pu?#'R;($`x+%;gF#`.>>#,?&##NFlig$U7w^.b$K)woS2'QIn:RlV)4'>19h(TK'@#b>%h("
    "*<PZ,dsm]+;L&N-VYeF31W*&+&o=4KbK'#$A,T02Zb-/3ZlK/2,]O&#x2.M.t`>8pPsi8/f]Facajf[#-eJN-H4%q$CwqhL%C<D#OCjM#pabI#[^5eQgs4`Eb2;/SYwoCENg(E#gm(&4"
    "3;*]XqtZiuT^]@bU<oN#5d>kaRflN[/1('#[#vx4q`r&fQQ0W$`sO1:D*JwBEaZ'v^j:_$]oBB#<.>>#qxlj#3####.9kZ#)N_^#ttoP'Z.[T.Yh%T$O9(@-iFC[-J9XX1+2tM(lPOV-"
    ")7<q.VMQT.bs^F$7]=kLHNvD-e>(@-aG_w-PmTa<.*7m0?hQT->fQT-&?C[-1]<F@*ts;NN@,gLK]3w$m`2''rsV3OC-I^+TIbih;7H5Tq=&F#XVc1:gE%E+(:L-MVCB8%G/5##$2Td2"
    "$Dac)D>du>6Q4P([c%l'J2w0([ls**i#P@5N?pq%$<,+%[.c58k/x0#I^r0$#?n0##3x.M.)TM'erOe#tBO;pIx^t@Xpi?m0<Gf(HW'H>0/,NB<0eu@6F&s$a(oh(SxMT/oYY&#k8;D3"
    "/K=c4m4^+4);Rv$@RM/:)7=#73(Pf)R]_supI]So>kG<-,XlO;imXC$H$*fhrPcIhYm*BQO'fhofpP44]]7;OqjQ#QrYJ(MUb4?Ik)]cieL/,)KY);?[)e/(m1e(#X`SI);qB^'4:j,%"
    "sC9J)93WuP+R6.M6DIN'HF[t-)4-$$`*vr%oDD?#nd;KC>d7BuRwV6pOk26p)p$cc$aDQ74LQlLXu'kL+,WOB'bRU/EoEb3[eX@$x'Ip.N29f3G+.N(hb9'fw2_:/)4%#M@IlLe]]?K#"
    "k3;W#6](G;>>#b7&f.:;@Kh/$ckIjS]k5guRi.cnBPt,A:SndD#a7@AD5WH@&9[A'$,GuuN?R#32gt&#IErb7bR*9%djE1#)Rl3+Z?]s$k$_DNE*bX$81@W$/cCv#677W$VZgu$76u2v"
    "8LXh(cLn20D*hm&k1.<$#cw<(4L$iLDI/(#P05##[FOcM0gW=&,Cno@hc3Y5chZs02.FCM6x+87mbRA6`;53`e^wXJ#d&V-N`Yn%cDT/)u9Hru4B=euJI[Ie#N=&uil)D#6s;Wu&E)4#"
    "qtUf:rkIm&G>MJ(](9>,1u4Q1shSM'HDaE*;2a$#2](g(A0B+*),,##QgU0(%H9I$]fke#26R.MgTVgC;JYd$IKLW5i9N78')c5pnKAD5NCEpLA<S)*_0/%#Ddq%4qI]u._/R^64/PN)"
    "3wiuPM>t0ZjLK.hpY_=c-V)ip(;Qf1rNL`CWnRE$l0f(vMTMruZt/%muBFw57vw[uCu)3N6AiuPK4=.$=vn+Md@6?$jJN)#:h&h7%d2/LF#*E*7;Zkp`.%1_->fv%3Wf.*u];O'D6K01"
    "9iAC4:OB#(H:Ea,`B6.3w[e#.$Je(#GNp]#a5Ih(OZDdMg816&1[4&#T.Q##nCf+Mg3DhLcrJfL1p5;-PvU0(LLh-MEZgO0;%TC4('R.M8@:p7^bZp.1p%O(dhZs0SfSh(,x,@53.>n0"
    ">[/Cu4A@92a0%K3*<ae*fD]k+)&9t@VVRL2WQpv$i<d)4?9w)4$rK,35Z5<./M0+*4cJ)=iek,2WPV=l,Y<rdAq#s-k5^LTNvEtU2BkVMFK5RD?)gvUBs:OP*V5r5jR(@mOZ4K=+.u/1"
    "q-i($*Al6#g%iY7Z7V?#C>Om+&4bA'p`E**I0I1,g@wo%+kM`+2:Z@,i&L*9jqgN(v@,87x(MW5QG/e%3Iqc)nM%d).Ep;.'&9f3iB7f3I*vM((CQ^paQnfA-d<RGvSVhiAVO*qU;oV-"
    "G'nXuZLpJ6Q9t8K`@QU#u8D,3d`B?V:[QXfX-&_S;p/guv@)S$s:r$#977_(ATu2v+bXs%Q#Wr&>l&m&-Fp#(MOr?#Z_7@#hG.%#K`aT%#Ypq%:,T02`)V02R`J&?J]Vs@Z./(#pY_%)"
    ":V`&FZYb]u2CI=Fu-AZ5%w:(vmnhV$Vb($#KXI%#(b4<[51HU7*3j1)`R0@#jYiO'%G-)*<_*i(Lws-$Ucke#)jl'&.G[6M02nlLO-t(;dj45pI>E*u'4TE#`GUv-RiXV-`^D.3@/;/("
    "1,/lf1HX=lW=/g(==wA@O'CSIbl;W?]#pig6BxOSxDg7IxjR]Fh4]=YR22NYOIuVW+qOm/q&_^#V4t9#%p<cb[e?>#vu+##t&tf_F6&>G]<0W$,AcY#/]:Z#,J(v#sXqG)6.Is$$x%5#"
    ">`####Rd--e]Y##+]U;$Duqr$4X;J:Iv2)N8i.R3X4@W$W<3L#^n:I3qgIO(sZ2Q;7_W/2T$r?]d]6MNGc/X%QYQx2H'$0(+]Hl-likV-Lmg>$SIY1kAs1G#,Hm%O$ImCE8XXiKFZt7I"
    "rJ=DjaGDG)^B+E,PLr?#ZEO]#)YDK$(LHeMLh1GDg'LdMaa+KNsf.p0$H2)#nKAD5s'oA6rP(Z-N<Tv-TawiL)UBRJ'S?T.5]:DjH_RpB6lV(Ehmn39Z5SF#7FnUCVrTM^ZxK$@b0^j#"
    "f5;]$<w%T$QMY##B?I?pJWo(<NjLJLtYKM0.*8N)Ofv]0i_/sR;N'tQ%.-c*,la%$Z8:m7Plx87fonL(mKvM(7?Z/_6:Zm(&7Yu%CGq7/<F(#$U=sH*ni`,2I[+F3I9&7pZ3Zh(7:>n0"
    "L*t5/VVRL2tb#Q8_BYD4qJ_202]39/9h-H3/=&3MTSXK3t,dqaxEvW915JY9&>Wkh*G3XWTlXM'J'aI4@9UkE_;:NT-f*G>@v-kW&*8N0xSb$6C#8;V6Yw+W=((muiSGl3[n3R%?>4s6"
    "Y<[D=<r3N)j8B5SWKOP&rBr2)sVKpREFI8%5dOj)s6wu#.'x+MC7sg(r9nY#pk'dMq+ew'0vWe)t$Tm&6<n8'gQwo@ir<+%9d(?6.TYA6pfe;.*g$i$Ut@X-DaAhuV<eouVP'gL$8CV$"
    "HBt87O[ZY#`XuCEt+oV6H#[`*VwId+<+RqRQpcN'?beL($l#;%<YD78$B%&+a:vi0aZ/,1**387xX>8p,041)8pl5/)0Tv-`BAO93pk,213ED#B:%@#>f.jeC@&c>=,#wA&q35Sc,Z]4"
    "='N:vkrfnH2_GYYw^(s$w7)+Q8l-W-]o<qM/v[o@u`G>#/jJj,fMr0(H.L$,5lB%t/R&C4o*#X7#jDB4_Ko%#APNfLm$Rq$bH<T[F$^Q&>@Uk+Gu@qLt?Xg9ZTx4pR6>T.gS@12oqJ,2"
    "3+*v#?;Rv$g^X=$gTF=$bZd&=]^D.3,ARW.`ge4(%.X@$1Zho<E0SG#^S^IqscLR3MeFC9/^?II-q+)3:N.[u5o;hHY2[0RM=mi%':dm8KC/(@EQwl&)?]Y#U/qRn@FfKc?;c$#AGs_7"
    "e74^4t)0PSDuvqR;S?qRP`4Y)`_18Iw<J]#g?q-M40u2vr#oW.mdh&$#q>]k-S:v#=/m'&]cOe#B.-##Z@OJ2;>CV[V]`%lXKlV%L&X02t^VBGSWLW5cnu,#*fDL#O20thc:TBfPH%gL"
    ";EhF*X<RF4AfpBo65SPf`p5rd*-=xk&_:87+2/0<cb>+u7C^kE_mw6D^D5%Jr51R#=+iW#xxY#uD`eqMKuGWLNQ#o$[5uU$OMc##^9O$#*BLj'=xP##Z#ge$Bc(U&c)<f<UMalfh%nw'"
    "+P,c*]Z_Kc=w<?#8iZs0kw0AuuT>a<V26O($9L#$K=9quh]57$.R[b%Upj;I@aADjWD-5(G%Qj)4pD)*XpCk'0R%1(qc9dMRhuJC`r7a<#%CB#'na>-F+RccKb;DjA@^FrK)B%kL3F<;"
    "djGN#;hJ'sn?CJurYqiECE<$^H-w.:Z=/vc(ix%+AaO`<i0A=-5]P/(k.QoR::K-)q=+.)$f*=(6veL%/toc2`o7T&qIPn&SsXb^?wUr&3o1wgSu:XoodvGGHj><%g/hUm.D0V6jToN("
    "hq[@-H$/%#nq<Z17Id4(Q08s$iL0+**/vGOY-[g#TZhW#h5q^uJHKqu/M)su8)4A#6YP)>]3X#F)83)5oqw@#IDx:dIL'fq>$.-''JWfLGfS<n@ZHD*nE*e'XfHGQ.m8UD47Y>A$),##"
    "J%$c$ndt&#Ceua7KhAQ&;OW5&BI[s$42G>#P5DS&kCwn7PmDB4bp/I$qdw##94Rs$?1%W$Eqap%9QOcMeU0dMc/,+%lb&*#0]1?#Z6[S%VGT:'m6jD'x&_.2X%dI3/oBI3^IEaMIn,@5"
    "K0[s0uilA60iMMM#%Gi(P>^J35G`o%#`;v#hVXV-l,lA#67Ll8FIb+D9Z9iTf$i/egOR+VD+eP&nf=YuO4k%+N5Jl8Unm;IqY0&kKpj&4o*F[7+kg6&<D$qR/+^)*)kUdM1@pq)'.lcM"
    "<Q.`$Uxp>#d%]h$jHgm$#+.W$J`(K*M.wWU*$X@66Y8,=):a;7rbY*RV(+U%ZwmO(CtdO(9%.&kh4O1gYX0lf/7@PS_d6afhD(#>f)c@bw%[iK;v0/1e-L]+bDF##<mDA+.:f9Vx)qRn"
    "A]V9V@,VoI_A<0(xhA]$7`k,2hW,/(pMqU%4h+Q&663fMU3VF*kZ9A#<3XU.YmDB4CHsfMrv^^#XgL0_<qRg*Z_jP(3rWX(v,&hCTJ0<7aA$?QDRbZ$d9<I3`scn0eWx4p>/-O(jhZs0"
    "4b,87A.;5(Z:_t(jI9'f=^r0(9]d8/$Ah8.gpq0(.i;Z#]WD.3?weTiE32+*G&9f3xc[Pf&0jGqw$?q/hM[su&^t1TCwo]0QPMZ:DX^v.,*rquVMStu?QqkE5EaD#'X4%lg@hSGBNvG#"
    "(#U/@+1LB#`?S:v:Td`*b$v9/1i%T$O>qU<cnH(#a:D,#%FmU7G9LG)^UQ>#H`ZF<@4RVqh]Mn#Fhc+#22h;-modX..9kZ#k`?=.njf%#*^Cp..r?uu^NL_AJ'mA##A(@-s%<U/ops6/"
    "]lIL(@(16MAH$'vb&M_$$xt+jO9M#$#?(@-tP$=.O?=U.<EFb*b;jW%N),##T&CR%FAl6#dsRX^u.(##-gt._4($6(%VE`#udr%,BBgx#PK53'kw#B=wc;B46mv,)`M5/_kDt/*9#c++"
    "^VI1(uqpY$`OKY$F'mkKaQ;I3mx<.3U]`Buw>%12x.i>7Ro&/3wL0V6YZ)22OF122t9JC#W)H&#_L#jB3olm::uRi$B3vM(-D$PdY9ND#*=:nu.d()7I)8LETG@a]:MD`3eha%4Hbbau"
    "I7FAsx9-S'W=Qh<]*ST$3?+euDL]&%wD6^=>vvIq+i?8%S-EG;wLo:&Y7%<$cWUK1p$]5%o+O9#.HH^79<nt/@>7bkI,fA(uJ'@#d5dR&0%=&'GT?D*9*Kx#nrw-)&T(_&p:6$#X(Lf#"
    "[aU3K_a*aE+Ij8.bl^6pT>W&#dY5<-^]Y8.O4_t@KmZ,2oma>-Db`$#'$YX$:jC#$&1EW6kPob44e9au:]hnu<^?K#v#1J#0K@cD.%3uua9(nu>iD;$B>@YYws1>PRc&/C;td:Q]f4rm"
    "OxmRnu.i^o('qRn%$nigbs5A43aO4)dfE:/1brS$$/=$M#di'#%/5##)]Z4&k-=$M/t+C-C*Ig3L0,##SB<s%26Tx#J+(C+5.`g(:M@w'oB:QASx:<$_7P?,d*g>#h8k]FntfjK/twX-"
    "uP<I3EaU02ea3@5?R1V6HqA['JcU2MB)Z(3^a7H#:Z.d)e4U8.2ctM(_4h8.mgNn8(xPm8G&9f3HVcci]wSE#vbkCW838tuu,A'rovXZ@HB(mnf<h8$#<q2S2JUA=IvcOoS86<(aX+iE"
    "oT#fLEh'##ic53#85RI/Pke%#2UXMM'KrD+A6F]&GQ>3'[j_G))A,3'1RYkK<mo2)c?DT7QBd12x>?-dQ8R-dT;@-d9lfjKo5OEEt_(dMcj=n0$K@2.ufb#F`Z+5pJB>&;PG3-vAJ=3:"
    "$$YxbNK#>cG&+_#'uKj$21lu#qreh%@[p%4+W1Z>L`IG)m2C^-.gHk'25<L(OHT6&pW>##HID?#dS@%#RFcN'k;cB&6+/CuA$dn0pa,(#OGs,F3Q/c$F=uM(5d#T8v_Mh)j@A(#?92cu"
    "*EhMGjqg%%hhY7lxBPcMsC(8%1tAp..0VCsIAw,=xHqM%FnBB#beT%%fJ`lJ,B@W&oK[5/a9RA,W*EQLdd<1#1oUvup1XA%./`$#pVbd7Jv3>5.GP##D*YJ('@ID*Wd;B4diXU*MI7[#"
    "GHlY#?iU;$*Auu#0gbWQSgHfVL7V;$aelF*-fh;$8Duu#t]e>#Xf>df3+V$#'mk.#=[*9#4l:$#AislA#VM`aCp2&+O?dF3Ll^t@#a/CGnt+@5IY1I$S+/T%+*387mh-=7DidJ*,1%K3"
    "tU9t@m7JC#h/UX-l,lA##0Tv-OSWF3>[&s$?]d8/a1E_&S6@c4>8rPeeLnou.[T`ZBkN4Hp2?^#/>)YN-I4<ZpLq'?E0PQux4mftvY4.?wXkq&?a*kQY&@SE:C@>#'i`@bP`lf_bg9D3"
    "<I+t$p8b8R=B:g(W7d;$jM(_&VCCf#g-Fm&uGu8.EZ)22K]uG3w3qNXMJhVKcqMe$>HStuu>m[FP%C^u*'>^$7ZtM$u*4R%H:nf_O%LS.S=U5%TU&(M,Q=q&t.JV62(>uuNTFs%J;9VQ"
    "^N^m&#sJ2'.Y,7&jjWH$X4Lf#o;,b%R#Csf5UIZ=shgj(`-b.1VMIaPR(J8@JX;QLbUSS@fWPF#=;9qu^fXU$()wN8MBlTopuWn$]tE9uE(g-v%v#NWZd@D*-F3mSS4Z5'ex$a>-JVV$"
    "'1*w#]d9:%$>(g(Q$t9%ba8e$mqe5&1KF<5;V/C4XL<5&n@s.L]NR0#h29BF^8<I3jdHa$OYQb%YKNP(X_,87KqV5(gw<E2jQij8'<>H3#=@X-K:a$''cA1(oo5jua+Mcu(foH$+1.fU"
    "sL'eu$Ia`u$JgM#GUUR#Wv:0$[^H1goB#5A27Ox%0Te[%;.%8#YM&`7+MXJ2BlQYQ<VET%6QjH*OUl=$Ml68%gVDo&d=d3',jb?#+CfB#-9ME0elNS7$&###?UdH-1nsD<t?(?%cG]t@"
    "-ChH%8q/s@vJl5p#jlA6p'oA68w&T.RV:D5N]3//9FI5/_5D0c.Co8%;79C4mOo$$Wce*3>3rhLA$vhLG$:a#W^Tv-h@4gL^3P)Wj6nJ78P9V6]bu>>G9fuc[Pts9NcvFH2?rDDqK4B6"
    "g,G;8Ewb?opr+/Gh8Q9B/&BPJR5wb`3Aq/N:3EVQOlWA,N>gG#9gE2O05xY#%gs5$,nYFI$XaP?2q?>-dsXBf'XpRn6VkigmFn1K7Fluu*kTP'A[52(Jj5Q8(*`v%`q@qRj(g>,WxhXQ"
    "f']8%OqN%bPT$n&RB=WQUGeh(vp$Pf#l44KRLp-)QfS5'seB^#.D:lK6.1k+k/4aELXg;?L@#H3^98n9JC/m(`-b.19e6h2j-lJ(#ba>-?:XF3J29f3KO1N(UMi/:?XRp.%m_>$(efJ1"
    "6HjL^E*I=us%kf.]/<?JmI/*?%kJ6+GsrH#$C.@#l^bl9J('qu9w*N-lnj.=@-[8@OpZ:2*Edg]H[D^3lZqr$I?l6#TDbU_b(Z=lPB'U@:Xf2qa/%n&%1niLe)H/(#L'd4i=s%$nC*p%"
    "[4ofLPT^U%?4iv#@_39%>91g(3b<30[sL?#r5]/LEwbZ.I$,)#ptnA6d`9D5_#Xn0B=v7#fiQm2Vj&kL/s'W$(k@X-L9(K(R3n_j$8]uYcLmf_7Z/p.gmsf_$A-j0T(+GMl]r8.AQ_c)"
    "w04T9spj9&@=Kh(u*_*W$@6&OC+VeV;]Y>#KPn0(UKp9V**q9V/KQ:Vn(u)Ne3#K3R+c?6>sphYcPEEGed4cOv4Gm)#.4+eGw3'P3$sOb2aWcDWgSBKtH'##fV#3#N(9m$Ysn%#Euo_#"
    "Q0e0#R`###]4Q>#1/)g$IaUg('dwS%.i?8%S7dY#PtFJ(Q)8>,dsfE%.A@gs>x*F3qsIO(XiEN;foDaM'*ju7f>FQ'QYQx2Eu5]%eK3R8=B-Z$u=R>%d_rW-5&]Z.a7mf_9[TU%MWs[t"
    "mdsf_lGQ]4GHv/(qOa%$ug>##2TOWQi1WiKO?*AF^(jiK*N'@#W(]w>sAmUT#d=8pFMiJ:(N?Z$+t]P:?N[w%T7?uu]7CD(Z@n)dx4V'#GONs@^_vG;@ihZ//A&2BX<-,rW%T8/MW%Be"
    "Q:?>#*[xP/H;2/(*-)K<,nFQ/WvlN''Y;?#Pj#1:BB:b,-=Zs06t+1:2UTau]_vu#nW%##,X7w^gTCG)(Wu<-[bi]&*_%*HqKw>Uu&Gs-XGl3Bvn>pSEukS[<8$W&Z-Jd+4JY>#Y/T8R"
    ":L7@#1'AjKB?,3'cw3m&D8d8.uP<I3ulc;-Lnqp$KA-W-.K4L#3pfS[]sX=>'*k;-U5*3&4Z>3#VQE1#Nphx$8ci,+ukd;$@Liv#GE5/(-eIL(r2JjK[QwDEB3>m/Gk,87?I-@51E<m$"
    "<<Tv-xB$A%@p<P(xI0quw@K6lsl$>cBLQs<cWiV-32X8n)Po77pjxe#mxt_.omQS%w1_j%9*_#.8]1v#oY#N#uGcB&3RUr#W%8jKv6c#%@';,%lT,3%fD]t@xreQ--Fa<=VQ;b5QO9k."
    "<u3hsAS^K/r)@S#i`<`:^Ao`t(vc+#2/f]uYS>)Mu_W$#w(EB#>oCv#*+0^'6%r]$:DojK?qw/l5:<p%3<8/L?a7mfj*6aN=UuC3**387;jPhG,dkacL0E>0Df*F3SqDoR;N5(Jtcj,="
    "WmS?0eX_Y#Vh$##Xn6L30UnX7::P;)/B=WQ%EXWQv$T3KXaa]FIWG3';Z]X%Bb_).&_aq8i4W;7aNZa$E&Z@-@FgK%+H>T.KxKt#Y6[i$-NN5$`pcah4e-guCdJJ08R720xmagQ#)>>#"
    "omQS%9$Oa%$RF5&+i?8%N<l`W9?$)*)jjlAB%PkpQtW=$AoQS%AW[12t5(Fc,u$&kjLm:&k>6A45^/d#brZg#t'pY(H7rv#20C)0qaP,)M.I5&+a59%ZF7/NN();<Z//nhYOD')TcN%;"
    "#?aqK@'(&vj__r$dZI%#2I>4:_&Wm/2Xlf(Zs?D*8w/]#LacN'1%r,)eJ@-)Mtsd$^7Lf#qenl&q?ja*sVZ*&Tno(MribO'=Y=h2tGHb<x5-E3T?w,*UwRX-#icV-d?;W^.:V]F)j(PJ"
    ".4&]bhvYE#B+/V#<_Hi9aX[^u3>NB$Lv'fuw%X)3>5GflNZ5/(MvY6#ifP/v$0^(Mwc)l(MRp.C]l8t%TCuf(5x1k'5GH/$MgC,)5:ik'.&2$'t'u2vZi%K)Z_GS7EcZ%-h>@e6-?N**"
    "uq#v$_e:O'2-fl'h5l?,1>W02c2V02)VN+0gc=?m:1i>7a1K;-@UIq.%oSL2eiML+DVeM.&t@X-4<^_.aj0m&.Y-K#,kjV4def/-WBjD#i>O`S37[ou]8uQ#STWeD+wR5B'wcF#vtfFM"
    "GT[&4#)>>#,PUV$4Al6#PDSx$&B6`a/W[GGh##=$jMY##h####,x/T%U#CDj[Clr-P&^&6j8>YY]/71(RcA>,b+S>#7xvqRT1$S+fhlrH.e+^#jr3e)HdUK(=^5C+*:HZ$9*###J.D9."
    "oP`fLH5*22L;?B5dNZh([E$)Q(MDpL/1r%M<(]o@g3I)N]4Gc4mwY2(GZ_,)'<-T%,m@d).M6]ko1ni0&<6rdutgIhO8cS%Nm)](vt-f_Q9_M?`vfA$%htXc]m3f_8u-;H&#axXZI6T."
    "jRrT%DU4LQ^g*/%Q5>##MH:;$Fft&#k/k$3lPtU_m$4onwkDB4t/:onZmGb%wf-/(gnNSA*Ce],47Sd%<18H%F'u2v-nks%wr),)s7pC44e1,)SvB973kZ.M^KicD-NneMB(@8%_#q^@"
    ",MDpLjKZs0r4,87ux4mA<nEn0&sA+*tX1W-S7K,3,Cd;%x/%^$eH5T%ux)/UI0Eouo[%l#FLsHJb:Q8D3q-3N5?`4ND/]aNuMIK$LSMg>nk'#A]MB%2ZE&##X^W1#9*p(M1?R##HmT@#"
    "&QRw#%/tS74u-W$`g/.$D(2bR%$2F3rcK`ErbQ$$C4N`uBkId#O(xfL1M6##]=#n$iHYd2C@%%#L1p(#/==E&ef*.)+bxe+MkNg$CjpB&.pa=&pTE5&-TXoRfiH63:TNp'es31#bQ@HD"
    "^8<I3XA9OC#RDs@'b2B<N%Ys6ll2]-DtuM(wpB:%A0K7nlk.rm2b0AkN3dwbUpaT#%#'q#4Cnl8mCl>#wo4Lol_(/:'tY)*nv/s$DHh+Mn^CZ%/&Is?6#Zc2p;n22L-@O(PrKv,<IET%"
    "3BWH*^g]1(X:78%fvpj*a7ta*F*;+**EUE<g-jQ/Yo^._9UVj)Mf?4(fK[Y#Ip>v$kihT7Y)KZ#<hr=-Idg40W%_;QdTb,2.Srh3V^mH3[BA['(>)s@)=/t-c13B<fvb^,pN@X-pc>)<"
    "-3l;Hde&A46`612?1Th2aZ(A4I:8OEE3fb>kW%H^7kdj1g[*:/lQ,Eg3N32<q6DnD4E6N:a+LJ#r=qauk*Q5LqD4<6LdYV-Xr=H0QtQguM,`pf=m6xkqEZ;&U7H1#qEG'v5vg>$1_''#"
    "AiA(#fdIL)[UiS%*N;4'f]Kj,mZ53'4.[)*jj4&#Jrfm'Lf*m/Z&jY,WrLdM&&&t%@EdT%GR<E2?<$@5j72mL5YjfL8ZwGGVVRL2fR'f)'na>-T?w,*)v8+4oPq;.1uOp^$[Rp.*d4x%"
    "q`<H=`N$]bGE<>#@M@E#3'mm<0$4v[#'95Dhe^W(9o7D#J5G>#aSZ_#35-q;J_j<aDujoCpv@E1<_'1;CuMjZ:'2F8Qi%?5o67p.2v$8@C=NZ#1tKJ%Mtw8%7`1v#G6>j'<_nw#tqAa#"
    "pQ_v%@0ZcMAF1GD^<*?#NqN%bWJWt(/3ge$p+7qL=h$-1.^`BuKQ%@5XI/R/rP(Z-h1tM(*?_s-oIW.Le5$MK;?ASRZ_I5&Au>p.$IbJ(%/5##]QUV$Fov&M>lA%#gmMHO_JTjKs'L-M"
    "BKf-)I=iK(5r[-*6sWT%D^;@(wNM>P@6iXQ=t(xP?5mE*SE/r'nh9q=VE3icJ;1;$ll]E])5C&Ot`VrZS&BjBgO.K:rV6(#8gv^7K8G>#0-BJ)XXu(+x03W$/AHqRZ^Yj'=HLg([j2&+"
    "I#7L(GqN%b:qG3'e2WlA%)###+l'dM_6vJC(]`Bu`;TKkJ*:P'V$9g1o*U1(];7^$W^Tv-Qsd)*oiN&+omJ*/]tV(Etv_L25<R.Y/T2c>J9m313u:H#]um]uh_t]uK<S.a^@<Zuh7tjL"
    "Ks%?5VJfQawY5xuM*#B$X.`$#wR+<%1mDB4/sHk'F$49%.#hpRrtOcMH[1GD2]uu#bSCC&*O:hLc(&7p`_U02*qJs-PcW/<)fpJ)XVS3%tTAX-%?W(ElCDDSorl</5H=e#kY$<]suJJ1"
    "J0JSR-S-W-_pkEI]M@cVZ6kZ#33?G&VK96&@+i;$:^#q%I<x;6Sib;-dTaZ#AXsP&C@@8%51[8%G.%W$E096&%k2.-3+V$#sZ*9#4'u2v[lr/)jSR+#j([0#m=G##J76Q514LN%W.-?#"
    ",)@8%IVFe#YGeo@L6wGGa.Se$LAh*%_.#@5,vkl(XS]`*1'3hLlwmA60Y2s@Me=<%0;qs6[^'B#2n@X-qYHv$XVD4u]mVsu^8QruKK)s#P*e6MuN7p.$1PMBSS4Z$4Kgw3I2PX74/m/$"
    "<U*t$I9:g(_H31#lf^g-W%@0>WBU02Gk,87@Nn:&=Zh/1t/U^unw8iu&qPN#JiA,2'<6G$3Z>N#9*>>#ZwIp$Q/dT.IH:;$oZ6A/9?8W7UYxVMJZfF-H%&rLOi6Q8CPN/2LeMX'+SBqu"
    "U6[ou;ZxW93k8e?>lU%F#A4s6)$K]=#)P:vU/qRn,LNs-u2WlAL8[#6)>VV$.VRm`MR@/$(`KS[9gx._1c'9'CBn4(/L0/LibZ;$m:'n$Y0`s^e7fJ5vKR5/P]hV@kD2K:r<,bujG^m$"
    "Lg[%#w>$(#,aQf*`33q.iq?x,Ravc)`Q,TS]A2o&HktV?>5^0MBXlf(__K^$G2N&+n5WdMcxgm(,0W)'Lb@$T'Cvb*h$Kg1<E.L(@tTH2,Z?HVmebgL6SYBGv/2DuQcI12HiVh#;w-87"
    ".D0V6gF*C5dr+@5RgRL2kiE.3GvA<T3euM(ql:^#'P4F3IO@X-J:b+&8vhHOWE=:(9ACp`Q[sjfm-Y(ErW$&rr=S4Hpk4t<2d3LE(EtP;FB0N<g0X?M1xXCs'`YguKvPLKX:^Cs'&(i#"
    "YJH2NcRxYGx,w.H3uVrM7[x9v.i1$#wj1C$QMY##)$###C4LfutpdG$?*(V%P5s80?35j'ucp2#kqk(N[^31#^9i+VqTJ#l*S?<$Hn<t$f;7w#h+7H',o[pqKlHP3.J=<#UJ@*n)+vZ5"
    "%DLfuxv<A#Hw)D#(]iLg'n7p.$&TxtRuIfL0G;/#,.E'M71b$#n1&?$<lAC4KBKQ&VPDK$?qs9%FF7@#L70F<=/.m0<UEs-=QZSJ4m3c418#,i.URa]<^=PSYTNY9Ee5DN-&rP/U:ZF:"
    "4RdlAa,4ZH,`CZ#ms(6/wn'9&Bw/U%%9qf(p0A9%h5r;$ZBPY#2xU?#,rm8'L+-C'1RofL`ODpLj7T]<M:#H3>0;hLZ6EZ#)3=PS/iM/m#K9Ok/gW48P9d;%.Wvt0c[eZ7eF.<$J'^6&"
    "QXu>5m.%%'qEoT%a=6vu:tF/(RLZ]F`foQf/J_OFhIIp.tCF222]Z%nP3*^/c'0gUJFk*rppu;AYCUxt'DYY#+;###dk+XLrj*$#l*F[7/Ocv(.TBpR25$6SLA#/_t@i&-'hQo'^)WB4"
    "xvcG)Ei-p%ig31#5_K/LK+3aa`HX,2ae.N<Q#.m0Ha>Q/.:JC#B&s0(NL4T%%*TF4(&r;.2+4R#.]QCumxU($csK=7t+-;Zw2%VZL@;20PM%41.*P:vWl'##g#X5/vsWM0+i?8%FH9/U"
    "F`08.7U#j':tewP8^+N)pisT)U.iG)j+b87Cvdo@AW[12'e_(#;p0W-*7>'fjw+/(Wu]s&//n0(H9Kq%XqjG*L(v5'>2ChL',cp%q(bt$g2'j(otec;mv=naLGKdMHYck-2K9q`st]CW"
    "XvF:`HmAgMnqH(#dL&&vvKR<$#`R%#V]#i(v6p2)[8+1)a@'+*B360LNT1k'2G#i7Kd;B449Y6&t=N:Ir8Dd*4+nAM@,EG)VW5F%Zoke#kLe`*wvXF'bm31#HKSK:FK;s@k9$OXI%AtU"
    "<w-87IG]02]P,W-ApC?J^#RL2om-H3AS'f)7JET%DqH)4ZGUv-d,HD#9S$$r_[<puaK7*F-ul'Yf>9uc+XvaXcIn1gCeJgtn)2ig/PtdW-9@8OYU^%O@VW,MW24E3S-4&#Pj#G4$,HqR"
    "<40L(-w1k':cm/$fY[-)<L.l'FA=@Y#dV5*,-0#,r_un&wV-/(VS&'#HMZ?#4tpp6N%TC4:.'L(IUUQAI3,n&&^p*%`lke#,Mww'#0C-OJr(a%u^*,1rdKpL(=Va%],9g2Y%H9i=%g+M"
    "dL-W.8n[XJWU$lLUhAd))sFcil/[Bl4`WY5jRgIhbHmA2:LOI#cDn(<4RBjCe*70epJ,W-CvGNi7@h19j>5<-qIZa2Yj8I$[@%%#3NQ^7pIw8%bv3j$s8ps$6`v6S<<_c)1dV$#K<0:%"
    "/hND<IjQd)rf<>,QK6&+.9G$,d&/v,IMd]#d9<I3j_`Bu5x5T:8pF5p6KW6pmJLt-)3@v?B#%b4.W8f39%x[-CwXt(2FYI)2@wguMf5h2r4Xr6Q7?VLoMpIST+Y(Ei#sWRvsCL2pFID3"
    "(It9[iJa$1]$K%1f8kwIM0LAX7qAspBuLM#$$###5NBw#q]:T/]sp#$3--oJ^4e(#7@.[u.[4q$WSk&#[(VV-3TNg3&20*>@H.,*Y)hF*eX78%*;2K$[`Yg-`Wkt$fj31#:#Ld1:0S],"
    "Db14+1I?K)N3i^or#c;-?BsQ.o@,877ui;-YH6*M`n=n0mmWd*FIeW-$))hl@^AX-kDsI3m(OF3M1'C4_R(f)5-ZNU$tl.7+-4(5Xp=cu&d(K#5*c9QCZ294mIsD.GWaj9D^^ZF<Z),W"
    "xq?m%cIpU@>[B]AxUoD*'`]`*E>1H+E><L(GQG3'AcMdOpu)0(AvD1Ll+T+*2G#i7AnbU%bmRp*d97%)Hqr$#kAg?#<N@Qf/kveMI2<A+J[]J(WNTe$aoItBqh^N(]3u2AYfHT((G6<-"
    "cbtg$h3ct(GuXL2hga>-n:gF43XEW6j3[5/;4;W-M.XF3ScoD*ckK^uEdNpY7$wX#L[9fLc/r@1sd9uc$/Xu2a'n=GkHBUdNOS>`a>:DgC;iQsj?pRn[C9s6,?GY>1be6/Z6YY#1-=$M"
    "xDp3MxtR,29#djutQjJM&X6+#PPt;?vkvXl%W=p.Ui8>,@)l,+]aTQ&c^8N,g)%L(9;wk'=ZHg(CPA**%#jIq.7KQ&`EeeMqHE1%99u2vJEXo&tvSs$02-A#8/9v,KI&j'44[S%5*OI)"
    "apI/&LW.@5.sWGuu4,87b7^V-a5a&RUZ:x$Q.+'=/XmZ$LMc+iMw]CW^Ox&=4A0t8sp/O&(]i5/Y^W1#=6p(M#Ba$#Yi*N.<0[p*Ykr6'(s&6&^8%L(%aj5&tjj;-JP[M..Vww'JFTS]"
    "r[%[gnH06&Pugs%.W[A+Q7mp&*)+'+3Iud&OG9t@&Q7m-K;bB.Hbad(h?[*#>Qab$.L3W-uXO$%`Fgdu&qPN#bGDu&bLQtJSw6Y38F.%#sY4q$Lg[%#Np*)*`6HW&3SZj(&#Db</g6T%"
    "+6hm&2W<**IkE=$6s`$#phOcMQw1GDRD7L((4:dM`f8B+/@1E'x)387+24Y5j[MuMr`<:K'&aF&DJS[85UdF<0^(<-NQUV$m=0q#deR%#+Pc##`:D,#w-HU7a+kM(kG4;-E7Us&ROQYu"
    "i6Sp$^6i$#N52j7v]n<?1lh;$;VUV$*75>YD$F=$JP^U[Wl_,D#=s@#9v16&/tQ+Wd0J[#<JY>#O2`K(JQu2vlXNv%^_%qRoFOe)]oah(:w#fVTu>>#B1rV$?0j>#D0`C$ZutM$=I.@#"
    "5:R<$.eFm#.0Qr#>w'/Lv-7v#v(^t@8w+f*+^9D56N0j(SZJm_pCSh(6A0ED9k^N(jkRA6?A2>7bH'T._#Xn0*VaDBHTGL'cD^U[/NwC#iC;uN-I4<ZpI_b>ZZbAuxKlqtOJ?)K%pjig"
    "1M<T%#[t7I)t@DjWKOP&1Qk,+K-0U%lE>##D30:%58w>#.0EI-gOmZ#RqN%bQr>(+vT/<Q=WxT&&5G>#u6eO$nnOn*mr@k'mKc)3sX&A$[G9t@9lv23L>[BGv'EpLO:U@6nFZsGl7kM("
    "OX(gL/o:DjcSL7nq3wHXx:N+&L1$##fV#3#&<7%%dOc##M>)u$m'w;$@qNX$Wvmo%ZRrS%qx=gLh)#K3dS,P&HU>bumI/a#[,iXKq_tA#rAGq;(m'##rfC#>[$;sRjw'hLEN*:MC?[tL"
    "g0N-G&Srr;@)ZjuhTke#0bN)<We'B#*j[+MLI$##-qsD<0MNY5-S,<%Yue.q4'1Q/`vBB#BkRw#fqN_&L,###?mnigMF.v?F9bcWNe7W-WRaY1t(FcMsmChN7Z_i#%Qh/#-0O?#4:[2("
    "KSW&.Q2GhOkDU-NAE0cN8ahi#I9C/#4E'[#)&O^#G/e-)`Kf)4doS+4ZPN&#+NGDXn+x]Px65_AbouM(5jjqJZ`oQaH[g?pK7A?NrioW$Sxmo%HUHDE<[u>[+aDE-S?(@-4euk-NS6X1"
    "l'/%#4Nu20os$cr3Q5Q&obH'St356#mT1v#[Z#,2vK+%$1AAp&$.o3oc64(ML^[`*i%q^45cu>#-b$s$9Ua5&W1FcMQqN%b/X8C4Hj(s(^#QJ(4f^F*VZv)4U_p0gSA`]AQ4oigCBuKE"
    "5k:4i[P44Mf@YSn33X)<ZQjp&/45##_@Nn3S9JC#IYi>$na.T$;H;)D$),##_[f=D&I1*QFERq&q6tT%>4hj(_/aB4ilj&+BbFG<*Ce],Ngu:%a9:,3K00-%HC.j#Ft8d%u%Fe)UCKT&"
    "&Nk^,AVai0WOBq)hb(@,uI)m/((-r%EgJ<Qq&L-2t8.Cu-J1@6IGEaM&lSh(i9$@54U<8p;cR^32[,B741At@dr+@5B4p+k.X)22druh2LIi8.^]6<.03QA4;EsI3M):I$;_$gL;B/g:"
    "e9;H*94qJ)db%T%F1*E**^B.*+2pb4qL11Z+@hR@Es[#L6:?gf4e5ISft38Y?M0i.;%[-E&]sB#VJq7%_Pt_jKG@&4IDE:SiBqB72U2:@4hXrLLGR%P@emic>jP.[L#<:9JH_x0/Ron2"
    "ew$X-XG:;$gC(##A'of_AQ_c)htLS.uj#</-8G>#rh$H)HL1gCS(D[#KA/**j;-U7(qm4(eIm,.UQ06&j-u2vWa.1Mpeo=%5%Vv#]D2g(a8F+N1m5<.KZV_AP$tt$*/,##oWXq%WF*p%"
    "mjtq%gsRDE^8<I3&Uet@-HSq$#,NW5q3H12/O$(#gbN>79a9D5Q]`Bu2M(kLbpu2AKxRe*ARp)32n@X-[J-Z$?DXI)ml:^#<Dar?u2Wm/FF3]-9Np=>SqWu>LEA20)M@;?P*Peqa+5##"
    "1JH<8Rd%b#d;`0$U<0$&'(gouBBQ=uH66<aB6xUHnj;xAl?VSu7V+##*Er(v1BNH$(;r$#=,>>#Z^D.3KVjMK@2m29o6UN(@Dn;%DA4ZIUX,>:6;+an&;qi0heMG2CWIa[bM>,)v[F5s"
    "eX_I#Wa=Y#?(Hru*)520B(aFi#)>>#>0#N%jvq[G#_P8/LHJ>%cEI/:1tkf(uuCv#Rmp6&?q$(+noN*4CL0R3HE1R3eJDR3o#6(#]=vF`Vf_@-Oj_@-1Z_@-7m_@-XW_@-bm?x-+eniM"
    ")JH-N*tLhNv`k/M&Ad)O-g/RN(_?MT0g$]->woxTb=8R3eOSR3[/5##lH:;$2:e0#0Yu##i,0@#&aKZ-*U*8Tm1o4f(%_%t)Y8cMS4M8%JujCN)^(?#;Tji*d@t6/3XrY#&8niLJf^t@"
    "*u*,2OA[h21c?X--[)@$:(>YuNhXeuJd3x#9#;A0?f^XunuiVdp.`$';$2N(HjE.3ig'u$a%NT/?;Rv$&jqiQ(jPJ<t4Am0/R`@,=V?S%)ah;5%s(c+sICOLUT;B#x(cu#gmN^-'4q-6"
    "Dccw9UfWF3nn=`#)%KZ.ZXOx-;3[m1/g-LpKH5t/h;PY-(^@tL$F1loYha]bC<&v#Sw,[-B4b]b$`GGins.#Mt6Xxb*TtuGhcak+N$nO(A/HPum5e;s1Ibr.7$sRRQvLk4,Z_$'MotM("
    "wb&#c>FAm#h6aLK[n?>#KkvRROD4.-6baN(qn6Ku0Fu>[v>`DtJ=cY#iGuG-IuP5Mg)o>/1J&/LBFAvL3DDbti0/F7bt<x#%k./Mj:F%$dNqn#FW<^-nT-F7sxh--LqBwK)*)Z-qeN^-"
    "_B#RW[?U`t5O<RN3%bB-mvqtLxjoRR_H#N$%'r]Md5b]bv;v9;rD^BfG3.Z$stnm->[qZgr6V)NceG[.^KPUtGhj3M08,LN)5b]bKdsS&?B<)NpT/cME$?`-'-UqV<%0loE/d]bWu#v#"
    "GwUH-XReb%'6vZ$-ZGN[Qud5#<TCRu*]c)MUBx/Mv>pV-ZP8L,u+-W-14gEe.M'gLtRGgLLr@X-F0GD3eHW-$(d0f#vB4Iu]aFQ[.=f:6E/Jk=;jB%MDA1[-0Q4L#h2wO$RI;<[hrX?-"
    "K&,o.$`K=PW^_TMGTQ##a+IA-DEmD-VR4O1xm8>G*uJxtTC&E[7qg'Au'R0u1q2D[p.P)N'*xQ-T^>T%5BKMu0Fu>[4tqNO+4Q*N78S#O$H3]-nnH)bQqd>#]-V?$vbHOf$Z_LKTO>#v"
    "N`c/$ncPlMb7V-Ms45GM>8gV6)Ui>#vxTi02n2#MpNd>[L_/>c?ONF0R6wK#Lg/M%19%_o&(s,+MH1x$_DAvLOpw;u@aP`/=]v@bYha]blB1Z-B8GL#14b]bkHQh,_1oK81FXkO5Q0%M"
    "<#_HN_:fX-.0wiL:X+`#;PgetQ8qB##8&E[-LW_agYU$Bi@C/#dx/B#4^uHN]]-lL?V`N8*QFAuhHO)8djK#$7DKs$8Eg@07&CH3e/4:vaarKPc20:[Xn:?$S3PuuiD^4M33.uL'L3]-"
    "WX=O1eRgV6'::;$SpxE[R#<nNb'#Q#e[(rt;(UW8F$RQ8Nf,g)eM`j)j4X_$j+I;8%['^#rh#gL.^d##fouM(iF%f$w1tpu%UYN[gx$6/$e7F;YX^`N(M?##l0$eM@:MhLXh6g)$5,RE"
    "5Q0%M(3^+i$`x(NC+>;RYp@Z$`.krLu2V_a0L@dX991lo;?'g18w3<-tcud$930T.$_ISRMVU=%<S+^#9v^oo^5st1oE4eH`WUnuRu;'M3.To%rKH#$c#B4XHc_)4Shm68XJm;[vY:lJ"
    "Bgj#@d'c41k#N=PAScUtlgM=P;4PG-8wll$1Zt;-c4b$%7ta8.G&O%F5w&T.&`xeht/g-9XgVq2D30g)2pTW-s:$3M'C5,MLK@F%7kswB7h$gLqL3r$Cn@X-%Z_qDol<cMkQ[lJ)(7M0"
    "w'*W-7HUm:=:`F81<L,b1P2Y$u40Ks8,GSRYkT&9o$_#$NqllLh(pk&gmOT.V$nO([Ch@/#_.,fC9xK8&>]`tt_L3XNf:<-$.wMMwcYV-B9`HmbiXIu&6*>.GesOJ,AWOFB;WhGOuh8."
    "0S*,)m,2ppYwBS-Y7_M9AnB^#QWaRMH+Jn$XPp58Q@F`8E*_a53?rHMe-x9.2jPT%_.]08xw(3;q:[LK:mh^8186?$KkvRRRo'P%n&@D*lS1g);NiA#5T)Z-pPcd$FoV20A0IfhJUEPu"
    "fr6</;d9/1^i/.$$hL;H$61>cY3FPu1jM=PNs^E,gmw%F&jE#$7#1K<,VGGisA=Dj4a(1.u_^xOHaoRRO.,l+m6J3M#BdJ(pWkX$G&XV$u=iE[SqtWON?Llo?O1^#&%MAMQ#I]-L4P1,"
    "#+2GD<PXkO[FNP&EQ(,)$Pl&4^3=&#nd0'#(?$(#8pm(#HJa)#X%T*#iUG+##1;,#3b.-#C<x-#Smk.#dG_/#)N/i)J,:.$3,7`&Y/XU*Duk?2Uf7?KG*s-$6`<?Kkc5F%l'(9M#/ILK"
    "/atgL+OB3K]cJCMD*g#KN)p6*em)C&79O<Q?k[mLf#W+r+>p;%m_%1#O^Bf$m*(9M-4v<-,nQ11`-%##,Cg]Xvmwu#a'kI5?(60#&;w0#6lj1#FF^2#VwP3#gQD4#w,85#1^+6#A8u6#"
    "Qih7#bC[8#rtN9#,OB:#<*6;#LZ)<#>7rY70e.8mWBRSm,4V6dk)1BiNU2,guS^d]A2,*^l#0cSTo`nX8EbWVRPPwK3;u<LnZP,HtTVtBxg%:C(@*M<sgNi<W15Z8^D0J3bVTf3tx?<."
    "TVdW.5`X.)G4`v#KF.<$1''IpXSS<bvoUu`BC-iQW.n2O@VraHAARa?>FK79u=8M1j5%d)RMtPjHBS9YiVKFJ9t;^Ji*u^@R`(:JY@Yj+rSUPJD^MxY9M*oXPrfj+eM5SAYw`I3WdK(b"
    "/:,k)-Q-#9YM$NG$1-iRXajWduS_8'-FaF6YM>tD$<ODUHaO2gen;]$-;>k3YMXCB$GrvWrc;c`D*)f?CWZI?9wBL>3et0>).]3=#r7n<o:vp;i(QT;skDY:mXv=:kEY&Ae35a@,&N_b"
    ",>Awc,V49e,o'Qf,1qig,Id+i/u,npO,FTo?9`;n/F##muR<`ke`UFj3MWp)ud^n&+nHpIY6Nd$v-NJ1l<wsId^Ea.PX`A,KV9rI.RY.qr(6q$jS-q/mGM/qjXeK'b0a>-';A0qb3>'*"
    "Yc=c*)ZZh(<bP-VDG<3pncZ>6XXC;-j:7[Z8ts9]#3#,2=e+R-u?38YuMjiM@eGJ->x5JW6qQL2L#G:.ffT;-QEm-M-*b=-.cEB-JBF3MxNu8M5dDE-:C+%Q9PU5S2Pg)Q#ahQShO3aI"
    "huVh#-EiVq8FvaMr#@LgIN*x'%AS`ti7PZ-GjP73$Df`tKZK&St43YlWP7C/OswuIdP`MM^8saMTqhd-'ab-6im6iG`LtH.^b`29cjw`&ICi;-[1iI&FN[pgiWLFWrrO.#Oj]=u>/2O%"
    "S>N)#ToA*#eI5+#u$),#/Ur,#?0f-#OaX.#`;L/#pl?0#*G31#:x&2#JRp2#Z-d3#k^V4#%9J5#5j=6#ED17#Uu$8#fOn8#v*b9#0[T:#@6H;#Pg;<#l.>>#J.OF3AvYg)ij@+4PuP8."
    ")w@+4R7g;.])sS*-MDn<Qc-/$Y`$a/C*&F.vGNS*/vZw'?^k-$`O@+O4:MhLU[%^P[MceXIA,,iG9TV-n7R&QF<5?KSgScMA0g#K-=f@'?]g2t@::1KUmA,M<$/Y-p*=R36sn92>1'sL"
    "]p;fq[Pl3+u]U=1`M^:)wuEpu_#1]ONgc8.>T@>KwP.f$Ex5[KcL1ObqG%_Sa#sC&G(=e?CZ+EGV0_lNqm#E&Z),##aH:;$kRf5#9$(,)Zxh]P$QK%=uCF&#<[('#i^*vu.p[k+G[Lk+"
    "ZT<%t:NCnuN3G>#YYqr$q%k1#FMOV-1bwQ/`/HYc$$'rHXdMB#+U%5%VfD:MwHWZ#JW)vYWr8&Ox/'J1&%Nk+tJ'aEHN<w$lqvCuu>8Lt/2'C-aXEY.<=%s$x<<Y.Mel.hR78R0`.pI$"
    "@g]=#sEqhLpt/%#Hjq%4hqC.3vfHB7vxE.3&:=]u78Mmu0K&2^*9IG;TcFvhT]=vhT^1:)jGiQ#/98i^SSQ0SA[L'M@__sL=5N($&wgO.fE9D3R>jA#YlZrJ06X=ldN']IdEX%I$#$J1"
    "/fp@IdKk@In?:@-'8wx-.G)UMs/=fMbpq`3qf:g)oGtJ-Nbm=&IO5,MVO*K6/+PY#>G`Q#5b_KSU`mKSne;,M5Mxo72:GYuw^AN-$h]j-/b]k+(*^V$Equ@ODITGM2]+<.6`3>5uTW2("
    ")rRs-En?]O:l-AO>.RAOHl[]4+2pb40<Tv-D[l@Ok/<%t%W,8IYQ3L#.XmcuOh+ru]HTDlDLWDu&T1Z-rYCv-pjq]NM]P4S9l/BT-c`3a@OcYuQj&M#Ng:?#%@K8Ms67V#S^#xM>l#/C"
    "KFvk4aT*YP7,qc.@I:;$w=.kbASekbk%W4bHqFjDRCvDO(+p+ML_XjL'X*w$;Dpj-</A2Y1=$=a^w.:TDcm1Yp%]/2Q@K0l/DLsb3H'[18*(,)?H&K:P/KP/ogE'S>^nO(i>ID*spwX-"
    "g?o8%2u$s$pn0N(CURP/=V_Z-<j=c44hTfLR.$M&%V'&O%.BO#Zk3ulYYW`<cs_KLxhe_u[tjZ;Y1^CjD:l4K<R4Q#XXnguam75/PvdS.Geo)h$AQJ1OImf1-PtR[wYe]+@H`4['tG[V"
    "6ZlC#q@QZu>HW.+[j-^3P$_/dw'kN#^30rmf[A._Q3Q]u0bj>.%>q'#:q<$?:,;j'-jDD*1K0q`>Y0gLw(]t-pxpm(ukXI)x(Fd=dV?_#hA2kull+PAJaH1j?S:@cVgnwuO(-g['&$g["
    "Vq#ru^n0YG@x.D#n_UYO@tU^u4ximu96#j[L*7#Mh6aY0?f0j$D^i,#=QWjL@)5i(5)MT/fR'f)Tuw[>r&KmAg>Z&4^Uvr-iL0+*I5^+4.m2v>PhG3+0T?(j>NXCs@J5rm&^$Z;)A3jh"
    "tPulu$7(5n2@LLo8p+Zuxc4/(G(N(N:qkV`,uQd`WROV-`7-W'#;XGu)ZjXV(AjpW.Ks>0]$%##UEHPf&S`$'3f^F*W-@w$N*RK)3twX-s7YG-8I+U%(W8f3b`M^#PmxSU.$<e?IKNM["
    "m)+Q%Uvor?kM;qD<.V'8AC42Q'lfx@_9P]aAR$Wen,t*aD3F&Rp/%I?.2<)#bhbgL3QV'#G?)a4ku*V/(H)&.#<8.M]*E.3snc;%Yf[T%KZRD*#8'o[k6SeHn]X)B,?KABS&TmUL#RC9"
    "iKNeP7C.=]j$JSUM*2m9daO?H'$L/G[@A7;@I,nFb6A.Wh;$%CnWQ$DM5&'#Kn.T$llUH-daEF.CUI5/QdpG*VC@`%^JYY#HML8i.hT5F]0'jU*#f5=NWwx*:Mh]+`5,JF_/O?K5s$2a"
    "ce>3#dL&&vhw2'MoL>gL8eTj(1u68%L8YY#O2$m&i=:R>%0)O#:a*pu:8ov3#kX8a[w]iKp<9*+ef)K(A^)&+0f-H3cVWI)GtqT/R'70)e4U8.^GZc%A3E.3mSoK60eQ&0W<;a466:L+"
    "x-84R`C4293gHm$:N-PA::-51DjZGtH5g(JQGR?q.3G>#B+c#%3fw8#<B2&.5qmq7]MtM(vg+U%Fuf&$6&cPG,h2NQ/?'M9fKu#KaBx_ABASP#m1CmRtj$qE#pA%$x)_.1?bEfPKk[R4"
    "o8R&#sOE/m*tZC#rcNig]aQPf$0WuP<Xx%#]ZV(++4Q(+LT#j'Tn*]#2G#i7:fAC4Y21]#mM3mfIbPmi+QJ2)-U:V&`oY^F%8UZ#XM[g%Pp4/_prPd+5&(G+go+B(8:uS7Lcl;$^bU%,"
    "b`)S&jo'-2WF122Ox^t@RKem0.v&7p5im-$6qAT(av2O+uj(CG/B0JN:44[$QYQx2v5Xg38&u>-^_BY%#R8<.$l2Q/80fX-K>m3D(uPe]'wS/s0ip'5jK:^-<TZ.8$M$D<*M'jLurc`f"
    "7Y-x62.4A-]?v:Z&.>>#G.p1g;qqigm1bo7bC^&$Xt?4$sbCv#53(3$vS$T.3b8C4VQ&85mh3k'F@xZ$9UET%7-fv#a#vC3dtZs0Y*50.l%EpLIRmv$mIx-)GG2^#eEj]u-gnru&<`g$"
    "?4l4AxhFA+,7>##m196$417`%<mSY,2mSrQMDp6&0L<0_*XYp0wCe(#[*b&-58:W7N@8Y.5>%<&8IPI)DH4K7]PLDaw7q;$P,`j$bre+MURUf)2AAD*':1hLg=]9&kr57]q[c#)moPxk"
    "6^o._0Ybs&%VE`#$3K6/AHeS%QQ&7pE;jn0'%9F%Mr+@5lc+p7YA16ptpcn0KYfB55A(kLlHm)%;D,c4_*p/1;5^+4nGUv-v:F.)B7ST%UNAZLxKcmcu'#7Y$-<iTdX@]6B%S1M.m:I$"
    "[+XEIkAAvKHklqCu/FnDSx2p/-=kP0rV4U1vWi-#2sI-#_3mR$mLb&#(?=d7=X79.:1[s$p(FS7n4e(#aL^&(EvD&+).d$,b]*i(ZV7Q-h<Or.3wfJ1<f/F.(n9:%WKp6&PpCK(LfZO("
    "n=T&(Zr=$,#Tw%+m,K;.-6SC5'pMB4;?gN0Y$;>Q=8</2*GhI3vSkP(V@3C&'VF&#GA:s)R7JC#6O=j0:>`:%#DXI)NW^:%vWtM0Zk_a4.W8f3VIJN:cW$C#8qPcF9o/J5+6GP+`)[BP"
    "2;VDG3]sI5,Hcl+q:KDP9BSl<M:3P<d<_PJ]xr%,0wu5Q4L.G40RoXh.e.j10wG9P5k3D5[&;iuRe)SM&n0#vin8EuQ;Nd$qMX&#Z,>>#(nuF#_$###)$=?#S<>R0K0KJ1tYxx%%b6uY"
    "[C9s6Grl]>^iL/)+5(02&VNt.^q@x0L*Y-?@2qf_(Yv`4'-LGNY)XW%ZC6p.6P?x0&q6uYO3ah&CeY+#9nd9/jKow#@98(OT,c1#+e9J%d0QD-l<m`-8aOe$4+,,)qP5##9D###ZCeW$"
    "T[L*4]'.lLsLC/#`h%T$KpY+#Rww%#S-._+[X&(O5UOE&D`?3#^T<8p+J3,2`Zs&3Odp_,]YVO'CO=j1x-@x6&$eO(nGUv--*JVQgnK;;ge'[Ngw^;;gbtZN,SCDZW^L_Q+F#@g2A(CQ"
    "+XYwgNu5%XtDK,R*_D(jbD)9.jK%##)TrV6Nf4;-)Ye8%kvl##];^Q&MN0Y$X.i>7,D5C5N#@Q/YXhi2RCT&$Xn@X-2.)N9$qs2FJecbuhem6$4;,d3f@xwbW6D%/.?Rg1XG:;$rvc+#"
    "t=7Z7*M5C+i6[p.m7_c)=-cU%#GIH+Y5Q3'2JRZ$%qvb*93%dMjBHX0^X+F39f:I3D@^UJbO-LGGM`>$jX(<%iC4gLZ%U&$&&8&4S-8QADF[l)v2tgSO&ic)Yw^aJ&ZX7'o(6ntUcvkC"
    "DYM/Pjmi3;J>%]#LhS+@&;TV3i71&.&p;4'VPcj@3GxR2D*$r/*+/R9%*>>#vY9F$u))G#@U[wuC$[0#6b@;$)'FZ5Z5YY#(1X.LV5#d;:b/2'[$j,+Gw]Q&@:@W$uW=6&4i(v#D_A2'"
    "N'2?#XmwJ3<9C.3YfED#6oLkL=KIO(^r+@5<=+u$?anO(4ctM(6,5Yu$aVoe+IxGg@7=fLum7Yu)uoOfWZ@P]$&;Y>jEc/DhJrY5n%Rd*IJ%JLWR#'#N)X,4A)Zl0mq@p$f+O9#HETu$"
    "pn8W77.NVme_sD+ncux0'9qRn(gpw03S]bM<%Z+DO,L-m)6`c)ULro%?vbf(jmRe?tV;e?%hiig>6(,)MY?X.v>uu#HN*^-shjw'(r$##clvw'QBw--Ano9232nw'7u4,MMOF)#?LVMM"
    "'`4i(m_/s7E<I?$>6xr$i_oFM*sPoL9'MHM'wxwug(7#6Kjn%lW4jD+v8@'Oc'xIMkhjmL.',cMRa0(,$8YY#^AU7nG<7/hE*%29uV$?.4t<U.]LVs$.mRj1cgiL([>cx,4vM[$]-p5-"
    "iVLt1V_G##$Qq>7@c9D5+tw68B3#-3MxO12bbWt(j7JC#]CI8%hiVD35Hrc)i&l]#Ro29/Ii^P88:@b4%OIf3m'tt$ARIw#7ZqV%Ns-x6q+E,)[w]%-K3gj*M40+L^K`?:jwPc*lLfH)"
    "j0h+Gp$mgXL_'JNE0RVA;L;/)p=#N5.XVP:I).M1(ITiD99B;Q6]T32OxDxC&#EKcdhHLE(b9w/DQL:/NEJ>6P;O9CS&Xf+_Mr0(9n`L3n=,,*1iL<-hM:;._-A=-b83*#MsJh=Xv''#"
    "gd0'#+B0&,i+8>,Jm2-*HRTvd*?^@#2+$,*(Bf)(jwTm$_J07`dG*22XR>3p->&38V<ngu-P/TRO,.&4o:8Y.MDJ)*+beX-`^D.3'2ah1$.,Q'[$a20e_BPl9>,loS<8+dG1E%_iFr_C"
    "RMv,VXC)Q*tX]-<)0IjDM(n>#GI*^Bn[FR#bYVQ#,pAhF,YI1]fe,:<N7Wc8AtMk1(^**#hc53#A*XX$gsn%#T7'1(_jG##QHc>#-4<<$9@%@#RmwJ3Lq'*)W@]F/rP(Z-VQwq7a%tY-"
    "`>,n%f_oo@[pC]Xm&pASqAtcJaWbJ*%e4#7WxoZ$<EC`>Qj_HOm#J%=9$_-Fm5Bt@I_IcM39=E*raRj1bHf),S07D#Kvja.rM1$'gmGn&VW&j,KC@w#.f'-M)x8C/3l1l(&>mhMpS,@5"
    "QTj0Ps77<.kmqc)ul9K-=+6tM(Pmf_h=%VB^au]-PMOu.2p%5=nsi8:HSc'I[OGkWaAt:6:UbV.87q]RH9?h-]UqZ^lDZ`6E5k%?*<LG5`>cwMPm-I/K+f5%-6T-8ICQO(xckx#Z><p%"
    "K)@)*+#xw#eHp;7pd3u7-_bA#DMC)4_32gL$'gK%G(TE#uamM3A4))AThr&8a3r,4pxn90$_)i9kChc5h^Sv0$RT@X;pDB-t)6-?Tfu;-_]=H6B^fW7P'^Q&>w86&]@`S%ewxg(U+9Q$"
    "ST<8p[F</1ta8B3uF+F3<BRL2+1+Vj>WAX-ckX1$&7&4uQF&&$O#*`uJpbmulvnf$UWvtLh_*$#R;mX$cui2Kc2]Jh4f1Z#?[s5&foIfLjVH=7RuED#bH:e?#l1l([Zr].x1YV-[UiS%"
    "B(#pSDH8v#.v0lSt;4%kU)%;6^XW1KLt7`<*ZXf_ja[Y#5EQlANFligjZAJ1qD_#.Lp+F+R>@h(qGtE(.vPB&-%ki'3@)D+MZqY(UOwYQ#ZvddFsTGpZ8fk(#`$a.bmS^3Y.;t-^-7.M"
    "biXs%Y?YO'a*RX-$Ah8.<2`^#bO^C4h=S_##F/[#m&l]#m/pKbStJ&_iI%`Cu;#N35)=JGcjhW]1KKCCdl$5J:RZ*=;/#*GjLF0;U`D`jaSu_bZ+PnuT`LjbP]=^u_)pp%*&CZ5MA1ka"
    "^(*q1re&OGlFN62)/5##Xn.T$?RW)#c,_'#TvT%#Jmb$0M4o9,(prk'hoV@,gN4X$dcL-M5?j>703.B#Kj41MkfQL2%_FC0tB:a#w+m]#/gis/gU?i3R2'p74p.t/v.6F+%$Qv./i2(,"
    "$CHx?\?YWB#KOk$0R)2g48p3J51)3l#i:Ea6R[eSBIVs&7TnwSB'hfk04MimtjNN79VH&sTb(wEe(J@']55chuct,Z$67>##3qVV7jU'dM3f:v#MT0>$2####GM1Q(ga,,2R.(i2v`O0P"
    "39k-$kv=PSh?w-$f9lmu%Y'i9J)6PJ)KuY>-L3;&RR2K*d8ge$==R8%Xh:Z#6`:v#L04x#JIiG3W?1=%lni.NB@1=%*E2GVK^Krm:6+G`<RF%XBNof_9VS`NPGbL^$=t+)/S=_.0VDs#"
    "bXC_%u;[`*-196&(Aoe)Z6R)(fKwro*D:qo;6RL2h3`[,l(TF4eC.<AnS?/(52qZ-OJU;.wWAE51D.P=4$#6858C:8gXRs$vqr5&jitqAj`oY>%/5##l9@`$.RW)#02<)#5Sno/?MTgM"
    "2=hf)Pv3B+A0(/)GUnw#;oS0(X9mfLAhDQ7@c9D5N7Iq$cj.4(:&Cs-E(vM9rs]G3^?iGMx*t1)`E)T/@piW/%61V%']3v#I=)'B:OR#98Q]e@R*7l;-BhZ'CkZ(+$eZ`Qfc1l<F@V`+"
    "8u[W.q?3D+/uj50sYl.Ds`vgN(3.uh<Qek05V.3u9viW/i5xbi<Y3J2S2hbcg_VTB0Xeu6ot<Z@,_wu6,*,##MTsl%x12d;XkDM04=kp%fg31#+AlY#R6UoeAw+j'xF+87Jd+7pde:I3"
    "oxpkL*&@12t,oO(F$nO(TH:&4CVE(TCr^&Th*>Y#ta0^u'(bTuUFW-?2KjfuX(ZR$tvK'#k3;f)c$Ju-Op]5'J*F#daOVV$ut-X-]ZW##NZ1N76A2>7/OU6pMF<E2OARL28m)J%=M4I)"
    "2r-x6Cm;hLcT]s$]r;9/SSS)*7IPvG-_/N:qwZ)>8oTC#n*@?.RK#A#jcrgu)D/3I1nUc%ls&^5an1H0&84+39,qe#8?fIC/SnoB((G6uU7J<Lg._Q4&pBi)7'?)#)?i(vi[_r$O#x%#"
    "p8YX7'_Rh&V@HP/g9g(On*_e&^-Ft$4'W(+o_W;7nT/c$&A4a3,D5C50241)$2f]43=]s$GgwF4<rZM&xY>ve6gfC%di,j34$m^@D1^mNpM`oKYpj(Kr8'&g'fj/T$3GK-Oq0U/,'^],"
    "HngQ0o>v]#D>.cg>aAWKm*0p1umJW_QFo?M&D[Sj@c+?8R%Q>@=V8B7Ou5#@9h3?\?%,,>uUQ>Ta7_fQaddHd)L4kb0Hq.[#4F(a4_hs:8T:$Z$n()X-kZFnac6^`PU'_%qb@<cDHrkq0"
    ">KR#$ln(l-Lh7naHIE)ISFxd=^dR%QlmQ:vjK%##<-of_>2l+`#)P:vFA+4%A:v(bbl>hLD)1'#L1$T.I`:8%ad9D31`i;$GkNT%`Vtci^b,7$Q5><$>xU;$d7ffLm?R=7Z8fk($G3,2"
    "><t))cX+F32(tt.n^kA#F]6W&s?#buHfZ+D=]Tl]9@k4A=+*A]F_xhtn+lQ#PXDuLpn4SR`t-j1O+-B4T'+&#xORZ7XnEfijR,(+3V3N':h6A292q;[3oE?$JI75/2QWudD5CC,a7J8#"
    "YT<8p0ge)M<fvGGsU)u$AWo58)mV<%#DXI)x&9Z-rYCh$=L$A%1'9<.j7%>lL5o_7Hrkq0Gg_pKWSGr?R6o;;3OvWSjq)t8Uv5#G?,gR;giAp0I_%5Fh9Iu/@,WL2g/pRRC?*e3)T_r?"
    "KiQVm*clo.RR2K*9Xld)N)tq.;nYg)'mjA++nqs-Bx?W]UK$12:uDg:7gaj1x-@x6-`/kX>'5g1r:Rv$sp,T%O71A$n5;*Q_8SOkT$gUTdl@`[%NI]uPe_l0?8<WB0&xp09QC@#LI@aA"
    "itsPkbNBLS)5SMN('+&5?*'kPY;eE,Rw]Y#c7(##e20#GH)w%+8#=T'+[cH)2d69es2.9e&_3D<A`T/)aqFw$9^lA#-r-x6w3m)3a7sVR9q86k+f;=SAi>(aoOe.596Pu-3F,@$OT'af"
    "DCHtOl<;GuZL7HF)0J]8nun+MGX&vPMV4H2e*F[7A[e>#jg[h&C$KQ&4.7<$x?Yv,)Iev#fBqJ%-:bS7INJZ#9+RS%h54x#-=ix0e>)CG7rZW#9-%12:Zt2(K[]o@Xmn;6:0fX-Bud/$"
    "r;Ln#$C2s#-aq;$+F:FVi#1onEkCH-a8&05N]:Y7)jB)j9cu>#Qbkrd%a=;-c1A=%#)Y(.ttFGMlS;Q71gPs@5,QW#u$aooa;o,MxD&SuxRwou5HUpLCvw;Ym?a%=7$_p.lwIp$JeN1#"
    "wC@Z7,W5D<+O%v#DDoY,Le_hhSpP)k^6Grdg8n0#=TE:e-AK;e+J+87kkwDMpc8metdJ^3GTo0M_02e0hbtM(xvs>-oQn68Kjdt/N)gk9nCi/:L3/t#CQlfQma9o[`Z7GYa.*1cnZU[u"
    ")=MP4R8`;.21[c4Z0buP5USi'2<sN(u#2##wV=3>)LJ6<ZD7K;i,H>Jt-Ax,<#&v$NNN/:M.RP&Tei,+fBDO%ab0:#.^;:&&FcB&?U[W$5ol##UmwJ3Sj*22F_M-ZB-<qo?QEI3pcO^4"
    "mLj(W85i(WfU(VQk;?iK3XdUQ$?+G;[5,W-;vQ'A_h+/(bK0:#PGTt:a;-,2Y'@mLHE0#Gf`*RNJR4onBJu?05&pu,V/Yghv<hq#OBcf(cj@%#Yvhx$B.m05#2tM(bkr;-woXo$ko]+4"
    "AMi(Ws+pNH$[f^8vXa]'^kpC4soXIuOr:LI8A$XT(mZX7KD=1<a$2Z$>?fZ4R&$]#jfPW$>@R8%`h:Z#:7R8%uk+,M-^^<&.FBP:n5_#$4awiL3B9JL%N6S@Yas109U0PA#S@t@oG>]="
    "vQ&DaZb*(N.&9/L<7?[$.B%<&]ATv#dn=d)>QrA#h,i1.@0;hLo,r90+S<c)_+&rddXwDPpnPv,x9sr&Yti,+7PP>#0lqV$s:;t$lds5&>bNp%$YZh-J%#.FgWV6p;*Xs-DN.IMkrlV-"
    "18bO5Ow+'_)VeCW185tl9s4#G$k*5S8H^G2xw$.$sZ2Z$4:fR293&W7i;+gha&Jfh7%.s$7/aE-]L-&%1?RL2wGguGX%pWRRF+AX$NGoIKwEoI-wu20x=Lbt87_BuL7YY#gi6onHpi`N"
    "^hHP/8#=T'9OPH)8GaUe8ogVe#D#'?RD44%edA#e9')m$)W6T%FF/[#]s*en.o'aVI0r,n4OMvX@^lk,]>x-35t8A,]#WO1q;m]#^]#.T2A.sid8)fT,Jalkva9;:'4`<J+$Fu9$YgBH"
    "v&0W-Ec?-m.[_c)OI2K*w#x-'u@[2'I(9T[Ck86&Y.te)L[u##K7eq$eF3,2tn_5q<&B.*693m0PBmH+,J[m0ChY`NnWt%HM^/mu)t[h?w$3H4j5Q4a$jmxB1FWh#R/;D5U'tV$bL>a$"
    "geN1#)2<)#JeuJ2Ore?$l4=e)NjCG)Zp]W-<2qH2E+4[$G^8%,tw;K*^<_x;S`0N(^XQlLSdQL2R':9&x^0#$sim$$@<Tv-oZr22S0#;/oBUq)^6Wp7S6fe*X6wC+`Kwg)V-`X8&;iuT"
    "/HCN06Ke$?6c0O1sZNr7Y-R_>tKh-+6+l.)?e=I2nSQO0A#2N0a***O(vYS/;/o&8owk,2b*3#Gaeho.m2C^--@xo%k#4*(WrS'+tYuo%&;[N*oERD*(ja>d=I`$#UmwJ3DwU#5$D:qo"
    "<9I12l]3@57lSL2S?b>-$DXI)e#//039#g1l7+&+6),D%x;uB#s.lQ#0_%9UQETj2c?GmuPjtt-E(>i(Mw%t/WD>98V04#G=c5:MmcnT.EWp[uXML`-qO)ZunKr[(H$3`+utM$#g_A&v"
    "r2:N$7kd(#Th9#0mid?$X[E+j%J,Z,A33b1k9)G4fT6C4Drj&'eihB#snFd;r?%9.aF?>#1@*1#9VVigTG-(#f/5##Su4;-u>3S[`S#s$JuYb+=F'f+lk5j'Rxn.2>XvRf?Iou$O^*&+"
    ")JOj+[,(Y$RPs[,Ggga#=u*F3WIX#AKY*N0dlOP(8`cu@*?(,;B6<X(wAv/1#^UD3J29f3S`jj10AQX$#t'Y$QD2U%SkJ8q(79^':YmS/5.=]bcen3:Sb@R(uS^9K`1K=7DReXqgboFM"
    ";SW</1X=t$S%IIsRQxp:YT1hu>fLW(w%UT8*IuV/;q;D+DUV?@IkfTKo`Q2ZxU>t&qDQn&i)67&RPN:S,[4sDI.tPW=^v%+prLJUI`:8%=8Xp'ZAXci;:@W$G_R8%P[u##mWO=7a8I12"
    "-rF12EZM&42n@X-oE*hueU.du&$ueu`LN5$O$XLW;OU582-YL)6mw8'*UYH)aBjl&f[^2%xws^-tiu8p_E]s$6O0L;7,s20*)TF4RPDO#2fxhuN6YoQl]p'B8@RE$4<YLk^uN3M9^irQ"
    "><pZAu%l?B9lUf:e#5#%FfTm(4.Rs$]tGR$YD,s$37<p%%G+87MSIb<hB0j(*2-W-uP=L#QxbcMa6.J:a>+J:k4cT$$OcJ$8ex=#$)>>#6e8(Mv+=$#xb0p&7]Cv#CFIW$7i1v#]*)O%"
    "U,^V$:bsp%&$2_A;#+87bQc89_d'N(d5Ph#WI+%$Cqh-M)OZV-KQkA#,m7dZxU]/IQ;o[t$rD/f.ZVBfIXdcDwe1p.(h#8eOXXEe>gVEe&<-_JZ'3$#$s2B#`6VK&:L*T%AIws$0%%w#"
    "[b>3%6+7W$C_aT%':)39$$dG*M;=aP[#<I3Ne_-Mm#hH%J%gA#Um(9.uHL(_B`_nuGOpiuGn1puLA`K$Z-RS#45'Uu$6pau,dP+#n%o%u6rN88Y?nW%L.6N):uhV$i=B'#Sjj>#TqH8%"
    "Z1Mg<:BIO(M(9_o/c?X-WFo:dOo?YcXU+PAakXaOYLL%F$`:`jsrev-q%ifU&>>s%8=m%$>I<p%6SlY#QnbrdIk*p%L[u##b5YlLLlU]M.O:D5dQSX-20wiLNbkA#$Z9p.<4FJLaRKJC"
    "qq6#GV>giuq:YY#83Puu>[qr$9e+^%3R(,)h:Vv7T/Zn&(Ae-)CX39%3o:v#ln0:#of_V$#Fcv,T,mV%af]8[KZov#4a&i%6u1?#o9(e)fVG@#Gx:;$NN#1%7i-W-0:7F%6S#HG7+@H#"
    "]]q'S;&5C5Zn8B3tARL2226h-@tZ6Ue?4]-,*nO(p_'v5?6MCs$H&8.hu-lJ,qDhkYIH'ZY)6mu9*MW#<>0]9l-'E<s'i;$sxw%#Q;a_7U/C`j4.-$,tjf%#;h:**<)Lg*kZ7)(%6P]#"
    "8$,,2tAmFaGMHd+YH2v#htc_+x,ntd7rPWe,D^Ve';b:eBIU6pi*=G%mw,@5Vd/@^,-%12p?fA5lhYhL_oQL26RG)4x1`T.Q(3]-Fj1l-(K%o:WC7<.]CI8%%T#t1XNDmacTqNMIsH?S"
    "T:ToMYA)Gun@DU8AA932raZY/[Z,A#pJ+a;(ggoU5'Iqi$kJP1_E:FVKh(v,ScA_-bdc)@]$mM:C*,##KG:;$n)u.#TiLY7JY(Z#o_33'K-'q%>h]2'?OjP&uOb&#J]>K:f<+5VFqY@5"
    "hs=i*33SX-t>lX$t2ig%'bZ&4=Mj(W$'veq+S<c)#/3#G#PFFoclVsu8oi;9Ugwu#[D&S[(,A>Y]Flr-@q$29jNKj0DLu.)iLdl/n&kp%twk9#TL78%:p4b++wTU%+a'^)4Gt:e:o($#"
    "DSj)M0<`-M(lH12'Lc9M4ms,:nA2H*9Ze)*Rp1T%g$4Q/e*@K)xq65/#F/[#SB:a#/]NT/?0aG>(1[cn=&kFIW-r,n*2^*SP?9I#gBP>#RFaP2[&[`kZ?,&-i&9lS>MCp$&akYJ8gQ:i"
    ")?&q<(#6pN)mS11v6j/4_)kfu+o<UKsu0N(NCE(#4aB<-o]GX4ke^l'p;Xe'%`Nj'1MntdAU3Daa_bgL>$7q7'x?B5nl^t@#.e0:'7#-3dQSX-]aCEuYxHF<=G[m0]Lj(WdIn+)@N)'v"
    "pboS#I0;D5j<ta%q:$##e20#GT(PV-eW+E,t=t**%mPn&h=5$,&5`O'p=Dk'gEM,'c-6#<]0pG3uYPW8fXCP%p.nm/kDsI3PT/i)XLs8<K8f&G$nRsVoujnL5G>ci(nUNdp8FC&tU8on"
    "<Ki`N8(KV6ls>40mT3L#pCtx+k-&q/5&120cQXX$4b#9&.6@Te(J+87&c#so&d<I3tC:qo7fLkLsc*22+^9D5DpMG)#0Tv-LqV[,g$9N0j=8C#.m@d)Z,,L::r:Z>c^0#$t#STKprBeO"
    "U2pq7sk>d=aDFW)#?[6:^&;_QANgeYm4uAlM_?:0pfMGP/a&L(NTB6&b=3Y7QnG@,*3uF;U8?4b=vV2:J>^g%wvw.#PlRfLVMZZ$spB^#lAR8#+Pc##J#w(#=TYn]e<<6/09QxXHZ@w#"
    "%tji02FNh#?&WAPudo92NB@W&Ww<s@ROsY1<02(AZeB>5Ql['A6hc'A_Mb^(4TI&#*uXfi;BdP9Qn,&u=@cWQ@Rhlo4#a]Pm*0*+F2uf(/@gfLd7F1^N4jq2.*To%05a>-XT2'#_bpU#"
    "?Re#M']Wcuce`S$fY`=-h:Hp/`h#V#4'>uu9v&c$DE*jLkUa/8<QGs.3TfERk9,R&X*u2v1biN'EIJ^-=ih2#ho1,%tP:@-@k5d.r.HU7Z]$x-ustsQd&XbNLUCU#b)HpL;ii&.q0RtQ"
    "'k2'N7wWuLmEL*#K_V>#j;_^#Boriq)s?eM%w4k/x%,##00/T.Q'GPg<&kv$all@=8ga;%&Xi5T8_u>#?#vk-Vj0Y1I4k)=mlat1cYCW$PqlxukPD&.G#EbN1*l*(@-l?-+K2a-hQg.?"
    "T8Uw0?Z0X_;^`^P;rAD*t`P>#79P0.e'LdMnvDf%38(##wv[##N<$/i3mA<%:.HVQO/Bw^:=Iw^e:VT+k]YCFY]kxuLB5[$X[BJi=9-T8lfi+MYpsi09Te2_Nf%*06=3dXW]s(v/6XGO"
    "X;CM#*Rqw-[&<HMX_2t0,d(@-#^NrM)]CU#nL44Ma5pf1&gnA#oPQ5M_O_w-S^gsLAxP5M[SN#5])<a-NXXF@ZntE@YrAD*vouY#1E$$$)r8k14/Sd+]uWuLqi/NMe(H@N5;nTMT4f`*"
    "drg._2Ivv)4Jto7kK8;2)+CYYIeLU21aW+*Rgs^oVWQJi28w>R=&xIMS'41^^c)QMe[qw-r*suLZBaO^/$&884`'#v;o<fMcU9d%Cs+##:dlO-ouL1.WJ,5NdfcgL26v#'6-oO9^MS^H"
    "`emM&`Bvr6/Xn;-6pVE-#=I3)e(vj)2*`U#T7p*#.9kZ#Y;C[-td7KWA1l48t`0#GMr?T.uoh7#eI'`$6w@5%J#6X$PPc##VD#5N;kkjL64#?0@8bo7a=?v?g5>X1d.nX136ilAUu(W-"
    "P@7X1Z:=31^W@Z$OYFgLqk<^M$$iU#@L6(#j0qA#wM$=.Je>q&(cDt-$@G,MFYw]MU6@V#ndrA.`Cp]+SNi2_gf56,8U@KWYdCx$LGlY#r&M.&r-ss-qF:U8xIh,bGE70^A>?q(UE]o8"
    "j(^GjWv<J:wYpM(h6Q#$P)^@)XcmX1l;OM94rLpp9sSvP92l48+64#GP6v&#bt5V#gEQXM(2Sn$rU^?%/klgLs?v2vtV8n8xMOKNR]s(v0*JNMNIR^$_4HVQEjjM1eRwG*Tt]v7YYWcu"
    "nLYB$b_Jc$r+KDkr^JeH?1i;-+uog%5g&,DhVp;-6+9$.'2$t7H^&WRqwL/:Efw5g+i?8%?b@PSDe&q.M1JO'+s+3)4Fp0%2^5#GM,@D*ZD^&6)GqdiWQpQ&d>i,)JJcg$VH#Z*Aq,##"
    ",U[=-q$f=$fXAYc;4`v#9R[w#adZD*&@Ds%j'.Q%No:D5E'BT(1vwmoV?wGG@T=,c17V9.=EI12mr,h2fax9.7(4]-4^k-$^#G:..K4L#'EC_&f&b.GT,rJ1bq-PA$*oxFV<<,;wNq0#"
    "),_L^$?+G;LO^qM#,Y:vx7YY#;L,M9Najfi9I&9gi>C:%j_M<-i&l?-C=OK&w:-O9gODW-nt&Eu7w(hL1#f0%@Km`-Lin,=b,LQ&V=jl&G#dQ/M+p486^4#GM*m&#=81]3qrl8.k;Pp%"
    "A'icjnS=EcU/F1^<,KXR=8R/Lix05/MH*##UdMig?QMm$PR4onL%Fd)wlNigNbr;-pZGl$kV8nN*ksl/Q%T49H[&WR>/D>c(^)2hPBD^#=aCxLi7NK_6#1c`d@HR&%tji0whov$D]l8."
    "HtBMhLTev$`-Vj<QCkNk'M3PJBB@W$pp`gLu4pfL?@(@-x#?D-DN;A.%%sn&[+PL>gCFv-9qs(6%w:c#uXm;#.0O?#1uZs$Kd9B#rx*87@o#O(`-]-Z^P'=%gP3/C?0;>,'A%s$>Uo31"
    "Q0OS7PcPZ#c2]t@'EQu7U6?Z$2<Lm9DZUAP*-WvnhGqQ&<O?8'>)lw&2N3Z$v5&F-S1dt$h<+:`F&YW*jl'hMU5cw$Gnc&#)m[(8hWrS&,'R2_.^.*'LtY&#N`l<:?25SR'ucDE+l15/"
    "T_Mg*4H::%BO+i(4K:Se.AF.)twj7&ioae)6Bk>7-sE'%(oLkL[gZh2p.'u?H6;h+Fk9k$7Mv9%GO=j1q/;Se:;-X7N,%i1)p=)>vc*PAs)p$oZmT<:5Kg_1YC/),qB)%,;lr`?pa^],"
    "OK?DuFL9cHa,jLlTf<?Ms_:0(6>dt$$+9p9'p/,jkBsjM/O,U&$l%j0Ll6=-eG=k%HnFPJ/S-W-WR7X1l1*4:-R9v-2hv`ka`5D<EGqs-&Af3:Dn8WRB-s]FV[Nq2'e*##d3v=PIG`8^"
    "uZ<20P(>T/.'qi'9C86/c:@`$9D#PNdM_lSVo`8^OLQuu]]Q41v>]8^l^e(%8Hv%.7/uoLBjZn]#L]0>Y#f7[]K%x0p%2T.Rn.T$UMII-,TII-:K>@0:m=6/PT*)5AJ0rMbbZ'A&rOJi"
    "fGDw._6wlLa+,W6<,[t.*(65]ic8kOQND'#`h#V#Y&>uuJx>d$lN9@9Z@/)3G[<&5q@-Q'lCv@9$Eo?9`<R3tG7OqVwTIxtv_TB#%##w)(MTP8'B`)+ZvMAOkpt%=(N=_/S/m)+prLJU"
    "38v'8.^,s$B1?s%7DoA#9L8<8M@HX1')u.#Xu_Y7]qWT%k(Ldi[vc3'7R82'bC^i%$`-6'g<$5f_.5o$.[)22x&dC#=EI12Qj.4(OGw-3;6RL2k'4]--)B;-HqYl$bogua:CTGM3&fta"
    "C01o#@lB>58`-Rd$_[oRU]a^PSV]58P4H5/J7De$j[%5S&J*,)5?a]+lBQ#$n`^H'8p^oo)xt+j>rCQ_8W%iL6mn`*(WPX-RInX1dODSRNY.F@co6X1Ws^i:Di28],=q(kFg(I*t`P>#"
    "o;_^#U1Tf)NYU<-V/Xf-Yw/r27KeiBKd-GMj#f.MK&[k*#J*u.U^V58NSmq2RMc##5L9:#P6w0#NYqr$JHUhLC9r:#+Pc##K/niL9FIU7p0LhL`ei,)m[YQUVD(@-sV_w-ZVYi9&1O2("
    "YXP2_x=FE'J&WAPFmZ^$TY$x-9tbWQ>Rhlogq0^P3I?M9^tnJ)L;]#$P(3:2Xx_<-bJL@-kVck-V7X_AfxgHQc?,R&Z=wp.vu+##NcE_A1)s]PV:b_AZhP^#@K(k'qgJ`AM*rq2RX_Y,"
    "p$@p&:JQ2L$Pn@BmEX&#wte-%$-OJ-=G0,.'h?R907'ZH:'=d;_TQX1V]mX17/%m&g`),)'l82''mwA#QOsY1tD=F@*t=X1DqJX1SA6X1dWC0(RnG&#-S4W.<_tE@EF/C&'82'#?Fj%v"
    "5Rc##oB4I#U$v7]GfL0_odHW&H4%W$[b]M'd,('#uvR+4xF#W-.:p:^A;`P-#J%m-k:&eds^lm#n;_^#cjIp]ZqNP&;CWT.atBj$IE:@-d?t-<pJ?$$3E$$$Y<bh)H[Xk'i:Uf#KqsWU"
    "%.n;-Z.m<-*'.)B/1t3LGMYV$1Q@W&X>XciEw3x#)bJs-@_)@?r8#XRsQ.i^JO2#G[5)rdP'K7&Zcqr$BS9Q(/ih;$4c:v#u%BC#RMR8#fT<8pYfED#FXP=?C)^g2G@H>#2ZMgthis?Y"
    "=_xhtgW1Z#RF]$NpX&vP2dU8f#)P:v#9aGMf2?`M@ch2#J9fNM5@%6g&>uu#gi6onGnl`N^hHP/-&V`3N,0r0O'5n0BiHihnkk9#i3,q/o#Y(&YU'^=Whgj(I?%12KJsJFPA^$&'fST%"
    "gN7g)Pd:)ZSCmQtxPN.XTFdmtqQGYP1rjE63AxZ.6F#C7.V'X/'Nj`uUo+L-gBN+;ENqKcuYOXhGqtw'q;oWhuQj-$*jM4047RW$#G+87^4`fb?&(Z-wLYS.q4vju$2?&%@C`T$#FG/$"
    "l_K`$CEEJMd1T08mRw.i8hFW-%tu+#xsCC$A6>##5f1$#wCIX74+Rs$91*p%<.iv#=I=EZ7xp`$1]>dGew0Z>hZUv@,OuM(<C6g)*3,F%:=v.h&%]$$`lu*7$JQo%Q,Cs-e_bA<rLdZ$"
    "Ew/i#1.N'M.9-##*E3gL>@=V7gh,7$`>#s$U-7<$orJr-[T4@&0o17Bv]8WR9=0PA)%#XqC*$rdmCp>$t:3U-F^^C-^(LS-6j`i/I2PX7N+%s$xgE:&7i_v#2%I8%`]hp%McjP$xWocG"
    ":Qek('?%12UmAg2-pRfLiS/@#7`h*%_`VkuWjhjtV:McuoVXAuNWGbu+DJX#uacF#=$<%t)A#HMFiU:#Bx6W-S9pw'2bZ3O]h/A=>(Mm/n8%&+w8#,2d(;Z7<_3q.k-lgLltxx$[RNO0"
    "N>=F+tLe`*$?Wl*O@8;QO%dU[VZZ`*.[)d*91uYdYv:k'a</[#p]%P'av53'c.i>7SYG)%%Y7B3uhOV(W`Ys-DdBT?B/5sKliic)n`WI)OgHd)-TBd>b`f+4ZtFj$qU$Y-#C7f3(0-$$"
    "S?1mQ+0fGe;diEI$+Xo:*KHF.n*Rc*b(NB#1RW22p]Tw6&SW-?J[#-Z()V>MQ@vkEOeGdZv/>]L'C;cu82<Z6,e2b=C.Ev#h<*52dDe;/')>>#Ov)m$g`:Z#Z%###S?*1#A(LF$[(+&#"
    "mIbo&'BRiL+;D>$,ue:%oMGn&b3H>#SeEp%B9&njH'G4:Uvt$gsE)i2T3=U.YTAX-i&l]#Ei/gLDnA,MU5WP(f?S:mLXLYPoBK>.aNM#6KwDYPS6.3RIu=Dsq4oduK^K32+&XS7PFXD*"
    "x6gC#USWQFCQ'b#n1m?TaUgKYZl&D$0@$(#X_531e^*-*R#+1))-,8/]ucci+WD4'DekI)[%=I)xTg8&`*[A2.+/f.aWO=7;7`W$=-W6pS;T;-k]n^$Pr+@5`7JC#@oI&FVSE-Z=NBjM"
    "WjHL%HeKd)69L#$P0(S0>`lT15'0&,DE9@uE%+>6jpS4V''?aobu,/2>HvQuTc^>$vj^g>)/RV7@U*@IQlHYuw2k=$RF[sKlOn;NK-O$^]-;a@,)HYuaTi2)R`O]>Dr[Z$D-_^#aMseF"
    "0ACM#8L^.=$uL5KR]<.FbTbg$W9@B>Y-$ftMZ&c$bDh2:uZUAP0WRw%e;hQ&`Jr_=GL1Q9%'^A=+*R#$e^*Z%]+dG3YQ,QBv+sh1^xNZ>QqJKsd#5SRbH%vP4Y0X%l),##6mt6%p(u.#"
    "'+F[7cxwI2&=W=QVbR@#2V7U%_54ci+GY>#1k$+*93/<-Bh86&P*CSeFWhc):i>8p%[)22<KEQ7JvP<%?6W6pB8Hs-PBrHMRo-+<V)QK);Vd8/mM6+<5%;9/s$av#MV'6;K2-E3njm2W"
    ")Qp_s,3+l9i>h7/Fidw-4M;qB/olF#08B+_#fX/OQ;FK%LQTY$a[1@,g3>n0AV2Q/'f1T.CEsf:vgQ#5ZbY+#/6X?#_2CB#>J69%%8[m0?Hbi0v^*>&0c`'#%0o12-nII*hcXS753#W."
    "LYp;.$Ls'kY>2*k24ti.b?,,20#jGaE&2X.;o_P/hSEud0PTvd'Ij;-='of$&x3;(L]u<KJn,@5,S.B3;bW#AIH4AFtYuS/wrTc$$I%)EH`QS'ch5g)AQCD3x1f]4@E]JDs;]WB+ZMU1"
    "x*T+%;+t)-'VBZ,4Xl/2amcx#vB&`ab2fS1irN8&Gq&6M@12]5%efthsTZs;#9H-?O[ho03R4S9ci<qCvW^>.t'x5/cU2s$JG'/:iUq8K=/=A+*<iA+jP,R<5kP9#%/5##r]kQ%9*u.#"
    "q&U'#&b7,*e_Rv-S_+:,mSFciB/U>*;V0vd)bA^)cX#N%fKwrohmYg$an<m/5^3@5M+(i2GnxB&sP(W-4,-F%:[#BAlf7u%l1MA=IN7t@umX]55p_E<wGqq%TVjKI9BSW8(lIJuB3G[L"
    ":p76:1tm_uW>Q[Cf_s5&;[Am&DSV^#]rP92DBHpL'i@C#&P#G4D%N9`18(##m2T9`vDB%#uhwZ7Ilqdi&]ZeiY-X9%0'EO%e?N>#D5on'0-@w'Qvc3'f)DK('L`G'fT<8pWXrkLP/O;p"
    "THWW#ixw-<q6X2(>ONVOdN_p$+:hB#I5^+4oPq;.c75+[v.'qu&WVO#V0#k-r,)oWE?+G;/.(B#ldOWMNH-##BYqr$PF;/7bV1Y7wvx%#CvN(+9]Ldi1GbUer(f8#_RcC+CgbL:;XW/2"
    ":78*YWdH12UmA,3e_[5/S,m]#/`r`Fd+?v$p=Do%RjPw-Vr**6dbmu9X`kW-w(Lx?fURF%WL,$(&Iam/m&kdu1V[a>@Pk.)^_ZmLkRB%%5SSx$t)$$$[bS(%5h4A=,B@W&,B=6/wG-R0"
    "-sZa&F'D+rd:T#,(NHDO(7LGj)<4S(iF^Y,[fl2(#l=x>G#('#cDgiuTN_U:rRhW7BfU;$`]0didhKU#Gkjp%St-s$%`m]c87TQiAR,@5I,`Hmrs`v5MUM/:[j^1K.vh(W_-3#G&b5oe"
    "xx0#G$4t]4JRIW-DPn9^'g#X$Oo4DsJiUS%qDU`3jIb)#'K4?#F12$#I4YciknUG'XX+Vd($`sdLeH`EwQ9to]s?12fHT#A@5_s7`Iem0W1tM(WSVi4(52^#$LG99Bo`iupXmVuEQs>["
    "<8KosRJG(5ho8%t%-82N`#We?\?fp@%mmmZ.cuJm$GD@B2K_R%#'LD/)`t$s$suus$8^bU&*1Xn&&vC0(i0_4f(sa6fRXgK/q>9B3IQ_O:IB-4(fsQ4(0241)x%8&4Lj9bE_Fd&4m3-K)"
    "S5a&+7uv,*FbW_Ptx2fLNm,v>TER=-cLES93vL1Swv6Su@[Q_uaqtI#-qUVM%:0RjX(ST%d),##2c68%n)u.#]nT]7GRel/B:]%#Q(R++.r0f384t=$4ZHH9D^%97I2RrdN&ds.H5BE+"
    "%?TS%UmwJ3RA,s-Wtr3C*S:_]pj)22.6'N(IMYh#32UBd;n,Q'PukV-Y7o]4[weP/3D0N(S,m]#O$WJ:^ZLa4v-`I3Tp'<.PjHd)E2_/)D3Jb.Ckv=uCHJA,wh9S7s<bN1`,Zo/F::f)"
    "49M80X0T;DsWtU.r?/@#Tuwx,sfL2:XTl_I4UKU/FJms.`:0G3s/M9nwB$^(Cw)H*Wb9E*pBvUZx)RA5#li;$5xsf2Y)>b+nB[A,JNk-41iMZ#$Dn7@1_6>6J_G_7,-k.'Y]-s.5tbt$"
    "#2ni1+a.S@odtP0.80P8$),##V<fT$bFw0#EnT]7MTlk0d:BC4wOOV-,ZHL2DC'o&+Tl01qGcn0,Xsf11)#G4xqrV$CEw+*9W+dDE'BEYtvKW5L>=Z5MxO12nt+@5$n`i2RCT&$U>M>$"
    "]q.[#.-ihLxBV8.Gq[P/QTweMQ1>)4]J2T%x;Rv$P:l8A@Hw*6LC6Z8C06M;#6U53)'6&J6KN_Ko9kxFG6+>7R<+(?1-LZ79f?R(WvlODGJg^+,8wD6h15Q1H_%5FW6/42K-sR8JF2<9"
    "[mnL2^bVC5XUeA6'fL?5Fggw#c`(T%RgaY#;7IW$FQ-d)*OR=7)0,i=^:'5sXfI-FK3_B#Tp%P#.7_BukY%a$jhwP/=tU#,#%kiKgJp;-6?r*%5@<6/BKGR&]1uk%TZGW-x(u.#jDlX7"
    "7Y_diVI1[,wb>3%47's%c<`G'XWTq%HM+OOlZ)223VuR%R7XP*64158nHKZ-OtUt68DUx&#sBE8R+>)4bO`8.$Wh(W4n3<-AvGx%,UB=8^8h>$ppbh)F#&'#oAoT$)Jo;3H9/W785Q_)"
    "gA4civ[TF*qgPjBlmF5p;v.E#uB.jBIQ<j1a$Q?IxwMi-n4iQCgL+PA[m]lS#]3U1*IVj9ICf`t>MCf<5L25gd`7Y%l3sqKZ1NU/:`1#G,N(v>&f/E<F[kxu03sP%P`:Z#?$g]>`#[w'"
    "Pam+M3'Evubd-iLVcT%#i9`F*7USM'iYg@ku*Z7$G#n`*j$Z/&)A/a<ARE/2f+`8.lNRX-sc#<-MA#d$M65D#3Qa;b;Xg1Bf]w7MU8dx6Du+A>iY_$-u^B`3$n#>5oq1(j&j@I+1lUv#"
    "RHFp%c%xL(HFvu#vp%##Y`<qMZ^x;-/)R:(rr;E<Ip8K29I+c<b+md$JQl:%noHm/pC0b*`p,3'Q;,`%TW-F3Zd*u@,4_B#[`Bqo,&+22`(VL=vb`+?8DRMXpgEcJK@vkE;DeiXiwHFJ"
    "xB;cu+$GD4w-:h;5rDv#SUH42Q-5A-0j0:73wJm$o*u.#AW]W7rhKU#`c0dibt>7$?x:?#Z5Gs$drl]c.,TKWmaV6pw3IM9VugN(MUM/:*N_/uSHhh$=[3j9Gx0'#B.:/#h%'P8AAsP("
    "vBb7&u@`9%%c[h(QO2$#Hd[/3YD5C5/OU6pRHDA.+fXI)BI)gL-]uM(u;Tv-Q$?%H5o)tfRZbA#iw.<9Z`WMW[a1#G$Wal2c<A@#C4gM#D_Dp/n,k-#%90W-Xqg'f<1dn$[;Xci][0:#"
    ">=*p%Ti<T#o1-bedv;jKqg6h*@mnO(tp=*t^Wh9jKL?hcfJN5$r<)m$5?O$MZFW$#eSvx#K/Z`-X<3L#EDhditg71&?q8q%a#<6fRg;Q1gFH=7'QbK&W%EH(t9JC#B?e5WCJXJM6,H/("
    "qWh^PKm(o@Vl@>5KIh6E,3cm/%/5##APUV$sx73%5S2K*##.=+TQt=$8^:sdH72$#&JH7&<X@KsAR,@5/uin2=*Zl$qm8W-8mb3FmSDK1oxf?9uv4qKeLc:QOGY?e'.U`$Et(G#tB`.&"
    "w,0W-'_,XL`/58R*c68%kD_]b*1_^#c7(##O]<R*(t?D*D`Tm(p8=I'.X1E*RN82'?\?O-;RA;FRv.[-;Gk0^#`Je+)6eaW-wki$7cc6(#o95'v?[#R$-._'#$DIX7e=d&k_gO&#:Z,)k"
    "InsjhZ8I8#=(+,2=IwCaHNX&#2+@20*iTvd'_1sd$v>6qGtY@5S0Uh;`tS,3bF]9Vo3H12IhQm2U;2]-:.Dd*]@SWH:/g2BT$D.3G2N`#gh]&$/&4616ur$-,-j.-A9u<Nve=#:,^xRQ"
    "2?k[Bo-jD%.<2W/6`db+:3Y6u;:jC,vW^>.R3dH?(RO9jOKBLSnm^7L)6OA5m6M&5@VZY#Kn8onr&3wga_hWh'[8Z7Ct)4%)x$p.i'm>#AxJVeC7?WebQ65fmlk.#P6>##173p%)*Eb$"
    "k94vo^;#s-qIW&N-j`qoEQb8.rf5x6sOnwIFL=j1^$ft$hu.&47BG0l7>IA4$Wh(WD),)Q.8Glk@`q?R9I'Cf(bC@#G3v98;)bm1mE->/XT,A#iJ@d2j4(A8m96XO)<&q<G*hoFluni="
    "B1Ji$tlIfLBb;DjYECD*+>3>5E-Lq%8gL[*?nch(l[jj'q'vd#n:n0#4<I=dJ:no%l+=k0L@3E28--@5bkuM(xH0wn+f7C#K)'J3o5e`*/u<T%QL*v#ZZGo/?-qBbuTm^(;)=Q1JL(Oe"
    "ZQ1PALX`s/ZVup8OP@]b=G-[-h3658k:%[-wWM=MFSeUQ%D0_+S)d40`[/'-;AD)3)R<8%6mw8'[/'?#YIP##9w#&%SNQS#^jKU82wr]-iTJ[#dmed#q^&.$%TjP$rQ6Cub*P:vBD&cr"
    "LkBds)lW`WFig4/oPIBPLZouG&YQ8%SPDX(P:=X(gbno%(uNX(#miX(_5]CsiU]fLnsM+rpMo34rSA(s(^<`s/cC;$Gvaw9jS=%t)H%GrsV#2Tq88Q0&lvS%UQM#$,g@U.mZxt#lBDX-"
    "WbfX(<[9e?G%OX(wefX(&@g34[:x34&lJX(1B]Y#xo3Mg]#jigxA>G2;O1N2$qA#2r`oS%%[<T+t[DW.?>Ih1T4Rd)&qv8''R%##koTb&ZI(w,)($f)ni9#-.ET^36a3@5n=,22Ur+@5"
    "QI3,2B+=3p^]u;-qN*e$oIB'=$htA#wcm&QRjh%.$s9n%NKtU..[Aj0aNLmCgBM'>7E#?S^78oBmrgHFd$kZu[3mn0H-Dk19]i.1#i]H<(gZs.1xHc4k7=9/6SgUCZ#2I*sh/_u>^@a5"
    "f[@&6&c5m0Y.Cs%_#>?\?9o&v13BaMQ6l^._=$o,+wfi#-0v&9[AjmNkxiV#-j,UHmej.4(8DQ*n216m0T=.bGes9-%/ZhAFMF3j1Ck,R_%;W#Rvl3ZS@?'E4uA^WN*-U]XrSsA#NeZP."
    "x%'6$&QuO(fC9>,@Ad7]E6#6#^YV3'>U?p7&BU,bvAUZ$,(Y8IO0<v6]8M]$6&1[#ewK,Pj#eG/&Tw<B28kB#xP>S/G^oa+.m:P0EE/*+IM3MEK,g,a<;[ABu#/BR^ZuJ45)0]?Am1K4"
    "42^=@wmS]=Tnkip;BA5/I#k%FHJ`teO$cc)(gC_$Y-u2vjwM6:-anx.dxqw-S'q%Ms?IU7Z]$x-?+1$R*%WeN'VCU#A`2E-tmi&.^B(@RN2?dN/H.UP$tw0^`xp`Ab4N:20A.:2s)D3$"
    "H,^5/6jUL.iX+O;aZq;Kn2wT-,l_@-?7ukLExZ=.e4NU91HL:D+KHR3j[>gL%3%/8(i8GjEp@/;f<D,<j<=q.X%fiplM0FRQMc##teO.#_/$I%)Uw:N-s1'#$&.<-cfG8%T8sCsxJSS."
    "##vY#qI$L:[aGO:tpwu#*cw2v2Ss'1XI]m0IX](#*]U+4%OR'AYZ8M^*NYs@^_vG;l.$L-l/n6%SHQvRM1_KM*P]%^?Gfd)GLevPYD_i#_-n6%J`K)=7RtY1Uqv^]VWQJiW-u'ASjd'A"
    "%Qw#2ECnM(6/s]P)/v92p)D3$Ys.dbT;O1^d>/@BaT@dM<n_@--r?x-uW_cM,^0=Rx<wT-+P:@-5^T,.ED)=R(>PgNSS3CV=2l48NFligR<v&#c'Qr#%hF@&CG1<-I6kB-k_U).3U;-N"
    "+#e1%IAVw9ZdSi^I`Sd+e+73#C<9:%)^Y%02.>>#[Rgq#u(*j$tv5Z$(4f/171cHPV;(@-7E(@-1F(@-jD_w-E,)APqmt@=j;NMgR<D^#0^C0C_//)tSw'gMq:Q:v,Wp4MDE7I-X8?D-"
    "9-Z`-NoC@0SB?^PqXVp%Q<7I-6jf/.4*M*MmjZn]Guq<1VK2YYwaxl'p51$P/,0VMs?IU7Z]$x-@Y[uQY-;hL/`2E-<dOk1K_V>#t)_^#1L;uuIX'gMSq`^%*@:s..+YfiDD.eQ&r?eQ"
    "&E@B1crjE-%J,%MYWmO-+P:@-JmDE-jD)P-3AOJ-n%)P-Y5%M.jfX&,hP-E>[pp_M%XnU7'pA7.3W.#Qe1l48Y%mf_5A4'#d0m7$CSZ`*(:w;-m3'C-p+Uh$B2a$8L'4m'fbUPT@,.U."
    "m>uu#Ujqw-1FbD8_T;U2CK0DIY;(@-'xtoM&M(:#G_2E-7Z2E-q)Pv'_clM(4#a]Pjnj)+E21,)?/c;-r2l;(QiIR3$@gR3d0`xBk[76,h-'n&q?\?GjaclM(%kC%JG6Launae`$=Cx#/"
    "axGxk>=2E5o;PgNu$Q[N?n_]$J*(up>/Z^ZfPmv$n`liT`aNP/'$329*`$s$hC(##AE5Q#05J+rLaR<$]vpi'e0W&&:`HlL_m;Z#_Jo,8N$@WI[ZD`sn=REn-PS3XC4v=P)d68@CTPQ#"
    "ivBB#(k^/^sAH7&>ETT.7uCc#3:lc-R8m92SY:Z#l7Auu@Qn?9i(>wL4`nb3(qs%=v_wD3-E@W&&1^6*_rgo.%i:p.oNki0s-]X1f9'Y13]gU#F);,#Dcc._cZ96#u80r.0I+T.d^`8$"
    "c<)'5foUY7=b;D+d4f+MvCbe)-U%A,nl&^+9rql/NCVV$1J+87h.Fb$#1.,v8f*F3d,(<-M>A3%Hv6^7jrS+4RS=P(LaSi)en(d*G(Sn/#%YlS#cWo[]RF-teE`f@-c:Vm(qs-cNhG>>"
    "[KlqSWc&aNadE5/7[)##qM^lSml;9^V47(^]tws$2^h5/Xn.T$Xqv&MWJ(@M[@CP8;2>_/s^lm#7N_^#od$%^oJ5',plcvPX;CM#dPqw-[&<HM'F$L21d(@-G%j&.ogR/N&PqkL0thD3"
    "IL+_].5]fiXHr>$)^WG@#0VF@./MX1_aw8'nQvO(SfhV$;44:2-Q&HMs(+aE2U-)kfnlKPn)D3$06I#$WmufrwU_<RZ$7&ObwYwN5Hq0Mt.$=R'b,x%*0uX1&i@X1,R4E,A1l48p%4s6"
    "^_#'#c'Qr#/Umc%r;rUmY4&;dG?&v#]?x+2Tix+M&]<T%7u6s$Q;s]+hOo/LLDmi'YDubjwkUC5m@#12,D5C5`1dF+0241)4uT/)L;2T/.,(M#c-EC7s0gwsk<gU$<Rh?%N5$JTh*P:v"
    "Dqw@=.4Tk$Gh-58(#2)FTru;-STXS&/5v=-88[l7^9PQ#aVF4%=_,Kjo;mQ#RquZ%1'%O(R1WM-Ll91&4hFa,s>P9.0XDD+r>lp.j)A%#qMNgjs0/B3gQ=m8m>wY6.Q:47Q)P:v.-t@="
    "P12d;g0F]b/#,x$d`B$#@Tc6#^J[-^Zh3p%mLvYPYG_lSY$)'#%81r$YC,:;Kqs%=JFiK*`GjvYGLRs$Uo:$#6*<a*Tl[T%^4Zi9S88L(WQpQ&mLVS%3(r;$'55##bI,N%c[q((4:NP&"
    "B=V$#b.i>7g/-@5Oh/B3r3G@HI*#H3oxpkL@.8v$*):a*U`#97N#G:.;;8UMCUYlS^b7UM#D1qXvZRP/(LdFi>2)JCF]lWISE<Dj$ZF5/kuXKu14v=PUb-Wo+Egf1Y5U]=Koka*>?NdM"
    "kRNuN'kGwN$D_kLQWuc20d(@-JF(@-:Qo314E'[#8#u6/W-xf1L/HU84C6MP<qbq2=K=ZM7@v2v<)]EOO#Y.#o-#'vuXkE$*_Q(#]O>+#r2fq9<'?`#3r,44eo8i/b:/#$1bMr7W#Ah2"
    "&*FB,/NAA,TuQ8%b(S:%Wo,k2MRZs2R#TG;(VTp7YCj11[D/f3W_x1.NABD(Wk@%#$^EQ76^)22Bdjl(]`uS.5A#12)YhW$2Hg*%3^9D5uZC3pe(Um2a+Rs$n_B+*o]m$$R(B,&Z.ZmB"
    "+ZDh)NCI8%<J]C4A<3E4ru1'&f)<a*,HHT%aT[Z$#Ri)+kGti(k18f37J7]eT2LHJ.6VgEpDkI3X_%lE-njk1]SY-*RHad*M=vYuSb@129-%l1?ov(+0Zk78DgS:J^;+UAE/[j')8Q0'"
    "9S&p&viO@?jPLX@nWw%5*rOPC[`D69B,(A?#r:f)6lbi(;OR7]lu(Z-iWe,*F3-P-3#Ai1Bh;H*Z^S:vd87uYo/@K1(qs%=NL$##;BcQ#Qki(IK'UsIL#bJi#S6D<*LJW->l7W(eek02"
    "3+Bd)tPRlf+i?8%3JO]O*,Yj1g5QJ(GGKJ&`wZY#R[$###2qf_5FNP&EcTm(CREp%VW/e$iJK2#u$nv#$>+87-hBIp.3_BuWeF6fO8$6qjX9N(PrN/2f_*D39D)D3$wFFV:owUd$[j(a"
    "?+xUdIvp4J>^Q)*5X[,Mo,xc7LG@A#TW5;+c`QV%HZjm+;D*x+lJ5A#QI-D+S096&i>vn&8QG^#[gOT%,&###`+%A#So?A##LQJ)a2VJ(NUIA#n6v?,CegF,c1B>,bY?V%Fgm,&ST<8p"
    "VoYC5nl^t@*rQ4(A(=3pgJ9H#<N<I3u)oO(h+/Cu2Tk>3I%j>P/9W6pb4kr(qlSL2vJV_4CRM8.N%f;%'a[D*'54Q'OiXV-jfE.3kpe:J`F8E%69HZ.r]&F$f-3xtplVO'PQe1C,W.&I"
    ".Nf]4hvAS%C7Y,4$?qoIobdw-`9&/'?+,Q'Llb0=qd]I_5Dh#*8E^%#2OJM'XHkG2VeYY,;6l5:AR[W$G-gQ&S@B9r=k>2(C:7[#M'TM'[mGn&AVF+*Nt%W$9b+/(v9_dM0_7;Q?0sY#"
    "Z2_:%J+pi(AV>>#JO@<$Ch<9%JgFH3cx(E(gN7I$@lC8p`%TB#l=cI3S8LIN)U,@5qqr-$h3U3K8hF7cDGK7cILS.2g8Kg2(94c4Q_e8.62M:%03kM([l$+4aL.s$'CuM(]+J?UqWf'M"
    "?L<TL2s2JUw'lRnJObe#x+;0Nv#nE#/).-c=:o-$KC9V6o6v,@(;K;cDb%-#0135&cfZGV6jx+;W-AVH#d$?.ggt`FW@eA@F_3l2N<q`39kA?&pSxTAXsvu5da0;%(f[%#=J2ve,&+22"
    "dK/Z3uF+F3qs`@5=hU_&aBj>3V_Ds-3>XQ'Ve-lL1o<P(5[Fp.S,m]#b%E(=8)Ne-U%`8.^Cn8%?5e/1L%;v#1S>c4o&l]#6D3#.Pl*:I=`bA#:$.TIuT^:/rtVv#^K51Hxg=T8<RA<$"
    "GMo90j.9C-PuD43a>R*,Z-(DQ9f4K;#9Mn/.K2K(nZ9R/(Z@d)N;b]+VZ?r/$X>*,]?G<BIWF`#l[LK(o@=M(9n[Y-wU+M(G/E[.xux<-Q-J],C9ft-P?P*%&gGMTah.T%)Wx=%o->M4"
    ":58$.e88&8P(-CH<ZsvB>,9F*)+G6'n8Q3'P^>rB-kcj'ugwH'QCiv#h0@(47nsq.4k<u-?wfN0Y2N+3dgsH*Z>M1+>EYx#vA*X.$),##[Yqr$%Bl6#FR)V7Nh.@#)Q$V%QLrr$(se,2"
    "/Deh2V&1#$&ctM(Yq[P//wnP']lIg:Ve;P(KPxqr[B6]F1oaE6GR@5CS$4]FpRd<P49HxXm>-nk,P'B#5i;e?gIG:Zp/i?5UU6cud,snQmPK6)(0&##Y]Q(#Y8uoL6pG,*$Z:D3/M2Z#"
    "ag5r%s+XE*rl.GVi%^F*wbL8.Qvv`*r,E,tspvGGE]+qeacsfLrCfX-]Xj=.hDu-$[7K,3TTF=$3$34Bo%#b[?&`#$AZgquY$t0#S^_fL#YuYuRh^/^iZ:g4+3@C>Qp(DaOIM)qxJuxO"
    "L3ELo6N^(.Do`Z-lAZ&#uZP+#C9'u$q,3)#xCEL1P4](#mQN:.E3B<$+/'J)(rW;20x,veOl:D5&d<I3f<a>3+XL<.JGUv-e0w9.W_&9.Df*F3rO[20-a0i)F^E.30mGb%)a[D*pe]e%"
    "vfd>#Kw;S&MM(d.o/hR/7.9_#POBqWtug*+T2?dK'&tT%MiCtQ7UCP=&-n5+`+s#$>^P/$<Rm@F4l^U7F(#kD?_(1*$D7;?m$m2OdxITYEQRFSi,9PSB-7nVT_tBH#X0%#$),##2_S5#"
    "rYO6%0+IW$r6>##V_.u-7ouLM[03MM]6<MM_BNMM-sFZM,go#MY9W$#3rA,Mf9W$#3hG<-AL`t-ogEZM3mx#M?24JML8=JMqil&#R=s'MO,UKMT[HLM)rn$v^H`t-PujrL-4=%v2UGs-"
    "osiuL/=4)#2SGs-4X=oLH)n+#ESGs-@Oc)MNPj(vSCg;-&OD&.(NqPMbJZ2vhRk&#RKh/#W(V$#USGs---wiLDJ=jL@pu&#T26&M5(SQM6.]QM3:SqLTi0KM?p('#2ND&.1l'hLqv1'#"
    "Qppo/oT0#vHf0'#5qN+._+;MMwBNMM5(SQM@r%qL&DD'#](k9#].iZu?UGs-_:;mLeaamLm,D'#9-:w-tBonL(.D'#%mG<-xGg;-MRx>-0Hg;-^wX?-;Hg;-`(A>-&qN+.FdsjL8Y/%#"
    "'_nI-QCg;-K9xu-EkQiLPFi'#@Gg;-R:)=-VDHL-ZA]'.Ix8kL+Eg;-7``=-;``=-5mG<-0Hg;-E;)=-;Hg;-U;)=-NTGs-OKNjL:Kr'#LGg;-TGg;-jaDE-Z0#O-Uk]j-()Ik=x(EV?"
    "1D>j1uUBL,V[?j1>v,:2EvQX(;eae$ax9j1v>dD-PND&.^.KkL%Eg;-7VjY.4s2%vp5)=-xXU).f%2MMc6<MMCLb%v?Y`=-/))t-DOc)M:F+RM;L4RMQsFZMQmx#M&]7(#<A]'.?4qHM"
    "DJ=jLJ]7(#/Hg;-0TGs-<l'hLWc@(#duQx-Q?<jL>KhkL-T]8#dS]F-,nA,M:j@(#xGSvL7Lb%v@Bg;-MRx>-8GwM0exUZu;XH(#5e,tLof1#vsAg;-Y[,%.wn.nLqiI(#uGg;-A``=-"
    "4m(T.p]^:#4-OJ-gSGs-/93jLF]tJM,.4%vWhG<-JDHL-v_U).l+;MMc0nlL:w[(#a9xu-]vSKMRIhkLk+](#/8':#Mi)M-eCg;-g:)=-jrA,M]%](#5`LxLwaamL<]-lLGao8#3wN+."
    "56LPMDGVPM7;)pLGv[(#/mG<-F``=-5Hg;-E;)=-;Hg;-Yb:1.]q5tLib6lLP<$##Bx9hLjh?lLkHc9#k1%wuBTGs-(mg%Mh;Gxu0Cg;-@Gg;-X_`=-NlG<-LGg;-TGg;-VGg;-55kj."
    "Kxf&v%@:@-kGg;-t(MT.TUG+#IM#<-%+#O-db;>Mw/2PM)5;PMnAMPMUVs(v+Nx>-_?0,.<T$QMx(SQMH.]QM4F+RME_4rL22x(#;<Y0.Dx9hL91x(#FGg;-/Hg;-.Bg;-t9xu->iPlL"
    ".jDxLZ?jZuqCg;-jPqw-T-<JMof1#vq`Y-M>9+)#wD:@-TGg;-VGg;-@qN+.'o.nL;>+)#i-wiLCslOM(/2PM=AMPMUlj,#]%*)#r+kB->sA,Mp8+)#p2RA-*(7*.3qYlL0XZ)M[H/wu"
    "bBg;-jPqw-U-<JMF]tJMFVOjL9'[lLMrU:#jPGLMmsANMhs&nLL&[lLe;.&MRN$+#eAg;-oGg;-1``=-X8RA-WK40MX>4)#;;)=-0Hg;-F``=-;TGs-]KNjLU>EMM4-elLaEm6#.@>qV"
    "?ahi'ie4;6]8xc<3E'N(NSPe$=1Y(N>Yw],HF,W.EXE_&NlV_&LA_e$TY_e$)qI9ir?`l8Qiq@-2Rx>-kGg;->L/Z0]k((v]8E)#<TfvLw()PM+AMPMT5voL5KF)#SwX?-0Hg;-F``=-"
    "0+%U0WoA*#&i8*#g-wiLvxuOM()doLC&:*#&Hg;-%F)a-$@B-mD&ii'n-Y59iJ^e$CWZ59W<X_&%x`e$t=]59c^X_&$XSMLm*cP9CTdP9EavP9DZBk=/dp(<rjVG<N-$Q9cWF_&)']i9"
    "11>m9.?p92NVX'8K7Zi9,rF2:K#E2:QG&3:UMM1p@I0kXSPLe$BLw?0Zi7qVYj&/:.(Y2:NTJq;GZ644@S0L,Dqwu#9u>J:'15YY13Fm'A)Pe$xe#qr[3nr-s*,p8_osM:lxd?K(Hn3+"
    "Q>l-$Bl]9M8*e?g6;2A=?;/5^S.gl^O8w.id('j:L3)j:N?;j:RQPX(kF`e$K)a-64#6D<u/SD=22uA>=0Wf:#(Y;.7qlcEGQMDFIVuoJlE?/;iDsJ;`NF_&Bl]9MNmt(<JiWe$pv38."
    "Q+DpJ-#:D<D:_G<[41d<'[w?0Mg$44mN1JLx88)=0po)=FM@rdfYIR*2g[R*7<tW_QMii'MtmD=o+V_&?ho92AIw?0V;6A=YH*`j(8Z;@d:Qe$A]BL,?KtKGJ9,:2J9,:2<l(F.fxQ]="
    ";<xu-VBCpLVsPoL/RM/#c.hHMhZ/wu0%),#omXoL@8=JMeaamL16*,#kYGs-vDpkLO<_*#AAg;-slG<-:eXv-8V`mL(aPOMEmQ+#7gG<-tGg;-CwX?->Rx>-KE:@-(mG<-)mG<-BRx>-"
    "W&Rx-w40sLYYrPM9eXRMLrkRMIp?TM3'7tLj;3,##V)..e4qHMQ]s(vv+uoL'c5oL[H/wucAg;-#l@u-GuRNM$t&nLEPN,#qlG<-%)`5/EQw8#nYWRMYqkRMG^_sL7;)pLB;K-#98.wu"
    "uSGs-)=G)M;3h&vcCg;-plG<-kGg;-I-0c.%l((vfZ`=-wGg;-K``=-;Hg;-S5)=-FcqD.]J`,#D/L;@V[P_&1^IR*@(eV@wtiV@oiv;7oJal89ldV@S_eV@S_eV@UkwV@0WF'S4=?XC"
    "d`K`EY2NDFr@`'8RKp92:6do@^KUe$Zi7qVLkw?0NiuoJ;XOQB%V(`jZrGM0TdA,3X,5d3;aCX(E2q:HZPL&5f@Qe$d5k:Q]c-^5f;Ke$G[,wpUh+;?]hiY?88`PBJvM&#9ldV@c0#RB"
    "NlUrH=nDi^ScWiB/s$mB0#.mBxPD&.=K#lLV2T-#/#.mBwG)a-d`Ae?+Ei+MV2T-#/#.mBpp,D-*(H`-^)Ik=)ZUk=)ZUk=&KOS.h9>/D*8D;eH+b-6t]Pe$./]9M?qlu5G`H_&*Td?K"
    "Sb;F.&_ew9&_ew9&_ew9G&RX(G&RX(G&RX(SD@X()cWlJKA&qru&)8@jqI_&pWJq;#9`o@P>Fl]-4dYH-l[PB%I(p]d6Ne$Sb%v#aWQP^hQBgM<k>MBwFg;-0Hg;->Z`=-IAxu-Oj%nL"
    ">0Q6#a[CT00&),#DtO6#s26&M$;DPMDGVPM?mx#Mm.s7#K]DuLP@$##Qna1#)rls-0P,lL2+)t-]RXgL:=`T.,Mc##1':W.D-4&#rrG<-N2:W.4l:$#Ff`=-8X`=-H/)t-38^kL?BK$."
    "lep`N*.;hLb*elLUgD;$Ur[fL^T4;NVRH##0*kZM+:#gLm?a$#bB`T.Oj9'#lnG<-:k@u-wav*NVGU[-*]V_&9o4R*Fu,29gTm92u&1PM0_uGM_2*6N>[`=-amG<-hjv5.v''<NQ:-##"
    "WRuG-UPqw-.K&,NbL?##kJP-N2'IAOpXQ##-`s3Ne_Z##CEM6NYed##Pvk8NZkm##I&Q=N[qv##0q<CNaEA<$-W`6N^'3$#BL;6N_-<$#U_1ANcNAw#3xt0Na9N$#'`J?Na?a$#(%*M-"
    "7fG<-Oa`=-g+B<&:8bo7EdrS&Dsb2(H#9F.=%5R*Pp,k04t7p&_59p&aGpP'AQ]5'>Rx>-Be%Y-umX_&B45R*:OhdM4)2hL::56'q``=-Gk@u-+W9<NVRH##S]Q]MB20COA30CO<X@IM"
    "K?&bMSE`hL=OcQ'+lG<-iV:@-iV:@-Lb%Y-#o:_]KO5R*^C^?5hh&a+CmX2(s(hK-*+kB-N``=-2=kB-2=kB-Yq`=-Yq`=-m;r#8r:pV.x.))X<SgM1UW0;e2C-?$1@`di/OvV%X/SgL"
    "4HS_%@<YRaPJlR*_1a:2QPuR*dV><-KrL*%;j9M-BpJF4f&@UCo7uLFFR]/G[ZRdF=r7fG'>ZhFwg0s7ogVD=&>.FHoZvRC:1k(I]_]t7x&kjES9L=BT=#SC#*N=B%Nn]-mm[1,#1AL,"
    "u:1eG4iY[..Hn-3o3J>-?lHMM=.rfD8ESE-%O;9CuicdG/iCrCw0m'#6mk.#XaX.#FZO.#fY5<-<sls-^IMmLe4B-#GTF.#w=Jj1JtI-#UvC$#HN=.#jV?/T-6G?-'fChOg)n+#qqN+."
    "#n-qL=wXrL?BojPerHMTCO[c`l:q-$jL`f_`Xo;VOT>AY;3+-3/pYc2:(e'&W#4GDufr>-S,X?^*/p>Hc0exFrwfS8U45&G.c@5B(3vfDm>+j1<-Cs7i-7L#gj0A=IYo+D&uY>H.MWq)"
    "Mah34<Un-$.R]rH14O2CH,f--J(h--gpXw0oODYGeLhM10%h]Gm#C5BmsZ,OvTg_-Mi5.6vVm-?cWx?0#V-AF0f?)4W$W,3*?7:r$<F&df(KW&xLRwTZ&2R3i3_9Ma&s.C(M=)FV/OJ-"
    "FSrt-:-W$8t4=Q1?v_x7rd+p8QQE;I5#SDF;fs>-XLKwB93(@'MPM'J=%g+M8,e+#$tQQM/KG&#/lr=-?5)=-udg,.Eq/kLPU#oL*LA/.sg$qLJ95O-*5)=-5XjU-ncEB-hMKK/8&*)#"
    "$H(u7R59$L@[F&d@2oiLAf=rL<.lrL_oY.#x5'<-YBrP-K/&F-@fr=-t-OJ-[AcG-M3il-Ht,XL$)H'SH`Ok+qEmJa7r%qL&*d.#(nHQM1+@A-D5)=-6won.V;L/#Lfr=-Or.>-kwR/M"
    "R;SqLGwXrL,r%qLe@g-#dFM.MN@n]-:Sm--BULwB$(%sI<1.<-mjk0MWkFrLa:oQMfs%qLG(crL<g=rLT<M.M9$/qLVfipLN2oiL>3urLl<SqL@R=RMrf0K-Jt:P-3IKC-BN#<-K=oY-"
    "$sxE@&xl?KgH_w0CXLwBZYwddM=He-NQmcEU#b-dPf._J6*(@'hE/vP6FD/#]4S>-B$jE-D#T,MSZjU-G)c(.:)trLmfr/#uOwB&+I]rH^]f]GBe,9BA?LSM_hipLu'C4NppDE-wo?2M"
    "IZv&Ni;CSM5ALSM'3oiL$rkRM90AqL=LKC-w=9C-/1'C-Ilr=-FwY28j;r`F;2MP-nEtJ-dQF0MEwXrLq.B-#A3pKMZKCsL4(8qL6;CSMj.Xk'A&Wq)GpT-H]mHedFL5)F1mC5BsWY>-"
    "&]g]Gv&VGE;P/_SFAc;-?T]F-^bLY8:kN>HIL%f$df*L5dAo+DdVNDF4iocE]xB;IH<qfDX*M,M*lP.#4o,D-Z5`P-R(A>-]?:@-P$vlMXZ`UNkKJF-L0#O-kE:@-XX4?-u=9C-skKDM"
    "a5$L-uIKC-QxRu-&)9SM]t%qL<iMY$(D6<-)&R2Ms2oiL[E:sL]xXrL:X+rL`kP.#9fd'8pZ*#H$iG>HkaqcE>CIqMBT:@'FO7<-h3(@-uRnr7I'b>-WtkR-kV3B-.V^C-l]3B-^MC)."
    "/*a?8N_'NC`EAVHLYl;-Dmls-mg$qL++_4.:dS%8<;bMCkRZL-'PwA-=8aM-w'wT--PE3MB2oiLeC0[8RhE>Hi,J21Mv.&GN/+#Hh2^29?,6EP8X+rLG5JqLCQLsLYskRMS3oiL?-lrL"
    "&6RA-;tP5MA&VH-OX4?-CbDE-rv'1MU]-J8&3MDFwqC;IV<2&GS2/&GCde]GW.E5B$w.vHK,F>HjGpKY=_.(A/p*aFJq[KlU/j`F[jR#HeZ2GDl?M>HnA1^5+v4L#?,j+MLC$LMdmwf&"
    "HcfKY1AG/2gYE)F=TXw0m++L5cw^9MQ8U-H&(Oq2n/@PAi/ZMCDbxlBL$aq)>.5)F+Fu^Clq%t-(mWrLQ>)=-_(LS-(:W6.2`P5NI@8F-(MM=-ObOZ-HlV&dB2oiLD3urLlmFrL`A]qL"
    "'A8C%G.3q`@%6;-UKH-ZB*0F%^h+KEH-lrL^Raj$M=He-j9Y>-gfj`FR6#mB6was-7q/kL.rXD'`NJ<-a$uZ-M]#.6hN&<->5m<-j^BK-KDPa8SkE>HDpvDYW.K-#g4Up&h5WiBAGSDF"
    "ZZYMCvGjq2DHG_&<=ns-2aaRMY(m.#CY`=-wWF0Mp]G-MDb)v79i.&G2,=aFRt@VHJ0I>Hah1#HSH.^Gu:JcM0#XB-2RWU-Z6(XN6STW$.&up7WXK;Il)_DM+5T-#D<8F-5k`a-DZ0WJ"
    "9dDE-2w8V-lkUH-gRh`.KaX.#ZNU[-49@)Atp#?@bj2E-m+m::$Qe]GQ?.#HN0i]G:CxlBm98N9,2VDF_]$f88PX`SfpoW_gPS,MxW5.#/UZ'-hU5.#uX&3C`Qx7IwJ(@'n5w1BExa-6"
    "2w^Ku7;7L>Q.(58_HkMC27^^$=e=rL/<SqL@?)=-I&vW-rhSe$u,=p7r@'x9]2ncEsUFr;,tWQ8SE/s7,J(sIxBpER?`9EPfcw>%wu4GDD,<MBfIB-mABEE>sU5.#U_.n8K#s2ie'm.#"
    "VseQ-n&6-.QX/T;O'S5BpnS'AR7fI;onX_/g[1Alf/K/M1rW#.A5$R;bw]3ia=g-#9eE<-0]AN->[1]Qh1,G-H%'&W3kP.#Oa[?$%&N@$iUlB$^Ag;-G93fPJ8`Q&2gh6kTmm+MA)8qL"
    "C7B-#Pn@-#NS1H-(F&(R2:2/#mKL@-@[;VSsi3(Ol7$##VWj-$vU'#vwKTsL2]K;-GDvP05`($#8Dluu`n'hLf7-##s=F&#xUX&#p@aw'26k-$NkgC,3x.L,C>AN0q6+Rs:cCZ#;JViL"
    "9&@T.P9G>#Ccxt%h_'R'GHBY$f3;'%`'w>#7(Fk4v:IGV(PvO2g-x6/+H*2LU7MMMJ/I+#N/:/#TZO.#*#krL0RX)#MKjJ2#-a[%ixNX?2oF:D&2-&=b@.[88W+Z337&UVAj1P2Gv#]2"
    "I,1YV+AcY#C(qkLd&L.&qOS2`%C#(MR:$##U(faNMg`V$*8G;I(?a$$@U_,)T)Ol2(Qct$?^]X%Ng5;B,g>LF2YvLFMIUR'DZ&=%JduRC(DvLF]uOJ2TETk0M>he;'AViF6JrEH:nA9."
    "pl$%-@kU#$LjN.-7vx0F(fmCH-34G-():N2As$e2HjEM2M8g+4qikw?b`:&5d#tJ2qPLHT6o=:&uMtT&u).K<DULK28LxT%kt,]8v&AUCMdbd=wikd=ej3.3UQ*I3@BrO1CN3.3xF<=."
    "aWG&,[0&b=0P-pDIeWkL=wRsA[w`V$TE3L#wX4R*laaZ#R-,##'xb(t*R@ddr3LV/jfLXL+HGo82kx7+7[f:Er3k=59Buvms?JD#P[wrcr:I%uD&22>1t#+sTqW5$gs@n?Qm9,@WYtx3"
    ".HS)D),]sqGlH&g3+NG<,xmCt70`a3tEMM&p^F^)Iw7KRf0]6LMNYD'#iJ_mD],aJ*j&+Wjb`%hfuQEj5+6KkO3B7)1)o3:dU[6:7+#uPcK,p?f8++#V]RSnG/,6BSnBlP>_:_KgRLJi"
    "eIBEC-Jq)B<tee1pqEMnhkd)(,`Vuf*)t=IlZXY3IQM4P7#'I?W6;c)TYQn&/-WZ19+_&5#KJ@hu5]T#B2##Qf&P-V)2,##K)9L#5#5L#&D%v$QQUP'U2iG)QNqC>*Rg[=c,>nheietN"
    "US7?[jQRh,I)+a*Su&U)MD^'j6K1A&oRH7IY)gW[40q&(KY5I-wQxc;ngGO:F1,C9u&-FHr(622?%ZlEDZRsS8.MnBJ)f'G;6aq)_]>fG%?+vB&=gt7s>L'>^i;h=,&D.G&mXm1?RRL2"
    "`@Sj04r-W$0PC;$p%6O2%Si9.nqGI6NHW;I[h@&Bbv=YTCN_S28k4g.3*UH2(*?`-V2d&QIL7w#R-,##o,:30]^l'q5A,'3[#cxoe'>mGA#3m%l5EL[xECMBLNE*JDTg00wVHX:v/Zlr"
    "Po`<Y1ISAAeNYa,#G%/kSf5soiB:A-0kB*Uk_+%]LndKT&l';*2%&@b>]-MY+Ero,X.6.6UO(prD2n,4/1U-BxLw$+L'Z102Rlt48YS8YeYNEpk-fE;./jLnDqBL$#';Alj,CCTOW%[t"
    "Z<>[QTWtC)j/Fg0sX8=UK_'Br.hHt^bSL6cZV:L],g#]eGNQY(g,02VG4e;sUM@>D?kV5`*]xsM1_6P$mx6LDEw58'Kk]J;_dvb*uC;*h1fE_K8Sepc;.s;d1&_##mFASna&63083hmX"
    ".*>[s(:w0#(.r,4cTfW$li/$9BXFp/]uvMrT>B88,ACW-Zjd:Cf/AlD&#?LF>qqX8<eZL2)>`_,Zdeb4CZnh2_#5L#3;?Ze6@)X-<vBBVjPp;-]277*s,%W%dVr4i:So/%n_aBYG5@6V"
    "IXxm5B)a)T.EwvW+%ElH2:w)Mr,^^LEqQ-CeLdHfx]fS767j9;aqHP^.Ufbd/:Ta$x?Z`?Rb=QoI>teqS..HaUm6rHWTKV^UtjT3Gei/SVFXR5=$mu(IP#3j=L4pY@w18:S/N^dlw0$A"
    "Y?H=2+[pc<4D6RjJp*NL8r*FD%j8K,8GDYHP>95d(pwe&jJa#v1Lb$]1+3ZogAgk+x3g@FF/JxiEwRpq$G?C'^[0b-QJA3,sv8SLp%v`n/DC?&vup.+2TG;'7_vObm_g1No.DNj(Nf;/"
    "pt[m(+/nK/A;SnNt7WQ&2/=G%$^XkLRjCHFv=7?$tf#l92emJ;o^&EY=R$m8]Z$9.ZRi)-bJ,]nv&<U*6>`<>T5M:[XWXv#U[+=$53#hWe`pj2V5:t-<%ikL&v2@$C$Q-3=<eh2rJH-5"
    "uDC04K/Ge3vrsO1J>>(5G>^F4uVQ-5Q7k<BF&9f33NaE#oRfHFn=6`$;+PV*jQB5^$[slHV9cE#5lKX)Y']+H?%ZlEmZhtB8i2h=2GtX8MBeh2;(<I$3j]b4<Q[l1lPkJ2&wo3/)?P>>"
    "X/,qiMch8.wm<@9_tvIM)L^8%n&t?Hi1Vv#TLA@#*P1Z#eKOveVBOx#S0,##6J)i3B(sCR>o,X+E+(OKu`Q:XaXT.95Htd]d4F@TM1w<nC[Zc)1fx>5Hc%%6ED1f'OF^6`01(1KqgouL"
    "nkQ=MY5g%brffrp%.GxCv#PX;7S6OnTiXrdv7W*6#^%,SG;Y]VkC-),r`Ra72dRbP7=,Wm@$'Np.ET(UUuFwXc+eb`)P^aa5_3HN/]=tpB5F[OHwiBO-1x-4LU6;nTET,Mg7C]UX&N9/"
    "^Sd#s-)aW4@ASfn3PqxKFZ%J@LDaJC&u<w>$KC0TU+et)J=6#c<x$e#.0Z*,G,?s(=rIbrk;i5`.YsdD=qmDMqg'o2+#IKq7d8KN;HT-%4tVv^:j@h%8&E][+/Av`JCQ7p1;4o)Dn/VO"
    "?'p7gxtDISJxEO#[jIPA#wQIJo:V^iAlQaHX'Q2*<*N4`781HG$Tn<=wT&X+(nZTS4_Lh40eFM3MRT]`bKk(9>,bFDKgnM8'qR4=gq1b#tBU/+iI.)oe_YCnx6]tJ0,K4Di2sg'IQ3^q"
    "-<#AlO^+`FN=u_IN=4#/Y2mW]_/RN]p3fO9lihaE5h7ZpCJJO/]TTA$oaan'o<FO8('35O]PnX`IsGG=2UunKtNaYC),pN^+iv)^Ixj_aPAp$c,eJYTi_S^%kk#-XlQRW#?i(EH'`eU&"
    "8fr8%t0UipuBP>#=(#3B+BU(#88@&,8l#[5Ll>1)u*k5&oCh)O:=^3CVd=x#?l<o2f1-%PSEs/:9uAg2F(W69C;pV.P)VT%-P:iFaUBQC/BfqCq]CEH`/9^H7Y7fGUe9T2@j_H2DKnL2"
    "Dp8+4p:hx?U3vh2`8gG3Z%d<-Xw6ZIVnpV%&N^qE.'GF:sWBQE`k;%aI6qFT@8a_Z6jO+<mO'KgA/B_ZNflM(@8O9>VWxZRm,+,[o_::/R3E_n(st>O?Ljh':oA?OdosY3`Qwvqs>(21"
    "b/)#ObpWA<M*jL.uYlOth%Cmf^TDj[[#_6t11T;DFGie9)f_$gP:0n1xtRR<gs$<mdQVCE829FF($qP2gKgsh#:Ap-Z;Lhg?T=j^@d3u]6xw?:qI.nC@+rjCrm]+hBwV0WZ><](`.GgL"
    "5KIW>6F>MTtM<dMTCIR9vrrO.m9@`1:4=m,0E4=IQQ]76=p1CBZk=5'flo-Us6j2.hFZ@t$%xO-YotO%R7ZkLapvn'[M_p.]$[>#8d0<f=86Q)VYv92e7`;$Z@k(NstNp%UX7W-#,EX("
    "M)9OLS:Ch$NlaV2r2+U)lJR$B#j@TS>EE$u7LX^=F^[xW.FKCBBhSN32L*dZqpwNV6al'H51q,)7J]e<_UsM0jipN2BC7NVv0_2;v,vv.:GTiF%kiTCFnOnB1/u'>R+RT.hcgN2$#/7&"
    "'YU6(g@h0M'Bc@&n,$L>bg>L>W889&0/,##ofvkbRZKH4MAKjZj1[/JuSTo[=d-D@[qRwYgWh,QFRC')x)tXBVew7eJvQb<U9Oa7togK[j<h@luc/IZ+.4wOs[`D#CYv3Q3:=SP=g1w="
    "gQ'(ZfC`VPP)<3bB%h6%pX]on%[_v-.%a<uHw&w1h4rYlbYN+<p][@&0C&wV8UEU13_/6o7(f/*QoR;7YFa+3fSqk:3X7p0TAvIoV=Z(N8$]Xbdra=qb*KRoBhGKc0J%NW_p$FKlNd;h"
    "NWJPpR,=hIAi(H#g8VO'(;_nSv'<$D6Om02gJ%7N+T3]n2q:pE*pb6hSlYnYAVT<0%*^B0DDxM+UX0a,#dF<q3xE0`%@0jm=G0?^t$xLH%s5FcER'7/3@RG=mhvV9@x/e]<X.4D#BG&v"
    "j4D2LnvZkCjVr6Armf-.BiPvn>AmZr9jC3hPqqT;5E2[.tmGs>2kQIT5']cFSZ4?HJt%6IxN)wLmY$V^[W7,OQ/hv]B[%CXqN26q-J']Ot@&muTo$8P4C7n`xM0u(I1mP1ugv=_pT1J^"
    "b9'w]ui1/h:PECZ1ZteIeTwESuda3RUS?).5NMf%puF,PCGB$qk3lihFxP_d,B>*lFw1hFSI-HdU%`YFrJnDf6w^i'$[bPi[8)Xi+RvH7(rMFEL'*OuW&`5J=AkMBHSq8.?uJ9#uUB#$"
    "(RAt-S/L:;2_V,3NYK32cI`Q9'>67(avQL2Jvs.3^?GS9^1YD4^&0g21_fH;M]%jL/OC](H*#j'-,Ph#_KNh#lPf@qh1^$,F)4XFXr=e;g#tg^)G%F8idb$o1Cl@_*8)>$k8ijbcO.u4"
    "@4oHip%tFb<oRMdGcSG)Ig6<O@]eNNfmf^.'.5[:qU1@Ap^+n=%1oab,6k%1ZPi8[a'w%e7xkonqpcA32Wr-2e.Ha<MT^p^LE]%2Z9JYDD_47rrw6:*OF=6l0CKSF1wdMXdAxK1owgos"
    "Oew2'u?S2X52iJ'cZ&:INrfop3[Cw?K[Zb>0JHgG^1Ro^<c[HpjQ:;MQ$:dJ2ps37R0v]nEqEO+@MJ[?V)`P1#t6GcJL)D3@'lV6u2=/RJvOaUj1&C<b+L)4_V7GFk+6o_@HH8+6qO]r"
    "JR9LP<Gw/gVB5cRId*l-4Uhj*K$;g/AaHpSWDhUf=(/,[_vaOYLO?*-6RYN:6vWhlEa)n-of:+c>C7Z.d5=KJQIqN.91ds';oh]]fnu_`_A1_A3<ER]&xbT>1<OiV3K,N:J+?*96'q=b"
    "`HM/l,D'ak352NcI,ix>R[A%p<_EbX*l,.pvb+5uPH%el$Q'_Jw><E;%6[aXWmwnXp+'*n='/^Abi[9i3<gMa2i.NqkrV+0YK^c7M=CRl;p/1ci+[ChZ$&@Hnkrks@&H814og#S)5aL3"
    "gV7sN[=j9<ragY`eG(xS6O<wN:RnY%=T]kLS.SauJwc-&;1E=-f^UX%n:M;$We)$#A;L/#tc/*#3u?'?q]r>(S$2'4Trk`#7J4Q_mQ)FlY5sx2+K5o3&K[gj+I2P2?9KOV83h0NdDTI0"
    "iZhtBBp8]8]'<^H9%tn%g1GI3k3%l1@fnR2didM&pf[e<$,l#n+6CT9)=caAm2=OL,=3F0a:r^SD.0W2FSKZ.-rn$$3s?U2&;uC.pxVF3*ZFcFE$u3CCY<822&$1F),PvBHB-LDv'TX7"
    "$>G`#-:1,3W*8G;EMo+DFoK`EH7dxFrm/F%j5WiB,m(@'-C&;HQDF>HRs2)F7'MDFXd<j1KDog<Ptnw#S0,##TR$4XkaYxInIF^ihM+mTcBX?lJ#HE8..'0J<Jm_Cq>Lw6$1p=g'ecCa"
    "v@Tsde`QEYU:/OjHhV)4#[3TCA_ajO=dwOig49ZgCEN.r*n#P=dM3`E%VHg1b6(p6o%FH^0b$N5t[madBUbAJv4x3KR558MB'oJ/X$CEbt39tW$L`jT2GobD]'P_0PDMF?M[5s%CD-vS"
    "m6oTrX_=h9ftg:6,itnpKqVimd5Y/g^2E9[k6V&Trw$W&)_mthPx(f*?7K`1KN<'^wa6Zce5ClZDb;viCw@4=+Du.3U_;lK?a@ds3*FA-1GHj?H82iFfCYibk4TAX`o'w6EdVe;_uSe]"
    "-e`7QC*Ro)YfK.I.w`NC:40K=BX,S,0im8Nv6,$tiO<_TJvT;X-*/3A]$:P@7Z7+7#h?2gpjH,-dnA<Bn)aqc66Dqkc;?n/0%m62@lZ'WDV,:#Jahj`Xgj5X%Ii6(-h^ofo/L.]>Yg89"
    "`ih_c0*(=V58EDtwVpxY`UT2LqFH_a)i6XS5Al`ILcVv4DuO.;&x@K.BwDP[=`n_oX5N&E;-:c2EVDbPqd?2F'X58g`uQL<2>2&.+fh*3E-;38p?kctE`7+AUf#xT;B+2<_%SI8ZN]D@"
    "bvlKEQ%S6b&kK4Eq)B8)t`uwe6RVS[1.?;ZW3FG29d46_)U/n$BvwW?XkmJV^#2++39,9.mk'r&nWFT.ZEbG2OOV9.HiiH2X]?g)8nL15St7.*G$+JK+-sgIKRN&:W.g,Zq9;a)9A&4C"
    "%P6q&/At'>AhG$0(dlD)lhWoIEaCS@Tw)gV139T8>d:'>9db7D%E0SDYT*v8IOI<$x)-5/,6Ph6t+,p/kiR_lBH_^GGl9C#mM++34SU3S)l-*.7]qo`J#k5F-RQCO3ANu2?$1fT1W$dc"
    "'8mdufZV[<G/]<fKu/L7<+?8'Du)BMt&W9pKVApZupx^klO_W-BOw?4'>63Y;1-DhGfNdEp#:;J:pe&Vf0e6TJCF7V6L&bA_Q?%q'110a1d4`uH,l2sMI72ro_CQ_'G[Vpur]P]GC77/"
    "*AuA3mYgHoo/>_2Y4(%LNd'DP'5gQ]=RbFqwJ#*BdTq&KsD?/.jWK`@T#(9W2JU&`7=S<HUiWg8TPd`N,HX`L-P'O:tsn's.wB>2j-e+I<QMhVU#TK2cOSs7OOdv$Cwuc)D/LN(W3FG2"
    "a'mhNtcan)%Pvs-Rs9(O/Yuc2;CM9.xD'(,Hs4u%*7uE*uiNi2D#t.3Hp<M257>^Q5MhomUnP_]EdOsg4l7,Ef?N3o_.e7(B%`;$_H8MBxs^cQ%dFG:Eg0daIBtE3i;0BAAe(_6a4eGc"
    "=MK*Xm;]qM=BU]qPJ+Hc0)q:&k#Z4Z=Gv%&(4I,0HOs0AUHxQNHGcq;6G^s9++:J1@M@fgkK-n--i]XCRv5agT9tvC/D/H=D?h0m'g>``NEBVcQLIuN4JvYQHUI`rNQD9/&o;PmsghOQ"
    "G=eY=ruw-q6u;*RS`vmgUp^Tmucpa67o(cM9q0bfd+kt./agK*;&G0*Y?+hgoR^mrb1ZVOqtPBegE*^lGx:_/+(n2#b<ZsMN4fvR3o:;SL@NEtPrnNHcUf].S4CE^o*CB>wle=8JW&?g"
    "8:c=#rCbA#*[7p&4aG,*w[0^#)%vW-_pJe$,a$@'<Fpe$@A+GMHM#v#4iZlEg)AeGnWOS.)>MMF0bg+MqC?uu&8>##)6>-OuTe##1rC$#3:RG;5jH&#4[$l;q0_'>6sH&#jViq2QMYY#"
    "n.i_&fEV'892i_&%SwK>:5i_&`rUV$FNcf(l;OVCqdae$Awae$G3be$MEbe$SWbe$Tk?-GDJVrQjx02Ut'ADX(-QV[22bi_B[R&c*>;MF&1ee$LCee$RUee$Xhee$_$fe$%[G-dHq#XL"
    "4.P^$'WgW$0fB#$<U(^#&_L&#uLbA#=aI#$Bgp=)<34<-n9nmLqqwM04l:$#Sww%#Kp/kL)M>gLeuBKM<Onl/;a>W-oHOF%XWrdMc[4_l+I:E4kDx;?^,&t#X8`($EwQO$/1pe$peYN%"
    "G90:%7EJX%BB]9&VOVo&5=(Z'PF@)(c_d,(Fac/(PM#7(d'l:(63R<(:_x](.i'N(Ud$W(3<l^(wQL?)7M(c)1x+)3t&eS)D_Fu)LHxt)B_B+*GD@2Ns#Pm)&W@QMm^n<*]LVX-#p9L#"
    "qcl$uS1^U@AM,+sSZL%>Tl?K)[CQK+GY`=-(<nmLZ*).Mq<nbsxbY>Hf0EL#RQ]*+aFWp+2MOjLOee),+9VhLFotjLpivc,lm*i,%2oiLa)6(-(V0(M9sX>-T6CkL1:5b-H82%.^[+#-"
    "@Vsl,Ug*/Os]j8AAiW#-&M#SN^X3,<C+^;.J4C<.%NXcNsg1@0E%X#-#s.QMUkM1/?.c(.pfsqMZ,G2C`3/8oOlScMjS,W-AZXL#AA9jLfQ1>?<(98]$?\?JrV79GM[PZaMO2E6.m3<.N"
    "1FpgLM5cB,%i0'#/bB#vVfsjLPIhkL=3`e34RX]4$'qx4m[8>5B8RY5pr.;6tbM]=IS);?jjKV?b'i1^PTEi^ZGPc`#SASeDHcofPSaJi/wi(jbtU*k@l(Ak)-F]kB4@Yl^A:VmvM:Mq"
    "6xJ`tq+f%ur4+Aus=F]u&xT#vW#F##KZa>#vU'^#w_B#$&+-?$+#qS%;'<s%(IV8&0<0j(1EK/)2NgJ)EOlS/FX1p/GbL50SmJg2?^>)3Qg]G3Rpxc3S#>)47+[D4U5u`4V>:&5WGUA5"
    "XPq]5^(N;7UmlV7`:/s7MsiEI@riEIl'+&#fVH(#U?O&#cYwi.'Ee8#/N#<-2mL5/YWEp#NK/(MDrNs#dO#<-a7T;-b7T;-c7T;-fI5s-uK/(MbF?)M=1k8$hjr8$w4T;-3@;=-$5T;-"
    "%5T;-`(m<-T5T;-U5T;-V5T;-W5T;-X5T;-gHvD-I9<#0__R%#ua;<#KW,11Er%5#m3u6#+Lb&#_M#<-9;Mt-]`LxLvCTl#b-W'Mt=Tl#m*m<-b7T;-c7T;-l*.m/k9x1$XX,7$'M#<-"
    "$5T;-%5T;-j(m<-_5T;-`5T;-VUus/axUZu_0iZur/q4.)V`mLE&0nLInH+#^T4oL$/2PMGib)#qMj)#Mr.>-CME/1]FUn#j8mr#ZXEp#8I5s-Z<;mL-D3mLYoBa#jhG<-c7T;-fI5s-"
    "+=;mLVx^&$(M#<-%5T;-hYlS.YA28$p6T;-q6T;-r6T;-s6T;-t6T;-u6T;-v6T;-w6T;-x6T;-'[lS.+ca5$X7T;-Y7T;-Z7T;-[7T;-]7T;-^7T;-_7T;-MR3V24(V$#U4i$#[Qk&#"
    "dOK:#?@;=-FYlS.E]^:#kM#<-q5T;-%ZlS.^]^:#/N#<-,6T;-=gG<-76T;-MgG<-_b:1.L25)MLf<s#S3^gL6*MP-[=.2.c`l)Mpiv)Meo)*M6;#gL$@,gL%F5gLdgjmL,<R7vK4H-."
    "w,p(M_2$)M[8-)M]>6)M^D?)M_JH)M</F8v9;q'#Y3u6#H5T;-P5T;-g5T;-)a/,MW6voL+=bJ-<)m<-E6T;-[T=_.BGUn#5[lS.vQgq#D7T;-L7T;-N7T;-Y7T;-f7T;-p7T;-q7T;-"
    "r7T;-w[lS.T[,7$&5T;-'5T;-(5T;-05T;-15T;-25T;-E5T;-F5T;-KYlS.<Pq3$%7T;-&7T;-'7T;-(7T;-)7T;-7XjfLtp#/_1/FM_.,bi_1voE@$uoE@i0SnL^S'(M2mU:#Thk(0"
    "Hg>3#'Ee8#[M#<-iL.U.mvR-#;tL1.fFA[8RNO&#GS'8/v$vj#@j](MB/7r#PK/(M@)7r#PO#<-N7T;-nAFV.CdVs#WO#<-eN.U.k44e#^O#<-f7T;-p7T;-q7T;-r7T;-s7T;-]@ll("
    "VSJiB>84)kxBLk+(nj-$00k-$13k-$26k-$Epk-$Fsk-$Gvk-$go_1T3;l-$Q>l-$RAl-$XV_=-,'l?-Z5T;-[5T;-]5T;-eaDE-_5T;-dYlS.eA28$%7T;-&7T;-'7T;-(7T;-)7T;-"
    "7XjfL5r#/_1/FM_.,bi_4<u^]';u^]M5>##4b;<#+6T;-0ZlS.M+rvu*ke+%?GW]4(:9>5[.SY5pXn#6K`3;6Dax?0n*ZiBU2`rdrg&>lkUg,bcr%o#m*m<-b7T;-c7T;-fI5s-m]le8"
    "xoPGaeSC_&%ej-$`5mN0d(U'#l,B2#5e,tLpwd+#75-t-ZX=oLZ<e+#ro>r#`Qgq#6O#<-:[lS.B)Mc#`7T;-a7T;-b7T;-c7T;-h[lS.j2i($#5T;-$5T;-%5T;-hYlS.OZ,7$p6T;-"
    "q6T;-r6T;-s6T;-t6T;-u6T;-v6T;-w6T;-x6T;-'[lS.=$Z7$X7T;-Y7T;-Z7T;-[7T;-]7T;-^7T;-ma&g1B_^:#I(V$##5i$#B?<jL2F4%v8+E%v^<a%vL=om8NwQQU5-V:#/8':#"
    "InM<#vFuG-W5T;-t:$x/9Qw8#AEe8#-)m<-obmB8>vx(k$m#X$wPEj`>0]o+P:H9i]KF887vtqA])&fhS#PrHP[S:dPfP+`rSk-$N(]w'gXD<-k3kn/Ug>3#DI<1#o(.m/Ppm(#De0'#"
    ")CGO-C'l?-xM#<-#N#<-)s:T.kb;<#A6`P-VW_@-;A;=-,6T;-BlWN0,$vj#j8mr#,FQ&M)VMn#>*m<->*.m/fQ6g#WGUn#D7T;-L7T;-N7T;-Y7T;-tZ`=-a7T;-b7T;-c7T;-d7T;-"
    "e7T;-(t.>-p7T;-q7T;-r7T;-%+.m/6Qq3$KXQ,$$5T;-%5T;-2(m<-'5T;-(5T;-05T;-15T;-25T;-E5T;-F5T;-G5T;-W^569udwV[Q=lo[&928]'BMS](Kio])T.5^*^IP^T^0wg"
    "-q*$M1#5$M.)>$M.BR7v&&g(MY,q(MZ2$)M[8-)M]>6)M^D?)M_JH)M`PQ)M:Da'15dX+`MQgi'U%NJ(HFpV.+f[PB,owlBjKnE#w5voLp4*$#1r%5#.^f=#]R1H->G5s-o;9]9$B0pp"
    "PBt4#Yg>3#;@O&#uM#<-`jTK-o])..?h$qLmd.n$QD>M^+ikl^h_.T.70]l#8I5s-QBP&8QjT;IZFTk+bqr-$ctr-$dwr-$e$s-$x:n7/NTuuu'E4gLa/@0/jrw%X2H?AXq*]`Xr3x%Y"
    "s<=AYtEX]YuNtxYvW9>ZwaTYZxjpuZ#t5;[#V0-OVBR7v6&g(MY,q(MZ2$)M[8-)M]>6)M^D?)M_JH)M`PQ)MBveN%Fdhi'1^1/(w3OJ(eqOQ_F9U&vH*p&vep.nL@T$+#61SnLmm&S%"
    "M:>_$WV9M^g$a29EiJQ_[hpR7w&_69RgC#$/Rw;-=A#t&hES+`(MmoogC9w%G/Yq7`AoVofws0v<[1xLqa;xLrgDxLsmMxLtsVxLu#axLv)jxLw/sxLx5&#M#</#M%4D,1b`e)3=#VZu"
    "=3%wu./iZu:,FxuIrZiLmf1#vBF?5.UesjLXIhkL+.+%v=W`mL0t&nL%$0nLK8*(vtq1(v=cFoL%/moLkVj(v)cqpL@r%qL04JqLt6)+v,xb2v3iw#MbiM3vjRU3vhT0%MkUo4v_;6&M"
    "PX[&ML3O'MN?b'M'nh6v6)p(MdPQ)M#WZ)MH>tc'fp1*M2]8+MqbA+MrhJ+MsnS+MH:Q:v6jm.'2)[w''kj-$(nj-$00k-$13k-$26k-$Epk-$Fsk-$Gvk-$sHh=Y//l-$M2l-$N5l-$"
    "%1?S7J7JiKRDp-$qGp-$rJp-$sMp-$tPp-$uSp-$vVp-$wYp-$x]p-$#ap-$$dp-$%gp-$&jp-$'mp-$(pp-$)sp-$*vp-$]lS#$0O#<--7T;-2[lS.]2i($X7T;-Y7T;-Z7T;-[7T;-"
    "]7T;-^7T;-_7T;-`7T;-S^LtLC0gfL#*WV$'SEG)9-RV-Un.F%'Ra@kW[m-$4Yfq)o`wRes*r+;#*<,<ndVG<?wNs-Dh%nLI;)pLNRH##-Oc)M4PqkLDS]8#DXkV.25>##),1l12-rvu"
    "v8.wuvFX&#?W`mL3$0nL'Ih*#'=fnLOa5oL1af#M?mx#Mx3pfLCX/%#0.F7/HXt&#%:3jL?<.>%T1DpJ4bD>#6=(^#L=eYdCsd-6*=P'AfFX'A1@6j_YH*`jO//2_]%YGMjw(hLF.;hL"
    "K`@2N=l@iLk3urL0;K-#A*2,#<5T;-1eF?-e5T;-KKB,.g?WJM,D^nLUa?+#FK#D/c[P+#^O+oLq`5oL4g>oL1mGoLBsPoL##ZoL$)doL5/moLF5voLbI;pL5RxqL&`4rLE^_sLs&7tL"
    "3xSfLMT]8#GeHA/18.wu.crmL]6jN;AjX&>nHXlJ:68p&Ik]PB=G#E=Q/Q4o/dpooII.F%g(m-$_$Jq;I<Pq2sLm-$AW_w'7Fn-$MdG_&E<]'/ttd'/lJ'/:%HSD=AW]q)=vfqDLuPw9"
    "(Owf->vTW-ov(F7on`ooKi-pocuM1p<6too5)s-k4jBtba>j*olxWL._T0#vPt+.;bWe'Aw_7_6LG-##Z]^:#H(uP8=Ofp&tMQ1p,K>^0ontE=3JqM(wdH</UT0#vG#_Q;qu_3=he4Po"
    "BqOlobNx4pcW=PpdaXlpi,C2q[LvV$$%?v$%.Z;%gqb59,Yj+M/?[tLYW41#J@f>-qQX]X>.R##H]^:#@Lb&#7G.%#EEe8#*gG<-k6AvPx@DB#t`WxO3h*9BU2N$>T,5v6*LeU-fLcW@"
    "R1Ok'lm?=-'P$M&?kwH->I#5_9EQ]4VG&m;+r:T.3uQ0#7*.m/9*nK#,.QV#E5T;-L6T;-_).m/323h#F7mr#_5T;-a5T;-o(m<-CX_@-iU3B-0$0?.o3u6#6G.8f0f[8fwNuOfk'wiM"
    "Efx8#N2(@-'sX?-;$u_.k]D8vEiTS/psS&vX@$(#-vg;-O,WV*MO;0=FPO&#%O#<-&[lS.T?U/#?O#<-E*.m/0IqJ#9<:R#'G5s-A26&M8^xS##gG<-X5`T.:g#V#wF4v-HxE$MjS'(M"
    "B/7r#qI/(M4f.g#+tE):CY]Z$rBe@kJ-s-$i0s-$j3s-$x2g@kD+axbAY+Dsm]Mcsnfi(ts1SDtQEOP&wJ#<-,ob5.&dZ(#^?qS-dJD8@TlaYHE?LN01RT6v4ep6vOS/(MZ2.7v@v57v"
    "ILo#%(R>8@_<Cj_7gO+`XTj-$wWj-$e7O9/w^Z$vONpkLRU$lLS[-lL7c6lLUh?lLVnHlLWtQlLX$[lL0LX%vi0mlL[6wlL]<*mL;vb4)*@Yrd98^?^_u_/4k^9(MK`9(M&Ge7$_-m7$"
    "8F8t-jb5lLfh?lLVnHlLWtQlLX$[lL8-_&v:/2X%jkp?B-cD]klNaxk+@<YleNJSo[V8G*uoDg+9I=)#^.mlL[6wlL]<*mL^B3mL/K<mL`NEmLBK_&v.tC$#[tQ0#5b#tLCrOrL'T`f#"
    ".:hf#b7T;-c7T;-fI5s-^kWrLaO%,$(M#<-)YlS./&1/#S2(@-55(@-55(@-;lv9/Vv`8v/gu)M95t8v55(@-7G_w-GF4gL@-_&vn+jE-Z%jE-jo##1YJ.%>&xMRqPHUH%.FWV$qK85&"
    ";Lo(mXm?m'(Oj;-N-he0oNa>#IZ&Z#w_B#$3Y?MjMoX5$;JwA-U5T;-V5T;-W5T;-<9dV?U>>`6m^UY?%GY)k(0Bul1d)GMecTl#9q>r#Hocj#(R6g#tZ`=-a7T;-b7T;-c7T;-l*.m/"
    "OB28$JXQ,$'M#<-$5T;-%5T;-ix=G-P5T;-Q5T;-6_7*._G;mL:OEmL:3_&v@bkgL+g>.#w=nqLNiw$0lwP;@):>)kH/,AkMkU`k_q?^lk].JqY)F##uLbA#vU'^#w_B#$$up>$T3.T1"
    ";fR$vuIgkLQOqkLRU$lLS[-lL+c6lLUh?lLVnHlLWtQlLX$[lL^B3mLII<mL`NEmLNgq=%RBO9`j@//(mclQa`Oh4]-Z`S]uUE-d.xwWUS4YlfaNB;?JWV]lGLF`-82;p%i=V5&0<0j("
    "1EK/)2NgJ)T/mf=(iJ+MX7H:v#Oe?Tu1`?T:/d?Tu1`?T+`1P]p1d?T),QK*GsE8vvRY)Mb]d)Mccm)Mdiv)Meo)*MZ.CI*-uPxO]aj-$%ej-$>Ku>-UKTT8[N#<.DM1c`nLGJ`,2AJi"
    ":FUp@M5*ZmDr7PobjrKPdrn1pgd=PpdaXlpejt1qwRPg+bUmuu.5t+.3F4gL%cPE.2f?_/Wg5N0^xUZu(4%wugx9hL*NJ#MUpG2vejTK-bA2*/DkN5v-_lr')a5oLab5oL64Ec#'VVW-"
    "p;^w0hFI=-a,$L-b5?h-2alr'Q$B1$uB'Z-aYk@RI6339r*b2inach816bi_m#g<-r[@Q-r[@Q-$Op,0F&j8vG,s8vv7o3%^fU9`^fU9`g)fP8&8LGjK4fJj?;'9v/g$n*i]#<-/b%+'"
    "Lr/*-@x.qLxUWwAqZO&#+hG<-$7T;-D7T;-E7T;-G7T;-m7T;-v[lS.4?:R#;5T;-s5T;-u5T;-'6T;-)6T;-+6T;-D6T;-><=;&sXSb%kE(;?oEIP/L@)20Qq;;$*T#,2ZEXc2bjoo%"
    "A=0;6v_^l8,cEG)l.6D<:Qk%=>$;>,$.JY>GSDV?Mpnr->vViBmMM/DMR#F7x:L`Eh2+AFJ-[21#_7L#Yv8VQNKNj1+^n7[16.q]`]$)3<lGi^hXZ+`S)P)4l7:L#C$LucU;1a4qF:L#"
    "Z&Alf.I9>5#1?fhHg>on^.a;7=X;L#=u3E#.DP##nY(?#neAE#6]u##trL?#tqSE#D1`$#'G7@#'(gE#LI.%#44FA#44#F#c6=&#:LkA#:@5F#oZt&#BqKB#BLGF#wsB'#H3qB#HXYF#"
    "tkc+#:):J#:flF#_,1/#AGhJ#Ar(G#mVq/#IlHK#I(;G#uo?0#O.nK#O4MG#+>w0#WRNL#W@`G#3VE1#^ksL#^LrG#;oj1#d-BM#dX.H#C192#kKpM#ke@H#Q[#3#p]8a#o#od#PVj)#"
    "vu]a#u/+e#Xo8*#*QVk#)==e#@2c6#J[hn#M[ke#RcU7#W0Ro#Wt9f#b7@8#aT3p#a*Lf#lUn8#isap#h<hf#(UB:##s5r#10*h#NTm;#Br`s#FZjh#$Yc##k5.w#%G`0$PU7%#5)Fx#"
    "=Y%1$v5_'#[q2%$s-:3$[xA*#@-S1$WF35$+L0:#CH)8$EWM8$Sd)<#T589$Wd`8$oi]=#k:P>#KBP##NRk&#)puu#l;U_#]oPxk1G###*tNPS51&@']+VV$*jb2MLsJfL_&wS%KM:;$"
    "Zl:MTlVl.UrhC>#0f-G`v*;/:Z2SV--f_;$(5>uu&`F=(wSo%F5(/P#02%]SZ@n(</cJV60D8MBPH?A+EK<VQGN2VZLt68.cr9L#@LKucsYkl/#1?fh9e*YlU;1a46C;L#eBBrmWMhA5"
    ";R;L#lvuOoY`H#6?_;L#%G,`sb@aZ6pJ/]t`$`=u_7&W7Q?<L#Tk=E#.DP##oY(?#okJE#6]u##urL?#vw]E#@%M$#';%@#'4#F#H=r$#-SI@#-@5F#PU@%#3ln@#3LGF#Xne%#:4FA#"
    ":XYF#c6=&#@LkA#@elF#kNb&#HqKB#Hq(G#wsB'#N3qB#N';G#)6h'#TK?C#T3MG#tkc+#&+;G#&@`G#&.2,#,C`G#,LrG#7'?&k1'LS@2$CS@7B-5A7QdlA8B$5A?sDMB=&&/C>aZlA"
    "GM]fCCP=GDD)<MBO(u(EI%U`EJGs.CWX6AFOOmxFPfSfC`3NYGV-JVHW15GDjp+8I]WboI^Ol(ErJCPJc,$2KdnL`E$&[iKiV;JLj6.AF,Vs+Mo+ScMpTexF415DNuUk%OvsEYG<bL]O"
    "%+->P&<';HD<euP,_`rQ-^^rHN#BSR23x4S3&?SINq#s645]S754s4JVK;58:`tl8;RSlJgXw+Vnw]rdAn4MK%)GYux.cY#$mk-$sbf:d*@#:;Ah-_#8ONP&/G###bm3L#0Zer6weT#$"
    "<S5L#R,w4A#x5Z$]_6L#QD[(W#O<8%x#;YY43e4]7t4R3qwwCa?LFk4uQj-$aqSM'aZ0]oOjYfE?^7rLX4pfLgC?uu1%M$#9xTw/Pqn%#ld0'#nQ^O04&*)#RoA*#3s=(Hk%oP'F<=7'"
    "RYsx+nHel/Xt?>#6fNY5Fq5Q9mC2A=815K1J[$l;IFV,>6sH&#1;l'AfeJM'Tf8>,pT*20(9[Qscrju5Vj[i9oOM]=M&5m'Q3'p'P;[`*fnLS..57A4V3=G2L-)58eip(<w0MF.`',/("
    "L);*+h$io.0AR]4CP1h2N9DP8gu5D<x3MF.Z'3$#KXI%#^9F&#v,_'#aAP##>DW)#]7p*#u*2,#.8'%HorBHM8^IIMBDOJML+UKMVhZLMF?O5Cqm=f#kGg;-qGg;-wGg;-'Hg;-3)`5/"
    "ov.>B$aWRMCK_SMM2eTMWojUMbUpVMYv=qEw)>f#vHg;-&Ig;-,Ig;-2Ig;->*`5/)4JuB8Nm]MJwN^MN3O'MCLif#VIg;-]Ig;-cIg;-iIg;-pR,W-ngj*%#8=GM,k1HM4E%IM<vnIM"
    "DPbJM9$5'GriB,3ZD:&5a%3v6g[+p8m<$j:#BSd<.DRFH[q`e$).ae$/@ae$5Rae$;eae$Ih)2Fu-:5JQINJM[N_]PfSooSpX),W*-q>Zl]gh2g=de$4Pde$:cde$@ude$F1ee$J7Re$"
    "U+m(jX4l%l_kdumeK]ook,UiqqcMcsU#pG3Xg]e$&GOGM,k1HM29iHM8^IIM/Ix=HMDo>$DGg;-LGg;-TGg;-]Gg;-k(`5/f(:oDbtkOM%AMPM+f.QM14fQM7XFRMnEv.#&5>##,AP##"
    "_Wns-^XbgL#u`5//Yu##fRXgL'Ons-g?-IME>h;-Ns7jLOL?>#%)###[PY2rlN3r$9uK^#A3,/(*+)J%WkFJ(xCTh$Ug8F.B4$$$-l68%*W=_/UfkA#J+WMNP^IIMj>h;-%U5mTNF6##"
    "r'ChLnq?W-3*gNGuAB%#nP?(#J]&*#khc+#5tI-#U)1/#v4n0#@@T2#aK;4#+Wx5#Kc_7#lnE9#6$-;#'GM2i:r6=NRKT?NXp5@N^5Q%NENx(<68bi_(i0^#6`>W-E<OF%]7DiMn@Oa*"
    "&Ao^fmfFZ,qhJ88/MD8AKJ>8JhG88S.E28]JB,8frpp-Q.l'dM)Fis-e+3v62hr8&`,+,M3(i)N=e_6A3RJM'rr:JUfUw+VU7*@'K^&]tvr+##qE75/6nDW%s7RcV[^vA#)p_w'H'OYd"
    "wgZJ;q5KmAeZv$$Rs9+$V)L+$Z5_+$_Aq+$cM-,$gY?,$kfQ,$ord,$s(w,$w43-$%AE-$)MW-$-Yj-$1f&.$5r8.$9(K.$=4^.$A@p.$EL,/$IX>/$MeP/$Qqc/$U'v/$x;Z91_*Qr#"
    "25YY#H>E)#o_r`%&(RV-@p;2BD2siBHJSJC-J>/:hYtf:lrTG;p46)<tLm`<xeMA=&(/#>*@fY>.XF;?2q's?63_S@:K?5AEM4,Dx_Q9`)=F@7H+2$#,JY##O*Gu#i[<p#mhNp#qtap#"
    "u*tp##70q#'CBq#+OTq#/[gq#3h#r#7t5r#;*Hr#?6Zr#CBmr#^;8t#cMSt##J1;$eEvr#Ps`s#k:cu#NnJa#`S]t#`Yft#hfxt#pr4u#q.Pu#x1c:$,S1v#eZM8$G.pa#lX(Z#$bnw#"
    "tcv$$:(rv#6r_v#@:7w#JXew#eW9#$mjT#$r&q#$'EH$$-Wd$$8&E%$EJ&&$Y1,'$,>u:$J@@w#FLRw#21%w#Qq3x#V'Fx#Z3Xx#aK'#$h^B#$t,$$$#96$$-^m$$=2W%$A>j%$IV8&$"
    "X=>'$bIP'$McJ&$S%p&$x15##RGFgLZq$##/>N)#1sI-#Qrn%#E:K2#H7J5#HF7+3E8u6#i)78#trB'##1k9#heS(M)1x(#X=#s-q'#)M+=4)#^=#s-xEP)M-IF)#b=#s-1?r*M5hX)#"
    "pff=#8^I+M2ht)#YR`S%euaV$m:BP83(@8%rG#p%sX#29<bsl&#&VM'%.v.:E?5/(*Snf(+LVf:MpLG)0(0)*1k7G;UJe`*6RGA+73o(<`1B>,=0%v,>TO`<hbYV-CZ<8.Ds0A=p<ro."
    "I/TP/J;hx=xm320OYli0PYHY>*HKJ1U..,2Vx);?u[;A='.sx=(Obr?'7SY>-X4;?.nBS@/hkr?3-LS@46$5AZO`S%KlIH#:k7-#=*fH#=w[H#B-]-#CB4I#C-oH#JE+.#IZXI#I9+I#"
    "R^O.#Os'J#OE=I#Zvt.#U5LJ#UQOI#e>L/#]S$K#]^bI#mVq/#clHK#cjtI#uo?0#i.nK#iv0J#'2e0#oF<L#o,CJ#/J31#u_aL#u8UJ#7cW1#%x/M#%EhJ#?%'2#+:TM#+Q$K#ICT2#"
    "2X,N#2^6K#Q[#3#5^8a#4rdg#PVj)#;v]a#:(wg#Xo8*#CEDk#@#R0#p6p*#wDnY%WJ-T8u;3EG&)g03:NLR#:.>>#rjg:#*8P>#H<7_8PeQpoVUo1p[k4Mp_N5T%9U-Grk#*Dsm/E`s"
    "rJ&Atv]A]t%/Yuu(>c>#+GuY#l'k4oI`v7nN,jS7wG''6^s_]$#Gv&6l:j(Gg$o%#c^gX8*QCq8,EA>#dD2JLsK_%OXDld3fbEcV`YJo[U;1a4bo9L#UfT+iWMhA55@;L#f?'VmY`H#6"
    "9L;L#msY4o[r)Z6>[;L#sGrLp^.a;7K-<L#-xCxt`@As7P<<L#ndGE#%,,##oSu>#pw]E#4Vl##vlC?#v-pE#>uC$#'5r?#':,F#F7i$#-M@@#-F>F#NO7%#3fe@#3RPF#Vh[%#9(4A#"
    "9_cF#a04&#@FbA#@kuF#iHX&#F_0B#Fw1G#qa''#LwTB#L-DG##$L'#R9$C#R9VG#+<q'#XQHC#XEiG#vql+#*1DG#*R%H#(4;,#0IiG#0_7H#0L`,#6b7H#6kIH#w`=vc;d)2B<av1B"
    "A)aiBA8AJCB)WiBIYx+DGcXcDHG8JCQ4:DEM7q%FNfo+DYeQ]FSb2>GT.PcDb?juGZ?frH[O1DEl&GSIaj'5Jbnh%FtV_lJg>?MKh6I]F&2w.LmiVfLnT*>G.c8GMs=o(NtsauG6=P`N"
    "#i0AO$<BVH>nhxO)=HYP*Z#8IFH*;Q0q%8R1&ZoIP/^oR6E=PS7D;PJP'?878Gxo79Ro1KXWVP8>r929?qOiKc@@JUpwAVdqg5JL[/c8.6eD>#0BBj(Zv]e$mYkr-$,>>#5jcG*<U(^#"
    "P[$K`j>glS^qx9)vTj-$uYsx+Q5b`#Gw35&hArIq`?U+roM#2Tf4DMThwADW[X+K2[9;0$`EM0$dQ`0$h^r0$utH'Qd1qK3n09DWrHp%XvaP]X$$2>Y(<iuYO::(#JF.%#NR@%#R_R%#"
    "Vke%#DLdxLm2RA-LK=dS<X);Z1ccF.ZTIVZ0m*8[4/bo[8GBP]xGRk+jIFTE%>/.QC9;-#p5[o#TPUn#L]hn#Pi$o#Tu6o#X+Io#(T;8$HZD8$MaM8$b(P:$7$H7$7n57$jr=:$K#s8$"
    "Y;A9$bM]9$if+:$G0Z7$?<m7$NH)8$]AJ9$oxF:$lD&=#F?O&#7;#s-mdHiLG:-##J;#s-+&BkLIF?##R;#s-CU`mLQeQ##kUG+#UaFoLOkv##v;#s-b;:pLW33$#Pgb.#ms<$MZQs$#"
    "BIQ&MuG7;#tZI%#a=#s-dJ.+MT#Z0+1r$s$Fk[S%G/W]+Kd1,)U;ic)Vi8>,S>ID*i075/jXpu,vanl/B#U]=CHRV-Kf=GD^aR`E_P48.WX6AFe>0>Ghxko.hdfrH&L5GM'%.20FH*;Q"
    "?t629@#ci0ie<GV>[u/1XAno#g[E5$khW5$wg,7$$O^6$/b#7$otj5$$Ug6$w006$%CK6$O15##o^?T-JFk3.M)4GM.*^fLdx^(McmSN-#%<M-$%<M-%%<M-jcBK-kcBK-lcBK-Nf)d%"
    "]>MfU'3li''9`uYEJc+`FGMJ(GSQrm6c>>#:',GVaDbf:qO;A=+[kr?;gDMBKrt(E['NYG6H':J&>WfL6I1AOFTarQV`:MTbABH)op%P#)KoP#9&cQ#IVUR#Y1IS#jb<T#$=0U#4n#V#"
    "DHmV#T#aW#eSSX#v7cu#/`:Z#?:.[#Okw[#`Ek]#pv^^#*QQ_#:,E`#J]8a#Z7,b#khub#%Cic#5t[d#ENOe#U)Cf#fY6g#v4*h#2r/i#DL#j#T'mj#eW`k#u2Sl#/dFm#?>:n#Oo-o#"
    "`Iwo#p$kp#*U^q#:0Qr#JaDs#Z;8t#kl+u#&Juu#5xhv#ER[w#U-Ox#f^B#$v86$$0j)%$@Ds%$Puf&$aOY'$q*M($+[@)$;64*$Kg'+$[Aq+$lrd,$&MW-$6(K.$FX>/$V320$gd%1$"
    "w>o1$1pb2$AJU3$Q%I4$@=cY#*7T;-+7T;--7T;-,7T;-F#oq.IF?>#$mk-$e)+.-E^jjLPotjLIu'kLJ%1kLK+:kLL1CkLTw`*O]Ku(3^5^G3Rpxc3S#>)4T,YD4U5u`4s;R&#`fG<-"
    "X5T;-Y5T;-_YlS.eD(v#s6T;-t6T;-%7T;-/7T;-57T;-p6T;-o6T;-;AS>-@`=Z-50f_/(22F%&jp-$hY=_8Clq-$Afq-$,DBF7BMZ&MOkw&ME_e&M04JqL#</#MHMu$M6chwM0Is:Q"
    "k>[YQ8hZ-?x67#M[tO8M=tb;ND:m`-Bd6l4TeS(M#'h(MZ2$)M^D?)M`PQ)Mb]d)MCiK=#NP7+MG:-##*CrT.%5G>#[cq@-$CT;-2q8gLQ3xo%/sel^0p*2_2fj-$-u0hLX3DhL2?VhL"
    "4KihLDYh<N2Ac$M&>pV-0[5R*=Wk-$?^k-$Adk-$Cjk-$^HMk+=f?.$>u2F%vRkA#`X`=-UfG<-#IpV-`d^w''FF_&,2?.$-A2F%+^IP^2fj-$&KLpLE``pL+lrpL-x.qL/.AqL1:SqL"
    "3FfqL5RxqL7_4rL9kFrL;wXrL>3urLeVUwMm(`oIPlYSJFY:5KHlqlKJ(RMLL:3/MNh;Db;(V`b3ls-$<o2F%<Lnxc2fj-$UgMuL'&buL-QxuM2s=SRpx]8Secwr6_1jV7aCJ88i$cp8"
    "n_u.:vS4L#n7dl/w_0B#/pYc2:)@T.>vv(#lh8*#0BP##>$S-#R/f-#U@O&#T/:/#g:L/#'@$(#VSjf1S>)4#U3<)#*?S5#?=@#M0[b)#4tql/+*78#0Xs)#H=#s-nr;'M86U*#W=#s-"
    ")xo(M:Bh*#[=#s-0@G)M<N$+#a=#s-6Xl)M>Z6+#n=#s-FW@+M@gH+#r4^V-V>:$^80k%=2+[S%,)?5&0Tcu>@*pi'8xRJ(<A@S@Kd1,)AUkc)B`w4AWca]+LW`Y,Q[p.CfU>;-V8xr-"
    "W$QfCn0VS.]c95/^B2GD$00/1g[if1l;+AF0mGG2;_<A==icxF%+8>>C?pu>JL`uG9NHPAU]*2BVK9PJA)aiB[1BJC]jp1KIYx+Db[YcDc2QiKQ4:DEh0r%FiP2JLYeQ]FnZ3>Gooi+M"
    "b?juGu8grHx@JcMp>(5J)&`lJ*rb%Oxo?MK/Pw.L0:C]O*JWfL5%9GM>q$>PB0IYPKZ*;QL,5PSKj&8RR8^oRTV1MTTG>PSU=B87Vee.UT?vo7[hYP8]-FfU]p729a9%/:CNG'?d7$##"
    "TE31#I8h*%pU,kE-V/F%8HX2(GpG<-sduH2c$<gG#B)m9=jL:1flBEH5]8X$VlSEi=####";

static void ImGui_ImplDX11_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig font_config;
    font_config.GlyphOffset.x = +1;
    font_config.GlyphOffset.y = -1;
    strcpy_s(font_config.Name, "Ariblk.ttf, 24px");
    auto f = io.Fonts->AddFontFromMemoryCompressedBase85TTF(Ariblk_compressed_data_base85, 24.0f, &font_config, io.Fonts->GetGlyphRangesCyrillic());

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // add remap for local rus
    for (ImWchar i = 0; i < 256; i++)
        f->AddRemapChar(0xC0 + i, 0x410 + i, true);

    // Ёё
    f->AddRemapChar(0xA8, 0x401, true);
    f->AddRemapChar(0xB8, 0x451, true);

     // №—
    f->AddRemapChar(0xB9, 0x2116, true);
    f->AddRemapChar(0x97, 0x2014, true);

    // ҐґЄєІіЇї
    f->AddRemapChar(0xA5, 0x490, true);
    f->AddRemapChar(0xB4, 0x491, true);

    f->AddRemapChar(0xAA, 0x404, true);
    f->AddRemapChar(0xBA, 0x454, true);

    f->AddRemapChar(0xB2, 0x406, true);
    f->AddRemapChar(0xB3, 0x456, true);

    f->AddRemapChar(0xAF, 0x407, true);
    f->AddRemapChar(0xBF, 0x457, true);

    // Upload texture to graphics system
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D* pTexture = NULL;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = pixels;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &g_pFontTextureView);
        pTexture->Release();
    }

    // Store our identifier
    io.Fonts->TexID = (void*)g_pFontTextureView;

    // Create texture sampler
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias = 0.f;
        desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        desc.MinLOD = 0.f;
        desc.MaxLOD = 0.f;
        g_pd3dDevice->CreateSamplerState(&desc, &g_pFontSampler);
    }
}

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data)
{
    ID3D11DeviceContext* ctx = g_pd3dDeviceContext;

    // Create and grow vertex/index buffers if needed
    if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (g_pVB)
        {
            g_pVB->Release();
            g_pVB = NULL;
        }
        g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = g_VertexBufferSize * sizeof(ImDrawVert);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        if (g_pd3dDevice->CreateBuffer(&desc, NULL, &g_pVB) < 0)
            return;
    }
    if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (g_pIB)
        {
            g_pIB->Release();
            g_pIB = NULL;
        }
        g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = g_IndexBufferSize * sizeof(ImDrawIdx);
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (g_pd3dDevice->CreateBuffer(&desc, NULL, &g_pIB) < 0)
            return;
    }

    // Copy and convert all vertices into a single contiguous buffer
    D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
    if (ctx->Map(g_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
        return;
    if (ctx->Map(g_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
        return;
    ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
    ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtx_dst += cmd_list->VtxBuffer.Size;
        idx_dst += cmd_list->IdxBuffer.Size;
    }
    ctx->Unmap(g_pVB, 0);
    ctx->Unmap(g_pIB, 0);

    // Setup orthographic projection matrix into our constant buffer
    {
        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        if (ctx->Map(g_pVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) != S_OK)
            return;
        VERTEX_CONSTANT_BUFFER* constant_buffer = (VERTEX_CONSTANT_BUFFER*)mapped_resource.pData;
        float L = 0.0f;
        float R = ImGui::GetIO().DisplaySize.x;
        float B = ImGui::GetIO().DisplaySize.y;
        float T = 0.0f;
        float mvp[4][4] = {
            {2.0f / (R - L), 0.0f, 0.0f, 0.0f},
            {0.0f, 2.0f / (T - B), 0.0f, 0.0f},
            {0.0f, 0.0f, 0.5f, 0.0f},
            {(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f},
        };
        memcpy(&constant_buffer->mvp, mvp, sizeof(mvp));
        ctx->Unmap(g_pVertexConstantBuffer, 0);
    }

    // Backup DX state that will be modified to restore it afterwards (unfortunately this is very ugly looking and verbose. Close your eyes!)
    struct BACKUP_DX11_STATE
    {
        UINT ScissorRectsCount, ViewportsCount;
        D3D11_RECT ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
        D3D11_VIEWPORT Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
        ID3D11RasterizerState* RS;
        ID3D11BlendState* BlendState;
        FLOAT BlendFactor[4];
        UINT SampleMask;
        UINT StencilRef;
        ID3D11DepthStencilState* DepthStencilState;
        ID3D11ShaderResourceView* PSShaderResource;
        ID3D11SamplerState* PSSampler;
        ID3D11PixelShader* PS;
        ID3D11VertexShader* VS;
        UINT PSInstancesCount, VSInstancesCount;
        ID3D11ClassInstance *PSInstances[256], *VSInstances[256]; // 256 is max according to PSSetShader documentation
        D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology;
        ID3D11Buffer *IndexBuffer, *VertexBuffer, *VSConstantBuffer;
        UINT IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
        DXGI_FORMAT IndexBufferFormat;
        ID3D11InputLayout* InputLayout;
    };
    BACKUP_DX11_STATE old;
    old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    ctx->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
    ctx->RSGetViewports(&old.ViewportsCount, old.Viewports);
    ctx->RSGetState(&old.RS);
    ctx->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
    ctx->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
    ctx->PSGetShaderResources(0, 1, &old.PSShaderResource);
    ctx->PSGetSamplers(0, 1, &old.PSSampler);
    old.PSInstancesCount = old.VSInstancesCount = 256;
    ctx->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
    ctx->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
    ctx->VSGetConstantBuffers(0, 1, &old.VSConstantBuffer);
    ctx->IAGetPrimitiveTopology(&old.PrimitiveTopology);
    ctx->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
    ctx->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
    ctx->IAGetInputLayout(&old.InputLayout);

    // Setup viewport
    D3D11_VIEWPORT vp;
    memset(&vp, 0, sizeof(D3D11_VIEWPORT));
    vp.Width = ImGui::GetIO().DisplaySize.x;
    vp.Height = ImGui::GetIO().DisplaySize.y;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = vp.TopLeftY = 0.0f;
    ctx->RSSetViewports(1, &vp);

    // Bind shader and vertex buffers
    unsigned int stride = sizeof(ImDrawVert);
    unsigned int offset = 0;
    ctx->IASetInputLayout(g_pInputLayout);
    ctx->IASetVertexBuffers(0, 1, &g_pVB, &stride, &offset);
    ctx->IASetIndexBuffer(g_pIB, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(g_pVertexShader, NULL, 0);
    ctx->VSSetConstantBuffers(0, 1, &g_pVertexConstantBuffer);
    ctx->PSSetShader(g_pPixelShader, NULL, 0);
    ctx->PSSetSamplers(0, 1, &g_pFontSampler);

    // Setup render state
    const float blend_factor[4] = {0.f, 0.f, 0.f, 0.f};
    ctx->OMSetBlendState(g_pBlendState, blend_factor, 0xffffffff);
    ctx->OMSetDepthStencilState(g_pDepthStencilState, 0);
    ctx->RSSetState(g_pRasterizerState);

    // Render command lists
    int vtx_offset = 0;
    int idx_offset = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                const D3D11_RECT r = {(LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w};
                ctx->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&pcmd->TextureId);
                ctx->RSSetScissorRects(1, &r);
                ctx->DrawIndexed(pcmd->ElemCount, pcmd->IdxOffset + idx_offset, pcmd->VtxOffset + vtx_offset);
            }
        }
        vtx_offset += cmd_list->VtxBuffer.Size;
        idx_offset += cmd_list->IdxBuffer.Size;
    }

    // Restore modified DX state
    ctx->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
    ctx->RSSetViewports(old.ViewportsCount, old.Viewports);
    ctx->RSSetState(old.RS);
    if (old.RS)
        old.RS->Release();
    ctx->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask);
    if (old.BlendState)
        old.BlendState->Release();
    ctx->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef);
    if (old.DepthStencilState)
        old.DepthStencilState->Release();
    ctx->PSSetShaderResources(0, 1, &old.PSShaderResource);
    if (old.PSShaderResource)
        old.PSShaderResource->Release();
    ctx->PSSetSamplers(0, 1, &old.PSSampler);
    if (old.PSSampler)
        old.PSSampler->Release();
    ctx->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount);
    if (old.PS)
        old.PS->Release();
    for (UINT i = 0; i < old.PSInstancesCount; i++)
        if (old.PSInstances[i])
            old.PSInstances[i]->Release();
    ctx->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount);
    if (old.VS)
        old.VS->Release();
    ctx->VSSetConstantBuffers(0, 1, &old.VSConstantBuffer);
    if (old.VSConstantBuffer)
        old.VSConstantBuffer->Release();
    for (UINT i = 0; i < old.VSInstancesCount; i++)
        if (old.VSInstances[i])
            old.VSInstances[i]->Release();
    ctx->IASetPrimitiveTopology(old.PrimitiveTopology);
    ctx->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset);
    if (old.IndexBuffer)
        old.IndexBuffer->Release();
    ctx->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
    if (old.VertexBuffer)
        old.VertexBuffer->Release();
    ctx->IASetInputLayout(old.InputLayout);
    if (old.InputLayout)
        old.InputLayout->Release();
}

bool ImGui_ImplDX11_CreateDeviceObjects()
{
    if (!g_pd3dDevice)
        return false;
    if (g_pFontSampler)
        ImGui_ImplDX11_InvalidateDeviceObjects();

    // By using D3DCompile() from <d3dcompiler.h> / d3dcompiler.lib, we introduce a dependency to a given version of d3dcompiler_XX.dll (see D3DCOMPILER_DLL_A)
    // If you would like to use this DX11 sample code but remove this dependency you can:
    //  1) compile once, save the compiled shader blobs into a file or source code and pass them to CreateVertexShader()/CreatePixelShader() [preferred solution]
    //  2) use code to detect any version of the DLL and grab a pointer to D3DCompile from the DLL.
    // See https://github.com/ocornut/imgui/pull/638 for sources and details.

    // Create the vertex shader
    {
        static const char* vertexShader =
            "cbuffer vertexBuffer : register(b0) \
            {\
            float4x4 ProjectionMatrix; \
            };\
            struct VS_INPUT\
            {\
            float2 pos : POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            \
            struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            \
            PS_INPUT main(VS_INPUT input)\
            {\
            PS_INPUT output;\
            output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
            output.col = input.col;\
            output.uv  = input.uv;\
            return output;\
            }";

        D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &g_pVertexShaderBlob, NULL);
        if (g_pVertexShaderBlob ==
            NULL) // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
            return false;
        if (g_pd3dDevice->CreateVertexShader((DWORD*)g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize(), NULL, &g_pVertexShader) != S_OK)
            return false;

        // Create the input layout
        D3D11_INPUT_ELEMENT_DESC local_layout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((ImDrawVert*)0)->uv), D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        if (g_pd3dDevice->CreateInputLayout(local_layout, 3, g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize(), &g_pInputLayout) != S_OK)
            return false;

        // Create the constant buffer
        {
            D3D11_BUFFER_DESC desc;
            desc.ByteWidth = sizeof(VERTEX_CONSTANT_BUFFER);
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;
            g_pd3dDevice->CreateBuffer(&desc, NULL, &g_pVertexConstantBuffer);
        }
    }

    // Create the pixel shader
    {
        static const char* pixelShader =
            "struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
            return out_col; \
            }";

        D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &g_pPixelShaderBlob, NULL);
        if (g_pPixelShaderBlob ==
            NULL) // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
            return false;
        if (g_pd3dDevice->CreatePixelShader((DWORD*)g_pPixelShaderBlob->GetBufferPointer(), g_pPixelShaderBlob->GetBufferSize(), NULL, &g_pPixelShader) != S_OK)
            return false;
    }

    // Create the blending setup
    {
        D3D11_BLEND_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.AlphaToCoverageEnable = false;
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        g_pd3dDevice->CreateBlendState(&desc, &g_pBlendState);
    }

    // Create the rasterizer state
    {
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.ScissorEnable = true;
        desc.DepthClipEnable = true;
        g_pd3dDevice->CreateRasterizerState(&desc, &g_pRasterizerState);
    }

    // Create depth-stencil State
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        desc.StencilEnable = false;
        desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.BackFace = desc.FrontFace;
        g_pd3dDevice->CreateDepthStencilState(&desc, &g_pDepthStencilState);
    }

    ImGui_ImplDX11_CreateFontsTexture();

    return true;
}

void ImGui_ImplDX11_InvalidateDeviceObjects()
{
    ImGui::EndFrame();
    ImGui::NewFrame();

    if (!g_pd3dDevice)
        return;

    if (g_pFontSampler)
    {
        g_pFontSampler->Release();
        g_pFontSampler = NULL;
    }
    if (g_pFontTextureView)
    {
        g_pFontTextureView->Release();
        g_pFontTextureView = NULL;
        ImGui::GetIO().Fonts->TexID = NULL;
    } // We copied g_pFontTextureView to io.Fonts->TexID so let's clear that as well.
    if (g_pIB)
    {
        g_pIB->Release();
        g_pIB = NULL;
    }
    if (g_pVB)
    {
        g_pVB->Release();
        g_pVB = NULL;
    }

    if (g_pBlendState)
    {
        g_pBlendState->Release();
        g_pBlendState = NULL;
    }
    if (g_pDepthStencilState)
    {
        g_pDepthStencilState->Release();
        g_pDepthStencilState = NULL;
    }
    if (g_pRasterizerState)
    {
        g_pRasterizerState->Release();
        g_pRasterizerState = NULL;
    }
    if (g_pPixelShader)
    {
        g_pPixelShader->Release();
        g_pPixelShader = NULL;
    }
    if (g_pPixelShaderBlob)
    {
        g_pPixelShaderBlob->Release();
        g_pPixelShaderBlob = NULL;
    }
    if (g_pVertexConstantBuffer)
    {
        g_pVertexConstantBuffer->Release();
        g_pVertexConstantBuffer = NULL;
    }
    if (g_pInputLayout)
    {
        g_pInputLayout->Release();
        g_pInputLayout = NULL;
    }
    if (g_pVertexShader)
    {
        g_pVertexShader->Release();
        g_pVertexShader = NULL;
    }
    if (g_pVertexShaderBlob)
    {
        g_pVertexShaderBlob->Release();
        g_pVertexShaderBlob = NULL;
    }
}

bool ImGui_ImplDX11_Init(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context)
{
    g_hWnd = (HWND)hwnd;
    g_pd3dDevice = device;
    g_pd3dDeviceContext = device_context;

    /*if (!QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
        return false;
    if (!QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
        return false;*/

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = DIK_TAB; // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_LeftArrow] = DIK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = DIK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = DIK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = DIK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = DIK_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = DIK_NEXT;
    io.KeyMap[ImGuiKey_Home] = DIK_HOME;
    io.KeyMap[ImGuiKey_End] = DIK_END;
    io.KeyMap[ImGuiKey_Delete] = DIK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = DIK_BACK;
    io.KeyMap[ImGuiKey_Enter] = DIK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = DIK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = DIK_A;
    io.KeyMap[ImGuiKey_C] = DIK_C;
    io.KeyMap[ImGuiKey_V] = DIK_V;
    io.KeyMap[ImGuiKey_X] = DIK_X;
    io.KeyMap[ImGuiKey_Y] = DIK_Y;
    io.KeyMap[ImGuiKey_Z] = DIK_Z;

    string_path fName;
    FS.update_path(fName, "$app_data_root$", io.IniFilename);
    io.IniFilename = xr_strdup(fName);

    FS.update_path(fName, "$logs$", io.LogFilename);
    io.LogFilename = xr_strdup(fName);

    io.ImeWindowHandle = g_hWnd;

    ImGui_ImplDX11_CreateDeviceObjects();

    return true;
}

void ImGui_ImplDX11_Shutdown()
{
    ImGui_ImplDX11_InvalidateDeviceObjects();
    g_pd3dDevice = NULL;
    g_pd3dDeviceContext = NULL;
    g_hWnd = (HWND)0;

    ImGuiIO& io = ImGui::GetIO();
    xr_free(io.IniFilename);
    xr_free(io.LogFilename);
}

void ImGui_ImplDX11_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    RECT rect;
    GetClientRect(Device.m_hWnd, &rect);
    io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

    if (g_TicksPerSecond == 0)
    {
        QueryPerformanceFrequency((LARGE_INTEGER*)&g_TicksPerSecond);
        QueryPerformanceCounter((LARGE_INTEGER*)&g_Time);
    }

    // Setup time step
    INT64 current_time;
    QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
    io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;

    g_Time = current_time;

    // Read keyboard modifiers inputs
    // io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    // io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    // io.KeyAlt =  (GetKeyState(VK_MENU) & 0x8000) != 0;
    // io.KeySuper = false;
    // io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
    // io.MousePos : filled by WM_MOUSEMOVE events
    // io.MouseDown : filled by WM_*BUTTON* events
    // io.MouseWheel : filled by WM_MOUSEWHEEL events

    // Hide OS mouse cursor if ImGui is drawing it
    // if (io.MouseDrawCursor)
    //	SetCursor(NULL);

    extern void Editor_OnFrame();

    Editor_OnFrame();

    // Start the frame
    ImGui::NewFrame();
    // ImGuizmo::BeginFrame(); // TODO ImGui For HUD Editor
}
