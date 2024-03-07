set_version("1.1.0")
set_languages("c11")
add_rules("mode.debug", "mode.release")

includes("@builtin/xpack")

target("bfi")
    set_kind("binary")
    add_files("bfi.c")

xpack("BFI")
    set_formats("nsis", "zip")
    set_title("Brainfuck interpreter")
    set_author("Qingyu Chen")
    set_description("A simple Brainfuck interpreter implemented in C.")
    set_homepage("https://github.com/chen-qingyu/bfi")
    set_licensefile("license")
    add_targets("bfi")
    add_installfiles("license", "readme.md", "hello.bf")
