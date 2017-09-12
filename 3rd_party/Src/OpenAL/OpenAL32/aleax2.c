#include "aleax2.h"
#include "Include/alEaxPresets.h"

extern ALboolean bEAX2Initialized;

ALboolean LongInRange(long lValue, long lMin, long lMax);
ALboolean ULongInRange(unsigned long ulValue, unsigned long ulMin, unsigned long ulMax);
ALboolean FloatInRange(float flValue, float flMin, float flMax);

ALenum eax2BufferGet(ALuint property, ALuint source, ALvoid *value, ALuint size, ALint iSWMixer)
{
	ALsource	*pSource;
	ALuint		ulBytes;
	ALenum		ALErrorCode = AL_NO_ERROR;
	
	pSource = (ALsource*)ALTHUNK_LOOKUPENTRY(source);

	if ((pSource->uservalue3) || (iSWMixer))
	{
		if (!bEAX2Initialized)
		{
			bEAX2Initialized = AL_TRUE;
		}

		if (pSource->uservalue3)
		{
			if (FAILED(IKsPropertySet_Get((LPKSPROPERTYSET)pSource->uservalue3, &DSPROPSETID_EAX20_BufferProperties, property, NULL, 0,
					value, size, &ulBytes)))
				ALErrorCode = AL_INVALID_OPERATION;
		}
	}
	else
	{
		ALErrorCode = AL_INVALID_OPERATION;
	}

	return ALErrorCode;
}


ALenum eax2ListenerGet(ALuint property, ALsource *pSource, ALvoid *value, ALuint size, ALint iSWMixer)
{
	ALuint		ulBytes;
	ALenum		ALErrorCode = AL_NO_ERROR;

	if (!bEAX2Initialized)
	{
		bEAX2Initialized = AL_TRUE;
	}

    if (pSource->uservalue3)
	{
		if (FAILED(IKsPropertySet_Get((LPKSPROPERTYSET)pSource->uservalue3, &DSPROPSETID_EAX20_ListenerProperties, property, NULL, 0, value, size, &ulBytes)))
			ALErrorCode = AL_INVALID_OPERATION;
	}

	return ALErrorCode;
}


ALenum eax2BufferSet(ALuint property, ALuint source, ALvoid *pValue, ALuint size, ALint iSWMixer)
{
	ALsource	*pSource;
	ALboolean	bSetValue = AL_FALSE;
	ALenum		ALErrorCode = AL_NO_ERROR;
	
	pSource = (ALsource*)ALTHUNK_LOOKUPENTRY(source);

	if (!bEAX2Initialized)
	{
		bEAX2Initialized = AL_TRUE;
	}

	switch(property & ~DSPROPERTY_EAXBUFFER_DEFERRED)
	{
	case DSPROPERTY_EAXBUFFER_NONE:
		bSetValue = AL_TRUE;
		break;

	case DSPROPERTY_EAXBUFFER_ALLPARAMETERS:
		if ((pValue) && (size >= sizeof(EAXBUFFERPROPERTIES)))
		{
			if (memcmp((void*)(&pSource->EAX20BP), pValue, sizeof(EAXBUFFERPROPERTIES)))
			{
				if ( (LongInRange(((LPEAXBUFFERPROPERTIES)pValue)->lDirect, EAXBUFFER_MINDIRECT, EAXBUFFER_MAXDIRECT)) &&
					 (LongInRange(((LPEAXBUFFERPROPERTIES)pValue)->lDirectHF, EAXBUFFER_MINDIRECTHF, EAXBUFFER_MAXDIRECTHF)) &&
					 (LongInRange(((LPEAXBUFFERPROPERTIES)pValue)->lRoom, EAXBUFFER_MINROOM, EAXBUFFER_MAXROOM)) &&
					 (LongInRange(((LPEAXBUFFERPROPERTIES)pValue)->lRoomHF, EAXBUFFER_MINROOMHF, EAXBUFFER_MAXROOMHF)) &&
					 (LongInRange(((LPEAXBUFFERPROPERTIES)pValue)->lObstruction, EAXBUFFER_MINOBSTRUCTION, EAXBUFFER_MAXOBSTRUCTION)) &&
					 (FloatInRange(((LPEAXBUFFERPROPERTIES)pValue)->flObstructionLFRatio, EAXBUFFER_MINOBSTRUCTIONLFRATIO, EAXBUFFER_MAXOBSTRUCTIONLFRATIO)) &&
					 (LongInRange(((LPEAXBUFFERPROPERTIES)pValue)->lOcclusion, EAXBUFFER_MINOCCLUSION, EAXBUFFER_MAXOCCLUSION)) &&
					 (FloatInRange(((LPEAXBUFFERPROPERTIES)pValue)->flOcclusionLFRatio, EAXBUFFER_MINOCCLUSIONLFRATIO, EAXBUFFER_MAXOCCLUSIONLFRATIO)) &&
					 (FloatInRange(((LPEAXBUFFERPROPERTIES)pValue)->flOcclusionRoomRatio, EAXBUFFER_MINOCCLUSIONROOMRATIO, EAXBUFFER_MAXOCCLUSIONROOMRATIO)) &&
					 (LongInRange(((LPEAXBUFFERPROPERTIES)pValue)->lOutsideVolumeHF, EAXBUFFER_MINOUTSIDEVOLUMEHF, EAXBUFFER_MAXOUTSIDEVOLUMEHF)) &&
					 (FloatInRange(((LPEAXBUFFERPROPERTIES)pValue)->flRoomRolloffFactor, EAXBUFFER_MINROOMROLLOFFFACTOR, EAXBUFFER_MAXROOMROLLOFFFACTOR)) &&
					 (FloatInRange(((LPEAXBUFFERPROPERTIES)pValue)->flAirAbsorptionFactor, EAXBUFFER_MINAIRABSORPTIONFACTOR, EAXBUFFER_MAXAIRABSORPTIONFACTOR)) &&
					 (ULongInRange(((LPEAXBUFFERPROPERTIES)pValue)->dwFlags, 0, ~EAXBUFFERFLAGS_RESERVED)) )
				{
					memcpy((void*)(&pSource->EAX20BP), pValue, sizeof(EAXBUFFERPROPERTIES));
					bSetValue = AL_TRUE;
				}
				else
				{
					ALErrorCode = AL_INVALID_OPERATION;
				}
			}
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_DIRECT:
		if ((pValue) && (size >= sizeof(long)) && (LongInRange(*((long*)pValue), EAXBUFFER_MINDIRECT, EAXBUFFER_MAXDIRECT)))
		{
			bSetValue = (pSource->EAX20BP.lDirect != (*(long*)pValue));
			pSource->EAX20BP.lDirect = (*(long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_DIRECTHF:
		if ((pValue) && (size >= sizeof(long)) && (LongInRange(*((long *)pValue), EAXBUFFER_MINDIRECTHF, EAXBUFFER_MAXDIRECTHF)))
		{
			bSetValue = (pSource->EAX20BP.lDirectHF != (*(long*)pValue));
			pSource->EAX20BP.lDirectHF = (*(long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_ROOM:
		if ((pValue) && (size >= sizeof(long)) && (LongInRange(*((long *)pValue), EAXBUFFER_MINROOM, EAXBUFFER_MAXROOM)))
		{
			bSetValue = (pSource->EAX20BP.lRoom != (*(long*)pValue));
			pSource->EAX20BP.lRoom = (*(long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_ROOMHF:
		if ((pValue) && (size >= sizeof(long)) && (LongInRange(*((long *)pValue), EAXBUFFER_MINROOMHF, EAXBUFFER_MAXROOMHF)))
		{
			bSetValue = (pSource->EAX20BP.lRoomHF != (*(long*)pValue));
			pSource->EAX20BP.lRoomHF = (*(long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_OBSTRUCTION:
		if ((pValue) && (size >= sizeof(long)) && (LongInRange(*((long *)pValue), EAXBUFFER_MINOBSTRUCTION, EAXBUFFER_MAXOBSTRUCTION)))
		{
			bSetValue = (pSource->EAX20BP.lObstruction != (*(long*)pValue));
			pSource->EAX20BP.lObstruction = (*(long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_OBSTRUCTIONLFRATIO:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*((float *)pValue), EAXBUFFER_MINOBSTRUCTIONLFRATIO, EAXBUFFER_MAXOBSTRUCTIONLFRATIO)))
		{
			bSetValue = (pSource->EAX20BP.flObstructionLFRatio != (*(float*)pValue));
			pSource->EAX20BP.flObstructionLFRatio = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_OCCLUSION:
		if ((pValue) && (size >= sizeof(long)) && (LongInRange(*((long *)pValue), EAXBUFFER_MINOCCLUSION, EAXBUFFER_MAXOCCLUSION)))
		{
			bSetValue = (pSource->EAX20BP.lOcclusion != (*(long*)pValue));
			pSource->EAX20BP.lOcclusion = (*(long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_OCCLUSIONLFRATIO:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*((float *)pValue), EAXBUFFER_MINOCCLUSIONLFRATIO, EAXBUFFER_MAXOCCLUSIONLFRATIO)))
		{
			bSetValue = (pSource->EAX20BP.flOcclusionLFRatio != (*(float*)pValue));
			pSource->EAX20BP.flOcclusionLFRatio = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_OCCLUSIONROOMRATIO:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*((float *)pValue), EAXBUFFER_MINOCCLUSIONROOMRATIO, EAXBUFFER_MAXOCCLUSIONROOMRATIO)))
		{
			bSetValue = (pSource->EAX20BP.flOcclusionRoomRatio != (*(float*)pValue));
			pSource->EAX20BP.flOcclusionRoomRatio = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_OUTSIDEVOLUMEHF:
		if ((pValue) && (size >= sizeof(long)) && (LongInRange(*((long *)pValue), EAXBUFFER_MINOUTSIDEVOLUMEHF, EAXBUFFER_MAXOUTSIDEVOLUMEHF)))
		{
			bSetValue = (pSource->EAX20BP.lOutsideVolumeHF != (*(long*)pValue));
			pSource->EAX20BP.lOutsideVolumeHF = (*(long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_ROOMROLLOFFFACTOR:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*((float *)pValue), EAXBUFFER_MINROOMROLLOFFFACTOR, EAXBUFFER_MAXROOMROLLOFFFACTOR)))
		{
			bSetValue = (pSource->EAX20BP.flRoomRolloffFactor != (*(float*)pValue));
			pSource->EAX20BP.flRoomRolloffFactor = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_AIRABSORPTIONFACTOR:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*((float *)pValue), EAXBUFFER_MINAIRABSORPTIONFACTOR, EAXBUFFER_MAXAIRABSORPTIONFACTOR)))
		{
			bSetValue = (pSource->EAX20BP.flAirAbsorptionFactor != (*(float*)pValue));
			pSource->EAX20BP.flAirAbsorptionFactor = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXBUFFER_FLAGS:
		if ((pValue) && (size >= sizeof(unsigned long)) && (ULongInRange(*((unsigned long *)pValue), 0, ~EAXBUFFERFLAGS_RESERVED)))
		{
			bSetValue = (pSource->EAX20BP.dwFlags != (*(unsigned long*)pValue));
			pSource->EAX20BP.dwFlags = (*(unsigned long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	default:
		ALErrorCode = AL_INVALID_OPERATION;
	}

	if (bSetValue)
	{
		if (pSource->uservalue3)
		{
			if (FAILED(IKsPropertySet_Set((LPKSPROPERTYSET)pSource->uservalue3, &DSPROPSETID_EAX20_BufferProperties, property, NULL, 0, pValue, size)))
				ALErrorCode = AL_INVALID_OPERATION;
		}
	}

	return ALErrorCode;
}


ALenum eax2ListenerSet(ALuint property, ALsource *pSource, ALvoid *pValue, ALuint size, ALint iSWMixer)
{
	ALCcontext	*pContext;
	ALenum		ALErrorCode = AL_NO_ERROR;
	ALboolean	bSetValue = AL_FALSE;
	ALboolean	bGetValue = AL_FALSE;
	ALuint		ulBytesReturned;

	pContext = alcGetCurrentContext();

	if (!bEAX2Initialized)
	{
		bEAX2Initialized = AL_TRUE;
	}

	switch(property & ~DSPROPERTY_EAXLISTENER_DEFERRED)
	{
	case DSPROPERTY_EAXLISTENER_NONE:
		bSetValue = AL_TRUE;
		break;

	case DSPROPERTY_EAXLISTENER_ALLPARAMETERS:
		if ((pValue) && (size >= sizeof(EAXLISTENERPROPERTIES)))
		{
			if (memcmp((void*)(&pContext->Listener.EAX20LP), pValue, sizeof(EAXLISTENERPROPERTIES)))
			{
				if ( (LongInRange(((LPEAXLISTENERPROPERTIES)pValue)->lRoom, EAXLISTENER_MINROOM, EAXLISTENER_MAXROOM)) &&
					 (LongInRange(((LPEAXLISTENERPROPERTIES)pValue)->lRoomHF, EAXLISTENER_MINROOMHF, EAXLISTENER_MAXROOMHF)) &&
					 (FloatInRange(((LPEAXLISTENERPROPERTIES)pValue)->flRoomRolloffFactor, EAXLISTENER_MINROOMROLLOFFFACTOR, EAXLISTENER_MAXROOMROLLOFFFACTOR)) &&
					 (FloatInRange(((LPEAXLISTENERPROPERTIES)pValue)->flDecayTime, EAXLISTENER_MINDECAYTIME, EAXLISTENER_MAXDECAYTIME)) &&
					 (FloatInRange(((LPEAXLISTENERPROPERTIES)pValue)->flDecayHFRatio, EAXLISTENER_MINDECAYHFRATIO, EAXLISTENER_MAXDECAYHFRATIO)) &&
					 (LongInRange(((LPEAXLISTENERPROPERTIES)pValue)->lReflections, EAXLISTENER_MINREFLECTIONS, EAXLISTENER_MAXREFLECTIONS)) &&
					 (FloatInRange(((LPEAXLISTENERPROPERTIES)pValue)->flReflectionsDelay, EAXLISTENER_MINREFLECTIONSDELAY, EAXLISTENER_MAXREFLECTIONSDELAY)) &&
					 (LongInRange(((LPEAXLISTENERPROPERTIES)pValue)->lReverb, EAXLISTENER_MINREVERB, EAXLISTENER_MAXREVERB)) &&
					 (FloatInRange(((LPEAXLISTENERPROPERTIES)pValue)->flReverbDelay, EAXLISTENER_MINREVERBDELAY, EAXLISTENER_MAXREVERBDELAY)) &&
					 (ULongInRange(((LPEAXLISTENERPROPERTIES)pValue)->dwEnvironment, 0, EAX_ENVIRONMENT_COUNT)) &&
					 (FloatInRange(((LPEAXLISTENERPROPERTIES)pValue)->flEnvironmentSize, EAXLISTENER_MINENVIRONMENTSIZE, EAXLISTENER_MAXENVIRONMENTSIZE)) &&
					 (FloatInRange(((LPEAXLISTENERPROPERTIES)pValue)->flEnvironmentDiffusion, EAXLISTENER_MINENVIRONMENTDIFFUSION, EAXLISTENER_MAXENVIRONMENTDIFFUSION)) &&
					 (FloatInRange(((LPEAXLISTENERPROPERTIES)pValue)->flAirAbsorptionHF, EAXLISTENER_MINAIRABSORPTIONHF, EAXLISTENER_MAXAIRABSORPTIONHF)) &&
					 (ULongInRange(((LPEAXLISTENERPROPERTIES)pValue)->dwFlags, 0, ~EAXLISTENERFLAGS_RESERVED)) )
				{
					memcpy((void*)(&pContext->Listener.EAX20LP), pValue, sizeof(EAXLISTENERPROPERTIES));
					bSetValue = AL_TRUE;
				}
				else
				{
					ALErrorCode = AL_INVALID_OPERATION;
				}
			}
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_ROOM:
		if ((pValue) && (size >= sizeof(long)) && (LongInRange(*(long*)pValue, EAXLISTENER_MINROOM, EAXLISTENER_MAXROOM)))
		{
			bSetValue = (pContext->Listener.EAX20LP.lRoom != (*(long*)pValue));
			pContext->Listener.EAX20LP.lRoom = (*(long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_ROOMHF:
		if ((pValue) && (size >= sizeof(long)) && (LongInRange(*(long*)pValue, EAXLISTENER_MINROOMHF, EAXLISTENER_MAXROOMHF)))
		{
			bSetValue = (pContext->Listener.EAX20LP.lRoomHF != (*(long*)pValue));
			pContext->Listener.EAX20LP.lRoomHF = (*(long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*(float*)pValue, EAXLISTENER_MINROOMROLLOFFFACTOR, EAXLISTENER_MAXROOMROLLOFFFACTOR)))
		{
			bSetValue = (pContext->Listener.EAX20LP.flRoomRolloffFactor != (*(float*)pValue));
			pContext->Listener.EAX20LP.flRoomRolloffFactor = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_DECAYTIME:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*(float*)pValue, EAXLISTENER_MINDECAYTIME, EAXLISTENER_MAXDECAYTIME)))
		{
			bSetValue = (pContext->Listener.EAX20LP.flDecayTime != (*(float*)pValue));
			pContext->Listener.EAX20LP.flDecayTime = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_DECAYHFRATIO:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*(float*)pValue, EAXLISTENER_MINDECAYHFRATIO, EAXLISTENER_MAXDECAYHFRATIO)))
		{
			bSetValue = (pContext->Listener.EAX20LP.flDecayHFRatio != (*(float*)pValue));
			pContext->Listener.EAX20LP.flDecayHFRatio = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_REFLECTIONS:
		if ((pValue) && (size >= sizeof(long)) && (LongInRange(*(long*)pValue, EAXLISTENER_MINREFLECTIONS, EAXLISTENER_MAXREFLECTIONS)))
		{
			bSetValue = (pContext->Listener.EAX20LP.lReflections != (*(long*)pValue));
			pContext->Listener.EAX20LP.lReflections = (*(long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*(float*)pValue, EAXLISTENER_MINREFLECTIONSDELAY, EAXLISTENER_MAXREFLECTIONSDELAY)))
		{
			bSetValue = (pContext->Listener.EAX20LP.flReflectionsDelay != (*(float*)pValue));
			pContext->Listener.EAX20LP.flReflectionsDelay = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_REVERB:
		if ((pValue) && (size >= sizeof(long)) && (LongInRange(*(long*)pValue, EAXLISTENER_MINREVERB, EAXLISTENER_MAXREVERB)))
		{
			bSetValue = (pContext->Listener.EAX20LP.lReverb != (*(long*)pValue));
			pContext->Listener.EAX20LP.lReverb = (*(long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_REVERBDELAY:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*(float*)pValue, EAXLISTENER_MINREVERBDELAY, EAXLISTENER_MAXREVERBDELAY)))
		{
			bSetValue = (pContext->Listener.EAX20LP.flReverbDelay != (*(float*)pValue));
			pContext->Listener.EAX20LP.flReverbDelay = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_ENVIRONMENT:
		if ((pValue) && (size >= sizeof(unsigned long)) && (ULongInRange(*(unsigned long*)pValue, 0, EAX_ENVIRONMENT_COUNT)))
		{
			// Always Set this property
			bGetValue = bSetValue = TRUE;
			memcpy((void*)(&pContext->Listener.EAX20LP), &EAX20Preset[*(unsigned long*)pValue], sizeof(EAXLISTENERPROPERTIES));
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*(float*)pValue, EAXLISTENER_MINENVIRONMENTSIZE, EAXLISTENER_MAXENVIRONMENTSIZE)))
		{
			bGetValue = bSetValue = (pContext->Listener.EAX20LP.flEnvironmentSize != (*(float*)pValue));
			pContext->Listener.EAX20LP.flEnvironmentSize = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*(float*)pValue, EAXLISTENER_MINENVIRONMENTDIFFUSION, EAXLISTENER_MAXENVIRONMENTDIFFUSION)))
		{
			bSetValue = (pContext->Listener.EAX20LP.flEnvironmentDiffusion != (*(float*)pValue));
			pContext->Listener.EAX20LP.flEnvironmentDiffusion = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF:
		if ((pValue) && (size >= sizeof(float)) && (FloatInRange(*(float*)pValue, EAXLISTENER_MINAIRABSORPTIONHF, EAXLISTENER_MAXAIRABSORPTIONHF)))
		{
			bSetValue = (pContext->Listener.EAX20LP.flAirAbsorptionHF != (*(float*)pValue));
			pContext->Listener.EAX20LP.flAirAbsorptionHF = (*(float*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	case DSPROPERTY_EAXLISTENER_FLAGS:
		if ((pValue) && (size >= sizeof(unsigned long)) && (ULongInRange(*(unsigned long*)pValue, 0, ~EAXLISTENERFLAGS_RESERVED)))
		{
			bSetValue = (pContext->Listener.EAX20LP.dwFlags != (*(unsigned long*)pValue));
			pContext->Listener.EAX20LP.dwFlags = (*(unsigned long*)pValue);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}
		break;

	default:
		ALErrorCode = AL_INVALID_OPERATION;
		break;
	}

	if (bSetValue)
	{
        if (pSource->uservalue3)
		{
			if (FAILED(IKsPropertySet_Set((LPKSPROPERTYSET)pSource->uservalue3, &DSPROPSETID_EAX20_ListenerProperties, property, NULL, 0, pValue, size)))
				ALErrorCode = AL_INVALID_OPERATION;

			if (bGetValue)
				IKsPropertySet_Get((LPKSPROPERTYSET)pSource->uservalue3, &DSPROPSETID_EAX20_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, NULL, 0,
					&pContext->Listener.EAX20LP, sizeof(pContext->Listener.EAX20LP), &ulBytesReturned);
		}
	}

	return ALErrorCode;
}