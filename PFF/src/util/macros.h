#pragma once

#define ARRAY_SIZE(array)								(sizeof(array) / sizeof(array[0]))


#include "core_config.h"

#ifdef PFF_RENDER_API_VULKAN
	#define RENDERER									PFF::render::vulkan::vk_renderer
	#define GET_RENDERER								PFF::render::vulkan::vk_renderer::get()
#endif

// ---------------------------------------------------------------------------------------------------------------------------------------
// implisite casting
// ---------------------------------------------------------------------------------------------------------------------------------------

#define PFF_IMPLISITE_CAST(type, field)					operator type& () { return field; };

#define PFF_IMPLISITE_CAST_CONST(type, field)			operator const type& () const { return field; };

#define PFF_IMPLISITE_CASTS(type, field)				PFF_IMPLISITE_CAST(type, field); PFF_IMPLISITE_CAST_CONST(type, field);

// ---------------------------------------------------------------------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------------------------------------------------------------------

#define DELETE_COPY(name)								public:																		\
														name(const name&) = delete;													\
														name& operator=(const name&) = delete

#define DELETE_MOVE(name)								public:																		\
														name(name&&) = delete;														\
														name& operator=(name&&) = delete

#define DELETE_COPY_MOVE(name)							public:																		\
														name(const name&) = delete;													\
														name& operator=(const name&) = delete;										\
														name(name&&) = delete;														\
														name& operator=(name&&) = delete

#define PFF_DEFAULT_CONSTRUCTORS(name)					public:																		\
														name() = default;															\
														name(const name&) = default;												
														//name(name&&) = default;

#define PFF_DEFAULT_COPY_CONSTRUCTOR(name)				public:																		\
														name(const name& other) = default;

/*
name(const name&) = default;
name& operator=(const name&) = default;
name(name&&) = default;
name& operator=(name&&) = default;
*/

// ---------------------------------------------------------------------------------------------------------------------------------------
// getters && setters
// ---------------------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------- getters -------------------------------------------
#define PFF_DEFAULT_GETTER(type, name)					FORCEINLINE type get_##name() { return m_##name;}
#define PFF_DEFAULT_GETTER_C(type, name)				FORCEINLINE type get_##name() const { return m_##name;}
#define PFF_DEFAULT_GETTER_POINTER(type, name)			FORCEINLINE type* get_##name##_pointer() { return &m_##name;}

#define PFF_DEFAULT_GETTERS(type, name)					PFF_DEFAULT_GETTER(type, name)												\
														PFF_DEFAULT_GETTER_POINTER(type, name)

#define PFF_DEFAULT_GETTERS_C(type, name)				PFF_DEFAULT_GETTER_C(type, name)											\
														PFF_DEFAULT_GETTER_POINTER(type, name)

#define PFF_GETTER(type, func_name, var_name)			FORCEINLINE type get_##func_name() { return var_name;}
#define PFF_GETTER_C(type, func_name, var_name)			FORCEINLINE const type get_##func_name() const { return var_name;}


// ------------------------------------------- setters -------------------------------------------
#define PFF_DEFAULT_SETTER(type, name)					FORCEINLINE void set_##name(type name) { m_##name = name;}
#define PFF_SETTER(type, func_name, var_name)			FORCEINLINE void set_##func_name(type name) { var_name = name;}


// ------------------------------------------- both togetter -------------------------------------------
#define PFF_DEFAULT_GETTER_SETTER(type, name)			PFF_DEFAULT_GETTER(type, name)												\
														PFF_DEFAULT_SETTER(type, name)

#define PFF_DEFAULT_GETTER_SETTER_C(type, name)			PFF_DEFAULT_GETTER_C(type, name)											\
														PFF_DEFAULT_SETTER(type, name)

#define PFF_DEFAULT_GETTER_SETTER_ALL(type, name)		PFF_DEFAULT_SETTER(type, name)												\
														PFF_DEFAULT_GETTER(type, name)												\
														PFF_DEFAULT_GETTER_POINTER(type, name)

// ---------------------------------------------------------------------------------------------------------------------------------------
// bit manipulation
// ---------------------------------------------------------------------------------------------------------------------------------------

#define BIT(x)											(1 << x)

#define BIND_FUNKTION(x)								std::bind(&x, this, std::placeholders::_1)

// ---------------------------------------------------------------------------------------------------------------------------------------
// string
// ---------------------------------------------------------------------------------------------------------------------------------------

#define CONSOLE_LIST_BEGIN								" " << (char)(200) << " "
