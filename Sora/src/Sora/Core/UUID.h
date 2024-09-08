#pragma once

#include <xhash>

namespace Sora {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);

		operator uint64_t() const { return mUUID; }
	private:
		uint64_t mUUID;
	};

}

namespace std {

	template<>
	struct hash<Sora::UUID>
	{
		std::size_t operator()(const Sora::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		};
	};

}