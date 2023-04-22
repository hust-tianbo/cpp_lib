load("@bazel_tools//tools/build_defs/repo:git.bzl","git_repository")
load("@bazel_tools//tools/build_defs/repo:git.bzl","new_git_repository")

git_repository(
    name = "com_github_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    tag = "20211102.0",
)

new_local_repository(
    name = "tbb",
    path = "/opt/intel/tbb",
    build_file = "//third_party/tbb:tbb.BUILD",
)