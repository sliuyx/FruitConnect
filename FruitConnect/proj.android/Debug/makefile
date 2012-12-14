################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include obj/local/armeabi/objs-debug/game_shared/hellocpp/subdir.mk
-include obj/local/armeabi/objs-debug/game_shared/__/__/Classes/subdir.mk
-include obj/local/armeabi/objs-debug/cocosdenshion_static/opensl/subdir.mk
-include obj/local/armeabi/objs-debug/cocosdenshion_static/jni/subdir.mk
-include obj/local/armeabi/objs-debug/cocosdenshion_static/subdir.mk
-include obj/local/armeabi/objs-debug/cocos_extension_static/network/subdir.mk
-include obj/local/armeabi/objs-debug/cocos_extension_static/GUI/CCScrollView/subdir.mk
-include obj/local/armeabi/objs-debug/cocos_extension_static/GUI/CCEditBox/subdir.mk
-include obj/local/armeabi/objs-debug/cocos_extension_static/GUI/CCControlExtension/subdir.mk
-include obj/local/armeabi/objs-debug/cocos_extension_static/CCBReader/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/touch_dispatcher/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/tilemap_parallax_nodes/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/textures/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/text_input_node/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/support/zip_support/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/support/image_support/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/support/data_support/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/support/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/sprite_nodes/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/shaders/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/script_support/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/platform/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/platform/android/jni/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/platform/android/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/particle_nodes/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/misc_nodes/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/menu_nodes/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/layers_scenes_transitions_nodes/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/label_nodes/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/keypad_dispatcher/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/kazmath/src/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/kazmath/src/GL/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/effects/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/cocoa/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/base_nodes/subdir.mk
-include obj/local/armeabi/objs-debug/cocos2dx_static/actions/subdir.mk
-include jni/hellocpp/subdir.mk
-include Classes/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: FruitConnect

# Tool invocations
FruitConnect: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	g++  -o"FruitConnect" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) FruitConnect
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
