///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "gsLargeInt.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Many parameters are gsi_u32* instead of gsLargeInt_t*.  
//    This was done to allow easy conversion of databuffer to gsLargeInt_t
//    Raw buffer destinations must have enough space to store the result
static gsi_bool gsiLargeIntPrint(FILE* logFile, const gsi_u32 *data, gsi_u32 length);
static gsi_bool gsiLargeIntResize(gsLargeInt_t *lint, gsi_u32 length);
static gsi_bool gsiLargeIntStripLeadingZeroes(gsLargeInt_t* lint);
static gsi_bool gsiLargeIntSizePower2(const gsLargeInt_t *src1, const gsLargeInt_t *src2, gsi_u32 *lenout);
static gsi_i32  gsiLargeIntCompare(const gsi_u32 *data1, gsi_u32 len1, const gsi_u32 *data2, gsi_u32 len2);

static gsi_bool gsiLargeIntKMult(const gsi_u32 *data1, const gsi_u32 *data2, gsi_u32 length, gsi_u32 *dest, gsi_u32 *lenout, gsi_u32 maxlen);
static gsi_bool gsiLargeIntMult (const gsi_u32 *data1, gsi_u32 length1, const gsi_u32 *data2, gsi_u32 length2, gsi_u32 *dest, gsi_u32 *lenout, gsi_u32 maxlen);
static gsi_bool gsiLargeIntDiv  (const gsi_u32 *src1, gsi_u32 length1, const gsLargeInt_t *divisor, gsLargeInt_t *dest, gsLargeInt_t *remainder);

// Dest may be data1 or data2 to support in-place arithmetic
static gsi_bool gsiLargeIntAdd  (const gsi_u32 *data1, gsi_u32 length1, const gsi_u32 *data2, gsi_u32 length2, gsi_u32 *dest, gsi_u32 *lenout, gsi_u32 maxlen);
static gsi_bool gsiLargeIntSub  (const gsi_u32 *amount, gsi_u32 length1, const gsi_u32 *from, gsi_u32 length2, gsi_u32 *dest, gsi_u32 *lenout);

// Special division, removes divisor directly from src1, leaving remainder
static gsi_bool gsiLargeIntSubDivide(gsi_u32 *src1, gsi_u32 length, const gsi_u32 *divisor, gsi_u32 dlen, gsi_u32 highbit, gsi_u32 *quotient);

// Montgomery utilities
//gsi_bool gsiLargeIntSquareM(const gsLargeInt_t *src, const gsLargeInt_t *mod, gsi_u32 modPrime, gsi_u32 R, gsLargeInt_t *dest);
gsi_bool gsiLargeIntMultM(gsLargeInt_t *src1, gsLargeInt_t *src2, const gsLargeInt_t *mod, gsi_u32 modPrime, gsLargeInt_t *dest);
gsi_bool gsiLargeIntInverseMod(const gsLargeInt_t *mod, gsi_u32 *modPrimeOut);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
 // execution timing/profiling
#define GS_LINT_TIMING
#ifdef GS_LINT_TIMING

typedef enum 
{
	GSLintTimerMult,  // "regular" multiplication
	GSLintTimerMultM, // montgomery
	GSLintTimerKMult, // karatsuba
	GSLintTimerAdd,
	GSLintTimerSub,   // subtract
	GSLintTimerDiv,
	GSLintTimerSubDivide, // atomic divide
	GSLintTimerSquareMod,
	GSLintTimerPowerMod, // modular exponentiation

	GSLintTimerCount
} GSLintTimerID;

typedef struct GSLintTimer
{
	gsi_time started;
	gsi_time total;
	gsi_u32  entries;
	gsi_u32  running; // already entered?
} GSLintTimer;
static struct GSLintTimer gTimers[GSLintTimerCount];

static void gsiLargeIntTimerEnter(GSLintTimerID id)
{
	if (gTimers[id].running==0)
	{
		gTimers[id].entries++;
		gTimers[id].started = current_time_hires();
		gTimers[id].running = 1;
	}
}
static void gsiLargeIntTimerExit(GSLintTimerID id)
{
	if (gTimers[id].running==1)
	{
		gTimers[id].total += current_time_hires()-gTimers[id].started;
		gTimers[id].running = 0;
	}
}

#define GSLINT_ENTERTIMER(id) gsiLargeIntTimerEnter(id)
#define GSLINT_EXITTIMER(id) gsiLargeIntTimerExit(id)

#else
#define GSLINT_ENTERTIMER(id)
#define GSLINT_EXITTIMER(id)
#endif
 



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
gsi_bool gsLargeIntSetValue(gsLargeInt_t *lint, gsi_u32 value)
{
	lint->mLength = 1;
	lint->mData[0] = value;
	return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Resize by:
//    Padding a GSLINT with leading zeroes.
//    or stripping lead zeroes.
// This function will not strip digits other than zero.
gsi_bool gsiLargeIntResize(gsLargeInt_t *lint, gsi_u32 length)
{
	if (length > GS_LARGEINT_INT_SIZE)
		return gsi_false;

	// strip leading zeroes until length is reached
	if (lint->mLength >= length)
	{
		while(lint->mLength > length && lint->mData[lint->mLength-1]==0)
			lint->mLength--; // check each digit to make sure it's zero
		if (lint->mLength == length)
			return gsi_true;
		else
			return gsi_false;
	}

	// otherwise, add zeroes until length is reached
	else
	{
		memset(&lint->mData[lint->mLength], 0, (length-lint->mLength)*sizeof(gsi_u32));
		lint->mLength = length;
		return gsi_true;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Makes two GSLINT the same size, the size being a power of 2
//  NOTE: Testing next multiple of two, not power of 2
gsi_bool gsiLargeIntSizePower2(const gsLargeInt_t *src1, const gsLargeInt_t *src2, gsi_u32 *lenout)
{
	unsigned int i = 0;

	int len1 = (int)src1->mLength;
	int len2 = (int)src2->mLength;

	// strip leading zeroes
	while(len1>0 && src1->mData[len1-1] == 0)
		len1--;
	while(len2>0 && src2->mData[len2-1] == 0)
		len2--;

	// set to longer length
	*lenout = (gsi_u32)max(len1, len2);
	
	// search for power of two >= length
	//   (this length is in digits, not bits)
	i=1;
	while(i < *lenout)
		i = i<<1;
	*lenout = i;

	if (*lenout > GS_LARGEINT_INT_SIZE)
		return gsi_false;
	return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Compare two integers
//     -1 = data1 < data2
//      0 = data1 = data2
//      1 = data1 > data2
static gsi_i32 gsiLargeIntCompare(const gsi_u32 *data1, gsi_u32 len1, const gsi_u32 *data2, gsi_u32 len2)
{
	// skip leading whitespace, if any
	while(data1[len1-1] == 0 && len1>0)
		len1--;
	while(data2[len2-1] == 0 && len2>0)
		len2--;
	if (len1<len2)
		return -1;
	else if (len1>len2)
		return 1;
	else 
	{
		// same size, compare digits
		while(len1 > 0)
		{
			if (data1[len1-1] < data2[len1-1])
				return -1;
			else if (data1[len1-1] > data2[len1-1])
				return 1;
			len1--;
		}
	}
	return 0; // equal!
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static gsi_bool gsiLargeIntStripLeadingZeroes(gsLargeInt_t* lint)
{
	while(lint->mLength >0 && lint->mData[lint->mLength-1]==0)
		lint->mLength--;
	return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Addition may cause overflow
gsi_bool gsLargeIntAdd(const gsLargeInt_t *src1, const gsLargeInt_t *src2, gsLargeInt_t *dest)
{
	gsi_bool result = gsiLargeIntAdd(src1->mData, src1->mLength, src2->mData, src2->mLength, dest->mData, &dest->mLength, GS_LARGEINT_INT_SIZE);
	if (gsi_is_false(result))
		memset(dest, 0, sizeof(gsLargeInt_t)); // overflow
	return result;
}

// len: In value = maxsize
//      Out value = actual size
static gsi_bool gsiLargeIntAdd(const gsi_u32 *data1, gsi_u32 length1, const gsi_u32 *data2, gsi_u32 length2, gsi_u32 *dest, gsi_u32 *lenout, gsi_u32 maxlen)
{
	gsi_u32 i=0;
	gsi_u64 carry = 0; // to hold overflow

	gsi_u32 shorterLen = 0;
	gsi_u32 longerLen = 0;
	//const gsi_u32 *shorterSrc = NULL;
	const gsi_u32 *longerSrc = NULL;

	GSLINT_ENTERTIMER(GSLintTimerAdd);

	if (maxlen < length1 || maxlen < length2)
		return gsi_false; // dest not large enough, OVERFLOW

	if (length1 < length2)
	{
		shorterLen = length1;
		//shorterSrc = data1;
		longerLen = length2;
		longerSrc = data2;
	}
	else
	{
		shorterLen = length2;
		//shorterSrc = data2;
		longerLen = length1;
		longerSrc = data1;
	}

	// Add digits until the shorterSrc's length is reached
	while(i < shorterLen)
	{
		carry += (gsi_u64)data1[i] + data2[i];
		dest[i] = (gsi_u32)carry;
		carry = carry >> 32;
		i++;
	}

	// Continue adding until carry is zero
	while((carry > 0) && (i < longerLen))
	{
		carry += (gsi_u64)longerSrc[i];
		dest[i] = (gsi_u32)carry;
		carry = carry >> 32;
		i++;
	}

	// Is there still a carry?
	//    do not perform length check here, so that we can support oversized buffers
	if (carry > 0) // && i < GS_LARGEINT_INT_SIZE)
	{
		if (maxlen <= i)
			return gsi_false; // OVERFLOW, no room for extra digit
		dest[i++] = (gsi_u32)carry;
		carry = 0;
	}

	// Copy the rest of the bytes straight over (careful of memory overlap)
	//    this can't happen if there was a carry (see above carry>0 check)
	if (i < longerLen)
	{
		// check overlap
		if (&dest[i] != &longerSrc[i])
			memcpy(&dest[i], &longerSrc[i], (longerLen-i)*sizeof(gsi_u32));
		i = longerLen;
	}
	*lenout = i;

	GSLINT_EXITTIMER(GSLintTimerAdd);

	if (carry)
		return gsi_false; // overflow
	else
		return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Subtraction may cause underflow
//   subtracts src1 FROM src2
//   strips leading zeroes (gsiLargeIntSub doesn't strip for compatability with karatsuba fixed size numbers)
gsi_bool gsLargeIntSub(const gsLargeInt_t *src1, const gsLargeInt_t *src2, gsLargeInt_t *dest)
{
	gsi_bool result = gsiLargeIntSub(src1->mData, src1->mLength, src2->mData, src2->mLength, dest->mData, &dest->mLength);
	if (gsi_is_true(result))
		gsiLargeIntStripLeadingZeroes(dest);
	return result;
}

gsi_bool gsiLargeIntSub(const gsi_u32 *src1, gsi_u32 length1, const gsi_u32 *src2, gsi_u32 length2, gsi_u32 *dest, gsi_u32 *lenout)
{
	gsi_u64 borrow = 0; // to hold overflow
	gsi_u32 shorterLen = min(length1, length2);
	gsi_u32 i=0;

	GSLINT_ENTERTIMER(GSLintTimerSub);

	//printf("--From: ");
	//gsiLargeIntPrint(src2, length2);
	//printf("--Subtracting: ");
	//gsiLargeIntPrint(src1, length1);

	// Subtract digits
	while(i < shorterLen)
	{
		borrow = (gsi_u64)src2[i] - src1[i] - borrow;
		dest[i] = (gsi_u32)borrow;
		borrow = borrow>>63; // shift to last bit.  This will be 1 if negative, 0 if positive
		i++;
	}
	while(i < length2)
	{
		borrow = (gsi_u64)src2[i]-borrow;
		dest[i] = (gsi_u32)borrow;
		borrow = borrow>>63;
		i++;
	}

	// check for underflow
	if (borrow != 0)
	{
		GSLINT_EXITTIMER(GSLintTimerSub);
		return gsi_false;
	}
	while(length1 > i) // make sure remaining digits are only leading zeroes
	{
		if (src1[i] != 0)
		{
			GSLINT_EXITTIMER(GSLintTimerSub);
			return gsi_false;
		}
		i++;
	}

	// Don't reduce length from subtraction, instead keep leading zeroes
	// (do this for ease of use with Karatsuba which requires Power2 length)
	*lenout = length2;

	GSLINT_EXITTIMER(GSLintTimerSub);
	return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Multiply using normal method (use KMult when working with LargeInt*LargeInt)
gsi_bool gsLargeIntMult(const gsLargeInt_t *src1, const gsLargeInt_t *src2, gsLargeInt_t *dest)
{
	gsi_bool result = gsiLargeIntMult(src1->mData, src1->mLength, src2->mData, src2->mLength, dest->mData, &dest->mLength, GS_LARGEINT_INT_SIZE);
	if (gsi_is_false(result))
		memset(dest, 0, sizeof(gsLargeInt_t)); // overflow
	return result;
}

static gsi_bool gsiLargeIntMult(const gsi_u32 *data1, gsi_u32 length1, const gsi_u32 *data2, gsi_u32 length2, gsi_u32 *dest, gsi_u32 *lenout, gsi_u32 maxlen)
{
	unsigned int i=0;
	unsigned int k=0;

	gsLargeInt_t temp;
	memset(&temp, 0, sizeof(temp));
	*lenout = 0;

	GSLINT_ENTERTIMER(GSLintTimerMult);

	for(i=0; i<length2; i++)
	{
		// don't have to multiply by 0
		if(data2[i] != 0)
		{
			// multiply data1 by data2[i]
			for (k=0; k<length1; k++)
			{
				// carry starts out as product
				//   (it is mathematically impossible for carry to overflow
				//    at the first addition [see below])
				gsi_u64 carry = (gsi_u64)data1[k] * data2[i];
				unsigned int digit = (unsigned int)(i+k);
				if (digit >= maxlen)
				{
					GSLINT_EXITTIMER(GSLintTimerMult);
					return gsi_false; // overflow
				}
				while(carry)
				{
					carry += temp.mData[digit];
					temp.mData[digit] = (gsi_u32)carry;
					carry = carry >> 32;
					digit++;
					if ((digit > maxlen) ||
						(digit == maxlen && carry>0))
					{
						GSLINT_EXITTIMER(GSLintTimerMult);
						return gsi_false; // overflow
					}
				}
				if (digit > (gsi_i32)temp.mLength)
					temp.mLength = (gsi_u32)digit;
			}
		}
	}
	// copy into destination (calculate length at this time)
	while(temp.mLength>0 && temp.mData[temp.mLength-1] == 0)
		temp.mLength--; // strip leading zeroes
	*lenout = temp.mLength;
	memcpy(dest, temp.mData, (*lenout)*sizeof(gsi_u32));

	GSLINT_EXITTIMER(GSLintTimerMult);
	return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// divide src1 by divisor
gsi_bool gsLargeIntDiv(const gsLargeInt_t *src1, const gsLargeInt_t *divisor, gsLargeInt_t *dest, gsLargeInt_t *remainder)
{
	// call the free-buffer version
	return gsiLargeIntDiv(src1->mData, src1->mLength, divisor, dest, remainder);
}

// length1 can be, at most, 2*GS_LARGEINT_INT_SIZE
static gsi_bool gsiLargeIntDiv(const gsi_u32 *src, gsi_u32 len, const gsLargeInt_t *div, gsLargeInt_t *dest, gsLargeInt_t *remainder)
{
	gsi_i32 result = 0; // temp, to store compare result
	gsi_i32 divisorHighBit = GS_LARGEINT_BYTE_SIZE-1; // pre-calculate this

	// Bytes used from src1
	int readIndex = 0;
	int readLength = 0;

	// setup scratch copies 
	gsLargeInt_t quotient;

	gsi_u32  scopy[GS_LARGEINT_INT_SIZE*2];  // we support double length source for division, when dest is null
	gsi_u32  scopyLen = len;

	const gsi_u32* divisorData = div->mData;
	gsi_u32  divisorLen = div->mLength;

	gsi_bool endLoop = gsi_false;
	
	GSLINT_ENTERTIMER(GSLintTimerDiv);

	// we only support oversized sources for calculating a remainder
	//    e.g. dest must be null
	if (scopyLen > GS_LARGEINT_INT_SIZE && dest != NULL)
		return gsi_false;

	// strip leading zeroes (from our scratch copies)
	while(scopyLen>0 && src[scopyLen-1]==0)
		scopyLen--;
	while(divisorLen>0 && divisorData[divisorLen-1]==0)
		divisorLen--;

	memcpy(scopy, src, scopyLen*sizeof(gsi_u32));
	memset(&quotient, 0, sizeof(quotient)); 

	// check the unusual cases
	if (scopyLen==0 || divisorLen==0)
	{
		if (dest)
		{
			dest->mData[0] = 0;
			dest->mLength = 0;
		}
		if (remainder)
		{
			remainder->mData[0] = 0;
			remainder->mLength = 0;
		}

		GSLINT_EXITTIMER(GSLintTimerDiv);

		if (divisorLen == 0)
			return gsi_false; // division by zero
		else
			return gsi_true; // zero divided, this is legal
	}
	if (gsiLargeIntCompare(scopy, scopyLen, divisorData, divisorLen)==-1)
	{
		// divisor is larger than source
		if (dest)
		{
			dest->mLength = 0;
			dest->mData[0] = 0;
		}
		remainder->mLength = scopyLen;
		memcpy(remainder->mData, scopy, scopyLen*sizeof(gsi_u32));
		GSLINT_EXITTIMER(GSLintTimerDiv);
		return gsi_true;
	}
	
	// calculate the divisor high bit
	while((divisorData[divisorLen-1]&(1<<(gsi_u32)divisorHighBit))==0 && divisorHighBit>=0)
		divisorHighBit--;
	if (divisorHighBit == -1)
	{
		GSLINT_EXITTIMER(GSLintTimerDiv);
		return gsi_false; // divide by zero
	}
	divisorHighBit += (divisorLen-1)*GS_LARGEINT_BYTE_SIZE;
	
	// position "sliding" window for first interation
	// 41529 / [71389]2564
	// WARNING: digits are indexed [2][1][0], first byte to read is index[2]
	readIndex = (int)(scopyLen - divisorLen);
	readLength = (int)divisorLen;

	//if (readIndex < 0)
	//	_asm {int 3}; // overflow readIndex
	
	do
	{
		result = gsiLargeIntCompare(&scopy[readIndex], (gsi_u32)readLength, divisorData, divisorLen);
		if (result == -1)
		{
			// scopy window is smaller, we'll need an extra digit
			if (readIndex > 0)
			{
				readIndex--; 
				readLength++;
			}
			else
			{
				// no more digits!
				endLoop = gsi_true;
			}
		}
		else if (result == 0)
		{
			// not likely! set digits to zero and slide window
			memset(&scopy[readIndex], 0, readLength*sizeof(gsi_u32));
			quotient.mData[readIndex] += 1;
			if (quotient.mLength < (gsi_u32)readIndex+readLength)
				quotient.mLength = (gsi_u32)readIndex+readLength;
			readIndex -= readLength;
			readLength = 1;

			if (readIndex < 0)
				endLoop = gsi_true;; // no more digits
		}
		else
		{
			// subtract directly onto our temp copy, so we don't have to worry about carry values
			gsi_u32 quotientTemp = 0;
			//if (readLength > 0xffff)
			//	_asm {int 3}
			if (gsi_is_false(gsiLargeIntSubDivide(&scopy[readIndex], (gsi_u32)readLength, divisorData, divisorLen, (gsi_u32)divisorHighBit, &quotientTemp)))
			{
				// overflow
				GSLINT_EXITTIMER(GSLintTimerDiv);
				return gsi_false;
			}
			quotient.mData[readIndex] += quotientTemp;
			if (quotient.mLength < (gsi_u32)readIndex+readLength)
				quotient.mLength = (gsi_u32)readIndex+readLength;
			// remove new leading zeroes
			while(scopy[readIndex+readLength-1] == 0 && readLength>1)
				readLength--;
			while(scopy[readIndex+readLength-1] == 0 && readIndex>1)
				readIndex--;
		}
	}
	while(gsi_is_false(endLoop));

	// no more digits, leftover is remainder
	if (readIndex >= 0)
	{
		memcpy(remainder->mData, &scopy[readIndex], readLength*sizeof(gsi_u32));
		remainder->mLength = (gsi_u32)readLength;
	}
	else
	{
		remainder->mData[0] = 0;
		remainder->mLength = 0;
	}

	// save off quotient, if desired
	if (dest)
	{
		memcpy(dest->mData, quotient.mData, quotient.mLength*sizeof(gsi_u32));
		dest->mLength = quotient.mLength;
	}
	GSLINT_EXITTIMER(GSLintTimerDiv);
	return gsi_true;
}


// atomic divide.  
//    Subtract divisor directly from src.
//    Leave remainder in src.
static gsi_bool gsiLargeIntSubDivide(gsi_u32 *src, gsi_u32 length, const gsi_u32 *divisor, gsi_u32 dlen, 
									 gsi_u32 highbit, gsi_u32 *quotient)
{
	gsi_u64 aboveBits = 0;
	gsLargeInt_t temp; // stores temporary product before subtraction
	gsLargeInt_t quotientCopy; // copy of quotient, length padded for multiplication

	GSLINT_ENTERTIMER(GSLintTimerSubDivide);
	// assert(src > divisor)
	// assert(src < (MAX_DIGIT_VALUE * divisor))
	//if(dlen==1 && *divisor==0)
	//	_asm {int 3} // division by zero

	// Q: how many times to subtract?
	// A: we estimate by taking the bits in src above the highest bit in divisor
	if (length > dlen)
		aboveBits = (src[length-2]&divisor[dlen-1]) | ((gsi_u64)src[length-1]<<GS_LARGEINT_BYTE_SIZE);
	else
		aboveBits = src[length-1];
	aboveBits /= divisor[dlen-1];

	memset(&quotientCopy, 0, sizeof(quotientCopy));
	quotientCopy.mData[0] = (gsi_u32)(aboveBits);
	quotientCopy.mData[1] = (gsi_u32)(aboveBits>>GS_LARGEINT_BYTE_SIZE);

	// We only support quotients up to MAX_INT
	if (quotientCopy.mData[1] != 0)
	{
		quotientCopy.mData[0] = (gsi_u32)(-1);
		quotientCopy.mData[1] = 0;
	}
	quotientCopy.mLength = 1;
		
	// multiply this value by divisor, and that's how much to subtract
	if (gsi_is_false(gsiLargeIntMult(divisor, dlen, quotientCopy.mData, quotientCopy.mLength, temp.mData, &temp.mLength, GS_LARGEINT_INT_SIZE)))
	{
		GSLINT_EXITTIMER(GSLintTimerSubDivide);
		return gsi_false; // overflow
	}

	// while subtraction amount is larger than src, reduce it
	while(gsiLargeIntCompare(temp.mData, temp.mLength, src, length)==1)
	{
		// divide by two
		quotientCopy.mData[0] = quotientCopy.mData[0]>>1;
		//if (quotientCopy.mData[0] == 0)
		//	_asm {int 3}
		if (gsi_is_false(gsiLargeIntMult(divisor, dlen, quotientCopy.mData, quotientCopy.mLength, temp.mData, &temp.mLength, GS_LARGEINT_INT_SIZE)))
		{
			GSLINT_EXITTIMER(GSLintTimerSubDivide);
			return gsi_false; // overflow
		}
	}
	//if (gsiLargeIntCompare(temp.mData, temp.mLength, src, length)==1)
	//	_asm {int 3} // temp > src, subtraction will cause underflow!
			
	// subtract it
	gsiLargeIntSub(temp.mData, temp.mLength, src, length, src, &length);

	*quotient = quotientCopy.mData[0];
	//if (quotientCopy.mData[1] != 0)
	//	_asm {int 3}
	GSLINT_EXITTIMER(GSLintTimerSubDivide);

	GSI_UNUSED(highbit);
	return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Multiply using Karatsuba
//   Karatsuba requires that the sizes be equal and a power of two
gsi_bool gsLargeIntKMult(const gsLargeInt_t *src1, const gsLargeInt_t *src2, gsLargeInt_t *dest)
{
	gsi_u32 len = 0;
	gsi_bool result = gsi_false;

	gsLargeInt_t temp; // to prevent issues if (src1 == src2 == dest)

	// quick check for multiplication by 0
	if (src1->mLength == 0 || src2->mLength == 0)
	{
		dest->mLength = 0;
		return gsi_true;
	}

	// when length is small it's faster to use "normal" multiplication
	if (max(src1->mLength,src2->mLength) < GS_LARGEINT_KARATSUBA_CUTOFF)
		return gsLargeIntMult(src1, src2, dest);

	// Check for size/length restrictions
	result = gsiLargeIntSizePower2(src1, src2, &len);
	if (gsi_is_false(result) || len>(GS_LARGEINT_INT_SIZE/2))
	{
		// try regular multiplication
		return gsLargeIntMult(src1, src2, dest); 
	}

	// (don't time above section since it defers to Mult)
	GSLINT_ENTERTIMER(GSLintTimerKMult);

	// clear the temporary dest
	memset(&temp, 0, sizeof(gsLargeInt_t));
	temp.mLength = 0;

	// resize if necessary
	if (src1->mLength != len || src2->mLength != len)
	{
		// size is not correct, make a copy then multiply
		gsLargeInt_t src1Copy;
		gsLargeInt_t src2Copy;
		memcpy(&src1Copy, src1, sizeof(gsLargeInt_t));
		memcpy(&src2Copy, src2, sizeof(gsLargeInt_t));
		gsiLargeIntResize(&src1Copy, len);
		gsiLargeIntResize(&src2Copy, len);

		result = gsiLargeIntKMult(src1Copy.mData, src2Copy.mData, len, temp.mData, &temp.mLength, GS_LARGEINT_INT_SIZE);
	}
	else
	{
		// size is correct, perform multiplication
		result = gsiLargeIntKMult(src1->mData, src2->mData, len, temp.mData, &temp.mLength, GS_LARGEINT_INT_SIZE);
	}
	if (gsi_is_true(result))
	{
		// strip leading zeroes and copy into dest
		gsiLargeIntStripLeadingZeroes(&temp);
		memcpy(dest, &temp, sizeof(gsLargeInt_t));
	}
	GSLINT_EXITTIMER(GSLintTimerKMult);
	return result;
}


// Utility for Karasuba
static gsi_bool gsiLargeIntKMult(const gsi_u32 *data1, const gsi_u32 *data2, gsi_u32 length,
								 gsi_u32 *dest, gsi_u32 *lenout, gsi_u32 maxlen)
{
	// No timer here, this function is only called from GSLINTKMult
	//GSLINT_ENTERTIMER(GSLintTimerKMult);

	// "normal" multiplication is faster when length is small
	if (length <= GS_LARGEINT_KARATSUBA_CUTOFF)
		return gsiLargeIntMult(data1, length, data2, length, dest, lenout, maxlen);
	else
	{
		gsLargeInt_t temp1, temp2, temp3;
		unsigned int halfLen = length>>1;    

		temp1.mLength = 0;
		temp2.mLength = 0;
		temp3.mLength = 0;

		//printf("Karasuba splitting at %d (1/2 = %d)\r\n", length, halfLen);

		// Karatsuba:  k = 12*34
		//  a = (1*3)
		//  b = (1+2)*(3+4)-a-c
		//  c = (2*4)
		//  k = a*B^N+b*B^(N/2)+c = a*100+b*10+c

		// Enter the recursive portion
		//   TH = top half
		//   BH = bottom half

		// Note that since (a*B^N + c) cannot overlap, we can immediately store both in dest

		// Compute a. (TH of data1 * TH of data2)
		//      Stores in TH of dest, so later *B^N isn't necessary
		//      For the example, this puts 1*3 into the high half 03xx
		gsiLargeIntKMult(&data1[halfLen], &data2[halfLen], halfLen, &dest[length], lenout, maxlen-length);
		//printf("Calculated A (%d) = ", *lenout);
		//gsiLargeIntPrint(&dest[length], *lenout);

		// Compute c. (BH of data1 * BH of data2)
		//      For the example, this puts 2*4 into the low half xx08
		gsiLargeIntKMult(data1, data2, halfLen, dest, lenout, maxlen);
		//printf("Calculated C (%d) = ", *lenout);
		//gsiLargeIntPrint(dest, *lenout);

		// Compute b1. (TH of data1 + BH of data1) 
		gsiLargeIntAdd(&data1[halfLen], halfLen, data1, halfLen, temp1.mData, &temp1.mLength, GS_LARGEINT_INT_SIZE);
		//printf("Calculated B1 (%d) = ", temp1.mLength);
		//gsiLargeIntPrint(temp1.mData, temp1.mLength);

		// Compute b2. (TH of data2 + BH of data2)
		gsiLargeIntAdd(&data2[halfLen], halfLen, data2, halfLen, temp2.mData, &temp2.mLength, GS_LARGEINT_INT_SIZE);
		//printf("Calculated B2 (%d) = ", temp2.mLength);
		//gsiLargeIntPrint(temp2.mData, temp2.mLength);

		// Compute b3. (b1*b2) (*B^N)
		//      For the example, (1+2)(3+4)*B^N = 21*B^N = 0210
		memset(&temp3, 0, sizeof(gsLargeInt_t));
		
		// May require resizing, but don't go above halfLen
		if (temp1.mLength > halfLen || temp2.mLength > halfLen)
			gsiLargeIntMult(temp1.mData, temp1.mLength, temp2.mData, temp2.mLength, &temp3.mData[halfLen], &temp3.mLength, GS_LARGEINT_INT_SIZE-halfLen);
		else
		{
			gsi_bool result = gsiLargeIntSizePower2(&temp1, &temp2, lenout);
			if (gsi_is_false(result))
				return gsi_false; // could not resize
			gsiLargeIntResize(&temp1, *lenout); // pad to new size
			gsiLargeIntResize(&temp2, *lenout); // pad to new size
			gsiLargeIntKMult(temp1.mData, temp2.mData, *lenout, &temp3.mData[halfLen], &temp3.mLength, GS_LARGEINT_INT_SIZE-halfLen);
		}
		temp3.mLength += halfLen; // fix length for temp3
		//if (temp3.mLength > GS_LARGEINT_INT_SIZE)
		//	_asm {int 3} // this should be at most temp1.mLength+temp2.mLength
		memset(temp3.mData, 0, halfLen*sizeof(gsi_u32));
		//printf("Calculated B3 (%d) = ", temp3.mLength);
		//gsiLargeIntPrint(&temp3.mData[halfLen], temp3.mLength-halfLen);

		// Compute final b. (b3-a-c) (*B^N)
		//      Note: The subtraction is in terms of (*B^N)
		//      For the example, 021x - 03x - 08x = 0100
		gsiLargeIntSub(&dest[length], length, &temp3.mData[halfLen], temp3.mLength-halfLen, &temp3.mData[halfLen], &temp3.mLength);
		temp3.mLength += halfLen;
		gsiLargeIntSub( dest        , length, &temp3.mData[halfLen], temp3.mLength-halfLen, &temp3.mData[halfLen], &temp3.mLength);
		temp3.mLength += halfLen;
		//printf("Calculated B (%d) = ", temp3.mLength);
		//gsiLargeIntPrint(temp3.mData, temp3.mLength);

		// Add em up
		//      Dest already contains A+C, so Add B
		//      For the example, 0308 + 0100 = 0408 (the correct answer)
		gsiLargeIntAdd(dest, length*2, temp3.mData, temp3.mLength, dest, lenout, maxlen);
	}
	// strip leading zeroes from dest
	while(*lenout > 0 && dest[*lenout-1] == 0)
		*lenout = *lenout-1;

	return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
gsi_bool gsLargeIntSquareMod(const gsLargeInt_t *lint, const gsLargeInt_t *mod, gsLargeInt_t *dest)
{
	int i = 0;
	int k = 0;
	int len = (gsi_i32)lint->mLength; // signed version
	gsi_u64 carry = 0;
	int oldShiftBit = 0;
	int newShiftBit = 0;
	gsi_bool result = gsi_false;

	gsi_u32 squareSums[GS_LARGEINT_INT_SIZE*2];   // temp dest for square sums
	gsi_u32 otherSums[GS_LARGEINT_INT_SIZE*2];    // temp dest for other sums
	gsi_u32 squareLen = 0;
	gsi_u32 otherLen = 0;

	GSLINT_ENTERTIMER(GSLintTimerSquareMod);

	memset(&squareSums, 0, sizeof(squareSums));
	memset(&otherSums, 0, sizeof(otherSums));

	// Go through each digit, multiplying with each other digit
	// (only do this once per pair, since AB == BA)
	// Ex: ABC * ABC, we want AB,AC,BC only
	for (i=1; i < len; i++)
	{
		for(k=0; k < i; k++)
		{
			carry += (gsi_u64)lint->mData[i]*lint->mData[k] + otherSums[i+k];
			otherSums[i+k] = (gsi_u32)carry;
			carry  = carry >> 32;
		}
		if(carry)
		{
			otherSums[i+k] = (gsi_u32)carry;
			carry = carry >> 32;
		}
	}

	// Multiply by 2 (because each internal pair appears twice)
	for (i=0; i < (2*len); i++)
	{
		newShiftBit = (otherSums[i] & 0x80000000)==0x80000000?1:0; // calc next carry 1 or 0
		otherSums[i] = (otherSums[i] << 1) + oldShiftBit; // do the shift
		oldShiftBit = newShiftBit;
	}
	// don't worry about left-overy carry because this can't overflow
	// maxlen N-digit*N-digit = 2n-digit

	// Go through each digit, multiplying with itself
	for (i=0; i <len; i++)
	{
		carry = (gsi_u64)lint->mData[i] * lint->mData[i];
		squareSums[i*2] = (gsi_u32)carry;
		squareSums[i*2+1] = (gsi_u32)(carry >> 32);
	}
	squareLen = (gsi_u32)2*len;
	otherLen = (gsi_u32)2*len; 

	// Add the two together
	result = gsiLargeIntAdd(otherSums, otherLen, squareSums, squareLen, squareSums, &squareLen, GS_LARGEINT_INT_SIZE*2);
	result = gsiLargeIntDiv(squareSums, squareLen, mod, NULL, dest);

	GSLINT_EXITTIMER(GSLintTimerSquareMod);
	return result;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Montgomery exponentiation (see HAC 14.94)
//
// SPECIAL NOTE:
//    A small public exponent will reduce the load on client encryption.
//    (below 65535 is a security risk, so don't go too small)
gsi_bool gsLargeIntPowerMod(const gsLargeInt_t *b, const gsLargeInt_t *p, const gsLargeInt_t *m, gsLargeInt_t *dest)
{
	int i=0; // temp/counter
	int k=0; // temp/counter
	
	gsi_u32 modPrime;

	gsi_u32 expHighBit; // highest bit set in exponent;

	gsLargeInt_t R; // "R" as used in the montgomery exponentiation algorithm.
	gsLargeInt_t Rmod;   // R%mod
	gsLargeInt_t R2mod;  // R^2%mod
	gsLargeInt_t temp;
	gsLargeInt_t xwiggle; // montgomery mult of (x,R2mod)

	gsLargeInt_t base;
	gsLargeInt_t power;
	gsLargeInt_t mod;

	GSLINT_ENTERTIMER(GSLintTimerPowerMod);

	memset(&R, 0, sizeof(R));
	memset(&Rmod, 0, sizeof(Rmod));
	memset(&R2mod, 0, sizeof(R2mod));
	memset(&temp, 0, sizeof(temp));
	memset(&xwiggle, 0, sizeof(xwiggle));

	memcpy(&base, b, sizeof(base));
	memcpy(&power, p, sizeof(power));
	memcpy(&mod, m, sizeof(mod));

	gsiLargeIntStripLeadingZeroes(&base);
	gsiLargeIntStripLeadingZeroes(&power);
	gsiLargeIntStripLeadingZeroes(&mod);

	// Catch the unusual cases
	if (mod.mLength == 0)
	{
		// mod 0 = undefined
		dest->mLength = 0;
		GSLINT_EXITTIMER(GSLintTimerPowerMod);
		return gsi_false;
	}
	else if (mod.mLength==1 && mod.mData[0]==1)
	{
		// mod 1 = 0
		dest->mLength = 0;
		dest->mData[0] = 0;
		GSLINT_EXITTIMER(GSLintTimerPowerMod);
		return gsi_true;
	}
	else if (power.mLength == 0)
	{
		// x^0 = 1
		dest->mLength = 1;
		dest->mData[0] = 1;
		GSLINT_EXITTIMER(GSLintTimerPowerMod);
		return gsi_true;
	}
	else if ((mod.mData[0]&1) == 0)
	{
		// Montgomery only works with odd modulus!
		// (rsa modulus is prime1*prime2, which must be odd)
		dest->mLength = 0;
		dest->mData[0] = 0;
		//_asm {int 3}
		GSLINT_EXITTIMER(GSLintTimerPowerMod);
		return gsi_false;
	}
	// If base is larger than mod, we can (must) reduce it
	if (gsiLargeIntCompare(base.mData, base.mLength, mod.mData, mod.mLength)!=-1)
	{
		gsLargeIntDiv(&base, &mod, NULL, &base);
	}
	if (base.mLength == 0)
	{
		// 0^e = 0
		dest->mLength = 0;
		dest->mData[0] = 0;
		GSLINT_EXITTIMER(GSLintTimerPowerMod);
		return gsi_true;
	}

	// find the highest bit set in power
	expHighBit=GS_LARGEINT_BYTE_SIZE;
	while(((1<<(expHighBit-1))&power.mData[power.mLength-1]) == 0)
		expHighBit--;
	expHighBit += ((power.mLength-1) * GS_LARGEINT_BYTE_SIZE); // add in 32 bits for each extra byte
	
	// On to the tricky tricky!
	//    1) We can't compute B^P and later apply the mod; B^P is just too big
	//       So we have to make modular reductions along the way
	//    2) Since modular reduction is essentially a division, we would like
	//       to use a mod 2^E so that division is just a bit strip.
	//       ex. (1383 mod 16) = binary(0000010101100111 mod 00010000) = 00000111 = dec 7

	// Precalculate some values that will come up repeatedly

	// calculate "R" (if mod=5678, R=10000 e.g. One digit higher)
	memset(&R, 0, sizeof(R));
	R.mLength = mod.mLength+1;
	if (R.mLength > GS_LARGEINT_INT_SIZE)
		return gsi_false; // you need to increase the large int capacity
	R.mData[R.mLength-1] = 1; // set first bit one byte higher than mod

	// find the multiplicative inverse of mod
	gsiLargeIntInverseMod(&mod, &modPrime);

	// calculate Rmod (R%mod)
	if (gsi_is_false(gsLargeIntDiv(&R, &mod, NULL, &Rmod)))
	{
		GSLINT_EXITTIMER(GSLintTimerPowerMod);
		return gsi_false;
	}

	// calculate R2mod  (R^2%mod = (Rmod*Rmod)%mod)
	if (gsi_is_false(gsLargeIntSquareMod(&Rmod, &mod, &R2mod)))
	{
		GSLINT_EXITTIMER(GSLintTimerPowerMod);
		return gsi_false;
	}

	// calculate xwiggle
	if (gsi_is_false(gsiLargeIntMultM(&base, &R2mod, &mod, modPrime, &xwiggle)))
	{
		GSLINT_EXITTIMER(GSLintTimerPowerMod);
		return gsi_false;
	}

	// loop through the BITS of power
	memcpy(dest, &Rmod, sizeof(gsLargeInt_t)); // start dest at Rmod
	for (i=(int)(expHighBit-1); i>=0; i--)
	{
		// mont square the current total
		gsiLargeIntMultM(dest, dest, &mod, modPrime, dest);
		k = (i/GS_LARGEINT_BYTE_SIZE);
		if ((power.mData[k] & (1<<i))==((gsi_u32)1<<i))
			gsiLargeIntMultM(dest, &xwiggle, &mod, modPrime, dest);
	}

	// Since we're working with Montgomery values (x*R2mod)
	// we have to adjust back to x
	temp.mLength = 1;
	temp.mData[0] = 1;
	gsiLargeIntMultM(dest, &temp, &mod, modPrime, dest);

	GSLINT_EXITTIMER(GSLintTimerPowerMod);
	return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Montgomery multiplication
//    Computes (src1*src2*r^-1)%mod
// Note:
//    This implementation is based on HAC14.36 which has a lot of room for improvement  
//    FLINT algorithm runs approx 30 times faster. 
gsi_bool gsiLargeIntMultM(gsLargeInt_t *x, gsLargeInt_t *y, const gsLargeInt_t *m, gsi_u32 modPrime, gsLargeInt_t *dest)
{
	int i=0;
	gsi_u64 xiy0;
	gsi_u32 u = 0;

	gsLargeInt_t A;
	gsLargeInt_t xiy;
	gsLargeInt_t temp; 

	GSLINT_ENTERTIMER(GSLintTimerMultM);

	gsiLargeIntStripLeadingZeroes(x);
	gsiLargeIntStripLeadingZeroes(y);

	// Check inputs
	i=(int)(m->mLength);
	while(i>0 && m->mData[i-1]==0)
		i--;
	if (i==0)
	{
		// modulus is zero, answer undefined
		dest->mData[0] = 0;
		dest->mLength = 0;
		GSLINT_EXITTIMER(GSLintTimerMultM);
		return gsi_false;
	}
	if (x->mLength==0)
	{
		// x == 0
		dest->mLength = 0;
		dest->mData[0] = 0;
		GSLINT_EXITTIMER(GSLintTimerMultM);
		return gsi_true;
	}
	if (y->mLength==0)
	{
		// y == 0
		dest->mLength = 0;
		dest->mData[0] = 0;
		GSLINT_EXITTIMER(GSLintTimerMultM);
		return gsi_true;
	}

	// We pad with zeroes so that we don't have to check for overruns in the loop below
	//   (note: resize will not remove non-zero digits from x or y)
	gsiLargeIntResize(x, m->mLength);
	gsiLargeIntResize(y, m->mLength);

	// Continue with the Multiplication
	memset(&A, 0, sizeof(A));
	memset(&temp, 0, sizeof(temp));
	memset(&xiy, 0, sizeof(xiy));
	
	for (i=0; (gsi_u32)i < m->mLength; i++)
	{
		xiy0 = (gsi_u64)x->mData[i]*y->mData[0];  // y[0], NOT y[i] !!
		u = (gsi_u32)((xiy0+A.mData[0])*modPrime); // strip bits over the first digit

		// A = (A+x[i]*y + u[i]*m)/b
		//    compute x[i]*y
		memset(temp.mData, 0, y->mLength*sizeof(gsi_u32)); // clear out a portion of temp
		temp.mData[0] = x->mData[i];
		temp.mLength = y->mLength; // xi padded with zeroes
		if (gsi_is_false(gsiLargeIntMult(temp.mData, temp.mLength, y->mData, y->mLength, xiy.mData, &xiy.mLength, GS_LARGEINT_INT_SIZE)))
		{
			// overflow
			dest->mLength = 0;
			dest->mData[0] = 0;
			GSLINT_EXITTIMER(GSLintTimerMultM);
			return gsi_false;
		}
		//    compute u[i]*m
		memset(temp.mData, 0, m->mLength*sizeof(gsi_u32)); // clear out a portion of temp
		temp.mData[0] = u;
		temp.mLength = m->mLength;
		//if (gsi_is_false(gsiLargeIntMult(temp.mData, temp.mLength, m->mData, m->mLength, temp.mData, &temp.mLength)))
		if (gsi_is_false(gsLargeIntKMult(&temp, m, &temp)))
		{
			// overflow
			dest->mLength = 0;
			dest->mData[0] = 0;
			GSLINT_EXITTIMER(GSLintTimerMultM);
			return gsi_false;
		}
		//    Add both to A
		if (gsi_is_false(gsiLargeIntAdd(xiy.mData, xiy.mLength, A.mData, A.mLength, A.mData, &A.mLength, GS_LARGEINT_INT_SIZE)))
		{
			// overflow
			dest->mLength = 0;
			dest->mData[0] = 0;
			GSLINT_EXITTIMER(GSLintTimerMultM);
			return gsi_false;
		}
		if (gsi_is_false(gsiLargeIntAdd(temp.mData, temp.mLength, A.mData, A.mLength, A.mData, &A.mLength, GS_LARGEINT_INT_SIZE)))
		{
			// overflow
			dest->mLength = 0;
			dest->mData[0] = 0;
			GSLINT_EXITTIMER(GSLintTimerMultM);
			return gsi_false;
		}
		//    Divide by b  (e.g. Remove first digit from A)
		if (A.mLength > 1)
		{
			memmove(&A.mData[0], &A.mData[1], (A.mLength-1)*sizeof(gsi_u32));
			A.mData[A.mLength-1] = 0;
			A.mLength--;
		}
		else
		{
			A.mLength = 0;
			A.mData[0] = 0;
		}
	}

	//if (A >= m then subtract another m)
	if (gsiLargeIntCompare(A.mData, A.mLength, m->mData, m->mLength)!=-1)
		gsiLargeIntSub(m->mData, m->mLength, A.mData, A.mLength, dest->mData, &dest->mLength);
	else
		memcpy(dest, &A, sizeof(A));
	GSLINT_EXITTIMER(GSLintTimerMultM);
	return gsi_true;
}

/*
//    Computes (src*src*r^-1)%mod
static gsi_bool gsiLargeIntSquareM(const gsLargeInt_t *src, const gsLargeInt_t *mod, gsi_u32 modPrime, gsi_u32 R, gsLargeInt_t *dest)
{
	GSI_UNUSED(src);
	GSI_UNUSED(mod);
	GSI_UNUSED(modPrime);
	GSI_UNUSED(R);
	GSI_UNUSED(dest);
	assert(0);
	return gsi_true;
}*/


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Calculate multiplicative inverse of mod, (-mod^-1 mod 2^R)
//    ala. Dusse and Kaliski, extended Euclidean algorithm
gsi_bool gsiLargeIntInverseMod(const gsLargeInt_t *mod, gsi_u32 *dest)
{
	gsi_u64 x=2;
	gsi_u64 y=1;
	gsi_u64 check = 0;

	gsi_u32 i=2;
	for (i = 2; i <= GS_LARGEINT_BYTE_SIZE; i++)
	{
		check = (gsi_u64)mod->mData[0] * y;
		if (x < (check & ((x<<1)-1)))
			y += x;
		x = x << 1;
	}
	*dest = (gsi_u32)(x-y);
	return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
gsi_bool gsLargeIntPrint(FILE* logFile, const gsLargeInt_t *lint)
{
	return gsiLargeIntPrint(logFile, lint->mData, lint->mLength);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
gsi_bool gsiLargeIntPrint(FILE* logFile, const gsi_u32 *data, gsi_u32 length)
{
// this is only specific to NITRO since for other platforms the fprintf will
// resolve to a STDOUT
#if !defined(_NITRO)
	while(length >0)
	{
		fprintf(logFile, "%08X", data[length-1]);
		length--;
	}
	fprintf(logFile, "\r\n");
	return gsi_true;
#else
	GSI_UNUSED(logFile);
	GSI_UNUSED(data);
	GSI_UNUSED(length);
	return gsi_false;			
#endif
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// stream of bytes, big endian.  (first byte = most significant digit)
gsi_bool gsLargeIntSetFromHexString(gsLargeInt_t *lint, const char* hexstream)
{
	gsi_u8* writePos = (gsi_u8*)lint->mData;
	gsi_u32 temp;
	int len = 0;

	GS_ASSERT(hexstream != NULL);
	
	len = (int)strlen(hexstream);
	if (len == 0)
	{
		lint->mLength = 0;
		lint->mData[0] = 0;
		return gsi_true;
	}
	if ((len/2) > (GS_LARGEINT_INT_SIZE*sizeof(gsi_u32)))
		return gsi_false;
	
	// 2 characters per byte, 4 bytes per integer
	lint->mLength = (len+7)/(2*sizeof(gsi_u32));
	lint->mData[lint->mLength-1] = 0; // set last byte to zero for left over characters
	
	while(len>=2)
	{
		sscanf((char*)(hexstream+len-2), "%02x", &temp); // sscanf requires a 4 byte dest
		*writePos++ = (gsi_u8)temp; // then we convert to byte, to ensure correct byte order
		len-=2;
	}
	if (len == 1)
	{
		sscanf((char*)(hexstream+len-1), "%01x", &temp); // sscanf requires a 4 byte dest
		*writePos++ = (gsi_u8)temp; // then we convert to byte, to ensure correct byte order
		len-=1;
	}
	return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Reverse bytes in a LINT, which are LittleEndian
//     ex: Packing an RSA message of which the first bytes are 0x00 0x02
//         The first bytes of the packet must become the MSD of the LINT
gsi_bool gsLargeIntReverseBytes(gsLargeInt_t *lint)
{
	char* left = NULL;
	char* right = NULL;

	char temp = '\0';

	if (lint->mLength == 0)
		return gsi_true;

	left = (char*)&lint->mData[0];
	right = ((char*)&lint->mData[lint->mLength])-1;

	while(left < right)
	{
		temp = *left;
		(*left++) = (*right);
		(*right--) = temp;
	}
	return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// hashing is made complicated by differing byte orders 
void gsLargeIntAddToMD5(gsLargeInt_t * lint, MD5_CTX * md5)
{
	// first, calculate the byte length
	int byteLength = (int)gsLargeIntGetByteLength(lint);
	gsi_u8 * dataStart = NULL;

	if (byteLength == 0)
		return; // no data

	dataStart = (gsi_u8*)lint->mData;
	if ((byteLength % 4) != 0)
		dataStart += 4 - (byteLength % sizeof(gsi_u32));

	// reverse to big-endian (MS) then hash
	gsLargeIntReverseBytes(lint);
	MD5Update(md5, dataStart, (unsigned int)byteLength);
	gsLargeIntReverseBytes(lint);
} 


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Length in bytes so leading zeroes can be dropped from hex strings
gsi_u32  gsLargeIntGetByteLength(gsLargeInt_t *lint)
{
	int intSize = (int)lint->mLength;
	int byteSize = 0;

	// skip leading zeroes
	while(intSize > 0 && lint->mData[intSize-1] == 0)
		intSize --;
	if (intSize == 0)
		return 0; // no data

	byteSize = intSize * 4;
	if (lint->mData[intSize-1] <= 0x000000FF)
		byteSize -= 3;
	else if (lint->mData[intSize-1] <= 0x0000FFFF)
		byteSize -= 2;
	else if (lint->mData[intSize-1] <= 0x00FFFFFF)
		byteSize -= 1;

	return (gsi_u32)byteSize;
}

