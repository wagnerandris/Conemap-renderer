#ifndef CONE_MAP_GENERATOR_HPP
#define CONE_MAP_GENERATOR_HPP

// STD
#include <optional>
#include <queue>
#include <thread>

// ImGui
#include "external/imgui/src/imgui.h"
#include "external/imgui-filebrowser/imfilebrowser.h"

// Cone map generation
#include "subprojects/Conemap/src/conemap.hpp"

template <typename T>
class ThreadSafeQueue {
	std::queue<T> q;
	std::mutex m;

public:
	void push(T v) {
		std::lock_guard<std::mutex> lock(m);
		q.push(std::move(v));
	}

	std::optional<T> try_pop() {
		std::lock_guard<std::mutex> lock(m);
		if (q.empty()) return std::nullopt;
		T v = std::move(q.front());
		q.pop();
		return v;
	}

	bool empty() {
		std::lock_guard<std::mutex> lock(m);
		return q.empty();
	}
};

class ConeMapGenerator {
	int generation_mode = 0;
	int height_mode = 0;

	ThreadSafeQueue<std::pair<std::filesystem::path, int>> input_queue;
	ThreadSafeQueue<std::filesystem::path> output_queue;

	std::counting_semaphore<> sem{0}; // initially 0
	std::jthread worker;
	std::atomic<bool> processing;

	std::filesystem::path output_path = std::filesystem::current_path();

	ImGui::FileBrowser directory_selector = ImGui::FileBrowser(
			ImGuiFileBrowserFlags_SelectDirectory |
			ImGuiFileBrowserFlags_HideRegularFiles |
			ImGuiFileBrowserFlags_ConfirmOnEnter |
			ImGuiFileBrowserFlags_EditPathString
			);

	ImGui::FileBrowser file_selector = ImGui::FileBrowser(
			ImGuiFileBrowserFlags_MultipleSelection |
			ImGuiFileBrowserFlags_ConfirmOnEnter |
			ImGuiFileBrowserFlags_EditPathString
			);

public:
	ConeMapGenerator() {
		directory_selector.SetTitle("Cone map generation output directory");

		file_selector.SetTypeFilters({".png", ".jpg", ".jpeg"});
		file_selector.SetTitle("Cone map generation input");

		// start worker thread
		worker = std::jthread([this](std::stop_token stoken) {
			while (!stoken.stop_requested()) {
				// wait until at least one input is queued
				sem.acquire();

				// check again, in case stop was requested while waiting
				if (stoken.stop_requested()) break;

				std::optional<std::pair<std::filesystem::path, int>> input = input_queue.try_pop();

				processing.store(true);
				
				bool analytic = input->second >> 1;
				bool depth = input->second & 1;

				std::filesystem::path output;
				if (analytic) {
					output = conemap::analytic(output_path, input->first, depth);
				} else {
					output = conemap::discrete(output_path, input->first, depth);
				}

				// Push result
				output_queue.push(output);

				processing.store(false);
			}
		});
	}

	~ConeMapGenerator() {
		worker.request_stop();
		sem.release();
	}

	std::filesystem::path compose() {
		ImGui::TextWrapped("Cone map generation output directory:\n%s", output_path.c_str());
		if (ImGui::Button("Change")) {
			directory_selector.Open();
		}
		directory_selector.Display();
		
		if (directory_selector.HasSelected()) {
			output_path = directory_selector.GetSelected();
			directory_selector.ClearSelected();
		}
		
		ImGui::Text("Generation mode:");
		ImGui::RadioButton("Discrete", &generation_mode, 0);
		ImGui::RadioButton("Analytic", &generation_mode, 1);

		ImGui::Text("Generation mode:");
		ImGui::RadioButton("Heightmap", &height_mode, 0);
		ImGui::RadioButton("Depth map", &height_mode, 1);

		// TODO wrapping texture
		if (ImGui::Button("Generate cone map from texture")) {
			file_selector.Open();
		}

		file_selector.Display();

		std::vector<std::filesystem::path> input_files;
		if (file_selector.HasSelected()) {
			input_files = file_selector.GetMultiSelected();
			file_selector.ClearSelected();

			for (std::filesystem::path input : input_files) {
				input_queue.push({input, (generation_mode << 1) + height_mode});
				// signal that an input can be processed
				sem.release();
			}
		}

		std::optional<std::filesystem::path> output = output_queue.try_pop();

		ImGui::TextColored(ImVec4(1, 1, 0, 1), // yellow
				"%s", processing.load() ? "Generating in progress" : "");

		if (output) {
			return *output;
		}
		return "";
	}
};

#endif
