#pragma once

// Quick and dirty definition
// MSVC for whatever reason flips overload ordering, and this has three of them
// So this is based on the linux bin which is correct, and MSVC will flip it to match the windows bin, fun
class IEntityResourceManifest {
public:
	virtual void AddResource(const char*) = 0;
	virtual void AddResource(const char*, void*) = 0;
	virtual void AddResource(const char*, void*, void*, void*) = 0;
	virtual void unk_04() = 0;
	virtual void unk_05() = 0;
	virtual void unk_06() = 0;
	virtual void unk_07() = 0;
	virtual void unk_08() = 0;
	virtual void unk_09() = 0;
	virtual void unk_10() = 0;
};
