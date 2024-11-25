#pragma once

#include <gametrace.h>

struct touchlist_t {
	Vector deltavelocity;
	trace_t trace;
};

class CTraceFilterPlayerMovementCS : public CTraceFilter {};
