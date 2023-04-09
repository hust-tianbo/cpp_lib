load("@rules_cc//cc:defs.bzl","cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "http",
    srcs = [
        "url.cc",
    ],
    hdrs = [
        "url.h",
    ],
    deps = [
        "//cpp_lib/util/strings",

        "@com_github_absl//absl/strings",
    ]
)