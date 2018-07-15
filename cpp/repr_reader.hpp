#ifndef _KAMERIS_FORMATS_REPR_READER_
#define _KAMERIS_FORMATS_REPR_READER_

#include <fstream>
#include <ios>
#include <memory>
#include <mutex>
#include <numeric>
#include <string>
#include <utility>

#include <boost/hana/concat.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/unpack.hpp>
#include <boost/variant.hpp>

#include <libkameris/utils/matrix_vector_adapters.hpp>
#include <libkameris/utils/types.hpp>

#include "common/binary_collection_io.hpp"
#include "common/binary_io.hpp"

#include "headers.hpp"

namespace kameris {
	class repr_reader {
	 private:
		std::ifstream _file;
		const repr_header _header;
		std::unique_ptr<uint64_t[]> _header_sizes;
		std::mutex _lock;

		repr_header read_header() {
			const std::unique_ptr<char[]> file_signature(read_array_binary<char>(_file, repr_header::signature.size()));
			for (size_t i = 0; i < repr_header::signature.size(); ++i) {
				if (file_signature[i] != repr_header::signature[i]) {
					throw std::invalid_argument("The given file is not a valid mm-repr file");
				}
			}

			repr_header result{};
			uint8_t is_sparse_val;
			element_underlying_t key_val = 0, value_val = 0;

			read_binary(_file, is_sparse_val);
			result.is_sparse = static_cast<bool>(is_sparse_val);
			read_binary(_file, key_val);
			result.key_type = static_cast<element_type>(key_val);
			read_binary(_file, value_val);
			result.value_type = static_cast<element_type>(value_val);
			read_binary(_file, result.count);
			read_binary(_file, result.rows);
			read_binary(_file, result.cols);

			return result;
		}

	 public:
		explicit repr_reader(const std::string &filename) : _file(filename, std::ios::binary), _header(read_header()) {
			if (_header.is_sparse) {
				_header_sizes.reset(read_array_binary<uint64_t>(_file, _header.count));
			}
		}

		const repr_header &header() const {
			return _header;
		}

		auto read_matrix(size_t index) {
			auto regular_matrix_types = boost::hana::transform(element_type_types,
				[](auto t) { return boost::hana::type_c<MatrixAdapter<typename decltype(t)::type>>; });
			auto sparse_vector_types = boost::hana::transform(element_type_types, [](auto t) {
				using SparseVectorAdapterT = decltype(make_sparse_vector_adapter(
					std::declval<std::map<uint64_t, typename decltype(t)::type>>(), size_t()));
				return boost::hana::type_c<SparseVectorAdapterT>;
			});
			auto result_type = boost::hana::unpack(
				boost::hana::concat(regular_matrix_types, sparse_vector_types), boost::hana::template_<boost::variant>);
			typename decltype(result_type)::type result;

			_lock.lock();
			boost::hana::for_each(element_type_types, [&](auto value_type) {
				using Value = typename decltype(value_type)::type;

				if (element_type_for_type<Value> == _header.value_type) {
					if (_header.is_sparse) {
						boost::hana::for_each(element_type_types, [&](auto key_type) {
							using Key = typename decltype(key_type)::type;

							if (element_type_for_type<Value> == _header.key_type) {
								_file.seekg(repr_header_size + (sizeof(uint64_t) * _header.count) +
									std::accumulate(&_header_sizes[0], &_header_sizes[index], uint64_t(0)));
								result = make_sparse_vector_adapter<uint64_t, Value>(
									read_map_binary<Key, Value>(_file, _header_sizes[index]),
									_header.rows * _header.cols);
							}
						});
					} else {
						_file.seekg(repr_header_size + (index * _header.rows * _header.cols * sizeof(Value)));
						result =
							make_matrix_adapter<Value>(read_array_binary<Value>(_file, _header.rows * _header.cols),
								_header.rows, _header.cols, true);
					}
				}
			});
			_lock.unlock();

			return result;
		}
	};
}

#endif
