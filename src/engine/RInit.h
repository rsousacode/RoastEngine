#include "../RDDefs.h"
#include "cpptoml.h"

namespace RInit {

    // Initial Setup Create Information
    // At this stage try parse initial configurations
    // or just use the default settings

    RDTypeComp
    GetSettingsRDType(const std::shared_ptr<cpptoml::table>& toml) {
        RDType type = RE_VULKAN;

        auto val = toml->get_as<std::string>("engine");

        if(val) {
            auto s = val->data();
            if(strcmp("Vulkan", s) == 0) {
                type = RE_VULKAN; goto ret;
            }
            if(strcmp("OpenGL", s) == 0) {
                type = RE_OPENGL; goto ret;
            }
            if(strcmp("Metal", s) == 0) {
                type = RE_METAL; goto ret;
            }
        }

    ret:
        RDTypeComp comp = {
                .Type = type
        };

        return comp;
    }

    inline SetupCreateInfo
    GetEngineSetupCreateInfo() {

        // Create default initialization information
        SetupCreateInfo createInfo{
                .typeCompatibility      =   {.Type = RE_VULKAN},
                .windowTitle        =    "Roast Engine",
                .majorVersion       =    1,
                .minorVersion       =    0,
                .branch             =    "",
                .windowWidth        =    1500,
                .windowHeight       =    900,
        };


        auto init = cpptoml::parse_file("../src/settings.toml");

        // Versioning
        // Engine Title
        auto title = init->get_as<std::string>("title");
        if (title) createInfo.windowTitle = title->data();

        // Major Version
        auto majorVersion = init->get_as<int>("major");
        if (majorVersion) createInfo.majorVersion = majorVersion.value_or(createInfo.majorVersion);

        // Minor Version
        auto minorVersion = init->get_as<int>("minor");
        if (minorVersion) createInfo.minorVersion = minorVersion.value_or(createInfo.minorVersion);

        // Branch
        auto branch = init->get_as<std::string>("branch");
        if (branch) createInfo.branch = branch->data();

        // Window Information
        // width
        auto width = init->get_as<int>("windowWidth");
        if (width) createInfo.windowWidth = width.value_or(createInfo.windowWidth);

        // height
        auto height = init->get_as<int>("windowHeight");
        if (height) createInfo.windowHeight = width.value_or(createInfo.windowHeight);

        createInfo.typeCompatibility = GetSettingsRDType(init);

        return createInfo;
    }

}