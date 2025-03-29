message(STATUS "DiagnosticCMake loaded")
include(CheckIncludeFileCXX)
include(CheckTypeSize)

check_include_file_cxx("cstdint" HAS_CSTDINT)
check_type_size("uint8_t" HAS_UINT8_T)
check_type_size("SiameseOriginalPacket" HAS_SIAM_ORIGINAL_PACKET)
check_type_size("SiameseRecoveryPacket" HAS_SIAM_RECOVERY_PACKET)