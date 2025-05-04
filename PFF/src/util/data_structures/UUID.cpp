
#include "util/pffpch.h"

#include "UUID.h"

namespace PFF{

//#define USE_EXPERIMENTAL_COLLISION_AVOIDANCE

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<u64> s_UniformDistribution;

#ifdef USE_EXPERIMENTAL_COLLISION_AVOIDANCE
	static std::unordered_set<u64> s_generated_UUIDs;		// To track generated UUIDs and avoid duplicates
	FORCEINLINE static bool is_qnique(u64 uuid) { return s_generated_UUIDs.find(uuid) == s_generated_UUIDs.end(); }
#endif // USE_EXPERIMENTAL_COLLISION_AVOIDANCE

	UUID::UUID() {
#ifdef USE_EXPERIMENTAL_COLLISION_AVOIDANCE

		do {
			m_UUID = s_UniformDistribution(s_Engine);
		} while (!is_qnique(m_UUID));
		s_generated_UUIDs.insert(m_UUID);
#else
		m_UUID = s_UniformDistribution(s_Engine);
#endif // USE_EXPERIMENTAL_COLLISION_AVOIDANCE
	}

	UUID::UUID(u64 uuid)
		: m_UUID(uuid) { }

	UUID::~UUID() { 
#ifdef USE_EXPERIMENTAL_COLLISION_AVOIDANCE
		s_generated_UUIDs.erase(m_UUID);
#endif // USE_EXPERIMENTAL_COLLISION_AVOIDANCE
	}

}
