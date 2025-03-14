cmake_minimum_required(VERSION 3.15)

# Set project name and version
project(GainPlugin VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE as a subdirectory
add_subdirectory(${JUCE_ROOT} JUCE)

# Initialize JUCE
juce_add_plugin(GainPlugin
    VERSION 1.0.0
    COMPANY_NAME "YourCompany"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE Gain
    FORMATS VST3
    PRODUCT_NAME "GainPlugin"
)

# Set source files
target_sources(GainPlugin PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

# Add JUCE modules
target_compile_definitions(GainPlugin
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# Link JUCE modules
target_link_libraries(GainPlugin
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Set VST3 SDK path if provided
if(DEFINED VST3_SDK_PATH)
    target_include_directories(GainPlugin PRIVATE ${VST3_SDK_PATH})
endif()

# Add macOS specific settings
if(APPLE)
    # Set deployment target to macOS 10.13 (High Sierra) or later
    set_target_properties(GainPlugin PROPERTIES
        XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.13"
    )
    
    # Enable ARM64 architecture for Apple Silicon support
    set_target_properties(GainPlugin PROPERTIES
        OSX_ARCHITECTURES "arm64;x86_64"
    )
endif() 

# Copy Resources directory to the build output directory
add_custom_command(TARGET GainPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    $<TARGET_FILE_DIR:GainPlugin>/Resources
    COMMENT "Copying Resources directory to build output"
)

# Also copy the image file directly to the build output directory
add_custom_command(TARGET GainPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/Resources/GainPlugin.png
    $<TARGET_FILE_DIR:GainPlugin>/GainPlugin.png
    COMMENT "Copying image file directly to build output"
)

# Copy to parent directory as well (for some plugin formats)
add_custom_command(TARGET GainPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/Resources/GainPlugin.png
    $<TARGET_FILE_DIR:GainPlugin>/../GainPlugin.png
    COMMENT "Copying image file to parent directory"
) 