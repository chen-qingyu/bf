# Brief

A simple [Brainfuck](https://esolangs.org/wiki/Brainfuck) interpreter implemented in C.

# Usage

This software is available in Microsoft's [WinGet](https://github.com/microsoft/winget-cli), if you are using Windows 10 or later, install it directly from `winget install ChenQingYu.BF`. Or, compile and install it as you please.

```
Usage: bf [options] <filename>
  options:
    -c, --comment  Enable comment feature: ';'.
    -d, --debug    Enable debug feature: '#'.
    -v, --version  Show program version and exit.
    -h, --help     Show this help message and exit.
```

# Example

```
$ cat hello.bf
++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.

$ bf hello.bf
Hello World!

$ cat test.bf
; Use `bf -cd test.bf` to run this.
+>++>+++ ; Set the values of the first three cells to 1, 2, and 3 respectively.
# ; Print the debug information.

$ bf -cd test.bf

-- -- -- -- -- -- 01 02 03 00 00 00 00 00 00 00
                        ^^
                         2
```
