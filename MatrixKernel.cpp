/*
 * Copyright (C) 2017 The Android Open Source Project
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

#include "MatrixKernel.h"

namespace android {
namespace vintf {

bool MatrixKernel::operator==(const MatrixKernel &other) const {
    if (mMinLts != other.mMinLts)
        return false;
    if (mConfigs != other.mConfigs)
        return false;
    return true;
}

void MatrixKernel::setSourceMatrixLevel(Level level) {
    mSourceMatrixLevel = level;
}

Level MatrixKernel::getSourceMatrixLevel() const {
    return mSourceMatrixLevel;
}

} // namespace vintf
} // namespace android
