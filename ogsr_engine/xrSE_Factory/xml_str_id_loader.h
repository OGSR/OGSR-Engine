///////////////////////////////////////////////////////////////
// xml_str_id_loader.h
// темплейтовый класс, который вначале проходится по файлу
// и считывает все строковые ID тегов, запоминает местонахождение
// элемента с ID и присваивает ему уникальный индекс
///////////////////////////////////////////////////////////////

#pragma once

#ifdef XRGAME_EXPORTS
#	include "ui/xrUIXmlParser.h"
#else // XRGAME_EXPORTS
#	include "xrUIXmlParser.h"
#	include "object_broker.h"
#endif // XRGAME_EXPORTS


//T_ID    - уникальный текстовый идентификатор (аттрибут id в XML файле)
//T_INDEX - уникальный числовой индекс 
//T_INIT -  класс где определена статическая InitXmlIdToIndex
//          функция инициализации file_str и tag_name
#define TEMPLATE_SPECIALIZATION template<typename T_ID, typename T_INDEX,  typename T_INIT>
#define CSXML_IdToIndex CXML_IdToIndex<T_ID, T_INDEX, T_INIT>

TEMPLATE_SPECIALIZATION
class CXML_IdToIndex
{
public:
	//структура хранит строковый id элемента 
	//файл и позицию, где этот элемент находится
	struct ITEM_DATA
	{
		T_ID		id;
		T_INDEX		index;
		int			pos_in_file;
		shared_str	file_name;
		CUIXml*		_xml;
	};

private:
	typedef xr_vector<ITEM_DATA>	T_VECTOR;
	static	T_VECTOR*		m_pItemDataVector;

protected:
	//имена xml файлов (разделенных запятой) из которых 
	//производить загрузку элементов
	static LPCSTR file_str;
	//имена тегов
	static LPCSTR tag_name;
public:
	CXML_IdToIndex							();
	virtual	~CXML_IdToIndex					();

	static	void				InitInternal ();
	
	static const ITEM_DATA*		GetById		(const T_ID& str_id, bool no_assert = false);
	static const ITEM_DATA*		GetByIndex	(T_INDEX index, bool no_assert = false);

	static const T_INDEX		IdToIndex	(const T_ID& str_id, T_INDEX default_index = T_INDEX(-1), bool no_assert = false)
	{
		const ITEM_DATA* item = GetById(str_id, no_assert);
		return item?item->index:default_index;
	}
	static const T_ID			IndexToId	(T_INDEX index, T_ID default_id = NULL, bool no_assert = false)
	{
		const ITEM_DATA* item = GetByIndex(index, no_assert);
		return item?item->id:default_id;
	}

	static const T_INDEX		GetMaxIndex	()					 {return m_pItemDataVector->size()-1;}

	//удаление статичекого массива
	static void					DeleteIdToIndexData		();
};


TEMPLATE_SPECIALIZATION
typename CSXML_IdToIndex::T_VECTOR* CSXML_IdToIndex::m_pItemDataVector = NULL;

TEMPLATE_SPECIALIZATION
LPCSTR CSXML_IdToIndex::file_str = NULL;
TEMPLATE_SPECIALIZATION
LPCSTR CSXML_IdToIndex::tag_name = NULL;


TEMPLATE_SPECIALIZATION
CSXML_IdToIndex::CXML_IdToIndex()
{
}


TEMPLATE_SPECIALIZATION
CSXML_IdToIndex::~CXML_IdToIndex()
{
}


TEMPLATE_SPECIALIZATION
const typename CSXML_IdToIndex::ITEM_DATA* CSXML_IdToIndex::GetById (const T_ID& str_id, bool no_assert)
{
	T_INIT::InitXmlIdToIndex();
		
	for(T_VECTOR::iterator it = m_pItemDataVector->begin();
		m_pItemDataVector->end() != it; it++)
	{
		if( (*it).id == str_id)
			break;
	}

	if(m_pItemDataVector->end() == it)
	{
		int i=0;
		for(T_VECTOR::iterator it = m_pItemDataVector->begin();	m_pItemDataVector->end() != it; it++,i++)
			Msg("[%d]=[%s]",i,*(*it).id );

		R_ASSERT3(no_assert, "item not found, id", *str_id);
		return NULL;
	}
		
	return &(*it);
}

TEMPLATE_SPECIALIZATION
const typename CSXML_IdToIndex::ITEM_DATA* CSXML_IdToIndex::GetByIndex(T_INDEX index, bool no_assert)
{
	if((size_t)index>=m_pItemDataVector->size())
	{
		R_ASSERT3(no_assert, "item by index not found in files", file_str);
		return NULL;
	}
	return &(*m_pItemDataVector)[index];
}

TEMPLATE_SPECIALIZATION
void CSXML_IdToIndex::DeleteIdToIndexData	()
{
	T_VECTOR::iterator it		= m_pItemDataVector->begin();
	T_VECTOR::iterator it_e		= m_pItemDataVector->end();

	xr_vector<CUIXml*>			_tmp;	
	for(;it!=it_e;++it)
	{
		xr_vector<CUIXml*>::iterator it_f = std::find(_tmp.begin(), _tmp.end(), (*it)._xml);
		if(it_f==_tmp.end())
			_tmp.push_back	((*it)._xml);
	}
	delete_data	(_tmp);

	VERIFY		(m_pItemDataVector);
	xr_delete	(m_pItemDataVector);


}

TEMPLATE_SPECIALIZATION
typename void	CSXML_IdToIndex::InitInternal ()
{
	VERIFY(!m_pItemDataVector);
	T_INIT::InitXmlIdToIndex();
	{
		m_pItemDataVector = xr_new<T_VECTOR>();

		VERIFY(file_str);
		VERIFY(tag_name);

		string_path	xml_file;
		int			count = _GetItemCount	(file_str);
		T_INDEX		index = 0;
		for (int it=0; it<count; ++it)	
		{
			_GetItem	(file_str, it, xml_file);

			CUIXml* uiXml			= xr_new<CUIXml>();
			xr_string				xml_file_full;
			xml_file_full			= xml_file;
			xml_file_full			+= ".xml";
			bool xml_result			= uiXml->Init(CONFIG_PATH, GAME_PATH, xml_file_full.c_str());
			R_ASSERT3				(xml_result, "error while parsing XML file", xml_file_full.c_str());

			//общий список
			int items_num			= uiXml->GetNodesNum(uiXml->GetRoot(), tag_name);

			for(int i=0; i<items_num; ++i)
			{
				LPCSTR item_name	= uiXml->ReadAttrib(uiXml->GetRoot(), tag_name, i, "id", NULL);

				string256			buf;
				sprintf				(buf, "id for item don't set, number %d in %s", i, xml_file);
				R_ASSERT2			(item_name, buf);


				//проверетить ID на уникальность
				T_VECTOR::iterator t_it = m_pItemDataVector->begin();
				for(;m_pItemDataVector->end() != t_it; t_it++)
				{
					if(shared_str((*t_it).id) == shared_str(item_name))
						break;
				}

				R_ASSERT3(m_pItemDataVector->end() == t_it, "duplicate item id", item_name);

				ITEM_DATA			data;
				data.id				= item_name;
				data.index			= index;
				data.pos_in_file	= i;
				data.file_name		= xml_file;
				data._xml			= uiXml;
				m_pItemDataVector->push_back(data);

				index++; 
			}
		}
	}
}

#undef TEMPLATE_SPECIALIZATION

