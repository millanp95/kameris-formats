#ifndef _KAMERIS_FORMATS_DIST_READER_
#define _KAMERIS_FORMATS_DIST_READER_

#include <algorithm>
#include <fstream>
#include <ios>
#include <memory>
#include <string>
#include <utility>

#include <boost/hana/transform.hpp>
#include <boost/hana/unpack.hpp>
#include <boost/variant.hpp>

#include <libkameris/utils/matrix_vector_adapters.hpp>

#include "common/binary_collection_io.hpp"
#include "common/binary_io.hpp"

#include "headers.hpp"

namespace kameris {
	class dist_reader {
	 private:
		static dist_header read_header(std::istream &file) {
			const std::unique_ptr<char[]> file_signature(read_array_binary<char>(file, dist_header::signature.size()));
			for (size_t i = 0; i < dist_header::signature.size(); ++i) {
				if (file_signature[i] != dist_header::signature[i]) {
					throw std::invalid_argument("The given file is not a valid mm-dist file");
				}
			}

			dist_header result{};
			element_underlying_t value_val = 0;

			read_binary(file, value_val);
			result.value_type = static_cast<element_type>(value_val);
			read_binary(file, result.size);

			return result;
		}

	 public:
		static auto read_matrix(const std::string &filename) {
			std::ifstream file(filename, std::ios::binary);
			dist_header header = read_header(file);

			auto dist_adapter_types = boost::hana::transform(element_type_types,
				[](auto t) { return boost::hana::type_c<SymmetricDistanceMatrixAdapter<typename decltype(t)::type>>; });
			auto result_type = boost::hana::unpack(dist_adapter_types, boost::hana::template_<boost::variant>);
			typename decltype(result_type)::type result;

			boost::hana::for_each(element_type_types, [&](auto type) {
				using Value = typename decltype(type)::type;

				if (element_type_for_type<Value> == header.value_type) {
					const size_t matrix_data_size = header.size * (header.size - 1) / 2;

					result = make_symmetric_distance_adapter<Value>(
						read_array_binary<Value>(file, matrix_data_size), header.size, true);
				}
			});
			return result;
		}
	};
}

#endif
