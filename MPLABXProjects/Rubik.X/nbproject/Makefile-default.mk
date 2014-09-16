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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Rubik.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Rubik.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../Rubik/Graphic.c ../../Microchip/Graphics/DisplayDriver.c ../../MMB/MMB.c ../../Microchip/Graphics/Primitive.c ../../Rubik/Poly.c ../../Rubik/Geometry4.c ../../Rubik/Cubes.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1604157985/Graphic.o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ${OBJECTDIR}/_ext/1445236994/MMB.o ${OBJECTDIR}/_ext/375547254/Primitive.o ${OBJECTDIR}/_ext/1604157985/Poly.o ${OBJECTDIR}/_ext/1604157985/Geometry4.o ${OBJECTDIR}/_ext/1604157985/Cubes.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1604157985/Graphic.o.d ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d ${OBJECTDIR}/_ext/1445236994/MMB.o.d ${OBJECTDIR}/_ext/375547254/Primitive.o.d ${OBJECTDIR}/_ext/1604157985/Poly.o.d ${OBJECTDIR}/_ext/1604157985/Geometry4.o.d ${OBJECTDIR}/_ext/1604157985/Cubes.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1604157985/Graphic.o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ${OBJECTDIR}/_ext/1445236994/MMB.o ${OBJECTDIR}/_ext/375547254/Primitive.o ${OBJECTDIR}/_ext/1604157985/Poly.o ${OBJECTDIR}/_ext/1604157985/Geometry4.o ${OBJECTDIR}/_ext/1604157985/Cubes.o

# Source Files
SOURCEFILES=../../Rubik/Graphic.c ../../Microchip/Graphics/DisplayDriver.c ../../MMB/MMB.c ../../Microchip/Graphics/Primitive.c ../../Rubik/Poly.c ../../Rubik/Geometry4.c ../../Rubik/Cubes.c


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Rubik.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

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
${OBJECTDIR}/_ext/1604157985/Graphic.o: ../../Rubik/Graphic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1604157985 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Graphic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Graphic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1604157985/Graphic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1604157985/Graphic.o.d" -o ${OBJECTDIR}/_ext/1604157985/Graphic.o ../../Rubik/Graphic.c   
	
${OBJECTDIR}/_ext/375547254/DisplayDriver.o: ../../Microchip/Graphics/DisplayDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" -o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ../../Microchip/Graphics/DisplayDriver.c   
	
${OBJECTDIR}/_ext/1445236994/MMB.o: ../../MMB/MMB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/MMB.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/MMB.o.d" -o ${OBJECTDIR}/_ext/1445236994/MMB.o ../../MMB/MMB.c   
	
${OBJECTDIR}/_ext/375547254/Primitive.o: ../../Microchip/Graphics/Primitive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Primitive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Primitive.o.d" -o ${OBJECTDIR}/_ext/375547254/Primitive.o ../../Microchip/Graphics/Primitive.c   
	
${OBJECTDIR}/_ext/1604157985/Poly.o: ../../Rubik/Poly.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1604157985 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Poly.o.d 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Poly.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1604157985/Poly.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1604157985/Poly.o.d" -o ${OBJECTDIR}/_ext/1604157985/Poly.o ../../Rubik/Poly.c   
	
${OBJECTDIR}/_ext/1604157985/Geometry4.o: ../../Rubik/Geometry4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1604157985 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Geometry4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Geometry4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1604157985/Geometry4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1604157985/Geometry4.o.d" -o ${OBJECTDIR}/_ext/1604157985/Geometry4.o ../../Rubik/Geometry4.c   
	
${OBJECTDIR}/_ext/1604157985/Cubes.o: ../../Rubik/Cubes.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1604157985 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Cubes.o.d 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Cubes.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1604157985/Cubes.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1604157985/Cubes.o.d" -o ${OBJECTDIR}/_ext/1604157985/Cubes.o ../../Rubik/Cubes.c   
	
else
${OBJECTDIR}/_ext/1604157985/Graphic.o: ../../Rubik/Graphic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1604157985 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Graphic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Graphic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1604157985/Graphic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1604157985/Graphic.o.d" -o ${OBJECTDIR}/_ext/1604157985/Graphic.o ../../Rubik/Graphic.c   
	
${OBJECTDIR}/_ext/375547254/DisplayDriver.o: ../../Microchip/Graphics/DisplayDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" -o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ../../Microchip/Graphics/DisplayDriver.c   
	
${OBJECTDIR}/_ext/1445236994/MMB.o: ../../MMB/MMB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/MMB.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/MMB.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/MMB.o.d" -o ${OBJECTDIR}/_ext/1445236994/MMB.o ../../MMB/MMB.c   
	
${OBJECTDIR}/_ext/375547254/Primitive.o: ../../Microchip/Graphics/Primitive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Primitive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Primitive.o.d" -o ${OBJECTDIR}/_ext/375547254/Primitive.o ../../Microchip/Graphics/Primitive.c   
	
${OBJECTDIR}/_ext/1604157985/Poly.o: ../../Rubik/Poly.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1604157985 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Poly.o.d 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Poly.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1604157985/Poly.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1604157985/Poly.o.d" -o ${OBJECTDIR}/_ext/1604157985/Poly.o ../../Rubik/Poly.c   
	
${OBJECTDIR}/_ext/1604157985/Geometry4.o: ../../Rubik/Geometry4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1604157985 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Geometry4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Geometry4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1604157985/Geometry4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1604157985/Geometry4.o.d" -o ${OBJECTDIR}/_ext/1604157985/Geometry4.o ../../Rubik/Geometry4.c   
	
${OBJECTDIR}/_ext/1604157985/Cubes.o: ../../Rubik/Cubes.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1604157985 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Cubes.o.d 
	@${RM} ${OBJECTDIR}/_ext/1604157985/Cubes.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1604157985/Cubes.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Rubik" -I"../../MMB" -I"../../Microchip/Include" -I"." -MMD -MF "${OBJECTDIR}/_ext/1604157985/Cubes.o.d" -o ${OBJECTDIR}/_ext/1604157985/Cubes.o ../../Rubik/Cubes.c   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Rubik.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Rubik.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,--defsym=_min_stack_size=1000,-L"../../Rubik",-L".",-Map="${DISTDIR}/Rubik.X.${IMAGE_TYPE}.map"
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Rubik.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Rubik.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_stack_size=1000,-L"../../Rubik",-L".",-Map="${DISTDIR}/Rubik.X.${IMAGE_TYPE}.map"
	${MP_CC_DIR}/xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Rubik.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
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
