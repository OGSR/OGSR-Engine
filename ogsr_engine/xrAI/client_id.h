#ifndef client_idH
#define client_idH
#pragma once

class ClientID {
	u32 id;
public:
	ClientID():id(0){};
	u32 value()const{return id;};
	void  set(u32 v){id=v;};
	void  setBroadcast(){set(0xffffffff);}
	bool  compare(u32 v) const{return id == v;};
	bool operator ==(const ClientID& other)const{return value() == other.value();};
	bool operator !=(const ClientID& other)const{return value() != other.value();};
	bool operator < (const ClientID& other)const{return value() < other.value();};
};
#endif
