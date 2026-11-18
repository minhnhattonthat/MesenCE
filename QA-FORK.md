# MesenCE QA fork

Small changes on top of upstream MesenCE for scripted test runs (`--testRunner` + a Lua script, driven by a harness
such as the translate-retro-game skill's `emu.py`). Behaviour, settings and command line are unchanged.

## Debugger RAM no longer scales with ROM size

Loading any Lua script creates the full debugger, which keeps tables with one entry per byte of every ROM and RAM:
access counters (40 bytes per byte) and the disassembly cache (12 bytes per byte). Upstream builds them with
`std::vector`, which writes every entry and so commits ~52 bytes of RAM per ROM byte: about 1.1 GB for an 18 MB GBA
ROM. The access counters were also memset on every state load and reset, committing them all again.

Here both tables are backed by demand-zero virtual memory (`Utilities/VirtualMemory.h`, `ZeroedArray<T>`): a page
takes physical RAM only once the game touches an address in it, and resetting the counters hands the pages back to
the OS instead of writing zeros. Address space and commit charge are still reserved for the full size; resident
memory (working set) follows what the game actually uses.

## Build

Same as upstream (see COMPILING.md). The upstream `Build Mesen` workflow runs on every push to this fork; releases
(`qa-N`) repackage its artifacts under upstream's asset names (`Mesen_qa-N_Windows.zip`, ...).

License: GPL-3.0, as upstream.
