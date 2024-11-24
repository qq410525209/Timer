#pragma once

#include "common.h"
#include <sdk/protobuf/cs_usercmd.pb.h>

class CSGOUserCmdPB;

class CInButtonState {
	void** vtable;

public:
	uint64_t pressed;
	uint64_t changed;
	uint64_t scroll;
};

class CUserCmdBase {
public:
	int cmdNum;
	uint8_t unk[4];

	virtual ~CUserCmdBase();

private:
	virtual void unk0();
	virtual void unk1();
	virtual void unk2();
	virtual void unk3();
	virtual void unk4();
	virtual void unk5();
	virtual void unk6();
};

template<typename T>
class CUserCmdBaseHost : public CUserCmdBase, public T {};

class CUserCmd : public CUserCmdBaseHost<CSGOUserCmdPB> {};

class PlayerCommand : public CUserCmd {
public:
	CInButtonState m_Buttonstates;

	// Not part of the player message
	uint32_t unknown[4];
	PlayerCommand* unknowncmd;
	PlayerCommand* unknowncmd2;
};
