/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/Common/Base64.h
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    Base64
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#ifndef QTC_COMMON_BASE64_H
#define QTC_COMMON_BASE64_H

#include <stddef.h>

#include <string>
#include <iostream>

namespace QtC {
  
  namespace Base64 {
  
    class Encoder {
      enum { CHARS_PER_LINE = 72 };
      enum EncoderStep { StepA, StepB, StepC };
    public:
      Encoder();
    
      int encodeBlock(const char* aInput, size_t aInputLength, char* aOutput);
      int encodeBlockEnd(char* aOutput);
    protected:  
      char encodeValue(char aValueIn);
    private:
      EncoderStep iStep;
      char iResult;
      int iStepCount;
    };

    void encode(std::istream& aInputStream, std::ostream& aOutputStream);
    std::string encode(const std::string &aInput);
    
    class Decoder {
      enum DecoderStep { StepA, StepB, StepC, StepD };
    public:
      Decoder();
      int decodeBlock(const char* aInput, const int aInputLength, char* aOutput);
    private:
      int decodeValue(char aValue);
    private:
      DecoderStep iStep;
      char	iPlainChar;
    };
    
    void decode(std::istream& aInputStream, std::ostream& aOutputStream);
    std::string decode(const std::string &aInput);
  
  } /* namespace Base64 */
    
} /* namespace QtC */

#endif /* QTC_COMMON_BASE64_H */
