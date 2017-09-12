$ ! Copyrigt (C) 2002 Rene Rivera, Johan Nilsson
$ ! Permission to copy, use, modify, sell and distribute this software
$ ! is granted provided this copyright notice appears in all copies.
$ ! This software is provided "as is" without express or implied
$ ! warranty, and with no claim as to its suitability for any purpose.
$ ! 
$ ! bootstrap build script for Jam
$ ! 
$ ON WARNING THEN CONTINUE
$ !
$ IF "" .NES. F$SEARCH("[.bootstrap_vms]*.*")
$ THEN
$   SET FILE/PROTECTION=(S:RWED) [.bootstrap_vms]*.*;*
$   DELETE [.bootstrap_vms]*.*;*
$ ENDIF
$ !
$ IF "" .NES. F$SEARCH("bootstrap_vms.dir")
$ THEN
$   SET FILE/PROT=(S:RWED) bootstrap_vms.dir
$   DELETE bootstrap_vms.dir;
$ ENDIF
$ !
$ CREATE/DIR [.bootstrap_vms]
$ !
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]builtins.obj builtins.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]command.obj command.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]compile.obj compile.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]execvms.obj execvms.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]expand.obj expand.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]filesys.obj filesys.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]filevms.obj filevms.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]glob.obj glob.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]hash.obj hash.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]hdrmacro.obj hdrmacro.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]headers.obj headers.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]jam.obj jam.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]jambase.obj jambase.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]jamgram.obj jamgram.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]lists.obj lists.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]make.obj make.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]make1.obj make1.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]modules.obj modules.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]newstr.obj newstr.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]option.obj option.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]parse.obj parse.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]pathvms.obj pathvms.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]pwd.obj pwd.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]regexp.obj regexp.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]rules.obj rules.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]scan.obj scan.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]search.obj search.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]strings.obj strings.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]subst.obj subst.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]timestamp.obj timestamp.c
$ cc /DEFINE=VMS /STANDARD=VAXC /OBJECT=[.bootstrap_vms]variable.obj variable.c
$ link -
 /EXECUTABLE=[.bootstrap_vms]jam0.exe -
 /NOMAP -
 [.bootstrap_vms]builtins.obj, -
 [.bootstrap_vms]command.obj, -
 [.bootstrap_vms]compile.obj, -
 [.bootstrap_vms]execvms.obj, -
 [.bootstrap_vms]expand.obj, -
 [.bootstrap_vms]filesys.obj, -
 [.bootstrap_vms]filevms.obj, -
 [.bootstrap_vms]glob.obj, -
 [.bootstrap_vms]hash.obj, -
 [.bootstrap_vms]hdrmacro.obj, -
 [.bootstrap_vms]headers.obj, -
 [.bootstrap_vms]jam.obj, -
 [.bootstrap_vms]jambase.obj, -
 [.bootstrap_vms]jamgram.obj, -
 [.bootstrap_vms]lists.obj, -
 [.bootstrap_vms]make.obj, -
 [.bootstrap_vms]make1.obj, -
 [.bootstrap_vms]modules.obj, -
 [.bootstrap_vms]newstr.obj, -
 [.bootstrap_vms]option.obj, -
 [.bootstrap_vms]parse.obj, -
 [.bootstrap_vms]pathvms.obj, -
 [.bootstrap_vms]pwd.obj, -
 [.bootstrap_vms]regexp.obj, -
 [.bootstrap_vms]rules.obj, -
 [.bootstrap_vms]scan.obj, -
 [.bootstrap_vms]search.obj, -
 [.bootstrap_vms]strings.obj, -
 [.bootstrap_vms]subst.obj, -
 [.bootstrap_vms]timestamp.obj, -
 [.bootstrap_vms]variable.obj
