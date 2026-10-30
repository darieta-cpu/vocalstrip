cmake_minimum_required(VERSION 3.22)

# Universal binary (Intel + Apple Silicon) - must be set before project()
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "" FORCE)

project(VocalStrip VERSION 1.0.0)

if(APPLE)
    set(PLUGIN_FORMATS AU VST3)
else()
    set(PLUGIN_FORMATS VST3)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

# NOTE: PLUGIN_CODE must differ from Console Strip ("Cstp"), otherwise the two
# Audio Units collide. MANUFACTURER_CODE stays the same ("Hmst").
juce_add_plugin(VocalStrip
    COMPANY_NAME "DaniArieta"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE Hmst
    PLUGIN_CODE Vcst
    FORMATS ${PLUGIN_FORMATS}
    PRODUCT_NAME "Vocal Strip"
    COPY_PLUGIN_AFTER_BUILD FALSE
    AU_MAIN_TYPE kAudioUnitType_Effect
    VST3_CATEGORIES Fx)

target_sources(VocalStrip PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h)

target_compile_definitions(VocalStrip PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

# juce_dsp is needed for the oversampler used by the Drive stage
target_link_libraries(VocalStrip
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)
