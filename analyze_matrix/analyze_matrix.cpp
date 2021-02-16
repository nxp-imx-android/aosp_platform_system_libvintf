/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <optional>
#include <set>

#include <android-base/logging.h>
#include <gflags/gflags.h>
#include <hidl-util/FqInstance.h>
#include <vintf/FileSystem.h>
#include <vintf/parse_string.h>
#include <vintf/parse_xml.h>

namespace android {
namespace vintf {

namespace {

template <typename T>
std::optional<T> readObject(const std::string& path, const XmlConverter<T>& converter) {
    std::string xml;
    std::string error;
    status_t err = details::FileSystemImpl().fetch(path, &xml, &error);
    if (err != OK) {
        LOG(ERROR) << "Cannot read '" << path << "': " << error;
        return std::nullopt;
    }
    auto ret = std::make_optional<T>();
    if (!converter(&ret.value(), xml, &error)) {
        LOG(ERROR) << "Cannot parse '" << path << "': " << error;
        return std::nullopt;
    }
    return ret;
}

template <typename F>
std::set<std::string> getDescription(const CompatibilityMatrix& mat, F descriptionFn) {
    std::set<std::string> set;
    mat.forEachInstance([&set, descriptionFn](const auto& matrixInstance) {
        for (auto minorVer = matrixInstance.versionRange().minMinor;
             minorVer >= matrixInstance.versionRange().minMinor &&
             minorVer <= matrixInstance.versionRange().maxMinor;
             ++minorVer) {
            Version version{matrixInstance.versionRange().majorVer, minorVer};
            set.insert(std::invoke(descriptionFn, matrixInstance, version));
        }
        return true;  // continue
    });
    return set;
}

}  // namespace

}  // namespace vintf
}  // namespace android

DEFINE_string(input, "", "Input compatibility matrix file");
static bool ValidateInput(const char* /* flagname */, const std::string& value) {
    return !value.empty();
}
DEFINE_validator(input, &ValidateInput);

DEFINE_bool(level, false, "Write level (FCM version) of the compatibility matrix.");
DEFINE_bool(interfaces, false, "Write strings like \"android.hardware.foo@1.0::IFoo\".");
DEFINE_bool(instances, false, "Write strings like \"android.hardware.foo@1.0::IFoo/default\".");

int main(int argc, char** argv) {
    using namespace android::vintf;

    gflags::ParseCommandLineFlags(&argc, &argv, true /* remove flags */);

    auto mat = readObject(FLAGS_input, gCompatibilityMatrixConverter);
    if (!mat) {
        return 1;
    }

    bool written = false;

    if (FLAGS_level) {
        if (mat->level() == Level::UNSPECIFIED) {
            LOG(WARNING) << "FCM version is unspecified.";
        }
        std::cout << mat->level() << std::endl;

        written = true;
    }

    if (FLAGS_interfaces) {
        auto interfaces = getDescription(*mat, &MatrixInstance::interfaceDescription);
        if (interfaces.empty()) {
            LOG(WARNING) << "No interfaces are found.";
        }

        for (const auto& interface : interfaces) {
            std::cout << interface << std::endl;
        }

        written = true;
    }

    if (FLAGS_instances) {
        auto instances = getDescription(*mat, &MatrixInstance::description);
        if (instances.empty()) {
            LOG(WARNING) << "No instances are found.";
        }

        for (const auto& instance : instances) {
            std::cout << instance << std::endl;
        }

        written = true;
    }

    if (!written) {
        LOG(ERROR) << "No output format is set.";
        return 1;
    }

    return 0;
}
