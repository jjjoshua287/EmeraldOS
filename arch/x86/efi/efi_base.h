#ifndef EFI_BASE_H
#define EFI_BASE_H

#include "types.h"

typedef u64 RETURN_STATUS;

#define MAX_BIT 0x8000000000000000ULL

#define ENCODE_ERROR(StatusCode) ((RETURN_STATUS)(MAX_BIT | (StatusCode)))
#define RETURN_ERROR(StatusCode) (((RETURN_STATUS)(StatusCode)) >= MAX_BIT)

// The operation completed successfully.
#define RETURN_SUCCESS  (RETURN_STATUS)(0)

// The image failed to load. 
#define RETURN_LOAD_ERROR  ENCODE_ERROR (1)

// The parameter was incorrect.
#define RETURN_INVALID_PARAMETER  ENCODE_ERROR (2)
 
// The operation is not supported. 
#define RETURN_UNSUPPORTED  ENCODE_ERROR (3)

// The buffer was not the proper size for the request. 
#define RETURN_BAD_BUFFER_SIZE  ENCODE_ERROR (4)

/* The buffer was not large enough to hold the requested data.
   The required buffer size is returned in the appropriate
   parameter when this error occurs. 
*/
#define RETURN_BUFFER_TOO_SMALL  ENCODE_ERROR (5)

// There is no data pending upon return.
#define RETURN_NOT_READY  ENCODE_ERROR (6)

// The physical device reported an error while attempting the operation.
#define RETURN_DEVICE_ERROR  ENCODE_ERROR (7)

// The device can not be written to. 
#define RETURN_WRITE_PROTECTED  ENCODE_ERROR (8)

// The resource has run out. 
#define RETURN_OUT_OF_RESOURCES  ENCODE_ERROR (9)
 
// An inconsistency was detected on the file system causing the operation to fail.
#define RETURN_VOLUME_CORRUPTED  ENCODE_ERROR (10)

// There is no more space on the file system. 
#define RETURN_VOLUME_FULL  ENCODE_ERROR (11)

// The device does not contain any medium to perform the operation. 
#define RETURN_NO_MEDIA  ENCODE_ERROR (12)

// The medium in the device has changed since the last access. 
#define RETURN_MEDIA_CHANGED  ENCODE_ERROR (13)

// The item was not found. 
#define RETURN_NOT_FOUND  ENCODE_ERROR (14)

// Access was denied. 
#define RETURN_ACCESS_DENIED  ENCODE_ERROR (15)

// The server was not found or did not respond to the request. 
#define RETURN_NO_RESPONSE  ENCODE_ERROR (16)

// A mapping to the device does not exist. 
#define RETURN_NO_MAPPING  ENCODE_ERROR (17)

// A timeout time expired. 
#define RETURN_TIMEOUT  ENCODE_ERROR (18)

// The protocol has not been started. 
#define RETURN_NOT_STARTED  ENCODE_ERROR (19)

// The protocol has already been started. 
#define RETURN_ALREADY_STARTED  ENCODE_ERROR (20)

// The operation was aborted. 
#define RETURN_ABORTED  ENCODE_ERROR (21)

// An ICMP error occurred during the network operation. 
#define RETURN_ICMP_ERROR  ENCODE_ERROR (22)

// A TFTP error occurred during the network operation. 
#define RETURN_TFTP_ERROR  ENCODE_ERROR (23)

// A protocol error occurred during the network operation. 
#define RETURN_PROTOCOL_ERROR  ENCODE_ERROR (24)

// A function encountered an internal version that was
// incompatible with a version requested by the caller. 
#define RETURN_INCOMPATIBLE_VERSION  ENCODE_ERROR (25)

// The function was not performed due to a security violation. 
#define RETURN_SECURITY_VIOLATION  ENCODE_ERROR (26)

// A CRC error was detected. 
#define RETURN_CRC_ERROR  ENCODE_ERROR (27)

// The beginning or end of media was reached. 
#define RETURN_END_OF_MEDIA  ENCODE_ERROR (28)

// The end of the file was reached. 
#define RETURN_END_OF_FILE  ENCODE_ERROR (31)

// The language specified was invalid. 
#define RETURN_INVALID_LANGUAGE  ENCODE_ERROR (32)

/* The security status of the data is unknown or compromised
   and the data must be updated or replaced to restore a valid
   security status. 
*/ 
#define RETURN_COMPROMISED_DATA  ENCODE_ERROR (33)

// There is an address conflict address allocation. 
#define RETURN_IP_ADDRESS_CONFLICT  ENCODE_ERROR (34)

// A HTTP error occurred during the network operation. 
#define RETURN_HTTP_ERROR  ENCODE_ERROR (35)

/* The string contained one or more characters that
   the device could not render and were skipped.
*/
#define RETURN_WARN_UNKNOWN_GLYPH  ENCODE_WARNING (1)

// The handle was closed, but the file was not deleted. 
#define RETURN_WARN_DELETE_FAILURE  ENCODE_WARNING (2)

/* The handle was closed, but the data to the file was not
   flushed properly.
*/
#define RETURN_WARN_WRITE_FAILURE  ENCODE_WARNING (3)

/* The resulting buffer was too small, and the data was
   truncated to the buffer size.
*/
#define RETURN_WARN_BUFFER_TOO_SMALL  ENCODE_WARNING (4)

// The data has not been updated within the timeframe set by
// local policy for this type of data. 
#define RETURN_WARN_STALE_DATA  ENCODE_WARNING (5)

// The resulting buffer contains UEFI-compliant file system.
#define RETURN_WARN_FILE_SYSTEM  ENCODE_WARNING (6)

// The operation will be processed across a system reset. 
#define RETURN_WARN_RESET_REQUIRED  ENCODE_WARNING (7)

#endif // EFI_BASE_H