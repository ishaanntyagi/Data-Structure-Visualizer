#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <queue>
#include <algorithm>

#define _CRT_SECURE_NO_WARNINGS
#define IMGUI_CONFIG_FLAGS_DOCKING_ENABLE (1)
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_ENABLE_FREETYPE
#define M_PI 3.14159265358979323846

// Linked List Classes and Functions
// Node structure for the linked list
struct Node {
	int data;
	Node* next;
	Node(int val) : data(val), next(nullptr) {}
};

// Linked List class
class LinkedList {
public:
	Node* head;
	LinkedList() : head(nullptr) {}

	void insertAtBeginning(int value) {
		Node* newNode = new Node(value);
		newNode->next = head;
		head = newNode;
	}

	void insertAtEnd(int value) {
		Node* newNode = new Node(value);
		if (!head) {
			head = newNode;
			return;
		}
		Node* temp = head;
		while (temp->next) temp = temp->next;
		temp->next = newNode;
	}

	void insertAtPosition(int value, int position) {
		if (position == 0) {
			insertAtBeginning(value);
			return;
		}
		Node* temp = head;
		for (int i = 0; i < position - 1 && temp; i++) {
			temp = temp->next;
		}
		if (!temp) return; // Position out of bounds
		Node* newNode = new Node(value);
		newNode->next = temp->next;
		temp->next = newNode;
	}

	void deleteAtPosition(int position) {
		if (!head) return; // Empty list
		if (position == 0) {
			Node* toDelete = head;
			head = head->next;
			delete toDelete;
			return;
		}
		Node* temp = head;
		for (int i = 0; i < position - 1 && temp && temp->next; i++) {
			temp = temp->next;
		}
		if (!temp || !temp->next) return; // Position out of bounds
		Node* toDelete = temp->next;
		temp->next = temp->next->next;
		delete toDelete;
	}

	std::vector<int> toVector() {
		std::vector<int> result;
		Node* temp = head;
		while (temp) {
			result.push_back(temp->data);
			temp = temp->next;
		}
		return result;
	}
};

// Draw a node (filled circle)
void DrawCircle(float x, float y, float radius, ImU32 color, int segments = 32) {
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();
	drawList->AddCircleFilled(ImVec2(x, y), radius, color, segments);
}

// Draw an arrow between two points
void DrawArrow(float x1, float y1, float x2, float y2, ImU32 color, float thickness = 2.0f) {
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();
	drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);

	// Draw arrowhead
	float arrowSize = 10.0f;
	float angle = atan2(y2 - y1, x2 - x1);
	float arrowX1 = x2 - arrowSize * cos(angle - M_PI / 6);
	float arrowY1 = y2 - arrowSize * sin(angle - M_PI / 6);
	float arrowX2 = x2 - arrowSize * cos(angle + M_PI / 6);
	float arrowY2 = y2 - arrowSize * sin(angle + M_PI / 6);
	drawList->AddTriangleFilled(ImVec2(x2, y2), ImVec2(arrowX1, arrowY1), ImVec2(arrowX2, arrowY2), color);
}

void RenderLinkedList(LinkedList& list) {
	static int inputValue = 0;
	static int position = 0;
	static std::string message = "";

	// UI Window for controls (Resizable and Movable Window)
	ImGui::Begin("Linked List");

	// Input for new node value
	ImGui::InputInt("Node Value", &inputValue);
	ImGui::InputInt("Position (0 for beginning)", &position);

	// Buttons for insertion operations
	if (ImGui::Button("Insert at Beginning")) {
		list.insertAtBeginning(inputValue);
		message = "Inserted " + std::to_string(inputValue) + " at the beginning.";
	}

	ImGui::SameLine();
	if (ImGui::Button("Insert at End")) {
		list.insertAtEnd(inputValue);
		message = "Inserted " + std::to_string(inputValue) + " at the end.";
	}

	ImGui::SameLine();
	if (ImGui::Button("Insert at Position")) {
		list.insertAtPosition(inputValue, position);
		message = "Inserted " + std::to_string(inputValue) + " at position " + std::to_string(position) + ".";
	}

	// Button for deletion operation
	if (ImGui::Button("Delete at Position")) {
		list.deleteAtPosition(position);
		message = "Deleted node at position " + std::to_string(position) + ".";
	}

	// Display the message
	if (!message.empty()) {
		ImGui::Text("%s", message.c_str());
	}

	// Reserve space for the rendering area
	ImGui::Text("Visualization:");
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	ImVec2 canvasSize = ImGui::GetContentRegionAvail();
	canvasSize.y = 300; // Set a fixed height for the visualization area
	ImGui::InvisibleButton("Canvas", canvasSize);
	ImDrawList* drawList = ImGui::GetWindowDrawList(); // Draw inside the window

	// Linked list visualization
	float x = canvasPos.x + 50.0f; // Starting x position
	float y = canvasPos.y + 150.0f; // Starting y position
	float radius = 40.0f;           // Node circle radius
	float xOffset = 150.0f;         // Horizontal spacing between nodes
	ImU32 nodeColor = IM_COL32(0, 102, 255, 255); // Blue color for nodes
	ImU32 arrowColor = IM_COL32(255, 255, 255, 255);
	ImU32 textColor = IM_COL32(255, 255, 255, 255);

	Node* temp = list.head;
	while (temp) {
		// Draw the node circle
		drawList->AddCircleFilled(ImVec2(x, y), radius, nodeColor);

		// Draw the value inside the node
		std::string valueStr = std::to_string(temp->data);
		ImVec2 textSize = ImGui::CalcTextSize(valueStr.c_str());
		drawList->AddText(ImVec2(x - textSize.x / 2, y - textSize.y / 2), textColor, valueStr.c_str());

		// Draw an arrow to the next node
		if (temp->next) {
			drawList->AddLine(ImVec2(x + radius, y), ImVec2(x + xOffset - radius, y), arrowColor, 2.0f);
			drawList->AddTriangleFilled(
				ImVec2(x + xOffset - radius - 10, y - 10),
				ImVec2(x + xOffset - radius - 10, y + 10),
				ImVec2(x + xOffset - radius, y),
				arrowColor);
		}

		x += xOffset; // Move to the next node position
		temp = temp->next;

		// Wrap nodes to the next line if they exceed the window width
		if (x + radius > canvasPos.x + canvasSize.x - 50) {
			x = canvasPos.x + 50.0f;
			y += 150.0f; // Move down for the next row
		}
	}

	ImGui::End();
}



// Sorting Classes and Functions
std::vector<int> data;
int numValues = 50;
int sortSpeed = 50; // Speed control (milliseconds)
bool isSorting = false;         // Indicates if sorting is in progress
bool isPaused = false;          // Indicates if sorting is paused
std::thread sortingThread;      // Thread for running sorting algorithms

// Highlighting indices
int compareIndex1 = -1;         // First index being compared
int compareIndex2 = -1;         // Second index being compared

// Function to generate random data with a parameter
void generateRandomData(int numValues) {
	data.clear();
	for (int i = 0; i < numValues; ++i) {
		data.push_back(rand() % 100 + 1); // Random values between 1 and 100
	}
}

// Sorting visualization

void bubbleSort() {
	for (size_t i = 0; i < data.size(); ++i) {
		if (!isSorting) return;
		for (size_t j = 0; j < data.size() - i - 1; ++j) {
			compareIndex1 = j;
			compareIndex2 = j + 1;

			if (data[j] > data[j + 1]) {
				std::swap(data[j], data[j + 1]);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(sortSpeed));
		}
	}
	compareIndex1 = compareIndex2 = -1; // Reset indices
	isSorting = false;
}


void selectionSort() {
	for (size_t i = 0; i < data.size(); ++i) {
		size_t minIndex = i;
		if (!isSorting) return;
		for (size_t j = i + 1; j < data.size(); ++j) {
			compareIndex1 = minIndex; // Current minimum
			compareIndex2 = j;       // Element being compared

			if (data[j] < data[minIndex]) {
				minIndex = j; // Update the minimum index
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(sortSpeed));
		}

		if (minIndex != i) {
			std::swap(data[i], data[minIndex]);
		}
	}

	compareIndex1 = compareIndex2 = -1; // Reset indices
	isSorting = false;
}

void merge(int left, int mid, int right) {
	std::vector<int> temp(right - left + 1);
	int i = left, j = mid + 1, k = 0;

	while (i <= mid && j <= right) {
		compareIndex1 = i;
		compareIndex2 = j;

		if (data[i] <= data[j]) {
			temp[k++] = data[i++];
		}
		else {
			temp[k++] = data[j++];
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(sortSpeed));
	}

	while (i <= mid) {
		compareIndex1 = i;
		temp[k++] = data[i++];
		std::this_thread::sleep_for(std::chrono::milliseconds(sortSpeed));
	}

	while (j <= right) {
		compareIndex2 = j;
		temp[k++] = data[j++];
		std::this_thread::sleep_for(std::chrono::milliseconds(sortSpeed));
	}

	for (int t = 0; t < temp.size(); ++t) {
		data[left + t] = temp[t];
	}
}

void mergeSortHelper(int left, int right) {
	if (!isSorting) return;
	if (left >= right) return;

	int mid = left + (right - left) / 2;

	mergeSortHelper(left, mid);
	mergeSortHelper(mid + 1, right);

	merge(left, mid, right);
}

void mergeSort() {
	mergeSortHelper(0, data.size() - 1);
	compareIndex1 = compareIndex2 = -1; // Reset indices
	isSorting = false;
}

int partition(int low, int high) {
	int pivot = data[high];
	int i = low - 1;

	for (int j = low; j < high; ++j) {
		compareIndex1 = j;
		compareIndex2 = high; // Pivot

		if (data[j] < pivot) {
			i++;
			std::swap(data[i], data[j]);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(sortSpeed));
	}

	std::swap(data[i + 1], data[high]);
	return i + 1;
}

void quickSortHelper(int low, int high) {
	if (!isSorting) return;
	if (low < high) {
		int pi = partition(low, high);

		quickSortHelper(low, pi - 1);
		quickSortHelper(pi + 1, high);
	}
}

void quickSort() {
	quickSortHelper(0, data.size() - 1);
	compareIndex1 = compareIndex2 = -1; // Reset indices
	isSorting = false;
}

void heapify(int n, int i) {
	int largest = i;
	int left = 2 * i + 1;
	int right = 2 * i + 2;

	compareIndex1 = i;

	if (left < n && data[left] > data[largest]) {
		largest = left;
	}

	compareIndex2 = largest;

	if (right < n && data[right] > data[largest]) {
		largest = right;
	}

	if (largest != i) {
		std::swap(data[i], data[largest]);
		std::this_thread::sleep_for(std::chrono::milliseconds(sortSpeed));
		heapify(n, largest);
	}
}

void heapSort() {
	int n = data.size();

	for (int i = n / 2 - 1; i >= 0; --i) {
		if (!isSorting) return;
		heapify(n, i);
	}

	for (int i = n - 1; i >= 0; --i) {
		if (!isSorting) return;
		std::swap(data[0], data[i]);
		compareIndex1 = 0;
		compareIndex2 = i;
		std::this_thread::sleep_for(std::chrono::milliseconds(sortSpeed));
		heapify(i, 0);
	}

	compareIndex1 = compareIndex2 = -1; // Reset indices
	isSorting = false;
}


void insertionSort() {
	for (size_t i = 1; i < data.size(); ++i) {
		int key = data[i];
		size_t j = i - 1;

		compareIndex1 = i; // Element being inserted

		while (j < data.size() && data[j] > key) { // Comparison
			compareIndex2 = j;

			data[j + 1] = data[j]; // Shift element
			j--;

			std::this_thread::sleep_for(std::chrono::milliseconds(sortSpeed));
		}

		data[j + 1] = key; // Insert the key at the correct position

		// Reset comparison indices after each insertion
		compareIndex1 = compareIndex2 = -1;
	}

	compareIndex1 = compareIndex2 = -1; // Final reset
	isSorting = false;
}

// for rendering data bars

void renderDataBars(ImDrawList* drawList, ImVec2 canvasPos, ImVec2 canvasSize) {
	if (data.empty()) return;

	float maxValue = *std::max_element(data.begin(), data.end());
	float barWidth = canvasSize.x / data.size(); // Calculate the width of each bar
	float barHeightScale = canvasSize.y / maxValue;

	// Set a minimum bar width for visibility
	if (barWidth < 1.0f) {
		barWidth = 1.0f; // Ensure bars are at least 1 pixel wide
	}

	for (size_t i = 0; i < data.size(); ++i) {
		float x0 = canvasPos.x + i * barWidth;
		float x1 = x0 + barWidth;
		float y1 = canvasPos.y + canvasSize.y;
		float y0 = y1 - data[i] * barHeightScale;

		// Generate a color based on the value of the bar
		float normalizedValue = static_cast<float>(data[i]) / maxValue; // Range: 0 to 1
		ImU32 color = IM_COL32(
			static_cast<int>(normalizedValue * 50),          // Red component
			static_cast<int>(normalizedValue * 255),         // Green component
			static_cast<int>((1.0f - normalizedValue) * 255), // Blue component
			255                                              // Alpha
		);

		// Highlight compared bars in red
		if (i == compareIndex1 || i == compareIndex2) {
			color = IM_COL32(255, 0, 0, 255); // Red for highlighted bars
		}

		// Draw the bar
		drawList->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color);

		// Add a black border for visibility if the bar width is large enough
		if (barWidth >= 2.0f) {
			drawList->AddRect(ImVec2(x0, y0), ImVec2(x1, y1), IM_COL32(0, 0, 0, 255));
		}
	}
}

void shuffleData() {
	std::random_shuffle(data.begin(), data.end());
}
void stopSorting() {
	if (isSorting) {
		isSorting = false; // Stop the sorting process
		if (sortingThread.joinable()) {
			sortingThread.join(); // Wait for the thread to finish
		}
	}
	compareIndex1 = compareIndex2 = -1; // Reset indices
}

void RenderSorting() {
	static int count = 50;
	static char userValues[1024] = "";
	static const char* sortNames[] = { "Bubble Sort", "Selection Sort", "Insertion Sort", "Merge Sort", "Quick Sort", "Heap Sort" };
	static int selectedAlgorithm = 0; // Index of the selected algorithm

	ImGui::Begin("Sorting");

	// Input controls
	ImGui::InputInt("Values to generate", &count, 1, 100);
	if (ImGui::Button("Generate Random Data")) {
		generateRandomData(count);
	}
	ImGui::SameLine();
	if (ImGui::Button("Shuffle Data")) {
		shuffleData();
	}
	ImGui::InputText("Custom Values (space-separated)", userValues, sizeof(userValues));
	if (ImGui::Button("Set Custom Data")) {
		data.clear();
		char* nextToken = nullptr;
		char* token = strtok_s(userValues, " ", &nextToken);
		while (token != nullptr) {
			data.push_back(std::atoi(token));
			token = strtok_s(nullptr, " ", &nextToken);
		}
	}

	// Sorting algorithm selection
	ImGui::Combo("Algorithm", &selectedAlgorithm, sortNames, IM_ARRAYSIZE(sortNames));

	// Speed control
	ImGui::SliderInt("Speed (ms)", &sortSpeed, 1, 200);

	// Start sorting
	if (!isSorting && ImGui::Button("Start Sorting")) {
		isSorting = true;

		// Pass selectedAlgorithm as a value to the lambda
		int algorithm = selectedAlgorithm;
		sortingThread = std::thread([algorithm]() {
			switch (algorithm) {
			case 0: bubbleSort(); break;
			case 1: selectionSort(); break;
			case 2: insertionSort(); break;
			case 3: mergeSort(); break;
			case 4: quickSort(); break;
			case 5: heapSort(); break;
			}
			});
		sortingThread.detach();
	}

	// Stop sorting
	if (isSorting && ImGui::Button("Stop Sorting")) {
		stopSorting();
	}

	// Visualize data
	ImGui::Text("Data Visualization:");
	ImGui::Separator();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	ImVec2 canvasSize = ImGui::GetContentRegionAvail();
	canvasSize.y = 300; // Fix canvas height
	ImGui::InvisibleButton("Canvas", canvasSize);
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// Render data bars
	renderDataBars(drawList, canvasPos, canvasSize);

	ImGui::End();
}

// Stack Class and Functions

class Stack {
public:
	void Push(int value) {
		data.push_back(value);
	}

	void Pop() {
		if (!data.empty()) {
			data.pop_back();
		}
	}

	const std::vector<int>& GetData() const {
		return data;
	}

private:
	std::vector<int> data;
};

void RenderStackUI(Stack& stack) {
	static int inputValue = 0;  // For user input
	static bool isPopping = false;
	static float popAnimTime = 0.0f;
	static float popAnimDuration = 0.5f;

	// Create a new ImGui window for the queue visualizer
	ImGui::Begin("Stack");

	// Input field for custom values to push to stack
	ImGui::InputInt("Enter Value to Push", &inputValue);

	if (ImGui::Button("Push")) {
		stack.Push(inputValue);
	}

	if (ImGui::Button("Pop") && !stack.GetData().empty() && !isPopping) {
		// Start animation for pop
		isPopping = true;
		popAnimTime = 0.0f;
	}

	// Draw the stack elements
	ImGui::Text("Current Stack:");

	// Get dynamic position for the stack based on the window's layout
	ImVec2 stackPosition = ImGui::GetCursorScreenPos();  // Starting position for the stack
	stackPosition.x += 150; // Shift the stack 150 pixels to the right
	stackPosition.y += 50;  // Shift the stack 50 pixels downward
	float elementHeight = 40.0f;  // Height of each stack element
	float elementWidth = 80.0f;   // Width of each stack element
	ImU32 elementColor = IM_COL32(100, 200, 255, 255);  // Element color

	if (isPopping) {
		popAnimTime += ImGui::GetIO().DeltaTime;
		if (popAnimTime >= popAnimDuration) {
			stack.Pop();
			isPopping = false;  // Stop the animation once the element is removed
		}
	}

	// Render elements in the stack
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	for (int i = 0; i < stack.GetData().size(); ++i) {
		float yOffset = stackPosition.y + (i * elementHeight);
		if (isPopping && i == stack.GetData().size() - 1) {
			// Animate the pop element
			float alpha = 1.0f - (popAnimTime / popAnimDuration);
			ImU32 popColor = IM_COL32(255, 100, 100, (int)(255 * alpha));
			draw_list->AddRectFilled(ImVec2(stackPosition.x, yOffset), ImVec2(stackPosition.x + elementWidth, yOffset + elementHeight), popColor);
		}
		else {
			draw_list->AddRectFilled(ImVec2(stackPosition.x, yOffset), ImVec2(stackPosition.x + elementWidth, yOffset + elementHeight), elementColor);
		}

		// Display stack value as text
		ImVec2 textPos(stackPosition.x + 10, yOffset + (elementHeight / 2) - 10);
		draw_list->AddText(textPos, IM_COL32(255, 255, 255, 255), std::to_string(stack.GetData()[i]).c_str());
	}

	ImGui::Text("Number of elements: %d", stack.GetData().size());
	ImGui::End();
}

// Queue Class and Functions

class Queue {
private:
	std::queue<int> q;

public:
	void enqueue(int value) {
		q.push(value);
	}

	void dequeue() {
		if (!q.empty()) {
			q.pop();
		}
	}

	std::vector<int> toVector() {
		std::vector<int> result;
		std::queue<int> temp = q;
		while (!temp.empty()) {
			result.push_back(temp.front());
			temp.pop();
		}
		return result;
	}

	bool isEmpty() {
		return q.empty();
	}
};

// Draw a 2D rectangle with gradient (for pseudo-3D effect)
void Draw3DRectangle(ImDrawList* drawList, float x, float y, float width, float height, ImU32 color1, ImU32 color2, ImU32 shadowColor) {
	// Shadow for depth
	drawList->AddRectFilled(ImVec2(x + 5, y + 5), ImVec2(x + width + 5, y + height + 5), shadowColor);

	// Gradient for pseudo-3D look
	drawList->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + width, y + height), color1, color1, color2, color2);
	// Border
	drawList->AddRect(ImVec2(x, y), ImVec2(x + width, y + height), IM_COL32(0, 0, 0, 255));
}

// Draw text in the center of a rectangle
void DrawTextInRectangle(ImDrawList* drawList, float x, float y, float width, float height, const std::string& text, ImU32 textColor) {
	ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
	drawList->AddText(ImVec2(x + (width - textSize.x) / 2, y + (height - textSize.y) / 2), textColor, text.c_str());
}

// Function to visualize the queue graphically
void RenderQueue(Queue& queue) {
	static int inputValue = 0;
	static std::string message = "";

	// Create a new ImGui window for the queue visualizer
	ImGui::Begin("Queue");

	// Input for new element
	ImGui::InputInt("Value to Enqueue", &inputValue);

	// Enqueue button
	if (ImGui::Button("Enqueue")) {
		queue.enqueue(inputValue);
		message = "Enqueued " + std::to_string(inputValue) + ".";
	}

	ImGui::SameLine();

	// Dequeue button
	if (ImGui::Button("Dequeue")) {
		if (!queue.isEmpty()) {
			queue.dequeue();
			message = "Dequeued an element.";
		}
		else {
			message = "Queue is empty! Cannot dequeue.";
		}
	}

	// Display the message
	if (!message.empty()) {
		ImGui::Text("%s", message.c_str());
	}

	// Get the current window's drawing list and position
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImVec2 windowPos = ImGui::GetWindowPos(); // Top-left corner of the window
	ImVec2 cursorPos = ImGui::GetCursorScreenPos(); // Cursor's position inside the window

	// Set up drawing coordinates relative to the window
	float x = cursorPos.x;
	float y = cursorPos.y + 50; // Start slightly below the cursor position
	float width = 100.0f, height = 60.0f;
	float xOffset = 110.0f;

	ImU32 baseColor1 = IM_COL32(100, 200, 255, 255);  // Top gradient color
	ImU32 baseColor2 = IM_COL32(50, 150, 200, 255);   // Bottom gradient color
	ImU32 shadowColor = IM_COL32(0, 0, 0, 80);        // Shadow color
	ImU32 textColor = IM_COL32(255, 255, 255, 255);   // Text color

	// Draw the queue elements graphically
	std::vector<int> elements = queue.toVector();
	for (int i = 0; i < elements.size(); i++) {
		// Draw each node with a 3D-like effect
		Draw3DRectangle(drawList, x, y, width, height, baseColor1, baseColor2, shadowColor);

		// Draw the value inside the rectangle
		DrawTextInRectangle(drawList, x, y, width, height, std::to_string(elements[i]), textColor);

		x += xOffset; // Move to the next position
	}

	// Add "Front" and "Rear" labels
	if (!elements.empty()) {
		drawList->AddText(ImVec2(cursorPos.x, y + height + 10.0f), textColor, "Front");
		drawList->AddText(ImVec2(x - xOffset, y + height + 10.0f), textColor, "Rear");
	}

	ImGui::End();
}

// Main Application
int main() {
	// Initialize GLFW and OpenGL
	if (!glfwInit()) return -1;
	const char* glsl_version = "#version 130";
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Cenceptualization of DS", NULL, NULL);
	if (!window) return -1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsClassic();

	// For more robust UI
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 10.0f;  // Rounded corners for windows
	style.FrameRounding = 5.0f;   // Rounded corners for buttons and input fields
	style.ScrollbarSize = 12.0f;  // Bigger scrollbar
	ImGui::GetIO().FontGlobalScale = 1.5f; // Adjust the scale factor (1.5x size)


	// Set button color
	ImVec4 buttonColor = ImVec4(0.2f, 0.5f, 0.8f, 1.0f);  // Light blue color
	ImVec4 buttonHoveredColor = ImVec4(0.3f, 0.6f, 1.0f, 1.0f); // Hovered color
	ImVec4 buttonActiveColor = ImVec4(0.1f, 0.4f, 0.7f, 1.0f); // Active color

	ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);

	// State Variables
	int selectedUI = 0;  // 0: Linked List, 1: Sorting, 2: Stack, 3: Queue
	LinkedList list;
	Stack stack;
	Queue queue;
	generateRandomData(50);

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Navigation Window
		ImGui::Begin("Main Menu");
		if (ImGui::Button("Linked List")) selectedUI = 0;
		if (ImGui::Button("Sorting")) selectedUI = 1;
		if (ImGui::Button("Stack")) selectedUI = 2;
		if (ImGui::Button("Queue")) selectedUI = 3;
		ImGui::End();

		// Render Selected UI
		if (selectedUI == 0) RenderLinkedList(list);
		else if (selectedUI == 1) RenderSorting();
		else if (selectedUI == 2) RenderStackUI(stack);
		else if (selectedUI == 3) RenderQueue(queue);

		// Render Background window
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
