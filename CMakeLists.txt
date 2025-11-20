cmake_minimum_required(VERSION 3.15)

project(Duckaverb VERSION 0.0.1)

# JUCE Setup using FetchContent
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(juce)

# Standard JUCE Plugin Setup
juce_add_plugin(Duckaverb
    COMPANY_NAME "h4ppy Labs"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE H4py
    PLUGIN_CODE Duck
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Duckaverb"
    BUNDLE_ID "com.h4ppylabs.duckaverb"
)

target_sources(Duckaverb
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

target_compile_features(Duckaverb PUBLIC cxx_std_17)

target_compile_definitions(Duckaverb PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Duckaverb
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

juce_generate_juce_header(Duckaverb)
