#include <iostream>
#include <cstring>
#include <cstdint>
#include <cstdlib>

#if defined(_MSC_VER)
	#include <intrin.h>
#elif defined(HAVE_GCC_GET_CPUID) && defined(USE_GCC_GET_CPUID)
	#include <cpuid.h>
#endif

enum instruction_set {
	DEFAULT = 0x0,
	LZCNT	= 0x1,
	POPCNT	= 0x2,
	BMI1	= 0x4,
	BMI2	= 0x8,
	NEON	= 0x10,
	AVX		= 0x20,
	AVX2	= 0x40,
	AVX512F = 0x80,
};

namespace {
	constexpr uint32_t cpuidAvx2Bit		= 1ul << 5;
	constexpr uint32_t cpuidBmi1Bit		= 1ul << 3;
	constexpr uint32_t cpuidBmi2Bit		= 1ul << 8;
	constexpr uint32_t cpuidAvx512Bit	= 1ul << 16;
	constexpr uint64_t cpuidAvx256Saved = 1ull << 2;
	constexpr uint64_t cpuidAvx512Saved = 7ull << 5;
	constexpr uint32_t cpuidOsxSave		= (1ul << 26) | (1ul << 27);
	constexpr uint32_t cpuidLzcntBit	= 1ul << 5;
	constexpr uint32_t cpuidPopcntBit	= 1ul << 23;
}

#if defined(__x86_64__) || defined(_M_AMD64)
static inline void cpuid(uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
inline static uint64_t xgetbv();
#endif

static void getCPUBrandString(char* brand) {
#if defined(__x86_64__) || defined(_M_AMD64)
	uint32_t regs[12];
	regs[0] = 0x80000000;
	cpuid(regs, regs + 1, regs + 2, regs + 3);
	if (regs[0] < 0x80000004)
		return;
	regs[0] = 0x80000002;
	cpuid(regs, regs + 1, regs + 2, regs + 3);
	regs[4] = 0x80000003;
	cpuid(regs + 4, regs + 5, regs + 6, regs + 7);
	regs[8] = 0x80000004;
	cpuid(regs + 8, regs + 9, regs + 10, regs + 11);

	memcpy(brand, regs, sizeof(regs));
#endif
}

void printCPUInfo(uint32_t supportedISA) {
#if defined(__x86_64__) || defined(_M_AMD64)
	char brand[49] = { 0 };
	getCPUBrandString(brand);
	std::cout << "CPU Brand: " << brand << "\n";
#endif
}

#if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)

inline static uint32_t detectSupportedArchitectures() {
	return instruction_set::NEON;
}

#elif defined(__x86_64__) || defined(_M_AMD64)

static inline void cpuid(uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
	#if defined(_MSC_VER)
	int32_t cpuInfo[4];
	__cpuidex(cpuInfo, *eax, *ecx);
	*eax = cpuInfo[0];
	*ebx = cpuInfo[1];
	*ecx = cpuInfo[2];
	*edx = cpuInfo[3];
	#elif defined(HAVE_GCC_GET_CPUID) && defined(USE_GCC_GET_CPUID)
	uint32_t level = *eax;
	__get_cpuid(level, eax, ebx, ecx, edx);
	#else
	uint32_t a = *eax, b, c = *ecx, d;
	asm volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(a), "c"(c));
	*eax = a;
	*ebx = b;
	*ecx = c;
	*edx = d;
	#endif
}

inline static uint64_t xgetbv() {
	#if defined(_MSC_VER)
	return _xgetbv(0);
	#else
	uint32_t eax, edx;
	asm volatile("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));
	return (( uint64_t )edx << 32) | eax;
	#endif
}

inline static uint32_t detectSupportedArchitectures() {
	std::uint32_t eax	  = 0;
	std::uint32_t ebx	  = 0;
	std::uint32_t ecx	  = 0;
	std::uint32_t edx	  = 0;
	std::uint32_t hostIsa = 0x0;

	eax = 0x1;
	ecx = 0x0;
	cpuid(&eax, &ebx, &ecx, &edx);

	if (ecx & cpuidLzcntBit) {
		hostIsa |= instruction_set::LZCNT;
	}

	if (ecx & cpuidPopcntBit) {
		hostIsa |= instruction_set::POPCNT;
	}

	if ((ecx & cpuidOsxSave) != cpuidOsxSave) {
		return hostIsa;
	}

	uint64_t xcr0 = xgetbv();

	if ((xcr0 & cpuidAvx256Saved) == 0) {
		return hostIsa;
	}

	if (ecx & cpuidAvx256Saved) {
		hostIsa |= instruction_set::AVX;
	}

	eax = 0x7;
	ecx = 0x0;
	cpuid(&eax, &ebx, &ecx, &edx);

	if (ebx & cpuidAvx2Bit) {
		hostIsa |= instruction_set::AVX2;
	}

	if (ebx & cpuidBmi1Bit) {
		hostIsa |= instruction_set::BMI1;
	}

	if (ebx & cpuidBmi2Bit) {
		hostIsa |= instruction_set::BMI2;
	}

	if (!((xcr0 & cpuidAvx512Saved) == cpuidAvx512Saved)) {
		return hostIsa;
	}

	if (ebx & cpuidAvx512Bit) {
		hostIsa |= instruction_set::AVX512F;
	}

	return hostIsa;
}

#elif defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)

inline static uint32_t detectSupportedArchitectures() {
	return instruction_set::NEON;
}

#else

inline static uint32_t detectSupportedArchitectures() {
	return instruction_set::DEFAULT;
}

#endif

int32_t main() {
	const auto supportedISA = detectSupportedArchitectures();
	printCPUInfo(supportedISA);
	return supportedISA;
}