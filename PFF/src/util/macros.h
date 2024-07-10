#pragma once

#define ARRAY_SIZE(array)							(sizeof(array) / sizeof(array[0]))

// ---------------------------------------------------------------------------------------------------------------------------------------
// implisite casting
// ---------------------------------------------------------------------------------------------------------------------------------------

#define PFF_IMPLISITE_CAST(type, field)				operator type& () { return field; };

#define PFF_IMPLISITE_CAST_CONST(type, field)		operator const type& () const { return field; };

#define PFF_IMPLISITE_CASTS(type, field)			PFF_IMPLISITE_CAST(type, field); PFF_IMPLISITE_CAST_CONST(type, field);

// ---------------------------------------------------------------------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------------------------------------------------------------------

#define DELETE_COPY(name)							name(const name&) = delete;								\
													name& operator=(const name&) = delete

#define DELETE_MOVE(name)							name(name&&) = delete;									\
													name& operator=(name&&) = delete

#define DELETE_COPY_MOVE(name)						name(const name&) = delete;								\
													name& operator=(const name&) = delete;					\
													name(name&&) = delete;									\
													name& operator=(name&&) = delete

#define PFF_DEFAULT_CONSTRUCTORS(name)				public: \
													name() = default;					\
													name(const name&) = default;	\
													name(name&&) = default;

#define PFF_DEFAULT_COPY_CONSTRUCTOR(name)			name(const name& other) = default;

/*
name(const name&) = default;
name& operator=(const name&) = default;
name(name&&) = default;
name& operator=(name&&) = default;
*/

// ---------------------------------------------------------------------------------------------------------------------------------------
// getters && setters
// ---------------------------------------------------------------------------------------------------------------------------------------

#define PFF_DEFAULT_GETTER(type, name)				FORCEINLINE type get_##name() { return m_##name;}

#define PFF_DEFAULT_GETTER_POINTER(type, name)		FORCEINLINE type* get_##name##_pointer() { return &m_##name;}

#define PFF_DEFAULT_GETTERS(type, name)				PFF_DEFAULT_GETTER(type, name)						\
													PFF_DEFAULT_GETTER_POINTER(type, name)


#define PFF_DEFAULT_SETTER(type, name)				FORCEINLINE void set_##name(type name) { m_##name = name;}

#define PFF_DEFAULT_GETTER_SETTER(type, name)		PFF_DEFAULT_GETTER(type, name)				\
													PFF_DEFAULT_SETTER(type, name)

#define PFF_DEFAULT_GETTER_SETTER_ALL(type, name)	PFF_DEFAULT_SETTER(type, name)				\
													PFF_DEFAULT_GETTER(type, name)				\
													PFF_DEFAULT_GETTER_POINTER(type, name)		

// ---------------------------------------------------------------------------------------------------------------------------------------
// bit manipulation
// ---------------------------------------------------------------------------------------------------------------------------------------

#define BIT(x)										(1 << x)

#define BIND_FUNKTION(x)							std::bind(&x, this, std::placeholders::_1)

// ---------------------------------------------------------------------------------------------------------------------------------------
// string
// ---------------------------------------------------------------------------------------------------------------------------------------

#define CONSOLE_LIST_BEGIN							" " << (char)(200) << " "
