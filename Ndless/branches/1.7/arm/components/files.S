#include <os.h>

  .xdef fileExists
  .xdef getFileSize
  .xdef openFileBufferized
  .xdef copyFile

# --------------------------------------------------------------------------
# Check if a file exists
#
# Input:
#   r0 = File name
#
# Output:
#   0 if the file exists, else -1
# --------------------------------------------------------------------------
fileExists:
  mov     r12, sp
  stmfd   sp!, {r1, r11-r12, lr, pc}
  sub     sp, sp, #0x28       @ stat struct size
  mov     r1, sp
  oscall  stat
  add     sp, sp, #0x28
  ldmfd   sp, {r1, r11, sp, pc}

# --------------------------------------------------------------------------
# Returns file size
#
# Input:
#   r0 = File name
#
# Output:
#   The size of the file else -1
# --------------------------------------------------------------------------
getFileSize:
  mov     r12, sp
  stmfd   sp!, {r1, r11-r12, lr, pc}
  sub     sp, sp, #0x28       @ stat struct size
  mov     r11, sp
  mov     r1, r11
  oscall  stat
  cmp     r0, #0
  bne     _getFileSize_error

_getFileSize_extractSizeOffset:
  ldr     r0, [r11, #0x14]    @ File size offset
  b       _getFileSize_end
  
_getFileSize_error:
  mov     r0, #-1
  
_getFileSize_end:
  add     sp, sp, #0x28
  ldmfd   sp, {r1, r11, sp, pc}

# --------------------------------------------------------------------------
# Open a file and copy the content into a buffer
#
# Input:
#   r0 = File name
#
# Output:
#   Returns the buffer address else NULL
# --------------------------------------------------------------------------
openFileBufferized:
  mov     r12, sp
  stmfd   sp!, {r1-r7, r11-r12, lr, pc}

  mov     r5, r0              @ file name
  
  # Open file
  adr     r1, rb
  oscall  fopen
  cmp     r0, #0
  beq     _openFileBufferized_end
  mov     r6, r0              @ file descriptor
  
  # Calculate source file size
  mov     r0, r5
  bl      getFileSize
  mov     r7, r0              @ file size
  
  # Allocate memory
  oscall  malloc
  cmp     r0, #0
  beq     _openFileBufferized_end
  mov     r4, r0              @ buffer address
  
  # Read content
  mov     r1, r7
  mov     r2, #1
  mov     r3, r6
  oscall  fread
  
  # Close file
  mov     r0, r6
  oscall  fclose
  
  # Return the buffer address
  mov     r0, r4
  
_openFileBufferized_end:  
  ldmfd   sp, {r1-r7, r11, sp, pc}

# --------------------------------------------------------------------------
# Copy a source file to a destination file
#
# Input:
#   r0 = Source file name
#   r1 = Destination file name
#
# Output:
#   Returns 0 if success else -1
# --------------------------------------------------------------------------
copyFile:
  mov     r12, sp
  stmfd   sp!, {r2-r7, r11-r12, lr, pc}

  mov     r5, r0              @ source file
  mov     r6, r1              @ destination file

  # Open the source file and create a buffer with its content
  bl      openFileBufferized
  cmp     r0, #0
  beq     _copyFile_error
  mov     r4, r0
  
  # Open the destination file
  mov     r0, r6
  adr     r1, wb
  oscall  fopen
  mov     r7, r0              @ destination file descriptor
  
  # Get the source file size
  mov     r0, r5
  bl      getFileSize
  mov     r1, r0
  
  # Write buffer content
  mov     r0, r4
  mov     r2, #1
  mov     r3, r7
  oscall  fwrite
  
  mov     r0, r7
  oscall  fclose
  
  # Unallocate memory
  mov     r0, r4
  oscall  free
  
  mov     r0, #0
  b       _copyFile_end
  
_copyFile_error:
  mov     r0, #-1
  
_copyFile_end:
  ldmfd   sp, {r2-r7, r11, sp, pc}

rb:   .string "rb"
wb:   .string "wb"
  .align
