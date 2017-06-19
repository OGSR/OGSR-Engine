///////////////////////////////////////////////////////////////
// InfoPortionsDefs.h
// общие объ€влени€ дл€ классов работающих с info_portion
///////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "object_interfaces.h"
//#define NO_INFO_INDEX	(-1)
//typedef int				INFO_INDEX;
typedef shared_str		INFO_ID;


struct INFO_DATA : public IPureSerializeObject<IReader,IWriter>
{
	INFO_DATA():info_id(NULL),receive_time(0){};
	INFO_DATA(INFO_ID id, ALife::_TIME_ID time):info_id(id),receive_time(time){};

	virtual void load (IReader& stream);
	virtual void save (IWriter&);

	INFO_ID				info_id;
	//врем€ получени€ нужно порции информации
	ALife::_TIME_ID		receive_time;
};

DEFINE_VECTOR		(INFO_DATA, KNOWN_INFO_VECTOR, KNOWN_INFO_VECTOR_IT);