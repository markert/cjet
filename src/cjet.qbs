/*
 *The MIT License (MIT)
 *
 * Copyright (c) <2014> <Stephan Gatzka>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

import qbs 1.0

Project {
  name: "cjet"
  minimumQbsVersion: "1.4.0"

  qbsSearchPaths: "../qbs/"

  SubProject {
    filePath: "../qbs/hardening.qbs"
    Properties {
      name: "hardening settings"
    }
  }

  SubProject {
    filePath: "../qbs/gccClang.qbs"
    Properties {
      name: "GCC/Clang switches"
    }
  }

  CppApplication {
    name: "cjet"

    Depends { name: "gccClang" }
    Depends { name: "hardening" }
    Depends { name: "generateCjetConfig" }
    Depends { name: "generateOsConfig" }
    Depends { name: "generateVersion" }

    consoleApplication: true

    cpp.warningLevel: "all"
    cpp.treatWarningsAsErrors: true
    cpp.positionIndependentCode: false
    cpp.includePaths: [".", buildDirectory]
    cpp.visibility: "hidden"
    cpp.useRPaths: false
    cpp.cLanguageVersion: "c99"

    Group {
      name: "platform independent"
      files: [
        "*.c",
      ]
      cpp.cLanguageVersion: "c99"
    }

    Group {
      name: "cJSON"
      files: [
        "json/*.c",
      ]
      cpp.cLanguageVersion: "c99"
    }

    Group {
      name: "version file"
      files: [
        "version.h.in"
      ]
      fileTags: ["version_tag"]
    }

    Group {
      name: "cjet config file"
      files: [
        "cjet_config.h.in"
      ]
      fileTags: ["cjet_config_tag"]
    }


    Group {
      condition: qbs.targetOS.contains("linux")
      name: "linux config file"
      files: [
        "linux/config/os_config.h.in"
      ]
      fileTags: ["os_config_tag"]
    }

    Group {
      condition: qbs.targetOS.contains("linux")
      name: "linux specific"
      prefix: "linux/"
      files: [
        "*.c",
      ]
      cpp.defines: "_GNU_SOURCE"
    }
  }
}
