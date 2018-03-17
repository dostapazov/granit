# ---------------------------------------------------------------------------
!if !$d(BCB)
BCB = $(MAKEDIR)\..
!endif

# ---------------------------------------------------------------------------
# IDE SECTION
# ---------------------------------------------------------------------------
# The following section of the project makefile is managed by the BCB IDE.
# It is recommended to use the IDE to change any of the values in this
# section.
# ---------------------------------------------------------------------------

VERSION = BCB.06.00
# ---------------------------------------------------------------------------
PROJECT = D:\USER\GRANIT\GRKRNL32\gripxmod.dll
OBJFILES = d:\temp\modem.obj d:\temp\modem_line.obj d:\temp\CRCUNIT.obj \
    d:\temp\gripxmod_main.obj d:\temp\grnet_line.obj \
    d:\temp\grnet_line_imp.obj d:\temp\grnet_server.obj \
    d:\temp\grnet_convert.obj d:\temp\grnet_diag.obj d:\temp\granit_diag.obj \
    d:\temp\gkmodules.obj d:\temp\modem_queue.obj d:\temp\grnet_dos.obj \
    d:\temp\kedatas.obj
RESFILES = gripxmod.res d:\temp\gripx_mod.res
MAINSOURCE = gripxmod.bpf
RESDEPEN = $(RESFILES)
LIBFILES = ..\..\..\LIB\gklib.lib ..\..\..\LIB\GKHANDLE.LIB ..\..\..\LIB\Ke_rtls.lib \
    ..\..\..\LIB\otdhlpr.lib D:\BCB\Lib\memmgr.lib
IDLFILES = 
IDLGENFILES = 
LIBRARIES = 
PACKAGES = vcl.bpi rtl.bpi dbrtl.bpi adortl.bpi vcldb.bpi vclx.bpi bdertl.bpi \
    vcldbx.bpi ibxpress.bpi dsnap.bpi cds.bpi bdecds.bpi qrpt.bpi teeui.bpi \
    teedb.bpi tee.bpi dss.bpi teeqr.bpi visualclx.bpi visualdbclx.bpi \
    dsnapcrba.bpi dsnapcon.bpi bcbsmp.bpi vclie.bpi xmlrtl.bpi inet.bpi \
    inetdbbde.bpi inetdbxpress.bpi inetdb.bpi nmfast.bpi webdsnap.bpi \
    bcbie.bpi websnap.bpi soaprtl.bpi dclocx.bpi dbexpress.bpi dbxcds.bpi \
    indy.bpi bcb2kaxserver.bpi
SPARELIBS = 
DEFFILE = 
OTHERFILES = 
# ---------------------------------------------------------------------------
DEBUGLIBPATH = $(BCB)\lib\debug
RELEASELIBPATH = $(BCB)\lib\release
USERDEFINES = _DEBUG
SYSDEFINES = NO_STRICT;_NO_VCL
INCLUDEPATH = ..\..\COMMON;$(BCB)\Projects;..\..\NETRTL;..\..\KE_RTL;..\..\..\PROJS;..\GKLIB;..\SRC\GrKrnl32\GRIPXMOD;$(BCB)\include;$(BCB)\include\vcl;D:\USER\GRANIT\INC;D:\USER\GRANIT\INC\GRKRNL32
LIBPATH = ..\..\COMMON;$(BCB)\Projects;..\..\NETRTL;..\..\KE_RTL;..\..\..\PROJS;..\GKLIB;..\SRC\GrKrnl32\GRIPXMOD;..\SRC\GrKrnl32\KERNEL\KEXPLORER;$(BCB)\lib\obj;$(BCB)\lib
WARNINGS= -w-par
PATHCPP = .;..\GKLIB;..\..\COMMON;..\..\KE_RTL
PATHASM = .;
PATHPAS = .;
PATHRC = .;
PATHOBJ = .;$(LIBPATH)
# ---------------------------------------------------------------------------
CFLAG1 = -WD -Od -H=d:\temp\grkrnl32.csm -Hc -Vx -Ve -X- -r- -a8 -b- -k -y -v -vi- \
    -tWD -tWM- -c
IDLCFLAGS = -I..\..\COMMON -I$(BCB)\Projects -I..\..\NETRTL -I..\..\KE_RTL \
    -I..\..\..\PROJS -I..\GKLIB -I..\SRC\GrKrnl32\GRIPXMOD -I$(BCB)\include \
    -I$(BCB)\include\vcl -ID:\USER\GRANIT\INC -ID:\USER\GRANIT\INC\GRKRNL32 \
    -src_suffix cpp -D_DEBUG -boa
PFLAGS = -N2d:\temp -N0d:\temp -$Y+ -$W -$O- -$A8 -v -JPHNE -M
RFLAGS = 
AFLAGS = /mx /w2 /zi
LFLAGS = -Id:\temp -D"" -B:0x12200000 -aa -Tpd -x -Gn -v
# ---------------------------------------------------------------------------
ALLOBJ = c0d32.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) $(LIBRARIES) import32.lib cw32.lib
# ---------------------------------------------------------------------------
!ifdef IDEOPTIONS

[Version Info]
IncludeVerInfo=0
AutoIncBuild=0
MajorVer=1
MinorVer=0
Release=0
Build=0
Debug=0
PreRelease=0
Special=0
Private=0
DLL=0

[Version Info Keys]
CompanyName=
FileDescription=
FileVersion=1.0.0.0
InternalName=
LegalCopyright=
LegalTrademarks=
OriginalFilename=
ProductName=
ProductVersion=1.0.0.0
Comments=

[Debugging]
DebugSourceDirs=$(BCB)\source\vcl

!endif





# ---------------------------------------------------------------------------
# MAKE SECTION
# ---------------------------------------------------------------------------
# This section of the project file is not used by the BCB IDE.  It is for
# the benefit of building from the command-line using the MAKE utility.
# ---------------------------------------------------------------------------

.autodepend
# ---------------------------------------------------------------------------
!if "$(USERDEFINES)" != ""
AUSERDEFINES = -d$(USERDEFINES:;= -d)
!else
AUSERDEFINES =
!endif

!if !$d(BCC32)
BCC32 = bcc32
!endif

!if !$d(CPP32)
CPP32 = cpp32
!endif

!if !$d(DCC32)
DCC32 = dcc32
!endif

!if !$d(TASM32)
TASM32 = tasm32
!endif

!if !$d(LINKER)
LINKER = ilink32
!endif

!if !$d(BRCC32)
BRCC32 = brcc32
!endif


# ---------------------------------------------------------------------------
!if $d(PATHCPP)
.PATH.CPP = $(PATHCPP)
.PATH.C   = $(PATHCPP)
!endif

!if $d(PATHPAS)
.PATH.PAS = $(PATHPAS)
!endif

!if $d(PATHASM)
.PATH.ASM = $(PATHASM)
!endif

!if $d(PATHRC)
.PATH.RC  = $(PATHRC)
!endif

!if $d(PATHOBJ)
.PATH.OBJ  = $(PATHOBJ)
!endif
# ---------------------------------------------------------------------------
$(PROJECT): $(OTHERFILES) $(IDLGENFILES) $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) -L$(LIBPATH) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES)
!
# ---------------------------------------------------------------------------
.pas.hpp:
    $(BCB)\BIN\$(DCC32) $(PFLAGS) -U$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -O$(INCLUDEPATH) --BCB {$< }

.pas.obj:
    $(BCB)\BIN\$(DCC32) $(PFLAGS) -U$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -O$(INCLUDEPATH) --BCB {$< }

.cpp.obj:
    $(BCB)\BIN\$(BCC32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n$(@D) {$< }

.c.obj:
    $(BCB)\BIN\$(BCC32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n$(@D) {$< }

.c.i:
    $(BCB)\BIN\$(CPP32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n. {$< }

.cpp.i:
    $(BCB)\BIN\$(CPP32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n. {$< }

.asm.obj:
    $(BCB)\BIN\$(TASM32) $(AFLAGS) -i$(INCLUDEPATH:;= -i) $(AUSERDEFINES) -d$(SYSDEFINES:;= -d) $<, $@

.rc.res:
    $(BCB)\BIN\$(BRCC32) $(RFLAGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -fo$@ $<



# ---------------------------------------------------------------------------




