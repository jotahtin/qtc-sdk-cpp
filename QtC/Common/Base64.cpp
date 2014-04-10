/* -*- mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
** File:       QtC/Common/Base64.cpp
** Copyright:  Copyright (c) 2014, Digia Plc. All rights reserved.
**             All other trademarks are the property of their respective owners.
** Comment:    Base64
** Author(s):  Jorma Tahtinen <Jorma.Tahtinen@digia.com>
*/

#include <sstream>

#include "QtC/Common/Base64.h"

namespace QtC {
  
  namespace Base64 {
    
    /*
    ** Base64 - Encoder
    */
    
    Encoder::Encoder() 
      : iStep(StepA), iResult(0), iStepCount(0)
    {
    }
    
    int Encoder::encodeBlock(const char* aInput, size_t aInputLength, char* aOutput) {
      const char* input = aInput;
      const char* const inputEnd = aInput + aInputLength;
      char* output = aOutput;
      char result;
      char fragment;
      
      result = iResult;
      
      switch (iStep) {
	while (1) {
	case StepA:
	  if (input == inputEnd) {
	    iResult = result;
	    iStep = StepA;
	    return output - aOutput;
	  }
	  fragment = *input++;
	  result = (fragment & 0x0fc) >> 2;
	  *output++ = encodeValue(result);
	  result = (fragment & 0x003) << 4;
	case StepB:
	  if (input == inputEnd) {
	    iResult = result;
	    iStep = StepB;
	    return output - aOutput;
	  }
	  fragment = *input++;
	  result |= (fragment & 0x0f0) >> 4;
	  *output++ = encodeValue(result);
	  result = (fragment & 0x00f) << 2;
	case StepC:
	  if (input == inputEnd) {
	    iResult = result;
	    iStep = StepC;
	    return output - aOutput;
	  }
	  fragment = *input++;
	  result |= (fragment & 0x0c0) >> 6;
	  *output++ = encodeValue(result);
	  result  = (fragment & 0x03f) >> 0;
	  *output++ = encodeValue(result);
	  
	  if (++iStepCount == CHARS_PER_LINE/4) {
	    *output++ = '\n';
	    iStepCount = 0;
	  }
	}
      }
      /* control should not reach here */
      return output - aOutput;
    }
    
    int Encoder::encodeBlockEnd(char* aOutput) {
      char* output = aOutput;
      
      switch (iStep) {
      case StepB:
	*output++ = encodeValue(iResult);
	*output++ = '=';
	*output++ = '=';
	break;
      case StepC:
	*output++ = encodeValue(iResult);
	*output++ = '=';
	break;
      case StepA:
	break;
      }
      
      // not for small messages
      // *output++ = '\n';
      
      return output - aOutput;
    }
    
    
    char Encoder::encodeValue(char aValueIn) {
      static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
	"ghijklmnopqrstuvwxyz0123456789+/";
      if (aValueIn > 63) return '=';
      return encoding[(int)aValueIn];
    }
    
    void encode(std::istream& aInputStream, std::ostream& aOutputStream) {
      const int BUFFER_SIZE = 8192;
      Encoder encoder;
      
      char* plaintext = new char[BUFFER_SIZE];
      char* code = new char[2*BUFFER_SIZE];
      int plainlength;
      int codelength;
      
      do {
	aInputStream.read(plaintext, BUFFER_SIZE);
	plainlength = aInputStream.gcount();
	codelength = encoder.encodeBlock(plaintext, plainlength, code);
	aOutputStream.write(code, codelength);
      }
      while (aInputStream.good() && plainlength > 0);
      
      codelength = encoder.encodeBlockEnd(code);
      aOutputStream.write(code, codelength);
      
      delete [] code;
      delete [] plaintext;
    }
    std::string encode(const std::string &aInput) {
      std::istringstream inputStream(aInput);
      std::ostringstream outputStream;
      encode(inputStream,outputStream);
      return outputStream.str();
    }
    
    /*
    ** Base64 - Decoder
    */
    
    Decoder::Decoder()
      : iStep(StepA), iPlainChar(0)
    {}
    
    int Decoder::decodeBlock(const char* aInput, const int aInputLength, char* aOutput) {
      const char* codechar = aInput;
      char* plainchar = aOutput;
      char fragment;
      
      *plainchar = iPlainChar;
      
      switch (iStep) {
	while (1) {
	case StepA:
	  do {
	    if (codechar == aInput+aInputLength) {
	      iStep = StepA;
	      iPlainChar = *plainchar;
	      return plainchar - aOutput;
	    }
	    fragment = (char)decodeValue(*codechar++);
	  } while (fragment < 0);
	  *plainchar    = (fragment & 0x03f) << 2;
	case StepB:
	  do {
	    if (codechar == aInput+aInputLength) {
	      iStep = StepB;
	      iPlainChar = *plainchar;
	      return plainchar - aOutput;
	    }
	    fragment = (char)decodeValue(*codechar++);
	  } while (fragment < 0);
	  *plainchar++ |= (fragment & 0x030) >> 4;
	  *plainchar    = (fragment & 0x00f) << 4;
	case StepC:
	  do {
	    if (codechar == aInput+aInputLength) {
	      iStep = StepC;
	      iPlainChar = *plainchar;
	      return plainchar - aOutput;
	    }
	    fragment = (char)decodeValue(*codechar++);
	  } while (fragment < 0);
	  *plainchar++ |= (fragment & 0x03c) >> 2;
	  *plainchar    = (fragment & 0x003) << 6;
	case StepD:
	  do {
	    if (codechar == aInput+aInputLength) {
	      iStep = StepD;
	      iPlainChar = *plainchar;
	      return plainchar - aOutput;
	    }
	    fragment = (char)decodeValue(*codechar++);
	  } while (fragment < 0);
	  *plainchar++   |= (fragment & 0x03f);
	}
      }
      /* control should not reach here */
      return plainchar - aOutput;
    }
    
    int Decoder::decodeValue(char aValue) {
      static const char decoding[] = {
	62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,
	-1,-1,-2,-1,-1,-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
	-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,
	36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51
      };
      static const char decoding_size = sizeof(decoding);
      
      aValue -= 43;
      if (aValue < 0 || aValue > decoding_size) return -1;
      return decoding[(int)aValue];
    }
    
    void decode(std::istream& aInputStream, std::ostream& aOutputStream) {
      const int BUFFER_SIZE = 8192;
      Decoder decoder;
      
      char* code = new char[BUFFER_SIZE];
      char* plaintext = new char[BUFFER_SIZE];
      int codelength;
      int plainlength;
      
      do {
	aInputStream.read((char*)code, BUFFER_SIZE);
	codelength = aInputStream.gcount();
	plainlength = decoder.decodeBlock(code, codelength, plaintext);
	aOutputStream.write((const char*)plaintext, plainlength);
      } while (aInputStream.good() && codelength > 0);
      
      delete [] code;
      delete [] plaintext;
    }
    
    std::string decode(const std::string &aInput) {
      std::istringstream inputStream(aInput);
      std::ostringstream outputStream;
      decode(inputStream,outputStream);
      return outputStream.str();
    }
    
  } /* namespace Base64 */
  
} /* namespace QtC */
