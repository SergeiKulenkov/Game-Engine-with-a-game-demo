#pragma once
#include <vector>
#include <string>
#include <string_view>

#include <imgui.h>

////////////////////

struct DebugField
{
	std::string name = "";
	float* value = nullptr;
	bool editable = false;
};

////////////////////

class DebugWindow
{
public:
	DebugWindow()
	{
		m_Fields.reserve(5);
	}

	~DebugWindow() {}

	void Render()
	{
		if (m_IsDisplaying && (m_Fields.size() != 0))
		{
			ImGui::Begin(windowName.data());
			for (const DebugField& field : m_Fields)
			{
				if (!field.editable) ImGui::Text(field.name.c_str(), *field.value);
				else
				{
					ImGui::PushItemWidth(fieldWidth);
					ImGui::DragFloat(field.name.c_str(), field.value, speedOfDragFloat, minDragFloat, maxDragFloat, "%.2f");
				}
				ImGui::Separator();
			}
			ImGui::End();
		}
	}

	void RegisterField(const std::string& name, float* value, bool editable = false, const uint8_t numberOfFractionalDigits = 1)
	{
		if (editable) m_Fields.emplace_back(DebugField(name, value, editable));
		else m_Fields.emplace_back(DebugField(name + " = %." + (char)('0' + numberOfFractionalDigits) + "f", value, editable));
	}

	void ChangeStatus() { m_IsDisplaying = !m_IsDisplaying; }

private:
	static constexpr std::string_view windowName = "Debug Window";
	static constexpr float fieldWidth = 40.f;
	static constexpr float minDragFloat = 0.f;
	static constexpr float maxDragFloat = 15.f;
	static constexpr float speedOfDragFloat = 0.2f;

	std::vector<DebugField> m_Fields;

	bool m_IsDisplaying = true;
};