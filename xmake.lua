set_version("1.0.0")
set_languages("c11")
add_rules("mode.debug", "mode.release")

includes("@builtin/xpack")

target("bf")
    set_kind("binary")
    add_files("bf.c")

xpack("bf")
    set_formats("nsis")
    set_title("Brainfuck interpreter")
    set_author("chen_qingyu")
    set_description("A simple Brainfuck interpreter implemented in C.")
    set_homepage("https://github.com/chen-qingyu/bf")
    set_licensefile("license")
    add_targets("bf")
