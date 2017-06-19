#pragma once

#include "alife_space.h"
#include "object_interfaces.h"

struct INFO_DATA : public IPureSerializeObject<IReader,IWriter>
{
	INFO_DATA			():info_id(NULL),receive_time(0)			{};
	INFO_DATA			(shared_str id, ALife::_TIME_ID time):info_id(id),receive_time(time){};

	virtual void		load			(IReader& stream);
	virtual void		save			(IWriter&);

	shared_str			info_id;
	//время получения нужно порции информации
	ALife::_TIME_ID		receive_time;
};

DEFINE_VECTOR		(INFO_DATA, KNOWN_INFO_VECTOR, KNOWN_INFO_VECTOR_IT);