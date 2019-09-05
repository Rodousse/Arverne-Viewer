#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace renderer
{

struct ViewportStateInfo
{
    VkViewport viewport = {};
    VkRect2D scissor = {};
    VkPipelineViewportStateCreateInfo viewportState = {};
};

struct GraphicsPipelineCreateInfo
{
public:

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    ViewportStateInfo viewportState = {};
    VkPipelineRasterizationStateCreateInfo rasterizerState = {};
    VkPipelineMultisampleStateCreateInfo multisamplingState = {};
    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    VkPipelineLayoutCreateInfo pipelineLayout = {};

public:
    void initDefaultValue();

    static VkPipelineInputAssemblyStateCreateInfo getDefaultInputAssemblyState();
    static ViewportStateInfo getDefaultViewportState();
    static VkPipelineRasterizationStateCreateInfo getDefaultRasterizerState();
    static VkPipelineMultisampleStateCreateInfo getDefaultMultisampleState();
    static VkPipelineDepthStencilStateCreateInfo getDefaultDepthStencilState();
    static VkPipelineColorBlendAttachmentState getDefaultColorBlendAttachment();
    static VkPipelineColorBlendStateCreateInfo getDefaultColorBlendState();
    static VkPipelineLayoutCreateInfo getDefaultPipelineLayout();
};

struct Pipeline
{
    VkPipeline graphicsPipeline = nullptr;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;

    VkDescriptorSetLayout descriptorSetLayout = nullptr;
    VkDescriptorPool descriptorPool = nullptr;
    std::vector<VkDescriptorSet> descriptorSets = {};
};

}
