#include "core/memory/memory_visualizer.hpp"
#include "utils/theme.hpp"

namespace stl_explorer {
namespace memory {

using namespace ftxui;

MemoryVisualizer::MemoryVisualizer() = default;

void MemoryVisualizer::setLayout(MemoryLayout layout) {
    layout_ = layout;
}

void MemoryVisualizer::setBlocks(const std::vector<MemoryBlock>& blocks) {
    blocks_ = blocks;
}

void MemoryVisualizer::setCapacity(size_t capacity) {
    capacity_ = capacity;
}

void MemoryVisualizer::setSize(size_t size) {
    size_ = size;
}

void MemoryVisualizer::highlightBlock(size_t index, bool highlight) {
    if (index < blocks_.size()) {
        blocks_[index].highlighted = highlight;
    }
}

void MemoryVisualizer::setIteratorPosition(size_t index) {
    for (auto& block : blocks_) {
        block.is_iterator_position = false;
    }
    if (index < blocks_.size()) {
        blocks_[index].is_iterator_position = true;
    }
}

void MemoryVisualizer::clearHighlights() {
    for (auto& block : blocks_) {
        block.highlighted = false;
        block.is_iterator_position = false;
    }
}

Element MemoryVisualizer::render() const {
    switch (layout_) {
        case MemoryLayout::Contiguous:
            return renderContiguous();
        case MemoryLayout::Linked:
            return renderLinked();
        case MemoryLayout::Tree:
            return renderTree();
        case MemoryLayout::Bucket:
            return renderBucket();
        default:
            return renderContiguous();
    }
}

Element MemoryVisualizer::renderCompact() const {
    return renderContiguous();
}

Element MemoryVisualizer::renderContiguous() const {
    auto& theme = utils::Theme::instance();

    std::vector<Element> block_elements;

    for (const auto& block : blocks_) {
        Color bg_color = block.allocated
            ? Color{0x8B, 0x8B, 0x8B}
            : Color{0x6C, 0x6E, 0x88};

        Color border_color = block.is_iterator_position
            ? Color{0xFF, 0x61, 0x95}
            : (block.highlighted ? Color{0x55, 0xB5, 0xE8} : Color{0x8B, 0x8B, 0x8B});

        std::string val = block.value;
        if (val.length() < 4) {
            val = std::string((4 - val.length()) / 2, ' ') + val + std::string((4 - val.length() + 1) / 2, ' ');
        }

        Element block_el = text(val) | borderLight;

        if (block.highlighted) {
            block_el = block_el | bold;
        }

        block_elements.push_back(block_el);
    }

    // Add capacity indicators (empty slots)
    size_t remaining = capacity_ > blocks_.size() ? capacity_ - blocks_.size() : 0;
    for (size_t i = 0; i < remaining && block_elements.size() < max_visible_blocks_; ++i) {
        block_elements.push_back(text("    ") | borderLight);
    }

    if (block_elements.empty()) {
        return text("(empty)") | dim | center;
    }

    return hbox(block_elements);
}

Element MemoryVisualizer::renderLinked() const {
    auto& theme = utils::Theme::instance();
    std::vector<Element> node_elements;

    for (size_t i = 0; i < blocks_.size(); ++i) {
        const auto& block = blocks_[i];

        Color border_color = block.highlighted
            ? Color{0x55, 0xB5, 0xE8}
            : theme.colors().muted;

        std::string val = block.value;
        if (val.length() < 4) {
            val = std::string((4 - val.length()) / 2, ' ') + val;
        }

        Element node = vbox({
            text(val) | center,
            text("next->")
        }) | borderLight;

        if (block.highlighted) {
            node = node | bold;
        }

        node_elements.push_back(node);

        if (i + 1 < blocks_.size()) {
            node_elements.push_back(text("<->") | color(Color{0x55, 0xB5, 0xE8}) | center);
        }
    }

    if (node_elements.empty()) {
        return text("(empty)") | dim | center;
    }

    return hbox(node_elements);
}

Element MemoryVisualizer::renderTree() const {
    // Simplified tree visualization for map/set
    return renderLinked();
}

Element MemoryVisualizer::renderBucket() const {
    // For unordered_map, show bucket representation
    return renderContiguous();
}

} // namespace memory
} // namespace stl_explorer
