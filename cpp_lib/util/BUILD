load("@rules_cc//cc:defs.bzl","cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
	name = "util",
	deps = [
		":singleton",
		"//cpp_lib/util/strings",
		"//cpp_lib/util/time",
		"//cpp_lib/util/math",
		"//cpp_lib/util/http",
	],
)

cc_library(
	name = "singleton",
	hdrs = [
		"singleton.h"
	],
)