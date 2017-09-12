/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2000 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#define _CRT_SECURE_NO_DEPRECATE // get rid of sprintf security warnings on VS2005

#include <math.h>
#include "alMain.h"
#include "AL/al.h"
#include "AL/alc.h"

// fixes for mingw32.
#if defined(max) && !defined(__max)
#define __max max
#endif
#if defined(min) && !defined(__min)
#define __min min
#endif

ALUAPI ALint ALUAPIENTRY aluF2L(ALfloat Value)
{
	double temp;

	temp=Value+(((65536.0*65536.0*16.0)+(65536.0*65536.0*8.0))*65536.0);
	return *((long *)&temp);
}

ALUAPI ALshort ALUAPIENTRY aluF2S(ALfloat Value)
{
	double temp;
	long i;

	temp=Value+(((65536.0*65536.0*16.0)+(65536.0*65536.0*8.0))*65536.0);
	i=(*((long *)&temp));
	if (i>32767)
		i=32767;
	else if (i<-32768)
		i=-32768;
	return ((short)i);
}

ALUAPI ALvoid ALUAPIENTRY aluCrossproduct(ALfloat *inVector1,ALfloat *inVector2,ALfloat *outVector)
{
	outVector[0]=(inVector1[1]*inVector2[2]-inVector1[2]*inVector2[1]);
	outVector[1]=(inVector1[2]*inVector2[0]-inVector1[0]*inVector2[2]);
	outVector[2]=(inVector1[0]*inVector2[1]-inVector1[1]*inVector2[0]);
}

ALUAPI ALfloat ALUAPIENTRY aluDotproduct(ALfloat *inVector1,ALfloat *inVector2)
{
	return (inVector1[0]*inVector2[0]+inVector1[1]*inVector2[1]+inVector1[2]*inVector2[2]);
}

ALUAPI ALvoid ALUAPIENTRY aluNormalize(ALfloat *inVector)
{
	ALfloat length,inverse_length;

	length=(ALfloat)sqrt(aluDotproduct(inVector,inVector));
	if (length != 0)
	{
		inverse_length=(1.0f/length);
		inVector[0]*=inverse_length;
		inVector[1]*=inverse_length;
		inVector[2]*=inverse_length;
	}
}

ALUAPI __inline ALvoid ALUAPIENTRY aluMatrixVector(ALfloat *vector,ALfloat matrix[3][3])
{
	ALfloat result[3];

	result[0]=vector[0]*matrix[0][0]+vector[1]*matrix[1][0]+vector[2]*matrix[2][0];
	result[1]=vector[0]*matrix[0][1]+vector[1]*matrix[1][1]+vector[2]*matrix[2][1];
	result[2]=vector[0]*matrix[0][2]+vector[1]*matrix[1][2]+vector[2]*matrix[2][2];
	memcpy(vector,result,sizeof(result));
}

ALUAPI ALvoid ALUAPIENTRY aluCalculateSourceParameters(ALuint source,ALuint freqOutput,ALuint numOutputChannels,ALfloat *drysend,ALfloat *wetsend,ALfloat *pitch)
{
	ALfloat ListenerOrientation[6],ListenerPosition[3],ListenerVelocity[3];
	ALfloat InnerAngle,OuterAngle,OuterGain,Angle,Distance,DryMix,WetMix;
	ALfloat Direction[3],Position[3],Velocity[3],SourceToListener[3];
	ALfloat MinVolume,MaxVolume,MinDist,MaxDist,Rolloff;
	ALfloat Pitch,ConeVolume,SourceVolume,PanningFB,PanningLR,ListenerGain;
	ALuint NumBufferChannels;
	ALfloat U[3],V[3],N[3];
	ALfloat DopplerFactor, DopplerVelocity, flSpeedOfSound, flMaxVelocity;
	ALfloat flVSS, flVLS;
	ALuint DistanceModel;
	ALfloat Matrix[3][3];
	ALint HeadRelative;
	ALuint Buffer;
	ALenum Error;
	ALfloat flAttenuation;

	if (alIsSource(source))
	{
		//Get global properties
		alGetFloatv(AL_DOPPLER_FACTOR,&DopplerFactor);
		alGetIntegerv(AL_DISTANCE_MODEL,&DistanceModel);
		alGetFloatv(AL_DOPPLER_VELOCITY,&DopplerVelocity);
		alGetFloatv(AL_SPEED_OF_SOUND,&flSpeedOfSound);
				
		//Get listener properties
		alGetListenerfv(AL_GAIN,&ListenerGain);
		alGetListenerfv(AL_POSITION,ListenerPosition);
		alGetListenerfv(AL_VELOCITY,ListenerVelocity);
		alGetListenerfv(AL_ORIENTATION,ListenerOrientation);

		//Get source properties
		alGetSourcef(source,AL_PITCH,&Pitch);
		alGetSourcef(source,AL_GAIN,&SourceVolume);
		alGetSourcei(source,AL_BUFFER,&Buffer);
		alGetSourcefv(source,AL_POSITION,Position);
		alGetSourcefv(source,AL_VELOCITY,Velocity);
		alGetSourcefv(source,AL_DIRECTION,Direction);
		alGetSourcef(source,AL_MIN_GAIN,&MinVolume);
		alGetSourcef(source,AL_MAX_GAIN,&MaxVolume);
		alGetSourcef(source,AL_REFERENCE_DISTANCE,&MinDist);
		alGetSourcef(source,AL_MAX_DISTANCE,&MaxDist);
		alGetSourcef(source,AL_ROLLOFF_FACTOR,&Rolloff);
		alGetSourcef(source,AL_CONE_OUTER_GAIN,&OuterGain);
		alGetSourcef(source,AL_CONE_INNER_ANGLE,&InnerAngle);
		alGetSourcef(source,AL_CONE_OUTER_ANGLE,&OuterAngle);
		alGetSourcei(source,AL_SOURCE_RELATIVE,&HeadRelative);
		
		//Set working variables
		DryMix=(ALfloat)(1.0f);
		WetMix=(ALfloat)(0.0f);

		//Get buffer properties
		alGetBufferi(Buffer,AL_CHANNELS,&NumBufferChannels);
		//Only apply 3D calculations for mono buffers
		if (NumBufferChannels==1)
		{
			//1. Translate Listener to origin (convert to head relative)
			if (HeadRelative==AL_FALSE)
			{
				Position[0]-=ListenerPosition[0];
				Position[1]-=ListenerPosition[1];
				Position[2]-=ListenerPosition[2];
			}
			
			//2. Calculate distance attenuation
			Distance=(ALfloat)sqrt(aluDotproduct(Position,Position));

			// Clamp to MinDist and MaxDist if appropriate
			if ((DistanceModel == AL_INVERSE_DISTANCE_CLAMPED) ||
				(DistanceModel == AL_LINEAR_DISTANCE_CLAMPED)  ||
				(DistanceModel == AL_EXPONENT_DISTANCE_CLAMPED))
			{
				Distance=(Distance<MinDist?MinDist:Distance);
				Distance=(Distance>MaxDist?MaxDist:Distance);
			}

			flAttenuation = 1.0f;
			switch (DistanceModel)
			{
			case AL_INVERSE_DISTANCE:
				if (MinDist > 0.0f)
				{
					if ((MinDist + (Rolloff * (Distance - MinDist))) > 0.0f)
						flAttenuation = MinDist / (MinDist + (Rolloff * (Distance - MinDist)));
					else
						flAttenuation = 1000000;
				}
				break;

			case AL_INVERSE_DISTANCE_CLAMPED:
				if ((MaxDist >= MinDist) && (MinDist > 0.0f))
				{
					if ((MinDist + (Rolloff * (Distance - MinDist))) > 0.0f)
						flAttenuation = MinDist / (MinDist + (Rolloff * (Distance - MinDist)));
					else
						flAttenuation = 1000000;
				}
				break;

			case AL_LINEAR_DISTANCE:
				if (MaxDist != MinDist)
					flAttenuation = 1.0f - (Rolloff*(Distance-MinDist)/(MaxDist - MinDist));
				break;

			case AL_LINEAR_DISTANCE_CLAMPED:
				if (MaxDist > MinDist)
					flAttenuation = 1.0f - (Rolloff*(Distance-MinDist)/(MaxDist - MinDist));
				break;

			case AL_EXPONENT_DISTANCE:
				if ((Distance > 0.0f) && (MinDist > 0.0f))
					flAttenuation = (ALfloat)pow(Distance/MinDist, -Rolloff);
				break;

			case AL_EXPONENT_DISTANCE_CLAMPED:
				if ((MaxDist >= MinDist) && (Distance > 0.0f) && (MinDist > 0.0f))
					flAttenuation = (ALfloat)pow(Distance/MinDist, -Rolloff);
				break;

			case AL_NONE:
			default:
				flAttenuation = 1.0f;
				break;
			}

			// Source Gain + Attenuation
			DryMix = SourceVolume * flAttenuation;
			
			// Clamp to Min/Max Gain
			DryMix=__min(DryMix,MaxVolume);
			DryMix=__max(DryMix,MinVolume);
			WetMix=__min(WetMix,MaxVolume);
			WetMix=__max(WetMix,MinVolume);
			//3. Apply directional soundcones
			SourceToListener[0]=-Position[0];
			SourceToListener[1]=-Position[1];
			SourceToListener[2]=-Position[2];
			aluNormalize(Direction);
			aluNormalize(SourceToListener);
			Angle=(ALfloat)(180.0*acos(aluDotproduct(Direction,SourceToListener))/3.141592654f);
			if ((Angle>=InnerAngle)&&(Angle<=OuterAngle))
				ConeVolume=(1.0f+(OuterGain-1.0f)*(Angle-InnerAngle)/(OuterAngle-InnerAngle));
			else if (Angle>OuterAngle)
				ConeVolume=(1.0f+(OuterGain-1.0f)                                           );
			else
				ConeVolume=1.0f;

			//4. Calculate Velocity
			if (DopplerFactor != 0.0f)
			{
				flVLS = aluDotproduct(ListenerVelocity, SourceToListener);
				flVSS = aluDotproduct(Velocity, SourceToListener);

				flMaxVelocity = (DopplerVelocity * flSpeedOfSound) / DopplerFactor;

				if (flVSS >= flMaxVelocity)
					flVSS = (flMaxVelocity - 1.0f);
				else if (flVSS <= -flMaxVelocity)
					flVSS = -flMaxVelocity + 1.0f;

				if (flVLS >= flMaxVelocity)
					flVLS = (flMaxVelocity - 1.0f);
				else if (flVLS <= -flMaxVelocity)
					flVLS = -flMaxVelocity + 1.0f;

				pitch[0] = Pitch * ((flSpeedOfSound * DopplerVelocity) - (DopplerFactor * flVLS)) /
								   ((flSpeedOfSound * DopplerVelocity) - (DopplerFactor * flVSS));
			}
			else
			{
				pitch[0] = Pitch;
			}

			//5. Align coordinate system axes
			aluCrossproduct(&ListenerOrientation[0],&ListenerOrientation[3],U); // Right-vector
			aluNormalize(U);								// Normalized Right-vector
			memcpy(V,&ListenerOrientation[3],sizeof(V));	// Up-vector
			aluNormalize(V);								// Normalized Up-vector
			memcpy(N,&ListenerOrientation[0],sizeof(N));	// At-vector
			aluNormalize(N);								// Normalized At-vector
			Matrix[0][0]=U[0]; Matrix[0][1]=V[0]; Matrix[0][2]=-N[0];
			Matrix[1][0]=U[1]; Matrix[1][1]=V[1]; Matrix[1][2]=-N[1];
			Matrix[2][0]=U[2]; Matrix[2][1]=V[2]; Matrix[2][2]=-N[2];
			aluMatrixVector(Position,Matrix);

            //6. Convert normalized position into font/back panning
			if (Distance != 0.0f)
			{
				aluNormalize(Position);
				PanningLR=(0.5f+0.5f*Position[0]);
				PanningFB=(0.5f+0.5f*Position[2]);
			}
			else
			{
				PanningLR=0.5f;
				PanningFB=0.5f;
			}

			//7. Convert front/back panning into channel volumes
			switch (numOutputChannels)
			{
				case 1:
					drysend[0]=(ConeVolume*ListenerGain*DryMix*(ALfloat)1.0f				  );	//Direct
					wetsend[0]=(ListenerGain*WetMix*(ALfloat)1.0f							  );	//Room
					break;
				case 2:
					drysend[0]=(ConeVolume*ListenerGain*DryMix*(ALfloat)sqrt((1.0f-PanningLR)));	//FL Direct
					drysend[1]=(ConeVolume*ListenerGain*DryMix*(ALfloat)sqrt((     PanningLR)));	//FR Direct
					wetsend[0]=(           ListenerGain*WetMix*(ALfloat)sqrt((1.0f-PanningLR)));	//FL Room
					wetsend[1]=(           ListenerGain*WetMix*(ALfloat)sqrt((     PanningLR)));	//FR Room
			 		break;
				default:
					break;
			}
		}
		else
		{
			//1. Stereo buffers always play from front left/front right
			switch (numOutputChannels)
			{
				case 1:
					drysend[0]=(SourceVolume*1.0f*ListenerGain);
					wetsend[0]=(SourceVolume*0.0f*ListenerGain);
					break;
				case 2:
					drysend[0]=(SourceVolume*1.0f*ListenerGain);
					drysend[1]=(SourceVolume*1.0f*ListenerGain);
					wetsend[0]=(SourceVolume*0.0f*ListenerGain);
					wetsend[1]=(SourceVolume*0.0f*ListenerGain);
			 		break;
				default:
					break;
			}
			pitch[0]=(ALfloat)(Pitch);
		}
		Error=alGetError();
	}
}

ALUAPI ALvoid ALUAPIENTRY aluMixData(ALvoid *context,ALvoid *buffer,ALsizei size,ALenum format)
{
	ALfloat Pitch,DrySend[OUTPUTCHANNELS],WetSend[OUTPUTCHANNELS];
	static float DryBuffer[BUFFERSIZE][OUTPUTCHANNELS];
	static float WetBuffer[BUFFERSIZE][OUTPUTCHANNELS];
	ALuint BlockAlign,BytesPerSample,BufferSize;
	ALuint DataSize,DataPosInt,DataPosFrac;
	ALuint Channels,Bits,Frequency,ulExtraSamples;
	ALint Looping,increment,State;
	ALuint Buffer,fraction;
	ALCcontext *ALContext;
	ALuint SamplesToDo;
	ALsource *ALSource;
	ALbuffer *ALBuffer;
	ALfloat value;
	ALshort *Data;
	ALuint i,j,k;
	ALenum Error;
	ALbufferlistitem *BufferListItem;
	ALuint loop;
	__int64 DataSize64,DataPos64;
	unsigned int fpuState;

	if (context)
	{
		ALContext=((ALCcontext *)context);
		SuspendContext(ALContext);

	//Save FPU state
	fpuState=_controlfp(0,0);
	//Change FPU rounding mode
	_controlfp(_RC_CHOP,_MCW_RC);


		if ((buffer)&&(size))
		{
			//Figure output format variables
			switch (format)
			{
				case AL_FORMAT_MONO8:
					BlockAlign=1;
					BytesPerSample=1;
					break;
				case AL_FORMAT_STEREO8:
					BlockAlign=2;
					BytesPerSample=1;
					break;
				case AL_FORMAT_MONO16:
					BlockAlign=2;
					BytesPerSample=2;
					break;
				case AL_FORMAT_STEREO16:
				default:
					BlockAlign=4;
					BytesPerSample=2;
					break;
			}
			//Setup variables
			ALSource=ALContext->Source;
			SamplesToDo=((size/BlockAlign)<BUFFERSIZE?(size/BlockAlign):BUFFERSIZE);
			//Clear mixing buffer
			memset(DryBuffer,0,SamplesToDo*OUTPUTCHANNELS*sizeof(ALfloat));
			memset(WetBuffer,0,SamplesToDo*OUTPUTCHANNELS*sizeof(ALfloat));
			//Actual mixing loop
			for (i=0;i<ALContext->SourceCount;i++)
			{
				j=0;
				State = ALSource->state;
				while ((State==AL_PLAYING)&&(j<SamplesToDo))
				{
                    aluCalculateSourceParameters((ALuint)ALSource->source,ALContext->Frequency,ALContext->Channels,DrySend,WetSend,&Pitch);
					//Get buffer info
					if (Buffer = ALSource->ulBufferID)
					{
                        ALBuffer = (ALbuffer*)ALTHUNK_LOOKUPENTRY(Buffer);

						Data = ALBuffer->data;
						Bits = (((ALBuffer->format==AL_FORMAT_MONO8)||(ALBuffer->format==AL_FORMAT_STEREO8))?8:16);
						DataSize = ALBuffer->size;
						Channels = (((ALBuffer->format==AL_FORMAT_MONO8)||(ALBuffer->format==AL_FORMAT_MONO16))?1:2);
						Frequency = ALBuffer->frequency;

						Pitch=((Pitch*Frequency)/ALContext->Frequency);
						DataSize=(DataSize/(Bits*Channels/8));
						//Get source info
						DataPosInt=ALSource->position;
						DataPosFrac=ALSource->position_fraction;
						//Compute 18.14 fixed point step
						increment=aluF2L(Pitch*(1L<<FRACTIONBITS));
						if (increment > (MAX_PITCH<<FRACTIONBITS)) increment=(MAX_PITCH<<FRACTIONBITS);
						//Figure out how many samples we can mix.
						//Pitch must be <= 4 (the number below !)
						DataSize64=DataSize+MAX_PITCH;
						DataSize64<<=FRACTIONBITS;
						DataPos64=DataPosInt;
						DataPos64<<=FRACTIONBITS;
						DataPos64+=DataPosFrac;
						BufferSize=(ALuint)((DataSize64-DataPos64)/increment);
						BufferListItem = ALSource->queue;
						for (loop = 0; loop < ALSource->BuffersAddedToDSBuffer; loop++)
							if (BufferListItem)
								BufferListItem = BufferListItem->next;
						if (BufferListItem) 
						{
							if (BufferListItem->next)
							{
								if (Channels==2)
								{
									if (((ALbuffer*)ALTHUNK_LOOKUPENTRY(BufferListItem->next->buffer))->data)
									{
										ulExtraSamples = min(((ALbuffer*)ALTHUNK_LOOKUPENTRY(BufferListItem->next->buffer))->size, 32);
										memcpy(&Data[DataSize*2], ((ALbuffer*)ALTHUNK_LOOKUPENTRY(BufferListItem->next->buffer))->data, ulExtraSamples);
									}
								}
								else
								{
									if (((ALbuffer*)ALTHUNK_LOOKUPENTRY(BufferListItem->next->buffer))->data)
									{
										ulExtraSamples = min(((ALbuffer*)ALTHUNK_LOOKUPENTRY(BufferListItem->next->buffer))->size, 16);
										memcpy(&Data[DataSize], ((ALbuffer*)ALTHUNK_LOOKUPENTRY(BufferListItem->next->buffer))->data, ulExtraSamples);
									}
								}
							}
							else if (ALSource->bLooping)
							{
								if (ALSource->queue->buffer)
								{										
									if (Channels==2)
									{
										if (((ALbuffer*)ALTHUNK_LOOKUPENTRY(ALSource->queue->buffer))->data)
										{
											ulExtraSamples = min(((ALbuffer*)ALTHUNK_LOOKUPENTRY(ALSource->queue->buffer))->size, 32);
											memcpy(&Data[DataSize*2], ((ALbuffer*)ALTHUNK_LOOKUPENTRY(ALSource->queue->buffer))->data, ulExtraSamples);
										}
									}
									else
									{
										if (((ALbuffer*)ALTHUNK_LOOKUPENTRY(ALSource->queue->buffer))->data)
										{
											ulExtraSamples = min(((ALbuffer*)ALTHUNK_LOOKUPENTRY(ALSource->queue->buffer))->size, 16);
											memcpy(&Data[DataSize], ((ALbuffer*)ALTHUNK_LOOKUPENTRY(ALSource->queue->buffer))->data, ulExtraSamples);
										}
									}
								}
							}
						}
						BufferSize=(BufferSize<(SamplesToDo-j)?BufferSize:(SamplesToDo-j));
						//Actual sample mixing loop
						Data+=DataPosInt*Channels;
						while (BufferSize--)
						{
							k=DataPosFrac>>FRACTIONBITS; fraction=DataPosFrac&FRACTIONMASK;
							if (Channels==1) 
							{
                                //First order interpolator
								value=(ALfloat)((ALshort)(((Data[k]*((1L<<FRACTIONBITS)-fraction))+(Data[k+1]*(fraction)))>>FRACTIONBITS));
								//Direct path final mix buffer and panning
								DryBuffer[j][0]+=value*DrySend[0];
								DryBuffer[j][1]+=value*DrySend[1];
								//Room path final mix buffer and panning
								WetBuffer[j][0]+=value*WetSend[0];
								WetBuffer[j][1]+=value*WetSend[1];
							}
							else
							{
                                //First order interpolator (left)
                                value=(ALfloat)((ALshort)(((Data[k*2  ]*((1L<<FRACTIONBITS)-fraction))+(Data[k*2+2]*(fraction)))>>FRACTIONBITS));
								//Direct path final mix buffer and panning (left)
								DryBuffer[j][0]+=value*DrySend[0];
								//Room path final mix buffer and panning (left)
								WetBuffer[j][0]+=value*WetSend[0];
                                //First order interpolator (right)
                                value=(ALfloat)((ALshort)(((Data[k*2+1]*((1L<<FRACTIONBITS)-fraction))+(Data[k*2+3]*(fraction)))>>FRACTIONBITS));
								//Direct path final mix buffer and panning (right)
								DryBuffer[j][1]+=value*DrySend[1];
								//Room path final mix buffer and panning (right)
								WetBuffer[j][1]+=value*WetSend[1];
							}
							DataPosFrac+=increment;
							j++;
						}
						DataPosInt+=(DataPosFrac>>FRACTIONBITS);
						DataPosFrac=(DataPosFrac&FRACTIONMASK);
						//Update source info
						ALSource->position=DataPosInt;
						ALSource->position_fraction=DataPosFrac;
					}
					//Handle looping sources
					if ((!Buffer)||(DataPosInt>=DataSize))
					{
						//queueing
						if (ALSource->queue)
						{
							Looping = ALSource->bLooping;
							if (ALSource->BuffersAddedToDSBuffer < (ALSource->BuffersInQueue-1))
							{
								BufferListItem = ALSource->queue;
								for (loop = 0; loop <= ALSource->BuffersAddedToDSBuffer; loop++)
								{
									if (BufferListItem)
									{
										if (!Looping)
											BufferListItem->bufferstate=PROCESSED;
										BufferListItem = BufferListItem->next;
									}
								}
								if (!Looping)
									ALSource->BuffersProcessed++;
								if (BufferListItem)
									ALSource->ulBufferID=BufferListItem->buffer;
								ALSource->position=DataPosInt-DataSize;
								ALSource->position_fraction=DataPosFrac;
								ALSource->BuffersAddedToDSBuffer++;
							}
							else
							{
								alSourceStop((ALuint)ALSource->source);
								if (Looping)
								{
                                    alSourceRewind((ALuint)ALSource->source);
                                    alSourcePlay((ALuint)ALSource->source);
									ALSource->position=DataPosInt-DataSize;
									ALSource->position_fraction=DataPosFrac;
								}
							}
						}
					}
					//Get source state
					State = ALSource->state;
				}
				ALSource=ALSource->next;
			}
			//Post processing loop
			switch (format)
			{
				case AL_FORMAT_MONO8:
					for (i=0;i<(size/BytesPerSample);i++)
						((ALubyte *)buffer)[i]=aluF2S(DryBuffer[i][0]+DryBuffer[i][1]+WetBuffer[i][0]+WetBuffer[i][1])+128;
					break;
				case AL_FORMAT_STEREO8:
					for (i=0;i<(size/BytesPerSample);i++)
						((ALubyte *)buffer)[i]=aluF2S(DryBuffer[i>>1][i&1]+WetBuffer[i>>1][i&1])+128;
					break;
				case AL_FORMAT_MONO16:
					for (i=0;i<(size/BytesPerSample);i++)
						((ALshort *)buffer)[i]=aluF2S(DryBuffer[i][0]+DryBuffer[i][1]+WetBuffer[i][0]+WetBuffer[i][1]);
					break;
				case AL_FORMAT_STEREO16:
				default:
					for (i=0;i<(size/BytesPerSample);i++)
						((ALshort *)buffer)[i]=aluF2S(DryBuffer[i>>1][i&1]+WetBuffer[i>>1][i&1]);
					break;
			}
		}

		Error=alGetError();
		ProcessContext(ALContext);

	//Restore FPU rounding mode
	_controlfp(fpuState,0xfffff);

	}
}
