load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "concurrent_map",
    hdrs = [
        "concurrent_map.h",
    ],
    deps = [
        "@tbb",
    ],
)

cc_library(
    name = "double_buffer",
    hdrs = [
        "double_buffer.h",
    ],
)