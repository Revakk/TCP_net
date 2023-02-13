#pragma once
#include <stdint.h>

enum class ip_resolver_messages : uint32_t
{
	VALIDATION_CHECK,
	VALIDATION_RESPONSE
};

enum class authority_messages : uint32_t
{
	REGISTRATION_REQUEST,
	LOGIN_REQUEST,
	STATUS,
	LIST_CA_CONTACTS
};
