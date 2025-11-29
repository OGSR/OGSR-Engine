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

static HWND g_hWnd = nullptr;
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static ID3D11Buffer* g_pVB = nullptr;
static ID3D11Buffer* g_pIB = nullptr;
static ID3D10Blob* g_pVertexShaderBlob = nullptr;
static ID3D11VertexShader* g_pVertexShader = nullptr;
static ID3D11InputLayout* g_pInputLayout = nullptr;
static ID3D11Buffer* g_pVertexConstantBuffer = nullptr;
static ID3D10Blob* g_pPixelShaderBlob = nullptr;
static ID3D11PixelShader* g_pPixelShader = nullptr;
static ID3D11SamplerState* g_pFontSampler = nullptr;
static ID3D11ShaderResourceView* g_pFontTextureView = nullptr;
static ID3D11RasterizerState* g_pRasterizerState = nullptr;
static ID3D11BlendState* g_pBlendState = nullptr;
static ID3D11DepthStencilState* g_pDepthStencilState = nullptr;

static int g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

// need for imgui
static INT64 g_Time = 0;
static INT64 g_TicksPerSecond = 0;

struct VERTEX_CONSTANT_BUFFER
{
    float mvp[4][4];
};

// File: 'Terminuz14.ttf' (81308 bytes)
// Exported using binary_to_compressed_c.cpp
constexpr const char Terminuz14_compressed_data_base85[53080 + 1] =
    "7])#######So85U'/###[),##.f1$#Q6>##%[n420tjXr0.>>#)jNC4aNV=BGQ`5#GLJM'%xmo%X`(*Hlme+M`cSV6<d5&5#-0%J/1jP(mSZ`*hW&]t<.d<BnB47Eqq.>-1@pV-TT$=("
    "%Y#n$-hG<-q*.m/<_[FHUrG%EU^>E6tD/2'n2JuB6<-/>',1,)[[qr$sJ[^IbDL/#r5S>-oU+41+>00Fq$1j**mDA+uOgk0P%S+HGE5i64)&#,+6)Z-g^A_1K/QlJbuciAeI1=#PpNw9"
    "h=+Vdktm#MXn,VdC`j7#_=&&$%C7#M_nv##UE31#N&###f4/>5FVO;->=Ke$Ckl'&(lT#$e#<X(X@(&l,@0:)g>t9)v`O;-eXZV$E2aA+2-cw'MUNF%sG[8#<J_/#*VUV$cvo*%FJ)4#"
    "51i$#eQj(N:.GL<VUHS7[I0AuJ;AwTS&8O+G4GCi^wUJ`GTK_&Zr&@TjEGqV0,ww'5eSF%U04>dCRB.$eh/F.CqC_&Is/kL7wB=Rd9^KNgkR`YZ:d9NbE$mPu&#GNYeHRWJQ7US):S48"
    "jJ:K*xxgt7EQ]F..WB_OP0R^OZ[*vNE11eSgRS780x).HF2_f/$fv>R4etX.TE]':MYLR8S>FW81IKL8%QYF%lWAvP/tne79#HppA`eJ8caQ&Q)coG&#KZ$$pPBA9A`6db#(-A%cXhDN"
    "GX9l'RLsT*.;nRNThbjTEO*_&>T_#C4vg0?:)PqTkwZ.V4B=@[fg@'8@'i=2ksd$#+cso9j6,?e2:(&(FK=<-DKv.)9%@<-w&##1TGiQSis^Q.t$t^P.r//1+hZ*W>KZu7(>[w'b=.l+"
    "s+b2-psbsQu0Sv&<WDD2aerm8+PBe=`%9:ef-bZ%)^u8ReWv.Pg['AUgFhTSc'>5*s/_'#?A3.$[?l6#)(MT.?ww%#-4ip.Qwrr$oFEX(Zl14+;10.$I8YY#0%,DN6&l4S?U^Y%^Q(v>"
    ",]UV$0u68%47no%8ONP&<h/2'@*gi'DBGJ(HZ(,)Ls_c)P5@D*TMw%+XfW]+gSY)N_-4&#c9F&#gEX&#kQk&#o^''#sj9'#wvK'#%-_'#)9q'#-E-(#1Q?(#5^Q(#9jd(#=vv(#A,3)#"
    "E8E)#IDW)#MPj)#Q]&*#Ui8*#YuJ*#^+^*#b7p*#fC,+#jO>+#n[P+#rhc+#vtu+#$+2,#(7D,#,CV,#0Oi,#4[%-#fT_pLJqanM21G`N@BWrQXx-JUhhEcVm9^%XoE#AXwv:YY#-VuY"
    ")QRrZ+^n7[1,k4]6A0P];iGi^@:`+`EO%G`^e35JMMWiBES4GDo##8I3HJ]FrbpxOW:PcDXB-DNdtS`WEOGVZ:wUfC4s]3FuOkA#Pd%T%Nk9PJZ_=]X^-p.LH%h%FctPP8qq[fUbvTlJ"
    "Pc#v#)S/GM#E6,)TWW]Fea48Ij^8;HSZa._+e8`a[t$##0VEJLr>Sw9M@ZY#o`Hg#mlQg#%;w0#15n0#9`W1#A.92#Ax&2#%@AvLb1`2#['Z3#]7/X#-Ri,#DIiG#?R^O0Qb7H#-:D,#"
    "AA8vL_dB(MFr%5%/NA>#nO3MKp]ZiK<**;QVxQfUNLp]+<fe]4*RXV?JIVYGx@Qc`t3Mci_ABv#;_/m&2<@AOW7acVBC5Gr(<PJU'9%vYEI.,`V0ZcruR4)4,aQf`'g^(t2C`D+Ns9v-"
    "e_bJ2&6s]53v4v65Ka`XjhvuZ54Q,*XSZ8/NG0&YF?OMhq/Qv$$di8&>hrD+[u%Q0g7U5B%:F^#EQ9Z-5X(6'>5r#$%bFdE-;Z&PLg%KVt0NvZ,6(Q^:/wJ`H4P&cU0e;e/c:?6Gu-98"
    "3U[mBAZ5HE,XvXu:v:'olD4;-;1&GVV<]Kl6*^KlJ&Pi^r;1)3WTS>?pYx/`r'2GiYvE##08VJMcg_m/[v@1$uU:;$]p@L$/pkM$^<KQ$wo%u,L95n,>_@p,]R5;-:N4>-w3q4.;$c(P"
    "eA7*N:@KVM#[[&M[a:)Ne]Ou-ccXjLTX+'.hFQl.E5@h.bQ2M/vg[p.F0.tL2@2)/RrQl.hY2L/?@q4.IubnMI^28$#'>uusc$4vG213v2W=2vZ4^.vlk*xu2WftufAAtuoV_nuL4Tiu"
    "##egu1Ircu^#*Du7[^Uu3OKUu43JPurY-Kuf5LJuK[.HunabDmZM[0mrCu.m]kNdlKFnclE.Icl/GCbl##cal4<L>lM@5Ul=RvJlr?0IlK[X=l^RG:lEPl2lkCY2laif1lJ2j0lrXw,l"
    "#dxdk1o:nkm%J[k')g5kVB=9k7cl6kq=56k_Dj4kN,E4kB^d3k:E?3k0-q2ks'=1kM&c&k*oP#k__5#kZ=m/$&*,##/L/(MBFHciYTpdmxiY/$D#%##6;=e#Mi?O$vGY##B$(,)k_Y)4"
    "#;GjL3TF:.oeY9C;a4I$FI,bu%kLEdOA&U:TAW`#wPd;#fKb&##$nO(NZ)F3PImO(NIHg)09tM(CG>8.e.Y)4Th^;.O@;=-7ENfLX%[2MhbpC-H2cc-vqHq;9SQ>d4pMBMPvBs.&,###"
    "pN`-QG3BJ1;As?B<,B+4R%XD#Y6J)4_i%RMxO%.MC;Z)4VE,RER[kD#8$JY-7b/I$]66GM>^sfL<VEYNE,sFrZ_f2CtEJEd^;nHH7L:C&Fr+^u?6NQd0P1bRVZA>#r+TV-WD:-m+m`wK"
    "t#bG3TcW;.1:<$gnv@+4_'M['Hpo6*Z$UfLqhAGM^X61MA#>sL,rAGMc2D9.-fIGMAe3>d;'3=(fXlGMYmS9CDfS&#Ei4p756rp/*Z>G2f?-IMRerA.4Djc)v(v<C[_G)4JWoG38<bJ-"
    "A@;=-7eh9MDD4jLt@)aO].k?NQYx7M/C'g-#Z1@'_LHQ(0.2t-FLi%MVX?>#U08G`jwdG;*L]'J2oTEn.X*W-`.[q2F$2'#+W@)$SdmeMvndLMXf*F3A1(gLh]CDNnX>W-4Yg6Epk-YM"
    "H<^;-gt<^-]p?e6f+mM(hg,qi/KCZ6xv//11U$gL0I(]$#vJ['kL,F7mdU3X`B(7#PhFJ(Q,k'8S:Z58a,>)4GU@)4OS7[^5sX?^EtE>dF/n;Si8I:M$GF3NtklgL9T8c$pbX;.PvxTM"
    "GL`-6iW4G`x]5>d`Je9`A2)O#62@8#f8iu5(>abMJkO?-g@dH.t16G`B*@v$.:#Z-?/aZ-(BK?dLSimLp;$##)s/kLchs>.Krgo.G7Rk+MH4PdQiIfLfD2G`%R,)*nf^G3tq-L5KL3g)"
    "SVSmCmrjrM5'8DCe*loLA8'@%H>uu#AYJ&#s_Vj(eqho.08m*4T#NF386fX-uq:X6L/'=:8iYB]3ctM(Z-jG3Wk2b$afv1';Q2Y$7PgIh-A'$$n@)-uJl=$^,hAp.eTA;?vAG_&12Qj("
    "d-LhLp-;hL>/H)4[#HZ6ZDsI37sG%.[k3qLgqj^-R%3R3$x9[Te(+kk=_EM0Aw;O4bo*CSTN`hL>:xiLa:s'N2RFjL/<x9.M6&tu2QlZpmr<Aux+KgL%KJEd9G@2CrMsPhX5fjLAPSF-"
    "?7Gc-Wcg9;V(m3FEJ1W-HlmW_dl.&4e/KC/8BF:.`^K]M+-J6M%ujnLSBiMN7Na+Mnit;.(=8DC#a*^#KTp0)B+$##shL%6@Q3Z6=&)P-jw#1/IMjD#4x)O=C23+@>1T9`FX`78+adpK"
    "l_2G`-E[Y,pIXw5qntM(P];Y$w_PZI0IXD#lErn3)lKHMx%6D-F##k-AveE[-q$1YIH13)ipJ%#RaX59n?^;.ODf48X#h>$2A]'.<&ffLf]5p8j@[0>;6&tu8AJj/n?^6$*jLEdCX1&G"
    "h?+?dksp=GU&>;?Lh=e*WKBD37?sI3o9MP8'XBN(d`MD3$iYhLgTTC-CW<#.t0O$M,CFQdo8tP8U^]2`fjJ%#YIV+8]Fq#$9wZ)4qwMi-F;9eZK[hGM(Um=M((Qt$D8xfL;E)qLKb;;?"
    "B)K?@&rx,&%s8>,QrtcM=(Ig)b9u`#UCsM-N;8F-Kg(P-F&Qt$&EMe$=K-2huZp5L/2o#/o6B2#8d0Wft$3G`.uQS%WKBD3R_Yw5^>(B#Ek=DClFP?pA4NQd%5YY#3[&JUnvE_/^[/2'"
    "6a=T8]0G)4Lv,U8n-F2CNfS(%Av(q'%Q+1#rnZN9MLg;.HDYL#QS=,M]DA+4sFbA#f&9m8-KYb%,dX58PH=kOub./1EU:kOikfi'pIXw5s<6g)BP&d)H'c,/=]a19Z$Xq28xxa#r=:DC"
    ",4(<.Y'3:.LR_GM?UZe%T$%HM=jx(:-fJm_2a<(8I)Z,*HJsQNY]8^#mI_P%3gqZ6$*x9.vCjc)N:Tk$sA[]u7%.XU&;$<-T*1%/gCq=Gmx6kX&xT^#wPd;#9OKC-,:uo0iRG)4d`MD3"
    "P6lD#bUV)4lAZEn35_)l]M[#%rq)tqPIr@C%63gL2;BoL(f?=luXV;:f3)d*O-ap7gI'N(.aeKY]B8p.>IHg)KBAqDQjo;-<;m`-%?n'83KKwMHF_19$tn=GNcW9`2W]B]=MQT.Oke%#"
    "psBe8%FGH386fX-B>xfL9k]V6.G+_.@pvC#Dn,H.=9W=YE.i;-6nVE-'=>l&=Y6>dCl6>dXQ4>dMlC<-A2ZX$PT/i)`?QcM+fNN0]Xj=.XuQ4Cli.[8RtW>dLJ:0CstC)N'%/qLP9M+8"
    "_%Jp&`4Qq;3G-U;<:e8.$>0N(CG(H3'Ec59N^3LYs),eZ(QCk=m5nA#WctJ-xE5w.C.eX-0^k-$FP&d)IKk%M2Wt1'$#bIh4dPW-t1(7jq/T<-a+ex-^Hv(N,@P[$rA5>d(FFe-XKRhL"
    ",m0a#%xw[-Ux$T.U9KZ-D7K$..)@j:FYkA#T&-X$&/GwKo%_acx66&%PE.gL%<?>#xX'^#MF1gL(.AX-QLmO(_0hfLJ1;X6Ea%I#51Gou*2bf-pbW*@1F-iLid,pLv-Rq$f7f:d`le^$"
    "bS###HL3]-Jl*F3m>sI3>?xTM;j8>CBltdO7+J6Mb]uG-p+.A-vo-A-em%n$2ffA$N$fe6>V@2CGE]9MV#nHHRju<:Y;/9/pIXw5*)-X$bYDD3wD9m8;,:wgh$Lh5bB&<-V^(*NUGKoL"
    "H(i%MhZ')Mwp//181R<-<J=?/@V5g)_<$gL>V#I#?+>'$`aGTMVIJEd+X<<--m>q8@5ij;1Ex9.vbGg)Z02N(:lgw%O]iBJ1+c&uj8$C#lHDHuK>6MO.BXfM2Pg$.uxpkL2*^fLP%t^-"
    "$V9L5tjB[9Ef2'M9N&;Qn]5W-%k-1,@Q*58AMtM(KZ:]$9prW/P?H)4WB8p.:8QQqktS9C[1f?8*fkKj'c?=lfB;gL,=`QO&L.IMRbtG%c6Dv$RW>Z6pAPEdSBm]uR&#GrF+0kkKr+A8"
    "3p=R*t8vER>;Dk#wPd;#F*.F']#G:.>aBK-pmc[.s0YD#NTiM%)Vu?9(d?2C>8:<-'k/V-+%hK-OI/j/nj9'#u<p]$cA5W-,KG$eOXj=.ug)Z$QoC)4]$%-M,YS;N]/G@MC1.AM1C;B'"
    "Nmi?BkBd>$54A']=p*H=;`su5w-t`#W<$hcw</9.odT3C#&6<-_9>%MqdUE:h+Uw0]QOp.2t=Snku_$'(?sI3XgfX-)-e#$9s/kL*[;;?Zw@W-(SgB]4.*X-]1#qrDMXw5u+[J(.Cil8"
    "agB#$5fB,MXx=SnrG5>dOxs%uCK'wLF$TfL7d&c$'3<F7mF+F3(0p[6o-e2MrXkD#A2K;-Af:a%c$g2CpI%2'2xN8pWK]d;Kc]2`X;KS8L;mg)NBCD3gwdLMYTbg$uQ;X6P0'`&rJnAu"
    "@c%I#rx;k:ChtA#N9%XSS1_KM2,OF31v6i$&B=p7,ZP6WQ01@@VeH1MN,J,8K?pTV9@h<CB1EF%e#&gLD.vW-S34WJFAtFN=JXD#U@XO)/KF:.CQ7eMsrT)&]gM#$d20oLx/Qt$)Vd<-"
    "S-MM%?qZGM@wgG3Wtko&;6&tu'jqh$w0E9r(26W-[Ytq'Ifl`%bGqp.@$(,)chomCCY<f/_9$,$N/B=l=*hqKQTER8p?GB$>Hi2%bfV,<D4=2C+NZbcEEo(<01oP'W?v<.Q&[J(a0YD#"
    "uC.@#&-/rLxlS9CB%b,2(=2/13Yo.CZ42G`-;ii'D0W)Nm#IF<wM1_ASVe&6ScQX.nC`0$U(C@/]3Xc2xAH/;QctA#O@]3b]<'U.N#G:.sSoo7_ug>$>ALm82+U^#E0`$#:*a'%m<#C-"
    "]w3]-ruQJ(96&tu4p(;Q=Jr@C(x9^u$-ddubr:8CoBoNO6`NrLC<:58Zx.9&5Y###E?\?N;L%CB#=xs`#_q>b#qlme%7Idruhhv1'e*loL*5<+MC82G`hidD+N:CgLZ3RA->9Zl$jXj=."
    "=Q5K:02v*7':'n(aHqx;5*S5's1_*e_?@+4hD]eMo0sFrY7VP-XM)Y$tW-j8:a<>d)DwmL;sZm%gAc/)f_h-4XJlh:ULu)4&0fX-v?CLEfb8;/B.'U#,*loL6r*'MLG#I#WnLW8PCOn-"
    "vG&LPO1i;-N(bj$Te0a#aZaQji5xQ;ac<>dC3+P:Z=^9VZ_7_f+5a`#i7'G.OhFJ(J1rM(U4`29l)[#6sp-/C>PS]uC,gc$:6&tueK%=(fnBi8_YX]uQsEhP?<XW-VAGq;@oWD#Tg;hL"
    "WAuM(=?]<:k2@2h[YLxLPtT=YVcR?R^6E$#5,>>#$e-^#&,-W-@w+F%@w//1+gR]u_?P%MLLuK.eTkNdt'e-6<YZ9`1mCd*Xj6.60YQ<-eeUp$I./d;jf/:`XW&CA)Na09,9tY-*k^&="
    "+O.0Wlnm.'05H>d),4cGD6]Y-xN>kX^m,.muc%Z-XPJ#$]5Vh#%<@3rt6emL5`$6%ZB(7#$3_d%r_b'4cQ3Z6iDTe$/I%;QSne8.extBC;SxA#JiQe$;j5/1(kWFeseiJ)GtR-k5kK&$"
    "+iNB-;dab$7PgIh(g>>dL`]C%@=@<-T$rd-?%%V'>nkD#<XYw5sj$/C'Mf;f>D'tu/Q?p-ej9@'*(:B##(1<d/xeV'ko###3V/d)J=Hg)e^3Z66$Jp7q4J3Mxq7DC;HV]uhe$q$7I(C&"
    "F0>>>d((kbffXkO?,eJ(d>5<-'$%uLTO.Pd]&5W-Z(tQNe2&gL^88W8hZ7_8_#EqM<sMF3o-$,Mux+[$=XsB8TG:>doL2W-GO)nj#TD^=^dfQaqV1.;lpGg)/:rkLm1H)413Ch$-$7tq"
    "aw.W-n%9kO+-Jt]_px-;S4Hg)0a5F<-9kM(Dd0%%OBZ)4EI^c9%o>e6pH,,9wAR,XjE3R%00O$M'r]0:866Z$kF#n_5_.Pd0:Ce#?h8,Ml,BHuB4Fc%cL*d*1cDT.'-p[6Z8ix$Rt;8."
    "1m=o$de(gLH+[6%HYnhsBK`.&v*fG*[KX'8axpkL(nHg)jdu-$Wl(<-,;sm.Bp#2'#O3^-*a34O(RE<-XcVU%4k3<-jfmX$)4/g)`g<dMw47F'qZN0YZn%68EUC_AIIr;-?/]^.qEwY6"
    "c5+G1#xw[-'Yj=.,?[S#5sP)/l:2/1YEV$9obh=Mgl80%UeLgLLYP&#A(qkL88H)4s,db<TOl(-A+Yc-fe'*lk/WQ..[a19CM:m8RWF&#uU'B#*`Uc;p+D)+_&-$6n$1N(J0nO(p/mER"
    "BB3jL`;EJ8iHF]uS5=gLlGHe%D:2Y]:'>Sn`S_$'u4C*eQpCSn(;OxBm@0B#uxxiLYcQLMBXuk-nvL$Bue-lL+*i]$9A@2CUU6HMdR?>#FVO;-AK]G;hM5W-[#7w[sk;i-1dI0Y^TIg)"
    "#3N,MoYmj;m`3>dI'4>d%qmLNrUZL-S)iR=0rXAuraPSd53MM%#I*1#p<pv$7ACFcO?Z)4g>V4DY[-lL>kK&$S=2=-HS4W%=T(_]Zp:8Cbm_d-2<h<L8mb;-^xvj9/hrQN()pu,d-LhL"
    "<j*F3xwaB-v%'g/d$E8.;LKu.NwmGMkU.PdELIb%2>n2C-=MGM-4br$]Q&Q8b:q^#A/s.CZ9V;-?,sV.q`6[92R95:W%pG3:]qpp,BF:.IP&<.b2v[-Do4lM-OA/.^q&,9$&%2heUDW-"
    "/VZ(6peP;-TGO2143:E</BT;.hPXisRwX8Mj'(l%NMUwB?lHI$(vW#$1[,H#_5%>%Y-L#>IaxQEWxQS%`>ru5#0,na&3(B#2@)'5,Kdruwsp=G&5>##&<Dk#0Q,+##?$(#H?EU-5qc[$"
    "0iE&=F#R#6GImhMW$*+M(:#gL<COF3BPg/)]H7g)>d/Co4@?,M5`%3%bHqHHI+]2C4NP6Mb44T=#gE>dwv$F7O/5##vbDs#0Vf5#9YQ/)_1b;.O2o>/7CK?d#E7#MA:$##X^j-$uVO;-"
    "akRP&VrSY,vsai0tc%Z-O$k5/B1Wq.w&pS8k>69KN[j=.&)d9%+GDn<Z3cQuD+M*MO>_190RVA.=Jr@C)BE,M`c+k%8vjGM>A'tu6fS^6he^-#62xH#wh?O$*/`$#.qvIMRSt`#-s$N0"
    "QiL%6b=WD#RmwiLb&lSMX%8DCk=.qLn'k=GJ?%m-eb_3FPpCSnNo4[$@A9e?G)J#$TVo^#fF-(#&m_u7/[c6<.F^L;JbmI40peC>k7T;.*l`5/vl.&4sK.29]rR4;GbFW%VXG_&q[;q7"
    "f9m&6wwSfLc#6L%pSXV-ituM(risFr9ZR^#ISA;?2K['#42xH#eF`G#I+ofLB#hG3@u[w'im[*@iLsM0#@GGMH#[`**Og+Mn/6X$`#Yq.NoWD#,]Q)4i(U#&Ml-YN1.VT.X3cQumgmL-"
    "/a8K%/(a(9o]b@n?C_19W7JVM/TnuLq]<S8h3O2CNJHW8^2),)k3v>$&C])QRl]D$T^BK-u0`g@iYgZ-+rJu.ov@+42i@m/]a)I#qs[*$nH`9.`1d-#ia1I$pb7G`EWCG)?kS#6%UjU8"
    "5,*Z6MS.HFU^ne-'Vl?TT8W=YUx5Pdfv>2Ccg4F7w)loL)V-+#k]83%#Ja_=5.,H3m>sI3L9sP_Iq-E.[?$2'iQD,M5-5k%UYo.C$+fG*@a3QAI'FA@OBZ)4k#juLSVfZ$6`NrLAwMV="
    "/f_$'q3n0#tlIfLO8sFr2e[V$'K5<-Xj=x9l'q>$bsX?^b9DgLov1-MA^Yw5+PafLnB4Z69X5H3ee-lLB6?X$V=(bnkW4G`wgCN`5W/c$Z8Yc;bF0et3Rt`#vBk)+802N(p[3[9^]Gm8"
    ",HwgucNBm/(<0/1gjZK#IUnuL2?9C-2_IU.VO]t#@PAZ$Mm'W-jBv9DaYo.CBA';Q9;:8%(biR8txJfFK.Xj.DAwo#n^87.dJTp7%n0[K%bbB]D-ZGC[3//1ksxs--+Qq7DLGGc@g*P("
    "2WYoLag0h$b*Z^##v@P#S;nmLB;^duDPL6MMY;;?JpxP8Z5h^#gL6(#jiPlLjD%+%e)ahLxeAK%]8d5hU_.f$-ZKd>E/G@MX%=F3i=MrAPT*HX1fG<-lnk4%Z@vP83jx+.24M3XdW`-#"
    "a6;$v-i?O$O`''#8pm(#n)i?BT#>)4lY:K::$wY6T<<O9Q8^;.AI76/sQ3Z6C]b/M8rcW%u=ZqM31GouCl8>C<.#7CVKS>PUd?=l$f#gLxsJfLHF?##YDBc.<&*)#NZFf-o4AB-C==S&"
    "Pm3jLAw'kLYv1x$Fw#'6272X-mE3j*0ot;M=s//1fqT&#oeI#14,3)#N]&*#Ip@+4$$3gL@Z2u7OZtY-)f_s/2IXD#kU#gLQ1M8.XBPdt-PI3'oo9XJ@3ou-Fo()<CHi>n/E=<'s[*w7"
    "?<X2C1:$OA&:)d*ecGp7sN0N(9o/p9UDTZ-e[kD#0_LJF4[>l$`I:0C$2-W-a[.[KaV$Q/UX4G`D7>##$xg^#?Wf5#VCKu?4x9A7;%b'4dxjJ/)IuM(js?8@?N?i3doSj%JT0AMmqP;-"
    "u-B=l(k)gL5gVa-1_^dFNuEE-v[Ag%5@Q1s14fQMtJJEdB,b>$I(R/)Z;8p/[AH)</LY)4Ckp+Mo5aIN]e'KMP,sq%aXr;-OY:H-4a*r$LIBB6N7Fn->*q6*oXm;-lGmIP5w1'#bWs^%"
    "?d9W-]]QLGs^ei9(#tY-_[7>d=5d9Dww(x'?#kCJlcbGMdQsf%AVJ&#gJQd*bOPV-faXW-UHCv[M0H)4]]wY%PcM3bgI$edtZTEn;AK?d[#Y/:pL)UMJ4G^#:%o%M1sx,&7f18.JC$##"
    "/PliakTJf$2,oA#6o&k$CuWD#kJ/J3Ga*QA@+$XhIT4W-_pu9)b?)-u9gJL:_;Pb%VWvTMr@M<-_.7'&:^$N0HgfX-AA0N(./)I;,-Oso0O.PduD?2C,<B<-Rk4c-%R(0<BArP-SsUX%"
    "baLANbcE,<D.0Z-WGs($,nHg)P^+W-cYSwBaWVQ1ZEX,%LeuS@QO@*-4Q&;Q]xWC&XUf5#HXI%#3@gG3%h>w$LAwY-%Ht58)<i^oVRB>QJZ0<-6Fo^$qWm>$?k;3(jWCG)iD'ZJ1s[>/"
    "'vsI3kq?lL(6K4M[s//1d0KJrddYT^2B'oLF)>W%-f*a+l,eA#K1<#6)k(7<kfCT.KprHCV*Aj%R*U9Cj)F@.-eOECqlY9rPl1<-&Xwq$^wr;-gHTZ$YgfX-SI(gL'<Y2C4*2p$Jt'2h"
    "AOQ,Me+B5;nMF&#io+gL]Vj$#=,>>#NREn<R(Rs-dOMn8erTB[f#k=G4BK>d)=>@R;C,-Mm?^-#FUh@Ij6K$.-d0=Q1TY++ij<p7/$ZEnxVH2CChf#%ke$kt;o(;Q0`>X(q3n0#qn9Lj"
    "rg>%.8;hxP9jDv#Lk3r$^TIg)`$@gLlr7DC%UM:VkDJgLt@GGM-H1[MGgA%#CTZ`-$8Np^vB33M#6fQMqOJEdU`v;])($xLV>2G`U'v4Vo#K%#]$(,)Ei-62?'L&$Auq[6g0tG<//w<("
    "*;BgLG=(=%@A@Sn@s'Q8d(=2C6>h;-mvdN9.%Q,*FNQ1%B(r;-lX_p$ue-lL$*x9.lZ'T.o#G:.nX-Q%?-op7d0vAmR8RtL/O&;QY[e8.)h[G;V?>2CA;Q32$9e;@Pw'6;GIuG-]F_L&"
    "@C8n;&Yjj$WR-jB]&X>dLf';Q5.M&OeX&%#uABjCYcWZ6)IuM(wmeM9A7=2C5F%(As)_^u50eQ%h>9XL8cQj9WJ'N(qk0C.?b0a#(m`a-nvL$BsuJgLKqJ*?UV,mqHV@M.(=2/1:JoA#"
    "[DJb-p0.WSdQs$#N:<$Q;0gfLHKXD#DAQ'm&*Fn%T#4jLc/]Q-]b_t%pF%*>uR;GBr2O2CGRa'?v05e9-<O2((tHOW1c>u@6,EeH2`J&$eG5<-YkMI&&N-qitg_S#qcJ,MTFSb-Z9-Ec"
    "?,sFrTPh;-iDQw$rG,W-%A?tJtf%Y%oE&N9H#ZF>?X:aNh&wm%?xiQawWj-$mVO;-(n(&+'wsM9V->)4?Uw;-73cg$7JwC#<pTN(HJjD#jZ*iM0])>&998w.&CGGM>fEto4R7U._R^du"
    "H4K;-bOo)<V%RW&lD>&#q)5C&vXj=.T7K&$VMXw5w1eJ(O#)a<rc(R<x,[P$nGp;-i[pn$VR5C&u9)I#ZDZ=&PUf5#0N'JAOb0^#5ci<AEF6t/U+1p$sPZ2C1:uT:*(EVne#3^#4O,W-"
    "4O/7)xjlw.GR@%#gMYq;aSY++)R_`$bE8W-*M,O2-($xL@)e7:+i&KMv5dS#8se^$.#I2C0]DY&vdFKN;o*F3HtTfL50_o%IjTj9Pe1nW<Bdg*rapC-/XIu/uCpFCKboBd8m#`-q%/kX"
    "/Vlr-/U(`$L]1W-7,-.4g3a*%Qt;8.n1MaELskA#W(tj$T*FW-wP_Kc*AQ29<ZO&#P%qc$Sd(;?[[6BR0KF:..0;hLCckV$t6DgLMA^dunq$p#58+FE8qe&,qW*20,6AgLlsk_$nXj=."
    "I3H#%EFiG3)Rg_-u6Ak=CZJ^?c_Up$Rlg?F(5>x80.6xL?WaIh^^>nEe^&SEZFBq^<LR4Cn8kM(nS-Z6+OmB]C)X,Mn[2>M5tT=YLjVqLBXU68j7,GcsVaIh=Vu]OeX&%#Y%]D=%psZ6"
    "thL%6c=WD##rIxetZgC-k&Bp:p'0*u_PR^$]m*[Kg2^>Pw+^%#ZhFJ(=KrQa7Wi7:_SHF9Bv3>dv@A4)bimF9WAN.d7/Z29+^Z$$t<r$#s&U'#t't`#Fh*N(kLo^-'uYV-XXTV6^LG-G"
    "e)v<%HfnGX'x7DC'HIRN1P9gL:<e4EYUFuBbQ&%#sE9V%#3tG3:PihlC7w8.B7E:.3;6>dKEgHG+j`a-HN5[[lnS9C%91S-:2bV$6[8<-ne'S-K$iF:Ja,g)tM@0lgu&tqDRa:8K#wY6"
    "rM?*M^1,[$$8q;-PoH&/n0Biu;JN6sN6QtLbBQX$MZGth(lw(mKPFjLXX7+%N?5s87:kM(3mGs-^5F%?V1G)4/_9>>ksN>d3g2'MtPXg$G/_/4:@,-Mr?,e>qZ_@I_uqI;9L'N(`';(v"
    "FD4jLvp.B.R0KZ-)jo,M*T+G/B*iIhG(N6M)-IE&22I2Cqx8;-7-I]%q)@D*&bm2VdC96>nxvY62Vob-QO%[@'<m`-Ap]Bdf0e6%@=Kt_ArwZ^(2Xe+<$>SnxJvD-*L3d>=85F@d=mhM"
    "1DjQ.kh-PdUXR/:wrDvo4O.Pd6<ap9_MgS8$*q>$O9a?^E,-W-H,-F%6^/hLamS9Ci4F>Q)Nh)<V4#VDuC#gLVB`oG#cgJ)DOI<-6Y]F-a>S'._3$,MgjFo$KN:uJ/v`E-1Y^t9=_iB6"
    "Ce4w$K0O>d7pPfL;6/,;N4$$$mh1$#c5[a-b&QJ(Fs*QAr_[?TZx)?G==]>etqYO%p2SQA:]JmCfwHm%bjk8.(gY9CW6'9.(p(;Qh[b>>(kK>?bW<p&[3^-Q]Bm(N.1&,;I#E'm19TkM"
    "_L^s$el6T.@4i$#owkR-Rc+o$q@i;-cbY[$wZ8>dr'hhlNRC[%&xO)'6%io.5))A7?V):%n6lD#)w@+4(>tM(8Egw#iqi;-mK3i%%ckA#x>J3%6TBj9Y,q'%/ecgLp-sFr*2Q<-3P3]A"
    "L)wY6/)UW$9a)F3eD'UDHINg)6SHp8aFk2C$QgIh2Ob$%Ef2'M-+vBCs`+gLMp`XFAsl2rcHj$#JI=?.8@>Z%T6&p.CrqM(KP;-;7eC_&BS3?J0^0W$wbapKd?a$#=WvD->K[x%b#U#6"
    "X2',;lCA-ug(F$%JbpT`=-a,MF,F$849kW8_<a$#bni294J#<.]Xj=.h-F#6:/,`%i,Ks7'O:_A?mGg:V5F2CWXsE-FASv$QW>d*k*=qgOdim$iWK9in`0i)`_8%nm8ds%`I:0CatI>d"
    "1KtZgiq%9.k@$2'u=Z<-a?aN&np$)*_.l,4RfTd'Xa*P(3`4(.^'q+M[<4K&s)_^u/eHi$=1@)E3)=2C&bM21;Lhp@[,s<%=wfi'rUkw5uNmG*(umM(-x<2C9L3>d*,#gLH:e<G.3jf4"
    "=W%iL)RFjL7KA+4h4Or9f3eq$s^3Z6A?1a#Rf%&4<7h/)A=:V@iA&7hJ/%Z8GO<h>%9JgL:Z:@%EW8q'0klgLW)HJ(9INh#SDl-$(_mV@V27<]EnT=Yr)Ts-.;k?MFDbWuD0S;-FWo]O"
    "Vq24BUitA#2VYw9*Ams-FMk%MYAg;-s('$.M_C*OTxEB-O=Ji$@xs`#u;XkO,<2Z-Z_h&?q*GE:im<wp]7a-Q1'ZU;R@wT.=(V$#(&b;MYIV=/g,f#64#4jL/HL5%nC]Rjeg9BZMF?##"
    "W@6)%xcsx+%u#gLa$b'4)NP<-v%e*&f0LB=AYGqV;AK?dm3x291Zus06We;#'imL-ufVh$#]^'H46V-MRlS9Ck(nm/ZQddu`#@5$C]c8.PR$##3J;K)Z(TY,I52Z6lHwn(2OXD#B.:jE"
    ")mH*MlO.Pd__QAO&U.Pd&ZZL-N'S%=4M0j(Y-S)4+@;3tSL8j0*IuM(AsvC#X0fX-w=0Z>(6='Gn0Biuq&w<CF%@?90kv9K@CCD.73`-#IYhs-ErVbN3]9&$Jim9.DffG3#]x(d%u7DC"
    "-ITk$u)_^uR$juLTH=4'G#GDOX9SS[M,@D*2alT.?5tM(^c?X$gLMm8A%dAmYg:9&evb-H<S(bH'sCo_NVu%M5OjZ<%U(?[dW/%#:Y8g%U^$Z-LMQm8SVbA#l=_DNFq>pL%6X0;6cJqD"
    "'9(]g?sdNNYr4[$m^B#$MsN1=>pa9B]>sI3Z]JB#=xs`#_uU_?fY-/8FR.ZRoD#f>Ks9B#D6&tuWx_ENfA2G`=a0A=p0jq0/nHg)XusZ-e[kD#3&[J(%a/3%Za3It>(i%M`WaIhB:;Oi"
    "0Gk;07)1G`Fo&&+WN0F,-ZG,%KGhuG7w1Xg-qHg)q%Tp:1nN>dbIxFD9^3>d<5)W-YVj0,AUIO;^P3v%D:bDu)#+`>h*>)4BTEgLMSNn8Hh'gGkD-I%slg;-V)pI@9]j9'JkY9:rLu^]"
    "&8`welP#vG'^bA#j3'Z6<R,Z%WxL<-sr5n>gtW>-Ic8U`N64<-^PWI&]lb,Mn)A@;g^fQaf2+>P1GmDH[C,F@B*o;-g6*i%cL2%&&RC`NJBrP-MnYl$eKUA$cX)+e730@#CQ]58L,^U'"
    "WNt1'7t/kLO]&w'Xp:8CB:hL3P)K4960G)4K#ei$0w`0CNCjT`6*CD3QoQX$m]^GMkx//1(#vJj]<xf$K/gKMwFNM-57Z]%#i]6L5_0a#`lqP8ha3>d.jSgLnr7DCV/Cga;&cr$RB=N9"
    "w8dG*9>,aN_)Bv$Vvf;-wOYQ;aRSA][DqI8B;64aI[]8)I%@<-[ne+%KSHVH4-9e?R$E8.2#E#>V1F2C;?x#/>v_?#`L]?^O1aB]lJUJM4Gk#$?C86Ck[>NDtYHA'asgx=K%R<_ew(x5"
    "4J6X$oj@+4JV/a#$5c4%SQc)<Vx^<h:4NQdZ=SnL$sS9C#*o62i)k=G+gsp':w=Sn2YLMED>6##u[K#$EaDE-UpJ6%n_OF3HWBq`/4jT`LSi=._Aon$w;'dM#gH0FJ8O2C4YB']P[fCM"
    ";`/,MlRm*&XUf5#]9F&#u<p]$s[<B-*:R20EQwY6(Ba1)`IaTB?(i%M<wMV=-(=2C<J6W-mkX/,kkS9C>]xA#)A/3`r^q:@:B5H3Pq(x53V):%C%XD#;.,+@)TU)<(<X2C3bNrLJ6^du"
    "d6+c%I#:t_2pk,$4nx/.$_?iLawtb$ljHgYVJT88gNc^$?,sFr:IV'AVl,;Qm-RW/^*of$a$qMK+&=#-Tu_W-+15OiCT=D<'X(*4NIHg)B<wiLs>=,%8e%;Qi`cs-S1;$<cFNX_rL,gL"
    "t<EJ8?Z0j(tuxfL1=1A&*a?['9WBm8l+=2CLswS@g:QV[3>.6/v$OJ(TZaxFe`'^#Q]BhLsgt$%p<qA#]C?L-83$a$kAPs-2ljKF@#CB#[v(>5r-Ak=8TvY-kt,^#mA#*laHqx;)Z3>d"
    "J'Mi%*VWECbU:,v3lAc&6.[e4`EaiM%&(q9FEg;.sChOjQKn*.lc0-MTC:J=5'Xs83x.qL,Q&;QK#6'$'4ECa8Uai0=sh9.XL3]-sUd;-nHc[$QN0.;kH$XJG+F$8Q@iMOl?3n%W^0K:"
    "&c[Ksl#g%#fZk;-<Ow6%v-P.<]10Z-P9du8)q//1$R)gLIuC[%7#r>G9x1NVihC5%fteE?\?rw],)&^0cQbZV-jpkEIc9u`#ZCrM9%2e#6rHHg)170C-h)^Z$0+=2C_$j;-6>xs:iK(C&"
    "pA7?R0lk0&7+(DNc^9&$f#:#6kEo/)tX]D=XJj5_'M]:&,0e^.BZim$LpU,;p[gHQb%CJ#s,<b$()qm8KFZ9iWj6b@R$E8.bmSPi0+J6MkEaf8#k5`IG0_P'm,/gLPeim$4_n#]t3&v7"
    "J`'^#Y=Us?$d5g)<*'8%)lKHMnFA*Hj<Hpg,dC['[4Yi4;0>Snftho.hIgn$OUkD#36=mANKs+5&t//1j5pZ$7[]T.bJ*-u^p2Y$u(^GM*ZeI-2&G`%CO,+#--W+NEt+D%Bvn^#aXYw5"
    "fInu>#5J'JH_8AeTEt1'XiYa*gkv58#UX2Ct0O$M91&$&RUf5#wEqhLsir`$[Mx8DsGN8A2YdYS5X.PdC`r(LrHH>Sh#k=Gnt-Id=sk;-p#4C&MG@W],*AX-Jax9.:/@k=Sq3XAtL]S#"
    "Fk-L>fn4bR0`7j90tW>dLa4/1&1jj9uJ%a+J[B^=PxoG3=N2_ArtRs-T@oS@bj3>d&-:[9hO/v>]7o%u,EG[$TW:kF)PI<-YWrm$&pg9;B.t`#2P0L>DH3jLjt7DCt-Tt:ekh,M>+#$%"
    "A;U<-u=dH.HL3]-m>Gs-T2WF=K'lA#m]Pd$oOt:C3slM9NCKNEAH*^%C<09.rU72'<*Zf4OFQ>#gj_A=[P^p9$.-)H]B2GYiCP4%@iQOEX2SS[ph](QR^M`$+<<Z$HJH3b5Yd6<7><Q/"
    "mxEHuc(w<CULr;-_pTc$oEs>?btnl&+Xo;-wff3%bKR$9u,A_8^JCp.7q(;QQ4IxeQ*TZ$P5W&#9>ho%:axiF[d[I-(fj/%[#G:.TO-O4Z.[,;[G4EZ&/moLEtE?%IU382Uq)$#d9YS&"
    "$xW,;*62cu+?ZD-<:$L-dXC0%j0$B#.80rT-%5_$s.YJM;N[B&MEtJCj#r/YP9;.Mo_U`NiD]Y%/.U-Mplh_GH:Y)4cr28I2epHQd+d;-h*`X'bg>N0C7YY#0Vf5##L?IMGK=:%:,B+4"
    "s]()nptS9CYJvWB(jkA#W6*/%SO(^8a=2G`tYZY8<+lo7fcC`d$VR0;J[eA,BNv1%#jp[6ZwdLMrLPW%j1EQ/$jPEdQ>f19r^.[KK,iA=X4vG*mVK;KB_Yw5pqu^]tZIM914>)4*b(^="
    "BPnk='d?2CKP$T.S5cQuqDH&%ORJ(MPw=Sn)L.jBf'F#-n4A^$@#LZJ5UkD#qV$Z6]t%b%u&,W-5>e#RKqoZ$=#MX0([ma$dUL=&rLA8#&5>##jSU5%sA<A+oE&?7/0Ed*OqIW-vF2to"
    "erE?%i2iEMtsnu%.8h;-0`de%2D,gLHfDE->1WU%%.Kj4J[kD#T25L#I=Jp.9:k=.0iV@&Kjhu8rfE>ddO<_J]7J&=(8P3`81^gLhl[Q-@CPG-c.PG--R_0%C`f`*6oI_FaK$LGs8P^O"
    "&b&b'48Dv9TL;aO^Fi;]wu*a*Hq<X-A44hEG1S&'cAL9.9Npl&5QPp.XgfX-]ZdHdSD:>d/][5/r%@5$x=Z+:oZkA#0dwlC4=O2(PgdPJ2*_>$NV;Y-GigQ(O3XX>$DTp93w(hLo]nO("
    "KVk/,`GsI36rx(:DqJe$/7-;u_t1P-%t2b$4s?Sn;uq;-hGmX$B(Ws-2eI294=KpTP2#?eTr*.FN=6##tR0^#'`87.&$<fM-dYV-tlL#ndtH6%k3qA#)4=,%<a&dM%,K;%Ur:<-j=9C-"
    "e<OG%Mi$<-oRTs&E'/eFVane-B45<&O14n-?)(+%oPus-i$hdMSk%d),>WD#r#[*7rwGHu5'uZ-R%xNOiM1Ddj5>>dwOA^$p1j2ijA7L%&D=,MNrK%%oOt:CCh3>dGc$9.7DPEd,tTp."
    "2t=Sn%Jl3FlF+F38t@X-Dqj;-JH>d$EC86CeT$Grf5J2KMl#I6w)loLHP0%%On.+-s';8..,?f*[eCgLDlS9CY?a,M$)2m&.]18.A][p.U?1a#U%0SMlL(c$pF/b6t`*vL,]4=:^>Pb%"
    "RpP+<v,F2C%sKH%Jq7o8@]@&,O__v>qm^>$*nVgLZ+h%%ou_?TmvtBCkrwFr*I.ZRVWZd&J.cLY]N=m8$s]G3Y#Us-,d5d;qP%_oPAc`%.;2/1[4q/bh<nq$,lT7)d=2G`#B;_A?#NF3"
    "Hg*P(jWw`Ng[5;d4+V'A3XwKY3LS@/PkOoMh8Go.?Wf5#N,.e%BTGwT+PZhYSnK&$Y3&H3T)ahL1kK&$($rtLn=Q&%B['2hKB@T@kH+_JJ&K0Mkxw$8h._#$gmIO9uFTYA)g.^.aQ3Z6"
    "?:Sn&RcuQ_%UWNM?II*&AGi,;ScmJ;6pr(v386X$f)>m8E9.O=l0@>&AR^#%24lAur[mHd<Jeo7LCT9VpG5l9K[l)4J=Hg)CAg;-)lN*,_ubo$C<I,MA$41^AZ.3KF0L#vrT_n#:aV78"
    "Ss1R<k*e#@[8%^$IOATiFHkpBn:4KjwpNn%PUf5#9(a3r#-5c%a;6W-4qmcZAg+/'8B,W-mFCJb$SVJs%x7DCxrI9rWVE>QpH=t-#FqhL]nHl$8WJ_m^'QJCxkL)-0+J6Ms:4f%(.kFd"
    "SM%TUEJ@2.$Dh%M*rJfLt-gfLY^5[$E=+Ls.Km`-+)_QV:xa'4VO^>>:6F2C$K+,%d*iIh&9hD<ZL4Au`X%*EdF>B$Uhsq%.M>s0-hq%OMq(P-eE6IPj/b^MR'Ls$l'/W-/&3F%r0hB>"
    "E%_>$c*oj(i?*209cO/kP$G[$vdI29.V^$JQ@On-(vI`6M?\?h%ZJoxR<w$=&QJg,M(BGc%3Od;#=OVg=RUC).8em_>*v*Q`:PRJO%%k=G`R@v/)c=Z%459a**A=W?A/)d*LbEW->sn?R"
    "E7+F3]Xj=.XN,<.aK;X6Z_$q$9;u0#M+?a*NFFwGcZ3>dLu69.J].,)i:LmA?%'Z-<'+;KI>tM(FTIl$@F;p7KXQ-dnAogLQJ+Z-*vP'QpD5&#fMh69R3qB[tVLh$(wZ)4wTV8@iJLfY"
    ")PN/%,<:DC[bD<90ETf$:`8,&5/S]ubItH:asBW/b;*M%ov@+4oX%f$Z_Y)4b-k_&Y,/;Ms1]i$ciCj9RRbA#GH4($_u>98htsY-MA66)S`:<-+)dn(]C#I#hiC<E`s0i:H`J'J0XL-m"
    "fQ`du]U)X9[b.E+hV)F3j9F#6[PQb9<`bA#(]Bp@w(Tq^SS4O%nJUQ/MX:;-&a:D3&hCQJjv55NK-H)4B@w;-cRh`$86fp7%ckA##V@b%YoI08WX_KcncT0=Si9wg(gv&=>$sR8t)B+c"
    "G(i%M8e-l$)'Xp7)X3sq$]+$%P`Ls-9u3bF-=LAnv?d]%gG-na7&h;-ukbS&j,?2Cs4;<-frre$#cM>dVJ#?esv;_82XkA#x<2=-?ibl8Y<-$$#[I%#dLkA>2%pG3c0W0G?IHg)PU?S1"
    "ac4c4L@tY-#Sdp7W5O2CeRYUDbL$gL#jD]$x^2gL]^3k<-X]6Eu3AT.KGX&#i=N1A27G-5a<?l.d%G)4beoIB[%Ow9CC6a*556N9uUC_AQ3k;-@DG?%/XdGM6/K->5S9-mn/Up.@Djc)"
    "]E(@K?m(Z65KW.GShkA#SmVE-PFoRM8:Qd$`C%kktJ=a*LkIW-T=m0GhJOdMa^9&$*wRa*7EKdDDL&UD[6?GMs3&Z$8PgIheY(N<S/-f;%Mu3+g*TRE]hHk?o@S49SG^:^07#1%GFi;-"
    ":=wuoB1.AM]w69K]E>[$LBUKE]c>p8TLrs'pxI^$rRx[>7A4K3M<Z)4Iick%)sSa*aG5<-(.08%PfY58c[f'/4BBQ8gL%a+R.Dp7HVBdOpbYV-&FnGDn9lj`$t//1>/QB?ds6UM#h*_$"
    "0QCw*p8'q7][?$$/Od;#:,>>#4U>[..%eX-ZXn'&CEaT'wp//12L*hY#J#gL2x.qLNPkG'*R6r@)s>g)38B*[3=jW-Ue=BeNEuw$x08G`L&h<&G/cwMj$Q;-?/if(YnHeQZPK-QV*-^$"
    "A1Vv=wQX]u<Tj6<g`FO%aK7kO<j=0Jkkv),Z>[u%W8us-5?<jL#NXD#$8eX$8kPZ-hwgA$?7m9''_.F@oOt:C#V*9.aW8G`rQ<p7g6G#[+19gOFST(O.KXD#*Nj-?Auq[6fW'DMP5^du"
    "KDdD-7+*J%e4)'S_'XU%7Od;#R$(,)L8DRqbW?T-E07a$L1Y)4hJu-$#;8I$C_)gL&#>Sn43Jb@Y>#BOG9W$#H*.gL4b*P(Atn;-?$R&%B/Z<-sb>Cd$R<nL$%L`0.`6/1lta1uIg8wJ"
    "URF&#isuN0RhFJ(qEwY6Z[[79_p6bmps>W%>'a^%x08G`3H^,D*;Iw[AcU&&o/[`*_p^#6v-0a#D(w/)B+eX-[2,W-cZ^p9XQt1'33P1K(#OB#-=6<C:rAUBw9q#$hM7%#3F*nLrYt`#"
    "5Ip9VC><,)X@X/%m[e8.$o%2'JrIn&J#CW-pTL?%YFt1B#3k<:K6fX-s9(8M>g*P('A@$Tk]k)c9sfSUo5LH%+G2>Sr:I1KFI/Jd4#o3r5'MHM2inO(J]J&$@r<h>E$.L>Tjgs-4pMBM"
    "q#8DCcE;eYk9aN&8+io.nF-BnYqeb%*@Y'AQXxR8:3t89pOt_$:'b-Zr@%9../B=ll:?p7]/i_Ac7YGMq)$a$^6qfLus)'6vnie.N7V9C<dAq$^X*#AE5Pn%5+_k+uQ))Zq5#&#JG%)Z"
    "4[kD#x;*#9_.(p$e0s;-F&50.4rWH;2.=2CwExP/38C=l7l5/1Qgr>[A^FJ'9<-Y8g&#A#=gUg:i>Y)4V#4jLAM*>NEt^_%0;2hD@p0wg'Sp&mB)4fMew3&+<jNv>6Ng9;O^F68Kf'B#"
    "?`Ek*W_QdM^fP;-GQPm/rnWD#KGWD#+5oq&k*M4B>nd#S^72G`299>d@Mh,=_cBV'd,@Ns26V-MP:[.%?f/c&FUTE#X'9R&Xs#U.sD*hu;IVX->p?IZxLU58+WmK)C.$##VF+F3@h8I$"
    ":Z$W]h3]u'@9Os-umL3Ex?O2CGFGk`Ic:W$6KR#$xbLP/P)SS[^7pu,?:n8.__OF3IOG)'`gx9.e[kD#p5>.>bEJ]@)d3>di8(T.Hl=DCB3dpLRGBw8]=6JrukX30ppB'#d$(,)f8>L#"
    "/2L%6,Jb`>/mP<.Vw(x5CQwY6nO&A>fM69g_&MX%8_KmL`e#Y9dT^3*mqS9C;P*XL_Yfp@-jQLGl<JPFwHqb=<hb]uVH+8NUsVqLqK$'8mxd<C#-Y<-=.%)%9>oI;w)^G3*]#gL48H_F"
    "0NO]u)qwq$$=;e?-k=r:9,5)%SAV2C5JEr9n2#&#sD<;&_gF[BOX<g)S5%JOES&;Q3[;E-AV2E-a>Lx&8Yg?$LMb&#U,>>#HGrDPA=1p7b,>)4e[M)4$<XG';6&tuH]ud$OST-dZSX,M"
    "peP;-`qx7&9:2[.EPY)%^<5)Mv?qKM/p/Rhkin`*N#-0:k#@>Q(^oGM,iTZ$B90K:id@Co/x2<-MMo6%`',/(-$#Q/Hg*P([]kw5N77>dlgl-$(xcIhP<MM%`gmA#97#4iwh)M-^>S`$"
    "fI@j::Rdv$]kY)4g;FgLVRJEd_G``:@lK.;Jl<>dwKgrK%wa7%,l?X-a[ZV-B$p*%risFr-];Y$QlDs-4NP6MIQR%+o,YW&l&g%#2<Zx'2Ps`#'LN*%1V/a#hY6<-T9bb$M&9p@W:xjk"
    ".@lZEqsSq'v%aPD6_YrBY3'Z$g;AIM&_kD#TOG<qxla*7sV=[f5v%'.7G1qLO`(#8gZ3>djGqs-?KNjLtZA+4NoWD#X?Rq7#VYH3h7Y)4HJjD#.>_)5K25%ME@Ad;=UWJMhC/&@bx];0"
    "fKO_&h%$##.vQJ(5nl<%Rt;8.triqLBo:X6Y$x#')gXA<#I4&u1k=DCtJnA#hC/Z$e+G(=[HH?$Jax9._<wiL&OlcMW<Y4/extBCOh>w%Z^Dp*S?=<-1*a2/cB:&$#&Up;8UkD#f$r^o"
    "Q%G)4kUfC##hv1'4gNR%<lG68D6O2CqW>@JrploheEr@Ij]7#%'7B$DP,2($Vk5&#XhFJ(_T/i)HJjD#gC]lJ/BP)4`>nB$JqgG3t%U58:l.NM&*0/1@3x1$q#vl$Yaw[[9L6d*/7'30"
    "p*3G`n5``*KTrKa:I[OC'4Be?SIKL*g)k=G&.(@'?e&K:6)a*INprHC3Yo.CFeCs?XUbA#mNoRAApB#$K-(fD.2=2CD$Eu&uN'['wF_)(s`0i)LifA$cMur$YvKs-h[^1KFV`QjGE+<-"
    "qx8;-%S>s-gKFnGn7+JG1EfX-vl.&4>0wiL2x.qL)<O3.`pQHD)J0hPm_oO9Hb7_8s2[`*c*U-MLXGoh0S`B#Tf%&4nYCou'J,/?.kkA#@#j*%vYnQD,(Ow9v,7X/2N&c$$h-I6R'4T."
    "9:k=.2)-h-wXw<(ac)K:nC6JrB`T4D6fDY$JFU,M1=FPA>h(RW;pMF3A:%oB<t>g)oVP'StGIh5#0S;-hNW2%55c^?SGFk4j7$##?RAZ$()E0E8kOT9J'L^#i>sI3]eX8I0,U-kBP2qL"
    "7&0h=DY@$To7f:dKWvFr4w<8%8abZeYsN+.j>SnLGCh1TOw=SnV?^.-c0vE7RF&H;Rug>$u[Uh$X>D(=?l7p&qRlvKCN;m$7sZh#C#(ijR;l?-M2K$&?&W;.r'NW-sxiKj0:8=.<B>Sn"
    "peP$$WKBD33w//15-P0Yq3n0#4C%;Q)XQ;-`>MfLXQeJ;5>fBJlXj=..Yl)5mkS9CRTV'#gaDs#?Wf5#,C3D<'V4?.-BF:.K9]a*5p&HM^fp.Ck6`99aYWq0e6Sb%J.@>d2wY9%O.7>d"
    "+6a;%.(H]%Zmh$0,`T[%@x^i/&C5n^-r]D$&IS2`3K*>5oY+##&,###G1p0%pjf8.<u6/1F=LZ$,>###lF@K:(B)c<p*f34#0S;-m_Me.6l:$#p3E3&C+XD#7&v;-@sni$Zx>p74RqKG"
    "Ba^MWbjJ%#7ewq$n`0i)V9Fp.4*x9.tA^)Rqwqc$?e;gLdu7DCj'2X%%6Gn<vn,-u01BiuM*<6_PU$39:2UB#FMsFr-a9G`2$B0'UiH9%N1QdF9Z7.Mv:xiL/NuM(6otM(bcsfL6Xg88"
    "w-L^u[)_^uCl5/1cQ_BfY^3a*(Yh&OiDv$%.T=,MU/4Z6`rQ/8H4CVCA(i%MKdGGM$@);Q.f1Pd+ja&?$xSfL(1lr$&If?TC39I#;ekD#A$E8.xkgq^8ZiN8t2ILaFU/Z$=LpetMl#)O"
    "4ht%=C:P)487j5C@<Px7V:^FEpk<>dK;87&0d,W-2?bCF/K$$$MUf5#IBGrB?1OF37`f`*$=`a*f0DjK0QJWpqJWU-ls<_$;G11#?FBDeiF]S#h6cA=D*iP0cBj+u@/0i)b`MD3Kd;[-"
    "DNAX-u6lD#uQ;X6Z^`i%YYW;?v2c3bJJbGMQpT=YQr9a*^PY<-?Nei.Mww%#]IAZ$A/-W-c+.F%d0YD#0Vpx]v=rB#Ic/U`7ZsKc#f9^#HwV'H_72G`%vp;-d(br&SOAm/uB:&$Jax9."
    "NqlZ%l_^V6:<V.&Sudh*Hg.n$jctJjap4KN1iitKSr)$#jjar$]g*P(Wvg%MYF)<dq#.-uk6,R%J)6t-;C3D<*e5g)0=V*1qEwY6^KKu.:=Hg)Gw[:@XH4AuHEet/R];;?:.TV-w`g#7"
    ">EMm$@.as-'ec`*H7Sf*3:?%.;QZ`*`7jrAsO'N(,L+-t-$3d*WS_W-b7<CnU6]8%Wn)b$P$&68IRse++00/1]Fc7u_Mk%MkVFP?p-B+E<QR8/S=I=Acrcj).>vt$Q[kD#5TKN(cQ3Z6"
    "25Ph#xJlW-A;OT'$u7DCK[V:%Y7Hp7;))d*L=^lNj)PT.7xL$#f2Ru%,r)W-Hd?dd?OL?dh-;]$2R8F%Ej5/1p9=SnuCpFCbspMNic]t^t:M*m0ecgLfv`B#H(b'4>bZV->T&;QnXAou"
    "WrCSn?pu9qVx&-u$3mw&l'GaZ=#BUA>f3>dw88a*ren(O#hps7Tw1HG.BF:.XY?#6]5iX%Rt;8.Q)B;-ZFII-CnJG&-[<R%Y1VD?JtKu]5#MD&Xuak1n^B#$7daIDvFX>nj&^%#Fg/0M"
    "cR^=@FYjUIs]aZ6+U1N(iNfP/^;_6;$f^n$.]f&=t8n,ZhkS9CvQj;-F:;m$i*Ma*jQwa->-Xd+:)EB1HdrI3XA+P(H>WD#?c[i$(lA%>Ah3>dC5ihk?iQX$n5FD8(qDf4$rT%%LqAm/"
    "LgfX-'-p[6`j'-%1j4Z6-Tgb%EhTBQ(CGGMaV$&YJ@*?d[iZ=Y$jPEd'`F&#Z1cZ<1iO<BFFMd*h$io.`R.T.k(OF3DetY$t_j=.]Xj=.VH,<.F;QT%`';a4p&;K*p'T9CM2&I#)Y_t]"
    "Cju/Ee6#h8Z^b]u-^muEpe0^#t=58.#KCSn^L5T/bw9hL[PwX$dq(x5lG9I$75we%?FU@%OKV5KHwK=8VHt1'5psj$N&>a*0b)w>JSkxujSo'.)hZ.M[WFo$8he;-cg:D&4`$.P/c?=l"
    "^siD6>@Xf%aieJj$/moLc;N$#CHkL:TXkA#8a9W$1Y<;?ZiYQsBKWB7Y,.#%=O?x%b/1<-uB9C-.DQh-h7eF<XQNIFC=bF3XYFjLam2;=Dfd)GsXAou6K(p#2w@P#U&K0MhJ1h&x.Ya*"
    "#u;W-n3]>eu:q[62^k3&l'@p73$(wemnQ*5nP>VHMT3-v4Rgrg(N0W$5@#-2Ee0a#6IZV-Pf%&48l*F3vM<i%X@vR8c.=2CO;:>dcAx/&0:CB#A#dIh9%N'JdPLtCEX<<-Y'=CnD_0a#"
    "jp@a*9xrW-fBd;0v2beA]47w^7]18.Y'3:.m#0i)u1(p$vNs;-?P[M.,o@Z$nZKZ6S9)I##2qs$)lKHMeq//1COVJt24fQMS+ARMZ'+G%o22pL-7D'#%xF)4(wJW-4'4L#9Hq209,B+4"
    "'wZ)4ig[J=A76;DV1$9KwYIa*kh*<-[f=b$-e_;?<(=2CH3b<-(I/b%'3Lp8CalQWXfd/Xt;Y,%DvPm/Vl*F3JOmO(PWJ(%KZ4W-0)lxetb5o$.?^M#@njr$ZQ&##<34G`tA,^Z*'aB%"
    "uf6DCN=%X$q8V#>;_5s.H-mUB-n<#-8PD#6KW:]$4nU@G]*wY6N=-*R6a](%jvvh%^<5)Mg[afLn1K;-F1o6iG[:6;bXv[/@enBna>Nt%oa[7;bMsI3H#0i)A>^;-wSur$7t3W-JLYn*"
    "5ED2+wp//1IXPZ7mR]S##hv1'0*SgLi&JjLcmaO<b&l,1AhFJ(=5tM(A,[J(14O2Ca3Ue+`=2G`aA1bIrvBBu%w%uK&,q'F-u^>$oR%T.GwgA$Nx,d$OdPg:#0F2Csa%#%Oh.W-T/=r7"
    "vXgY&`eow>45P)40ZgfbKaWR8T7<Z6J9H)49^]GM2L'K-XUV[%k,YN9#JH4C4]o.CFVO;-HJMJ(;l###'vsI3oV;6/*U1N(p<?GMo_v,%ag'v*BbDb*[BMZ>1wBB#eTq]$wIRC@xwSfL"
    "Tf<#&J=fZNNKJEd]vau&^]###,vQJ(%NR>%lXj=.Q>%YK-hma$XRk>>#PF&#n_rJ:5QE-Z5Z)F3]x2Z6(,+`>7=g;.2k^rgiD?e%[ppEILO1gLWMWe$`fv1'6VH,Dro1p/Q#Bx>S_R=H"
    ":`=x>JJO2C<l[gEHrFp7Ej&D76L8j-gfKh3C$`I&Kppp.o6T9C)cO)vlL2G`J5<A+QlRfLuQR*N@ej=.e?.lL*&Tr-YiDvo)N9W$NLuFrBK>;?(r0C&%ld`%P5l;-[^N;&/8L%6b-nQ%"
    "MA-p%-0u(&sE-pIjhbA#)rv&OYt`0Cc`6#6ZcBa*:PRd;of@p&,6pr-bhZ3:K'=2CDGxdm`*4i:Dc#:;sqBK<>l0gL_Os$#rxJ$/,qq`N4=,f*V:`a*V+)<-1buK&c7D,cRE_g%tI$6'"
    "ceLS.,7&>-J&4^%*fm<-5)tV$n=KX?^NO&#o8<PE^*U)&digDRHQNn89_,v]]D4Z6`=tM(QX)P(&fg)P,=U%%=SAY%Ca_m/ksp=GE=(nu%]<P>x62oAvToRA$.eA#D^g8..Nwu,w,g8."
    "NIHg)Rv(N0Auq[6*0x9.OF6eM_;v[$DK(a<?4O2C-n;U)U;(W-w&wmsr-I0upVXS[;w+/(pIXw5'0fX-=%&qLrU]S##(1<dG[ni$4-B=l9x9_8Otn)?2.WDFIobA#AkA#6Y?O(&>L5i:"
    "UTO]uT*)t-UE&b*Joi<-%B5PO?h)QN>c6,4]i>.=WCcOK$G(q'vM8PB)EIad,&?X$G%kD#:=Hg)7w&W$Ufu;-$NTn$fa%I#nipW$^=bH9#-I21l+1l%Iw'=(9PaA=9uG,*9G(gCi@c3)"
    "3_0a#<gk?B/@.lLZ.'k$<QUh>H(r=GVPEEYXQt1'3Av1',66s%lh*T.u7L%6H)+b$^w&Z6mK^_-_]>F>bC:5%X/Z)<K.2oA[+v8/*Z>G2sel:I/w^>$6/iD<7sfRJ]e/q`<Vq-$N=`QA"
    "$jHh,.ggs-VU8MKZF<$6,T/i)>uv9.YJ5F>U/4Z6-;HX$/@.lL/XkD#rjUx$t_RgN7WNvS(C6c*[3fs-mv3h=AnV78hS1_AdRRB&,ioG286fX-'YTV6HXj=.ROmO(#,Cc$f@I?d[(1<d"
    "kQt:Cw%At@hAo%u26b_$@VS]uj2RUMf[o3%/+s^6TL@X$iHi-Yg2/8>J[(pJvDd-.%2D+MAL);?'hT/)qWI6%SQ`LL9[bA#*J=s.4aPg)nV,lL9J-a$n;9KCN7O0P6`NrLkpL]$TE/?@"
    "dZ8%#Ym<^%/Y*?@^B/W$<oWD#*j8>C&&^-9tA(lKstlg+lB^$.GV0AM(Pah?FaP#v;`B#$M9h-MtQ=F3Q&[J(okY)4e^3Z6PK;X6^)ik$K0Y3Ds[Aoum:)I#HSC5$0PK1KR>D2C/)T8@"
    "Q7_W/)W%`4']$##,vQJ(t`MD3>tgXofs,[$(A0N(cxLI$8vsI3TQ4L#jqC-Mp,b]FS@cC-L9x,8H?^5U2/?T%%`dM3K6$-&;YM)FTGfBJWJuf*J:,v7hWZek(`ew'09x9.m(j=.>JWfL"
    "tG$I6sg20C]o,-uuY=@9Pa`.&g.Hs*6iDN9/n]G3s@]U;bg'Q88vsY-dMEmJC04>d^^.gLN>,HMg3=K&gUY`-R3n0#`^$4$A:]jK@GTZ-D*-^$_LAmM;1'^$6L[>#T=2/1$dkg$s#+?R"
    "ddtg$AC86C6=A<--XGl$WMjB-Ul1e$f%2g*+nG*O;?u`b-V<08&oBW?0>D^#9*F#+8+9ZGt4,H3NbZV-E%a5/ojLEd^9_WH'-q#$nw_?#JAI>%U>Lk&VI>A+WG$T.Rf%&4pJY`%R'g20"
    "Jl_;.P6lD#<G-+%`b.<-Bc`&.G-/;Mwu1p$]?D[%K6D_&T/Sk+V]b?KED]W?AFV499l?(#,vQJ(%uw@#PImO(k_Y)4,i$1#5ZeBu`8-;u#:$C#$Fc7ufPB*:H7q#$wN7%#DeAZ$8+Tau"
    "O<H)4MHq:du9wx-3bNrL8Ht1'aNq%uH2^]<q:p&?33DhL:23Y.X0*P(rFCD.QLmO(=;8Au*2GG-t2#>^cPR%+IF7W-FBKFI;%I)4KQAZMAJJEdGj9>d(hL&uOMuFr;_Qt_cUR<-a-%I-"
    "V2+L9x.3Z6OY2;?hXl^ZAgo?Mq&DP-KR=3&GB8a*J?P<-B<bDY3Bx9.BD0N(83lA#/E_78Lnt]uRrrHCe7+<U%r7DCgDwglPFa8.PS&##o?/W-@)gQjEs-b6xL9WM][Flt4N&;QhpuA="
    "2>.&Gu7H<-W&g'/WtYH0]_G)4u9rI3A$,F%KMi=.le<R3C%Bx>Q@O2CsvWq;_C^DQ6o8gLww4A9rr<#-@e]W-MJ=lE)*x9.?`-W-Of^4j&[W3Map(-Mii?=lUGdw'&VG#ne#k=GrG*^#"
    "hllr^G`k/Mb8L%6R^V'8'#-U;HOiG3wl(*%U6&p._]Ol8&66pgc]BK-]l&r%uMC%G0LH9i51Va*9JSh*3>ds-f9rkLvc6g)ihns-A;xfLKWBMBsnN>deK`l*_'qmJkcQM3NtuT:^PO&#"
    "iS6I%.P)k(Z(TY,U_?X-.q7b@6_-L>YuCs-g_-lL<,n.6/'0/1esOEdK2dc%OdmgOmw-2&%G@k=H]8d)IH;#?Ux,@dmp(p#a#HHu#7YY#kSuIUHgCwKv]sx+([kU%:UON0eS7u.:%1N("
    "].Qq74[3>d;mhEcA,sFrgO(gLfc[a$U<L/?7'rS/'Zo:qXFdd$=IHg)A5Hg)]$UfL9'P58Q`Pb>?:^,M/]Ls$@=kh#Ze_gL2+<d*@%`a*oMDQ/c0YD#Ul.&4@mO,D4]$'R+x_5/kxcIh"
    "gNX%FwSLp1_EwR83]W-?MSu;-B&qK-?bvxL*BGGM)*i%M682G`l8Q2Cw=tO`N7qY&a=&[>]E-tf7u%;%v,Q&'pv(.$XwbW%q;wA#^U5o:F='[8TbK-Qr(g`-kMvE@ZS###k'4]-SRx+/"
    "Z0x9.(=_TrkKB$AfnN>de*loL$8sFr,))n8c.%KW4N9f$r9Fg:VC`'8O#Y`%a][^$ivtBCBL/W-(w8?7hqoTC:e$iL1X'N(cfOt92UkD#Bp#5&^<(nu##CBu5[ml$P;ka*.0YA?1CV/)"
    "6`jEepPwl*BjZMMqUQ;-F8mi'ESXV-WV/d)aQ3Z6R)_^uRv4qLE8x3%@TdD<YJm92mb^)4Q,###)Y6G`^S@D*2uOl;/R)m(>E,<.XXb'4R[C7J4%vQ:KZlU<-4ww'K`Um8`h?#6v)CCe"
    "I>P_/XfP;-IOBYo7e0a#u_=_-tNRm%dgZ=Y3BZ=Yu$8W-F^vbN[l?dFoa=Y$,^R<.cxF)4E.^S#NC>B>9qN>d=w%'HH_nVoQ),L:L7,H3SQhA$lUl,MAO&;Q^V=,Mgp,L'<_n`%,PhG3"
    "p-@a$34lWfrn6mL0=L`$/''MuYWWEClS_1&-jFp&Vf?63616g)NIHg)Xgx9.>.OF3H_@e*GK,9.t28G`RX$gL_%].'[Z5RC]4e]-cO/bI]l*F3C_0a#'>jbM;mS9CNSLs--Un%M7c/,M"
    "fWeI-ks>T%a&Pj(d-LhL0JlrJ$=KZ-Bw(x5t66]%'YkgL0mS9CQ1pA#s)S/&9w@9MH:w[-tR#Obel.&40^a$%#6jrrkkS9Cj6S>d652,>39WFN)*)R<gWo@@=l*F3e/mfXZndLM4lGp8"
    "S(k<:kjP*Rq>$2'1geH4t<rp$KTgj9#+#H34t</:#g#<.sG#J*xo%Z-[D3<f'%8DCZ?bH9uap82gm2;=Q`#cE5W06it*WhbWES/&k.$##%vsI3jotG'uQ;X68Gh.X18dX$@1.9pH$_0%"
    "tq-5?+xq[G)2=6D/0Ed*R-],M-%7g)X0fX-e[kD#OM*Z-o_iIlC,sFrGH0W-xXAF%fI:0Co;2/1>4Q4VbxFK3D];Q&V%2W-wAY)YKI3]-'fg/Cj=*Z6-M_WAn*]i*Nd9b*oSm/:Zh[&,"
    "bw9hLW^.d)5af(5u0dx51-g8.hOd;#a.K&8rT+^&5cp:DuJI*&a3PW%n'2^mT#xY6e@k_$PhdV-(1AT%7&Dp.7>[9CURLvnm$T9Ck?x1'jIbb/h=2G`tT;3V_/QJ(#YieNZGuk%]]=,M"
    "3;I]MPQ)nuCCK?d8&Ne$B['2h4$:N'p8g>eaLXMEhw=Sn]a+W-4<'_S/:rkL7DqIXG=Tc&a$o<-/Llb$Ym@j%?Oc]uB$cn*B.JW-b`^Xog1u6`nn6&%,]LS.Ggx_%fRpU@HcI1CK?xD$"
    ",j#v*`(5gLhZ:*:M'=2CVa5d;+kK.;3a;'Q1Rt`#D5MH%St;8.O1cOKGo2voXhx(:=%?Z]bDSS[1M###4NAX-2`*Q/W'rFrk)1<dYsL3CV]fIhu03/1x_@e*^4'n/oxw%#stWD#Vutl8"
    "se?#6_oE?%(j(&=%E<#/3BGO-UC'k._9$,$/Cai%i&)`=m8)d*p]AZ$C9V-Mq-;hL`o1]$/qL]uYHQ]uS-D9.s5edu%;hAOHJ2G`4V###d1Q5V*0fX-/KV;.98Aq`%,>,MhN&;QwcuNO"
    "(Zi05B94<-:N>g$spaG3@r-W-uS0b$xHJEdhNKB?r?W/Np^u%M`+v.#t$tw9]5hG*0H2Z-aF4_JWjBp.v?1a#?vo(mk5Ch$6LN>dOZJ'SK5X5A_uF^A9%OJ%@a_7&5A;<-%Rg_-@h8I$"
    "*28GP+/jt%=g0W->ZiHZ(xcIhA%(gL$daIh(p(;Q;_sS%Wu<f4465O-Q_)>&.+8Q8m->)4`&A4KIB%4=@16a*4Cb)<@h?dF9Z/e*J4^v>*(BU^6]F_.Zf'x5[VSa$RbZV-ie-lL87&F-"
    "GJ>3b,j#v*H-sMKbJIOtQ7]Wo.1P$%A^xAIA(OF3<U3W-?id$e.#%&4b9u`#g_=0*A>?tKfAC[eOo()/Sd$Y%1Od;#N:J)</44K3FD4jLc'IY%Xe=kF[_Y)4xv'oSpC.Jlsc?k*x;>-M"
    "r,G[$$$At@]-UB#[v(>5Z42G`2>s+&+g5X-@4lBAM(j6EcQ3Z6'p5X$*]Q<-r6iP2%F(b:r*eXfjm9-%R&3f<a8kM(%6CW-D`:jV@x3KOG?5HMSN_`.$XWECGa*u*Nvq>GflfQa*N@X/"
    "?(OF3^_;p77xmW_39k-$a02B-#k5`-5[+[^^d`.&1p[u'uqP;-6APP/:avY-:r-r7VRZpKtpKp92jAiF=7F2CR.,d$f#E3`qeP;-pGSsB0#WE-l2YPVr;nL%-BKgC]'9*>6+Un$F=r(?"
    "O&TO91C;G>[iGp8aA`BSx_W2C>g&?@2n-a$ha%I#q3NEE$*dG*b%%p%av+W-ukh>9j*7Q%oo,W-A1#RNbnvcMLxvt$o^B#$X2jFN&k*P(&nsn8U*$Z$pWGkF9;u0#1Idru^lq)..MgsL"
    "+nu9q_6of$lT'^#&0*F5a,QJ(0+M4Bf+wY6q-RmLb[<B-]=Tc$P8W=Y4oDk9iG@*->dD($U$Yk4QF0k.mg*P(Imdi$KLLd=Mag/Hi(a7u]hG<-cjJZ$9/XXSX8To8pl5g)nXwnV1(Yv%"
    "R4Fb&ngO'mtmfB&:>Vb<L&L&FWpmA#vOlo.2t=Sn0>S6i5qp&vD1OF3D6;hLb4L%6kJQ[$)mc;-o/=,%11F2CLZ2<`mnS9C9vjI-DW1>8=ddUR:r;r7ir,g)]j%gL+qr&.]<3jL&#r)&"
    "X7@]@ikS9CS_7&Os?Z>9hYE/2F(CL>vqd;-8<BoL2oHg)RPX$Kg@C:D1b*HDxNAFeCf)v*C6sDNor@Q-')lo$nv@+4Z>n;fs%w.%Y>XDN15;p$xJ5N9J+l3F:(H%>GMF]uo$JF%)];j9"
    "(0)d*]e%N(kx8^$'$(Z6(7(%%qd?s?Pm0B#<PK1K+WE[%,:[58/VgJ)@@.H3>UkD#K4+'=xqI$7<1sY$Ir'gLI/h#_[Ap49%T'N(=<$MNINwq7PngdO?(i%MPB8DCpI%2'7l5/1VrT%+"
    "lqG*nR$f<-;s4]9K-wY6a%O$%)r@X1TVZ0PnBuF&3G29.v$OJ(09>sh^bdM&p#vWBC[k]uXgv1'L+oA=i'S5'2fqG3b3^OT9gkA#5[h[-A*JO)f&k=GflIY%]]f$gWZVwBh7RL'>ZLV%"
    "I>2]-/OZE7^r_1&DfD0>H5YY#b:am$)Np;-PD:w0lqkj*DBi0.'ec`*9a4i<N@S495Ab>nhpJ%##o1d*c%i/CnP6_#=f?X-Tklw5oK2d*Ph8<-6ssj$D6ASU^6:A;tkE>dp#rU@q9[w'"
    "Gu@P#;T3>dTpf^J5C+YH-`$iLo=T;.Q@i;-XBR'OdM&,9H/'Y]Yq=D:2`AUDWs*g&K%g6'TUf5#'?$(#20r]$Eff`*@pKW-p>1b?tw:8.74lpW:=#_$BDjc)f%`a%rIsO<QIOR-*_on$"
    "R(T]u^-KgLxfOECL02^#Wo6C%C3>Snw&'/1_7tQNRk`G3t6f'&e;1g)qKj;-a9Nb$F]>&O9hjn'9JrNtsp^r(eH3jP;GH`F5ZH?$JAO&#&hrW@#M&wo$'6i;9s9B#-^xY6IP=o$M/Qs."
    "R_T;.xrCW-ZOIk=?MsX%uORs-'t(d*d_.<-1Ut_$3N<UrBAfs9pPG&#5O.e*G;V<-Yl[W]BCCD.vl.&4G1Ot:-r/E<Jd3>d7[^#>akZ-Hpe?i+k2#&#[AY`%g*A4COD_6rBs*P(ZYkw5"
    "`:Qgb-V<f%T1@w#(l?T%08n2C]a)I#V'KV-%df4VA;5?IfgA%#D`*A[cDsI3kFhwS#1qb*CF[Q8^'=2CeUW;o&CGGM0M-W-8:p3+we^>$$Z/A%J-[a*Jq9g*K'xW9ZN(lK&NI&#svB#G"
    "Kj*KEa=2G`nn=`Q)X<C9+A`B#Tf%&4D>pV-b3O.;jBSN2(-'+%$6V9C7r0PdB-s]F8[9N([75;-fP-<-$a]J&e0/>Gf3*Z6%5-uQ:xYR]8HRf5I9T+I6H_k*i,MQ8Z=8FI8Od;#JR@%#"
    "t[lc*G)m29F>$$$*0fX-E60I$/t'r%Vu(W-3g'LGs)_^uTQGn*AO.PdK@/q'b8SS[bOPV-VL>3(p`0i)-mun]ASK/)xN<a*eR&39UlE>dC7,w%ER^@8`C#I#J-9;HdJQC@HoqA&/al;-"
    "u;Kc$4MjD#a0YD#X^3b*hd9dM+i+$%-5$n8>MUamLK&Rj>ev.:FWgJ)R9KW-q.'/P0L*N0/j4Z66fv[-emwiLpX%.FW2F2CZ_0*9t/2hbRh6n8h^c6<]QZ`*u];i*[b=t-EmZo@WCi=K"
    "I=w[-T4W63:&ZC&4_@e*YI=Q/<>MGM$A1PdZgk;-`;ukO#xDi-O<C1L9bAg)^k9fFm^:#?-@mbWr4D2OV(A5DNm6XL305k-k#+^dh:AouF7wi$dwXQrqF4b-7R:[%?;2<-$:Rq$@xs`#"
    "F@T*@/1Ss-x,@%He&l42NXj=.u+:p7<o3>dp`@v>=w:=0WK1k$/sI&#_Mta*j:VdMA`0a#hiP<-#PTC-dZjZ':P#gLT]ud$dP$6/thni'/tn(<LqQ#6m<GPAx],6:F_auN'q<MIWdt]u"
    "#$HHuG_&l9m?.KshjJ%#*H?e%?V[/1JH/i)4ImO(m_b'4ePvE&N5s]ubOiTd/oYe?xB*hu1L6`%$1I0u@p(;QOl](%JdgB&qD]j27u>>@K$wY6)&V)&=jT/:7]h?BZ`Um818O2CEWb#S"
    "jL2G`Q8?SnlAgf8o>WVn(Z[oDQtt0(pf_5/Z0x9.;)^b*_n>^=2;ksS'SGs-OmxuG5'CA7VNee-7A_w'/g9QN;'>Snl'n&6dW9Z$fFW:.B[ZV-_&J/&vt#gLuxvl$qi;o8Da8U)hBrZ<"
    "swq[GWR'N(SIZYS,cPW-TJWEe5o;dcuc2a09];w*>1DgL3*JV%EB,I8XLl)4H0nO(6Jkv%3j4c4TiG2C'ZeduWN$1%l+)S%f;&gLT6[C@n#K%#[hFJ(9qv]8dl5d.DQwY6E]'Y&Y1?$@"
    "BcB=S/W0o%*Eqd%FatKGj9gV%l*_`*HsL6/[Kb&#V;p0Mw5L%6fwu/@%^'^#LQ8;'Uc8'f0ptUA`F=1U$$X72g:5k%7PgIh%VF]uvNBNF<BFQd;5?6sb[V6N;j8>CWGN&#Prhs-/-<JM"
    "c9xiL87(-%v_j=.T6KZ-XXTV6J%Xh5I.8Jrn=1?p1KToLu(a7u<p:xKwh^/)atrk^G32R<Gr9x5fPF2CG/$T%#SO]up,>w$SKX>doY+##q[MA&:n]f1B.vu&^cY%&]66GMtI1N(q=.H("
    "EHH^=d4=2C*N26<ijKZ$>`(q7*^P8/?\?X3&#:&T.b>2]-v(TF-[I.m->v#ZAABMm$+C]T%ORJ(Mk(@$A=6O2C`4n0#U$QFWYpBn+^Wd`*=M4>-AOrY-g?J6abW@)$k[u##F$(,)>uv9."
    "D7Wq.Fo0N(4ZCp.7q.Pd5D*YB_2_^=XBWpBekAv.9(V$#+<ph_EUP4ish=5$=a=U$2sM(Z@w=SnBu*mScunHdQ]A>#Y[t%+6W&8/)>uu#1B0[M`:N$##,BP8WuPg))QA+4(t]*4q#G:."
    "7CNEdksp=G<c^GM5DR6%@/o^f$0S;-^#wE7>X1?%2QF:.oDsI3b#pm%bpP2Ck`a'8_aoBd`WH&#&.$_#kSuIUUml:dTu-A-l*)m$g0#huuDrk4GsC2CN)D2Cb%ns-lG#)<''Zg)cQ3Z6"
    "&kt;j5U.Pd^7EWB1m?l*<Evs-Ww%>G5?^;.]RBk.D[kD#dSt&?Td3>dv:5qVY`gZp4(lEI_<7_8=j5/1XZ)eZj^7_8AKXe6gRP>dUvUh#4>h;-_`%f$?0niL#bH##9rC$#VUvf*VEx;-"
    "VWww?I:X2CD$j^M5WY8.'G:;$/U6(#2t=Snj*io.$6BJ1U_?X-HofS:L$)x5/awiLvFF:.CZ`]$K>mA#a+x;%3.5F%(vTa*8+Sm8)3I;q]UY*$owAgL.ULH%m-7gLM6$##KL9k./c=U$"
    "E[*+.k6GPAA#b>d8Pl=G-NL@iROQ##('BN(T0so.$Mm)4p@cS#qUKv*:J&<-4_)t%a34G`Yu8>,Q54a*[aFW-E-=L#wwdLM%^CD3c]8&(n+$L-0rs3M=x8Fe-DOb*pwV<-C41@27C/aF"
    "]'$`IYe'S-5;1%%=R?gL,#.>%CvVEej^CG)@>,edD&RJ(Zax9.+hv]%H&&B#SKGvn1I[m/Q>f19o;2/1atFp7V1L'#(G:;$2h?O$Y;GCbE@h>$*DUk*^i7<-';EtBWk0B#Z_Y)42dZh#"
    "bGcG;%jj7MuB)t%#_B#$'tn%#jWt&#s*9&$5OMp$].OF3Wv7n$;,B+4?(fAG#W^/),rL4ja7^duDHL-Ze(T#$3L1q7>[b]u96QtL4WaIhLlBgL1R+1#>)#H7Tej=.<6fX-XY?#6oG.a*"
    "KV_p7ug2eHAdk-$XN-;FJugXS($uj*?xQa*uY[n8^YO&#FAxn$k85l9,hmTK7]?O@N(gG3YW;a-r6[R]7`>mq'FGGMg>O@85]c6<uQj-$])1G`'ec`*U;:WIDH2hbc+`b2GCJFI$Ew%+"
    "bOPV-oE_EQ7ekD#sQ3Z6#fx^$+Zh$'^$aQjwNLU&(EEk8CA6$$DXWECuKX%+^:%T.pi_S#7Guj$c#c8.KaR;-CUA+vM<Z)4R*,g$HHd'AZ_Y)4R:2=-d:FmAZ^@vn)FGGM$>xi*$U1<-"
    "?&FNNm-g`=AJH+<6.jB#A4f:dZ9V;-ch[k=o`0i)>/K;-2BlOJ<l0B##Dm=Gj66^#WM)E%W*8<-_NXR-S)OWWXFOR-VbnT%r%%^FmmN>dx_@e*j#CgLH:MhLFSt`#hTaG3Nhs8.cxF)4"
    "r,(9.BK>;?/X]Q8J7O2(I0+N(aF.w@I90]:s-=2C.5F^#:@$h*%v$9.e^V;-TB7-tSOqkLnx<:%vST29T]TsL?E;N8V(=2C`INq%FBa@>;s,g)oi+dMB<L%6&lo`*gcvDNS]K%%EKFW-"
    "pR*r['LBJ`pjf;-j4q(%l_OF3YZLnjE49Z-DII1McdYK&MqI)E;JGF@Tr&h%l80Q8]]g/)d-LhL+.;hL*TtfM%WkD#O:7>d>7x-QmM1Dd[^A=l:wtJW(Q+1#cYtk%CJsc)T4+S&'EsI3"
    "2Zlo.kb2/13Sk$KPPWw;UAXA[R`:S%=I7m8x[97D*n$&OA@)p*eZ#30=9W=Y7CNEdOQYt(<g#9%*18G`n5``*U_?X-o&X_%xBSp.NbZV-B9)/Y%daIh$tn=G]B]<-48:x&kKaQj@vQJ("
    "Yrx`*ZM_p7K@c)49,B+4sf<?%k1a2CK=5)MS=8BLEZgO0PR$##%####<2niL`,(XSc/8q*3/igLmIr=/mxLZ-k,',;g7L^#_.kFdmM,E%dfL1*2Od;#/a.^$o9p8p<#0i)c4qJC=aC$^"
    "^S-<.[P:Y_=c=B#)l@e#RvpG&H'O>d9s+A8`MgKM5Y###DtuM(b9u`#'l0B#NbZV-oi9a*J#29.$ErFr9p?a<LYD(=DZT/)G7?xLZrHg)rh.ZT3#VD3Rf%&40T`3X9JeC#s3oB&r+Cr7"
    "^:=2C.&@2C(w7a*n2.<-'j)M-`fbv-Om^$+oc$X-W9k,O?f*F3-n($5p/5W-)Jd3=a,8w%1tHg)T-C0c(R)x5;_G)41j4Z6Je$&IhFcO%tCM*?)Mt2Cksp=GkLm*%xjo/:E#CB#2Vo.C"
    "XfCKN;u<F3B1gv.<6fX-H[(G@PGpkL6)i%M]*E-Gc`3>dwB@p&`#9m_0x(t-(oPfLZ]d^,Gj5/1t,C;?&,###4n@X--(u]uQ8Dcul'eT%AO,+#k6_S-A`;Y$9%1N(A&&hl5Lv]%;NL6M"
    "WSmWKvh<>dbXrh*u[Na*oT(<-QmfV4D]-<.P6lD#J]c8.^LuFrANv*%oD3^#Oc-n8iBF]ulND(=)>_WJ+_uGMHY_7D=F_19QBt1'>3U&'*p<A+;1G?75UkD#6R4)&dYDD3ttS,t6S]F-"
    "/BpV-3OJ0s@1.AMwO,k%eP#<-9]*j$7vY<-*wKrBwPF2Cw@9C-3%4j&AXSJMKuh;-L'>c%ObZV-3X]s-FMk%Mc?CG-#q//1(_pI%^wCgLYR+1#%iuk%Ck:JFU/4Z6Rs_x$hI:0C38C=l"
    "$`?=lHiJ&#k%)t-^-)s@gV4l9_t>g),)7xGmeGs.uj*RU$^aIhegrZ.vVaIh8C=2CjeFt%vCTCX]q$L*:KihLRN+JI8-7g)e^3Z6@f%&4C,:s7*g'B#]).w>#d0^##K<3&rBU&#iZsn%"
    "XLjl&`t-q$pR2N((m:A=I8Fk4Nl)W-Wl+w@>B6'$wiA6s;j8>C.Pr29QXbA#E-0<->9[8D_j*J-dt(o*Ni$66<[.Pd[hX=YH`6t-l?U.RMI(]$+,0IO+bE20wGq,4dpK-%ug_S#wFoKR"
    "*`U`NNKs$#C8kGM9_nO(^9-,M[(Tq$#3',;=E4Auaw,D-0Y?.%4v,T.kxcIhK]]C%LE>d%BJXw5&0fX-YpCwK9@x1'-2O$M^(c(.&W3nKK:$90;w8.EcI0N()`vlLkGJn$XQa'?O#41^"
    "@^iA#8['6/#UZS[_@f+M=)OF3pWoJ:c]3>d1/@2CI<%?$:E`pFfxBB#eh<SnM=n92QGm92-EA#6H)T88FO=&u,jRMd@jK8/Z9V;-<'<VQ@CGGMAbu&#VLbA#.0jiLThwA%9<'F7bfA`A"
    "NNZq)tlIfLG3I*.-E@[>:b,g)%cis-V#4jL8NZ)4AY5g):vC_&C>(W-Oalk;.#/'.,m,:9h^3>dHRe8.kc:DC:4IQ'9f###u$9?.LT/i)uYW#$)HdEn&Uj;-=xsnLKrS9Cah6M3'[JEd"
    "avj=GY:9xT7XEs-3gJb*;KgW-x[<wTgh3[T7pJkj4N&;QiFd;-#9]QFmFY'AXj`RS+WO&#k]Jn.Kww%#UA@^$Ji$*4>[ZV-9e&7LvG6*Mc$/qL<Umh-FK&R_1TgoLbj%f$@*&REq3n0#"
    "xZj-$4Rjj(rgai0u^L9ru@n0>G^X)4LBwms[?@+4l7tA#c@Vt-g_-lL@T-<.u&k*./h:+=fsbZe=qC[%Fj1qLj^xrL/%QmW+=Qo$TUIW-2aHW/dQ&%#D^^4&Cao%F*^,g)B$O/)cMQ9i"
    "Eff`*5D6<-f3ON.p>bG;R]@<%$+TBo%q*JGow9B#.V'^#'F'p(iM1Dd%u#gLwM)7#@FU`.TtuM(N#Sr3=]C=la+gIh%2G>#3,ED#xWf5#5$'k063(B#:[_P$WMW1#*lO&#->Tl9r[-1,"
    "M/###t16G`@iF#$Ln2T&hTP'APtn;-WxS,MK,&n$I6%gLq;I]MC4r*%sm7K`/xQS%A9GJ(lC2N(#;GjLG/gfLN',V/tODf6*qB2Cw$M,MghNB-,tfN-v=G0.eb'-Mj]<j$;=Hg)G^SRf"
    "1k3>d*-5wgwrapuwVWEC<]mHd@[Ld-^;VF7ceJM'6]###BBrHM?>Bi)A(eX-sws^-5%#*-2@^k$F+l>$'msjM%Ghl8i8HpK9O-29M2T;.2l7W-:DE6s,cd+8]w^#$/0O$M5C?>#J-EtC"
    "ZScJ,jOxo#(d6F.e2[4vwKx>-ATR2.DCmhMmFuM(uuW`:3(72hssEU;aACq8oZ2:2ba95&h0F#6#geu/k_b'47l8>CxJQL-MVMA.7H/Jd>*+W-?-uQW_+VN(V*x9._B</MkbZ(?9/F2C"
    "uFPS.$&>uu%eF?-=@ZX$EH=_/'-F2CwQjQsW*G6u$54g$s.rR<ZD###-&[J(S0W8..(a^]'YWj$*Na>$Cwx?9Ef2'MgggC-6sY@.7p&;Q@-x;-<;iP-#ZuD%s>$2')tY=YM'%Z$)D_G3"
    "e(9B#Ej5/1<FP]uOP(S%s'0x9$^tw9X$o%MWpT=Y.OP9`_DuM(u$Uo-LQ2F.ivdA#iE$?Q#UZS[t+>REmLtu5;$UfLjFt1'3Kn>$]/LS-+A?a$Q*D^#&+T@QtAC<.A$(,)).#Z-oAwA#"
    "[fM*I#fXR<'x&OF[HoM(.4J#6<]Qt1RR+3C?)%[#Y4iEM31t88q[lQW9,`$'wdJN(7k3?#E4?&%d`<Au:XFl8F'(T3Zxh]P/Lt1'2vqUI30=2Ck&Gs-V0niL<x(hL5HuM(NI$.MH1V8."
    "9#h8.$<>DC/PNfb$:Y(NWklgLjPqkLpjbk1>T$&4dW0/1n,dduS?fZNhFg^$Y)fGMGhWb$Oj</M.]-lLa<Y0.8'K0MdA8b-'UE0lW7+S3qw@_S>gM*e)G0N(@mTK-=qYpNN-J6Mgr?]-"
    "$_x2DT/eu%t3Gk4U6d>%uKB2C7mW9VYqVBOqkHm8,G[0>S)SU2b=>gLPG,d0-&[J(sQ3Z6n?xR9/._^#&i,Jt%?TQN;L6M9('4m')C*W-f&-=1C/4gLvE2G`Gc/%O$7n]OM*G-;VEg;."
    "D1>b@>._GM;UV$%sr)J%M6Bm/4NAX-p*3Z6RQDp*246gL=[1N<A$K&veId*Oi8ULM+]aIhuIHb9[dl2;$@c(Ni/<$#/gwiLNMx(<C[3>d.2k>$bS6T-(]wm%r/pM(S-Y*R^lG*R.X)dM"
    "b-HF[9)aa-r9Xq2ak:X6[x9LM<4*/8'I1_A<x;sKbY,)/KV;;?UF;4^VLp;.MMRAG__3>d6n)dMK3Y[M:A:@-ckQX.sq(x5HG+o$$A#I#du4qLBrdX.aP(SuLW.b%QFu]8PD(I;hjK#$"
    "0gmEM5#/qL%I:@-fV?iMK]P5Ov`K`$3I?6sUTV#$:@h<C>_?=CEf2'MESN08>3:kb+MqG36C.eQW$-^$Z%#Gra-/[Bbk&j*_+Zp7L=:kbj@dER]?ZcMi(xY6pUHq;Pd+W-72CkbxWB2C"
    "=TEJ`Zpf;-Fa*;MP4I@d^3bU2242S360IwB]sI?$KG/JdCqMBMnH1[MS-F$8t0CWf?=8v$?2Cs-NE1qL3>a7uaP%d#@(iLduK*^O,'QJ(7_OZ6po0PdS>Xw0MVxFr,K%=1F&###Y6D^#"
    "c_-lLp+)mMQFt1'-%V)NRecgL5Tt`#8a]Q:9&QPM:[T@NLh1f9o?EZ6HXTV6O7P2<aug>$uxv'89^cF@(*[^#0Djc)#aX0M%8u8OYcwe-BGoNOXtTfL=A];NwY&d#$bo[9w&h#$MUf5#"
    "VuX-?[1l-?s7`0lMVxFr+[hxuJEf;-j$(S-.1-a$k,b581f0^#:L8$M_sBh$HDpk4;M'gL0p@X-GD6?Ixp//1GJQvIEnT=Y&$tP8(CT9V[c###whg<MSldM9Lg]mCZUAG9CBm&Q^]=^%"
    "oN^_]QK7eM&q'TOlV*_&MX:gLO7V-M^$TfLIvv.%@6wiLYc&k$7PgIhdK_GV.,fN%*B'<-F$ar8=xW-ZI4Zi9g^,g),ckZ7-v$&4G)P6Mr_aIh@INauhdA3Mg5H*&VWf29V(Jn3.X^h#"
    "#.EW-$lBitq8Uh$keDMM_*^fLOu3$%Is/kLmGXk%hR>,M]G9@%XsgG*b:l,Mvm0a#IS/a#^brDlxHJEdhDgOKkjf2D9?VhLtWnO(4.uSMSoYV-9f./17gc5/lgcS#4_3vMQ,N;8nG4K3"
    "6N1h.2_0a#&UU=%m=`J6&D6<-Rj[j%9.P.;5XF$K77GB$-'<b$jSc;-mVWU-+smi$n-vo7R%L^#^Hq:d'T)4)Eh&;OQ##.;M@iRs]#5W-H(N9`bI2<-fJ_5%JhdDNK=Tb%PUf5#8tfN-"
    ">M?.N;*QJ(ebUd-g/VL53*7B#'Y`-#sL'^#7Y531jbP;-n:'58CvsY-EFag)D1eX-atL]$Qe+`MjUa7uEV3>dP+*##4,x)v8*loLk%<$#Hak8.ZTSp._1b;.mi3$%:E1qL[B(%OwT&ns"
    "$r`>dV>?;?eI^'Z;l+G-K[Ld-Z?5%7'@uM(:f2i-Qo=Oj@:I]McIt1'qx8;-YXgX9Cp#0)E@E:.HJ2qLoxN;MWj;;?>w@P#h^sfLLY^WM$3<$#N$(,)c0+T.Ae0a#%VI'M1NYZeZNt1'"
    "G(K0M368rLL4lr*JopGMj>?uu)*M*MqC9s$Y_Y)4,fIs-Kg?k*GgLT.[MR%+*P8ib5;*>5-U=;?a^f;-?:cG-=1Yc-SI,G#]Y(b?BSE-Z-.wgt&/moL;%VdMu6?l$G0fX-o#T)4SbDnE"
    "(sp=G@^:RE?BPEd=]a19F-(3CQe./1C*@29*U3X]I$)x5H2pS8j-=2CI>+?$qaNrL3D%'MVKJR0H$(,)NZ)F3K.U-M.9fX-M5eQMf=&SupFJPuaAmY9'[3>d]e?)N(WZL-f(3u.B+eX-"
    "e@*.HCX;.VIXC_&6`NrLNk&3M3NsFr[/D8#%,>>#5W&L9pl7R3QUkD#;do&]JM^(%Gum8.Q4N1KIJX,MeV$j#[?r]$e:QLM7VnO(8%%H3Is-qLjsT=Y>K]b-3@kWh5N0<-C4SRM.x2$#"
    "hHJH;7m9^#a7SbIi40B#TF1qLYx7DCGDh*IOnaE[kY]d=FsK]=:hC_]K>c;-u$@q$p6j;-2'eMM#b2'%dI:0CUDLEdu(KgL94N$#JnYl$eR$]If,#A#<o-eQW[kD#&30@#B]u5D*sPT%"
    "-%lA#=Dd-.HDD:M+F#I#V_%i*7g9gLIlm##Z3i4BE;G)4`g.n$s*IXJ4_0a#HTTm8,N@&GSRvt:u>$2'f*n=G7G53rSJo3%i&K'd;1uo7pWdURs%1^urMbA#*.T=:(KF&#tdo;-V'b+."
    "*0jFNOD`8.PImO(jZK)+XpHB6w;0W$f.mHk3FF3N&=&#+5-^,M?J?##GvTWfnc(B#dJ*<.DVg/)u,0W-Qdm^7Ix<N.$XWECG,S^#@#/Bdul&$$^Jr@C=9/lKM,k>-O)x?9@+9&$5Y0%%"
    "IQ4lVK'.*.`g2'Mg:8DClFa7unOiTdB#$emo5UWJTf;;?7`###(0V;.^/g;-PveXCDlBwKwBDe#8:xKGn#Me$Gf/a3&,###)XQ;-O*oo%K2w%+ESXV-3J_p$xQ6(>gZ'^#d#3^#VTWb$"
    ",sp=GFrrHCHRhGMvQl%MU2BHu,_GJ8.IF&#cH/<-qvpd%GJsc)uW)x5;sbn%(MdW-7=&L>kjK?nJ<[q7lsfG3,jtO%c,p[6F7ie$q<0-4L2J3%xn>,Mc#7a.5rC$#X.^c0ZH/i)3fSA$"
    "ZWER8kl]G3_CmGD&#Xw9.UdGMucRf$GM_<-R#iL.%$nO(%?TW-_:WpBG[Ld-2x6Qq$aC-%CT/,N(p9h$aLr<-pIOn-0q2<J'XZa$9o0N(n$3Z6oj[*@fp%U:eg'B#r#Aq:8*w<(C2+M?"
    "bpE>d$SS2`T/SS[muFk/[?O&#ZU5g)o.n,4ln`:8Mps)#7bkD#FH*;QkYg;-a`@[=L3O2Cf@6<Aj:Aou7S2b&D7es-J#=2Cm&[#6#n@X-g8NB#14;S$^>#q9e)7AIwm'-%k/@8#`Wp%4"
    "Y9</MN/T9C(fW9Ct&HouA@G-&kKbA#mf3j.3`v[-^,/b-f-m--s)_^uC1Ar%?<&=#:hFJ(T+pA#G^Re-0;'.-RRt`#-q//1n3C&uk,G6uD)WI.^OVS%a&&Z$5AR2CTsn<Lu'.ktKJS>-"
    "SUZL-^274>,O=&uA0hutm$e'8RWbA#>s5?d]@'-u_-NSt&K%f$X#juLNqD#M=#AX-N,.qLFjPl$?tH,MtQ&;QA]>d$EquA#xSG_/hUVfQ9?#&#^4a`#^?qb>(VT/)//dg$uic&#v&U'#"
    "0WH(#@2<)#Pc/*#a=#+#qnl+#+I`,#;$S-#KTF.#xh)Q.:?(B#Hen;-^Y#<-3q,[>h[k]u%kLEd#H,W-Kx4oA*f]q)vL-?$/h?%=733mAE4=x>ViX$KF+G&#'Li8&9(HpK%v>&#ma.-#"
    ":_,3#=PC7#v6t9#&;Bv&2f)<-?s&n$7^,g)+7Yc&r$xL,Hj8a*C7G&bFZvD-oj-mN<n$&OGF;=-7Lm<-]>=b*4mUa*@4:Ea0ZT8%2'oa*iY;HM&_Xg$Te$La%m<^-A`V%'C4ic>:'?-P"
    "`AmD%C7dq7G_?T.:vv(#JPj)#Z+^*#k[P+#%7D,#5h7-#EB+.#Ust.#fMh/#*>Ph8l/Z<%kAY)4TmwiLWaZ)418nJ(Z9k=.cMi=./(;T._.OF3n^R^O3;l0MF%)u&)N;X%C7iID9A5B?"
    "Q3(B[;k^KO=##)%>nL(&ZMS&GKMUS[:4NO9Dh'B#?>NgR<x0`$_&###BA';Q#`;5&pIXw5?EwY6d;:DC3tuFr6Y;W-<5QpgFTx>-C^=Z-j.-)/F,>>#jAHJr$efM*vaT_-WnV:;Ke$HM"
    ":AGGM9@[m%D[>Au<'v&Qe0]F%t],N(/+BtUDYBqV`$/W-E.G+lRaH2(-w8W-UXlZg@`9X6;TxVNBlS9CL=7#PrKtJ-[obkLG].Pd$tn=GGO9DtW_Ma-&I<_S-?^M#wh#gLRJ@3'_DN&#"
    "<;v5/5*XS%9/`r?`5<Z6gRs`#UL,^#XTHn<38R2Cge3IQ^:>nCH`(A&7H%B#,=k63<hxY6Dt6E.<-B=l>oXs?/9q3CFRA;?Lerr$rUkw51j</M+JAouP3+Aua7ENj>,@./;hFJ(YXd--"
    "lH&o(F3C(D#$n--r%>/MOE#I#;@,w.Q8?Sn2^a*.slRfL`bY*$vH*1#<,/b-l[s[G`99Z-mGpkLKVO4%rsx)[8AM-%m[eG*jM&gLD/42BpQSCO'7F2C:.TV-?ui,t_,i9&HW,M7[KLwK"
    "rvYk*-EF:.sI,$%<H+Hk)CGGMwu1W-gZwwpt4+wpbf%&4*R2NNU[-lLhIXD#&A#dM382G`fEP<%nf9dMA/TA-'*q/+?&p(%6RwM9RKbqCCg]$M8DGK(S5YY#2Vo.CjbP;-g3el/7:8N9"
    "(^LZ%rQBC-IZDa--TdEnF[+n&&gnA#(^,EQbbcW%V]o.C_%r;-@lb_.Ip@+4^YQl&CTWe?Z$UfL0O&;Q%Pq$&am2^#u6kZ%>hv9D-Hqj0?L7%#;3@#6Y&lA#nHTw9%Ca8.Lq#Grg2)3C"
    "7,NSt7pJe?P?0etZ>SS[Cd?D*efo?9^<-gLw37g)JH/i)?uq[6+#iD3KTZ[$UK<gLqo?=lBul=G+6*gL34)=-BV2a-E3I.DB'3Osg+($&ia$)*+&+XCsF$##a1L%6$cCP/mX5H3J*hfL"
    "86)B#d9u`#>HF:.<#G:.cVjfL7fcgL=#V3CFE^r*vY(39x,jEIN8V2C5gQ_Q#xSfL)D#I#3J#d$Z]6<-amCx$P55&S(*x9.tf6Z6waLhL3JsI3#07`G%`Th5NF7W-QJ+Xh8.l$BS%ka%"
    ",BxeDHE-$$vH*1#I64f*i2cX->-M$B^kY)4Csb([CG>f*[oAW-/T$@fF:I]M4s7DCHwX3FPY0_EP$&oSqrq%.dLchS$rJfL:K2G`+3%#,t2LFach%njk^UhGSAG`%KU1N(n_TIVw<Kt:"
    "^98W-8_&<8D3,2$xVZ[$j3.lBP-tY-t3gE&5Tw>$UFgBfgDt2;-gCD3Vl*F3:u-lX=(i;-;=2Bq?kFrLEZ`X-UJFhGRdgI;W@#1OOWUX%Rn.W-OnAk=ue-lLrHA+4b_Y1%YI;o8B5h'@"
    "to`t%'s_>G'qhP00APu&_<MgL%kKcGJVW-?THh_G)Q)c<=W(C&VgaGM5>?E&<*2GMcK>e#,Xf29H=nXHr-^*@]BsZ^[<l8I4U,qr;M_m8p4M4BhjHdb,i=j$<3$DecAdJ(8=Hg)rD8n$"
    "ew(x55=$f=F+F$8&)EVnu8xcMWOZY#kSuIU+A_VngPU7)Qg@#6aq8Z.v?1a#9[5[.Rm`'4^?:U9^c<>d=so(Na/I1KI*F>d3QV9r:LI`%]Yo.Cp*3G`O4b`*_]@f*=u6<-c(#o$,o?X-"
    "<XYw5JWV%%gHDHu]6&tutf4s1$UY%+38C=l(fW9CVxIfLF.Xj.-Mc##YgJ?0bEX&##$nO(xQj-$@$/i)&<5p82F5s.=SDs-e?.lLJt.PdUx5Pd[47DCBL#GrYfo$'IKk%MlSAoua>(nu"
    "$1BiuKgMY$Yic;-HA^2`:$>Snd$$>FmoM*&lHBn)])QJ(gSvw*sXTW-2JDtUx^W1#Q5Y%#Kea1#S$V?#,PsD#^LEL#o>]P#U=[S#*0F]#%5Uf#=U3p#>mT#$?(w,$1JU3$OOCV$.7>B$"
    ".?MK$=uXq$0+5^$Ofdc$9=*l$`h$8%L[1$%&q(,%i8;O%h[f=%jq]E%A$BM%DS2W%E@s`%e%Lf%xGVk%ntQ4&<[Lv%rWu'&UTG/&4096&Z)b=&HpwD&l;VK&H;)S&uY^Y&RV0b&p`@h&"
    "]R+q&Kexx&w=?('bY5)M)s8=umhJ<?/o%*a)w6b*-#uaE`_`aj+/u#uocT0:.oB*N`G*td#5H[#L:tB47tg<?5[pZcRc7[>nLlNg&Zm*3_-2[Y7'mh(M:.CFIh]*aGdVI)9TenA32u6]"
    "I8XC+h&27J:8<Un],>1:%-W[YsYg$u#7P=?#?xtZ?'-P'jDeOKFi87oc>Y1:o-C+W;e^o&/CT%>s-lIV9FOV%IGg1Cn@:of$%r%,v,$>H[HY+aQvri(G)boAS$,J`xm'p/>Iv+NJ8`%l"
    "g>YP9qe+2U5l.^#W4VJDj5@Dbn[Q&,&dI8Jx.B>dpU$E+$QrVIV7^Vn3OP&>M73p]pbm&,(Em>Hx`eDb$+w&,d(UpAS1.W[ArD&um9Zd3P6JdNoBDpoEH7?\?GpNdWun=^lH/%q/,_JKD"
    "K&XdW-Z0Kr/]wj:59adW;/:Kr-ppW7/L:3UQ-Uw$4,oWIDqwpfekY-3#5[?QGxMEtr9X-ET2D-jw2'@6U7*X[Kx)esI$Qe<=wh3UGl2es52QX71xAXR;#CkqKhKL;;XcqSKxDeslX74C"
    "<(2@d_k-f3)mleN]W4:f]('S0ms]LMREtqfq9V.3][U:JgPvkhJ<0G+,.ErAtn%`YUAK:oB%hr&C8C`5DwNrAC@k@QNloL`IZ<xmVNXf*U%.A6RJ*GF8*PxZf3G(uYHv.<p<p:](^3#%"
    "lghl:GL0AQ1+GfjuRF;/oTNSKZ-/Ad@7V5(faZM;YQ;;SIA$5qbH-m:Yv<5Ut>W#%<koSB4N`S^Dhj/*N>jGFg8dSgR<t/*.r2Z@&%J)Yj3YGtOD1H43)`/NmW/acD7VT'LQu5C<Nm;]"
    "2&0$%24/<A:TVs]:JpZ%(TNN;xnn)Y^fUsoOu.$7I.FHOlwo/sn<P<8[K`ZRKbGNi##N*,9;@0N?n)*lQVQ*57N9tJgR(n_BJ0C$j$5[7,C'bYbATBm3.C[.q[Y*GTfX6_X6kn(Tjq6C"
    "DhAhW(LpNr0/Yh<nDXtSfA9bl7xFC-_WT[@4+s0WS0i0j^=JC69$ACHXM/=]0e5OrdCsb5[q,+PlFEUp$</o:$j'uSWHehjKQ]71=Ht[IG1^Ug?:uC-c&DuA:oQ7UlSpbl<lZ+>a4U7_"
    "/f9D-=H9]IKU:oh1;ji*ejG]@V#wCZFvnItPk8D?%wb+cit;&%JfPP;fF/8LtRWuoDxQJ=ZGS][iN'W'sV.vA5j?Dd.Z4p:s982a+Nq8(%&xVB5^qccA.l,,n^.dPDpWJtP.i2<P6:jW"
    "w/69(wV%?A1->ja^8:91MTOdGpxQ&n$V333FU=QVFLO,iOp`*B81..B$QmLB.-+7BwCN:BrfX?B9WDEBUv,hBd-`PB^NiXBq2m`B`P).Ch9%mBx'exBtbh)CZae2CYMO;C-KMACGETcC"
    ".vgOC&AqWC^Uh`CJ$rhCWrBsCW0e&DkG00DMB.6DBLj:DnU$ADF$.IDu6PODq>`XDWu@'E_t4kDr4-pD#'DtDd=hwDl[OCE]*P0EqIX;EiSiAERb$EEDi`IErLdPEA>OVE:<#[E>=L`E"
    "8,ddE1hhhE$AZlEo&`pEl$3uEpCF?F*jG)F;Nw.FTKu4FYIH9FdunYFn8^CF#[hHF.(sMF;V9SFH#DXFVTa^FahXcFdr>hFi27mFn<sqFxtT<GD7o'Gd4m-G/J94GSiwUG&8MAGKMpGG"
    "soNNG<mLTGVa8ZGmsZaG4X4gGMIvlGjh^8H0P.#H[42*HI$s2H(h;UHaN6AH:K_HHha+OHP;GWHY']bHi>*fHWb`iHB#.mH/FdpHwxUtH`ZENIIMX_Iw1^cInmbgI*hi2J.]wqIAMcwI"
    "4e0%J/%)*JVRp0J#`*7JML@>JY#1HJNx-QJ(WZ]J`Zr.Kqr4sJ#%D&K^*2IKJBv5K0>s>KxbV8#-tw5'rkC>#6%Ke$,<,F%/1L^#4=35&=_h#$JTGJ(.+_>$W0mS/3LHZ$bZXc20=?v$"
    "q[3v6<T0j(ua$3:'5>>#o=<X(VE0A=01L^#B=PfC1:h#$ol#;H2C-?$-]%GV3LHZ$G#Fl]0=?v$nRS:fh4v1F@-HAJ@AwRC#*N=B+V?1<Vl-)OdhEA-<9RL2=`$_>->hoDRB*g2<b/FO"
    "h+J/22^7JCXUh%FPC-AFi)u4Ji_)d3&qpMCp5(@'i,w1B2^Eg;l(q(<Z]-^5ZB0Z1hPT,3es(L5'J_/-*]'@BERl?BCXLwBcJ>_/wU7X1N5l-$Ur4^#vC58.ac$)3oAHpK:LuDNK>@4$"
    "'->>#(8P>#,Gc>#0Su>#4`1?#8lC?#<xU?#@.i?#D:%@#HF7@#LRI@#P_[@#Tkn@#Xw*A#]-=A#a9OA#eEbA#iQtA#m^0B#qjBB#uvTB##-hB#'9$C#+E6C#/QHC#3^ZC#7jmC#;v)D#"
    "?,<D#C8ND#GDaD#KPsD#O]/E#SiAE#WuSE#[+gE#`7#F#dC5F#hOGF#l[YF#phlF#tt(G#x*;G#&7MG#*C`G#.OrG#2[.H#6h@H#:tRH#>*fH#B6xH#FB4I#JNFI#NZXI#RgkI#Vs'J#"
    "Z):J#_5LJ#cA_J#gMqJ#kY-K#of?K#srQK#w(eK#%5wK#)A3L#-MEL#1YWL#5fjL#9r&M#=(9M#A4KM#E@^M#ILpM#MX,N#Qe>N#UqPN#Y'dN#^3vN#b?2O#fKDO#jWVO#ndiO#rp%P#"
    "v&8P#$3JP#(?]P#,KoP#0W+Q#4d=Q#8pOQ#<&cQ#@2uQ#D>1R#HJCR#LVUR#PchR#To$S#X%7S#]1IS#a=[S#eInS#iU*T#mb<T#qnNT#u$bT##1tT#'=0U#+IBU#/UTU#3bgU#7n#V#"
    ";$6V#?0HV#C<ZV#GHmV#KT)W#Oa;W#SmMW#W#aW#[/sW#`;/X#dGAX#hSSX#l`fX#plxX#tx4Y#x.GY#'>YY#*GlY#.S(Z#2`:Z#6lLZ#:x_Z#>.rZ#B:.[#FF@[#JRR[#N_e[#Rkw[#"
    "Vw3]#Z-F]#_9X]#cEk]#gQ'^#k^9^#ojK^#sv^^#w,q^#%9-_#)E?_#-QQ_#1^d_#5jv_#9v2`#=,E`#A8W`#EDj`#IP&a#M]8a#QiJa#Uu]a#Y+pa#^7,b#bC>b#fOPb#j[cb#nhub#"
    "rt1c#v*Dc#$7Vc#(Cic#,O%d#0[7d#4hId#8t[d#<*od#@6+e#DB=e#HNOe#LZbe#Pgte#Ts0f#X)Cf#]5Uf#aAhf#eM$g#iY6g#mfHg#qrZg#u(ng##5*h#'A<h#+MNh#/Yah#3fsh#"
    "7r/i#;(Bi#?4Ti#C@gi#GL#j#KX5j#OeGj#SqYj#W'mj#[3)k#`?;k#dKMk#hW`k#ldrk#pp.l#t&Al#x2Sl#&?fl#*Kxl#.W4m#2dFm#6pXm#:&lm#>2(n#B>:n#FJLn#JV_n#Ncqn#"
    "Ro-o#V%@o#Z1Ro#_=eo#cIwo#gU3p#kbEp#onWp#s$kp#w0'q#%=9q#)IKq#-U^q#1bpq#5n,r#9$?r#=0Qr#A<dr#EHvr#IT2s#MaDs#QmVs#U#js#Y/&t#^;8t#bGJt#fS]t#j`ot#"
    "nl+u#rx=u#v.Pu#$;cu#)Juu#,S1v#0`Cv#4lUv#8xhv#<.%w#@:7w#DFIw#HR[w#L_nw#Pk*x#Tw<x#X-Ox#]9bx#aEtx#eQ0#$i^B#$mjT#$qvg#$u,$$$#96$$'EH$$+QZ$$/^m$$"
    "3j)%$7v;%$;,N%$?8a%$CDs%$GP/&$K]A&$OiS&$Suf&$W+#'$[75'$`CG'$dOY'$h[l'$lh(($pt:($t*M($x6`($&Cr($*O.)$.[@)$2hR)$6te)$:*x)$>64*$BBF*$FNX*$JZk*$"
    "Ng'+$Rs9+$V)L+$Z5_+$_Aq+$cM-,$gY?,$kfQ,$ord,$s(w,$w43-$%AE-$)MW-$-Yj-$1f&.$5r8.$9(K.$=4^.$A@p.$EL,/$IX>/$MeP/$Qqc/$U'v/$Y320$^?D0$bKV0$fWi0$"
    "jd%1$np71$r&J1$v2]1$$?o1$(K+2$,W=2$0dO2$4pb2$8&u2$<213$@>C3$DJU3$HVh3$Lc$4$Po64$T%I4$X1[4$]=n4$aI*5$eU<5$ibN5$mna5$q$t5$u006$#=B6$'IT6$+Ug6$"
    "/b#7$3n57$7$H7$;0Z7$?<m7$CH)8$GT;8$KaM8$Om`8$S#s8$W//9$[;A9$`GS9$dSf9$h`x9$ll4:$pxF:$t.Y:$x:l:$&G(;$+V:;$.`L;$2l_;$6xq;$:..<$>:@<$BFR<$FRe<$"
    "J_w<$Nk3=$RwE=$V-X=$Z9k=$_E'>$cQ9>$g^K>$kj^>$ovp>$s,-?$w8?\?$%EQ?$)Qd?$-^v?$1j2@$5vD@$9,W@$=8j@$AD&A$EP8A$I]JA$Mi]A$QuoA$U+,B$Y7>B$^CPB$bOcB$"
    "f[uB$jh1C$ntCC$r*VC$v6iC$$C%D$(O7D$,[ID$r+xEF#6hC%KGRR2<Cn-NXn).NYt2.NZ$<.N[*E.N]0N.N^6W.N_<a.N`Bj.Nhs]/Ni#g/Nj)p/Nk/#0Nl5,0Nl2pjMYmdL2Wa&g2"
    "b4hwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK.@RhMw'3i2b4hwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK"
    "(giwK(giwK/CRhMx-<i2b4hwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK0FRhM#4Ei2b4hwK*miwK*miwK*miwK*miwK*miwK*miwK*miwK"
    "*miwK*miwK*miwK*miwK*miwK*miwK1IRhM$:Ni2b4hwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK;hRhM&wSj2YA8FI-E:FI-E:FI-E:FI-E:FI"
    "-E:FI-E:FI-E:FI-E:FI-E:FI-E:FI-E:FI.KCFIZ+]88rX:d-$vQF%6fN886fN886fN886fN886fN886fN886fN886fN886fN886fN886fN886fN886fN888usS8rX:d-%vQF%7ojS8"
    "7ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS89(9p8rX:d-&vQF%8x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p8"
    ":1T59rX:d-'vQF%9+K599+K599+K599+K599+K599+K599+K599+K599+K599+K599+K599+K599+K59;:pP9rX:d-(vQF%:4gP9:4gP9:4gP9:4gP9:4gP9:4gP9:4gP9:4gP9:4gP9"
    ":4gP9:4gP9:4gP9:4gP9%'4g2?Fwq$-?eh2$'4g2$4.e-4T`wK+7RhM%cQL2%'4g2$4.e-4T`wK+7RhM%cQL2%'4g2$4.e-4T`wK+7RhM%cQL2%'4g2$4.e-5ZiwK+7RhMtkmh2b4hwK"
    "%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK,NTs-=@[hM&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N"
    "&p).N<V<7%0Q*i2&3bG3hZ;Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g23^9W-9giwKSAY-v2Tt;-CU<7%^n8Ks"
    "Z/je3=3@Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9te/g2O0sZ9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%"
    "^n8Ksl$SR9s_&g22Tt;-CU<7%2eN.N<V<7%^n8Ks]AJF4[o(N9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ks^Jfb4hZ;Ksl$SR9s_&g2"
    "2Tt;-3JU5%M20?nl$SR9c_&g2x#t;-3JU5%M20?nl$SR9de/g2hjf(%(qvJao*&;8_G:&5q4`A5pl1e$-viwKo7n>[_Bj.N[vf(%(;SdFje`U8@gA,3Qdo;-dmf(%x^vJai[D:8;hA,3"
    "3cq;-e$22%grd>[hR)u7+hA,32cq;-IWWU-&.iH-bX>1%&wDKam*]R9;hA,3e>p;-w+M*%f^_&vlw@79&iA,31Tt;-CR<7%^k/Kslt.r8eq]G3v#t;-3DU5%M,t>nXmdL2KSc;-PGW/%"
    "4M2Wfa`=(57p0W-%X:&vhX2u7hufG3Mqjr$HnAV8'ufG3Ntjr$HnAV8'ufG3FCwq$u>h2DiX2u79p]G3O>Hm/s6L=%KGRR2C0*q-BPSdFaiXC5s8YA5s8YA5E]ZA5s8YA5s8YA5s8YA5"
    "s8YA5Rg+W-8-:FIJUr;-^I>1%Gbju7dn]G3,<t;-?+I6%QDB?npBF4:eq]G3$$t;-7PU5%QDB?npBF4:eq]G3$$t;-7PU5%QDB?npBF4:eq]G3$$t;-7PU5%?W^/N&QU5%QDB?nkeM:8"
    "GEf;-7PU5%QDB?npBF4:eq]G3$$t;-lDY)%QDB?npBF4:eq]G3$$t;-qb+w$5WMW/jbM:8No]G3$$t;-7PU5%QDB?npBF4:fwfG3`BS<:eq]G3$$t;-7PU5%vDq2DlniU8*;Z/:eq]G3"
    "$$t;-7PU5%QDB?npBF4:eq]G3$$t;-7PU5%0s''do9+o9E$#d3iVp;-'P@+%Aj,KNkhV:8[),d3Dnjr$?.nt7'xxc3$;6W-?G9RN?.nt7((,d3Dnjr$?.nt7'xxc3Vm4W-t?:&vo?4o9"
    "C),d3[F#l$?.nt7d](e3VV)WH`Yce3iuQF%<,(W-JnRpKY&3i2%w*g2%w*g2aA,W-9^iwK7mt;-K'08%fWL&vZ&3i2aA,W-9^iwKM6B?7Y&3i2%w*g25=%eFp?4o9B##d3Udo;-j#g(%"
    ".YxdFp?4o9B##d3Udo;-j#g(%.YxdFlqrU8s##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3=%r;-QGW/%.YxdFp?4o9C),d3wxt19E$#d3Udo;-j#g(%.YxdF"
    "p?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9C),d3trL*%Udo;-j#g(%.YxdFp?4o9B##d3"
    "Udo;-j#g(%.YxdFn-S79%$#d3Udo;-j#g(%.YxdFn-S79=$#d3NUr;-j#g(%/jk&dp?4o9B##d3Udo;-j#g(%.YxdFm$8r8(%#d38mt;-L*08%In/;8B)T&vm$8r8(%#d39v9W-R0SpK"
    "t)08%g^U&vm$8r8(%#d38mt;-L*08%g^U&vm$8r8n$#d3)E9W-v=;pplN<7%_k&Ksm$8r8v$#d3fHs;-$9o3%.db&dm$8r8E$#d3Unr;-j-22%.db&dm$8r8E$#d3E=r;-YxJ0%t'Yp^"
    "m$8r85$#d3E=r;-YxJ0%o8I'vumgL;)+,d3/(9X;(%#d3@mt;-TB08%o8I'vn'8r8kCvM9(%#d3:mt;-TB08%qVj?%p?4o9/xxc3B)l;-TB08%UdYW8thF59U[BqpumgL;n$#d30<t;-"
    "D7I6%_R@qpumgL;n$#d30<t;-D7I6%_R@qpumgL;n$#d30<t;-D7I6%_R@qpumgL;o*,d33&M*%:GjwKa4m;-vtF<%MdfpK[+c4%Tk[E5x':oD''o2V6+pGM/u779KN%gLlA^P9luZw'"
    "<h4:2&5-W-w?QWSkt779,d.F%*>L,3-+of$AL/79A.,F%l$1F%9d*F7V&#d3OBLkFaPYD4fYu`4_J:&523VR<o<#x',@n-NWhv-N(<QD-R<QD-R<QD-R<QD-R<QD-R<QD-R<QD-R<QD-"
    "S?QD-l?TOM1jZL2B-f;-gOK0%Sa&g2e5-'?@nC*NgFwq$@Cjq8L/,F%=`e&d@nC*N6J#l$p-G59rUJe4VgZL2ODxfLtjv^8<cA,3tQ2gLk)TNM*iv-Nnjv-N9s779;kJ,3?Cwq$'Pp9V"
    "Z[sJs?e(eM?aVU8]xZw'C;c;-gsJc$En44:5iJ,3o[fZ$sFDF7kO#gL;=/W86vZw'+h-na4ZF,3-C*W-l>7q9HxOp7vTZHdoS'g2.M'gLs`Cq7,cA,37qn;-Xu/k$Db]R91.,F%Y[xpK"
    "@nC*N4[bg$JBUL;V-,F%MMG,36C$gLD7ev:K#^,3<.7LM-;+o95iJ,3],6X$@[93i(M#<-LqE884xZw'KA:j9thA,3@v3gLtxlm9^%[w'`t6q9[>8+4las;-+db4%0c+eFX-7r7`(>)4"
    "Bt$gL60Wb$f@pK<?e(eMnOX4:_vZw'Uw@)4Bt$gLib-q$rIf-d2Qnh2W+D)4/Xn;-3>U5%M&b>nln`:8g->)4Url;-3>U5%M&b>nln`:8g->)4u,9W-:^iwK&2JdFZ,<i2W-/WSkeDu7"
    "l,>)4u'n;-4E[#%21$?IrNX4:viJ,3RQ#9%Bt$gL4v5X$4GvVfrNX4:gi4R*AOwK<@nC*N?^'`$fLhwKBt$gLQoXl8+,>)46[*W--_JWArNX4:_vZw'tsj;-J]'`$fLhwKBt$gLtI'`$"
    "Cs5KNrNX4:gi4R*QFh&QrNX4:gi4R*#,h;-J]'`$I0Y4:V-,F%2SZ/:_vZw'=v^/:_vZw'`8g;-J]'`$a:,3Dkn`:86->)4Bt$gLQN`m$f@pK<[>8+4xh#gLX+09:>vZw'+J`32@nC*N"
    "TFY[$E+V_Axh#gL.[)'%xh#gL60iF:>vZw'hGY/:>vZw'cp&Q8#,>)48M-W-$?04V8=P)NKc)'%h>04VX-7r7x[.4VPRCq7x[.4V@nC*NLVUh$V;@WAp3AV8e5P)4ANO&dp<]r8@.G)4"
    "B=r3<-iJ,3^gBW$E[/:DUDxfLn3Vn;vw],3GORhMJG:p$5EMR<0?*gL@kb5:<xZw'_.:3i@nC*NE=Sn$8bRkXvd)gL,K^s$oK/W-fJB?n8=P)N5?q-%ajE3V[>8+45M'gLL,%v8+,>)4"
    "X](W-,<I'v8=P)NA$Go$Hl3dX3BWb$I0Y4:8jJ,3Igd.%g?^Ks?e(eM3vi:8e5YD4iVp;-aI>1%GXNu7q1YD42cq;-LG4_$S2W+=6-,F%wFoP9d40k$%@;*>l/;3Vk'fn9$BpK2b78(#"
    "$,Guu(SUV$D.W&*E%###";


static void ImGui_ImplDX11_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();

    // ImFontConfig font_config;
    // font_config.OversampleH = 1; //or 2 is the same
    // font_config.OversampleV = 1;
    // font_config.PixelSnapH = 1;
    // io.Fonts->AddFontFromFileTTF("DroidSans.ttf", 13.0f, &font_config, io.Fonts->GetGlyphRangesCyrillic());
    // io.Fonts->AddFontFromFileTTF("ProggyClean.ttf", 13.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());

    // io.Fonts->AddFontDefault();

    ImFontConfig font_config;
    font_config.GlyphOffset.x = +1;
    font_config.GlyphOffset.y = -1;
    strcpy(font_config.Name, "Terminuz14.ttf, 14px");
    auto f = io.Fonts->AddFontFromMemoryCompressedBase85TTF(Terminuz14_compressed_data_base85, 14.0f, &font_config, io.Fonts->GetGlyphRangesCyrillic());

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

        ID3D11Texture2D* pTexture = nullptr;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = pixels;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

        //ID3DBlob* saved = 0;
        //HRESULT hr = D3DX11SaveTextureToMemory(HW.pContext, pTexture, D3DX11_IFF_DDS, &saved, 0);
        //
        //if (hr == D3D_OK)
        //{
        //    IWriter* fs = FS.w_open(fsgame::screenshots, "Terminuz14.dds");
        //    if (fs)
        //    {
        //        fs->w(saved->GetBufferPointer(), (u32)saved->GetBufferSize());
        //        FS.w_close(fs);
        //    }
        //}
        //_RELEASE(saved);

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
    RCache.ApplyRTandZB();

    ID3D11DeviceContext* ctx = g_pd3dDeviceContext;

    // Create and grow vertex/index buffers if needed
    if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (g_pVB)
        {
            g_pVB->Release();
            g_pVB = nullptr;
        }
        g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = g_VertexBufferSize * sizeof(ImDrawVert);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        if (g_pd3dDevice->CreateBuffer(&desc, nullptr, &g_pVB) < 0)
            return;
    }
    if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (g_pIB)
        {
            g_pIB->Release();
            g_pIB = nullptr;
        }
        g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = g_IndexBufferSize * sizeof(ImDrawIdx);
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (g_pd3dDevice->CreateBuffer(&desc, nullptr, &g_pIB) < 0)
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
    ctx->VSSetShader(g_pVertexShader, nullptr, 0);
    ctx->VSSetConstantBuffers(0, 1, &g_pVertexConstantBuffer);
    ctx->PSSetShader(g_pPixelShader, nullptr, 0);
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
        constexpr const char* vertexShader =
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

        D3DCompile(vertexShader, strlen(vertexShader), nullptr, nullptr, nullptr, "main", "vs_4_0", 0, 0, &g_pVertexShaderBlob, nullptr);
        if (g_pVertexShaderBlob ==
        nullptr) // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
            return false;
        if (g_pd3dDevice->CreateVertexShader((DWORD*)g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize(), nullptr, &g_pVertexShader) != S_OK)
            return false;

        // Create the input layout
        const D3D11_INPUT_ELEMENT_DESC local_layout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((ImDrawVert*)nullptr)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((ImDrawVert*)nullptr)->uv), D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)nullptr)->col), D3D11_INPUT_PER_VERTEX_DATA, 0},
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
            g_pd3dDevice->CreateBuffer(&desc, nullptr, &g_pVertexConstantBuffer);
        }
    }

    // Create the pixel shader
    {
        constexpr const char* pixelShader =
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

        D3DCompile(pixelShader, strlen(pixelShader), nullptr, nullptr, nullptr, "main", "ps_4_0", 0, 0, &g_pPixelShaderBlob, nullptr);
        if (g_pPixelShaderBlob ==
        nullptr) // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
            return false;
        if (g_pd3dDevice->CreatePixelShader((DWORD*)g_pPixelShaderBlob->GetBufferPointer(), g_pPixelShaderBlob->GetBufferSize(), nullptr, &g_pPixelShader) != S_OK)
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
    //ImGui::EndFrame();
    //ImGui::NewFrame();

    if (!g_pd3dDevice)
        return;

    if (g_pFontSampler)
    {
        g_pFontSampler->Release();
        g_pFontSampler = nullptr;
    }
    if (g_pFontTextureView)
    {
        g_pFontTextureView->Release();
        g_pFontTextureView = nullptr;
        ImGui::GetIO().Fonts->TexID = nullptr;
    } // We copied g_pFontTextureView to io.Fonts->TexID so let's clear that as well.
    if (g_pIB)
    {
        g_pIB->Release();
        g_pIB = nullptr;
    }
    if (g_pVB)
    {
        g_pVB->Release();
        g_pVB = nullptr;
    }

    if (g_pBlendState)
    {
        g_pBlendState->Release();
        g_pBlendState = nullptr;
    }
    if (g_pDepthStencilState)
    {
        g_pDepthStencilState->Release();
        g_pDepthStencilState = nullptr;
    }
    if (g_pRasterizerState)
    {
        g_pRasterizerState->Release();
        g_pRasterizerState = nullptr;
    }
    if (g_pPixelShader)
    {
        g_pPixelShader->Release();
        g_pPixelShader = nullptr;
    }
    if (g_pPixelShaderBlob)
    {
        g_pPixelShaderBlob->Release();
        g_pPixelShaderBlob = nullptr;
    }
    if (g_pVertexConstantBuffer)
    {
        g_pVertexConstantBuffer->Release();
        g_pVertexConstantBuffer = nullptr;
    }
    if (g_pInputLayout)
    {
        g_pInputLayout->Release();
        g_pInputLayout = nullptr;
    }
    if (g_pVertexShader)
    {
        g_pVertexShader->Release();
        g_pVertexShader = nullptr;
    }
    if (g_pVertexShaderBlob)
    {
        g_pVertexShaderBlob->Release();
        g_pVertexShaderBlob = nullptr;
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
    FS.update_path(fName, fsgame::app_data_root, io.IniFilename);
    io.IniFilename = xr_strdup(fName);

    FS.update_path(fName, fsgame::logs, io.LogFilename);
    io.LogFilename = xr_strdup(fName);

    io.ImeWindowHandle = g_hWnd;

    return true;
}

void ImGui_ImplDX11_Shutdown()
{
    ImGui_ImplDX11_InvalidateDeviceObjects();
    g_pd3dDevice = nullptr;
    g_pd3dDeviceContext = nullptr;
    g_hWnd = (HWND)nullptr;

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
