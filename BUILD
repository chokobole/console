load("@com_github_bazelbuild_buildtools//buildifier:def.bzl", "buildifier")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_windows")
load("@com_chokobole_bazel_utils//:linkopts.bzl", "safest_code_linkopts")
load("//bazel:console_cc.bzl", "console_cc_library", "console_cc_test")

exports_files(["LICENSE"])

console_cc_library(
    name = "console",
    srcs = [
        "console/animation.cc",
        "console/autocompletion.cc",
        "console/console.cc",
        "console/flag.cc",
        "console/sgr_parameters.cc",
        "console/stream.cc",
    ],
    hdrs = [
        "console/animation.h",
        "console/autocompletion.h",
        "console/console.h",
        "console/export.h",
        "console/flag.h",
        "console/flag_forward.h",
        "console/flag_value_traits.h",
        "console/sgr_parameters.h",
        "console/sgr_parameters_list.h",
        "console/stream.h",
    ],
    linkopts = if_windows([
        "version.lib",
    ]) + safest_code_linkopts(),
    visibility = ["//visibility:public"],
    deps = [
        "@com_chokobole_color//:color",
    ],
)

console_cc_test(
    name = "console_unittests",
    srcs = [
        "console/animation_unittest.cc",
        "console/flag_unittest.cc",
    ],
    deps = [
        ":console",
        "@com_google_googletest//:gtest_main",
    ],
)

buildifier(
    name = "buildifier",
)
