# ISL Java Bindings (OSGi / Eclipse RCP)

Java/JNI bindings intended to make ISL functionality available to Java applications, with a focus on **Eclipse/OSGi** distribution.

> Note: The JNI implementation in this repository binds directly to the upstream ISL library. JNI calls use ISL APIs (e.g., `isl_ctx_alloc`, `isl_set_read_from_str`) and map ISL errors to Java exceptions. See `bundles/com.emmtrix.isl.jni/src/main/c/isl_jni.c`.

## Repository structure

This is a multi-module Maven/Tycho build :contentReference[oaicite:1]{index=1}:

- `bundles/com.emmtrix.isl.core` – Java API (core types)
- `bundles/com.emmtrix.isl.jni` – JNI bundle (native entry points)
- `bundles/com.emmtrix.isl.native.*` – platform-specific OSGi fragments shipping native code
  - `win32.x86_64`, `linux.x86_64`, `macosx.aarch64` :contentReference[oaicite:2]{index=2}
- `features/com.emmtrix.isl.feature` – Eclipse feature
- `releng/com.emmtrix.isl.site` – p2 update site project
- `tests/*` – build-wiring tests

The build targets **Java 11** :contentReference[oaicite:3]{index=3} and uses **Tycho** :contentReference[oaicite:4]{index=4}.

## Java API (core)

Entry point is `IslContext`. Contexts are **not thread-safe** and must be confined to a thread or externally synchronized :contentReference[oaicite:5]{index=5}.

Native code is loaded on first use via `System.loadLibrary("isl_jni")` :contentReference[oaicite:6]{index=6}.

Example:

```java
import com.emmtrix.isl.core.IslContext;
import com.emmtrix.isl.core.IslSet;

try (IslContext ctx = IslContext.create()) {
  try (IslSet set = ctx.readSet("{ [i] : 0 <= i < 10 }")) {
    // work with set
  }
}
````

Resource management:

* Most native-backed objects implement `AutoCloseable` and release native resources in `close()` .

## Native packaging (OSGi)

Native code is shipped as **OSGi fragments** attached to `com.emmtrix.isl.jni`. Each fragment declares its platform-specific native library via `Bundle-NativeCode`, e.g.:

* Linux x86_64: `native/linux/x86_64/libisl_wrapper.so` 
* Windows x86_64: `native/win32/x86_64/isl_wrapper.dll` 
* macOS aarch64: `native/macosx/aarch64/libisl_wrapper.dylib` 

## Building

Prerequisites:

* JDK 11 
* Maven
* ISL development headers and libraries available to the native build toolchain (for JNI).

Build everything (including tests, feature, p2 repo):

```bash
mvn clean verify
```

## Versioning and releases (OSGi + p2)

Keep OSGi and p2 versions aligned:

* Use the same SemVer base across bundles, feature, and p2 category, and append `.qualifier` for OSGi artifacts .
* Tag releases as `vX.Y.Z` (e.g., `v0.1.0`) matching that SemVer base .

## Development notes

* The JNI layer calls directly into ISL and maps ISL error categories to Java exceptions implemented in `isl_jni.c`.
* A small CMake-based native wrapper exists under `native/isl-wrapper` (builds a shared library named `isl_wrapper`).

## License

TBD
