#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <ftxui/dom/elements.hpp>

namespace stl_explorer {
namespace memory {

struct MemoryBlock {
    std::string value;
    void* address{nullptr};
    bool allocated{true};
    bool highlighted{false};
    bool is_iterator_position{false};
};

enum class MemoryLayout {
    Contiguous,    // vector, array
    Linked,        // list, forward_list
    Dynamic,       // deque's segmented
    Bucket,        // unordered containers
    Tree           // map, set (balanced tree)
};

class MemoryVisualizer {
public:
    MemoryVisualizer();

    void setLayout(MemoryLayout layout);
    void setBlocks(const std::vector<MemoryBlock>& blocks);
    void setCapacity(size_t capacity);
    void setSize(size_t size);
    void highlightBlock(size_t index, bool highlight = true);
    void setIteratorPosition(size_t index);
    void clearHighlights();

    ftxui::Element render() const;
    ftxui::Element renderCompact() const;

    void setMaxVisibleBlocks(size_t count) { max_visible_blocks_ = count; }

private:
    ftxui::Element renderContiguous() const;
    ftxui::Element renderLinked() const;
    ftxui::Element renderTree() const;
    ftxui::Element renderBucket() const;

    std::vector<MemoryBlock> blocks_;
    MemoryLayout layout_{MemoryLayout::Contiguous};
    size_t capacity_{0};
    size_t size_{0};
    size_t max_visible_blocks_{20};
};

} // namespace memory
} // namespace stl_explorer
