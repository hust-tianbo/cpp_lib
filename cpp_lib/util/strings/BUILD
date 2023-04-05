load("@rules_cc//cc:defs.bzl","cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "strings",
    srcs = glob(
        [
            "*.cc",
            "*.cpp",
        ],
        exclude = [
            "*_test.cpp",
            "*_test.cc",
        ],
    ),
    hdrs = glob(
        [
            "*.h",
            "*.hpp",
        ],
    ),
    deps = [
        "@com_github_absl//absl/strings",
        "@com_github_absl//absl/time",
    ]
)