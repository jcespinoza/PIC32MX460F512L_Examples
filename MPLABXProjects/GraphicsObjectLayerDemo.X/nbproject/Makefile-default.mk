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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/GraphicsObjectLayerDemo.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/GraphicsObjectLayerDemo.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED="../../Graphics Object Layer Demo/TouchScreen.c" "../../Graphics Object Layer Demo/CustomControlDemo.c" "../../Graphics Object Layer Demo/Fonts.c" "../../Graphics Object Layer Demo/MainDemo.c" "../../Graphics Object Layer Demo/SideButtons.c" "../../Graphics Object Layer Demo/rtcc.c" "../../Graphics Object Layer Demo/Pictures C32.c" ../../Microchip/Graphics/Window.c ../../Microchip/Graphics/Button.c ../../Microchip/Graphics/Chart.c ../../Microchip/Graphics/CheckBox.c ../../Microchip/Graphics/EditBox.c ../../Microchip/Graphics/GOL.c ../../Microchip/Graphics/GOLFontDefault.c ../../Microchip/Graphics/Grid.c ../../Microchip/Graphics/GroupBox.c ../../Microchip/Graphics/ListBox.c ../../Microchip/Graphics/Meter.c ../../Microchip/Graphics/Picture.c ../../Microchip/Graphics/Primitive.c ../../Microchip/Graphics/ProgressBar.c ../../Microchip/Graphics/RadioButton.c ../../Microchip/Graphics/RoundDial.c ../../Microchip/Graphics/Slider.c ../../Microchip/Graphics/StaticText.c ../../Microchip/Graphics/DisplayDriver.c ../../Microchip/Graphics/DigitalMeter.c "../../Graphics Object Layer Demo/Monofont.c" ../../MMB/SerialEEPROM.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1790482850/TouchScreen.o ${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o ${OBJECTDIR}/_ext/1790482850/Fonts.o ${OBJECTDIR}/_ext/1790482850/MainDemo.o ${OBJECTDIR}/_ext/1790482850/SideButtons.o ${OBJECTDIR}/_ext/1790482850/rtcc.o "${OBJECTDIR}/_ext/1790482850/Pictures C32.o" ${OBJECTDIR}/_ext/375547254/Window.o ${OBJECTDIR}/_ext/375547254/Button.o ${OBJECTDIR}/_ext/375547254/Chart.o ${OBJECTDIR}/_ext/375547254/CheckBox.o ${OBJECTDIR}/_ext/375547254/EditBox.o ${OBJECTDIR}/_ext/375547254/GOL.o ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o ${OBJECTDIR}/_ext/375547254/Grid.o ${OBJECTDIR}/_ext/375547254/GroupBox.o ${OBJECTDIR}/_ext/375547254/ListBox.o ${OBJECTDIR}/_ext/375547254/Meter.o ${OBJECTDIR}/_ext/375547254/Picture.o ${OBJECTDIR}/_ext/375547254/Primitive.o ${OBJECTDIR}/_ext/375547254/ProgressBar.o ${OBJECTDIR}/_ext/375547254/RadioButton.o ${OBJECTDIR}/_ext/375547254/RoundDial.o ${OBJECTDIR}/_ext/375547254/Slider.o ${OBJECTDIR}/_ext/375547254/StaticText.o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ${OBJECTDIR}/_ext/375547254/DigitalMeter.o ${OBJECTDIR}/_ext/1790482850/Monofont.o ${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1790482850/TouchScreen.o.d ${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o.d ${OBJECTDIR}/_ext/1790482850/Fonts.o.d ${OBJECTDIR}/_ext/1790482850/MainDemo.o.d ${OBJECTDIR}/_ext/1790482850/SideButtons.o.d ${OBJECTDIR}/_ext/1790482850/rtcc.o.d "${OBJECTDIR}/_ext/1790482850/Pictures C32.o.d" ${OBJECTDIR}/_ext/375547254/Window.o.d ${OBJECTDIR}/_ext/375547254/Button.o.d ${OBJECTDIR}/_ext/375547254/Chart.o.d ${OBJECTDIR}/_ext/375547254/CheckBox.o.d ${OBJECTDIR}/_ext/375547254/EditBox.o.d ${OBJECTDIR}/_ext/375547254/GOL.o.d ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d ${OBJECTDIR}/_ext/375547254/Grid.o.d ${OBJECTDIR}/_ext/375547254/GroupBox.o.d ${OBJECTDIR}/_ext/375547254/ListBox.o.d ${OBJECTDIR}/_ext/375547254/Meter.o.d ${OBJECTDIR}/_ext/375547254/Picture.o.d ${OBJECTDIR}/_ext/375547254/Primitive.o.d ${OBJECTDIR}/_ext/375547254/ProgressBar.o.d ${OBJECTDIR}/_ext/375547254/RadioButton.o.d ${OBJECTDIR}/_ext/375547254/RoundDial.o.d ${OBJECTDIR}/_ext/375547254/Slider.o.d ${OBJECTDIR}/_ext/375547254/StaticText.o.d ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d ${OBJECTDIR}/_ext/375547254/DigitalMeter.o.d ${OBJECTDIR}/_ext/1790482850/Monofont.o.d ${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1790482850/TouchScreen.o ${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o ${OBJECTDIR}/_ext/1790482850/Fonts.o ${OBJECTDIR}/_ext/1790482850/MainDemo.o ${OBJECTDIR}/_ext/1790482850/SideButtons.o ${OBJECTDIR}/_ext/1790482850/rtcc.o ${OBJECTDIR}/_ext/1790482850/Pictures\ C32.o ${OBJECTDIR}/_ext/375547254/Window.o ${OBJECTDIR}/_ext/375547254/Button.o ${OBJECTDIR}/_ext/375547254/Chart.o ${OBJECTDIR}/_ext/375547254/CheckBox.o ${OBJECTDIR}/_ext/375547254/EditBox.o ${OBJECTDIR}/_ext/375547254/GOL.o ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o ${OBJECTDIR}/_ext/375547254/Grid.o ${OBJECTDIR}/_ext/375547254/GroupBox.o ${OBJECTDIR}/_ext/375547254/ListBox.o ${OBJECTDIR}/_ext/375547254/Meter.o ${OBJECTDIR}/_ext/375547254/Picture.o ${OBJECTDIR}/_ext/375547254/Primitive.o ${OBJECTDIR}/_ext/375547254/ProgressBar.o ${OBJECTDIR}/_ext/375547254/RadioButton.o ${OBJECTDIR}/_ext/375547254/RoundDial.o ${OBJECTDIR}/_ext/375547254/Slider.o ${OBJECTDIR}/_ext/375547254/StaticText.o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ${OBJECTDIR}/_ext/375547254/DigitalMeter.o ${OBJECTDIR}/_ext/1790482850/Monofont.o ${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o

# Source Files
SOURCEFILES=../../Graphics Object Layer Demo/TouchScreen.c ../../Graphics Object Layer Demo/CustomControlDemo.c ../../Graphics Object Layer Demo/Fonts.c ../../Graphics Object Layer Demo/MainDemo.c ../../Graphics Object Layer Demo/SideButtons.c ../../Graphics Object Layer Demo/rtcc.c ../../Graphics Object Layer Demo/Pictures C32.c ../../Microchip/Graphics/Window.c ../../Microchip/Graphics/Button.c ../../Microchip/Graphics/Chart.c ../../Microchip/Graphics/CheckBox.c ../../Microchip/Graphics/EditBox.c ../../Microchip/Graphics/GOL.c ../../Microchip/Graphics/GOLFontDefault.c ../../Microchip/Graphics/Grid.c ../../Microchip/Graphics/GroupBox.c ../../Microchip/Graphics/ListBox.c ../../Microchip/Graphics/Meter.c ../../Microchip/Graphics/Picture.c ../../Microchip/Graphics/Primitive.c ../../Microchip/Graphics/ProgressBar.c ../../Microchip/Graphics/RadioButton.c ../../Microchip/Graphics/RoundDial.c ../../Microchip/Graphics/Slider.c ../../Microchip/Graphics/StaticText.c ../../Microchip/Graphics/DisplayDriver.c ../../Microchip/Graphics/DigitalMeter.c ../../Graphics Object Layer Demo/Monofont.c ../../MMB/SerialEEPROM.c


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/GraphicsObjectLayerDemo.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

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
${OBJECTDIR}/_ext/1790482850/TouchScreen.o: ../../Graphics\ Object\ Layer\ Demo/TouchScreen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/TouchScreen.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/TouchScreen.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/TouchScreen.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/TouchScreen.o.d" -o ${OBJECTDIR}/_ext/1790482850/TouchScreen.o "../../Graphics Object Layer Demo/TouchScreen.c"   
	
${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o: ../../Graphics\ Object\ Layer\ Demo/CustomControlDemo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o.d" -o ${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o "../../Graphics Object Layer Demo/CustomControlDemo.c"   
	
${OBJECTDIR}/_ext/1790482850/Fonts.o: ../../Graphics\ Object\ Layer\ Demo/Fonts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/Fonts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/Fonts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/Fonts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/Fonts.o.d" -o ${OBJECTDIR}/_ext/1790482850/Fonts.o "../../Graphics Object Layer Demo/Fonts.c"   
	
${OBJECTDIR}/_ext/1790482850/MainDemo.o: ../../Graphics\ Object\ Layer\ Demo/MainDemo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/MainDemo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/MainDemo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/MainDemo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/MainDemo.o.d" -o ${OBJECTDIR}/_ext/1790482850/MainDemo.o "../../Graphics Object Layer Demo/MainDemo.c"   
	
${OBJECTDIR}/_ext/1790482850/SideButtons.o: ../../Graphics\ Object\ Layer\ Demo/SideButtons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/SideButtons.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/SideButtons.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/SideButtons.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/SideButtons.o.d" -o ${OBJECTDIR}/_ext/1790482850/SideButtons.o "../../Graphics Object Layer Demo/SideButtons.c"   
	
${OBJECTDIR}/_ext/1790482850/rtcc.o: ../../Graphics\ Object\ Layer\ Demo/rtcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/rtcc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/rtcc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/rtcc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/rtcc.o.d" -o ${OBJECTDIR}/_ext/1790482850/rtcc.o "../../Graphics Object Layer Demo/rtcc.c"   
	
${OBJECTDIR}/_ext/1790482850/Pictures\ C32.o: ../../Graphics\ Object\ Layer\ Demo/Pictures\ C32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/Pictures\ C32.o.d 
	@${RM} "${OBJECTDIR}/_ext/1790482850/Pictures C32.o" 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/Pictures C32.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/Pictures C32.o.d" -o "${OBJECTDIR}/_ext/1790482850/Pictures C32.o" "../../Graphics Object Layer Demo/Pictures C32.c"   
	
${OBJECTDIR}/_ext/375547254/Window.o: ../../Microchip/Graphics/Window.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Window.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Window.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Window.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Window.o.d" -o ${OBJECTDIR}/_ext/375547254/Window.o ../../Microchip/Graphics/Window.c   
	
${OBJECTDIR}/_ext/375547254/Button.o: ../../Microchip/Graphics/Button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Button.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Button.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Button.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Button.o.d" -o ${OBJECTDIR}/_ext/375547254/Button.o ../../Microchip/Graphics/Button.c   
	
${OBJECTDIR}/_ext/375547254/Chart.o: ../../Microchip/Graphics/Chart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Chart.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Chart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Chart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Chart.o.d" -o ${OBJECTDIR}/_ext/375547254/Chart.o ../../Microchip/Graphics/Chart.c   
	
${OBJECTDIR}/_ext/375547254/CheckBox.o: ../../Microchip/Graphics/CheckBox.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/CheckBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/CheckBox.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/CheckBox.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/CheckBox.o.d" -o ${OBJECTDIR}/_ext/375547254/CheckBox.o ../../Microchip/Graphics/CheckBox.c   
	
${OBJECTDIR}/_ext/375547254/EditBox.o: ../../Microchip/Graphics/EditBox.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/EditBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/EditBox.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/EditBox.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/EditBox.o.d" -o ${OBJECTDIR}/_ext/375547254/EditBox.o ../../Microchip/Graphics/EditBox.c   
	
${OBJECTDIR}/_ext/375547254/GOL.o: ../../Microchip/Graphics/GOL.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOL.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOL.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/GOL.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/GOL.o.d" -o ${OBJECTDIR}/_ext/375547254/GOL.o ../../Microchip/Graphics/GOL.c   
	
${OBJECTDIR}/_ext/375547254/GOLFontDefault.o: ../../Microchip/Graphics/GOLFontDefault.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d" -o ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o ../../Microchip/Graphics/GOLFontDefault.c   
	
${OBJECTDIR}/_ext/375547254/Grid.o: ../../Microchip/Graphics/Grid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Grid.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Grid.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Grid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Grid.o.d" -o ${OBJECTDIR}/_ext/375547254/Grid.o ../../Microchip/Graphics/Grid.c   
	
${OBJECTDIR}/_ext/375547254/GroupBox.o: ../../Microchip/Graphics/GroupBox.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/GroupBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/GroupBox.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/GroupBox.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/GroupBox.o.d" -o ${OBJECTDIR}/_ext/375547254/GroupBox.o ../../Microchip/Graphics/GroupBox.c   
	
${OBJECTDIR}/_ext/375547254/ListBox.o: ../../Microchip/Graphics/ListBox.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/ListBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/ListBox.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/ListBox.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/ListBox.o.d" -o ${OBJECTDIR}/_ext/375547254/ListBox.o ../../Microchip/Graphics/ListBox.c   
	
${OBJECTDIR}/_ext/375547254/Meter.o: ../../Microchip/Graphics/Meter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Meter.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Meter.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Meter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Meter.o.d" -o ${OBJECTDIR}/_ext/375547254/Meter.o ../../Microchip/Graphics/Meter.c   
	
${OBJECTDIR}/_ext/375547254/Picture.o: ../../Microchip/Graphics/Picture.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Picture.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Picture.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Picture.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Picture.o.d" -o ${OBJECTDIR}/_ext/375547254/Picture.o ../../Microchip/Graphics/Picture.c   
	
${OBJECTDIR}/_ext/375547254/Primitive.o: ../../Microchip/Graphics/Primitive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Primitive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Primitive.o.d" -o ${OBJECTDIR}/_ext/375547254/Primitive.o ../../Microchip/Graphics/Primitive.c   
	
${OBJECTDIR}/_ext/375547254/ProgressBar.o: ../../Microchip/Graphics/ProgressBar.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/ProgressBar.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/ProgressBar.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/ProgressBar.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/ProgressBar.o.d" -o ${OBJECTDIR}/_ext/375547254/ProgressBar.o ../../Microchip/Graphics/ProgressBar.c   
	
${OBJECTDIR}/_ext/375547254/RadioButton.o: ../../Microchip/Graphics/RadioButton.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/RadioButton.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/RadioButton.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/RadioButton.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/RadioButton.o.d" -o ${OBJECTDIR}/_ext/375547254/RadioButton.o ../../Microchip/Graphics/RadioButton.c   
	
${OBJECTDIR}/_ext/375547254/RoundDial.o: ../../Microchip/Graphics/RoundDial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/RoundDial.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/RoundDial.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/RoundDial.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/RoundDial.o.d" -o ${OBJECTDIR}/_ext/375547254/RoundDial.o ../../Microchip/Graphics/RoundDial.c   
	
${OBJECTDIR}/_ext/375547254/Slider.o: ../../Microchip/Graphics/Slider.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Slider.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Slider.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Slider.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Slider.o.d" -o ${OBJECTDIR}/_ext/375547254/Slider.o ../../Microchip/Graphics/Slider.c   
	
${OBJECTDIR}/_ext/375547254/StaticText.o: ../../Microchip/Graphics/StaticText.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/StaticText.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/StaticText.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/StaticText.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/StaticText.o.d" -o ${OBJECTDIR}/_ext/375547254/StaticText.o ../../Microchip/Graphics/StaticText.c   
	
${OBJECTDIR}/_ext/375547254/DisplayDriver.o: ../../Microchip/Graphics/DisplayDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" -o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ../../Microchip/Graphics/DisplayDriver.c   
	
${OBJECTDIR}/_ext/375547254/DigitalMeter.o: ../../Microchip/Graphics/DigitalMeter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/DigitalMeter.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/DigitalMeter.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/DigitalMeter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/DigitalMeter.o.d" -o ${OBJECTDIR}/_ext/375547254/DigitalMeter.o ../../Microchip/Graphics/DigitalMeter.c   
	
${OBJECTDIR}/_ext/1790482850/Monofont.o: ../../Graphics\ Object\ Layer\ Demo/Monofont.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/Monofont.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/Monofont.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/Monofont.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/Monofont.o.d" -o ${OBJECTDIR}/_ext/1790482850/Monofont.o "../../Graphics Object Layer Demo/Monofont.c"   
	
${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o: ../../MMB/SerialEEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o.d" -o ${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o ../../MMB/SerialEEPROM.c   
	
else
${OBJECTDIR}/_ext/1790482850/TouchScreen.o: ../../Graphics\ Object\ Layer\ Demo/TouchScreen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/TouchScreen.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/TouchScreen.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/TouchScreen.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/TouchScreen.o.d" -o ${OBJECTDIR}/_ext/1790482850/TouchScreen.o "../../Graphics Object Layer Demo/TouchScreen.c"   
	
${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o: ../../Graphics\ Object\ Layer\ Demo/CustomControlDemo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o.d" -o ${OBJECTDIR}/_ext/1790482850/CustomControlDemo.o "../../Graphics Object Layer Demo/CustomControlDemo.c"   
	
${OBJECTDIR}/_ext/1790482850/Fonts.o: ../../Graphics\ Object\ Layer\ Demo/Fonts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/Fonts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/Fonts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/Fonts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/Fonts.o.d" -o ${OBJECTDIR}/_ext/1790482850/Fonts.o "../../Graphics Object Layer Demo/Fonts.c"   
	
${OBJECTDIR}/_ext/1790482850/MainDemo.o: ../../Graphics\ Object\ Layer\ Demo/MainDemo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/MainDemo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/MainDemo.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/MainDemo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/MainDemo.o.d" -o ${OBJECTDIR}/_ext/1790482850/MainDemo.o "../../Graphics Object Layer Demo/MainDemo.c"   
	
${OBJECTDIR}/_ext/1790482850/SideButtons.o: ../../Graphics\ Object\ Layer\ Demo/SideButtons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/SideButtons.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/SideButtons.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/SideButtons.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/SideButtons.o.d" -o ${OBJECTDIR}/_ext/1790482850/SideButtons.o "../../Graphics Object Layer Demo/SideButtons.c"   
	
${OBJECTDIR}/_ext/1790482850/rtcc.o: ../../Graphics\ Object\ Layer\ Demo/rtcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/rtcc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/rtcc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/rtcc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/rtcc.o.d" -o ${OBJECTDIR}/_ext/1790482850/rtcc.o "../../Graphics Object Layer Demo/rtcc.c"   
	
${OBJECTDIR}/_ext/1790482850/Pictures\ C32.o: ../../Graphics\ Object\ Layer\ Demo/Pictures\ C32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/Pictures\ C32.o.d 
	@${RM} "${OBJECTDIR}/_ext/1790482850/Pictures C32.o" 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/Pictures C32.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/Pictures C32.o.d" -o "${OBJECTDIR}/_ext/1790482850/Pictures C32.o" "../../Graphics Object Layer Demo/Pictures C32.c"   
	
${OBJECTDIR}/_ext/375547254/Window.o: ../../Microchip/Graphics/Window.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Window.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Window.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Window.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Window.o.d" -o ${OBJECTDIR}/_ext/375547254/Window.o ../../Microchip/Graphics/Window.c   
	
${OBJECTDIR}/_ext/375547254/Button.o: ../../Microchip/Graphics/Button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Button.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Button.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Button.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Button.o.d" -o ${OBJECTDIR}/_ext/375547254/Button.o ../../Microchip/Graphics/Button.c   
	
${OBJECTDIR}/_ext/375547254/Chart.o: ../../Microchip/Graphics/Chart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Chart.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Chart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Chart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Chart.o.d" -o ${OBJECTDIR}/_ext/375547254/Chart.o ../../Microchip/Graphics/Chart.c   
	
${OBJECTDIR}/_ext/375547254/CheckBox.o: ../../Microchip/Graphics/CheckBox.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/CheckBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/CheckBox.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/CheckBox.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/CheckBox.o.d" -o ${OBJECTDIR}/_ext/375547254/CheckBox.o ../../Microchip/Graphics/CheckBox.c   
	
${OBJECTDIR}/_ext/375547254/EditBox.o: ../../Microchip/Graphics/EditBox.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/EditBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/EditBox.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/EditBox.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/EditBox.o.d" -o ${OBJECTDIR}/_ext/375547254/EditBox.o ../../Microchip/Graphics/EditBox.c   
	
${OBJECTDIR}/_ext/375547254/GOL.o: ../../Microchip/Graphics/GOL.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOL.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOL.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/GOL.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/GOL.o.d" -o ${OBJECTDIR}/_ext/375547254/GOL.o ../../Microchip/Graphics/GOL.c   
	
${OBJECTDIR}/_ext/375547254/GOLFontDefault.o: ../../Microchip/Graphics/GOLFontDefault.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/GOLFontDefault.o.d" -o ${OBJECTDIR}/_ext/375547254/GOLFontDefault.o ../../Microchip/Graphics/GOLFontDefault.c   
	
${OBJECTDIR}/_ext/375547254/Grid.o: ../../Microchip/Graphics/Grid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Grid.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Grid.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Grid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Grid.o.d" -o ${OBJECTDIR}/_ext/375547254/Grid.o ../../Microchip/Graphics/Grid.c   
	
${OBJECTDIR}/_ext/375547254/GroupBox.o: ../../Microchip/Graphics/GroupBox.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/GroupBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/GroupBox.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/GroupBox.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/GroupBox.o.d" -o ${OBJECTDIR}/_ext/375547254/GroupBox.o ../../Microchip/Graphics/GroupBox.c   
	
${OBJECTDIR}/_ext/375547254/ListBox.o: ../../Microchip/Graphics/ListBox.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/ListBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/ListBox.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/ListBox.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/ListBox.o.d" -o ${OBJECTDIR}/_ext/375547254/ListBox.o ../../Microchip/Graphics/ListBox.c   
	
${OBJECTDIR}/_ext/375547254/Meter.o: ../../Microchip/Graphics/Meter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Meter.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Meter.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Meter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Meter.o.d" -o ${OBJECTDIR}/_ext/375547254/Meter.o ../../Microchip/Graphics/Meter.c   
	
${OBJECTDIR}/_ext/375547254/Picture.o: ../../Microchip/Graphics/Picture.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Picture.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Picture.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Picture.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Picture.o.d" -o ${OBJECTDIR}/_ext/375547254/Picture.o ../../Microchip/Graphics/Picture.c   
	
${OBJECTDIR}/_ext/375547254/Primitive.o: ../../Microchip/Graphics/Primitive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Primitive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Primitive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Primitive.o.d" -o ${OBJECTDIR}/_ext/375547254/Primitive.o ../../Microchip/Graphics/Primitive.c   
	
${OBJECTDIR}/_ext/375547254/ProgressBar.o: ../../Microchip/Graphics/ProgressBar.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/ProgressBar.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/ProgressBar.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/ProgressBar.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/ProgressBar.o.d" -o ${OBJECTDIR}/_ext/375547254/ProgressBar.o ../../Microchip/Graphics/ProgressBar.c   
	
${OBJECTDIR}/_ext/375547254/RadioButton.o: ../../Microchip/Graphics/RadioButton.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/RadioButton.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/RadioButton.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/RadioButton.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/RadioButton.o.d" -o ${OBJECTDIR}/_ext/375547254/RadioButton.o ../../Microchip/Graphics/RadioButton.c   
	
${OBJECTDIR}/_ext/375547254/RoundDial.o: ../../Microchip/Graphics/RoundDial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/RoundDial.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/RoundDial.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/RoundDial.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/RoundDial.o.d" -o ${OBJECTDIR}/_ext/375547254/RoundDial.o ../../Microchip/Graphics/RoundDial.c   
	
${OBJECTDIR}/_ext/375547254/Slider.o: ../../Microchip/Graphics/Slider.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/Slider.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/Slider.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/Slider.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/Slider.o.d" -o ${OBJECTDIR}/_ext/375547254/Slider.o ../../Microchip/Graphics/Slider.c   
	
${OBJECTDIR}/_ext/375547254/StaticText.o: ../../Microchip/Graphics/StaticText.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/StaticText.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/StaticText.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/StaticText.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/StaticText.o.d" -o ${OBJECTDIR}/_ext/375547254/StaticText.o ../../Microchip/Graphics/StaticText.c   
	
${OBJECTDIR}/_ext/375547254/DisplayDriver.o: ../../Microchip/Graphics/DisplayDriver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/DisplayDriver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/DisplayDriver.o.d" -o ${OBJECTDIR}/_ext/375547254/DisplayDriver.o ../../Microchip/Graphics/DisplayDriver.c   
	
${OBJECTDIR}/_ext/375547254/DigitalMeter.o: ../../Microchip/Graphics/DigitalMeter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/375547254 
	@${RM} ${OBJECTDIR}/_ext/375547254/DigitalMeter.o.d 
	@${RM} ${OBJECTDIR}/_ext/375547254/DigitalMeter.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/375547254/DigitalMeter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/375547254/DigitalMeter.o.d" -o ${OBJECTDIR}/_ext/375547254/DigitalMeter.o ../../Microchip/Graphics/DigitalMeter.c   
	
${OBJECTDIR}/_ext/1790482850/Monofont.o: ../../Graphics\ Object\ Layer\ Demo/Monofont.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1790482850 
	@${RM} ${OBJECTDIR}/_ext/1790482850/Monofont.o.d 
	@${RM} ${OBJECTDIR}/_ext/1790482850/Monofont.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1790482850/Monofont.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1790482850/Monofont.o.d" -o ${OBJECTDIR}/_ext/1790482850/Monofont.o "../../Graphics Object Layer Demo/Monofont.c"   
	
${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o: ../../MMB/SerialEEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1445236994 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -O1 -I"../../Graphics Object Layer Demo" -I"../../MMB" -I"../../Microchip/Include" -I"../../Microchip/Include/Graphics" -I"." -MMD -MF "${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o.d" -o ${OBJECTDIR}/_ext/1445236994/SerialEEPROM.o ../../MMB/SerialEEPROM.c   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/GraphicsObjectLayerDemo.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION) -Os -o dist/${CND_CONF}/${IMAGE_TYPE}/GraphicsObjectLayerDemo.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,--defsym=_min_heap_size=8192,--defsym=_min_stack_size=2048,-L"../../Graphics Object Layer Demo",-L".",-Map="${DISTDIR}/GraphicsObjectLayerDemo.X.${IMAGE_TYPE}.map"
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/GraphicsObjectLayerDemo.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION) -Os -o dist/${CND_CONF}/${IMAGE_TYPE}/GraphicsObjectLayerDemo.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=8192,--defsym=_min_stack_size=2048,-L"../../Graphics Object Layer Demo",-L".",-Map="${DISTDIR}/GraphicsObjectLayerDemo.X.${IMAGE_TYPE}.map"
	${MP_CC_DIR}/xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/GraphicsObjectLayerDemo.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
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
