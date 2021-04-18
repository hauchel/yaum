// Tabelle mit abhängigen für jedes der 81 Felder
const static byte PROGMEM depend [ANZCHK][ANZDEP + 1] = {
  {},
  { 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x13, 0x14, 0x15, 0x1C, 0x25, 0x2E, 0x37, 0x40, 0x49, 0x00},
  { 0x01, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x13, 0x14, 0x15, 0x1D, 0x26, 0x2F, 0x38, 0x41, 0x4A, 0x00},
  { 0x01, 0x02, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x13, 0x14, 0x15, 0x1E, 0x27, 0x30, 0x39, 0x42, 0x4B, 0x00},
  { 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0D, 0x0E, 0x0F, 0x16, 0x17, 0x18, 0x1F, 0x28, 0x31, 0x3A, 0x43, 0x4C, 0x00},
  { 0x01, 0x02, 0x03, 0x04, 0x06, 0x07, 0x08, 0x09, 0x0D, 0x0E, 0x0F, 0x16, 0x17, 0x18, 0x20, 0x29, 0x32, 0x3B, 0x44, 0x4D, 0x00},
  { 0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x09, 0x0D, 0x0E, 0x0F, 0x16, 0x17, 0x18, 0x21, 0x2A, 0x33, 0x3C, 0x45, 0x4E, 0x00},
  { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x08, 0x09, 0x10, 0x11, 0x12, 0x19, 0x1A, 0x1B, 0x22, 0x2B, 0x34, 0x3D, 0x46, 0x4F, 0x00},
  { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x09, 0x10, 0x11, 0x12, 0x19, 0x1A, 0x1B, 0x23, 0x2C, 0x35, 0x3E, 0x47, 0x50, 0x00},
  { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x10, 0x11, 0x12, 0x19, 0x1A, 0x1B, 0x24, 0x2D, 0x36, 0x3F, 0x48, 0x51, 0x00}, //9
  { 0x01, 0x02, 0x03, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x1C, 0x25, 0x2E, 0x37, 0x40, 0x49, 0x00},
  { 0x01, 0x02, 0x03, 0x0A, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x1D, 0x26, 0x2F, 0x38, 0x41, 0x4A, 0x00},
  { 0x01, 0x02, 0x03, 0x0A, 0x0B, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x1E, 0x27, 0x30, 0x39, 0x42, 0x4B, 0x00},
  { 0x04, 0x05, 0x06, 0x0A, 0x0B, 0x0C, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x16, 0x17, 0x18, 0x1F, 0x28, 0x31, 0x3A, 0x43, 0x4C, 0x00},
  { 0x04, 0x05, 0x06, 0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12, 0x16, 0x17, 0x18, 0x20, 0x29, 0x32, 0x3B, 0x44, 0x4D, 0x00},
  { 0x04, 0x05, 0x06, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x10, 0x11, 0x12, 0x16, 0x17, 0x18, 0x21, 0x2A, 0x33, 0x3C, 0x45, 0x4E, 0x00},
  { 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x11, 0x12, 0x19, 0x1A, 0x1B, 0x22, 0x2B, 0x34, 0x3D, 0x46, 0x4F, 0x00},
  { 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x12, 0x19, 0x1A, 0x1B, 0x23, 0x2C, 0x35, 0x3E, 0x47, 0x50, 0x00},
  { 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x19, 0x1A, 0x1B, 0x24, 0x2D, 0x36, 0x3F, 0x48, 0x51, 0x00}, //18
  { 0x01, 0x02, 0x03, 0x0A, 0x0B, 0x0C, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x25, 0x2E, 0x37, 0x40, 0x49, 0x00},
  { 0x01, 0x02, 0x03, 0x0A, 0x0B, 0x0C, 0x13, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1D, 0x26, 0x2F, 0x38, 0x41, 0x4A, 0x00},
  { 0x01, 0x02, 0x03, 0x0A, 0x0B, 0x0C, 0x13, 0x14, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1E, 0x27, 0x30, 0x39, 0x42, 0x4B, 0x00},
  { 0x04, 0x05, 0x06, 0x0D, 0x0E, 0x0F, 0x13, 0x14, 0x15, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1F, 0x28, 0x31, 0x3A, 0x43, 0x4C, 0x00},
  { 0x04, 0x05, 0x06, 0x0D, 0x0E, 0x0F, 0x13, 0x14, 0x15, 0x16, 0x18, 0x19, 0x1A, 0x1B, 0x20, 0x29, 0x32, 0x3B, 0x44, 0x4D, 0x00},
  { 0x04, 0x05, 0x06, 0x0D, 0x0E, 0x0F, 0x13, 0x14, 0x15, 0x16, 0x17, 0x19, 0x1A, 0x1B, 0x21, 0x2A, 0x33, 0x3C, 0x45, 0x4E, 0x00},
  { 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x1A, 0x1B, 0x22, 0x2B, 0x34, 0x3D, 0x46, 0x4F, 0x00},
  { 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1B, 0x23, 0x2C, 0x35, 0x3E, 0x47, 0x50, 0x00},
  { 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x24, 0x2D, 0x36, 0x3F, 0x48, 0x51, 0x00}, //27
  { 0x01, 0x0A, 0x13, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2E, 0x2F, 0x30, 0x37, 0x40, 0x49, 0x00},
  { 0x02, 0x0B, 0x14, 0x1C, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2E, 0x2F, 0x30, 0x38, 0x41, 0x4A, 0x00},
  { 0x03, 0x0C, 0x15, 0x1C, 0x1D, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2E, 0x2F, 0x30, 0x39, 0x42, 0x4B, 0x00},
  { 0x04, 0x0D, 0x16, 0x1C, 0x1D, 0x1E, 0x20, 0x21, 0x22, 0x23, 0x24, 0x28, 0x29, 0x2A, 0x31, 0x32, 0x33, 0x3A, 0x43, 0x4C, 0x00},
  { 0x05, 0x0E, 0x17, 0x1C, 0x1D, 0x1E, 0x1F, 0x21, 0x22, 0x23, 0x24, 0x28, 0x29, 0x2A, 0x31, 0x32, 0x33, 0x3B, 0x44, 0x4D, 0x00},
  { 0x06, 0x0F, 0x18, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x22, 0x23, 0x24, 0x28, 0x29, 0x2A, 0x31, 0x32, 0x33, 0x3C, 0x45, 0x4E, 0x00},
  { 0x07, 0x10, 0x19, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x2B, 0x2C, 0x2D, 0x34, 0x35, 0x36, 0x3D, 0x46, 0x4F, 0x00},
  { 0x08, 0x11, 0x1A, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x24, 0x2B, 0x2C, 0x2D, 0x34, 0x35, 0x36, 0x3E, 0x47, 0x50, 0x00},
  { 0x09, 0x12, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x2B, 0x2C, 0x2D, 0x34, 0x35, 0x36, 0x3F, 0x48, 0x51, 0x00}, //36
  { 0x01, 0x0A, 0x13, 0x1C, 0x1D, 0x1E, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x37, 0x40, 0x49, 0x00},
  { 0x02, 0x0B, 0x14, 0x1C, 0x1D, 0x1E, 0x25, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x38, 0x41, 0x4A, 0x00},
  { 0x03, 0x0C, 0x15, 0x1C, 0x1D, 0x1E, 0x25, 0x26, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x39, 0x42, 0x4B, 0x00},
  { 0x04, 0x0D, 0x16, 0x1F, 0x20, 0x21, 0x25, 0x26, 0x27, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x31, 0x32, 0x33, 0x3A, 0x43, 0x4C, 0x00},
  { 0x05, 0x0E, 0x17, 0x1F, 0x20, 0x21, 0x25, 0x26, 0x27, 0x28, 0x2A, 0x2B, 0x2C, 0x2D, 0x31, 0x32, 0x33, 0x3B, 0x44, 0x4D, 0x00},
  { 0x06, 0x0F, 0x18, 0x1F, 0x20, 0x21, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2B, 0x2C, 0x2D, 0x31, 0x32, 0x33, 0x3C, 0x45, 0x4E, 0x00},
  { 0x07, 0x10, 0x19, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2C, 0x2D, 0x34, 0x35, 0x36, 0x3D, 0x46, 0x4F, 0x00},
  { 0x08, 0x11, 0x1A, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2D, 0x34, 0x35, 0x36, 0x3E, 0x47, 0x50, 0x00},
  { 0x09, 0x12, 0x1B, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x34, 0x35, 0x36, 0x3F, 0x48, 0x51, 0x00}, //45
  { 0x01, 0x0A, 0x13, 0x1C, 0x1D, 0x1E, 0x25, 0x26, 0x27, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x40, 0x49, 0x00},
  { 0x02, 0x0B, 0x14, 0x1C, 0x1D, 0x1E, 0x25, 0x26, 0x27, 0x2E, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x38, 0x41, 0x4A, 0x00},
  { 0x03, 0x0C, 0x15, 0x1C, 0x1D, 0x1E, 0x25, 0x26, 0x27, 0x2E, 0x2F, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x39, 0x42, 0x4B, 0x00},
  { 0x04, 0x0D, 0x16, 0x1F, 0x20, 0x21, 0x28, 0x29, 0x2A, 0x2E, 0x2F, 0x30, 0x32, 0x33, 0x34, 0x35, 0x36, 0x3A, 0x43, 0x4C, 0x00},
  { 0x05, 0x0E, 0x17, 0x1F, 0x20, 0x21, 0x28, 0x29, 0x2A, 0x2E, 0x2F, 0x30, 0x31, 0x33, 0x34, 0x35, 0x36, 0x3B, 0x44, 0x4D, 0x00},
  { 0x06, 0x0F, 0x18, 0x1F, 0x20, 0x21, 0x28, 0x29, 0x2A, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x34, 0x35, 0x36, 0x3C, 0x45, 0x4E, 0x00},
  { 0x07, 0x10, 0x19, 0x22, 0x23, 0x24, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x35, 0x36, 0x3D, 0x46, 0x4F, 0x00},
  { 0x08, 0x11, 0x1A, 0x22, 0x23, 0x24, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x36, 0x3E, 0x47, 0x50, 0x00},
  { 0x09, 0x12, 0x1B, 0x22, 0x23, 0x24, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x3F, 0x48, 0x51, 0x00}, //54
  { 0x01, 0x0A, 0x13, 0x1C, 0x25, 0x2E, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x49, 0x4A, 0x4B, 0x00},
  { 0x02, 0x0B, 0x14, 0x1D, 0x26, 0x2F, 0x37, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x49, 0x4A, 0x4B, 0x00},
  { 0x03, 0x0C, 0x15, 0x1E, 0x27, 0x30, 0x37, 0x38, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x49, 0x4A, 0x4B, 0x00},
  { 0x04, 0x0D, 0x16, 0x1F, 0x28, 0x31, 0x37, 0x38, 0x39, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x43, 0x44, 0x45, 0x4C, 0x4D, 0x4E, 0x00},
  { 0x05, 0x0E, 0x17, 0x20, 0x29, 0x32, 0x37, 0x38, 0x39, 0x3A, 0x3C, 0x3D, 0x3E, 0x3F, 0x43, 0x44, 0x45, 0x4C, 0x4D, 0x4E, 0x00},
  { 0x06, 0x0F, 0x18, 0x21, 0x2A, 0x33, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3D, 0x3E, 0x3F, 0x43, 0x44, 0x45, 0x4C, 0x4D, 0x4E, 0x00},
  { 0x07, 0x10, 0x19, 0x22, 0x2B, 0x34, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3E, 0x3F, 0x46, 0x47, 0x48, 0x4F, 0x50, 0x51, 0x00},
  { 0x08, 0x11, 0x1A, 0x23, 0x2C, 0x35, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3F, 0x46, 0x47, 0x48, 0x4F, 0x50, 0x51, 0x00},
  { 0x09, 0x12, 0x1B, 0x24, 0x2D, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x46, 0x47, 0x48, 0x4F, 0x50, 0x51, 0x00}, //63
  { 0x01, 0x0A, 0x13, 0x1C, 0x25, 0x2E, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x00},
  { 0x02, 0x0B, 0x14, 0x1D, 0x26, 0x2F, 0x37, 0x38, 0x39, 0x40, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x00},
  { 0x03, 0x0C, 0x15, 0x1E, 0x27, 0x30, 0x37, 0x38, 0x39, 0x40, 0x41, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x00},
  { 0x04, 0x0D, 0x16, 0x1F, 0x28, 0x31, 0x3A, 0x3B, 0x3C, 0x40, 0x41, 0x42, 0x44, 0x45, 0x46, 0x47, 0x48, 0x4C, 0x4D, 0x4E, 0x00},
  { 0x05, 0x0E, 0x17, 0x20, 0x29, 0x32, 0x3A, 0x3B, 0x3C, 0x40, 0x41, 0x42, 0x43, 0x45, 0x46, 0x47, 0x48, 0x4C, 0x4D, 0x4E, 0x00},
  { 0x06, 0x0F, 0x18, 0x21, 0x2A, 0x33, 0x3A, 0x3B, 0x3C, 0x40, 0x41, 0x42, 0x43, 0x44, 0x46, 0x47, 0x48, 0x4C, 0x4D, 0x4E, 0x00},
  { 0x07, 0x10, 0x19, 0x22, 0x2B, 0x34, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x47, 0x48, 0x4F, 0x50, 0x51, 0x00},
  { 0x08, 0x11, 0x1A, 0x23, 0x2C, 0x35, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x48, 0x4F, 0x50, 0x51, 0x00},
  { 0x09, 0x12, 0x1B, 0x24, 0x2D, 0x36, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x4F, 0x50, 0x51, 0x00}, //72
  { 0x01, 0x0A, 0x13, 0x1C, 0x25, 0x2E, 0x37, 0x38, 0x39, 0x40, 0x41, 0x42, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x00},
  { 0x02, 0x0B, 0x14, 0x1D, 0x26, 0x2F, 0x37, 0x38, 0x39, 0x40, 0x41, 0x42, 0x49, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x00},
  { 0x03, 0x0C, 0x15, 0x1E, 0x27, 0x30, 0x37, 0x38, 0x39, 0x40, 0x41, 0x42, 0x49, 0x4A, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x00},
  { 0x04, 0x0D, 0x16, 0x1F, 0x28, 0x31, 0x3A, 0x3B, 0x3C, 0x43, 0x44, 0x45, 0x49, 0x4A, 0x4B, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x00},
  { 0x05, 0x0E, 0x17, 0x20, 0x29, 0x32, 0x3A, 0x3B, 0x3C, 0x43, 0x44, 0x45, 0x49, 0x4A, 0x4B, 0x4C, 0x4E, 0x4F, 0x50, 0x51, 0x00},
  { 0x06, 0x0F, 0x18, 0x21, 0x2A, 0x33, 0x3A, 0x3B, 0x3C, 0x43, 0x44, 0x45, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4F, 0x50, 0x51, 0x00},
  { 0x07, 0x10, 0x19, 0x22, 0x2B, 0x34, 0x3D, 0x3E, 0x3F, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x50, 0x51, 0x00},
  { 0x08, 0x11, 0x1A, 0x23, 0x2C, 0x35, 0x3D, 0x3E, 0x3F, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x51, 0x00},
  { 0x09, 0x12, 0x1B, 0x24, 0x2D, 0x36, 0x3D, 0x3E, 0x3F, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x00}, //81
};

