load("@rules_cc//cc:defs.bzl","cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "file",
    srcs = [
        "file.cc",
    ],
    hdrs = [
        "file.h",
    ],
    deps = [
        "//cpp_lib/util/strings",

        "@com_github_absl//absl/strings",
    ]
)