# sctime

sctime is a Qt/C++ application for recording working time ("Zeiterfassung")
booked against a hierarchy of departments, accounts, and sub-accounts, plus
on-call/standby duty ("Bereitschaft") and legally-required break-time
tracking. It builds as a native desktop app (Linux, Windows, macOS) and as a
WebAssembly app that runs in the browser.

## Documentation

- **Developers**: start at [doc/README.md](doc/README.md) — an up-to-date
  architecture overview with diagrams, build instructions for every target,
  and a glossary of the domain's German terminology.
- **Users**: in-app help is available from the "Help" menu (source in
  [help/](help/)).

## Building

See [doc/08-build-targets.md](doc/08-build-targets.md) for full instructions,
including the WebAssembly build and platform-specific requirements. Quick
start for a native desktop build:

```bash
qmake -r sctime.pro
make
src/sctime
```

## License

GPLv3 — see [COPYING](COPYING).

## Contact

[Github issues](https://github.com/scVENUS/sctime/issues)
