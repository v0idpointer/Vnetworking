#pragma once

#include <Vnetworking/Security/SecureConnection.h>

#include <cstdint>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#define SECURITY_WIN32
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <wincrypt.h>
#include <schannel.h>
#include <security.h>

#pragma comment (lib, "WS2_32.lib")
#pragma comment (lib, "secur32.lib")
#pragma comment (lib, "crypt32.lib")

namespace Vnetworking::Security::Private {

	// copies NativeSecurityContext_t to Schannel's SecHandle
	static inline void ToSchannelHandle(const NativeSecurityContext_t& in, SecHandle& out) noexcept {
		out.dwLower = static_cast<ULONG_PTR>(in.Lower);
		out.dwUpper = static_cast<ULONG_PTR>(in.Upper);
	}

	// copies SecHandle to NativeSecurityContext_t
	static inline void ToVnetsecHandle(const SecHandle& in, NativeSecurityContext_t& out) noexcept {
		out.Lower = static_cast<std::uintptr_t>(in.dwLower);
		out.Upper = static_cast<std::uintptr_t>(in.dwUpper);
	}

	// checks if SecHandle is not a null handle.
	static inline bool IsValidSecHandle(const SecHandle& secHandle) noexcept {
		return !((secHandle.dwLower == 0) && (secHandle.dwUpper == 0));
	}

	constexpr const std::int32_t HANDSHAKE_BUFFER_LEN = 16384;

	static inline void InitializeSecurityBuffer(SecBuffer& securityBuffer, const std::uint32_t BufferType, const std::uint32_t cbBuffer, void* const pvBuffer) {
		securityBuffer.BufferType = BufferType;
		securityBuffer.cbBuffer = cbBuffer;
		securityBuffer.pvBuffer = pvBuffer;
	}

	static inline void InitializeSecurityBufferDesc(SecBufferDesc& securityBufferDesc, const std::uint32_t ulVersion, const std::uint32_t cBuffers, SecBuffer* const pBuffers) {
		securityBufferDesc.ulVersion = ulVersion;
		securityBufferDesc.cBuffers = cBuffers;
		securityBufferDesc.pBuffers = pBuffers;
	}

	static inline void FreeBuffers(SecBufferDesc& securityBufferDesc) {
		for (unsigned long i = 0; i < securityBufferDesc.cBuffers; ++i) {
			void* buffer = securityBufferDesc.pBuffers[i].pvBuffer;
			if (buffer) FreeContextBuffer(buffer);
		}
	}

	static inline void SafeFreeBuffers(SecBufferDesc& inputSecurityBufferDesc, SecBufferDesc& outputSecurityBufferDesc, const std::vector<std::uint8_t>& buf) {

		void* buffer = reinterpret_cast<void*>(const_cast<unsigned char*>(buf.data()));

		for (unsigned int i = 0; i < inputSecurityBufferDesc.cBuffers; ++i)
			if (inputSecurityBufferDesc.pBuffers[i].pvBuffer == buffer)
				inputSecurityBufferDesc.pBuffers[i].pvBuffer = nullptr;

		for (unsigned int i = 0; i < outputSecurityBufferDesc.cBuffers; ++i)
			if (outputSecurityBufferDesc.pBuffers[i].pvBuffer == buffer)
				outputSecurityBufferDesc.pBuffers[i].pvBuffer = nullptr;

		FreeBuffers(inputSecurityBufferDesc);
		FreeBuffers(outputSecurityBufferDesc);

	}

}