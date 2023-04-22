load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "cache",
    hdrs = glob([
        "*.h",
    ]),
    deps = [
        "//cpp_lib/coro",
        "@tbb",
    ],
)