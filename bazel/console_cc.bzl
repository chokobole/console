load("@com_chokobole_bazel_utils//:cxxopts.bzl", "cxx14")
load("@com_chokobole_bazel_utils//:dsym.bzl", "dsym")
load("@com_chokobole_bazel_utils//:copts.bzl", "safest_code_copts")
load("@com_chokobole_bazel_utils//:linkopts.bzl", "safest_code_linkopts")

def console_copts():
    return cxx14()

def _console_copts():
    return console_copts() + safest_code_copts()

def _linkopts():
    return safest_code_linkopts()

def console_cc_library(
        name,
        copts = _console_copts(),
        linkopts = _linkopts(),
        **kwargs):
    native.cc_library(
        name = name,
        copts = copts,
        linkopts = linkopts,
        **kwargs
    )

def console_cc_binary(
        name,
        copts = _console_copts(),
        linkopts = _linkopts(),
        **kwargs):
    native.cc_binary(
        name = name,
        copts = copts,
        linkopts = linkopts,
        **kwargs
    )

    dsym(name = name)

def console_cc_test(
        name,
        copts = _console_copts(),
        linkopts = _linkopts(),
        **kwargs):
    native.cc_test(
        name = name,
        copts = copts,
        linkopts = linkopts,
        **kwargs
    )

    dsym(
        name = name,
        testonly = 1,
    )
