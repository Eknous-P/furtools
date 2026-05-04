#ifndef _FURCOMMON_H
#define _FURCOMMON_H

#include "common.h"

namespace Furnace {
  typedef u32 BlockMagic;
  typedef char LongMagic[16];
  typedef u16 Version;
  typedef u32 BlockSize;
  typedef u32 BLockPtr;

  constexpr BlockMagic MAGIC(u8 a, u8 b, u8 c, u8 d) {return (BlockMagic)a|(b<<8)|(c<<16)|(d<<24);}

  enum FileOperationError {
    fileOK=0,
    fileEOF,
    fileMagicError,
    fileBlockSizeMismatch,
    fileInvalidInsFeature,
    fileInvalidHeaderSize,
    fileEmptyMacro,
    fileTooManyItems,
  };

  constexpr Version latestVersion=246;
}


#endif
