// Offline, single-threaded C++ allocation budget test. Never link into the app.
#include "MdaEPianoEngine.h"
#include "mdaEPianoData.h" // sizeof only; does not copy samples into the pool.
#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <new>
#include <type_traits>

extern config_t configuration;
extern uint8_t master_volume;

namespace {
// Monotonic arena: frees reduce live payload, but storage is reclaimed only at
// the end of a run. Suitable for this engine's allocate-once buffer lifetime.
struct Pool {
    struct alignas(std::max_align_t) Header { std::size_t bytes; };
    unsigned char* data;
    std::size_t capacity, used = 0, live = 0, peakLive = 0, calls = 0;
    std::size_t failures = 0, failedBytes = 0, failedEnd = 0;
    bool sealed = false;

    void* allocate(std::size_t bytes, std::size_t alignment) {
        ++calls;
        bytes = std::max(bytes, std::size_t{1});
        alignment = std::max(alignment, alignof(Header));
        // Check all arithmetic before advancing the cursor.
        const auto max = std::numeric_limits<std::size_t>::max();
        const bool overflow = used > max - sizeof(Header);
        const auto initial = overflow ? max : used + sizeof(Header);
        const auto base = reinterpret_cast<std::uintptr_t>(data);
        const auto padding = (alignment - ((base + initial) & (alignment - 1))) & (alignment - 1);
        const auto start = overflow || initial > max - padding ? max : initial + padding;
        const auto end = start > max - bytes ? max : start + bytes;
        if (sealed || end > capacity || end == max) {
            ++failures;
            failedBytes = bytes;
            failedEnd = end;
            throw std::bad_alloc(); // No fallback to the system heap.
        }
        auto* result = data + start;
        ::new (result - sizeof(Header)) Header{bytes};
        used = end;
        live += bytes;
        peakLive = std::max(peakLive, live);
        return result;
    }

    bool owns(const void* p) const {
        const auto address = reinterpret_cast<std::uintptr_t>(p);
        const auto base = reinterpret_cast<std::uintptr_t>(data);
        return address >= base && address - base < capacity;
    }
    void release(void* p) {
        auto* header = reinterpret_cast<Header*>(
            static_cast<unsigned char*>(p) - sizeof(Header));
        live -= header->bytes;
    }
};
Pool* activePool = nullptr;

void* allocate(std::size_t bytes, std::size_t alignment) {
    if (activePool) return activePool->allocate(bytes, alignment);
    void* p = nullptr;
    if (posix_memalign(&p, alignment, std::max(bytes, std::size_t{1})) != 0)
        throw std::bad_alloc();
    return p;
}
void release(void* p) noexcept {
    if (!p) return;
    if (activePool && activePool->owns(p)) activePool->release(p);
    else std::free(p);
}
} // namespace

// These replacements affect this test executable only. The current engine
// uses new/new[]; C malloc, OS allocations and function-call stacks are excluded.
void* operator new(std::size_t n) { return allocate(n, alignof(std::max_align_t)); }
void* operator new[](std::size_t n) { return allocate(n, alignof(std::max_align_t)); }
void operator delete(void* p) noexcept { release(p); }
void operator delete[](void* p) noexcept { release(p); }
void operator delete(void* p, std::size_t) noexcept { release(p); }
void operator delete[](void* p, std::size_t) noexcept { release(p); }
void* operator new(std::size_t n, std::align_val_t a) { return allocate(n, std::size_t(a)); }
void* operator new[](std::size_t n, std::align_val_t a) { return allocate(n, std::size_t(a)); }
void operator delete(void* p, std::align_val_t) noexcept { release(p); }
void operator delete[](void* p, std::align_val_t) noexcept { release(p); }
void operator delete(void* p, std::size_t, std::align_val_t) noexcept { release(p); }
void operator delete[](void* p, std::size_t, std::align_val_t) noexcept { release(p); }
void* operator new(std::size_t n, const std::nothrow_t&) noexcept {
    try { return ::operator new(n); } catch (...) { return nullptr; }
}
void* operator new[](std::size_t n, const std::nothrow_t&) noexcept {
    try { return ::operator new[](n); } catch (...) { return nullptr; }
}
void operator delete(void* p, const std::nothrow_t&) noexcept { release(p); }
void operator delete[](void* p, const std::nothrow_t&) noexcept { release(p); }
void* operator new(std::size_t n, std::align_val_t a, const std::nothrow_t&) noexcept {
    try { return ::operator new(n, a); } catch (...) { return nullptr; }
}
void* operator new[](std::size_t n, std::align_val_t a, const std::nothrow_t&) noexcept {
    try { return ::operator new[](n, a); } catch (...) { return nullptr; }
}
void operator delete(void* p, std::align_val_t, const std::nothrow_t&) noexcept { release(p); }
void operator delete[](void* p, std::align_val_t, const std::nothrow_t&) noexcept { release(p); }

namespace {
struct CheckFailure { const char* message; };
void require(bool ok, const char* message) { if (!ok) throw CheckFailure{message}; }

template<class Engine>
void reset(Engine& engine) {
    if constexpr (std::is_same_v<Engine, mdaEPiano>) engine.reset_voices();
    else engine.reset();
}
template<class Engine>
void noteOff(Engine& engine, int note) {
    if constexpr (std::is_same_v<Engine, mdaEPiano>) engine.noteOn(note, 0);
    else engine.noteOff(note);
}
template<class Engine>
void sustain(Engine& engine, bool on) {
    if constexpr (std::is_same_v<Engine, mdaEPiano>) engine.processMidiController(64, on ? 127 : 0);
    else engine.sustainPedal(on);
}
template<class Engine>
void render(Engine& engine, int16_t* left, int16_t* right, unsigned frames) {
    if constexpr (std::is_same_v<Engine, mdaEPiano>) engine.process(right, left, frames);
    else engine.render(left, right, frames);
}

struct Workload { int maxVoices = 0; unsigned long long renderedFrames = 0; };
template<class Engine>
Workload exercise(Engine& engine, int16_t* left, int16_t* right,
                  unsigned frames, unsigned blocks) {
    Workload result;
    auto block = [&] {
        left[0] = right[0] = left[frames + 1] = right[frames + 1] = 12345;
        render(engine, left + 1, right + 1, frames);
        require(left[0] == 12345 && right[0] == 12345 &&
                left[frames + 1] == 12345 && right[frames + 1] == 12345,
                "output buffer guard overwritten");
        result.renderedFrames += frames;
    };
    for (int velocity : {32, 70, 110}) {
        for (int note = 36; note <= 96; ++note) {
            reset(engine);
            engine.noteOn(note, velocity);
            bool audible = false;
            for (unsigned b = 0; b < blocks; ++b) {
                block();
                for (unsigned i = 1; i <= frames; ++i)
                    audible |= left[i] != 0 || right[i] != 0;
            }
            require(audible, "silent note in sweep");
            noteOff(engine, note);
            block();
        }
    }
    reset(engine);
    sustain(engine, true);
    // Exceed capacity to exercise voice stealing, with all three velocity layers.
    for (int i = 0; i < NVOICES * 2; ++i) {
        const int note = 36 + i % 61;
        engine.noteOn(note, 32 + (i % 3) * 39);
        result.maxVoices = std::max(result.maxVoices, int(engine.getActiveVoices()));
        noteOff(engine, note);
    }
    require(result.maxVoices == NVOICES, "did not reach full polyphony");
    for (unsigned b = 0; b < blocks; ++b) block();
    sustain(engine, false);
    for (unsigned b = 0; b < blocks; ++b) block();
    reset(engine);
    block();
    for (unsigned i = 1; i <= frames; ++i)
        require(left[i] == 0 && right[i] == 0, "reset did not silence output");
    return result;
}

template<class Engine>
int run(Pool& pool, unsigned frames, unsigned blocks) {
    const char* phase = "construct";
    const char* failure = nullptr;
    std::size_t engineBytes = 0, initCalls = 0, initUsed = 0, runtimeCalls = 0;
    Workload workload;
    int status = 0;
    activePool = &pool;
    try {
        // Object, nested C++ allocations and caller-owned output all use the pool.
        std::unique_ptr<Engine> engine(new Engine{});
        engineBytes = pool.live;
        phase = "output buffers";
        std::unique_ptr<int16_t[]> left(new int16_t[frames + 2]);
        std::unique_ptr<int16_t[]> right(new int16_t[frames + 2]);
        initCalls = pool.calls;
        initUsed = pool.used;
        phase = "events/render (allocations forbidden)";
        pool.sealed = true;
        workload = exercise(*engine, left.get(), right.get(), frames, blocks);
        runtimeCalls = pool.calls - initCalls;
        phase = "destruction";
    } catch (const std::bad_alloc&) {
        status = 2;
        failure = "pool exhausted or allocation attempted after initialization";
    } catch (const CheckFailure& e) {
        status = 1;
        failure = e.message;
    } catch (...) {
        status = 1;
        failure = "unexpected exception";
    }
    activePool = nullptr;
    if (initCalls) runtimeCalls = pool.calls - initCalls;
    if (!status && pool.live != 0) { status = 1; failure = "live C++ allocations after destruction"; }
    std::printf("pool_capacity_bytes=%zu\n", pool.capacity);
    std::printf("object_bytes=%zu\n", sizeof(Engine));
    std::printf("engine_object_and_nested_payload_bytes=%zu\n", engineBytes);
    std::printf("output_payload_bytes=%zu\n", std::size_t(frames + 2) * 2 * sizeof(int16_t));
    std::printf("init_allocations=%zu\ninit_pool_used_bytes=%zu\n", initCalls, initUsed);
    std::printf("total_allocation_attempts=%zu\n", pool.calls);
    std::printf("peak_live_payload_bytes=%zu\npool_high_water_bytes=%zu\n", pool.peakLive, pool.used);
    std::printf("pool_metadata_and_padding_bytes=%zu\n", pool.used - pool.peakLive);
    std::printf("runtime_allocation_attempts=%zu\nallocation_failures=%zu\n", runtimeCalls, pool.failures);
    std::printf("live_payload_after_teardown_bytes=%zu\n", pool.live);
    std::printf("sample_readonly_bytes=%zu (outside pool)\n", sizeof(epianoData));
    std::printf("shared_global_payload_bytes=%zu (outside pool)\n", sizeof(configuration) + sizeof(master_volume));
    std::printf("voice_capacity=%d\nmax_observed_voices=%d\n", NVOICES, workload.maxVoices);
    std::printf("sample_rate=%d\nframes_per_block=%u\nblocks_per_note=%u\nrendered_frames=%llu\n",
                AUDIO_SAMPLE_RATE, frames, blocks, workload.renderedFrames);
    if (failure) {
        std::printf("FAIL: phase=%s: %s\n", phase, failure);
        if (pool.failures)
            std::printf("failed_request_bytes=%zu\nfailed_request_end_bytes=%zu\n", pool.failedBytes, pool.failedEnd);
        // Existing effects constructors do not unwind all partially built arrays.
        // Failure runs discard the entire arena; this is not successful teardown.
    } else {
        std::puts("PASS: bounded C++ pool, note/velocity sweep, voice stealing, sustain, guards, silence, teardown");
    }
    return status;
}

bool number(const char* text, std::size_t& value) {
    if (!text[0]) return false;
    for (const char* p = text; *p; ++p) if (*p < '0' || *p > '9') return false;
    errno = 0;
    char* end = nullptr;
    const auto parsed = std::strtoull(text, &end, 10);
    if (errno || *end || parsed > std::numeric_limits<std::size_t>::max()) return false;
    value = static_cast<std::size_t>(parsed);
    return true;
}
} // namespace

int main(int argc, char** argv) {
    std::size_t bytes = 0, frames = 441, blocks = 64;
    const char* mode = "wrapper";
    bool valid = true;
    for (int i = 1; i < argc && valid; i += 2) {
        if (i + 1 == argc) { valid = false; break; }
        if (std::strcmp(argv[i], "--engine") == 0) mode = argv[i + 1];
        else if (std::strcmp(argv[i], "--pool-bytes") == 0) valid = number(argv[i + 1], bytes);
        else if (std::strcmp(argv[i], "--frames") == 0) valid = number(argv[i + 1], frames);
        else if (std::strcmp(argv[i], "--blocks") == 0) valid = number(argv[i + 1], blocks);
        else valid = false;
    }
    if (!valid || bytes == 0 || frames == 0 || frames > 65536 || blocks == 0 || blocks > 100000 ||
        (std::strcmp(mode, "core") != 0 && std::strcmp(mode, "wrapper") != 0)) {
        std::fprintf(stderr, "Usage: %s --pool-bytes N [--engine core|wrapper] [--frames 441] [--blocks 64]\n"
                     "N is bytes, excluding samples, stack, C malloc and macOS/UI. Exit: 0 pass, 1 check/usage, 2 pool failure.\n", argv[0]);
        return 1;
    }
    // Account for alignment padding relative to the actual backing address.
    void* memory = nullptr;
    if (posix_memalign(&memory, alignof(std::max_align_t), bytes) != 0) {
        std::fputs("FAIL: cannot allocate backing pool\n", stderr);
        return 2;
    }
    Pool pool{static_cast<unsigned char*>(memory), bytes};
    std::printf("engine=%s\n", mode);
    const int status = std::strcmp(mode, "core") == 0
        ? run<mdaEPiano>(pool, unsigned(frames), unsigned(blocks))
        : run<MdaEPianoEngine>(pool, unsigned(frames), unsigned(blocks));
    std::free(memory);
    return status;
}
