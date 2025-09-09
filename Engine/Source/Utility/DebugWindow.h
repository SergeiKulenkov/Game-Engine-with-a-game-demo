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
	float min = 0.f;
	float max = 10.f;
	std::string format = "%.1f";
};

////////////////////

class DebugWindow
{
public:
	DebugWindow()
	{
		m_Fields.reserve(defaultNumberOfFields);
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
					ImGui::DragFloat(field.name.c_str(), field.value, speedOfDragFloat, field.min, field.max, field.format.c_str());
				}
				ImGui::Separator();
			}
			ImGui::End();
		}
	}

	void RegisterEditableField(const std::string& name, float* value, float max = 10.f, float min = 0.f, const uint8_t numberOfFractionalDigits = 1)
	{
		const std::string format = (std::string)"%." + (char)('0' + numberOfFractionalDigits) + "f";
		m_Fields.emplace_back(DebugField(name, value, true, min, max, format));
	}

	void RegisterField(const std::string& name, float* value, const uint8_t numberOfFractionalDigits = 1)
	{
		const std::string format = (std::string)"%." + (char)('0' + numberOfFractionalDigits) + "f";
		m_Fields.emplace_back(DebugField(name + " = " + format, value, false));
	}

	void ChangeStatus() { m_IsDisplaying = !m_IsDisplaying; }

private:
	static constexpr std::string_view windowName = "Debug Window";
	static constexpr uint8_t defaultNumberOfFields = 5;

	static constexpr float fieldWidth = 40.f;
	static constexpr float speedOfDragFloat = 0.2f;

	std::vector<DebugField> m_Fields;

	bool m_IsDisplaying = true;
};