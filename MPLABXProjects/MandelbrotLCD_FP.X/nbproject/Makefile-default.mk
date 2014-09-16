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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MandelbrotLCD_FP.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MandelbrotLCD_FP.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../Microchip/Graphics/Primitive.c ../../Microchip/Graphics/DisplayDriver.c ../../Microchip/Graphics/GOLFontDefault.c ../../MMB/MMB.c "../../Mandelbrot LCD FP/MainDemo.c"

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/375547254/Primitive.o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o ${OBJECTDIR}/_ext/1445236994/MMB.o ${OBJECTDIR}/_ext/1398283881/MainDemo.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/375547254/Primitive.o.d ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d ${OBJECTDIR}/_ext/1445236994/MMB.o.d ${OBJECTDIR}/_ext/1398283881/MainDemo.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/375547254/Primitive.o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o ${OBJECTDIR}/_ext/1445236994/MMB.o ${OBJECTDIR}/_ext/1398283881/MainDemo.o

# Source Files
SOURCEFILES=../../Microchip/Graphics/Primitive.c ../../Microchip/Graphics/DisplayDriver.c ../../Microchip/Graphics/GOLFontDefault.c ../../MMB/MMB.c ../../Mandelbrot LCD FP/MainDemo.c


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/MandelbrotLCD_FP.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

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
${OBJECTDIR}/_ext/375547254/Primitive.o: ../../Microchip/Graphics/Primitive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Primitive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Mandelbrot LCD FP" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Primitive.o.d" -o ${OBJECTDIR}/_ext/375547254/Primitive.o ../../Microchip/Graphics/Primitive.c   
	
${OBJECTDIR}/_ext/375547254/DisplayDriver.o: ../../Microchip/Graphics/DisplayDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Mandelbrot LCD FP" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" -o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ../../Microchip/Graphics/DisplayDriver.c   
	
${OBJECTDIR}/_ext/375547254/GOLFontDefault.o: ../../Microchip/Graphics/GOLFontDefault.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Mandelbrot LCD FP" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d" -o ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o ../../Microchip/Graphics/GOLFontDefault.c   
	
${OBJECTDIR}/_ext/1445236994/MMB.o: ../../MMB/MMB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/MMB.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Mandelbrot LCD FP" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/MMB.o.d" -o ${OBJECTDIR}/_ext/1445236994/MMB.o ../../MMB/MMB.c   
	
${OBJECTDIR}/_ext/1398283881/MainDemo.o: ../../Mandelbrot\ LCD\ FP/MainDemo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1398283881 
	@${RM} ${OBJECTDIR}/_ext/1398283881/MainDemo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1398283881/MainDemo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1398283881/MainDemo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Mandelbrot LCD FP" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1398283881/MainDemo.o.d" -o ${OBJECTDIR}/_ext/1398283881/MainDemo.o "../../Mandelbrot LCD FP/MainDemo.c"   
	
else
${OBJECTDIR}/_ext/375547254/Primitive.o: ../../Microchip/Graphics/Primitive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Primitive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Mandelbrot LCD FP" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Primitive.o.d" -o ${OBJECTDIR}/_ext/375547254/Primitive.o ../../Microchip/Graphics/Primitive.c   
	
${OBJECTDIR}/_ext/375547254/DisplayDriver.o: ../../Microchip/Graphics/DisplayDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Mandelbrot LCD FP" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" -o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ../../Microchip/Graphics/DisplayDriver.c   
	
${OBJECTDIR}/_ext/375547254/GOLFontDefault.o: ../../Microchip/Graphics/GOLFontDefault.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Mandelbrot LCD FP" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d" -o ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o ../../Microchip/Graphics/GOLFontDefault.c   
	
${OBJECTDIR}/_ext/1445236994/MMB.o: ../../MMB/MMB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/MMB.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Mandelbrot LCD FP" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/MMB.o.d" -o ${OBJECTDIR}/_ext/1445236994/MMB.o ../../MMB/MMB.c   
	
${OBJECTDIR}/_ext/1398283881/MainDemo.o: ../../Mandelbrot\ LCD\ FP/MainDemo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1398283881 
	@${RM} ${OBJECTDIR}/_ext/1398283881/MainDemo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1398283881/MainDemo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1398283881/MainDemo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Mandelbrot LCD FP" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1398283881/MainDemo.o.d" -o ${OBJECTDIR}/_ext/1398283881/MainDemo.o "../../Mandelbrot LCD FP/MainDemo.c"   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/MandelbrotLCD_FP.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION) -Os -o dist/${CND_CONF}/${IMAGE_TYPE}/MandelbrotLCD_FP.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,--defsym=_min_heap_size=5000,--defsym=_min_stack_size=2048,-L"../../Mandelbrot LCD FP",-L".",-Map="${DISTDIR}/MandelbrotLCD_FP.X.${IMAGE_TYPE}.map"
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/MandelbrotLCD_FP.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION) -Os -o dist/${CND_CONF}/${IMAGE_TYPE}/MandelbrotLCD_FP.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=5000,--defsym=_min_stack_size=2048,-L"../../Mandelbrot LCD FP",-L".",-Map="${DISTDIR}/MandelbrotLCD_FP.X.${IMAGE_TYPE}.map"
	${MP_CC_DIR}/xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/MandelbrotLCD_FP.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
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
