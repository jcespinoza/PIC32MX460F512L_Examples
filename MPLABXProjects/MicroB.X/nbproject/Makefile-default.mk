#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MicroB.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MicroB.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../MicroB/SDMMC.c ../../MicroB/boot_load_hex.c ../../MicroB/fileio.c ../../MicroB/MicroB.c ../../MicroB/MiniGraphic.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1964899394/SDMMC.o ${OBJECTDIR}/_ext/1964899394/boot_load_hex.o ${OBJECTDIR}/_ext/1964899394/fileio.o ${OBJECTDIR}/_ext/1964899394/MicroB.o ${OBJECTDIR}/_ext/1964899394/MiniGraphic.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1964899394/SDMMC.o.d ${OBJECTDIR}/_ext/1964899394/boot_load_hex.o.d ${OBJECTDIR}/_ext/1964899394/fileio.o.d ${OBJECTDIR}/_ext/1964899394/MicroB.o.d ${OBJECTDIR}/_ext/1964899394/MiniGraphic.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1964899394/SDMMC.o ${OBJECTDIR}/_ext/1964899394/boot_load_hex.o ${OBJECTDIR}/_ext/1964899394/fileio.o ${OBJECTDIR}/_ext/1964899394/MicroB.o ${OBJECTDIR}/_ext/1964899394/MiniGraphic.o

# Source Files
SOURCEFILES=../../MicroB/SDMMC.c ../../MicroB/boot_load_hex.c ../../MicroB/fileio.c ../../MicroB/MicroB.c ../../MicroB/MiniGraphic.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/MicroB.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX460F512L
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1964899394/SDMMC.o: ../../MicroB/SDMMC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1964899394 
	@${RM} ${OBJECTDIR}/_ext/1964899394/SDMMC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1964899394/SDMMC.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1964899394/SDMMC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -O2 -fomit-frame-pointer -I"../../MicroB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1964899394/SDMMC.o.d" -o ${OBJECTDIR}/_ext/1964899394/SDMMC.o ../../MicroB/SDMMC.c   
	
${OBJECTDIR}/_ext/1964899394/boot_load_hex.o: ../../MicroB/boot_load_hex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1964899394 
	@${RM} ${OBJECTDIR}/_ext/1964899394/boot_load_hex.o.d 
	@${RM} ${OBJECTDIR}/_ext/1964899394/boot_load_hex.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1964899394/boot_load_hex.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -O2 -fomit-frame-pointer -I"../../MicroB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1964899394/boot_load_hex.o.d" -o ${OBJECTDIR}/_ext/1964899394/boot_load_hex.o ../../MicroB/boot_load_hex.c   
	
${OBJECTDIR}/_ext/1964899394/fileio.o: ../../MicroB/fileio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1964899394 
	@${RM} ${OBJECTDIR}/_ext/1964899394/fileio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1964899394/fileio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1964899394/fileio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -O2 -fomit-frame-pointer -I"../../MicroB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1964899394/fileio.o.d" -o ${OBJECTDIR}/_ext/1964899394/fileio.o ../../MicroB/fileio.c   
	
${OBJECTDIR}/_ext/1964899394/MicroB.o: ../../MicroB/MicroB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1964899394 
	@${RM} ${OBJECTDIR}/_ext/1964899394/MicroB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1964899394/MicroB.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1964899394/MicroB.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -O2 -fomit-frame-pointer -I"../../MicroB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1964899394/MicroB.o.d" -o ${OBJECTDIR}/_ext/1964899394/MicroB.o ../../MicroB/MicroB.c   
	
${OBJECTDIR}/_ext/1964899394/MiniGraphic.o: ../../MicroB/MiniGraphic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1964899394 
	@${RM} ${OBJECTDIR}/_ext/1964899394/MiniGraphic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1964899394/MiniGraphic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1964899394/MiniGraphic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -O2 -fomit-frame-pointer -I"../../MicroB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1964899394/MiniGraphic.o.d" -o ${OBJECTDIR}/_ext/1964899394/MiniGraphic.o ../../MicroB/MiniGraphic.c   
	
else
${OBJECTDIR}/_ext/1964899394/SDMMC.o: ../../MicroB/SDMMC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1964899394 
	@${RM} ${OBJECTDIR}/_ext/1964899394/SDMMC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1964899394/SDMMC.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1964899394/SDMMC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -O2 -fomit-frame-pointer -I"../../MicroB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1964899394/SDMMC.o.d" -o ${OBJECTDIR}/_ext/1964899394/SDMMC.o ../../MicroB/SDMMC.c   
	
${OBJECTDIR}/_ext/1964899394/boot_load_hex.o: ../../MicroB/boot_load_hex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1964899394 
	@${RM} ${OBJECTDIR}/_ext/1964899394/boot_load_hex.o.d 
	@${RM} ${OBJECTDIR}/_ext/1964899394/boot_load_hex.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1964899394/boot_load_hex.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -O2 -fomit-frame-pointer -I"../../MicroB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1964899394/boot_load_hex.o.d" -o ${OBJECTDIR}/_ext/1964899394/boot_load_hex.o ../../MicroB/boot_load_hex.c   
	
${OBJECTDIR}/_ext/1964899394/fileio.o: ../../MicroB/fileio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1964899394 
	@${RM} ${OBJECTDIR}/_ext/1964899394/fileio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1964899394/fileio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1964899394/fileio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -O2 -fomit-frame-pointer -I"../../MicroB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1964899394/fileio.o.d" -o ${OBJECTDIR}/_ext/1964899394/fileio.o ../../MicroB/fileio.c   
	
${OBJECTDIR}/_ext/1964899394/MicroB.o: ../../MicroB/MicroB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1964899394 
	@${RM} ${OBJECTDIR}/_ext/1964899394/MicroB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1964899394/MicroB.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1964899394/MicroB.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -O2 -fomit-frame-pointer -I"../../MicroB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1964899394/MicroB.o.d" -o ${OBJECTDIR}/_ext/1964899394/MicroB.o ../../MicroB/MicroB.c   
	
${OBJECTDIR}/_ext/1964899394/MiniGraphic.o: ../../MicroB/MiniGraphic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1964899394 
	@${RM} ${OBJECTDIR}/_ext/1964899394/MiniGraphic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1964899394/MiniGraphic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1964899394/MiniGraphic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -O2 -fomit-frame-pointer -I"../../MicroB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1964899394/MiniGraphic.o.d" -o ${OBJECTDIR}/_ext/1964899394/MiniGraphic.o ../../MicroB/MiniGraphic.c   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/MicroB.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -o dist/${CND_CONF}/${IMAGE_TYPE}/MicroB.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,--defsym=_min_stack_size=256,-L"../../MicroB/C:/Program Files/Microchip/MPLAB C32/lib",-L"../../MicroB/C:/Program Files/Microchip/MPLAB C32/pic32mx/lib",-L".",-Map="${DISTDIR}/MicroB.X.${IMAGE_TYPE}.map",--gc-sections
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/MicroB.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION) -mips16 -mno-float -o dist/${CND_CONF}/${IMAGE_TYPE}/MicroB.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_stack_size=256,-L"../../MicroB/C:/Program Files/Microchip/MPLAB C32/lib",-L"../../MicroB/C:/Program Files/Microchip/MPLAB C32/pic32mx/lib",-L".",-Map="${DISTDIR}/MicroB.X.${IMAGE_TYPE}.map",--gc-sections
	${MP_CC_DIR}/xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/MicroB.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
