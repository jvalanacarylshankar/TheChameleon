#pragma once

#include "common.hpp"


class Overlay : public Entity
{
public:
	int view_port[4];

	bool init(bool in_alert_mode, int max_cooldown);
	void destroy();
	void draw(const mat3& projection)override;

	void oscillation();

	void set_cooldown(int val);

	void update_alert_mode(bool val);

private:
	bool m_alert_mode;
	float m_dead_time;
	float m_oscillation_value;

	int m_cooldown;
	int m_max_cooldown;
};
