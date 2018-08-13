
#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/detail/implicit_cast.hpp>
#include <luabind/detail/convert_to_lua.hpp>

namespace luabind { namespace detail 
{
	template<typename Iter>
	struct iterator_pair_state
	{
		typedef iterator_pair_state<Iter> self_t;

		static int step(lua_State* L)
		{
			self_t& state = *static_cast<self_t*>(lua_touserdata(L, lua_upvalueindex(1)));

			if (state.start == state.end)
			{
				lua_pushnil(L);
				return 1;
			}
			else
			{
				convert_to_lua(L, (*state.start).first);
				convert_to_lua(L, (*state.start).second);
				++state.start;
				return 2;
			}
		}

		explicit iterator_pair_state( Iter&& s, Iter&& e )
			: start(std::move(s))
			, end(std::move(e))
		{}

	private:
		Iter start, end;
	};

	struct iterator_pair_converter
	{
		template<typename T>
		void apply(lua_State* L, T& c)
		{
			auto it_begin = c.crbegin();
			using state_t = iterator_pair_state<decltype( it_begin )>;

			// note that this should be destructed, for now.. just hope that iterator is a pod
			void* iter = lua_newuserdata(L, sizeof(state_t));
			new (iter) state_t(std::move(it_begin), c.crend());
			lua_pushcclosure(L, state_t::step, 1);
		}
	};

	struct iterator_pair_policy : conversion_policy<0>
	{
		static void precall(lua_State*, const index_map&) {}
		static void postcall(lua_State*, const index_map&) {}

		template<typename T, Direction>
		struct generate_converter
		{
			typedef iterator_pair_converter type;
		};
	};

}}

namespace luabind
{
	namespace
	{
		detail::policy_cons<detail::iterator_pair_policy> return_stl_pair_iterator;
	}
}
