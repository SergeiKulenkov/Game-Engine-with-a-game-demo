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

struct DebugCheckbox
{
	std::string_view name = "";
	bool* activated = nullptr;
};

////////////////////

// example: Collisions method
// o basic o quad tree o grid
struct DebugRadioButtonSection
{
	std::string_view title = "";
	uint8_t* activatedIndex = nullptr;
	std::vector<std::string_view> labels;
};

////////////////////

class DebugWindow
{
public:
	DebugWindow()
	{
		m_Fields.reserve(defaultNumberOfFields);
		m_Checkboxes.reserve(defaultNumberOfCheckboxes);
		m_RadioButtons.reserve(defaultNumberOfRadioButtons);
	}

	~DebugWindow() {}

	void ChangeStatus() { m_IsDisplaying = !m_IsDisplaying; }

	void Render()
	{
		if (m_IsDisplaying &&
			((m_Fields.size() != 0) || (m_Checkboxes.size() != 0) || (m_RadioButtons.size() != 0)))
		{
			ImGui::Begin(windowName.data());
			for (const DebugCheckbox& checkbox : m_Checkboxes)
			{
				ImGui::Checkbox(checkbox.name.data(), checkbox.activated);
				ImGui::Separator();
			}

			for (DebugRadioButtonSection& section : m_RadioButtons)
			{
				bool pressed = false;
				ImGui::Text(section.title.data());
				for (uint8_t i = 0; i < section.labels.size(); i++)
				{
					pressed = ImGui::RadioButton(section.labels[i].data(), i == (*section.activatedIndex));
					if (pressed) *section.activatedIndex = i;
					if (i != section.labels.size() - 1) ImGui::SameLine();
				}
			}

			ImGui::Separator();
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

	void RegisterCheckbox(const std::string_view& name, bool* activated)
	{
		m_Checkboxes.emplace_back(DebugCheckbox(name, activated));
	}

	void RegisterRadioButton(const std::string_view& sectionTitle, uint8_t* activatedIndex, const std::vector<std::string_view>& labels)
	{
		m_RadioButtons.emplace_back(DebugRadioButtonSection(sectionTitle, activatedIndex, labels));
	}

private:
	static constexpr std::string_view windowName = "Debug Window";
	static constexpr uint8_t defaultNumberOfFields = 10;
	static constexpr uint8_t defaultNumberOfCheckboxes = 5;
	static constexpr uint8_t defaultNumberOfRadioButtons = 5;

	static constexpr float fieldWidth = 45.f;
	static constexpr float speedOfDragFloat = 0.2f;

	std::vector<DebugField> m_Fields;
	std::vector<DebugCheckbox> m_Checkboxes;
	std::vector<DebugRadioButtonSection> m_RadioButtons;

	bool m_IsDisplaying = true;
};