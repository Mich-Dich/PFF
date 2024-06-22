#pragma once

// Implisite cast operator
#define PFF_IMPLISITE_CASTS(type, field)		PFF_IMPLISITE_CAST(type, field); PFF_IMPLISITE_CAST_CONST(type, field);
#define PFF_IMPLISITE_CAST(type, field)			operator type& () { return field; };
#define PFF_IMPLISITE_CAST_CONST(type, field)	operator const type& () const { return field; };

#define PFF_DEFAULT_CONSTRUCTORS(name)			public: \
												name() = default;					\
												name(const name&) = default;	\
												name(name&&) = default;

/*
name(const name&) = default;
name& operator=(const name&) = default;
name(name&&) = default;
name& operator=(name&&) = default;
*/

#define PFF_DEFAULT_COPY_CONSTRUCTOR(name)		name(const name& other) = default;

#define ARRAY_SIZE(array)						(sizeof(array) / sizeof(array[0]))

#define APP_NAMESPACE PFF

#define BIT(x) (1 << x)

#define BIND_FN(x)								std::bind(&x, this, std::placeholders::_1)

#define DELETE_COPY(type)						type(const type&) = delete;								\
												type& operator=(const type&) = delete

#define DELETE_MOVE(type)						type(type&&) = delete;									\
												type& operator=(type&&) = delete

#define DELETE_COPY_MOVE(type)					type(const type&) = delete;								\
												type& operator=(const type&) = delete;					\
												type(type&&) = delete;									\
												type& operator=(type&&) = delete

#define CONSOLE_LIST_BEGIN						" " << (char)(200) << " "
