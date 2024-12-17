#pragma once

namespace PFF {

	class UUID {
	public:

		PFF_DEFAULT_COPY_CONSTRUCTOR(UUID);
		UUID();
		UUID(u64 uuid);
		~UUID();

		operator u64() const { return m_UUID; }

	private:

		u64 m_UUID;
	};

}

namespace std {

	template <typename T> struct hash;

	template<>
	struct hash<PFF::UUID> {
		std::size_t operator()(const PFF::UUID& uuid) const { return (u64)uuid; }
	};

}
