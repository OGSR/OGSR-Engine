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

#include "alMain.h"
#include "AL/al.h"
#include "AL/alc.h"
#include "alError.h"
#include "alEax.h"
#include "alListener.h"
#include "aleax2.h"

// EAX 2.0 GUIDs
const GUID DSPROPSETID_EAX20_ListenerProperties
				= { 0x306a6a8, 0xb224, 0x11d2, { 0x99, 0xe5, 0x0, 0x0, 0xe8, 0xd8, 0xc7, 0x22 } };

const GUID DSPROPSETID_EAX20_BufferProperties
				= { 0x306a6a7, 0xb224, 0x11d2, {0x99, 0xe5, 0x0, 0x0, 0xe8, 0xd8, 0xc7, 0x22 } };

EAXGUID GetEAXGuid(const GUID *propertySetID);

ALboolean LongInRange(long lValue, long lMin, long lMax);
ALboolean ULongInRange(unsigned long ulValue, unsigned long ulMin, unsigned long ulMax);
ALboolean FloatInRange(float flValue, float flMin, float flMax);

ALboolean bEAX2Initialized = AL_FALSE;
/*
	Test for support of appropriate EAX Version
*/
ALboolean CheckEAXSupport(const ALchar *szEAXName)
{
	ALCcontext		*ALContext;
	ALCdevice		*ALCDevice;
	ALsource		*ALSource;
	LPKSPROPERTYSET	lpPropertySet = NULL;
	GUID			ListenerGuid, BufferGuid;
	ALuint			ListenerProperty, BufferProperty;
	ALuint			i, ulSupport;
	ALuint			property, size;
	ALint			value = 0xFFFFFFFF;
	ALfloat			fvalue;
	ALboolean		bSourceGenerated = AL_FALSE;
	ALboolean		bEAXSupported = AL_FALSE;
	ALint			iSWReverbMixer = 0;
	
	ALContext = alcGetCurrentContext();
	ALCDevice = alcGetContextsDevice(ALContext);
	SuspendContext(ALContext);


	if (iSWReverbMixer)
	{
	}
	else
	{
		// To test for EAX support we will need a valid Source
		ALSource = ALContext->Source;

		// See if one has already been created
		for (i=0;i<ALContext->SourceCount;i++)
		{
			if (ALSource->uservalue3)
			{
				lpPropertySet = ALSource->uservalue3;
				break;
			}
			ALSource = ALSource->next;
		}

		// If we didn't find a valid source, create one now
		if (lpPropertySet == NULL)
		{
			ALuint alsource = 0;
			alGenSources(1, ((ALuint *)(&alsource)));
			if (alGetError() == AL_NO_ERROR)
			{
				ALSource = (ALsource*)ALTHUNK_LOOKUPENTRY(alsource);
				lpPropertySet = ALSource->uservalue3;
				bSourceGenerated = AL_TRUE;
			}
		}

		// If Property Set Interface hasn't been obtained, EAX support is not available
		if (lpPropertySet)
		{
			if ( (_stricmp(szEAXName, "EAX") == 0) || (_stricmp(szEAXName, "EAX2.0") == 0) )
			{
				ListenerGuid = DSPROPSETID_EAX20_ListenerProperties;
				BufferGuid = DSPROPSETID_EAX20_BufferProperties;
				ListenerProperty = 1;	// LISTENER_ALL
				BufferProperty = 1;		// BUFFER_ALL
				property = 2;			// ROOM
				size = sizeof(ALint);
				value = -10000;
			}
			else
			{
				// Unknown EAX Name
				if (bSourceGenerated)
					alDeleteSources(1, ((ALuint *)&ALSource->source));

				ProcessContext(ALContext);
				return AL_FALSE;
			}
			
			if (SUCCEEDED(IKsPropertySet_QuerySupport(lpPropertySet, &ListenerGuid, ListenerProperty, &ulSupport)))
			{
				if ( (ulSupport & KSPROPERTY_SUPPORT_GET) && (ulSupport & KSPROPERTY_SUPPORT_SET))
				{
					if (SUCCEEDED(IKsPropertySet_QuerySupport(lpPropertySet, &BufferGuid, BufferProperty, &ulSupport)))
					{
						if ( (ulSupport & KSPROPERTY_SUPPORT_GET) && (ulSupport & KSPROPERTY_SUPPORT_SET) )
						{
							// Fully supported !
							bEAXSupported = AL_TRUE;

							// Set Default Property
							if (value != 0xFFFFFFFF)
								IKsPropertySet_Set(lpPropertySet, &ListenerGuid, property, NULL, 0, &value, sizeof(ALint));
							else
								IKsPropertySet_Set(lpPropertySet, &ListenerGuid, property, NULL, 0, &fvalue, sizeof(ALfloat));
						}
					}
				}
			}
		}

		if (bSourceGenerated)
			alDeleteSources(1, ((ALuint *)&ALSource->source));
	}

	ProcessContext(ALContext);
	return bEAXSupported;
}


/*
	EAXGet(propertySetID, property, source, value, size)

	propertySetID : GUID of EAX Property Set (defined in eax.h files)
	property	  : Property in Property Set to retrieve (enumerations defined in eax.h files)
	source		  : Source to use to retrieve EAX affect (this can be NULL for Listener Property Sets)
	value		  : Pointer to memory location to retrieve value
	size		  : Size of data pointed to by value

	Returns AL_INVALID_NAME if a valid Source name was required but not given
	Returns AL_INVALID_OPERATION if the Source is 2D
	Returns AL_INVALID_VALUE if the GUID is not recognized
*/
ALAPI ALenum ALAPIENTRY EAXGet(const GUID *propertySetID,ALuint property,ALuint source,ALvoid *value,ALuint size)
{
	ALuint i;
	ALsource	*ALSource;
	ALCcontext	*ALContext;
	ALenum		ALErrorCode = AL_NO_ERROR;
	ALboolean	bGenSource = AL_FALSE;
	EAXGUID		eaxGuid = EAXNONE;
	ALint		iSWReverbMixer = 0;

	ALContext = alcGetCurrentContext();
	SuspendContext(ALContext);

	eaxGuid = GetEAXGuid(propertySetID);

	if (eaxGuid & EAXBUFFERGUID)
	{
		if (alIsSource(source))
		{
			if (eaxGuid == EAX2B)
				ALErrorCode = eax2BufferGet(property, source, value, size, iSWReverbMixer);
		}
		else
		{
			ALErrorCode = AL_INVALID_NAME;
		}
	}
	else if (eaxGuid & EAXLISTENERGUID)
	{
		// If source is valid use that, otherwise find a source
        if (alIsSource(source))
		{
            ALSource = (ALsource*)ALTHUNK_LOOKUPENTRY(source);
		}
		else
		{
			ALSource = ALContext->Source;

			// See if one has already been created
			for (i=0;i<ALContext->SourceCount;i++)
			{
				if ((ALSource->uservalue3) || (iSWReverbMixer))
					break;
				ALSource = ALSource->next;
			}

			// If an appropriate source wasn't created, generate one now
			if (ALSource == NULL)
			{
                ALuint alsource = 0;
                alGenSources(1, ((ALuint *)(&alsource)));
                if (alGetError() == AL_NO_ERROR)
				{
					ALSource = (ALsource*)ALTHUNK_LOOKUPENTRY(alsource);
                    bGenSource = AL_TRUE;
				}
			}
		}

		if (ALSource)
		{
			if (eaxGuid == EAX2L)
				ALErrorCode = eax2ListenerGet(property, ALSource, value, size, iSWReverbMixer);
		}
		else
		{
			ALErrorCode = AL_INVALID_OPERATION;
		}

		// If we generated a source to get the EAX Listener property, release it now
		if (bGenSource)
            alDeleteSources(1, (ALuint *)&ALSource->source);
	}
	else
	{
		ALErrorCode = AL_INVALID_VALUE;
	}

	ProcessContext(ALContext);

	return ALErrorCode;
}


/*
	EAXSet(propertySetID, property, source, value, size)

	propertySetID : GUID of EAX Property Set (defined in eax.h files)
	property	  : Property in Property Set to affect (enumerations defined in eax.h files)
	source		  : Source to apply EAX affects to (this can be NULL for Listener Property Sets)
	value		  : Pointer to value to set
	size		  : Size of data pointed to by value

	Returns AL_INVALID_NAME if a valid Source name was required but not given
	Returns AL_INVALID_OPERATION if the Source is 2D or the EAX call fails
	Returns AL_INVALID_VALUE if the GUID is not recognized
*/
ALAPI ALenum ALAPIENTRY EAXSet(const GUID *propertySetID,ALuint property,ALuint source,ALvoid *pValue,ALuint size)
{
	ALsource	*ALSource;
	ALCcontext	*ALContext;
	ALCcontext  *ALCContext;
	ALuint		i;
	ALenum		ALErrorCode = AL_NO_ERROR;
	EAXGUID		eaxGuid;
	ALboolean	bGenSource = AL_FALSE;
	ALint		iSWReverbMixer = 0;
	ALCdevice   *ALDevice = NULL;
	
	ALContext=alcGetCurrentContext();
	SuspendContext(ALContext);

	ALCContext = ALContext;

	eaxGuid = GetEAXGuid(propertySetID);

	if (eaxGuid & EAXBUFFERGUID)
	{
		if (alIsSource(source))
		{
			if (eaxGuid == EAX2B)
				ALErrorCode = eax2BufferSet(property, source, pValue, size, iSWReverbMixer);
		}
		else
		{
			ALErrorCode = AL_INVALID_NAME;
		}
	}
	else if (eaxGuid & EAXLISTENERGUID)
	{
		// If source is valid use that, otherwise find a source
        if (alIsSource(source))
		{
            ALSource = (ALsource*)ALTHUNK_LOOKUPENTRY(source);
		}
		else
		{
			ALSource = ALContext->Source;

			// See if one has already been created
			for (i=0;i<ALContext->SourceCount;i++)
			{
                if ((ALSource->uservalue3) || (iSWReverbMixer))
					break;
				ALSource = ALSource->next;
			}

			// If an appropriate source wasn't created, generate one now
			if (ALSource == NULL)
			{
                ALuint alsource = 0;
                alGenSources(1, ((ALuint *)(&alsource)));
				if (alGetError() == AL_NO_ERROR)
				{
					ALSource = (ALsource*)ALTHUNK_LOOKUPENTRY(alsource);
					bGenSource = AL_TRUE;
				}
			}
		}

		if (ALSource)
		{
			if (eaxGuid == EAX2L)
				ALErrorCode = eax2ListenerSet(property, ALSource, pValue, size, iSWReverbMixer);
		}
		else
			ALErrorCode = AL_INVALID_OPERATION;
	}
	else
	{
		ALErrorCode = AL_INVALID_VALUE;
	}

	// If we generated a source to set the EAX Listener property, release it now
	if (bGenSource)
        alDeleteSources(1, (ALuint *)&ALSource->source);

	ProcessContext(ALCContext);
	return ALErrorCode;
}

ALboolean LongInRange(long lValue, long lMin, long lMax)
{
	return ((lValue >= lMin) && (lValue <= lMax));
}

ALboolean ULongInRange(unsigned long ulValue, unsigned long ulMin, unsigned long ulMax)
{
	return ((ulValue >= ulMin) && (ulValue <= ulMax));
}

ALboolean FloatInRange(float flValue, float flMin, float flMax)
{
	return ((flValue >= flMin) && (flValue <= flMax));
}

EAXGUID GetEAXGuid(const GUID *propertySetID)
{
	EAXGUID eaxGuid = EAXNONE;

	if ( IsEqualGUID(propertySetID, &DSPROPSETID_EAX20_ListenerProperties) )
		eaxGuid = EAX2L;
	else if ( IsEqualGUID(propertySetID, &DSPROPSETID_EAX20_BufferProperties) )
		eaxGuid = EAX2B;

	return eaxGuid;
}
