#include "renderer/GraphicsPipeline.h"


namespace renderer
{

void GraphicsPipelineCreateInfo::initDefaultValue()
{
    inputAssemblyState = getDefaultInputAssemblyState();
    viewportState = getDefaultViewportState();

    viewportState.viewportState.viewportCount = 1;
    viewportState.viewportState.pViewports = &viewportState.viewport;
    viewportState.viewportState.scissorCount = 1;
    viewportState.viewportState.pScissors = &viewportState.scissor;

    rasterizerState = getDefaultRasterizerState();
    multisamplingState = getDefaultMultisampleState();
    depthStencilState = getDefaultDepthStencilState();

    colorBlendAttachment = getDefaultColorBlendAttachment();
    colorBlendState = getDefaultColorBlendState();
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachment;

    pipelineLayout = getDefaultPipelineLayout();

}

VkPipelineInputAssemblyStateCreateInfo GraphicsPipelineCreateInfo::getDefaultInputAssemblyState()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    return inputAssembly;
}

ViewportStateInfo GraphicsPipelineCreateInfo::getDefaultViewportState()
{
    ViewportStateInfo viewportInfo = {};
    viewportInfo.viewport.x = 0.0f;
    viewportInfo.viewport.y = 0.0f;
    viewportInfo.viewport.minDepth = 0.0f;
    viewportInfo.viewport.maxDepth = 1.0f;

    viewportInfo.scissor.offset = { 0, 0 };

    viewportInfo.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    return viewportInfo;
}

VkPipelineRasterizationStateCreateInfo GraphicsPipelineCreateInfo::getDefaultRasterizerState()
{
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    return rasterizer;
}

VkPipelineMultisampleStateCreateInfo GraphicsPipelineCreateInfo::getDefaultMultisampleState()
{
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    return multisampling;
}

VkPipelineDepthStencilStateCreateInfo GraphicsPipelineCreateInfo::getDefaultDepthStencilState()
{
    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    return depthStencil;
}

VkPipelineColorBlendAttachmentState GraphicsPipelineCreateInfo::getDefaultColorBlendAttachment()
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; //Seems to have pretty cool fast features here
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo GraphicsPipelineCreateInfo::getDefaultColorBlendState()
{
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    return colorBlending;
}

VkPipelineLayoutCreateInfo GraphicsPipelineCreateInfo::getDefaultPipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    return pipelineLayoutInfo;
}

}
