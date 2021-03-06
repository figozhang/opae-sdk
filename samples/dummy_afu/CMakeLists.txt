## Copyright(c) 2020, Intel Corporation
##
## Redistribution  and  use  in source  and  binary  forms,  with  or  without
## modification, are permitted provided that the following conditions are met:
##
## * Redistributions of  source code  must retain the  above copyright notice,
##   this list of conditions and the following disclaimer.
## * Redistributions in binary form must reproduce the above copyright notice,
##   this list of conditions and the following disclaimer in the documentation
##   and/or other materials provided with the distribution.
## * Neither the name  of Intel Corporation  nor the names of its contributors
##   may be used to  endorse or promote  products derived  from this  software
##   without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
## IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED.  IN NO EVENT  SHALL THE COPYRIGHT OWNER  OR CONTRIBUTORS BE
## LIABLE  FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
## CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT LIMITED  TO,  PROCUREMENT  OF
## SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE,  DATA, OR PROFITS;  OR BUSINESS
## INTERRUPTION)  HOWEVER CAUSED  AND ON ANY THEORY  OF LIABILITY,  WHETHER IN
## CONTRACT,  STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE  OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.

set(CLI11_URL
    https://github.com/CLIUtils/CLI11.git
    CACHE STRING "URL for CLI11 Project")
set(CLI11_TAG
    v1.9.1
    CACHE STRING "Tag to checkout for CLI11")

set(SPDLOG_URL
    https://github.com/gabime/spdlog.git
    CACHE STRING "URL for SPDLOG Project")
set(SPDLOG_TAG
    v1.7.0
    CACHE STRING "Tag to checkout for SPDLOG")

opae_external_project_add(PROJECT_NAME CLI11
                          GIT_URL ${CLI11_URL}
                          GIT_TAG ${CLI11_TAG}
                          NO_ADD_SUBDIRECTORY
)

opae_external_project_add(PROJECT_NAME spdlog
                          GIT_URL ${SPDLOG_URL}
                          GIT_TAG ${SPDLOG_TAG}
                          NO_ADD_SUBDIRECTORY
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CLI11_ROOT}/include
    ${spdlog_ROOT}/include
)

opae_add_executable(TARGET dummy_afu
    SOURCE dummy_afu.cpp
    LIBS
        opae-c
        opae-cxx-core
        ${libjson-c_LIBRARIES}
        ${libuuid_LIBRARIES}
    COMPONENT samplebin
)

target_compile_options(dummy_afu PUBLIC -Wno-unused-result)
target_include_directories(dummy_afu
    PRIVATE ${spdlog_ROOT}/include
)
