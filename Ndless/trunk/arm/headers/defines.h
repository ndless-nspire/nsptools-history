# ----------------------------------
# Hardware
# ----------------------------------
  .set  SCREEN_BASE_ADDRESS,         0xA4000100
  .set  KEY_MAP,                     0x900E0000

# ----------------------------------
# Addresses
# ----------------------------------
  .set  OS_BASE_ADDRESS,             0x10000000

# ----------------------------------
# Others
# ----------------------------------
  .set SCREEN_BYTES_SIZE,            38400

# ----------------------------------
# KEYS
# ----------------------------------
# Offset 0x10
  .set  KEY_NSPIRE_RET,              0b000000000001
  .set  KEY_NSPIRE_ENTER,            0b000000000010
  .set  KEY_NSPIRE_THETA,            0b010000000000

# Offset 0x12
  .set  KEY_NSPIRE_PI,               0b010000000000

# Check if a key is pressed
  .macro is_key_pressed row, column
    ldr     r0, =(KEY_MAP + \row)
    ldrh    r0, [r0]
    tst     r0, \column
  .endm

