#ifndef _KAMERIS_FORMATS_ELEMENT_TYPE_
#define _KAMERIS_FORMATS_ELEMENT_TYPE_

#include <cstdint>
#include <stdexcept>
#include <type_traits>

#include <boost/hana/at_key.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/keys.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/type.hpp>

namespace kameris {
	// clang-format off
	enum class element_type : uint8_t {
		uint8, uint16, uint32, uint64, //unsigned integral types
		float32, float64 //float types
	};
	// clang-format on
	using element_underlying_t = typename std::underlying_type<element_type>::type;

	inline void check_element_type_valid(element_type t) {
		if (t != element_type::uint8 && t != element_type::uint16 && t != element_type::uint32 &&
			/**/ t != element_type::uint64 && t != element_type::float32 && t != element_type::float64) {
			throw std::invalid_argument("Unknown value for element_type");
		}
	}

	constexpr auto type_to_element_type_map = boost::hana::make_map( //
		boost::hana::make_pair(boost::hana::type_c<uint8_t>, element_type::uint8),
		boost::hana::make_pair(boost::hana::type_c<uint16_t>, element_type::uint16),
		boost::hana::make_pair(boost::hana::type_c<uint32_t>, element_type::uint32),
		boost::hana::make_pair(boost::hana::type_c<uint64_t>, element_type::uint64),
		boost::hana::make_pair(boost::hana::type_c<float>, element_type::float32),
		boost::hana::make_pair(boost::hana::type_c<double>, element_type::float64));

	constexpr auto element_type_types = boost::hana::keys(type_to_element_type_map);

	template <typename T>
	constexpr element_type element_type_for_type = type_to_element_type_map[boost::hana::type_c<T>];
}

#endif
