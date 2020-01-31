#pragma once
#include <utility>
#include <algorithm>
#include <type_traits>
#include <gsl/gsl>

#include "system.h"
#include "component_pool.h"

namespace ecs {
	class entity;
}

namespace ecs::detail
{
	// The implementation of a system specialized on its components
	template <class ExecutionPolicy, typename UpdatePrototype, class FirstComponent, class ...Components>
	class system_impl final : public system
	{
		// Determines if the first component is an entity
		static constexpr bool is_first_component_entity = std::is_same_v<FirstComponent, entity_id> || std::is_same_v<FirstComponent, entity>;

		// Calculate the number of components
		static constexpr size_t num_components = sizeof...(Components) + (is_first_component_entity ? 0 : 1);

		// The first type in the system, entity or component
		using first_type = std::conditional_t<is_first_component_entity, FirstComponent, FirstComponent*>;

		// Tuple holding all pools used by this system
		using tup_pools = std::conditional_t<is_first_component_entity,
			std::tuple<                                 component_pool<Components> &...>,
			std::tuple<component_pool<FirstComponent>&, component_pool<Components> &...>>;

		// Holds an entity range and a pointer to the first component from each pool in that range
		using range_arguments = std::conditional_t<is_first_component_entity,
			std::tuple<entity_range,                  Components* ...>,
			std::tuple<entity_range, FirstComponent*, Components* ...>>;

		// Holds the arguments for a range of entities
		std::vector<range_arguments> arguments;

		// A tuple of the fully typed component pools used by this system
		tup_pools const pools;

		// The user supplied system
		UpdatePrototype update_func;

	public:
		// Constructor for when the first argument to the system is _not_ an entity
		system_impl(UpdatePrototype update_func, component_pool<FirstComponent> & first_pool, component_pool<Components> &... pools)
			: pools{ first_pool, pools... }
			, update_func{ update_func }
		{
			build_args();
		}

		// Constructor for when the first argument to the system _is_ an entity
		system_impl(UpdatePrototype update_func, component_pool<Components> &... pools)
			: pools{ pools... }
			, update_func{ update_func }
		{
			build_args();
		}

		void update() noexcept override
		{
			// Call the system for all pairs of components that match the system signature
			for (auto const& argument : arguments) {
				auto const range = std::get<entity_range>(argument);
				std::for_each(ExecutionPolicy{}, range.begin(), range.end(), [this, &argument, first_id = range.first().id](auto ent) {
					// Small helper function
					auto const extract_arg = [](auto ptr, /*[[maybe_unused]]*/ ptrdiff_t offset) noexcept {
						using T = std::decay_t<decltype(*ptr)>;
						if constexpr (!is_shared_v<T>) {
							GSL_SUPPRESS(bounds.1) // this access is checked in the loop
							return ptr + offset;
						}
						else {
							(void)offset; // silence unused parameter warning
							return ptr;
						}
					};


					auto const offset = ent.id - first_id;

					if constexpr (is_first_component_entity) {
						update_func(ent,
									*extract_arg(std::get<Components*>(argument), offset)...);
					}
					else {
						update_func(*extract_arg(std::get<FirstComponent*>(argument), offset),
									*extract_arg(std::get<Components*>(argument), offset)...);
					}
				});
			}
		}

	private:
		// Handle changes when the component pools change
		void process_changes() override
		{
			auto constexpr is_pools_modified = [](auto const& ...pools) { return (pools.is_data_modified() || ...); };
			bool const is_modified = std::apply(is_pools_modified, pools);
	
			if (is_modified)
				build_args();
		}

		void build_args()
		{
			entity_range_view const entities = std::get<0>(pools).get_entities();

			if constexpr (num_components == 1)
			{
				// Build the arguments
				build_args(entities);
			}
			else
			{
				// When there are more than one component required for a system,
				// find the intersection of the sets of entities that have those components

				// Intersects two ranges of entities
				auto constexpr intersector = [](entity_range_view view_a, entity_range_view view_b) {
					std::vector<entity_range> result;

					if (view_a.empty() || view_b.empty())
						return result;

					auto it_a = view_a.begin();
					auto it_b = view_b.begin();

					while (it_a != view_a.end() && it_b != view_b.end()) {
						if (it_a->overlaps(*it_b))
							result.push_back(entity_range::intersect(*it_a, *it_b));

						if (it_a->last() < it_b->last()) // range a is inside range b, move to the next range in a
							++it_a;
						else if (it_b->last() < it_a->last()) // range b is inside range a, move to the next range in b
							++it_b;
						else { // ranges are equal, move to next ones
							++it_a;
							++it_b;
						}
					}

					return result;
				};

				auto constexpr do_intersection = [intersector](entity_range_view initial, entity_range_view first, auto ...rest) {
					std::vector<entity_range> intersect = intersector(initial, first);
					((intersect = intersector(intersect, rest)), ...);
					return intersect;
				};

				// Build the arguments
				auto const intersect = do_intersection(entities, get_pool<Components>().get_entities()...);
				build_args(intersect);
			}
		}

		// Convert a set of entities into arguments that can be passed to the system
		void build_args(entity_range_view entities)
		{
			// Build the arguments for the ranges
			arguments.clear();
			for (auto const range : entities) {
				if constexpr (is_first_component_entity)
					arguments.emplace_back(range,                                               get_component<Components>(range.first())...);
				else
					arguments.emplace_back(range, get_component<FirstComponent>(range.first()), get_component<Components>(range.first())...);
			}
		}

		template <typename Component>
		component_pool<Component>& get_pool() const
		{
			return std::get<component_pool<Component>&>(pools);
		}

		template <typename Component>
		Component* get_component(entity_id const entity)
		{
			return &get_pool<Component>().find_component_data(entity);
		}
	};
}
