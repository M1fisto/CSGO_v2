#include "../globals.h"

std::wstring c_base_combat_weapon::get_name() {
	const auto weapon_data = get_cs_weapon_data();
	if (!weapon_data)
		return L"";

	return interfaces::m_localize->find_safe(weapon_data->m_hud_name);
}

c_cs_weapon_data* c_base_combat_weapon::get_cs_weapon_data() { return interfaces::m_weapon_system->get_cs_weapon_data(get_item_definition_index()); }

player_info_t c_cs_player::get_info() {
	auto ret = player_info_t();

	interfaces::m_engine->get_player_info(get_index(), &ret);

	return ret;
}

int c_base_animating::get_sequence_activity(int sequence) {
	const auto model = get_model();
	if (!model)
		return -1;

	const auto hdr = interfaces::m_model_info->get_studio_model(model);
	if (!hdr)
		return -1;

	static const auto get_sequence_activity_fn = SIG("client.dll", "55 8B EC 53 8B 5D 08 56 8B F1 83").cast<int(__fastcall*)(void*, studiohdr_t*, int)>();

	return get_sequence_activity_fn(this, hdr, sequence);
}

c_base_combat_weapon* c_base_combat_character::get_active_weapon() {
	const auto handle = get_active_weapon_handle();
	if (!handle.is_valid())
		return nullptr;

	return reinterpret_cast<c_base_combat_weapon*>(handle.get());
}


bool c_base_combat_weapon::is_reload() {
	static auto inReload = *(uint32_t*)(SIG("client.dll", "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

bool c_base_combat_weapon::is_gun() {
	switch (get_cs_weapon_data()->m_weapon_type)
	{
	case WEAPON_TYPE_C4:
		return false;
	case WEAPON_TYPE_GRENADE:
		return false;
	case WEAPON_TYPE_KNIFE:
		return false;
	case WEAPON_TYPE_UNKNOWN:
		return false;
	default:
		return true;
	}
}

bool c_base_combat_weapon::is_gun() {
	switch (get_cs_weapon_data()->m_weapon_type)
	{
	case WEAPON_TYPE_C4:
		return false;
	case WEAPON_TYPE_GRENADE:
		return false;
	case WEAPON_TYPE_KNIFE:
		return false;
	case WEAPON_TYPE_UNKNOWN:
		return false;
	default:
		return true;
	}
}

bool c_cs_player::is_local_player() {
	return get_index() == interfaces::m_engine->get_local_player();
}

bool c_base_entity::is_enemy() {
	if (this == globals::m_local)
		return false;

	static const auto mp_teammates_are_enemies = interfaces::m_cvar_system->find_var(FNV1A("mp_teammates_are_enemies"));
	if (mp_teammates_are_enemies->get_bool())
		return true;

	return get_team() != globals::m_local->get_team();
}

vec3_t c_cs_player::get_hitbox_position(int hitbox_id) {
	matrix3x4_t matrix[128];

	if (this->setup_bones(matrix, 128, 0x100, 0.f)) {
		studiohdr_t* hdr = interfaces::m_model_info->get_studio_model(this->get_model());

		if (hdr) {
			mstudiobbox_t* hitbox = hdr->get_hitbox_set(0)->get_hitbox(hitbox_id);

			if (hitbox) {
				vec3_t
					min = vec3_t{ },
					max = vec3_t{ };

				math::vector_transform(hitbox->m_obb_min, matrix[hitbox->m_bone], min);
				math::vector_transform(hitbox->m_obb_max, matrix[hitbox->m_bone], max);

				return (min + max) / 2.0f;
			}
		}
	}

	return vec3_t{ };
}