cmake_minimum_required(VERSION 3.22)

project(AkiraMulti VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to your JUCE checkout
add_subdirectory("/Users/gustavoakira/Python/JUCE/JUCE" JUCE)

juce_add_plugin(AkiraMulti
    COMPANY_NAME "G Akira"
    BUNDLE_ID "tech.gustavoakira.akiramulti"
    PLUGIN_MANUFACTURER_CODE Gakr
    PLUGIN_CODE AkMu

    FORMATS
        Standalone
        VST3
        AU

    PRODUCT_NAME "Akira Multi"
)

juce_generate_juce_header(AkiraMulti)

target_sources(AkiraMulti
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

target_compile_definitions(AkiraMulti
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(AkiraMulti
    PRIVATE
        juce::juce_dsp
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
