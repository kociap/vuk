#pragma once

#include "vuk/Future.hpp"
#include "vuk/SPIRVTemplate.hpp"
#include "vuk/partials/CountWithIndirect.hpp"
#include "vuk/partials/StaticComputePBI.hpp"

namespace vuk {
	namespace detail {
		// from: https://stackoverflow.com/a/28213747
		template<typename T>
		struct closure_traits {};

#define REM_CTOR(...) __VA_ARGS__
#define SPEC(cv, var, is_var)                                                                                                                                  \
	template<typename C, typename R, typename... Args>                                                                                                           \
	struct closure_traits<R (C::*)(Args... REM_CTOR var) cv> {                                                                                                   \
		using arity = std::integral_constant<std::size_t, sizeof...(Args)>;                                                                                        \
		using is_variadic = std::integral_constant<bool, is_var>;                                                                                                  \
		using is_const = std::is_const<int cv>;                                                                                                                    \
                                                                                                                                                               \
		using result_type = R;                                                                                                                                     \
                                                                                                                                                               \
		template<std::size_t i>                                                                                                                                    \
		using arg = typename std::tuple_element<i, std::tuple<Args...>>::type;                                                                                     \
		static constexpr size_t count = sizeof...(Args);                                                                                                           \
		using types = std::tuple<Args...>;                                                                                                                         \
	};

		SPEC(const, (, ...), 1)
		SPEC(const, (), 0)
		SPEC(, (, ...), 1)
		SPEC(, (), 0)

#undef SPEC
#undef REM_CTOR

		template<size_t i, typename T>
		struct IndexTypePair {
			static constexpr size_t index{ i };
			using type = T;
		};

		template<int i, typename T>
		struct drop {
			using type = T;
		};

		template<typename T>
		struct drop<0, T> {
			using type = T;
		};

		template<int i, typename T, typename... Ts>
		requires(i > 0) struct drop<i, std::tuple<T, Ts...>> {
			using type = typename drop<i - 1, std::tuple<Ts...>>::type;
		};

		// from: https://stackoverflow.com/a/38746757
		template<typename... T>
		struct make_index_type_tuple_helper {
			template<typename V>
			struct idx;

			template<size_t... Indices>
			struct idx<std::index_sequence<Indices...>> {
				using tuple_type = std::tuple<IndexTypePair<Indices, T>...>;
			};

			using tuple_type = typename idx<std::make_index_sequence<sizeof...(T)>>::tuple_type;
		};

		template<typename... T>
		using make_index_type_tuple = typename make_index_type_tuple_helper<T...>::tuple_type;

		template<class T1, class T2>
		struct SPIRVBinaryMap : public spirv::SPIRVTemplate<SPIRVBinaryMap<T1, T2>> {
			static constexpr const uint32_t template_bytes[] = {
				0x07230203, 0x00010500, 0x0008000a, 0x00000171, 0x00000000, 0x00020011, 0x00000001, 0x0006000b, 0x00000001, 0x4c534c47, 0x6474732e, 0x3035342e,
				0x00000000, 0x0003000e, 0x00000000, 0x00000001, 0x000b000f, 0x00000005, 0x00000004, 0x6e69616d, 0x00000000, 0x0000000f, 0x0000002d, 0x00000034,
				0x00000042, 0x00000047, 0x0000004e, 0x00060010, 0x00000004, 0x00000011, 0x00000040, 0x00000001, 0x00000001, 0x00030003, 0x00000002, 0x000001c2,
				0x00040005, 0x00000004, 0x6e69616d, 0x00000000, 0x00070005, 0x0000000f, 0x4e5f6c67, 0x6f576d75, 0x72476b72, 0x7370756f, 0x00000000, 0x00080005,
				0x0000002d, 0x475f6c67, 0x61626f6c, 0x766e496c, 0x7461636f, 0x496e6f69, 0x00000044, 0x00050005, 0x00000032, 0x66667542, 0x6f437265, 0x00746e75,
				0x00040006, 0x00000032, 0x00000000, 0x0000006e, 0x00030005, 0x00000034, 0x00000000, 0x00050005, 0x00000040, 0x66667542, 0x754f7265, 0x00000074,
				0x00060006, 0x00000040, 0x00000000, 0x61746164, 0x74756f5f, 0x00000000, 0x00030005, 0x00000042, 0x00000000, 0x00050005, 0x00000045, 0x66667542,
				0x6e497265, 0x00000030, 0x00060006, 0x00000045, 0x00000000, 0x61746164, 0x306e695f, 0x00000000, 0x00030005, 0x00000047, 0x00000000, 0x00050005,
				0x0000004c, 0x66667542, 0x6e497265, 0x00000031, 0x00060006, 0x0000004c, 0x00000000, 0x61746164, 0x316e695f, 0x00000000, 0x00030005, 0x0000004e,
				0x00000000, 0x00040047, 0x0000000f, 0x0000000b, 0x00000018, 0x00040047, 0x0000002d, 0x0000000b, 0x0000001c, 0x00040048, 0x00000032, 0x00000000,
				0x00000018, 0x00050048, 0x00000032, 0x00000000, 0x00000023, 0x0000000c, 0x00030047, 0x00000032, 0x00000002, 0x00040047, 0x00000034, 0x00000022,
				0x00000000, 0x00040047, 0x00000034, 0x00000021, 0x00000004, 0x00040047, 0x0000003f, 0x00000006, 0x00000004, 0x00040048, 0x00000040, 0x00000000,
				0x00000017, 0x00050048, 0x00000040, 0x00000000, 0x00000023, 0x00000000, 0x00030047, 0x00000040, 0x00000002, 0x00040047, 0x00000042, 0x00000022,
				0x00000000, 0x00040047, 0x00000042, 0x00000021, 0x00000001, 0x00040047, 0x00000044, 0x00000006, 0x00000004, 0x00040048, 0x00000045, 0x00000000,
				0x00000017, 0x00050048, 0x00000045, 0x00000000, 0x00000023, 0x00000000, 0x00030047, 0x00000045, 0x00000002, 0x00040047, 0x00000047, 0x00000022,
				0x00000000, 0x00040047, 0x00000047, 0x00000021, 0x00000000, 0x00040047, 0x0000004b, 0x00000006, 0x00000004, 0x00040048, 0x0000004c, 0x00000000,
				0x00000017, 0x00050048, 0x0000004c, 0x00000000, 0x00000023, 0x00000000, 0x00030047, 0x0000004c, 0x00000002, 0x00040047, 0x0000004e, 0x00000022,
				0x00000000, 0x00040047, 0x0000004e, 0x00000021, 0x00000002, 0x00040047, 0x00000013, 0x0000000b, 0x00000019, 0x00020013, 0x00000002, 0x00030021,
				0x00000003, 0x00000002, 0x00040015, 0x00000006, 0x00000020, 0x00000000, 0x00040017, 0x00000007, 0x00000006, 0x00000003, 0x00040020, 0x0000000e,
				0x00000001, 0x00000007, 0x0004003b, 0x0000000e, 0x0000000f, 0x00000001, 0x0004002b, 0x00000006, 0x00000011, 0x00000040, 0x0004002b, 0x00000006,
				0x00000012, 0x00000001, 0x0006002c, 0x00000007, 0x00000013, 0x00000011, 0x00000012, 0x00000012, 0x0004002b, 0x00000006, 0x0000001c, 0x00000000,
				0x0004003b, 0x0000000e, 0x0000002d, 0x00000001, 0x0003001e, 0x00000032, 0x00000006, 0x00040020, 0x00000033, 0x0000000c, 0x00000032, 0x0004003b,
				0x00000033, 0x00000034, 0x0000000c, 0x00040015, 0x00000035, 0x00000020, 0x00000001, 0x0004002b, 0x00000035, 0x00000036, 0x00000000, 0x00040020,
				0x00000037, 0x0000000c, 0x00000006, 0x00020014, 0x0000003a, 0x0003001d, 0x0000003f, 0x00000006, 0x0003001e, 0x00000040, 0x0000003f, 0x00040020,
				0x00000041, 0x0000000c, 0x00000040, 0x0004003b, 0x00000041, 0x00000042, 0x0000000c, 0x0003001d, 0x00000044, 0x00000006, 0x0003001e, 0x00000045,
				0x00000044, 0x00040020, 0x00000046, 0x0000000c, 0x00000045, 0x0004003b, 0x00000046, 0x00000047, 0x0000000c, 0x0003001d, 0x0000004b, 0x00000006,
				0x0003001e, 0x0000004c, 0x0000004b, 0x00040020, 0x0000004d, 0x0000000c, 0x0000004c, 0x0004003b, 0x0000004d, 0x0000004e, 0x0000000c, 0x00050036,
				0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x000200f8, 0x00000005, 0x000300f7, 0x00000054, 0x00000000, 0x000300fb, 0x0000001c, 0x00000055,
				0x000200f8, 0x00000055, 0x0004003d, 0x00000007, 0x0000002f, 0x0000002d, 0x0004003d, 0x00000007, 0x0000005d, 0x0000000f, 0x00050084, 0x00000007,
				0x0000005e, 0x0000005d, 0x00000013, 0x00050051, 0x00000006, 0x00000060, 0x0000002f, 0x00000002, 0x00050051, 0x00000006, 0x00000062, 0x0000005e,
				0x00000001, 0x00050084, 0x00000006, 0x00000063, 0x00000060, 0x00000062, 0x00050051, 0x00000006, 0x00000065, 0x0000005e, 0x00000000, 0x00050084,
				0x00000006, 0x00000066, 0x00000063, 0x00000065, 0x00050051, 0x00000006, 0x00000068, 0x0000002f, 0x00000001, 0x00050084, 0x00000006, 0x0000006b,
				0x00000068, 0x00000065, 0x00050080, 0x00000006, 0x0000006c, 0x00000066, 0x0000006b, 0x00050051, 0x00000006, 0x0000006e, 0x0000002f, 0x00000000,
				0x00050080, 0x00000006, 0x00000070, 0x0000006c, 0x0000006e, 0x00050041, 0x00000037, 0x00000038, 0x00000034, 0x00000036, 0x0004003d, 0x00000006,
				0x00000039, 0x00000038, 0x000500ae, 0x0000003a, 0x0000003b, 0x00000070, 0x00000039, 0x000300f7, 0x0000003d, 0x00000000, 0x000400fa, 0x0000003b,
				0x0000003c, 0x0000003d, 0x000200f8, 0x0000003c, 0x000200f9, 0x00000054, 0x000200f8, 0x0000003d, 0x00060041, 0x00000037, 0x00000049, 0x00000047,
				0x00000036, 0x00000070, 0x0004003d, 0x00000006, 0x0000004a, 0x00000049, 0x00060041, 0x00000037, 0x00000050, 0x0000004e, 0x00000036, 0x00000070,
				0x0004003d, 0x00000006, 0x00000051, 0x00000050, 0x00050080, 0x00000006, 0x00000052, 0x0000004a, 0x00000051, 0x00060041, 0x00000037, 0x00000053,
				0x00000042, 0x00000036, 0x00000070, 0x0003003e, 0x00000053, 0x00000052, 0x000200f9, 0x00000054, 0x000200f8, 0x00000054, 0x000100fd, 0x00010038
			};

			static constexpr uint32_t max_id = 200;
			static constexpr std::array predef_types = { spirv::SPIRType{ spirv::type_name<spirv::Type<uint32_t>>(), 6u },
				                                           spirv::SPIRType{ spirv::type_name<spirv::Type<bool>>(), 58u },
				                                           spirv::SPIRType{ spirv::type_name<spirv::Type<spirv::ptr<spv::StorageClassStorageBuffer, uint32_t>>>(),
				                                                            55u } };

			static constexpr std::span prelude = std::span(template_bytes, 0x0000040 / 4); // we need to keep this useless line here to make clang happy
			static constexpr std::span second_bit = std::span(template_bytes + 0x0000050c / 4, 0x0000072c / 4 - 0x0000050c / 4);
			static constexpr std::span epilogue = std::span(template_bytes + 0x00000738 / 4, 6);

			template<class F>
			static constexpr auto specialize(spirv::SPIRVModule& mod, F& f) {
				using namespace spirv;

				constexpr TypeStruct<Member<TypeRuntimeArray<Type<T1>>, 0>> strT = {};
				constexpr auto ptr_to_struct = Type<ptr<spv::StorageClassStorageBuffer, decltype(strT)>>{};
				auto vA = Variable<decltype(ptr_to_struct), spv::StorageClassStorageBuffer>(mod, 0, 0);
				auto ldA = access_chain<0u>(vA, Id(112u));
				auto A = Load(ldA);
				auto vB = Variable<decltype(ptr_to_struct), spv::StorageClassStorageBuffer>(mod, 0, 2);
				auto ldB = access_chain<0u>(vB, Id(112u));
				auto B = Load(ldB);

				// process remaining arguments
				using traits = closure_traits<decltype(&F::template operator()<decltype(A), decltype(B)>)>;
				constexpr auto count = traits::count;
				if constexpr (count > 2) {
					auto third_arg = std::get<3>(traits::args);
				}

				constexpr TypeStruct<Member<TypeRuntimeArray<typename decltype(f(A, B))::type>, 0>> dstT = {};
				constexpr auto ptr_to_dst_struct = Type<ptr<spv::StorageClassStorageBuffer, decltype(dstT)>>{};
				auto vDst = Variable<decltype(ptr_to_dst_struct), spv::StorageClassStorageBuffer>(mod, 0, 1);
				auto stDst = access_chain<0u>(vDst, Id(112u));
				return spirv::Store{ stDst, f(A, B) };
			}
		};

		template<class T1, class F>
		struct SPIRVUnaryMap : public spirv::SPIRVTemplate<SPIRVUnaryMap<T1, F>> {
			static constexpr const uint32_t template_bytes[] = {
				0x07230203, 0x00010500, 0x0008000a, 0x00000171, 0x00000000, 0x00020011, 0x00000001, 0x0006000b, 0x00000001, 0x4c534c47, 0x6474732e, 0x3035342e,
				0x00000000, 0x0003000e, 0x00000000, 0x00000001, 0x000b000f, 0x00000005, 0x00000004, 0x6e69616d, 0x00000000, 0x0000000f, 0x0000002d, 0x00000034,
				0x00000042, 0x00000047, 0x0000004e, 0x00060010, 0x00000004, 0x00000011, 0x00000040, 0x00000001, 0x00000001, 0x00030003, 0x00000002, 0x000001c2,
				0x00040005, 0x00000004, 0x6e69616d, 0x00000000, 0x00070005, 0x0000000f, 0x4e5f6c67, 0x6f576d75, 0x72476b72, 0x7370756f, 0x00000000, 0x00080005,
				0x0000002d, 0x475f6c67, 0x61626f6c, 0x766e496c, 0x7461636f, 0x496e6f69, 0x00000044, 0x00050005, 0x00000032, 0x66667542, 0x6f437265, 0x00746e75,
				0x00040006, 0x00000032, 0x00000000, 0x0000006e, 0x00030005, 0x00000034, 0x00000000, 0x00050005, 0x00000040, 0x66667542, 0x754f7265, 0x00000074,
				0x00060006, 0x00000040, 0x00000000, 0x61746164, 0x74756f5f, 0x00000000, 0x00030005, 0x00000042, 0x00000000, 0x00050005, 0x00000045, 0x66667542,
				0x6e497265, 0x00000030, 0x00060006, 0x00000045, 0x00000000, 0x61746164, 0x306e695f, 0x00000000, 0x00030005, 0x00000047, 0x00000000, 0x00050005,
				0x0000004c, 0x66667542, 0x6e497265, 0x00000031, 0x00060006, 0x0000004c, 0x00000000, 0x61746164, 0x316e695f, 0x00000000, 0x00030005, 0x0000004e,
				0x00000000, 0x00040047, 0x0000000f, 0x0000000b, 0x00000018, 0x00040047, 0x0000002d, 0x0000000b, 0x0000001c, 0x00040048, 0x00000032, 0x00000000,
				0x00000018, 0x00050048, 0x00000032, 0x00000000, 0x00000023, 0x0000000c, 0x00030047, 0x00000032, 0x00000002, 0x00040047, 0x00000034, 0x00000022,
				0x00000000, 0x00040047, 0x00000034, 0x00000021, 0x00000004, 0x00040047, 0x0000003f, 0x00000006, 0x00000004, 0x00040048, 0x00000040, 0x00000000,
				0x00000017, 0x00050048, 0x00000040, 0x00000000, 0x00000023, 0x00000000, 0x00030047, 0x00000040, 0x00000002, 0x00040047, 0x00000042, 0x00000022,
				0x00000000, 0x00040047, 0x00000042, 0x00000021, 0x00000001, 0x00040047, 0x00000044, 0x00000006, 0x00000004, 0x00040048, 0x00000045, 0x00000000,
				0x00000017, 0x00050048, 0x00000045, 0x00000000, 0x00000023, 0x00000000, 0x00030047, 0x00000045, 0x00000002, 0x00040047, 0x00000047, 0x00000022,
				0x00000000, 0x00040047, 0x00000047, 0x00000021, 0x00000000, 0x00040047, 0x0000004b, 0x00000006, 0x00000004, 0x00040048, 0x0000004c, 0x00000000,
				0x00000017, 0x00050048, 0x0000004c, 0x00000000, 0x00000023, 0x00000000, 0x00030047, 0x0000004c, 0x00000002, 0x00040047, 0x0000004e, 0x00000022,
				0x00000000, 0x00040047, 0x0000004e, 0x00000021, 0x00000002, 0x00040047, 0x00000013, 0x0000000b, 0x00000019, 0x00020013, 0x00000002, 0x00030021,
				0x00000003, 0x00000002, 0x00040015, 0x00000006, 0x00000020, 0x00000000, 0x00040017, 0x00000007, 0x00000006, 0x00000003, 0x00040020, 0x0000000e,
				0x00000001, 0x00000007, 0x0004003b, 0x0000000e, 0x0000000f, 0x00000001, 0x0004002b, 0x00000006, 0x00000011, 0x00000040, 0x0004002b, 0x00000006,
				0x00000012, 0x00000001, 0x0006002c, 0x00000007, 0x00000013, 0x00000011, 0x00000012, 0x00000012, 0x0004002b, 0x00000006, 0x0000001c, 0x00000000,
				0x0004003b, 0x0000000e, 0x0000002d, 0x00000001, 0x0003001e, 0x00000032, 0x00000006, 0x00040020, 0x00000033, 0x0000000c, 0x00000032, 0x0004003b,
				0x00000033, 0x00000034, 0x0000000c, 0x00040015, 0x00000035, 0x00000020, 0x00000001, 0x0004002b, 0x00000035, 0x00000036, 0x00000000, 0x00040020,
				0x00000037, 0x0000000c, 0x00000006, 0x00020014, 0x0000003a, 0x0003001d, 0x0000003f, 0x00000006, 0x0003001e, 0x00000040, 0x0000003f, 0x00040020,
				0x00000041, 0x0000000c, 0x00000040, 0x0004003b, 0x00000041, 0x00000042, 0x0000000c, 0x0003001d, 0x00000044, 0x00000006, 0x0003001e, 0x00000045,
				0x00000044, 0x00040020, 0x00000046, 0x0000000c, 0x00000045, 0x0004003b, 0x00000046, 0x00000047, 0x0000000c, 0x0003001d, 0x0000004b, 0x00000006,
				0x0003001e, 0x0000004c, 0x0000004b, 0x00040020, 0x0000004d, 0x0000000c, 0x0000004c, 0x0004003b, 0x0000004d, 0x0000004e, 0x0000000c, 0x00050036,
				0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x000200f8, 0x00000005, 0x000300f7, 0x00000054, 0x00000000, 0x000300fb, 0x0000001c, 0x00000055,
				0x000200f8, 0x00000055, 0x0004003d, 0x00000007, 0x0000002f, 0x0000002d, 0x0004003d, 0x00000007, 0x0000005d, 0x0000000f, 0x00050084, 0x00000007,
				0x0000005e, 0x0000005d, 0x00000013, 0x00050051, 0x00000006, 0x00000060, 0x0000002f, 0x00000002, 0x00050051, 0x00000006, 0x00000062, 0x0000005e,
				0x00000001, 0x00050084, 0x00000006, 0x00000063, 0x00000060, 0x00000062, 0x00050051, 0x00000006, 0x00000065, 0x0000005e, 0x00000000, 0x00050084,
				0x00000006, 0x00000066, 0x00000063, 0x00000065, 0x00050051, 0x00000006, 0x00000068, 0x0000002f, 0x00000001, 0x00050084, 0x00000006, 0x0000006b,
				0x00000068, 0x00000065, 0x00050080, 0x00000006, 0x0000006c, 0x00000066, 0x0000006b, 0x00050051, 0x00000006, 0x0000006e, 0x0000002f, 0x00000000,
				0x00050080, 0x00000006, 0x00000070, 0x0000006c, 0x0000006e, 0x00050041, 0x00000037, 0x00000038, 0x00000034, 0x00000036, 0x0004003d, 0x00000006,
				0x00000039, 0x00000038, 0x000500ae, 0x0000003a, 0x0000003b, 0x00000070, 0x00000039, 0x000300f7, 0x0000003d, 0x00000000, 0x000400fa, 0x0000003b,
				0x0000003c, 0x0000003d, 0x000200f8, 0x0000003c, 0x000200f9, 0x00000054, 0x000200f8, 0x0000003d, 0x00060041, 0x00000037, 0x00000049, 0x00000047,
				0x00000036, 0x00000070, 0x0004003d, 0x00000006, 0x0000004a, 0x00000049, 0x00060041, 0x00000037, 0x00000050, 0x0000004e, 0x00000036, 0x00000070,
				0x0004003d, 0x00000006, 0x00000051, 0x00000050, 0x00050080, 0x00000006, 0x00000052, 0x0000004a, 0x00000051, 0x00060041, 0x00000037, 0x00000053,
				0x00000042, 0x00000036, 0x00000070, 0x0003003e, 0x00000053, 0x00000052, 0x000200f9, 0x00000054, 0x000200f8, 0x00000054, 0x000100fd, 0x00010038
			};

			static constexpr uint32_t num_inputs = 1;
			static constexpr uint32_t max_id = 200;
			static constexpr std::array predef_types = { spirv::SPIRType{ spirv::type_name<spirv::Type<uint32_t>>(), 6u },
				                                           spirv::SPIRType{ spirv::type_name<spirv::Type<bool>>(), 58u },
				                                           spirv::SPIRType{ spirv::type_name<spirv::Type<spirv::ptr<spv::StorageClassStorageBuffer, uint32_t>>>(),
				                                                            55u } };

			static constexpr std::span prelude = std::span(template_bytes, 0x0000040 / 4);
			static constexpr std::span epilogue = std::span(template_bytes + 0x00000738 / 4, 6);
			static constexpr std::span second_bit = std::span(template_bytes + 0x0000050c / 4, 0x0000072c / 4 - 0x0000050c / 4);

			template<typename>
			struct variadic_input_helper;

			template<typename... Args>
			struct variadic_input_helper<std::tuple<Args...>> {
				using type = make_index_type_tuple<Args...>;

				static constexpr auto apply(spirv::SPIRVModule& mod) {
					return std::tuple{ std::remove_reference_t<typename Args::type>(
						  spirv::Load(spirv::MemberAccessChain<0, typename std::remove_reference_t<typename Args::type>::Variable>(
						      typename std::remove_reference_t<typename Args::type>::Variable(mod, 0, 5 + Args::index))))... };
				}
			};

			static constexpr auto specialize(spirv::SPIRVModule& mod, F& f) {
				using namespace spirv;

				// make fixed inputs
				constexpr TypeStruct<Member<TypeRuntimeArray<Type<T1>>, 0>> strT = {};
				constexpr auto ptr_to_struct = Type<ptr<spv::StorageClassStorageBuffer, decltype(strT)>>{};
				auto vA = Variable<decltype(ptr_to_struct), spv::StorageClassStorageBuffer>(mod, 0, 0);
				auto ldA = access_chain<0u>(vA, Id(112u));
				auto A = Load(ldA);

				auto fixed_inputs = std::tuple{ A };

				using traits = closure_traits<decltype(&F::template operator()<decltype(A)>)>;
				// make output
				constexpr TypeStruct<Member<TypeRuntimeArray<typename traits::result_type::type>, 0>> dstT = {};
				constexpr auto ptr_to_dst_struct = Type<ptr<spv::StorageClassStorageBuffer, decltype(dstT)>>{};
				auto vDst = Variable<decltype(ptr_to_dst_struct), spv::StorageClassStorageBuffer>(mod, 0, 1);
				auto stDst = access_chain<0u>(vDst, Id(112u));
				
				// make variadic inputs
				using itt = typename variadic_input_helper<typename drop<num_inputs, typename traits::types>::type>::type;
				auto variadic_inputs = variadic_input_helper<itt>::apply(mod);

				auto inputs = std::tuple_cat(fixed_inputs, variadic_inputs);
				return spirv::Store{ stDst, std::apply(f, inputs) };
			}
		};
	} // namespace detail

	template<size_t N>
	std::array<Name, N> unique_names(Name prefix = "arg_") {
		std::array<Name, N> names = {};
		for (auto i = 0; i < N; i++) {
			names[i] = prefix.append(std::to_string(i));
		}
		return names;
	}

	template<class T, class F, class... Args>
	inline Future unary_map(Context& ctx, const F& fn, Future src, Future dst, Future count, Args&&... extra_params) {
		constexpr auto spv_result = detail::SPIRVUnaryMap<T, F>().compile(F{});
		static auto pbi = detail::static_compute_pbi(ctx, spv_result.second.data(), spv_result.first, "unary");
		std::shared_ptr<RenderGraph> rgp = std::make_shared<RenderGraph>("unary_map");
		rgp->attach_in("src", std::move(src));
		if (dst) {
			rgp->attach_in("dst", std::move(dst));
		} else {
			rgp->attach_buffer("dst", Buffer{ .memory_usage = vuk::MemoryUsage::eGPUonly });
			rgp->inference_rule("dst", same_size_as("src"));
		}
		rgp->attach_in("count", std::move(count));
		auto unq_names = unique_names<sizeof...(Args)>();
		std::array<Future, sizeof...(Args)> futs = { std::move(extra_params)... };

		std::vector<vuk::Resource> resources = { "src"_buffer >> eComputeRead, "dst"_buffer >> eComputeWrite, "count"_buffer >> (eComputeRead | eIndirectRead) };
		for (size_t i = 0; i < sizeof...(Args); i++) {
			rgp->attach_in(unq_names[i], std::move(futs[i]));
			resources.emplace_back(unq_names[i], vuk::Resource::Type::eBuffer, eComputeRW, unq_names[i].append("+"));
		}
		rgp->add_pass({ .name = "unary_map", .resources = std::move(resources), .execute = [=](CommandBuffer& command_buffer) {
			               command_buffer.bind_buffer(0, 0, "src");
			               command_buffer.bind_buffer(0, 1, "dst");
			               command_buffer.bind_buffer(0, 2, "src");
			               command_buffer.bind_buffer(0, 4, "count");
			               for (size_t i = 0; i < sizeof...(Args); i++) {
				               command_buffer.bind_buffer(0, 5 + i, unq_names[i]);
			               }
			               command_buffer.bind_compute_pipeline(pbi);
			               command_buffer.dispatch_indirect("count");
		               } });

		std::array<Future*, sizeof...(Args)> futps = { &extra_params... };
		for (size_t i = 0; i < sizeof...(Args); i++) {
			*futps[i] = Future{ rgp, unq_names[i].append("+") };
		}
		return { rgp, "dst+" };
	}

	template<class T, class F>
	inline Future binary_map(Context& ctx, Future src_a, Future src_b, Future dst, Future count, const F& fn) {
		constexpr auto spv_result = detail::SPIRVBinaryMap<T, uint32_t>().compile([](auto A, auto B) { return F{}(A, B); });
		static auto pbi = detail::static_compute_pbi(ctx, spv_result.second.data(), spv_result.first, "binary");
		std::shared_ptr<RenderGraph> rgp = std::make_shared<RenderGraph>("binary_map");
		rgp->attach_in("src_a", std::move(src_a));
		rgp->attach_in("src_b", std::move(src_b));
		if (dst) {
			rgp->attach_in("dst", std::move(dst));
		} else {
			rgp->attach_buffer("dst", Buffer{ .memory_usage = vuk::MemoryUsage::eGPUonly });
			rgp->inference_rule("dst", same_size_as("src_a"));
		}
		rgp->attach_in("count", std::move(count));
		rgp->add_pass({ .name = "unary_map",
		                .resources = { "src_a"_buffer >> eComputeRead,
		                               "src_b"_buffer >> eComputeRead,
		                               "dst"_buffer >> eComputeWrite,
		                               "count"_buffer >> (eComputeRead | eIndirectRead) },
		                .execute = [](CommandBuffer& command_buffer) {
			                command_buffer.bind_buffer(0, 0, "src_a");
			                command_buffer.bind_buffer(0, 1, "dst");
			                command_buffer.bind_buffer(0, 2, "src_b");
			                command_buffer.bind_buffer(0, 4, "count");
			                command_buffer.bind_compute_pipeline(pbi);
			                command_buffer.dispatch_indirect("count");
		                } });
		return { rgp, "dst+" };
	}
} // namespace vuk