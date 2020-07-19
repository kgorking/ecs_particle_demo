#pragma once
#include <ecs/component_specifier.h>
#include <vector>

class input {
	ecs_flags(ecs::share);

	struct val {
		int key;
		int scancode;
		int action;
		int mods;
	};
	std::vector<val> values;

public:
	void add(int key, int scancode, int action, int mods) {
		values.push_back(val{key, scancode, action, mods});
	}

	void clear() {
		values.clear();
	}

	bool is_pressed(int key) const {
		for (auto const& v : values) {
			if (v.key == key)
				return true;
		}

		return false;
	}
};
