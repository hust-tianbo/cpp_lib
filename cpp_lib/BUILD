load("@rules_cc//cc:defs.bzl","cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
	name = "core",
	deps = [
		"//cpp_lib/util",
		"//cpp_lib/error",
		"//cpp_lib/coro",
		"//cpp_lib/reloader",
		"//cpp_lib/serialize:json",
		"//cpp_lib/container:concurrent_map",
		"//cpp_lib/container:double_buffer",
	],
)