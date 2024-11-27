#pragma once
#include <vector>
#include <memory>
#include "AABB.h"
#include <glm/glm.hpp>

struct LineSegment {
    glm::vec2 start;
    glm::vec2 end;
    AABB bounds;

    LineSegment(const glm::vec2& s, const glm::vec2& e) 
        : start(s), end(e), bounds(s, e) {}
};

class BVHNode {
public:
    AABB bounds;
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;
    std::vector<LineSegment> segments;
    bool isLeaf;

    static constexpr size_t MAX_SEGMENTS_PER_LEAF = 4;

    BVHNode() : isLeaf(true) {}

    // Returns true if the given line segment intersects with any segment in the BVH
    bool intersectsLine(const glm::vec2& start, const glm::vec2& end) const {
        // Create AABB for the query line segment
        AABB queryBounds(start, end);
        
        // Early exit if no overlap with node bounds
        if (!bounds.intersects(queryBounds)) {
            return false;
        }

        // Check segments in leaf node
        if (isLeaf) {
            for (const auto& segment : segments) {
                if (auto intersection = GeometryUtils::LineSegmentIntersect(
                    start, end, 
                    segment.start, segment.end)) {
                    return true;
                }
            }
            return false;
        }

        // Recursively check children
        return (left && left->intersectsLine(start, end)) || 
               (right && right->intersectsLine(start, end));
    }

    static std::unique_ptr<BVHNode> build(std::vector<LineSegment>& segments, int depth = 0) {
        auto node = std::make_unique<BVHNode>();
        
        // Compute bounds
        for (const auto& segment : segments) {
            node->bounds = node->bounds.merge(segment.bounds);
        }

        // If few enough segments, make a leaf
        if (segments.size() <= MAX_SEGMENTS_PER_LEAF || depth > 20) {
            node->segments = std::move(segments);
            return node;
        }

        // Split along longest axis
        glm::vec2 extent = node->bounds.max - node->bounds.min;
        int axis = extent.x > extent.y ? 0 : 1;

        // Sort segments by centroid
        std::sort(segments.begin(), segments.end(),
            [axis](const LineSegment& a, const LineSegment& b) {
                glm::vec2 centroidA = (a.start + a.end) * 0.5f;
                glm::vec2 centroidB = (b.start + b.end) * 0.5f;
                return centroidA[axis] < centroidB[axis];
            });

        // Split segments into two groups
        size_t mid = segments.size() / 2;
        std::vector<LineSegment> leftSegments(segments.begin(), segments.begin() + mid);
        std::vector<LineSegment> rightSegments(segments.begin() + mid, segments.end());

        // Recursively build children
        node->isLeaf = false;
        node->left = build(leftSegments, depth + 1);
        node->right = build(rightSegments, depth + 1);

        return node;
    }
};
