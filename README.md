# durian
<p style="text-align: center;">"Smells like rotten memory!"</p>

# What?
Durian is a tool for stress testing a `malloc` implementation. It can reveal the shortcomings or bugs of a memory manager just by running a few simple tests.

# Why?
I needed to write my own `malloc` a while back and noticed that there were little to no things one could do in order to debug a memory manager, so I decided to create this little tool.

# How?
First, compile the source by running `make`. Then, compile your memory manager as a shared library and run durian like this

```sh
$ durian --check-integrity --library ./yourmalloc.so
```

For a full list of options, run `durian --help`.

```sh
$ durian --help
$ Usage: durian [options]
$  Run various tests in order to determine if a malloc implementation is up to standard.
$ Options:
$  -l, --library          override malloc with another shared library
$  -f, --file             output file
$  -i, --check-integrity  check heap integrity
$  -b, --benchmark        benchmark allocation time
$  -h, --help             display this help message
```
