load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "reloader",
    srcs = [
        "reloadable_file.cc",
        "watchdog.cc",
    ],
    hdrs = [
        "error_def.h",
        "reloadable_conf.h",
        "reloadable_file.h",
        "reloadable_json.h",
        "watchdog.h",
    ],
    deps = [
        "//cpp_lib/container:double_buffer",
        "//cpp_lib/error",
        "//cpp_lib/util",
        "//cpp_lib/serialize:json",
    ],
)