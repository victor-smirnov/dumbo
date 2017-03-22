
// Copyright 2017 Victor Smirnov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#pragma once

#include <cstring>
#include <type_traits>

namespace dumbo {
namespace v1 {
namespace tools {

template <typename TypeTo, typename TypeFrom>
TypeTo* ptr_cast(TypeFrom* ptr)
{
    static_assert(std::is_pointer<TypeFrom*>::value, "");
    static_assert(std::is_pointer<TypeTo*>::value, "");
    
    TypeTo* result;
    
    std::memcpy(result, ptr, sizeof(ptr));
    
    return result;
}


}}}
