#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <memory>

#include <imgui.h>

////////////////////

struct DebugField
{
	DebugField(const std::string& name, const bool editable)
		: name(name), editable(editable)
	{}
	virtual ~DebugField() {}

	std::string name = "";
	bool editable = false;
};

////////////////////

struct DebugFieldFloat : public DebugField
{
	DebugFieldFloat(const std::string& name, float* value, const bool editable, const float max = 10.f, const float min = 0.f, const std::string format = "%.1f")
		: DebugField(name, editable), value(value), max(max), min(min), format(format)
	{}

	float* value = nullptr;
	float min = 0.f;
	float max = 10.f;
	std::string format = "%.1f";
};

////////////////////

struct DebugFieldInteger : public DebugField
{
	DebugFieldInteger(const std::string& name, int* value, const bool editable, const int max = 10, const int min = 0)
		: DebugField(name, editable), value(value), max(max), min(min)
	{}

	int* value = nullptr;
	int min = 0;
	int max = 10;
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
				ImGui::Separator();
			}

			for (const std::unique_ptr<DebugField>& field : m_Fields)
			{
				const DebugFieldFloat* fieldFloat = dynamic_cast<const DebugFieldFloat*>(field.get());
				if (fieldFloat != nullptr)
				{
					if (!fieldFloat->editable) ImGui::Text(fieldFloat->name.c_str(), *(fieldFloat->value));
					else
					{
						ImGui::PushItemWidth(fieldWidth);
						ImGui::DragFloat(fieldFloat->name.c_str(), fieldFloat->value, speedOfDragFloat, fieldFloat->min, fieldFloat->max, fieldFloat->format.c_str());
					}
				}
				else
				{
					const DebugFieldInteger* fieldInterger = dynamic_cast<const DebugFieldInteger*>(field.get());
					if (fieldInterger != nullptr)
					{
						if (!fieldInterger->editable) ImGui::Text(fieldInterger->name.c_str(), *(fieldInterger->value));
						else
						{
							ImGui::PushItemWidth(fieldWidth);
							ImGui::DragInt(fieldInterger->name.c_str(), fieldInterger->value, speedOfDragInt, fieldInterger->min, fieldInterger->max);
						}
					}
				}
				ImGui::Separator();
			}
			ImGui::End();
		}
	}

	void RegisterEditableFieldFloat(const std::string& name, float* value, float max = 10.f, float min = 0.f, const uint8_t numberOfFractionalDigits = 1)
	{
		const std::string format = (std::string)"%." + (char)('0' + numberOfFractionalDigits) + "f";
		m_Fields.push_back(std::make_unique<DebugFieldFloat>(name, value, true, max, min, format));
	}

	void RegisterFieldFloat(const std::string& name, float* value, const uint8_t numberOfFractionalDigits = 1)
	{
		const std::string format = (std::string)"%." + (char)('0' + numberOfFractionalDigits) + "f";
		m_Fields.push_back(std::make_unique<DebugFieldFloat>(name + " = " + format, value, false));
	}

	void RegisterEditableFieldInteger(const std::string& name, int* value, int max = 10.f, int min = 0.f)
	{
		m_Fields.push_back(std::make_unique<DebugFieldInteger>(name, value, true, max, min));
	}

	void RegisterFieldInteger(const std::string& name, int* value)
	{
		m_Fields.push_back(std::make_unique<DebugFieldInteger>(name + " = %d", value, false));
	}

	void RegisterCheckbox(const std::string_view& name, bool* activated)
	{
		m_Checkboxes.push_back(DebugCheckbox(name, activated));
	}

	void RegisterRadioButton(const std::string_view& sectionTitle, uint8_t* activatedIndex, const std::vector<std::string_view>& labels)
	{
		m_RadioButtons.push_back(DebugRadioButtonSection(sectionTitle, activatedIndex, labels));
	}

private:
	static constexpr std::string_view windowName = "Debug Window";
	static constexpr uint8_t defaultNumberOfFields = 10;
	static constexpr uint8_t defaultNumberOfCheckboxes = 5;
	static constexpr uint8_t defaultNumberOfRadioButtons = 5;

	static constexpr float fieldWidth = 45.f;
	static constexpr float speedOfDragFloat = 0.2f;
	static constexpr uint8_t speedOfDragInt = 1;

	std::vector<std::unique_ptr<DebugField>> m_Fields;
	std::vector<DebugCheckbox> m_Checkboxes;
	std::vector<DebugRadioButtonSection> m_RadioButtons;

	bool m_IsDisplaying = true;
};