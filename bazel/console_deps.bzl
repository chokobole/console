load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def console_deps():
    if not native.existing_rule("com_google_googletest"):
        http_archive(
            name = "com_google_googletest",
            sha256 = "353ab86e35cea1cd386115279cf4b16695bbf21b897bfbf2721cf4cb5f64ade8",
            strip_prefix = "googletest-997d343dd680e541ef96ce71ee54a91daf2577a0",
            urls = [
                "https://mirror.bazel.build/github.com/google/googletest/archive/997d343dd680e541ef96ce71ee54a91daf2577a0.zip",
                "https://github.com/google/googletest/archive/997d343dd680e541ef96ce71ee54a91daf2577a0.zip",
            ],
        )

    if not native.existing_rule("com_chokobole_color"):
        http_archive(
            name = "com_chokobole_color",
            sha256 = "14d43ab7723db5f70586ab993d21467489307da18f8fe5e28aa3ee335a247f3b",
            strip_prefix = "color-703ae9e1a30868cecd6bb0962c475a15c656b109",
            urls = [
                "https://github.com/chokobole/color/archive/703ae9e1a30868cecd6bb0962c475a15c656b109.zip",
            ],
        )
