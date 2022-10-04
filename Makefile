-include Makefile.config

TARGET = $(APP_NAME)
PROJECT_ROOT = project
MODIFYAUTHOR = $(AUTHOR_NAME)

# 不需要:
# o  使用make的内置规则和变量(这样可以提高性能并避免难以调试的行为);
# o  打印"Entering directory ...";
MAKEFLAGS += -rR --no-print-directory

# 把更多的注意力放在警告上, 默认情况下不那么冗长
# 使用'make v=1'查看完整的命令
ifeq ("$(origin V)", "command line")
  KBUILD_VERBOSE = $(V)
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

# Kbuild支持将输出文件保存在单独的目录中.
# 要在单独的目录中定位输出文件，支持两个语法.
# 在这两种情况下，工作目录都必须是工程的根目录.
# 1) O=
# 使用 "make O=dir/to/store/output/files/"
#
# 2) 设置 KBUILD_OUTPUT
# 将环境变量KBUILD_OUTPUT设置为指向应放置输出文件的目录.
# export KBUILD_OUTPUT=dir/to/store/output/files/
# make
#
# O=赋值优先于KBUILD_OUTPUT环境变量.

# 默认目标文件
PHONY := _all
_all:

# 在obj目录中调用make时设置KBUILD_SRC
# KBUILD_SRC不打算由普通用户使用(目前)
ifeq ($(KBUILD_SRC),)

# 在源码所在的目录中调用make
# 是否要在单独的目录中定位输出文件？
ifeq ("$(origin O)", "command line")
  KBUILD_OUTPUT := $(O)
endif

ifeq ("$(origin W)", "command line")
  export KBUILD_ENABLE_EXTRA_GCC_CHECKS := $(W)
endif

# 取消顶级Makefile上的隐式规则
$(CURDIR)/Makefile Makefile: ;

ifneq ($(KBUILD_OUTPUT),)
# 在输出目录中调用第二个make，传递相关变量检查输出目录是否实际存在
saved-output  := $(KBUILD_OUTPUT)
KBUILD_OUTPUT := $(shell cd $(KBUILD_OUTPUT) && /bin/pwd)
$(if $(KBUILD_OUTPUT),, \
     $(error output directory "$(saved-output)" does not exist))

PHONY += $(MAKECMDGOALS) sub-make

$(filter-out _all sub-make $(CURDIR)/Makefile, $(MAKECMDGOALS)) _all: sub-make
	$(Q)@:

sub-make: FORCE
	$(if $(KBUILD_VERBOSE:1=),@)$(MAKE) -C $(KBUILD_OUTPUT) \
	KBUILD_SRC=$(CURDIR) \
	-f $(CURDIR)/Makefile \
	$(filter-out _all sub-make,$(MAKECMDGOALS))

# 将处理留给上述make调用
skip-makefile := 1
endif # ifneq ($(KBUILD_OUTPUT),)
endif # ifeq ($(KBUILD_SRC),)

# 如果这是make的最后一次调用，我们将处理Makefile的其余部分
ifeq ($(skip-makefile),)

# 如果构建一个外部模块，我们不关心all: 规则，而是_all依赖的模块
PHONY += all
_all: all

srctree		:= $(if $(KBUILD_SRC),$(KBUILD_SRC),$(CURDIR))
objtree		:= $(CURDIR)
src			:= $(srctree)
obj			:= $(objtree)

VPATH		:= $(srctree)

export srctree objtree VPATH

SUBARCH := $(shell uname -m | sed -e s/i.86/x86/ -e s/x86_64/x86/ \
				  -e s/sun4u/sparc64/ \
				  -e s/arm.*/arm/ -e s/sa110/arm/ \
				  -e s/s390x/s390/ -e s/parisc64/parisc/ \
				  -e s/ppc.*/powerpc/ -e s/mips.*/mips/ \
				  -e s/sh[234].*/sh/ -e s/aarch64.*/arm64/ )
# 平台架构
ARCH 			?= $(SUBARCH)
CROSS_COMPILE	?= $(CONFIG_CROSS_COMPILE:"%"=%)

KCONFIG_CONFIG	?= .config
export KCONFIG_CONFIG

# kbuild使用的shell
CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	  else if [ -x /bin/bash ]; then echo /bin/bash; \
	  else echo sh; fi ; fi)

HOSTCC       = gcc
HOSTCXX      = g++
HOSTCFLAGS   = -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer -Wno-format-overflow
HOSTCXXFLAGS = -O2

# 美化输出
# ---------------------------------------------------------------------------
#
# 通常，在执行命令之前，我们会回显整个命令.
# 通过设置echo $($(quiet)$(cmd))，我们现在可以设置$(quiet)来选择其他形式的输出，例如.
#
#         quiet_cmd_cc_o_c = Compiling $(RELDIR)/$@
#         cmd_cc_o_c       = $(CC) $(c_flags) -c -o $@ $<
#
# 如果$(quiet)为空，则将打印整个命令.
# 如果设置为"quiet_"，则只打印短版本.
# 如果设置为"silent_"，则不会打印任何内容，因为变量$(silent_cmd_cc_o_c)不存在.
#
# 一个简单的变体是在命令前面加上$(Q) - 这对于在非详细模式下隐藏的命令很有用.
#
#	$(Q)ln $@ :<
#
# 如果KBUILD_VERBOSE等于0，则将隐藏上述命令.
# 如果KBUILD_VERBOSE等于1，则显示上述命令.

ifeq ($(KBUILD_VERBOSE),1)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif

# 如果用户正在运行make -s(静默模式)，则禁止命令的回显
ifneq ($(findstring s,$(MAKEFLAGS)),)
  quiet=silent_
endif

export quiet Q KBUILD_VERBOSE

# 查找与工程源码的根相关的make头文件
MAKEFLAGS += --include-dir=$(srctree)

# 我们需要一些通用定义(不要试图重新生成文件)
$(srctree)/scripts/Kbuild.include: ;
include $(srctree)/scripts/Kbuild.include

# 构建使用的变量(CC, etc...)
AS				= $(CROSS_COMPILE)as
LD				= $(CROSS_COMPILE)ld
CC				= $(CROSS_COMPILE)gcc
CPP				= $(CC) -E
CXX				= $(CROSS_COMPILE)g++
AR				= $(CROSS_COMPILE)ar
NM				= $(CROSS_COMPILE)nm
SIZE			= $(CROSS_COMPILE)size
STRIP			= $(CROSS_COMPILE)strip
OBJCOPY			= $(CROSS_COMPILE)objcopy
OBJDUMP			= $(CROSS_COMPILE)objdump
AWK				= awk
INSTALLKERNEL	:= installkernel
PERL			= perl

CHECKFLAGS      := -D__linux__ -Dlinux -D__STDC__ -Dunix -D__unix__ -Wbitwise -Wno-return-void $(CF)
CFLAGS_KERNEL	=
CXXFLAGS_KERNEL	=
AFLAGS_KERNEL	=

CHECKFLAGS 		+= $(if $(CONFIG_ARCH_64BIT),-m64,-m32)

# 必须引用include/目录时，请使用LINUXINCLUDE
# 需要与O=选项兼容
LINUXINCLUDE     := -Iinclude \
                   $(if $(KBUILD_SRC), -I$(srctree)/include) \
                   -include include/generated/autoconf.h \
				   -I$(PROJECT_ROOT)/ \
				   -I$(PROJECT_ROOT)/entry \
				   -I$(PROJECT_ROOT)/platform \
				   -I$(PROJECT_ROOT)/3rdparty/include \
				   $(USER_LIB_INCLUDE_FLAG)

KBUILD_AFLAGS_KERNEL 	:=
KBUILD_AFLAGS   		:= -D__ASSEMBLY__

# 从include/config/app.release(如果存在)读取APPRELEASE
APPRELEASE = $(shell cat include/config/app.release 2> /dev/null)
APPVERSION = $(VERSION)$(if $(PATCHLEVEL),.$(PATCHLEVEL)$(if $(SUBLEVEL),.$(SUBLEVEL)))$(EXTRAVERSION)

# 编译日期时间
COMPILE_TIME := `date +%H:%M:%S`
COMPILE_DATE := $(shell date --rfc-3339=date)

KBUILD_CFLAGS 	 += -DMODIFY_AUTHOR=\"$(MODIFYAUTHOR)\" -DAPP_NAME=\"$(TARGET)\" -DBUILD_OS=\"`uname -n`\" -DBUILD_VERSION=\"$(APPVERSION)\" -DBUILD_DATE=\"$(COMPILE_DATE)\" -DBUILD_TIME=\"$(COMPILE_TIME)\"
KBUILD_CFLAGS	 += -DOS_VERSION=\"`lsb_release -r --short`\" -DOS_CODENAME=\"`lsb_release -c --short`\"

KBUILD_CXXFLAGS  += -DMODIFY_AUTHOR=\"$(MODIFYAUTHOR)\" -DAPP_NAME=\"$(TARGET)\" -DBUILD_OS=\"`uname -n`\" -DBUILD_VERSION=\"$(APPVERSION)\" -DBUILD_DATE=\"$(COMPILE_DATE)\" -DBUILD_TIME=\"$(COMPILE_TIME)\"
KBUILD_CXXFLAGS  += -DOS_VERSION=\"`lsb_release -r --short`\" -DOS_CODENAME=\"`lsb_release -c --short`\"

export APPRELEASE APPVERSION
export ARCH SUBARCH SRCARCH CONFIG_SHELL HOSTCC HOSTCFLAGS CROSS_COMPILE AS LD CC CXX
export CPP AR NM SIZE STRIP OBJCOPY OBJDUMP
export MAKE AWK GENKSYMS INSTALLKERNEL PERL UTS_MACHINE
export HOSTCXX HOSTCXXFLAGS

export KBUILD_CPPFLAGS NOSTDINC_FLAGS LINUXINCLUDE OBJCOPYFLAGS LDFLAGS KBUILD_CXXFLAGS
export KBUILD_CFLAGS CFLAGS_KERNEL CXXFLAGS_KERNEL
export KBUILD_AFLAGS AFLAGS_KERNEL
export KBUILD_AFLAGS_KERNEL KBUILD_CFLAGS_KERNEL KBUILD_CXXFLAGS_KERNEL
export KBUILD_ARFLAGS

# 查找中要忽略的文件 ... 声明
RCS_FIND_IGNORE := \( -name SCCS -o -name BitKeeper -o -name .svn -o -name CVS -o -name .pc -o -name .hg -o -name .git -o -name document -o -name sdk \) -prune -o

# ===========================================================================
# *config目标和生成目标之间共享的规则

# 内置scripts/的基本帮助程序
PHONY += scripts_basic
scripts_basic:
	$(Q)$(MAKE) $(build)=scripts/basic

# 要避免任何隐式规则进入，请定义一个空命令.
scripts/basic/%: scripts_basic ;

PHONY += outputmakefile
# 如果使用单独的输出目录，则outputmakefile在输出目录中生成Makefile。这允许在输出目录中方便地使用make
outputmakefile:
ifneq ($(KBUILD_SRC),)
	$(Q)ln -fsn $(srctree) source
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkmakefile \
	    $(srctree) $(objtree) $(VERSION) $(PATCHLEVEL)
endif

# 为了确保不包含任何*config目标的.config，请尽早捕获它们，并将它们交给scripts/kconfig/Makefile
# 允许在调用make时指定更多目标，包括混合*config目标和构建目标
# 例如 'make oldconfig all'
# 当指定了混合目标时进行检测，并再次调用make，因此在本例中也不包括.config(对于*config)

no-dot-config-targets := clean mrproper distclean \
			 cscope gtags TAGS tags help %docs check% coccicheck \
			 include/linux/version.h headers_% \
			 kernelversion %src-pkg

config-targets := 0
mixed-targets  := 0
dot-config     := 1

ifneq ($(filter $(no-dot-config-targets), $(MAKECMDGOALS)),)
	ifeq ($(filter-out $(no-dot-config-targets), $(MAKECMDGOALS)),)
		dot-config := 0
	endif
endif

ifneq ($(filter config %config,$(MAKECMDGOALS)),)
        config-targets := 1
        ifneq ($(filter-out config %config,$(MAKECMDGOALS)),)
                mixed-targets := 1
        endif
endif

ifeq ($(mixed-targets),1)
# ===========================================================================
# 我们使用混合目标来调用(*config和构建目标)
# 一个接一个处理

%:: FORCE
	$(Q)$(MAKE) -C $(srctree) KBUILD_SRC= $@

else
ifeq ($(config-targets),1)
# ===========================================================================
# 仅*config目标 - 确保先决条件已更新，并在scripts/kconfig中下降以使*config目标

# 在需要时可以读取架构指定的Makefile设置KBUILD_DEFCONFIG
# KBUILD_DEFCONFIG可以指出用于'make defconfig'的替代默认配置
#include $(srctree)/arch/$(SRCARCH)/Makefile
export KBUILD_DEFCONFIG KBUILD_KCONFIG

config: scripts_basic outputmakefile FORCE
	$(Q)mkdir -p include/linux include/config
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

%config: scripts_basic outputmakefile FORCE
	$(Q)mkdir -p include/linux include/config
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

else

ifeq ($(dot-config),1)
# 读入一个配置
-include include/config/auto.conf

# 读入对所有Kconfig*文件的依赖项，确保在检测到更改时运行oldconfig
-include include/config/auto.conf.cmd

# 要避免任何隐式规则进入，请定义一个空命令
$(KCONFIG_CONFIG) include/config/auto.conf.cmd: ;

# 如果.config比include/config/auto.conf更新，则有人修改了它，忘记了运行make oldconfig
# 如果auto.conf.cmd丢失，那么我们可能在一个干净的树中，因此我们执行配置步骤以确保捕获更新的kconfig文件
include/config/%.conf: $(KCONFIG_CONFIG) include/config/auto.conf.cmd
	$(Q)$(MAKE) -f $(srctree)/Makefile silentoldconfig

else
# 需要虚拟目标，因为用作先决条件
include/config/auto.conf: ;
endif # $(dot-config)

# 当命令行上没有给定目标时，all: target是默认值
# 这允许用户只发出'make'来构建包含模块的内核
# 默认为$(TARGET)，但是arch makefile通常会添加更多的目标
all: $(TARGET)

libs-y		:=
objs-y		:= $(PROJECT_ROOT)

$(TARGET)-dirs	:= $(objs-y) $(libs-y)
$(TARGET)-objs	:= $(patsubst %,%/built-in.o, $(objs-y))
$(TARGET)-libs	:= $(patsubst %,%/lib.a, $(libs-y))
$(TARGET)-all	:= $($(TARGET)-objs) $($(TARGET)-libs)

# ifeq ($(APP_NAME),rv1126_azure_gui)
# AZURE_THREADX_LDS=$(PROJECT_ROOT)/gui/azure/threadx/cortex_a7/asm/sample_threadx.ld

# # 在预先链接的$(TARGET)上执行modpost。最终链接的$(TARGET)根据链接器脚本重命名了相关部分
# quiet_cmd_$(TARGET) = LD $@
#       cmd_$(TARGET) = $(CXX) $(LDFLAGS) -o $@ -T $(AZURE_THREADX_LDS) -e Vectors \
#       -Wl,--start-group $($(TARGET)-libs) $($(TARGET)-objs) $(USER_LINK_LIB) $(USER_PTH_RT_LIB) -Wl,--end-group
# #      -Wl,-wrap,malloc,-wrap,free,-wrap,realloc,--start-group $($(TARGET)-libs) $($(TARGET)-objs) $(USER_LINK_LIB) $(USER_PTH_RT_LIB) -Wl,--end-group
# else
# 在预先链接的$(TARGET)上执行modpost。最终链接的$(TARGET)根据链接器脚本重命名了相关部分
quiet_cmd_$(TARGET) = LD $@
      cmd_$(TARGET) = $(CXX) $(LDFLAGS) -o $@ \
      -Wl,--start-group $($(TARGET)-libs) $($(TARGET)-objs) $(USER_LINK_LIB) $(USER_PTH_RT_LIB) -Wl,--end-group
#      -Wl,-wrap,malloc,-wrap,free,-wrap,realloc,--start-group $($(TARGET)-libs) $($(TARGET)-objs) $(USER_LINK_LIB) $(USER_PTH_RT_LIB) -Wl,--end-group
# endif

# 最终生成目标文件
$(TARGET): $($(TARGET)-all) FORCE
	$(call if_changed,$(TARGET))
	$(Q)echo
	$(Q)echo 'Application: $@ is ready => ${APPVERSION}'
#	$(Q)$(STRIP) $@
	$(Q)$(SIZE) $@
	$(Q)du -sh $@

# 实际对象是在降序时生成的，请确保没有隐式规则进入
$(sort $($(TARGET)-all)): $($(TARGET)-dirs) ;

# 句柄降序到$($(TARGET)-dirs)中列出的子目录中
# 预设语言环境变量以加快生成过程。在运行make menuconfig等时，将区域设置调整限制在此位置以避免错误的语言设置
# 错误消息仍以原始语言显示

#PHONY += $($(TARGET)-dirs)
#$($(TARGET)-dirs): 准备脚本
PHONY += $($(TARGET)-dirs)
$($(TARGET)-dirs): scripts_basic
	$(Q)$(MAKE) $(build)=$@

###
# 清理工作分三个层次进行
# make clean     删除大多数生成的文件，留下足够的空间来构建外部模块
# make mrproper  删除当前配置和所有生成的文件
# make distclean 删除编辑器备份文件、修补剩余文件等

# 用'make clean'删除的目录和文件
CLEAN_DIRS  +=
CLEAN_FILES += $(TARGET)

# 用'make mrprope'删除目录和文件
MRPROPER_DIRS  += include/config include/generated
MRPROPER_FILES += .config .config.old tags TAGS cscope* GPATH GTAGS GRTAGS GSYMS

clean: rm-dirs  := $(CLEAN_DIRS)
clean: rm-files := $(CLEAN_FILES)
clean-dirs      := $(addprefix _clean_, $($(TARGET)-dirs))

PHONY += $(clean-dirs) clean archclean
$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

clean: $(clean-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)
	@find . $(RCS_FIND_IGNORE) \
		\( -name '*.[oas]' -o -name '.*.cmd' \
		-o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \
		-o -name modules.builtin -o -name '.tmp_*.o.*' \
		-o -name '*.gcno' \) -type f -print | xargs rm -f

mrproper: rm-dirs  := $(wildcard $(MRPROPER_DIRS))
mrproper: rm-files := $(wildcard $(MRPROPER_FILES))
mrproper-dirs      := $(addprefix _mrproper_, scripts)

PHONY += $(mrproper-dirs) mrproper
$(mrproper-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _mrproper_%,%,$@)

mrproper: clean $(mrproper-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)

PHONY += distclean
distclean: mrproper
	@find $(srctree) $(RCS_FIND_IGNORE) \
		\( -name '*.orig' -o -name '*.rej' -o -name '*~' \
		-o -name '*.bak' -o -name '#*#' -o -name '.*.orig' \
		-o -name '.*.rej' -o -size 0 \
		-o -name '*%' -o -name '.*.cmd' -o -name 'core' \) \
		-type f -print | xargs rm -f

quiet_cmd_rmdirs = $(if $(wildcard $(rm-dirs)),CLEAN   $(wildcard $(rm-dirs)))
      cmd_rmdirs = rm -rf $(rm-dirs)

quiet_cmd_rmfiles = $(if $(wildcard $(rm-files)),CLEAN   $(wildcard $(rm-files)))
      cmd_rmfiles = rm -f $(rm-files)

# 速记 $(Q)$(MAKE) -f scripts/Makefile.clean obj=dir
# 用法:
# $(Q)$(MAKE) $(clean)=dir
clean := -f $(if $(KBUILD_SRC),$(srctree)/)scripts/Makefile.clean obj

help:
	@echo  '$(KBUILD_CXXFLAGS)'
	@echo  'Cleaning targets:'
	@echo  '  clean		  - Remove most generated files but keep the config and'
	@echo  '                    enough build support to build external modules'
	@echo  '  mrproper	  - Remove all generated files + config + various backup files'
	@echo  '  distclean	  - mrproper + remove editor backup and patch files'
	@echo  ''
	@echo  'Configuration targets:'
	@$(MAKE) -f $(srctree)/scripts/kconfig/Makefile help
	@echo  ''
	@echo  'Other generic targets:'
	@echo  '  all		  - Build all targets marked with [*]'
	@echo  '  $(TARGET)	  - Build the application'
	@echo  '  dir/            - Build all files in dir and below'
	@echo  '  dir/file.[oisS] - Build specified target only'
	@echo  '  dir/file.lst    - Build specified mixed source/assembly target only'
	@echo  '                    (requires a recent binutils and recent build (System.map))'
	@echo  '  tags/TAGS	  - Generate tags file for editors'
	@echo  '  cscope	  - Generate cscope index'
	@echo  '  gtags           - Generate GNU GLOBAL index'
	@echo  '  kernelrelease	  - Output the release version string'
	@echo  '  kernelversion	  - Output the version stored in Makefile'
	 echo  ''
	@echo  'Static analysers'
	@echo  '  checkstack      - Generate a list of stack hogs'
	@echo  '  namespacecheck  - Name space analysis on compiled kernel'
	@echo  '  versioncheck    - Sanity check on version.h usage'
	@echo  '  includecheck    - Check for duplicate included header files'
	@echo  '  export_report   - List the usages of all exported symbols'
	@echo  '  headers_check   - Sanity check on exported headers'
	@echo  ''
	@echo  ''
	@echo  ''
	@echo  '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
	@echo  '  make V=2   [targets] 2 => give reason for rebuild of target'
	@echo  '  make O=dir [targets] Locate all output files in "dir", including .config'
	@echo  '  make W=n   [targets] Enable extra gcc checks, n=1,2,3 where'
	@echo  '		1: warnings which may be relevant and do not occur too often'
	@echo  '		2: warnings which occur quite often but may still be relevant'
	@echo  '		3: more obscure warnings, can most likely be ignored'
	@echo  '		Multiple levels can be combined with W=12 or W=123'
	@echo  '  make RECORDMCOUNT_WARN=1 [targets] Warn about ignored mcount sections'
	@echo  ''
	@echo  'Execute "make" or "make all" to build all targets marked with [*] '
	@echo  'For further info see the ./README.md file'
endif
endif

endif

PHONY += FORCE
FORCE:

.PHONY: $(PHONY)
