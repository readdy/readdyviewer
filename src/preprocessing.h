/********************************************************************
 * Copyright © 2018 Computational Molecular Biology Group,          *
 *                  Freie Universität Berlin (GER)                  *
 *                                                                  *
 * This file is part of ReaDDy.                                     *
 *                                                                  *
 * ReaDDy is free software: you can redistribute it and/or modify   *
 * it under the terms of the GNU Lesser General Public License as   *
 * published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU Lesser General Public License for more details.              *
 *                                                                  *
 * You should have received a copy of the GNU Lesser General        *
 * Public License along with this program. If not, see              *
 * <http://www.gnu.org/licenses/>.                                  *
 ********************************************************************/


/**
 * << detailed description >>
 *
 * @file Preprocessing.h
 * @brief << brief description >>
 * @author clonker
 * @date 2/14/18
 */



#pragma once

#include <vector>
#include <algorithm>
#include <glm/gtx/norm.hpp>
#include "common.h"

namespace rv {

namespace {
template<class T>
void reorder(std::vector<T> &vA, std::vector<std::size_t> &vI) {
    std::size_t i, j, k;
    T t;
    for (i = 0; i < vA.size(); i++) {
        if (i != vI[i]) {
            t = vA[i];
            k = i;
            while (i != (j = vI[k])) {
                // every move places a value in it's final location
                vA[k] = vA[j];
                vI[k] = k;
                k = j;
            }
            vA[k] = t;
            vI[k] = k;
        }
    }
}
}

template<typename T>
std::vector<T> strideFrames(std::size_t stride, const std::vector<T> &frames) {
    std::vector<T> result;

    std::size_t count {0};
    std::copy_if(frames.begin(), frames.end(), std::back_inserter(result), [&](const auto &f) {
        return count++ % stride == 0;
    });

    return result;
}

template<typename T, bool sanity=true>
void sortById(std::vector<std::vector<T>> &frames, edges_type &edges) {
    std::size_t t = 0;
    for(auto &frame : frames) {
        std::vector<std::size_t> indices(frame.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::sort(indices.begin(), indices.end(), [&](const auto i1, const auto i2) {
            return frame.at(i1).id < frame.at(i2).id;
        });

        {
            auto idxCopy = indices;
            reorder(frame, idxCopy);
        }

        if (sanity) {
            for (auto it = frame.begin(); it != frame.end() - 1; ++it) {
                if (it->id > (it + 1)->id) {
                    throw std::runtime_error("the frames are not properly sorted by ID, internal error");
                }
            }
        }

        if (edges.size() > t) {
            for (auto &e : edges.at(t)) {
                std::get<0>(e) = indices.at(std::get<0>(e));
                std::get<1>(e) = indices.at(std::get<1>(e));
            }
        }

        ++t;
    }
}

struct TrajectoryEntries {
    std::size_t maxNParticles;
    std::vector<std::size_t> nParticlesPerFrame;
    std::vector<glm::vec4> posTypes;
    edges_type edges;
    std::size_t maxType {0};
};

template<typename T>
TrajectoryEntries convertTrajectory(std::vector<std::vector<T>> frames, edges_type edges, std::size_t stride,
                                    std::size_t smoothing, float smoothingCutoff) {
    if (stride > 1) {
        frames = strideFrames(stride, frames);
        edges = strideFrames(stride, edges);
    }

    TrajectoryEntries result;

    std::transform(frames.begin(), frames.end(), std::back_inserter(result.nParticlesPerFrame), [](const auto &frame) {
        return frame.size();
    });

    result.maxNParticles = 0;
    {
        auto it = std::max_element(result.nParticlesPerFrame.begin(), result.nParticlesPerFrame.end());
        if(it != result.nParticlesPerFrame.end()) result.maxNParticles = *it;
    }

    log::debug("got max number of particles in a single frame: {}", result.maxNParticles);

    result.posTypes.resize(result.maxNParticles * frames.size());

    float scale {1.f};
    glm::vec3 posTranslation {0.f, 0.f, 0.f};

    if (smoothing > 1) {
        sortById(frames, edges);
        for (std::size_t i = 0; i < frames.size(); ++i) {
            std::size_t offset = i * result.maxNParticles;
            const auto &frame = frames.at(i);

            std::size_t k = 0;
            for (auto itParticle = frame.begin(); itParticle != frame.end(); ++itParticle, ++k) {

                auto it_pt = result.posTypes.begin() + offset + k;

                auto referenceFrame = glm::vec4(0, 0, 0, 0);

                bool reset = false;
                std::size_t n = 0;
                for (std::size_t j = i >= smoothing ? i - smoothing : 0;
                     j < std::min(i + smoothing, frames.size()); ++j) {
                    const auto &otherFrame = frames.at(j);

                    auto findIt = std::lower_bound(otherFrame.begin(), otherFrame.end(), itParticle->id,
                                                   [](const auto &entry, const auto id) {
                                                       return entry.id < id;
                                                   });
                    if (findIt != otherFrame.end() && findIt->id == itParticle->id) {
                        if (smoothingCutoff > 0) {
                            if(glm::l2Norm(findIt->pos - itParticle->pos) > smoothingCutoff) {
                                reset = true;
                                break;
                            }
                            referenceFrame += glm::vec4(scale * (findIt->pos + posTranslation), 0);
                            ++n;
                        } else {
                            referenceFrame += glm::vec4(scale * (findIt->pos + posTranslation), 0);
                            ++n;
                        }
                    }
                }
                if(reset) {
                    referenceFrame = glm::vec4(itParticle->pos, 0);
                } else {
                    referenceFrame /= static_cast<double>(n);
                }
                referenceFrame.w = frame.at(k).type;
                *it_pt = referenceFrame;
                result.maxType = std::max(result.maxType, static_cast<std::size_t>(frame.at(k).type));
            }
        }
    } else {
        for (std::size_t i = 0; i < frames.size(); ++i) {
            const auto &frame = frames.at(i);
            std::size_t j = 0;
            std::size_t offset = i * result.maxNParticles;
            auto it_pt = result.posTypes.begin() + offset;
            auto itParticles = frame.begin();
            for (; it_pt != result.posTypes.begin() + offset + frame.size(); ++it_pt, ++j, ++itParticles) {
                // project into [0, 10]**3
                if (j < frame.size()) {
                    *it_pt = glm::vec4(scale * (itParticles->pos + posTranslation), itParticles->type);
                    result.maxType = std::max(result.maxType, static_cast<std::size_t>(itParticles->type));
                }
            }
        }
    }

    result.edges = std::move(edges);
    return result;
}

}
