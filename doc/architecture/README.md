---
package: st_facilities
summary: Grab-bag of shared Science Tools utilities — file/list/env helpers, FITS convenience functions, FITS image/table readers, interpolation, 1-D quadrature (dgaus8), GSL root finding and a CPU timer.
layer: io
depends_on: [astro, tip, facilities, cfitsio, GSL, f2c]
test_depends_on: [CppUnit]
used_by: [Likelihood, irfs, dataSubselector, evtbin, SolarSystemTools, observationSim, pyExposure, pyLikelihood, rspgen, timeSystem, celestialSources]
last_reviewed_commit: d9fc286
---

# st_facilities Architecture

**st_facilities** is the shared toolbox of the Science Tools. It has no
single theme. Each class is a small, mostly static utility that grew up
because two or more packages needed the same thing: expanding `@filelist`
arguments, checking that a file exists, copying FITS files and writing
checksums, interpolating tables, integrating a 1-D function, finding a
root. About 15 packages link it, and `Likelihood` and `irfs` lean on it
the most.

Because the utilities are unrelated, this document describes **each
utility's contract** (what it promises, what it silently does) rather than
a single data flow.

## Purpose and scope

**Does:**
- File-system and environment helpers: file existence (`Util::fileExists`,
  `file_ok`), reading text files as lines (`readLines`), `@list` expansion
  (`FileSys::expandFileList`, `Util::resolve_fits_files`), environment
  variable expansion (`Env`), and package data and XML paths
  (`Environment`, a front end to `facilities::commonUtilities`).
- FITS conveniences on top of tip/cfitsio: reading a column or vector cell
  (`FitsUtil::getTableVector`, `getRecordVector`), HDU and column names,
  checksums and the `FILENAME` keyword, filtered whole-file copy (`fcopy`),
  and date keywords (`Util::writeDateKeywords`).
- Small readers: a 2-/3-D FITS image with simple WCS (`FitsImage`), and an
  IRF-style (energy × cos θ) table with bilinear interpolation (`FitsTable`
  + `Bilinear`).
- Numerics: 1-D linear and bilinear interpolation (`Util::interpolate`,
  `Util::bilinear`), adaptive 8-point Gauss–Legendre quadrature (`dgaus8`
  in two forms, see below), and Brent root finding on GSL (`RootFinder`).
- A CPU-time accumulator for profiling (`Timer`).

**Does not:**
- Provide an application framework, parameter handling or output streams.
  Those are `st_app`, `hoops`/`ape` and `st_stream`.
- Do coordinate transforms or projections. It *calls* `astro::ProjBase`
  and `astro::SkyProj`.
- Know about LAT event or spacecraft semantics, apart from the MET
  date-keyword helper and the `ENERG_LO/CTHETA_LO` layout that
  `FitsTable` assumes.

## Position in the system

| Direction | Package | Why |
|-----------|---------|-----|
| Depends on | `astro` | `JulianDate` (in `Util.h`'s signatures), `SkyDir`, `SkyProj`/`ProjBase`. Linked `PUBLIC`. |
| Depends on | `tip` | `FitsImage`, `FitsTable`, `FitsUtil`, `Util::writeDateKeywords`. Linked `PRIVATE`; public headers only forward-declare tip types. |
| Depends on | `cfitsio` (external) | Called directly by `Util::isFitsFile` and most of `FitsUtil`. Linked `PRIVATE`. |
| Depends on | GSL (external) | `RootFinder.h` is a header-only template on `gsl_roots`. Linked `PUBLIC` for that reason. |
| Depends on | f2c (external) | The `dgaus8` object library (`src/dgaus8.c`, SLATEC translated by f2c) includes `f2c/f2c.h`. Linked `PRIVATE`, and its objects are folded into `libst_facilities.a`. |
| Depends on (undeclared) | `facilities` | `Util.h` includes `facilities/Util.h`. `Environment.cxx` uses `commonUtilities`, and `FitsUtil.cxx` uses `facilities::Util::basename`. It is not in `target_link_libraries`; it arrives transitively through `astro`/`tip`. |
| Header-only (undeclared) | `st_stream` | `Timer.h` includes `st_stream/StreamFormatter.h`. No st_facilities source includes `Timer.h`, so the library does not need it, but any client that includes `Timer.h` must link `st_stream` itself (`Likelihood` does). |

### Consumers

Derived from `#include "st_facilities/..."` in CMake-built code under `src/`.

| Consumer | What it uses |
|----------|--------------|
| `Likelihood` | Almost everything: `Util` (file checks, `resolve_fits_files`, `interpolate`/`bilinear`, `skyDir2pixel`/`pixel2SkyDir`, `writeDateKeywords`), `Environment::dataPath`/`xmlPath`, `FitsUtil` (`writeChecksums`, `fcopy`, `getRecordVector`), `FitsImage`, `GaussianQuadrature::dgaus8` (template; radial profiles, diffuse responses), `RootFinder` (`MeanPsf`), `Timer`, and `libStApiExports.h`. |
| `irfs` | `GaussianQuadrature::integrate` (f2c `dgaus8_`) in `irfInterface::IPsf`, `GaussianQuadrature::dgaus8` (template) in `latResponse`, raw `dgaus8_` from `st_facilities/dgaus8.h` in `dc1a`/`dc2`/`test` responses, `RootFinder` (`IPsf`), `Environment` (CALDB/data paths), `Env`, `FitsUtil`, `Util`. `irfInterface` links st_facilities `PUBLIC`, so every irfs client inherits it. |
| `evtbin` | `FileSys::expandFileList` (all `@list` handling for `gtbin`: events, FT2, GTIs) and `Env`. |
| `dataSubselector` | `Util` (`file_ok`, `resolve_fits_files`, `writeDateKeywords`), `FitsUtil` (`fcopy`, `writeChecksums`), `Environment`. |
| `SolarSystemTools`, `pyExposure`, `celestialSources` (`eblAtten`) | `Util`. |
| `observationSim` | `Util`, `FitsUtil`, `Environment`. No direct CMake link; it comes through `irfInterface`. |
| `rspgen`, `timeSystem` | `Env` (`timeSystem`: `Env::getEnv("TIMING_DIR")` for `leapsec.fits`). |
| `pyLikelihood` | SWIG-wraps `FitsImage` and `Util` (with `%extend` helpers for `resolve_fits_files`, `skyDir2pixel` and `pixel2SkyDir`). |
| Tests only | `catalogAccess`, `optimizers`, `burstFit` and `sane` include `Environment`/`Env` only in their test programs. |
| CMake link only | `orbitSim`, `BayesianBlocks` and `sourceIdentify` (`gtsrcid`) link st_facilities but include none of its headers. The `optimizers` library links it `PRIVATE` without using it (it has its **own** copy of `dgaus8.c`; see gotchas). |
| Not built | `fitsGen` apps (`makeFT1`, `irfTuple`, `egret2FT1`), `map_tools`' `exposure_map`, `irfs/handoff_response` and `g25Response`, and `skymaps` include it, but are not part of the CMake build. |

Nothing outside st_facilities includes `FitsTable.h` or `Bilinear.h`.

`docs/architecture/dependency-layers.md` lists st_facilities as `io`,
depending on `astro`, `tip`. That matches CMake. Its "used by" list omits
`observationSim` and `pyLikelihood`, which include the headers and get the
library transitively. It includes `orbitSim`, `BayesianBlocks`,
`sourceIdentify` and `optimizers`, which link it but do not include it
(outside tests).

## Key abstractions

Everything is in namespace `st_facilities`, and almost everything is a
static function on a class used as a namespace.

| Utility | Header | Contract |
|---------|--------|----------|
| `Util::fileExists(name)` | `Util.h` | `true` if an `ifstream` can open it. **No** environment expansion. |
| `Util::file_ok(name)` | `Util.h` | Expands env vars (`facilities::Util::expandEnvVar`), then throws `runtime_error("File not found: …")` if the file does not exist. |
| `Util::readLines(file, lines, skip="#", clean=false)` | `Util.h` | Expands env vars, then `file_ok`. Returns non-blank lines (`""` and `" "` only) whose first character is not in `skip`. `clean` strips everything from the first CR onward. |
| `Util::resolve_fits_files(name, files)` | `Util.h` | Strips one leading `@` and expands env vars. If cfitsio can open the result, `files = {name}`. **Otherwise the file is read as a list** with `readLines` (`#` comments skipped; entries are *not* env-expanded or checked). |
| `Util::isFitsFile`, `isXmlFile` | `Util.h` | `fits_open_file` succeeds / last `.`-token is `xml`. |
| `Util::interpolate(x, y, xx)` | `Util.h` | Linear interpolation on **ascending** `x`. Throws `range_error` outside `[x.front(), x.back()]`. |
| `Util::bilinear(xx, x, yy, y, z)` | `Util.h` | Two overloads: flat `z[i*ny + j]` (x-major) or `z[i][j]`. Accepts ascending or descending axes (flat version). **Extrapolates linearly** outside the grid; it does not throw. |
| `Util::writeDateKeywords(ext, tstart, tstop, extension, mission_start)` | `Util.h` | Writes `DATE` (now, UTC), `DATE-OBS`/`DATE-END` (MET + mission start, no leap seconds), `TSTART`/`TSTOP`, and `TELAPSE` if already present. Errors on the first three groups are swallowed. |
| `Util::skyDir2pixel` / `pixel2SkyDir` | `Util.h` | `astro::ProjBase` adaptors that pick (ra,dec) or (l,b) by `proj.isGalactic()`. |
| `Util::currentTime`, `expectedException` | `Util.h` | UTC `JulianDate` now / substring test on `what()` (used heavily in unit tests). |
| `FileSys::expandFileList(name)` | `FileSys.h` | If the env-expanded `name` starts with `@`, returns each non-empty line of that file, env-expanded. Otherwise returns `{name}`. **No** comment handling, trimming or FITS sniffing. |
| `Env` | `Env.h` | `expandEnvVar` (`$X`, `${X}`, `$(X)`; throws, *after* filling the output, if any variable is unset), `getEnv` (throws if unset), `appendFileName`/`appendPath`, and legacy `getDataDir`/`getXmlDir` (`$<PKG>ROOT/data`, else `$DATAPATH`/`$XMLPATH`). |
| `Environment` | `Environment.h` | Singleton whose first use calls `facilities::commonUtilities::setupEnvironment()`. Static `dataPath`, `xmlPath`, `packagePath` and `getEnv` forward to `commonUtilities`. |
| `FitsUtil` | `FitsUtil.h` | `getTableVector` (whole column → `vector<double>`), `getRecordVector` (one vector cell, row `recordNum`), `getFitsHduName(file, hdu)` (1-based absolute HDU, returns `EXTNAME`), `getFitsColNames`, `writeChecksums(file)` (every HDU, then `writeFilename`), `writeFilename` (`FILENAME` = basename in the primary header), and `fcopy(in, out, ext, filter, clobber)` (`fits_copy_file` of `in[ext][filter]`, all HDUs). |
| `FitsImage` | `FitsImage.h` | Reads a 2-D or 3-D image (float) plus `CRVAL/CDELT/CRPIX/CTYPE` per axis. `getCelestialArrays` and `getSolidAngles` use a `SkyProj` built by `skyProjCreate` (`CTYPE1` must start `RA` or `GLON`; `CDELT` + optional `CROTA2` only). It is designed to be subclassed (protected members). |
| `FitsTable` + `Bilinear` | `FitsTable.h`, `Bilinear.h` | Loads an IRF-style row (`ENERG_LO/HI`, `CTHETA_LO/HI` and a named parameter vector, row `nrow`). Interpolates bilinearly in (log10 E, cos θ) on bin centres, with the grid padded to `logE ∈ [0,10]`, `cos θ ∈ [-1,1]` by repeating edge values. Copied from `irfs/handoff_response`. |
| `GaussianQuadrature::dgaus8<F>(f, a, b, err, ierr)` | `GaussianQuadrature.h` | Header-only C++ port of SLATEC DGAUS8. Takes any functor `double(double) const`, is **re-entrant** (locals only), and throws `GaussianQuadrature::dgaus8Exception` (`errCode()` −1 or 2) instead of returning `ierr ≠ 1`. |
| `GaussianQuadrature::integrate(fp, a, b, err, ier)` | `GaussianQuadrature.h` | Calls the f2c `dgaus8_` with a plain `double(*)(double*)`. If `ier != 1`, it **silently falls back** to a fixed 96-point Gauss–Legendre rule and returns that, leaving `ier` as set by `dgaus8_`. |
| `dgaus8_` | `st_facilities/dgaus8.h` | Raw `extern "C"` f2c SLATEC routine (`src/dgaus8.c`). Its `xermsg` calls are commented out, so errors are reported only through `ierr`. |
| `RootFinder::find_root<F>(f, lo, hi, y, rtol=1e-3, atol=0)` | `RootFinder.h` | Solves `f(x) = y` with GSL Brent, up to 100 iterations, and returns the last estimate. Compiled only when `ScienceTools` is defined (CMake sets it `PUBLIC`). |
| `Timer` | `Timer.h` | Header-only. `start`/`stop` accumulate **`std::clock()` CPU time**. `report()` prints seconds through `st_stream`. |
| `libStApiExports.h` | | Defines `SCIENCETOOLS_API` (empty everywhere; a legacy Windows DLL macro). Still included by `Likelihood` and `irfs` headers. |

### Two dgaus8 implementations (plus one unused)

```
st_facilities/GaussianQuadrature.h   template dgaus8<F>   C++ port, functor, re-entrant, throws      ← Likelihood, latResponse
st_facilities/dgaus8.h + src/dgaus8.c   dgaus8_ (f2c)     C, function pointer, static locals          ← GaussianQuadrature::integrate, irfs dc*/test responses
src/dgaus8.h                          st_facilities::dgaus8 template, throws const char*   — not installed, not used
src/dgaus8.f                          Fortran source of the f2c translation — not compiled
```

## Data flow and control flow

There is no single flow. The two most common call chains are these.

**`@list` arguments.** `evtbin` uses `FileSys::expandFileList`, which
needs an explicit `@`. `Likelihood`, `dataSubselector` and
`pyLikelihood` use `Util::resolve_fits_files`, which *sniffs* the file
and treats anything cfitsio cannot open as a list:

```
"@events.txt" ─▶ FileSys::expandFileList ─▶ Env::expandEnvVar(name) ─▶ starts with '@'? read lines, expand each
"events.txt"  ─▶ Util::resolve_fits_files ─▶ strip '@', expand ─▶ isFitsFile? {name} : readLines(name)  (comments skipped, entries not expanded)
```

**Quadrature in IRF code.** Integrands are free or static functions that
take their parameters from **static variables** set just before the call
(for example `IPsf::setStaticVariables` then
`GaussianQuadrature::integrate(&coneIntegrand, …)`), because the f2c
interface has no user-data pointer. Newer code uses the template
`dgaus8` with a functor and needs no statics.

## Public interfaces

### C++ API

All headers in `st_facilities/` are public. The most used, in order, are
`Util.h`, `Environment.h`, `FitsUtil.h`, `GaussianQuadrature.h`, `Env.h`,
`FileSys.h` and `FitsImage.h`.

Idiomatic integration (preferred: re-entrant, takes a functor, throws on
failure):

```cpp
#include "st_facilities/GaussianQuadrature.h"

struct Integrand { double operator()(double x) const { return x*x; } };
Integrand f;
double err(1e-5);  // relative tolerance in; if negative, estimated error out
int ierr;
double v = st_facilities::GaussianQuadrature::dgaus8(f, 0., 1., err, ierr);
// throws GaussianQuadrature::dgaus8Exception (errCode() -1 or 2) on failure
```

### Applications

None. The package builds the static library `st_facilities`, the object
library `dgaus8` and the test `test_st_facilities`.

### Python

None of its own (`st_facilitiesLib.py` is legacy SCons). `FitsImage` and
`Util` are exposed through `pyLikelihood`.

### File formats

None of its own. `FitsTable` expects one row with the vector columns
`ENERG_LO`, `ENERG_HI`, `CTHETA_LO`, `CTHETA_HI` and a parameter column
(values laid out energy-fastest). `FitsImage` expects a primary or image
extension with `CRVALn/CDELTn/CRPIXn/CTYPEn`.

## Build and test

- **CMake targets:** `dgaus8` (OBJECT, from `src/dgaus8.c`, `PRIVATE
  f2c::f2c`) and `st_facilities` (**STATIC**). The static library links
  `PRIVATE dgaus8 cfitsio tip` and `PUBLIC astro GSL::gsl`, and defines
  `ScienceTools` as a `PUBLIC` compile definition (which enables
  `RootFinder`). `TRAP_FPE` is defined `PRIVATE` on non-Apple platforms.
  The `install(TARGETS …)` has `EXPORT fermiTargets` **commented out**, so
  the library is installed but not exported.
- **Headers** install to `${FERMI_INSTALL_INCLUDEDIR}/st_facilities`.
  `data/` (a single sample `list_file`) installs to
  `${FERMI_INSTALL_DATADIR}` (`share/fermitools/data/`) with no package
  subdirectory. Nothing in the code reads it.
- **Test executable:** `test_st_facilities` (`src/test/test.cxx`), a CppUnit
  `TextTestRunner`. It is **not registered with CTest**, so run it by
  hand. It exits 0 on success and 1 on failure.
  - Active tests: template `dgaus8` (including a nested integral and a
    sharp energy-dispersion-like integrand), `GaussianQuadrature::integrate`,
    `RootFinder`, `Util::file_ok`/`readLines`/`expectedException`/
    `resolve_fits_files`, and `Env::appendPath`.
  - Disabled: `Env::expandEnvVar` (needs `STTEST=sttest`),
    `Env::getDataDir` and `FileSys::expandFileList` (need
    `$ST_FACILITIESROOT`). `FitsImage`, `FitsUtil`, `FitsTable`,
    `Bilinear`, `Environment` and `Timer` have no tests here.
  - It needs no environment variables and no data files. `setUp` writes
    `test_file.txt` into the **current working directory** and `tearDown`
    deletes it, so run it from a scratch directory.
  - `TRAP_FPE` is defined only on the library target, so the test's
    `#ifdef TRAP_FPE` / `<fenv.h>` block is never compiled in. Floating-point
    exceptions are **not** trapped in the test.
- **Legacy:** `SConscript`, `st_facilitiesLib.py`, `.cvsignore`.
  `doc/release.notes` stops at 2017 (00-22-02); later history is in git.
- **CI:** `azure-pipelines.yml` (shared `fermi-lat/ci` templates).

## Invariants and gotchas

**dgaus8 and quadrature**
- **ABI mismatch on `ierr` (suspected).** The bundled f2c is patched to
  `typedef int integer` (`externals/f2c/f2c.patch`), so `dgaus8_` writes a
  4-byte `int`. The public prototype in `st_facilities/dgaus8.h` and
  `GaussianQuadrature::integrate` pass a `long *`. On 64-bit little-endian
  machines only the low half of the caller's `long` is written. If the
  caller did not zero it (the in-package test declares `long ier;`
  uninitialised), `ier == 1` can be false after a successful call, and
  `integrate` then **silently replaces the adaptive result with the fixed
  96-point rule**. An error code of −1 reads back as 4294967295. This is
  from reading the code; not checked at runtime. `irfs::IPsf` initialises
  `ierr(0)`, which hides it. Unmerged branches in this repo
  (`toolchain-fix`: "update dgaus8 function call";
  `feature/f26_linuxBuild`: "Use doublereal instead of double", "Updated
  dgaus8 parameters") change `dgaus8_`'s signature. The top-level
  `CMakeLists.txt` notes a known "difference between compiled and
  downloaded F2C" next to `USE_SYSTEM_F2C`. With an unpatched system f2c
  (`long integer`), the prototype would match.
- **`dgaus8_` is not re-entrant or thread-safe.** f2c made every local
  `static` (`aa`, `hh`, `vl`, `gr`, `lr`, `l`, `est`, …). An integrand that
  itself calls `dgaus8_`/`GaussianQuadrature::integrate` corrupts the
  outer integration. Use the template `GaussianQuadrature::dgaus8` for
  nested integrals. The test's `Foo` case does exactly that.
- **Duplicate symbols.** `optimizers/src/dgaus8.c` is a second copy of the
  same f2c file, built into the shared `optimizers` library, with the
  `xermsg_` diagnostics *enabled*. Both define the global symbols
  `dgaus8_`, `i1mach_`, `d1mach_`, `xermsg_` and others. Which one a
  binary that links both `optimizers` and `st_facilities` (for example
  `Likelihood`) calls depends on link order and symbol interposition
  (inferred; not checked at runtime).
- `err` is an in/out parameter. A positive value is the requested
  relative accuracy. `0` means √(machine ε). If it is negative on input,
  the estimated error is written back.
- The template `dgaus8` **throws** where SLATEC would return `ierr = -1`
  (|a−b| too small relative to |b|) or `ierr = 2` (probably inaccurate).
  Wrap calls if a slightly inaccurate answer is acceptable.
- `GaussianQuadrature.h` and `src/dgaus8.h` both define
  `sign(double,double)` in an anonymous namespace in a header.

**File lists and environment**
- `FileSys::expandFileList` and `Util::resolve_fits_files` are **not
  interchangeable**. The first needs `@`, keeps comment lines, keeps
  leading and trailing whitespace (and a CR, if the list has Windows line
  endings) and env-expands each entry. The second needs no `@`, treats any
  non-FITS file (including a corrupt FITS file) as a list, skips `#` lines
  and does not expand entries.
- `Env::expandEnvVar` (used by `expandFileList`) **throws** if a path
  contains `$` followed by an unset variable or by no name, but fills in
  `expanded` first. File names that contain a literal `$` cannot go
  through `expandFileList`.
- `expandFileList` reads with `getline(buf, FILENAME_MAX)`. A line longer
  than that sets `failbit` and **silently ends the list**.
- `Env::getDataDir`/`getXmlDir` use the CMT/SCons `$<PKG>ROOT` convention
  and fall back to `$DATAPATH`/`$XMLPATH`. In a conda install neither is
  normally set. Use `Environment::dataPath` (or
  `facilities::commonUtilities::getDataPath`) instead.
- `Environment.h` declares `pfilesPath()`, but it is **never defined**.
  Calling it fails at link time.
- `Util::cleanLine` passes a 1-char, **non-NUL-terminated** array to
  `std::string::find(const char*)` and to `stringTokenize`, which is
  undefined behaviour (from reading the code; not checked at runtime).
- `Util::readLines` treats `skip` as a **set of characters**
  (`find_first_of`), not as a prefix string. Only exactly `""` and `" "`
  count as blank lines.

**FITS helpers**
- `FitsUtil::getFitsHduName` reads `EXTNAME` into `char extname[20]`
  (cfitsio may write up to 71 bytes) and `comment[72]` (`FLEN_COMMENT` is
  73). An `EXTNAME` longer than 19 characters overflows the stack buffer
  (from reading the code; not checked at runtime).
- `FitsUtil.cxx` includes `fitsio.h` **inside `namespace st_facilities`**.
  It works because the functions are `extern "C"`, but it is fragile.
- `FitsImage::read_fits_image` resizes `m_axes` to 2 for a 3-D image with
  `NAXIS3 = 1`, but then loops over all 3 dims with `m_axes.at(i)`. Such
  images throw `std::out_of_range` (from reading the code; not checked at
  runtime). Axis parameters are stored as `float`, and `CD`/`PC` matrices
  are ignored (only `CDELT` and `CROTA2`). `getCelestialArrays` and
  `getSolidAngles` reopen the file and build a new `SkyProj` on every
  call. `solidAngle` is a flat two-triangle approximation.
- `FitsTable` and `FitsUtil::getRecordVector` read vector cells as
  `float`, so double-precision columns lose precision. `FitsTable::value`
  throws `invalid_argument` for log10(E) > 10, and clamps cos θ at the last
  bin centre. Its copy-assignment operator is protected and does nothing.
- `Util::writeDateKeywords` ignores its `extension` argument, swallows
  errors when setting `DATE`, `DATE-OBS`/`DATE-END` and `TSTART`/`TSTOP`,
  and does not write `ONTIME`, despite the header comment. `DATE-OBS`
  is MET seconds plus the mission start, with no leap-second correction.

**Numerics**
- `Util::interpolate` requires ascending abscissae and returns
  `y.back()` exactly at `x.back()`. `Util::bilinear` never throws: outside
  the grid it extrapolates from the edge cell. `Bilinear` (used by
  `FitsTable`) clamps below the grid and throws above it.
- `RootFinder::find_root` does not check `gsl_root_fsolver_set`. If
  `f(lo) − y` and `f(hi) − y` have the same sign, GSL's error handler runs
  (the default one calls `abort()`). It also returns the last estimate
  after 100 iterations without reporting non-convergence.
- `Timer` measures **process CPU time** (`std::clock`), not wall time, so
  multi-threaded sections over-count. `report()` divides the accumulated
  clock ticks; `operator()()` returns raw ticks.

**Build**
- `st_facilities` is a **static** library, so each shared library that
  links it gets its own copy of its code and statics (for example
  `Environment::s_instance`).
- The `ScienceTools` macro is defined for every target that links
  st_facilities. `RootFinder.h` compiles to nothing without it.

## Extension points

- **New utility:** put it here only if two or more packages need it and it
  has no physics or instrument knowledge beyond FITS conventions (see the
  placement guidance in `docs/architecture/README.md`). Add a static
  function to the closest existing class (`Util`, `FitsUtil`, `Env`), or a
  new header and source in `st_facilities/` and `src/`, and list the
  source in `CMakeLists.txt`. If it needs a new external, link it
  `PUBLIC` only if its headers appear in the public header.
- **New integrand:** prefer `GaussianQuadrature::dgaus8` with a functor.
  Use `GaussianQuadrature::integrate`/`dgaus8_` only when you must match
  existing static-variable code, and never nest it.
- **Fixing the `dgaus8_` interface:** change `st_facilities/dgaus8.h` and
  `GaussianQuadrature::integrate` to the f2c `integer` type (or use
  `f2c.h` types), and update the direct callers in
  `irfs/{dc1a,dc2,test}Response`. Coordinate with `optimizers`, which
  carries its own copy, and with the pending `toolchain-fix` /
  `feature/f26_linuxBuild` branches.

Any header change here rebuilds most of the analysis layer. Run
`test_st_facilities`, and at least the `irfs` (`latResponse`),
`Likelihood`, `dataSubselector` and `evtbin` tests.

## Further reading

- SLATEC DGAUS8: <https://www.netlib.org/slatec/src/dgaus8.f>. The local
  Fortran is in `src/dgaus8.f`.
- GSL one-dimensional root finding:
  <https://www.gnu.org/software/gsl/doc/html/roots.html>.
- `src/facilities` (`commonUtilities`), the environment layer that
  `Environment` wraps.
- `doc/release.notes`: history up to 2017.
