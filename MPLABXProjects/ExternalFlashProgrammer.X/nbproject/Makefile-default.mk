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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ExternalFlashProgrammer.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ExternalFlashProgrammer.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED="../../External Flash Programmer/MainDemo.c" ../../Microchip/Graphics/DisplayDriver.c ../../Microchip/Graphics/Primitive.c ../../MMB/LCDTerminal.c ../../MMB/LCDTerminalFont.c ../../MMB/MMB.c ../../MMB/SimpleUART.c ../../MMB/SerialFLASH.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1818477883/MainDemo.o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ${OBJECTDIR}/_ext/375547254/Primitive.o ${OBJECTDIR}/_ext/1445236994/LCDTerminal.o ${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o ${OBJECTDIR}/_ext/1445236994/MMB.o ${OBJECTDIR}/_ext/1445236994/SimpleUART.o ${OBJECTDIR}/_ext/1445236994/SerialFLASH.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1818477883/MainDemo.o.d ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d ${OBJECTDIR}/_ext/375547254/Primitive.o.d ${OBJECTDIR}/_ext/1445236994/LCDTerminal.o.d ${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o.d ${OBJECTDIR}/_ext/1445236994/MMB.o.d ${OBJECTDIR}/_ext/1445236994/SimpleUART.o.d ${OBJECTDIR}/_ext/1445236994/SerialFLASH.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1818477883/MainDemo.o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ${OBJECTDIR}/_ext/375547254/Primitive.o ${OBJECTDIR}/_ext/1445236994/LCDTerminal.o ${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o ${OBJECTDIR}/_ext/1445236994/MMB.o ${OBJECTDIR}/_ext/1445236994/SimpleUART.o ${OBJECTDIR}/_ext/1445236994/SerialFLASH.o

# Source Files
SOURCEFILES=../../External Flash Programmer/MainDemo.c ../../Microchip/Graphics/DisplayDriver.c ../../Microchip/Graphics/Primitive.c ../../MMB/LCDTerminal.c ../../MMB/LCDTerminalFont.c ../../MMB/MMB.c ../../MMB/SimpleUART.c ../../MMB/SerialFLASH.c


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/ExternalFlashProgrammer.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

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
${OBJECTDIR}/_ext/1818477883/MainDemo.o: ../../External\ Flash\ Programmer/MainDemo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1818477883 
	@${RM} ${OBJECTDIR}/_ext/1818477883/MainDemo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1818477883/MainDemo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1818477883/MainDemo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1818477883/MainDemo.o.d" -o ${OBJECTDIR}/_ext/1818477883/MainDemo.o "../../External Flash Programmer/MainDemo.c"   
	
${OBJECTDIR}/_ext/375547254/DisplayDriver.o: ../../Microchip/Graphics/DisplayDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" -o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ../../Microchip/Graphics/DisplayDriver.c   
	
${OBJECTDIR}/_ext/375547254/Primitive.o: ../../Microchip/Graphics/Primitive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Primitive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Primitive.o.d" -o ${OBJECTDIR}/_ext/375547254/Primitive.o ../../Microchip/Graphics/Primitive.c   
	
${OBJECTDIR}/_ext/1445236994/LCDTerminal.o: ../../MMB/LCDTerminal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/LCDTerminal.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/LCDTerminal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/LCDTerminal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/LCDTerminal.o.d" -o ${OBJECTDIR}/_ext/1445236994/LCDTerminal.o ../../MMB/LCDTerminal.c   
	
${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o: ../../MMB/LCDTerminalFont.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o.d" -o ${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o ../../MMB/LCDTerminalFont.c   
	
${OBJECTDIR}/_ext/1445236994/MMB.o: ../../MMB/MMB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/MMB.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/MMB.o.d" -o ${OBJECTDIR}/_ext/1445236994/MMB.o ../../MMB/MMB.c   
	
${OBJECTDIR}/_ext/1445236994/SimpleUART.o: ../../MMB/SimpleUART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SimpleUART.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SimpleUART.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/SimpleUART.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/SimpleUART.o.d" -o ${OBJECTDIR}/_ext/1445236994/SimpleUART.o ../../MMB/SimpleUART.c   
	
${OBJECTDIR}/_ext/1445236994/SerialFLASH.o: ../../MMB/SerialFLASH.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SerialFLASH.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SerialFLASH.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/SerialFLASH.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/SerialFLASH.o.d" -o ${OBJECTDIR}/_ext/1445236994/SerialFLASH.o ../../MMB/SerialFLASH.c   
	
else
${OBJECTDIR}/_ext/1818477883/MainDemo.o: ../../External\ Flash\ Programmer/MainDemo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1818477883 
	@${RM} ${OBJECTDIR}/_ext/1818477883/MainDemo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1818477883/MainDemo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1818477883/MainDemo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1818477883/MainDemo.o.d" -o ${OBJECTDIR}/_ext/1818477883/MainDemo.o "../../External Flash Programmer/MainDemo.c"   
	
${OBJECTDIR}/_ext/375547254/DisplayDriver.o: ../../Microchip/Graphics/DisplayDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" -o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ../../Microchip/Graphics/DisplayDriver.c   
	
${OBJECTDIR}/_ext/375547254/Primitive.o: ../../Microchip/Graphics/Primitive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Primitive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Primitive.o.d" -o ${OBJECTDIR}/_ext/375547254/Primitive.o ../../Microchip/Graphics/Primitive.c   
	
${OBJECTDIR}/_ext/1445236994/LCDTerminal.o: ../../MMB/LCDTerminal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/LCDTerminal.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/LCDTerminal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/LCDTerminal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/LCDTerminal.o.d" -o ${OBJECTDIR}/_ext/1445236994/LCDTerminal.o ../../MMB/LCDTerminal.c   
	
${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o: ../../MMB/LCDTerminalFont.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o.d" -o ${OBJECTDIR}/_ext/1445236994/LCDTerminalFont.o ../../MMB/LCDTerminalFont.c   
	
${OBJECTDIR}/_ext/1445236994/MMB.o: ../../MMB/MMB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/MMB.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/MMB.o.d" -o ${OBJECTDIR}/_ext/1445236994/MMB.o ../../MMB/MMB.c   
	
${OBJECTDIR}/_ext/1445236994/SimpleUART.o: ../../MMB/SimpleUART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SimpleUART.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SimpleUART.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/SimpleUART.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/SimpleUART.o.d" -o ${OBJECTDIR}/_ext/1445236994/SimpleUART.o ../../MMB/SimpleUART.c   
	
${OBJECTDIR}/_ext/1445236994/SerialFLASH.o: ../../MMB/SerialFLASH.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SerialFLASH.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SerialFLASH.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/SerialFLASH.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../External Flash Programmer" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/SerialFLASH.o.d" -o ${OBJECTDIR}/_ext/1445236994/SerialFLASH.o ../../MMB/SerialFLASH.c   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/ExternalFlashProgrammer.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/ExternalFlashProgrammer.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,--defsym=_min_heap_size=2048,--defsym=_min_stack_size=2048,-L"../../External Flash Programmer",-L".",-Map="${DISTDIR}/ExternalFlashProgrammer.X.${IMAGE_TYPE}.map"
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/ExternalFlashProgrammer.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/ExternalFlashProgrammer.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=2048,--defsym=_min_stack_size=2048,-L"../../External Flash Programmer",-L".",-Map="${DISTDIR}/ExternalFlashProgrammer.X.${IMAGE_TYPE}.map"
	${MP_CC_DIR}/xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/ExternalFlashProgrammer.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
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
