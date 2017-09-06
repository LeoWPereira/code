""" 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 """ 
 #!/usr/bin/env python
#
# Copyright 2008, Google Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""A script to prepare version informtion for use the gtest Info.plist file.

  This script extracts the version information from the configure.ac file and
  uses it to generate a header file containing the same information. The
  #defines in this header file will be included in during the generation of
  the Info.plist of the framework, giving the correct value to the version
  shown in the Finder.

  This script makes the following assumptions (these are faults of the script,
  not problems with the Autoconf):
    1. The AC_INIT macro will be contained within the first 1024 characters
       of configure.ac
    2. The version string will be 3 integers separated by periods and will be
       surrounded by squre brackets, "[" and "]" (e.g. [1.0.1]). The first
       segment represents the major version, the second represents the minor
       version and the third represents the fix version.
    3. No ")" character exists between the opening "(" and closing ")" of
       AC_INIT, including in comments and character strings.
"""

import sys
import re

# Read the command line argument (the output directory for Version.h)
if (len(sys.argv) < 3):
  print "Usage: versiongenerate.py input_dir output_dir"
  sys.exit(1)
else:
  input_dir = sys.argv[1]
  output_dir = sys.argv[2]

# Read the first 1024 characters of the configure.ac file
config_file = open("%s/configure.ac" % input_dir, 'r')
buffer_size = 1024
opening_string = config_file.read(buffer_size)
config_file.close()

# Extract the version string from the AC_INIT macro
#   The following init_expression means:
#     Extract three integers separated by periods and surrounded by squre
#     brackets(e.g. "[1.0.1]") between "AC_INIT(" and ")". Do not be greedy
#     (*? is the non-greedy flag) since that would pull in everything between
#     the first "(" and the last ")" in the file.
version_expression = re.compile(r"AC_INIT\(.*?\[(\d+)\.(\d+)\.(\d+)\].*?\)",
                                re.DOTALL)
version_values = version_expression.search(opening_string)
major_version = version_values.group(1)
minor_version = version_values.group(2)
fix_version = version_values.group(3)

# Write the version information to a header file to be included in the
# Info.plist file.
file_data = """//
// DO NOT MODIFY THIS FILE (but you can delete it)
//
// This file is autogenerated by the versiongenerate.py script. This script
// is executed in a "Run Script" build phase when creating gtest.framework. This
// header file is not used during compilation of C-source. Rather, it simply
// defines some version strings for substitution in the Info.plist. Because of
// this, we are not not restricted to C-syntax nor are we using include guards.
//

#define GTEST_VERSIONINFO_SHORT %s.%s
#define GTEST_VERSIONINFO_LONG %s.%s.%s

""" % (major_version, minor_version, major_version, minor_version, fix_version)
version_file = open("%s/Version.h" % output_dir, 'w')
version_file.write(file_data)
version_file.close()
